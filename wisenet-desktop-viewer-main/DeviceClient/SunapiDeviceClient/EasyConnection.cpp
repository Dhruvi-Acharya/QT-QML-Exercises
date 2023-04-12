/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include "EasyConnection.h"

#include <QTcpServer>
#include <QCoreApplication>
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

#include "SunapiDeviceClientLogSettings.h"
#include "SunapiDeviceClientUtil.h"

#include "ExceptionCode.h"
#include "sipproxy.h"

namespace Wisenet
{
namespace Device
{

AvailableTcpPort::AvailableTcpPort()
{
}

bool AvailableTcpPort::getPort(unsigned short& port)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    QTcpServer server;

    if (!server.listen()) {
        server.close();
        return false;
    }

    port = server.serverPort();
    server.close();

    SPDLOG_INFO("Available Tcp Port is {}", port);
    return true;
}

EasyConnection::EasyConnection(const std::string& deviceUUID)
    :m_p2pUID("")
    ,m_echandle(nullptr)
    ,m_devicePortProxyHandle(nullptr)
    ,m_httpsPortProxyHandle(nullptr)
    ,m_rtspPortProxyHandle(nullptr)
{
    m_logPrefix = "[EasyConnection](" + deviceUUID + ")";
}

bool EasyConnection::isValidProductP2PID(const std::string & productP2PID)
{
    if(productP2PID.empty()){
        SPDLOG_DEBUG("productP2PID is empty.");
        return false;
    }

    if(std::string::npos == productP2PID.find("HBIRD_")){
        SPDLOG_DEBUG("productID is invalid. productID = {}", productP2PID);
        return false;
    }

    return true;
}

bool EasyConnection::GetIpAndPort(Wisenet::Device::DeviceConnectInfo &connInfo, const Wisenet::Library::CloudP2PInfo &p2pInfo)
{
    SLOG_INFO("GetIpAndPortFromP2PServer - start");

    //std::string sampleUid = "htwdtest2101000012";
    //std::map<std::string, std::string> config = {
    //    {"MQTT_HOST","13.125.150.31:5000"},
    //    {"MQTT_CLIENT_ID", "htwu-8982633b-72b0-4764-86ab-ae2831868dfb-w-6hsn6dkf"},
    //    {"MQTT_AUTH_ID", "htwu-8982633b-72b0-4764-86ab-ae2831868dfb"},
    //    {"MQTT_AUTH_PW", "eyJhbGciOiJSUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VySWQiOiJodHd1LTg5ODI2MzNiLTcyYjAtNDc2NC04NmFiLWFlMjgzMTg2OGRmYiIsImxvY2F0aW9uIjoidXMtd2VzdC0xIiwidXNlclN0YXR1cyI6ImFjdGl2ZSIsInNjb3BlIjoiZGV2aWNlLmh0d2R0ZXN0MjEwMTAwMDAxMi5vd25lciIsImlhdCI6MTYxNjk4MjYwNywiZXhwIjoxNjE2OTg5ODA2LCJpc3MiOiJodHcuaGJpcmQtaW90LmNvbSIsImp0aSI6ImJhOGI3ZTYxLWVkYjItNGFiOC1iYjVlLWY1YTM2YTZiOThkNyJ9.l2K5Tmy8K1R7znG95jd1pqPqTjDjpdkGyCiuIQY-6AgrTv-_f6G9sM8gSxSw36g4LX8ZXwjU2pPxuLNKf0jYCaZ4X1PsV7R6jQufZ7nMapfu1ZdaC1uVDKQmTYfIdA0IF1q7SuHSnblU-JuUJ6SIiZJyN0nhzroDODhRnYccFQz5KUozxXscEYzDdjJAdAqKOhkPNZ4AH9Ctfw50TfiJIHKVu1HOdDVZO06vcpp4Yz172mn-U9yUKU-ox-2IXCMYJBt4KQkndBnGFINFE32mtTjc_G3YnvMtZih__WKIkA-7HOg5HOLNELfeuGBuDJHVfO0hNQV8PSigqNNYQzgVSQ"},
    //    {"TURN_URL", "stun:stun.l.google.com:19302"}
    //};
    std::map<std::string, std::string> config;
    config.emplace("MQTT_HOST", p2pInfo.iotHubUrl);
    config.emplace("MQTT_CLIENT_ID", p2pInfo.iotHubId);
    config.emplace("MQTT_AUTH_ID", p2pInfo.iotHubAuthName);
    config.emplace("MQTT_AUTH_PW", p2pInfo.iotHubAuthPassword);

    if (!p2pInfo.turnUrl.empty()) {
        config.emplace("TURN_URL", p2pInfo.turnUrl);
    }

    if (!p2pInfo.turnId.empty()) {
        config.emplace("TURN_ID", p2pInfo.turnId);
    }

    if (!p2pInfo.turnCredential.empty()) {
        config.emplace("TURN_CREDENTIAL", p2pInfo.turnCredential);
    }

    if (!p2pInfo.stunUrl.empty()) {
        config.emplace("STUN_URL", p2pInfo.stunUrl);
    }
    SPDLOG_DEBUG("p2pInfo.supportDtls = {} - usageDtlsMode = {}", p2pInfo.supportDtls, connInfo.usageDtlsMode);

    if (p2pInfo.supportDtls) {
        if(connInfo.usageDtlsMode){
            config.emplace("DTLS", "on");
        }
        else {
            config.emplace("DTLS", "off");
        }
    }

    config.emplace("USE_SSL", "1");

    std::string trustStoreFilePath = QCoreApplication::applicationDirPath().toLocal8Bit().toStdString();
    trustStoreFilePath += "/RootCA/ca-certificates.crt";
    config.emplace("TRUST_STORE_FILE", trustStoreFilePath);
 
    SPDLOG_TRACE("----- P2P CONNECTION INFO START------");
    for(auto& itor : config){
        SPDLOG_TRACE("----- KEY={},VALUE={}", itor.first, itor.second);
    }
    SPDLOG_TRACE("----- deviceId={}", p2pInfo.deviceId);
    SPDLOG_TRACE("----- P2P CONNECTION INFO END------");

    std::string subDeviceID = ""; //EMUL 테스트에서만 사용.

    StopP2PService();

    m_p2pUID = p2pInfo.deviceId; //실제.

    SLOG_DEBUG("SIPPROXY_Open - Start");
    try {
        m_echandle = SIPPROXY_Open(m_p2pUID, config);
        if (nullptr == m_echandle) {
            SLOG_INFO("Failed to open SIPPROXY_Open. UID={}", connInfo.host);
            return false;
        }
    }
    catch (const legosip::Exception_t& e) {

        if (e == legosip::Exception_t::EXC_SESSION_EXCEED_MAX_CLIENT) {
            SLOG_INFO("Failed to open SIPPROXY_Open. UID={}, Exception=EXC_SESSION_EXCEED_MAX_CLIENT", connInfo.host);
            return false;
        }

        SLOG_INFO("Failed to open SIPPROXY_Open. UID={}, Exception={}", connInfo.host, e);
        return false;
    }
   

    SLOG_DEBUG("SIPPROXY_Open - Success");

    std::string deviceInfo;
    SLOG_DEBUG("SIPPROXY_GetDeviceInfo - start");
    if(!SIPPROXY_GetDeviceInfo(m_p2pUID, subDeviceID, deviceInfo)){
        SLOG_INFO("Failed to get SIPPROXY_GetDeviceInfo. UID={}", connInfo.host);
        return false;
    }

    SLOG_DEBUG("SIPPROXY_GetDeviceInfo - Success");

    unsigned short devicePort = 0, httpsPort = 0, rtspPort = 0;
    if(!parseDeviceInfoJson(deviceInfo, devicePort, httpsPort, rtspPort)){
        SLOG_INFO("Failed to parse deviceinfo on SIPPROXY. UID={}", connInfo.host);
        return false;
    }

    if (0 == devicePort && 0 == httpsPort) {
        SLOG_ERROR("Unknown DevicePort And HttpsPort . UID={}", connInfo.host);
        return false;
    }
    if (0 == rtspPort) {
        SLOG_ERROR("Unknown RtspPort . UID={}", connInfo.host);
        return false;
    }
    //rtspPort = 558; // 삭제되어야 함.

    SLOG_DEBUG("parseDeviceInfoJson - Success DevicePort:{}, HttpsPort:{}, RtspPort:{}", devicePort, httpsPort, rtspPort);

    SIPPROXY_CONNECTION_TYPE type = GetConnectedType();  

    if (SIPPROXY_CONNECTION_TYPE::eP2P == type) {
        connInfo.connectedType = ConnectedType::P2P;
        SLOG_DEBUG("SIPPROXY_GetConnectionType - ConnectedType : P2P");
    }
    else if (SIPPROXY_CONNECTION_TYPE::eRelay == type) {
        connInfo.connectedType = ConnectedType::P2P_RELAY;
        SLOG_DEBUG("SIPPROXY_GetConnectionType - ConnectedType : RELAY");
    }
    else {
        SLOG_DEBUG("SIPPROXY_GetConnectionType - ConnectedType : Unkown");
        connInfo.connectedType = ConnectedType::P2P;
    }

    unsigned short deviceProxyPort = 0;
    {
        if (0 != devicePort)
        {
            // device port
            if (!AvailableTcpPort::getInstance().getPort(deviceProxyPort)) {

                SLOG_ERROR("Failed to get Available TCP Port.");
                return false;
            }

            SLOG_DEBUG("getAvailableTcpPort for DevicePort - Available Port:{}", deviceProxyPort);

            SLOG_DEBUG("SIPPROXY_StartServer devicePort - start");
            m_devicePortProxyHandle = SIPPROXY_StartServer(m_echandle, subDeviceID, deviceProxyPort, devicePort);
            if (m_devicePortProxyHandle == nullptr)
            {
                SLOG_ERROR("SIPPROXY_StartServerfor Device Port failed");
                return false;
            }

            SLOG_DEBUG("SIPPROXY_StartServer devicePort - SUCCESS");
        }
    }
    unsigned short httpsProxyPort = 0;
    {
        //https port
        if(0 != httpsPort)
        {
            if(!AvailableTcpPort::getInstance().getPort(httpsProxyPort)){

                SLOG_ERROR("Failed to get Available TCP Port.");
                return false;
            }

            SLOG_DEBUG("getAvailableTcpPort for HttpsPort - Available Port:{}", httpsProxyPort);
            SLOG_DEBUG("SIPPROXY_StartServer httpsPort - start");
            m_httpsPortProxyHandle = SIPPROXY_StartServer(m_echandle, subDeviceID, httpsProxyPort, httpsPort);
            if (m_httpsPortProxyHandle == nullptr)
            {
                SLOG_ERROR("SIPPROXY_StartServerfor Https Port failed");
                return false;
            }
            SLOG_DEBUG("SIPPROXY_StartServer httpsPort - SUCCESS");
        }
    }

    unsigned short rtspProxyPort = 0;
    {
        //rtsp port

        if(!AvailableTcpPort::getInstance().getPort(rtspProxyPort)){

            SLOG_ERROR("Failed to get Available TCP Port.");
            return false;
        }

        SLOG_DEBUG("getAvailableTcpPort for RTSP Port - Available Port:{}", rtspProxyPort);
        SLOG_DEBUG("SIPPROXY_StartServer rtspPort - start");
        m_rtspPortProxyHandle = SIPPROXY_StartServer(m_echandle, subDeviceID, rtspProxyPort, rtspPort);
        if (m_rtspPortProxyHandle == nullptr)
        {
            SLOG_ERROR("SIPPROXY_StartServer for RTSP Port failed");
            return false;
        }
        SLOG_DEBUG("SIPPROXY_StartServer rtspPort - SUCCESS");
    }

    connInfo.ip = "127.0.0.1";
    connInfo.port = deviceProxyPort;
    if(httpsPort != 0){
        connInfo.sslPort = httpsProxyPort;
    }
    connInfo.rtspPort = rtspProxyPort;
    if (0 == deviceProxyPort) {
        connInfo.isSSL = true;
    }
    SLOG_INFO("GetIpAndPortFromP2PServer Result: IP:{}, Device PORT:{}, RTSP PORT: {}, isSSL: {}",connInfo.ip, connInfo.port, connInfo.rtspPort, connInfo.isSSL);
    return true;
}

SIPPROXY_CONNECTION_TYPE EasyConnection::GetConnectedType()
{
    unsigned int count = 0;
    SIPPROXY_CONNECTION_TYPE type = SIPPROXY_CONNECTION_TYPE::eUnknown;

    if (nullptr == m_echandle) {
        return type;
    }

    while (count < 5) {

        SLOG_DEBUG("SIPPROXY_GetConnectionType - Start : count={}", count);
        type = SIPPROXY_GetConnectionType(m_echandle);
        SLOG_DEBUG("SIPPROXY_GetConnectionType - Success : type={}", type);

        if (SIPPROXY_CONNECTION_TYPE::eUnknown != type) {
            break;
        }
        count++;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return type;
}

bool EasyConnection::StopP2PService()
{
    if(m_devicePortProxyHandle){
        try {
            SLOG_DEBUG("SIPPROXY_StopServer devicePort - start");
            if (!SIPPROXY_StopServer(m_devicePortProxyHandle)) {
                SLOG_ERROR("Failed to stop SIPPROXY_Stop for devicePort");
            }
            SLOG_DEBUG("SIPPROXY_StopServer devicePort - SUCCESS");
        }
        catch (const std::exception& e) {
            SLOG_ERROR("SIPPROXY_StopServer devicePort Exception: {}", e.what());
        }
        m_devicePortProxyHandle = nullptr;
    }

    if(m_rtspPortProxyHandle){
        try{
            SLOG_DEBUG("SIPPROXY_StopServer rtspPort - start");
            if (!SIPPROXY_StopServer(m_rtspPortProxyHandle)) {
                SLOG_ERROR("Failed to stop SIPPROXY_Stop for rtspPort");
            }
            SLOG_DEBUG("SIPPROXY_StopServer rtspPort - SUCCESS");
        }
        catch (const std::exception& e) {
            SLOG_ERROR("SIPPROXY_StopServer rtspPort Exception: {}", e.what());
        }
        
        m_rtspPortProxyHandle = nullptr;
    }

    if(m_httpsPortProxyHandle){
        try {
            SLOG_DEBUG("SIPPROXY_StopServer httpsPort - start");
            if (!SIPPROXY_StopServer(m_httpsPortProxyHandle)) {
                SLOG_ERROR("Failed to stop SIPPROXY_Stop for httpsPort");
            }
            SLOG_DEBUG("SIPPROXY_StopServer httpsPort - SUCCESS");
        }
        catch (const std::exception& e) {
            SLOG_ERROR("SIPPROXY_StopServer httpsPort Exception: {}", e.what());
        }
        
        m_httpsPortProxyHandle = nullptr;
    }

    if(m_echandle){
        try {
            SLOG_DEBUG("SIPPROXY_Close - start");
            if (!SIPPROXY_Close(m_echandle)) {
                SLOG_ERROR("Failed to close SIPPROXY_Close.");
            }
            SLOG_DEBUG("SIPPROXY_Close - SUCCESS");
        }
        catch (const std::exception& e) {
            SLOG_ERROR("SIPPROXY_Close Exception: {}", e.what());
        }
        
        m_echandle = nullptr;
    }

    return true;
}

bool EasyConnection::parseDeviceInfoJson(const std::string &deviceInfo, unsigned short& devicePort, unsigned short& httpsPort, unsigned short& rtspPort)
{
    rapidjson::Document document;

    size_t pos = deviceInfo.find_first_of("{");
    if(pos == std::string::npos){
        SLOG_DEBUG("Invalid Json format in SIPPROXY_deviceInfo message.");
        return false;
    }

    auto jsonString = deviceInfo.substr(pos);
    SLOG_DEBUG("Json of SIPPROXY_GetDeviceInfo: {}", jsonString);

    if (document.Parse(jsonString.c_str()).HasParseError()) {
        SLOG_ERROR("Failed to parse Json of SIPPROXY_GetDeviceInfo. ErrorOffset={}, ErrorMessage={}", document.GetErrorOffset(), GetParseError_En(document.GetParseError()));
        return false;
    }

    std::string devicePortString("");
    std::string rtspPortString("");
    std::string httpsPortString("");

    if (document.HasMember("http")) {
        devicePortString = document["http"].GetString();
    }

    if(document.HasMember("https")){
       httpsPortString = document["https"].GetString();
    }

    if (document.HasMember("rtsp")) {
        rtspPortString = document["rtsp"].GetString();
    }

    try{

        if (!devicePortString.empty()) {
            devicePort = (unsigned short)std::stoi(devicePortString);
        }
        if (!rtspPortString.empty()) {
            rtspPort = (unsigned short)std::stoi(rtspPortString);
        }
        if(!httpsPortString.empty()){
            httpsPort = (unsigned short)std::stoi(httpsPortString);
        }

    }catch(std::exception& e){
        SLOG_ERROR("Invalid value in http({}) or rtsp({} or https({}), Exception={}", devicePortString,rtspPortString, httpsPortString, e.what());
        return false;
    }

    return true;
}

}
}

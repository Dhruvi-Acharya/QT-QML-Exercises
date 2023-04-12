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
#include "CloudService.h"

#include <stdlib.h>
#include <time.h>
#include <set>

#include <QUuid>

#include <thread>
#include <boost/algorithm/string/predicate.hpp>

#include "ThreadPool.h"
#include "WeakCallback.h"
#include "CloudHttps.h"

namespace Wisenet
{
namespace Library
{

CloudService::CloudService()
    :m_contextPool(std::make_shared<ThreadPool>(16))
    ,m_strand(m_contextPool->ioContext().get_executor())
    ,m_changeUserHandler(nullptr)
    ,m_useSTUN(true)
    ,m_useTURN(true)
    ,m_useNewCloudAPI(true)
{
}

void CloudService::Start(const std::map<std::string, std::string> &configs, const CloudUser &cloudUser, const ChangeUserHandler &changeUserHandler)
{
    for(auto& config : configs){
        if(0 == config.first.compare("iothuburl")){
            m_iotHubUrl = config.second;
        }else if(0 == config.first.compare("apiurl")){
            m_connectionInfo.api = config.second;
        }else if(0 == config.first.compare("authurl")){
            m_connectionInfo.auth = config.second;
        }else if(0 == config.first.compare("turnurl")){
            m_connectionInfo.turn = config.second;
        }else if(0 == config.first.compare("port")){
            m_connectionInfo.port = config.second;
        }else if (0 == config.first.compare("useSTUN")) {
            if (boost::iequals("true", config.second)) {
                m_useSTUN = true;
            }
            else {
                m_useSTUN = false;
            }
        }else if (0 == config.first.compare("useTURN")) {
            if (boost::iequals("true", config.second)) {
                m_useTURN = true;
            }
            else {
                m_useTURN = false;
            }
        }
        else if (0 == config.first.compare("useNewCloudAPI")) {
            if (boost::iequals("true", config.second)) {
                m_useNewCloudAPI = true;
            }
            else {
                m_useNewCloudAPI = false;
            }
        }
    }

    m_cloudUser = cloudUser;

    m_changeUserHandler = changeUserHandler;

    m_contextPool->Start();
}

void CloudService::Stop()
{
    m_contextPool->Stop();
}

void CloudService::AsyncGetP2PInfo(CloudDevice device, const GetP2PInfoHandler &getP2PInfoHandler)
{
    boost::asio::post(m_strand,
        WeakCallback(shared_from_this(),
            [this, device, getP2PInfoHandler]()
            {
                
        if (!m_cloudUser.userName.empty() && !m_cloudUser.password.empty()) {
            // Already exists a userName and password.
            SPDLOG_INFO("Already exists a userName and password.");
            // 정상적으로 접속 가능한지 확인한다.
            CloudUserInfo userInfo;
            userInfo.userName = m_cloudUser.userName;
            userInfo.password = m_cloudUser.password;

            CloudHttps http(m_contextPool->ioContext(), m_connectionInfo);
            int status = http.GetToken(userInfo, m_accessToken, m_iotHubAuthName, m_iotHubAuthPassword);

            if (404 == status) {
                if (!createUser(m_cloudUser)) {
                    CloudP2PInfo p2pInfo;
                    getP2PInfoHandler(CloudErrorCode::AddingUserError, p2pInfo);
                    return;
                }

                SPDLOG_INFO("Created a user. case1");
                m_changeUserHandler(m_cloudUser);
            }
            else if (200 != status && 201 != status) {
                CloudP2PInfo p2pInfo;
                getP2PInfoHandler(CloudErrorCode::NetworkError, p2pInfo);
                return;
            }           

        }else {
            if(!createUser(m_cloudUser)){
                CloudP2PInfo p2pInfo;
                getP2PInfoHandler(CloudErrorCode::AddingUserError,p2pInfo);
                return;
            }

            SPDLOG_INFO("Created a user. case2");
            m_changeUserHandler(m_cloudUser);
        }


        asyncGetP2PInfo(m_cloudUser, device, getP2PInfoHandler);

    }));
}

void CloudService::AsyncDeleteDevice(const std::vector<std::string>& productP2PIDs)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
        [this, productP2PIDs]()
        {
            if (m_cloudUser.userName.empty()) {
                SPDLOG_ERROR("Failed to delete devices in Wisenet Cloud : Wisenet Cloud UserName not exists.");
                return;
            }

            if (productP2PIDs.empty()) {
                SPDLOG_DEBUG("No exists productP2PID");
                return;
            }

            //1.rId리스트 만들기
            std::vector<std::string> rIds;
            for (auto& p2pId : productP2PIDs) {

                rIds.emplace_back(toRID(p2pId));
            }

            if (rIds.empty()) {
                SPDLOG_DEBUG("No exists rID");
                return;
            }

            CloudHttps http(m_contextPool->ioContext(), m_connectionInfo);
            //1. Token 획득(access_token, access_token_iothub)
            CloudUserInfo userInfo;
            userInfo.userName = m_cloudUser.userName;
            userInfo.password = m_cloudUser.password;

            std::string accessToken;
            std::string iotHubAuthName;
            std::string iotHubAuthPassword;
            if (!http.GetToken(userInfo, accessToken, iotHubAuthName, iotHubAuthPassword)) {
                SPDLOG_ERROR("Failed to delete devices in Wisenet Cloud : Failed to get a token in Wisenet Cloud.");
                return;
            }

            //2. GetDevices로 획득 (rId리스트에 포함되어 있거나, latestIssuedDeviceId 가 false인 목록으로 삭제할 deviceId 목록 만듬.
            CloudDevicesInfo devices;
            if (!http.GetDevices(accessToken, "", devices)) {
                SPDLOG_ERROR("Failed to delete devices in Wisenet Cloud : Failed to get devices in Wisenet Cloud.");
                return;
            }

            CloudDeleteDevicesInfo deleteDevices;

            for (auto device : devices.data.devices) {

                if (!device.latestIssuedDeviceId) {

                    deleteDevices.deviceIds.emplace_back(device.deviceId);
                    SPDLOG_INFO("Delete Device (latestIssuedDeviceId is false.) rId={}, deviceID={}", device.rId, device.deviceId);
                    continue;
                }

                for (auto& rId : rIds) {
                    if (boost::iequals(rId, device.rId)) {
                        deleteDevices.deviceIds.emplace_back(device.deviceId);
                        SPDLOG_INFO("Delete Device rId={}, deviceID={}", device.rId, device.deviceId);
                        break;
                    }
                }
            }

            if (deleteDevices.deviceIds.empty()) {
                SPDLOG_ERROR("Failed to delete devices in Wisenet Cloud : No exists deviceIds in Wisenet Cloud.");
                return;
            }

            //3. DeleteDevices로 삭제.
            if (!http.DeleteDevices(accessToken, deleteDevices)) {
                SPDLOG_ERROR("Failed to delete devices in Wisenet Cloud : Failed to delete devices in Wisenet Cloud.");
                return;
            }
        }));
}

bool CloudService::createUser(CloudUser& cloudUser)
{
    CloudHttps http(m_contextPool->ioContext(),m_connectionInfo);
    // Need to create Cloud User. (try to create max 10.)
    bool created = false; int count = 0;
    CloudUserInfo createUser;
    do{
        count++;
        SPDLOG_INFO("Try to create a user. count={}", count);
        createKey(createUser.userName,createUser.password);
        created = http.CreateUser(createUser);
        if(!created){
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }

    }while(!created && (count <= 10));

    if(created){
        cloudUser.userName = createUser.userName;
        cloudUser.password = createUser.password;

        if(cloudUser.cloudUserID.empty()){
            // Create cloudUserID
            cloudUser.cloudUserID = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
        }
    }

    return created;
}

void CloudService::asyncGetP2PInfo(CloudUser cloudUser, CloudDevice device, const GetP2PInfoHandler &getP2PInfoHandler)
{
    boost::asio::post(m_contextPool->ioContext(),
               WeakCallback(shared_from_this(),
                            [this, cloudUser, device, getP2PInfoHandler]()
    {
        //0. productID에서 특수문자 제거하여 rId를 만듬.
        auto rId = toRID(device.productP2PID);

        CloudP2PInfo p2pInfo, emptyP2PInfo;

        p2pInfo.iotHubId = m_iotHubId;
        p2pInfo.iotHubUrl = m_iotHubUrl;
        p2pInfo.iotHubAuthPassword = m_iotHubAuthPassword;

        //1. Token 획득(access_token, access_token_iothub)
        CloudHttps http(m_contextPool->ioContext(),m_connectionInfo);
        CloudUserInfo userInfo;
        CloudDevicesInfo devices;

        std::string accessToken = m_accessToken;
        std::string iotHubAuthName = m_iotHubAuthName;

        userInfo.userName = cloudUser.userName;
        userInfo.password = cloudUser.password;

        // 2022.11.28. 토큰 획득을 실패 했을 경우에만 다시 토큰을 요청하도록 한다.
        if (m_accessToken.empty() || m_iotHubAuthName.empty() || m_iotHubAuthPassword.empty())
        {
            userInfo.userName = cloudUser.userName;
            userInfo.password = cloudUser.password;
            if(!http.GetToken(userInfo, accessToken, iotHubAuthName, p2pInfo.iotHubAuthPassword)){
                getP2PInfoHandler(CloudErrorCode::NetworkError,emptyP2PInfo);
                return;
            }
        }

        //2. rId로 deviceId 리스트를 획득한다.
        if(!http.GetDevices(accessToken, rId, devices)){
            getP2PInfoHandler(CloudErrorCode::NetworkError,emptyP2PInfo);
            return;
        }

        //3. deviceId리스트 중에서 마지막 발행된 DeviceId와 DTLS ON/OFF 기능 지원 여부 값을 얻는다.
        bool supportDtls = false;
        getLatestIssuedDeviceIdAndSupportDtls(rId, devices, p2pInfo.deviceId, supportDtls);

        p2pInfo.supportDtls = supportDtls;
        SPDLOG_DEBUG("CloudService::asyncGetP2PInfo p2pInfo.supportDtls={}", p2pInfo.supportDtls);


        //4. 등록된 deviceId가 없으면 장비 추가 시도.
        if(p2pInfo.deviceId.empty())
        {
            CloudDeviceRegisterInfo deviceRegisterInfo;
            deviceRegisterInfo.rid = rId;
            deviceRegisterInfo.deviceUser = device.userName;
            deviceRegisterInfo.devicePassword = device.password;

            std::this_thread::sleep_for(std::chrono::seconds(2)); // 잠시 대기

            int status = http.RegisterDevice(accessToken, deviceRegisterInfo, p2pInfo.deviceId);

            if(200 != status && 201 != status){
                if(401002 == status){
                    getP2PInfoHandler(CloudErrorCode::UnAuthorizedDevice, emptyP2PInfo);
                }else{
                    getP2PInfoHandler(CloudErrorCode::AddingDeviceError, emptyP2PInfo);
                }
                return;
            }

            //장비가 추가되어 accessTokenIothub 가 변경되었으니 Token을 다시 받아야 함.
            if(!http.GetToken(userInfo, accessToken, iotHubAuthName, p2pInfo.iotHubAuthPassword)){
                getP2PInfoHandler(CloudErrorCode::NetworkError, emptyP2PInfo);
                return;
            }

            CloudDevicesInfo devices;
            if(!http.GetDevices(accessToken, rId, devices)){
                getP2PInfoHandler(CloudErrorCode::NetworkError,emptyP2PInfo);
                return;
            }

            //3. deviceId리스트 중에서 마지막 발행된 DeviceId와 DTLS ON/OFF 기능 지원 여부 값을 얻는다.
            bool supportDtls = false;
            getLatestIssuedDeviceIdAndSupportDtls(rId, devices, p2pInfo.deviceId, supportDtls);

            p2pInfo.supportDtls = supportDtls;
        }

        p2pInfo.iotHubAuthName = iotHubAuthName;
        p2pInfo.iotHubId = createIotHubId(iotHubAuthName);
        SPDLOG_TRACE("New Created IotHubID. iotHubAuthName={}, iotHubId={}", p2pInfo.iotHubAuthName, p2pInfo.iotHubId);
        
        

        //4. turn url, id, credeitial, stun url
        if (!m_useNewCloudAPI) {
            CloudTurnInfo turnInfo;
            if (!http.GetTurnConfig(accessToken, turnInfo)) {
                getP2PInfoHandler(CloudErrorCode::InvalidValue, emptyP2PInfo);
                return;
            }

            //5. turn 정보를 p2pInfo에 채워 넣는다.
            if (!pushTurnInfo(p2pInfo, turnInfo)) {
                getP2PInfoHandler(CloudErrorCode::InvalidValue, emptyP2PInfo);
                return;
            }

        }else {
            CloudTurnInfo2 turnInfo;
            if (!http.GetTurnConfig2(accessToken, turnInfo)) {
                getP2PInfoHandler(CloudErrorCode::InvalidValue, emptyP2PInfo);
                return;
            }

            p2pInfo.stunUrl = turnInfo.data.stunAddress;
            p2pInfo.turnUrl = turnInfo.data.turnAddress;
            p2pInfo.turnId = turnInfo.data.turnUserName;
            p2pInfo.turnCredential = turnInfo.data.turnCredential;
        }
        

        // config 처리.
        if (!m_useSTUN) {
            p2pInfo.stunUrl = "";
            SPDLOG_DEBUG("Reset STUN URL.");
        }

        if (!m_useTURN) {
            p2pInfo.turnUrl = "";
            p2pInfo.turnId = "";
            p2pInfo.turnCredential = "";
            SPDLOG_DEBUG("Reset TURN URL.");
        }

        getP2PInfoHandler(CloudErrorCode::NoError, p2pInfo);

    }));
}

std::string CloudService::toRID(const std::string & productP2PID)
{
    //HBIRD_ 제외
    std::string macAddress = productP2PID.substr(6, productP2PID.length() - 6);
    SPDLOG_INFO("productId to macAddress. productP2PId = {}, macAddress = {}", productP2PID, macAddress);
    std::string rID;

    static std::set<unsigned char> numberAlpha = {'0','1','2','3','4','5','6','7','8','9',
                                           'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z',
                                           'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
    for(auto& ch : macAddress){

        if(numberAlpha.end() != numberAlpha.find(ch)){
            rID += ch;
        }
    }

    SPDLOG_TRACE("Changed productId to rId. productP2PId = {}, rId = {}", productP2PID, rID);
    return rID;
}

bool CloudService::getLatestIssuedDeviceIdAndSupportDtls(const std::string& rId, const CloudDevicesInfo &deviceInfo, std::string& deviceId, bool& supportDtls)
{
    bool isDtls = false;

    for(auto& device : deviceInfo.data.devices){

        //latestIssuedDeviceId 는 장치에서 마지막으로 등록하여 발급된 deviceId임을 알려주는 값.
        if(boost::iequals(rId,device.rId) && device.latestIssuedDeviceId){
            deviceId = device.deviceId;


            // 2022.05.04. 신, 장비(ARN/XRN/QRN)는 DTLS ON/OFF 기능을 지원함
            // 구, 장비는 Cloud RestAPI를 호출하면 "encyrption" 필드가 존재하지 않는다.
            supportDtls = false;
            for (auto& encryption : device.encryption) {
                if ("dtls" == encryption) {
                    isDtls = true;
                }
            }

            supportDtls = isDtls;
            SPDLOG_INFO("Successfully found DeviceID. id={} supportDtls={}", deviceId, supportDtls);

            return true;
        }

    }

    SPDLOG_ERROR("Not found a deviceID. rId={}", rId);
    return false;
}

bool CloudService::pushTurnInfo(CloudP2PInfo &p2pInfo, const CloudTurnInfo &turnInfo)
{
    for(auto& iceServer : turnInfo.data.iceServers){

        bool isTurn = false;
        for(auto& url : iceServer.urls){

            size_t pos = url.find("turn:");
            if(pos != std::string::npos){
                p2pInfo.turnUrl = url.substr(pos, url.length()-pos);
                isTurn = true;
                break;
            }
            pos = url.find("stun:");
            if(pos != std::string::npos){
                p2pInfo.stunUrl = url.substr(pos, url.length()-pos);
                break;
            }
        }

        if(isTurn){
            p2pInfo.turnId = iceServer.userName;
            p2pInfo.turnCredential = iceServer.credential;
        }
    }

    if(p2pInfo.turnUrl.empty()){
        SPDLOG_ERROR("Turn URL is empty.");
        return false;
    }
    if(p2pInfo.turnId.empty()){
        SPDLOG_ERROR("Turn ID is empty.");
        return false;
    }
    if(p2pInfo.turnCredential.empty()){
        SPDLOG_ERROR("Turn Credential is empty.");
        return false;
    }
    if(p2pInfo.stunUrl.empty()){
        SPDLOG_ERROR("Stun URL is empty.");
        return false;
    }

    return true;
}

void CloudService::createKey(std::string& userName, std::string& password)
{
    //creat a username. (ID: Max 32 character)
    userName = QUuid::createUuid().toString(QUuid::Id128).toStdString();

    //create a password (PW: Max 14 character) : 필수 영대/영소/숫자/특수 문자 중 3종류포함.
    QString uuid = QUuid::createUuid().toString(QUuid::Id128);
    QString pass = uuid.left(10); // 10자리는 랜덤하게 만듬.
    pass += "vZ3!"; // 4자리는 필수항목이 들어가도록 추가.
    password = pass.toStdString();
}

std::string CloudService::createIotHubId(const std::string &userName)
{
    std::string iotHubId;
    // 생성 규칙: userId + “-client_type-“ + [0-9a-z]{8}
    // client type :“-t-“ “-w-“, “-i-“, “-a-“ : thick client(wisenet viewer), webviewer, iOS, android

     static const char alphanum[] = "0123456789abcdefghijklmnopqrstuvwxyz";

     std::random_device rd;

     std::mt19937 gen(rd());

     std::uniform_int_distribution<int> dis(0, 35);

    iotHubId = userName + "-t-";
    for(int i = 0; i < 8; ++i){
        iotHubId += alphanum[dis(gen) % (sizeof(alphanum) - 1)];
    }
    return iotHubId;
}
}
}

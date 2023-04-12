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
#include "CloudHttps.h"

#include <boost/beast.hpp>
#include <boost/beast/ssl.hpp>

#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/error.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace Wisenet
{
namespace Library
{

const int CloudHttps::VERSION = 11;
const std::string CloudHttps::USER_URI = "/v1.0/user-management/user";
const std::string CloudHttps::TOKEN_URI = "/v1.0/oauth2/token";
const std::string CloudHttps::DEVICE_URI = "/v1.0/device-management/user/device";
const std::string CloudHttps::DEVICES_URI = "/v1.0/device-management/user/devices";
const std::string CloudHttps::TURN_URI = "/v1.0/turn/token";
CloudHttps::CloudHttps(boost::asio::io_context& ioc, const CloudHttpConnectionInfo& connectionInfo)
    :m_connectionInfo(connectionInfo)
    ,m_ioContext(ioc)
    ,m_logPrefix("[CLOUD HTTPS]")
{
    SPDLOG_DEBUG("CloudHttps::CloudHttps()");
}

CloudHttps::~CloudHttps()
{
    SPDLOG_DEBUG("CloudHttps::~CloudHttps()");
}

bool CloudHttps::CreateUser(const CloudUserInfo& info)
{
    SPDLOG_DEBUG("CloudHttps::CreateUser - START");
    auto user = info;
    JsonWriter writer;
    writer & user;

    int status = 0;
    std::string response;

    requestPost(boost::beast::http::verb::post, m_connectionInfo.api, USER_URI,"", writer.GetString(),status,response);

    if(200 == status || 201 == status){
        SPDLOG_INFO("CreateUser Succeeded - username={}",info.userName);
        return true;
    }

    SPDLOG_ERROR("CreateUser Failed - status={}",response);

    return false;
}

int CloudHttps::GetToken(const CloudUserInfo &info, std::string& accessToken, std::string& iothubAuthName, std::string &accessTokenIotHub)
{
    std::string target = TOKEN_URI;
    //client_id 는 windows 클라이언트 고정 값.(4496c8fa-a617-484f-8bae-64693b07200d)
    target += "?grant_type=password&username=" + info.userName + "&password=" + info.password + "&client_id=4496c8fa-a617-484f-8bae-64693b07200d";

    int status = 0;
    std::string response;
    SPDLOG_TRACE("target = {}", target);
    SPDLOG_DEBUG("target = {}", TOKEN_URI);
    requestPost(boost::beast::http::verb::post, m_connectionInfo.auth, target,"", "", status, response);

    if(200 == status || 201 == status){

        CloudTokenInfo tokenInfo;
        JsonReader reader(response.c_str());
        reader & tokenInfo;

        accessToken = tokenInfo.accessToken;
        iothubAuthName = tokenInfo.userId;
        accessTokenIotHub = tokenInfo.accessTokenIotHub;
        SPDLOG_TRACE("GetToken Succeeded - accessToken = {}", accessToken);
        SPDLOG_TRACE("GetToken Succeeded - userId = {}", iothubAuthName);
        SPDLOG_TRACE("GetToken Succeeded - accessTokenIotHub = {}", accessTokenIotHub);

        return status;
    }

    SPDLOG_ERROR("GetToken Failed - status={}",response);

    return status;

}

int CloudHttps::RegisterDevice(const std::string &accessToken, const CloudDeviceRegisterInfo &deviceRegisterInfo, std::string& deviceId)
{
    auto info = deviceRegisterInfo;
    JsonWriter writer;
    writer & info;

    SPDLOG_TRACE("RegisterDevice request = {}", writer.GetString());
    int status = 0;
    std::string response;
    requestPost(boost::beast::http::verb::post, m_connectionInfo.api, DEVICE_URI, accessToken ,writer.GetString(),status,response);

    if(200 == status || 201 == status){
        CloudDeviceIdInfo deviceIdInfo;
        JsonReader reader(response.c_str());
        reader & deviceIdInfo;

        deviceId = deviceIdInfo.data.deviceId;

        SPDLOG_INFO("Successfully registered device. DeviceId={}",deviceId);
    }else{
        CloudErrorInfo errorInfo;
        JsonReader reader(response.c_str());
        reader & errorInfo;

        status = errorInfo.code;
        SPDLOG_ERROR("RegisterDevice Failed - status={}",response);
    }

    return status;
}

bool CloudHttps::GetDevices(const std::string &accessToken, const std::string& rId, CloudDevicesInfo &devicesInfo)
{
    std::string target = DEVICES_URI;

    if(!rId.empty()){
        target += "?rId=";
        target += rId;
    }

    int status = 0;
    std::string response;
    requestPost(boost::beast::http::verb::get, m_connectionInfo.api, target ,accessToken,"", status,response);

    if(200 == status || 201 == status){
        SPDLOG_DEBUG("before reader = {}", response.c_str());
        JsonReader reader(response.c_str());
        reader & devicesInfo;

        SPDLOG_DEBUG("devices size = {}", devicesInfo.data.devices.size());
        for(auto device : devicesInfo.data.devices){
            SPDLOG_DEBUG("Device Info - deviceId = {}", device.deviceId);
            SPDLOG_DEBUG("Device Info - firmware = {}", device.firmware);
            SPDLOG_DEBUG("Device Info - model = {}", device.model);
            SPDLOG_DEBUG("Device Info - maxChannels = {} size()={}", device.maxChannels, device.encryption.size());
            for (auto encryption : device.encryption) {
                SPDLOG_DEBUG("Device Info - encryption = {}", encryption);
            }
        }

        return true;
    }

    SPDLOG_ERROR("GetDevices Failed - status={}",response);
    return false;
}

bool CloudHttps::GetTurnConfig(const std::string &accessToken, CloudTurnInfo &turnInfo)
{
    int status = 0;
    std::string response;
    requestPost(boost::beast::http::verb::post, m_connectionInfo.turn, TURN_URI ,accessToken,"", status,response);

    if(200 == status || 201 == status){
        JsonReader reader(response.c_str());
        reader & turnInfo;

        SPDLOG_DEBUG("iceServers size = {}", turnInfo.data.iceServers.size());
        for(auto iceServer : turnInfo.data.iceServers){
            SPDLOG_DEBUG("IceServer Info - url size = {}", iceServer.urls.size());

            for(auto url : iceServer.urls){
                SPDLOG_DEBUG("  IceServer Info - url = {}", url);
            }

            SPDLOG_DEBUG("IceServer Info - username = {}", iceServer.userName);
            SPDLOG_DEBUG("IceServer Info - credential = {}", iceServer.credential);
        }

        return true;
    }

    SPDLOG_ERROR("GetTurnConfig Failed - status={}",response);
    return false;
}

bool CloudHttps::GetTurnConfig2(const std::string& accessToken, CloudTurnInfo2& turnInfo)
{
    int status = 0;
    std::string response;
    requestPost(boost::beast::http::verb::post, m_connectionInfo.turn, TURN_URI, accessToken, "", status, response);

    if (200 == status || 201 == status) {
        JsonReader reader(response.c_str());
        reader& turnInfo;
        SPDLOG_DEBUG("stunAddress = {}", turnInfo.data.stunAddress);
        SPDLOG_DEBUG("turnAddress = {}", turnInfo.data.turnAddress);
        SPDLOG_DEBUG("turnUserName = {}", turnInfo.data.turnUserName);
        SPDLOG_DEBUG("turnCredential = {}", turnInfo.data.turnCredential);
        return true;
    }

    SPDLOG_ERROR("GetTurnConfig2 Failed - status={}", response);
    return false;
}

bool CloudHttps::DeleteDevices(const std::string& accessToken, const CloudDeleteDevicesInfo& deleteDevicesInfo)
{
    auto info = deleteDevicesInfo;
    JsonWriter writer;
    writer& info;

    SPDLOG_INFO("DeleteDevices request = {}", writer.GetString());
    int status = 0;
    std::string response;
    requestPost(boost::beast::http::verb::delete_, m_connectionInfo.api, DEVICES_URI, accessToken, writer.GetString(), status, response);

    if (200 == status || 201 == status) {

        SPDLOG_INFO("Successfully DeleteDevice. Response={}", response);
    }
    else {
        SPDLOG_ERROR("DeleteDevice Failed - status={}", response);
    }

    return status;
}

void CloudHttps::requestPost(boost::beast::http::verb verb, const std::string& host, const std::string &target, const std::string& auth, const std::string &postData, int& status, std::string& response)
{
    try{
        SPDLOG_DEBUG("CloudHttps::requestPost - START, host={}", host);
        // The SSL context is required, and holds certificates
        boost::asio::ssl::context ctx(boost::asio::ssl::context::tlsv12_client);

        SPDLOG_DEBUG("CloudHttps::requestPost - set_default_verify_paths");
        // This holds the root certificate used for verification
        ctx.set_default_verify_paths();
        // Verify the remote server's certificate
        SPDLOG_DEBUG("CloudHttps::requestPost - set_verify_mode");
        ctx.set_verify_mode(boost::asio::ssl::verify_none);

        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        boost::beast::ssl_stream<boost::beast::tcp_stream> stream(m_ioContext, ctx);

        SPDLOG_DEBUG("CloudHttps::requestPost - SSL_set_tlsext_host_name");
        // Set SNI Hostname (many hosts need this to handshake successfully)
        if(! SSL_set_tlsext_host_name(stream.native_handle(), host.c_str()))
        {
            boost::beast::error_code ec{static_cast<int>(::ERR_get_error()), boost::asio::error::get_ssl_category()};
            throw boost::beast::system_error{ec};
        }

        SPDLOG_DEBUG("CloudHttps::requestPost - resolve");
        // Look up the domain name
        boost::asio::ip::tcp::resolver::query query(host, m_connectionInfo.port);
        auto const results = resolver.resolve(query);

        SPDLOG_DEBUG("CloudHttps::requestPost - connect");
        // Make the connection on the IP address we get from a lookup
        boost::beast::get_lowest_layer(stream).connect(results);

        SPDLOG_DEBUG("CloudHttps::requestPost - handshake");
        // Perform the SSL handshake
        stream.handshake(boost::asio::ssl::stream_base::client);


        // Set up an HTTP GET request message
        boost::beast::http::request<boost::beast::http::string_body> req{verb, target.c_str(), VERSION};
        req.set(boost::beast::http::field::host, host);
        req.set(boost::beast::http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        if(!auth.empty()){
            std::string authorization = "Bearer " + auth;
            req.set(boost::beast::http::field::authorization, authorization);
        }

        if(!postData.empty()){
            req.content_length(postData.size());
            req.set(boost::beast::http::field::content_type, "application/json");
            req.body() = postData;
        }

        SPDLOG_DEBUG("CloudHttps::requestPost - write");

        // Send the HTTP request to the remote host
        boost::beast::http::write(stream, req);

        // This buffer is used for reading and must be persisted
        boost::beast::flat_buffer buffer;

        // Declare a container to hold the response
        boost::beast::http::response<boost::beast::http::dynamic_body> res;

        // Receive the HTTP response
        boost::beast::http::read(stream, buffer, res);

        std::stringstream ss;
        ss << "\n" << res;
        SPDLOG_DEBUG("CloudHttps::requestPost - Respone");
 
        status = res.result_int();

        if(res.body().size() > 0){
            SPDLOG_DEBUG("CloudHttps::requestPost - Respone Body size = {}", res.body().size());
            response = boost::beast::buffers_to_string(res.body().data());
        }

        boost::beast::error_code ec;
        boost::beast::get_lowest_layer(stream).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        boost::beast::get_lowest_layer(stream).close();

        if(ec == boost::asio::error::eof)
        {
            // Rationale:
            // http://stackoverflow.com/questions/25587403/boost-asio-ssl-async-shutdown-always-finishes-with-an-error
            ec = {};
        }
        if(ec)
            throw boost::beast::system_error{ec};

    }catch(std::exception const& e){
        SPDLOG_ERROR("Error = {}", e.what());
    }
}

}
}

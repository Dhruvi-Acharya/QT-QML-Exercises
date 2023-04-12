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
#pragma once

#include "CloudJsonStructure.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include "LogSettings.h"

namespace Wisenet
{
namespace Library
{

struct CloudHttpConnectionInfo
{
    std::string    api;
    std::string    auth;
    std::string    turn;
    std::string    port;
};

class CloudHttps
{
public:
    explicit CloudHttps(boost::asio::io_context& ioc, const CloudHttpConnectionInfo& connectionInfo);
    ~CloudHttps();

    bool CreateUser(const CloudUserInfo& info);
    int GetToken(const CloudUserInfo& info, std::string& accessToken, std::string& iothubAuthName, std::string& accessTokenIotHub);
    int RegisterDevice(const std::string& accessToken, const CloudDeviceRegisterInfo& deviceRegisterInfo, std::string& deviceId);
    bool GetDevices(const std::string& accessToken, const std::string& rId, CloudDevicesInfo& devicesInfo);
    bool GetTurnConfig(const std::string& accessToken, CloudTurnInfo& turnInfo);
    bool GetTurnConfig2(const std::string& accessToken, CloudTurnInfo2& turnInfo);
    bool DeleteDevices(const std::string& accessToken, const CloudDeleteDevicesInfo& deleteDevicesInfo);
private:
    void requestPost(boost::beast::http::verb verb, const std::string& host, const std::string& target, const std::string& auth, const std::string& postData, int& status, std::string& response);

    static const int            VERSION;
    static const std::string    USER_URI;
    static const std::string    TOKEN_URI;
    static const std::string    DEVICE_URI;
    static const std::string    DEVICES_URI;
    static const std::string    TURN_URI;

    CloudHttpConnectionInfo     m_connectionInfo;
    boost::asio::io_context&    m_ioContext;
    std::string m_logPrefix;
};

}
}

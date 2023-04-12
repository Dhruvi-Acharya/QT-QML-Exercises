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

#include <memory>
#include <string>
#include <functional>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include "CloudHttps.h"

class ThreadPool;

namespace Wisenet
{
namespace Library
{

struct CloudP2PInfo
{
    std::string deviceId;
    std::string iotHubUrl;          // SIPPROXY KEY: MQTT_HOST
    std::string iotHubId;           // SIPPROXY KEY: MQTT_CLIENT_ID
    std::string iotHubAuthName;     // SIPPROXY KEY: MQTT_AUTH_ID
    std::string iotHubAuthPassword; // SIPPROXY KEY: MQTT_AUTH_PW
    std::string turnUrl;            // SIPPROXY KEY: TURN_URL
    std::string turnId;             // SIPPROXY KEY:
    std::string turnCredential;     // SIPPROXY KEY:
    std::string stunUrl;            // SIPPROXY KEY:
    bool supportDtls = false;       // SIPPROXY KEY:
};

struct CloudUser
{
    std::string cloudUserID;
    std::string userName;
    std::string password;
};

struct CloudDevice
{
    std::string productP2PID;
    std::string userName;
    std::string password;
};

enum class CloudErrorCode
{
    NoError = 0,
    NetworkError,
    UnAuthorizedDevice,
    InvalidValue,
    AddingUserError,
    AddingDeviceError
};

typedef std::function <void(const CloudUser&)> ChangeUserHandler;
typedef std::function <void(const CloudErrorCode&, const CloudP2PInfo&)> GetP2PInfoHandler;

class CloudService : public std::enable_shared_from_this<CloudService>
{
public:
    CloudService();

    void Start(const std::map<std::string, std::string>& configs, const CloudUser& cloudUser, ChangeUserHandler const& changeUserHandler);
    void Stop();
    void AsyncGetP2PInfo(CloudDevice device,  GetP2PInfoHandler const& getP2PInfoHandler);
    void AsyncDeleteDevice(const std::vector<std::string>& productP2PIDs);

private:
    bool createUser(CloudUser& cloudUser);
    void asyncGetP2PInfo(CloudUser cloudUser, CloudDevice device, GetP2PInfoHandler const& getP2PInfoHandler);

    std::string toRID(const std::string& productP2PID);
    bool getLatestIssuedDeviceIdAndSupportDtls(const std::string& rId, const CloudDevicesInfo& deviceInfo, std::string& deviceId, bool& supportDtls);
    bool pushTurnInfo(CloudP2PInfo& p2pInfo, const CloudTurnInfo& turnInfo);
    void createKey(std::string& userName, std::string& password);
    std::string createIotHubId(const std::string& userName);

    std::shared_ptr<ThreadPool> m_contextPool;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;

    ChangeUserHandler       m_changeUserHandler;
    CloudUser               m_cloudUser;
    CloudHttpConnectionInfo m_connectionInfo;
    std::string             m_iotHubUrl;
    std::string             m_iotHubId;

    bool                    m_useSTUN;
    bool                    m_useTURN;
    bool                    m_useNewCloudAPI;

    std::string             m_accessToken;
    std::string             m_iotHubAuthName;
    std::string             m_iotHubAuthPassword;
};


}
}

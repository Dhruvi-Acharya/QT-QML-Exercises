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

#include <DeviceClient/DeviceStructure.h>
#include <boost/asio.hpp>

#include <sipproxy.h>

#include "CloudService.h"

namespace Wisenet
{
namespace Device
{

class AvailableTcpPort
{
public:
    AvailableTcpPort();
    static AvailableTcpPort& getInstance() {
        static AvailableTcpPort s;
        return s;
    }

    bool getPort(unsigned short& port);
private:
    std::mutex m_mutex;
};

class EasyConnection
{
public:
    EasyConnection(const std::string& deviceUUID);

    static bool isValidProductP2PID(const std::string& productP2PID);

    bool GetIpAndPort(Wisenet::Device::DeviceConnectInfo& connInfo, const Wisenet::Library::CloudP2PInfo & p2pInfo);

    SIPPROXY_CONNECTION_TYPE GetConnectedType();

    bool StopP2PService();
private:    
    bool parseDeviceInfoJson(const std::string& deviceInfo, unsigned short& devicePort, unsigned short& httpsPort, unsigned short& rtspPort);
 
    std::string                 m_logPrefix;

    std::string                 m_p2pUID;
    EC_HANDLE                   m_echandle;
    SIPPROXY_HANDLE             m_devicePortProxyHandle;
    SIPPROXY_HANDLE             m_httpsPortProxyHandle;
    SIPPROXY_HANDLE             m_rtspPortProxyHandle;
};

}
}

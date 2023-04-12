/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/

#include "tst_test__sunapideviceclient.h"
#include <boost/core/ignore_unused.hpp>
#include "CloudService.h"

void Test__SunapiDeviceClient::test_ConnectDisconnect()
{
    if (!m_deviceClient) {
        m_deviceClient = std::make_shared<Wisenet::Device::SunapiDeviceClient>(m_threadPool.ioContext(),
                                                                               m_streamIoContextPool.ioContext(),
                                                                               m_deviceUuid,
                                                                               m_cloudService, 15, false, 1831);
    }
    m_deviceClient->SetDeviceEventHandler([](const EventBaseSharedPtr& event)
        {
            boost::ignore_unused(event);
        });

    auto request = std::make_shared<DeviceConnectRequest>();
    request->deviceID = m_deviceUuid;
    request->connectionInfo.host = m_ip;
    request->connectionInfo.port = m_port;
    request->connectionInfo.user = m_user;
    request->connectionInfo.password = m_password;

    SPDLOG_DEBUG("DeviceConnect");

    InitializeNotify();
    m_deviceClient->DeviceConnect(request,
                                  [this, request](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceConnect() failed, error_string={}", response->errorString());
            Notify(false);
            return;
        }
        auto deviceObj = std::static_pointer_cast<DeviceConnectResponse>(response);

        QVERIFY(request->connectionInfo.host == deviceObj->device.connectionInfo.host);
        QVERIFY(request->connectionInfo.port == deviceObj->device.connectionInfo.port);
        QVERIFY(request->connectionInfo.user == deviceObj->device.connectionInfo.user);

        QVERIFY(deviceObj->device.channels.size());

        QVERIFY(deviceObj->deviceProfileInfo.videoProfiles.size() > 0);
        SPDLOG_DEBUG("PROFILE Channel Count={}", deviceObj->deviceProfileInfo.videoProfiles.size());


        Notify(true);

    });

    if(!Wait(5)) {
        QFAIL("DeviceConnect() failed");
    }

    auto disConnectRequest = std::make_shared<DeviceDisconnectRequest>();
    m_deviceClient->DeviceDisconnect(disConnectRequest, nullptr);
    m_deviceClient.reset();
}

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

#include <thread>
#include <mutex>

#include <boost/core/ignore_unused.hpp>
#include <condition_variable>
#include <chrono>


void Test__SunapiDeviceClient::test_AreaZoom()
{
    SPDLOG_DEBUG("[1] DeviceConnect");

    std::string originalPassword = m_password;
    QVERIFY2(connectInternal(), "[1] --> Can not connect to the device..");

    // AreaZoom

    SPDLOG_DEBUG("[2] Area Zoom");

    auto request = std::make_shared<DeviceAreaZoomRequest>();

    request->channelID = "1";
    request->profile = 1;
    request->x1 = 10;
    request->y1 = 10;
    request->x2 = 1000;
    request->y2 = 1000;
    request->tileWidth = 1920;
    request->tileHeight = 1080;

    InitializeNotify();
    m_deviceClient->DeviceAreaZoom(request,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[2] --> DeviceAreaZoom() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[2] --> Failed to control a area zoom.");

    disconnectInternal();

    std::this_thread::sleep_for(std::chrono::seconds(5));
}

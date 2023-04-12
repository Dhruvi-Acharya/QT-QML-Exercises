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


void Test__SunapiDeviceClient::test_DigitalAutoTracking()
{
    SPDLOG_DEBUG("[1] DeviceConnect");
    QVERIFY2(connectInternal(), "[1] --> Can not connect to the device..");

    // DeviceControlDigitalAutoTracking start
    auto controlDatRequest = std::make_shared<DeviceSetDigitalAutoTrackingRequest>();
    controlDatRequest->channelID = "1";
    controlDatRequest->profile = 2;
    controlDatRequest->enable = true;

    SPDLOG_DEBUG("[2] DeviceSetDigitalAutoTracking. Profile = 2, Mode = Start");
    InitializeNotify();
    m_deviceClient->DeviceSetDigitalAutoTracking(controlDatRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[2] --> DeviceSetDigitalAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[2] --> Failed to call a DeviceSetDigitalAutoTracking.");

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // DeviceControlDigitalAutoTracking stop
    controlDatRequest->channelID = "1";
    controlDatRequest->profile = 2;
    controlDatRequest->enable = false;

    SPDLOG_DEBUG("[3] DeviceSetDigitalAutoTracking. Profile = 2, Mode = Stop");
    InitializeNotify();
    m_deviceClient->DeviceSetDigitalAutoTracking(controlDatRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[3] --> DeviceSetDigitalAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[3] --> Failed to call a DeviceSetDigitalAutoTracking.");


    disconnectInternal();

    std::this_thread::sleep_for(std::chrono::seconds(5));
}

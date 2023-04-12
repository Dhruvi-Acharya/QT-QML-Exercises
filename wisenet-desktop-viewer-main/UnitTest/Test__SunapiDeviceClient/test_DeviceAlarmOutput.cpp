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

#pragma once
#include "tst_test__sunapideviceclient.h"

void Test__SunapiDeviceClient::test_AlarmOutput()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    SPDLOG_INFO("test_AlarmOutput");

    //set alarmOutput on
    auto setRequest = std::make_shared<DeviceSetAlarmOutputRequest>();
    setRequest->on = true;
    setRequest->deviceOutputID = "1";

    InitializeNotify();
    m_deviceClient->DeviceSetAlarmOutput(setRequest,[this] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceSetAlarmOutput() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceSetAlarmOutput() alarmOutput({})success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceSetAlarmOutput() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //set alarmOutput off
    setRequest->on = false;
    setRequest->deviceOutputID = "1";

    InitializeNotify();
    m_deviceClient->DeviceSetAlarmOutput(setRequest,[this] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceSetAlarmOutput() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceSetAlarmOutput() alarmOutput({})success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceSetAlarmOutput() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

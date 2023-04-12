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

void Test__SunapiDeviceClient::test_Defog()
{
    SPDLOG_INFO("test_ImageEnhancementsDefog");
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    //set defog
    auto setRequest = std::make_shared<DeviceSetDefogRequest>();
    setRequest->channelID = "1";
    setRequest->defogMode = DefogMode::Auto;
    setRequest->defogLevel = 5;

    InitializeNotify();
    m_deviceClient->DeviceSetDefog(setRequest,
                                   [this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceSetDefog() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceSetDefog() success, set defog info");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceSetDefog() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //check defog info
    auto getRequest = std::make_shared<DeviceGetDefogRequest>();
    getRequest->channelID = "1";

    DefogMode defogMode = DefogMode::Off;
    int defogLevel = 0;

    InitializeNotify();
    m_deviceClient->DeviceGetDefog(getRequest,
                                   [this, &defogMode, &defogLevel](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceGetDefog() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            auto defogResponse = std::static_pointer_cast<DeviceGetDefogResponse>(response);
            SPDLOG_INFO("DeviceGetDefog() success, get defog info");
            SPDLOG_INFO("DeviceGetDefog() defogMode : {}", defogResponse->defogMode);
            SPDLOG_INFO("DeviceGetDefog() defogLevel : {}", defogResponse->defogLevel);

            defogMode = defogResponse->defogMode;
            defogLevel = defogResponse->defogLevel;
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceGetDefog() failed");
    QVERIFY(defogMode == setRequest->defogMode);
    QVERIFY(defogLevel == setRequest->defogLevel);

    //set defogmode to off
    setRequest->defogMode = DefogMode::Off;
    InitializeNotify();
    m_deviceClient->DeviceSetDefog(setRequest,
                                   [this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceSetDefog() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceSetDefog() success, set defog info");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceSetDefog() failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

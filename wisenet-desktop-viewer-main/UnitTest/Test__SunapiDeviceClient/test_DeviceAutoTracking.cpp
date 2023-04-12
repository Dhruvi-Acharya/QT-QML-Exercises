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


void Test__SunapiDeviceClient::test_AutoTracking()
{
    SPDLOG_DEBUG("[1] DeviceConnect");

    QVERIFY2(connectInternal(), "[1] --> Can not connect to the device..");

    // DeviceSetAutoTracking : Enable = true
    auto deviceSetAutoTrackingRequest = std::make_shared<DeviceSetAutoTrackingRequest>();
    deviceSetAutoTrackingRequest->channelID = "1";
    deviceSetAutoTrackingRequest->enable = true;

    SPDLOG_DEBUG("[2] DeviceSetAutoTracking. Enable = {}", deviceSetAutoTrackingRequest->enable);
    InitializeNotify();
    m_deviceClient->DeviceSetAutoTracking(deviceSetAutoTrackingRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[2] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[2] --> Failed to call a DeviceSetAutoTracking.");

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // DeviceGetAutoTracking
    SPDLOG_DEBUG("[3] DeviceGetAutoTracking");

    auto request = std::make_shared<DeviceGetAutoTrackingRequest>();
    request->channelID = "1";
    auto enable = false;

    InitializeNotify();
    m_deviceClient->DeviceGetAutoTracking(request,[this, &enable](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[3] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            enable = std::static_pointer_cast<DeviceGetAutoTrackingResponse>(response)->enable;
            SPDLOG_INFO("[3] --> Enable = {}", enable);
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[3] --> Failed to call a DeviceGetAutoTracking.");

    SPDLOG_INFO("[3] --> Set Enable = {}, Get Enable = {}", true, enable);
    QVERIFY2(enable, "[3] --> The values obtained after setting do not match.");

    // DeviceSetAutoTracking : Enable = false
    deviceSetAutoTrackingRequest->channelID = "1";
    deviceSetAutoTrackingRequest->enable = false;

    SPDLOG_DEBUG("[4] DeviceSetAutoTracking. Enable = {}", deviceSetAutoTrackingRequest->enable);
    InitializeNotify();
    m_deviceClient->DeviceSetAutoTracking(deviceSetAutoTrackingRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[4] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[4] --> Failed to call a DeviceSetAutoTracking.");

    std::this_thread::sleep_for(std::chrono::seconds(5));

    // DeviceGetAutoTracking

    SPDLOG_DEBUG("[5] DeviceGetAutoTracking");
    request->channelID = "1";


    enable = false;
    InitializeNotify();
    m_deviceClient->DeviceGetAutoTracking(request,[this, &enable](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[5] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            enable = std::static_pointer_cast<DeviceGetAutoTrackingResponse>(response)->enable;
            SPDLOG_INFO("[5] --> Enable = {}", enable);
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[5] --> Failed to call a DeviceGetAutoTracking.");

    SPDLOG_INFO("[5] --> Set Enable = {}, Get Enable = {}", false, enable);
    QVERIFY2(!enable, "[5] --> The values obtained after setting do not match.");

    // DeviceControlTargetLockCoordinate
    auto deviceControlTargetLockCoordinateRequest = std::make_shared<DeviceSetTargetLockCoordinateRequest>();
    deviceControlTargetLockCoordinateRequest->channelID = "1";
    deviceControlTargetLockCoordinateRequest->x = 5000;
    deviceControlTargetLockCoordinateRequest->y = 5000;

    SPDLOG_DEBUG("[6] DeviceSetTargetLockCoordinate. x = {}, y = {}", deviceControlTargetLockCoordinateRequest->x, deviceControlTargetLockCoordinateRequest->y);
    InitializeNotify();
    m_deviceClient->DeviceSetTargetLockCoordinate(deviceControlTargetLockCoordinateRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[6] --> DeviceSetTargetLockCoordinate() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[6] --> Failed to call a DeviceSetTargetLockCoordinate.");

    std::this_thread::sleep_for(std::chrono::seconds(10));

    // DeviceGetAutoTracking
    SPDLOG_DEBUG("[7] DeviceGetAutoTracking");
    request->channelID = "1";
    enable = false;
    InitializeNotify();
    m_deviceClient->DeviceGetAutoTracking(request,[this, &enable](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[7] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            enable = std::static_pointer_cast<DeviceGetAutoTrackingResponse>(response)->enable;
            SPDLOG_INFO("[7] --> Enable = {}", enable);
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[7] --> Failed to call a DeviceGetAutoTracking.");

    SPDLOG_INFO("[7] --> Set Enable = {}, Get Enable = {}", true, enable);
    QVERIFY2(enable, "[7] --> The values obtained after setting do not match.");

    // DeviceSetAutoTracking : Enable = false
    deviceSetAutoTrackingRequest->channelID = "1";
    deviceSetAutoTrackingRequest->enable = false;

    SPDLOG_DEBUG("[8] DeviceSetAutoTracking. Enable = {}", deviceSetAutoTrackingRequest->enable);
    InitializeNotify();
    m_deviceClient->DeviceSetAutoTracking(deviceSetAutoTrackingRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("[8] --> DeviceGetAutoTracking() failed, error_string={}", response->errorString());
            Notify(false);
        } else {
            Notify(true);
        }
    });

    QVERIFY2(Wait(),"[8] --> Failed to call a DeviceSetAutoTracking.");

    std::this_thread::sleep_for(std::chrono::seconds(5));

    disconnectInternal();

    std::this_thread::sleep_for(std::chrono::seconds(5));
}

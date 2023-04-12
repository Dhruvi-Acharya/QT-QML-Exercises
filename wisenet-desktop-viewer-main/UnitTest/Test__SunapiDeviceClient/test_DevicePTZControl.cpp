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

void Test__SunapiDeviceClient::test_DevicePTZControl()
{
    SPDLOG_INFO("test_DevicePTZControl");

    QVERIFY2(connectInternal(), "Can not connect to the device..");

    /* PTZControl */

    //move
    SPDLOG_INFO("[PTZControl] Move device");
    auto request = std::make_shared<DevicePTZContinuousRequest>();
    request->channelID = "1";
    request->pan = -55;
    request->tilt = 2;
    request->zoom = 0;

    InitializeNotify();
    m_deviceClient->DevicePTZContinuous(request,
                                        [this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(3), "DevicePTZContinuousControl() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //stop
    SPDLOG_INFO("[PTZControl] Stop device");

    auto stopRequest = std::make_shared<DevicePTZStopRequest>();
    InitializeNotify();
    m_deviceClient->DevicePTZStop(stopRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(2), "DevicePTZContinuousControl()-focus failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //focus -> manual로 변경되어야 함.
    //focus test (10번정도 해야 변화를 볼 수 있음)
    auto focusRequest = std::make_shared<DevicePTZFocusRequest>();
    focusRequest->focus = DevicePTZFocusRequest::Focus::Near;
    for(int i = 0; i< 10 ; i++){
        InitializeNotify();
        m_deviceClient->DevicePTZFocus(focusRequest, [this](const ResponseBaseSharedPtr& response)
        {
            if(response->isFailed()){
                SPDLOG_WARN("DevicePTZFocusControl() failed, error_string={}", response->errorString());
                Notify(false);
            }
            else
                Notify(true);
        });
        QVERIFY2(Wait(3), "DevicePTZFocusControl() failed");
    }

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    focusRequest->focus = DevicePTZFocusRequest::Focus::Far;
    for(int i = 0; i< 10 ; i++){
        InitializeNotify();
        m_deviceClient->DevicePTZFocus(focusRequest,
                                       [this](const ResponseBaseSharedPtr& response)
        {
            if(response->isFailed()){
                SPDLOG_WARN("DevicePTZFocusControl() failed, error_string={}", response->errorString());
                Notify(false);
            }
            else
                Notify(true);
        });
        QVERIFY2(Wait(3), "DevicePTZFocusControl() failed");
    }
    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

void Test__SunapiDeviceClient::test_DPTZControlPanorama()
{
    SPDLOG_INFO("test_DPTZControl - panorama");

    QVERIFY2(connectInternal(), "Can not connect to the device..");

    /*Fish-eye DPTZ Move*/

    //panorama move
    SPDLOG_INFO("[DPTZControl] Move Panorama profile");
    auto request = std::make_shared<DevicePTZContinuousRequest>();
    request->channelID = "1";
    request->pan = 40;
    request->viewModeType = ViewModeType::Panorama;

    InitializeNotify();
    m_deviceClient->DevicePTZContinuous(request,
                                        [this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(), "DevicePTZContinuousControl() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //stop
    SPDLOG_INFO("[DPTZControl] Stop Panorama profile");
    auto stopRequest = std::make_shared<DevicePTZStopRequest>();
    stopRequest->viewModeType = ViewModeType::Panorama;
    InitializeNotify();
    m_deviceClient->DevicePTZStop(stopRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(2), "DevicePTZStopControl failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(10));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

void Test__SunapiDeviceClient::test_DPTZControlQuadView()
{
    SPDLOG_INFO("test_DPTZControl - QuadView");

    QVERIFY2(connectInternal(), "Can not connect to the device..");

    /*Fish-eye DPTZ Move*/

    //QuadView move
    SPDLOG_INFO("[DPTZControl] Move QuadView profile");
    auto request = std::make_shared<DevicePTZContinuousRequest>();
    request->channelID = "1";
    request->pan = 100;
    request->subViewIndex = 1;
    InitializeNotify();
    m_deviceClient->DevicePTZContinuous(request,
                                        [this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(), "DevicePTZContinuousControl() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    //stop
    SPDLOG_INFO("[DPTZControl] Stop QuadView profile");
    auto stopRequest = std::make_shared<DevicePTZStopRequest>();
    stopRequest->subViewIndex = 1;
    InitializeNotify();
    m_deviceClient->DevicePTZStop(stopRequest,[this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZContinuousControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(2), "DevicePTZStopControl failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

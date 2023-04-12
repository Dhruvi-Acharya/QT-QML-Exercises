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
#include <map>
#include "tst_test__sunapideviceclient.h"

void Test__SunapiDeviceClient::test_DevicePreset()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    SPDLOG_INFO("test_DevicePreset");

    //get Preset List
    SPDLOG_INFO("[test_DevicePreset] Get Preset List");

    auto getRequest = std::make_shared<DeviceGetPresetRequest>();
    getRequest->channelID = "1";

    std::map<unsigned int, std::string> presets;
    InitializeNotify();
    m_deviceClient->DeviceGetPreset(getRequest, [this, &presets]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("GetDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceGetDevicePreset() success, get preset list");
            auto presetResponse = std::static_pointer_cast<DeviceGetPresetResponse>(response);
            for(std::map<unsigned int, std::string>::iterator iter = presetResponse->presets.begin();
                iter != presetResponse->presets.end(); iter++){
                SPDLOG_INFO("GetDevicePreset() Result : [ {} , {} ]",(*iter).first ,(*iter).second);
            }
            presets = presetResponse->presets;
            Notify(true);
        }
    });    
    QVERIFY2(Wait(3), "DeviceGetDevicePreset() failed");

    //ADD and update preset
    SPDLOG_INFO("[test_DevicePresetManagement] Add Preset");
    auto addRequest = std::make_shared<DeviceAddPresetRequest>();

    std::map<unsigned int, std::string>::iterator iter;
    iter = presets.end();
    --iter;

    addRequest->channelID = "1";
    addRequest->preset = (*iter).first + 1;
    addRequest->name = std::to_string(addRequest->preset);

    InitializeNotify();
    m_deviceClient->DeviceAddPreset(addRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("AddDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("AddDevicePreset() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceAddDevicePreset() failed");

    //move preset1
    SPDLOG_INFO("[test_DevicePreset] Move Preset1");
    auto moveRequest = std::make_shared<DeviceMovePresetRequest>();

    iter = presets.begin();

    moveRequest->channelID = "1";
    moveRequest->preset = (*iter).first;

    InitializeNotify();
    m_deviceClient->DeviceMovePreset(moveRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceMovePresetRequest() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceMovePresetRequest() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceMovePresetRequest() failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //move preset2
    SPDLOG_INFO("[test_DevicePreset] Move Preset2");
    moveRequest->channelID = "1";
    moveRequest->preset = addRequest->preset;

    InitializeNotify();
    m_deviceClient->DeviceMovePreset(moveRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceMoveDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceMoveDevicePreset() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceMoveDevicePreset() failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //remove preset
    SPDLOG_INFO("[test_DevicePreset] Remove Preset");
    auto removeRequest = std::make_shared<DeviceRemovePresetRequest>();

    removeRequest->channelID = "1";
    removeRequest->preset = addRequest->preset;

    InitializeNotify();
    m_deviceClient->DeviceRemovePreset(removeRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceRemovePresetRequest() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceRemovePresetRequest() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(3), "DeviceRemovePresetRequest() failed");

    //잘 지워졌는지 list 얻어서 확인
    SPDLOG_INFO("[test_DevicePreset] Check Preset List");
    InitializeNotify();
    m_deviceClient->DeviceGetPreset(getRequest, [this, moveRequest]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("GetDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceGetDevicePreset() success, get preset list");
            auto presetResponse = std::static_pointer_cast<DeviceGetPresetResponse>(response);
            //fail to find
            if(presetResponse->presets.find(moveRequest->preset) == presetResponse->presets.end())
                Notify(true);
            else
                Notify(false);
        }
    });
    QVERIFY2(Wait(3), "DeviceGetDevicePreset() failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

void Test__SunapiDeviceClient::test_DPTZPreset()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    SPDLOG_INFO("test_DevicePreset");

    //get Preset List
    SPDLOG_INFO("[test_DevicePreset] Get Preset List");

    auto getRequest = std::make_shared<DeviceGetPresetRequest>();
    getRequest->channelID = "1";

    std::map<unsigned int, std::string> presets;
    InitializeNotify();
    m_deviceClient->DeviceGetPreset(getRequest, [this, &presets]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("GetDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceGetDevicePreset() success, get preset list");
            auto presetResponse = std::static_pointer_cast<DeviceGetPresetResponse>(response);
            for(std::map<unsigned int, std::string>::iterator iter = presetResponse->presets.begin();
                iter != presetResponse->presets.end(); iter++){
                SPDLOG_INFO("GetDevicePreset() Result : [ {} , {} ]",(*iter).first ,(*iter).second);
            }
            presets = presetResponse->presets;
            Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceGetDevicePreset() failed");

    //ADD and update preset
    SPDLOG_INFO("[test_DevicePresetManagement] Add Preset");
    auto addRequest = std::make_shared<DeviceAddPresetRequest>();

    std::map<unsigned int, std::string>::iterator iter;
    iter = presets.end();
    --iter;

    addRequest->channelID = "1";
    addRequest->preset = (*iter).first + 1;
    addRequest->name = std::to_string(addRequest->preset);
    addRequest->subViewIndex = 1;

    InitializeNotify();
    m_deviceClient->DeviceAddPreset(addRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("AddDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("AddDevicePreset() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceAddDevicePreset() failed");

    //move preset1
    SPDLOG_INFO("[test_DevicePreset] Move Preset1");
    auto moveRequest = std::make_shared<DeviceMovePresetRequest>();

    iter = presets.begin();

    moveRequest->channelID = "1";
    moveRequest->preset = (*iter).first;
    moveRequest->subViewIndex = 1;

    InitializeNotify();
    m_deviceClient->DeviceMovePreset(moveRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceMovePresetRequest() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceMovePresetRequest() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceMovePresetRequest() failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //move preset2
    SPDLOG_INFO("[test_DevicePreset] Move Preset2");
    moveRequest->channelID = "1";
    moveRequest->preset = addRequest->preset;
    moveRequest->subViewIndex = 1;

    InitializeNotify();
    m_deviceClient->DeviceMovePreset(moveRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceMoveDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceMoveDevicePreset() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceMoveDevicePreset() failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));

    //remove preset
    SPDLOG_INFO("[test_DevicePreset] Remove Preset");
    auto removeRequest = std::make_shared<DeviceRemovePresetRequest>();

    removeRequest->channelID = "1";
    removeRequest->preset = addRequest->preset;

    InitializeNotify();
    m_deviceClient->DeviceRemovePreset(removeRequest, [this]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceRemovePresetRequest() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceRemovePresetRequest() success");
            Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceRemovePresetRequest() failed");

    //잘 지워졌는지 list 얻어서 확인
    SPDLOG_INFO("[test_DevicePreset] Check Preset List");
    InitializeNotify();
    m_deviceClient->DeviceGetPreset(getRequest, [this, moveRequest]
                                    (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("GetDevicePreset() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceGetDevicePreset() success, get preset list");
            auto presetResponse = std::static_pointer_cast<DeviceGetPresetResponse>(response);
            //fail to find
            if(presetResponse->presets.find(moveRequest->preset) == presetResponse->presets.end())
                Notify(true);
            else
                Notify(false);
        }
    });
    QVERIFY2(Wait(5), "DeviceGetDevicePreset() failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}


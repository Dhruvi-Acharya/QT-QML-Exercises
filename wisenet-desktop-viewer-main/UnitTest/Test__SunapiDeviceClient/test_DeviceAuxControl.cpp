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

void Test__SunapiDeviceClient::test_AuxControl()
{
    SPDLOG_INFO("test_DeviceAuxControl");

    QVERIFY2(connectInternal(), "Can not connect to the device..");

    //Aux Control
    auto auxRequest = std::make_shared<DevicePTZAuxControlRequest>();
    auxRequest->channelID = "1";
    auxRequest->command = "SpinningDryOn";
    InitializeNotify();
    m_deviceClient->DevicePTZAuxControl(auxRequest, [this](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed()){
            SPDLOG_WARN("DevicePTZAuxControl() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else
            Notify(true);
    });
    QVERIFY2(Wait(2), "DevicePTZAuxControl() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

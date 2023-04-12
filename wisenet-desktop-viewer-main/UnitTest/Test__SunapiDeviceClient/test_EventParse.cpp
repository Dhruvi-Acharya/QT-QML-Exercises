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
#include <boost/core/ignore_unused.hpp>

void Test__SunapiDeviceClient::test_EventParse()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    InitializeNotify();
    m_deviceClient->SetDeviceEventHandler([this](const DeviceEventBaseSharedPtr& event)
    {
        deviceEventHandler(event);
    });


    QVERIFY2(Wait(600), "Test_EventParse() failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");

    SPDLOG_INFO("test_EventParse");
}

void Test__SunapiDeviceClient::deviceEventHandler(const DeviceEventBaseSharedPtr &event)
{
    boost::ignore_unused(event);
    SPDLOG_INFO("deviceEventHandler()");
}

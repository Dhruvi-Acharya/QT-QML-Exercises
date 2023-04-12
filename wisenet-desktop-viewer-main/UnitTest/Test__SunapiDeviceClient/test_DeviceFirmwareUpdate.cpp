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
void Test__SunapiDeviceClient::test_FirmwareUpdate()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    auto request = std::make_shared<DeviceFirmwareUpdateRequest>();
    request->firmwareFilePath = "XND-8081REV_1.41.01_20201216_R86.img";

    InitializeNotify();
    m_deviceClient->DeviceFirmwareUpdate(request,
                                   [this, request]
                                   (const ResponseBaseSharedPtr& response)
    {
        if (!response->isContinue) {
            bool isSuccess = !response->isFailed();
            if (!isSuccess) {
                SPDLOG_WARN("DeviceFirmwareUpdate() failed, error_string={}", response->errorString());
            }
            else {
                SPDLOG_INFO("DeviceFirmwareUpdate() success, finalize test_configBackup.bin");
            }
            Notify(isSuccess);
        }
        else {
            SPDLOG_DEBUG("DeviceFirmwareUpdate() continue, failed={}", response->isFailed());
        }

    });

    QVERIFY2(Wait(300),"Failed to update device firmware."); // wait response 5minutes
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

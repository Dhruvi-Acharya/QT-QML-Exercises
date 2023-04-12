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

void Test__SunapiDeviceClient::test_ConfigBackup()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    auto request = std::make_shared<DeviceConfigBackupRequest>();
    request->backupFilePath = "test_configBackup.dat";
    request->password = "init123!!";

    std::atomic_bool completed(false);
    std::atomic_bool isSuccess(false);

    m_deviceClient->DeviceConfigBackup(request,
                                   [this, request, &completed, &isSuccess]
                                   (const ResponseBaseSharedPtr& response)
    {
        isSuccess = !response->isFailed();
        if (!isSuccess) {
            SPDLOG_WARN("DeviceConfigBackup() failed, error_string={}", response->errorString());
        }
        else {
            SPDLOG_INFO("DeviceConfigBackup() success, save test_configBackup.bin");
        }

        completed = true;

    });

    QTRY_VERIFY_WITH_TIMEOUT(completed, 10000);
    QVERIFY2(isSuccess, "DeviceConfigBackup() failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

void Test__SunapiDeviceClient::test_ConfigRestore()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    auto request = std::make_shared<DeviceConfigRestoreRequest>();
    request->excludeNetworkSettings = true;
    request->restoreFilePath = "test_configBackup.dat";
    request->password = "init123!!";

    std::atomic_bool completed(false);
    std::atomic_bool isSuccess(false);

    m_deviceClient->DeviceConfigRestore(request,
                                   [this, request, &completed, &isSuccess]
                                   (const ResponseBaseSharedPtr& response)
    {
        isSuccess = !response->isFailed();
        if (!isSuccess) {
            SPDLOG_WARN("DeviceConfigRestore() failed, error_string={}", response->errorString());
        }
        else {
            SPDLOG_INFO("DeviceConfigRestore() success, send test_configBackup.bin");
        }

        completed = true;

    });

    QTRY_VERIFY_WITH_TIMEOUT(completed, 10000);
    QVERIFY2(isSuccess, "DeviceConfigRestore() failed");

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

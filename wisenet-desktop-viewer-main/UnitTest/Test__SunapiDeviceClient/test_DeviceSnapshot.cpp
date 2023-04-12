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

void Test__SunapiDeviceClient::test_Snapshot()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    auto request = std::make_shared<DeviceSnapshotRequest>();
    request->channelID = "1";

    InitializeNotify();
    m_deviceClient->DeviceSnapshot(request,
                                   [this, request]
                                   (const ResponseBaseSharedPtr& response)
    {
        bool isSuccess = !response->isFailed();

        if (!isSuccess) {
            SPDLOG_WARN("DeviceSnapshot() failed, error_string={}", response->errorString());
        }
        else {
            SPDLOG_INFO("DeviceSnapshot() success, save test_snapshot.jpeg");
            auto snapshotResponse = std::static_pointer_cast<DeviceSnapshotResponse>(response);
            std::ofstream outFile("test_snapshotNew.jpeg",
                                  std::ofstream::out | std::ofstream::binary);
            std::copy(snapshotResponse->snapshotData.begin(),
                      snapshotResponse->snapshotData.end(),
                      std::ostreambuf_iterator<char>(outFile));
            outFile.close();
        }

        Notify(isSuccess);

    });

    QVERIFY2(Wait(3),"Failed to snapshot."); // wait response 5minutes
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

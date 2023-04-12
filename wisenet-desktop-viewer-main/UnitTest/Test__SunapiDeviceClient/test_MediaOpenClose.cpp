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
#include "TimeUtil.h"

void Test__SunapiDeviceClient::test_MediaOpenClose()
{
    SPDLOG_DEBUG("test_MediaOpenClose");


    /* DeviceConnect */
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));


    /* MediaOpenClose */

    // Open Video1
    SPDLOG_DEBUG("[VIDEO] Open Video1");
    DeviceMediaOpenRequestSharedPtr mediaOpenRequest1 = std::make_shared<DeviceMediaOpenRequest>();
    mediaOpenRequest1->mediaID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    mediaOpenRequest1->channelID = "1";
    mediaOpenRequest1->mediaRequestInfo.streamType = StreamType::liveHigh;
    mediaOpenRequest1->mediaRequestInfo.profileId = "2";
    std::string strStartTime = "2021-03-27T00:00:00Z";
    mediaOpenRequest1->mediaRequestInfo.startTime =Wisenet::Common::utcMsecsFromIsoString(strStartTime);
    mediaOpenRequest1->mediaRequestInfo.trackId = 100;
    mediaOpenRequest1->mediaRequestInfo.playbackSessionId = "sakjfsakfjh";
    mediaOpenRequest1->mediaRequestInfo.streamHandler = [=](const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        if(mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO)
        {
            const Wisenet::Media::VideoSourceFrameSharedPtr& videoFrame = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaSourceFrame);
            std::string videoTime = Wisenet::Common::utcMsecsToUtcIsoString(videoFrame->frameTime.ptsTimestampMsec);

            if (videoFrame->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME) {
                SPDLOG_DEBUG("[VIDEO1]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                             videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                             videoFrame->videoFrameType, videoTime, videoFrame->frameTime.rtpTimestampMsec);
            }
            else {
                SPDLOG_TRACE("[VIDEO1]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                             videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                             videoFrame->videoFrameType, videoTime, videoFrame->frameTime.rtpTimestampMsec);
            }
        }
    };

    InitializeNotify();
    m_deviceClient->DeviceMediaOpen(mediaOpenRequest1,
                                    [=](const ResponseBaseSharedPtr& response)
    {
        SPDLOG_DEBUG("[VIDEO] MediaOpen1 Response : {}", response->errorCode);
        if(response->isFailed())
            Notify(false);
        else
            Notify(true);
    });
    QVERIFY2(Wait(600), "[VIDEO] MediaOpen1 Request failed");

    // Receive video1 for 10 sec.
    uint waitSec = 10;
    while(waitSec > 0)
    {
        waitSec--;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

 /*
    // Open Video2
    SPDLOG_DEBUG("[VIDEO] Open Video2");
    DeviceMediaOpenRequestSharedPtr mediaOpenRequest2 = std::make_shared<DeviceMediaOpenRequest>();
    mediaOpenRequest2->mediaUUID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();
    mediaOpenRequest2->uuidInfo.channel = 2;
    mediaOpenRequest2->mediaRequestInfo.streamType = StreamType::playbackHigh;
    mediaOpenRequest2->mediaRequestInfo.profileId = 2;
    mediaOpenRequest2->mediaRequestInfo.streamHandler = [=](const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        if(mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO)
        {
            const Wisenet::Media::VideoSourceFrameSharedPtr& videoFrame = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaSourceFrame);

            if (videoFrame->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME) {
                SPDLOG_DEBUG("[VIDEO2]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                             videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                             videoFrame->videoFrameType, videoFrame->frameTime.ptsTimestampMsec, videoFrame->frameTime.rtpTimestampMsec);
            }
            else {
                SPDLOG_TRACE("[VIDEO2]  size={}, width={}, height={}, type={}, ptsMsec={}, timestampMsec={}",
                             videoFrame->getDataSize(), videoFrame->videoWidth, videoFrame->videoHeight,
                             videoFrame->videoFrameType, videoFrame->frameTime.ptsTimestampMsec, videoFrame->frameTime.rtpTimestampMsec);
            }
        }
    };

    InitializeNotify();
    m_deviceClient->DeviceMediaOpen(m_streamIoContextPool.ioContext(),
                                    mediaOpenRequest2,
                                    [=](const ResponseBaseSharedPtr& response)
    {
        SPDLOG_DEBUG("[VIDEO] MediaOpen2 Response : {}", response->errorCode);
        if(response->isFailed())
            Notify(false);
        else
            Notify(true);
    });
    QVERIFY2(Wait(600), "[VIDEO] MediaOpen2 Request failed");

    // Receive video1&2 for 10 sec.
    waitSec = 10;
    while(waitSec > 0)
    {
        waitSec--;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
*/

    // Close Video1
    SPDLOG_DEBUG("[VIDEO] Close Video1");
    DeviceMediaCloseRequestSharedPtr mediaCloseRequest1 = std::make_shared<DeviceMediaCloseRequest>();
    mediaCloseRequest1->mediaID = mediaOpenRequest1->mediaID;
    m_deviceClient->DeviceMediaClose(mediaCloseRequest1, nullptr);

/*
    // Receive video2 for 10 sec.
    waitSec = 10;
    while(waitSec > 0)
    {
        waitSec--;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    // Close Video2
    SPDLOG_DEBUG("[VIDEO] Close Video2");
    DeviceMediaCloseRequestSharedPtr mediaCloseRequest2 = std::make_shared<DeviceMediaCloseRequest>();
    mediaCloseRequest2->mediaUUID = mediaOpenRequest2->mediaUUID;
    m_deviceClient->DeviceMediaClose(mediaCloseRequest2);
*/
    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));


    /* DeviceDisconnect */
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

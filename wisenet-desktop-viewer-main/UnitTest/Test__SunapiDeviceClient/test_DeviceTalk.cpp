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

void Test__SunapiDeviceClient::test_DeviceTalk()
{
    SPDLOG_DEBUG("test_DeviceTalk");


    /* DeviceConnect */
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));



    // Talk Start
    SPDLOG_DEBUG("Request Talk Start");

    DeviceTalkRequestSharedPtr talkRequest = std::make_shared<DeviceTalkRequest>();
    talkRequest->talkStart = true;
    talkRequest->channelID = "1";

    InitializeNotify();
    m_deviceClient->DeviceTalk( talkRequest,
                                [=](const ResponseBaseSharedPtr& response)
    {
        SPDLOG_DEBUG("Talk Start Response : {}", response->errorCode);
        if(response->isFailed())
            Notify(false);
        else
            Notify(true);
    });
    QVERIFY2(Wait(600), "Talk Start Request failed");

    // Send audio sample data
    std::ifstream fin(m_audioSamplePath, std::ios::in | std::ios::binary);
    int count = 0;
    if (fin.is_open())
    {
        unsigned char * data = new unsigned char[2000];
        while (1)
        {
            fin.read((char*)data, 800);

            count++;
            if (count == 2000)
                break;

            if (!fin)
                break;

            Wisenet::Media::AudioSourceFrameSharedPtr audioFrame = std::make_shared<Wisenet::Media::AudioSourceFrame>(data, 800);

            DeviceSendTalkDataRequestSharedPtr sendAudioRequest = std::make_shared<DeviceSendTalkDataRequest>();
            sendAudioRequest->channelID = "1";
            sendAudioRequest->audioFrame = audioFrame;

            m_deviceClient->DeviceSendTalkData(sendAudioRequest, nullptr);

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        delete[] data;
    }

    // Talk Stop
    SPDLOG_DEBUG("Request Talk Stop");

    DeviceTalkRequestSharedPtr stopReauest = std::make_shared<DeviceTalkRequest>();
    stopReauest->talkStart = false;
    stopReauest->channelID = "1";

    InitializeNotify();
    m_deviceClient->DeviceTalk( stopReauest,
                                [=](const ResponseBaseSharedPtr& response)
    {
        SPDLOG_DEBUG("Talk Stop Response : {}", response->errorCode);
        if(response->isFailed())
            Notify(false);
        else
            Notify(true);
    });
    QVERIFY2(Wait(600), "Talk Stop Request failed");

    // wait 5seconds
    std::this_thread::sleep_for(std::chrono::seconds(5));



    /* DeviceDisconnect */
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

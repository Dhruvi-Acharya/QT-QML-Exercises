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

void Test__SunapiDeviceClient::test_OnlyRecordingTimeline()
{
    SPDLOG_DEBUG("test_RecordingTimeline");

    /* DeviceConnect */
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    std::string strFromDate = "2021-10-03T11:24:42Z";
    std::string strToDate = "2022-05-18T04:35:17Z";

    /* Timeline */
    InitializeNotify();

    DeviceGetTimelineRequestSharedPtr timelineRequest = std::make_shared<DeviceGetTimelineRequest>();

    timelineRequest->param.FromDate = Wisenet::Common::utcMsecsFromIsoString(strFromDate);
    timelineRequest->param.ToDate = Wisenet::Common::utcMsecsFromIsoString(strToDate);
    timelineRequest->param.OverlappedID = 100;
    timelineRequest->param.ChannelIDList.push_back("15");
    timelineRequest->param.ChannelIDList.push_back("16");
    //timelineRequest->param.PrimaryDeviceIPAddress = "192.168.1.1";

    m_deviceClient->DeviceGetTimeline(timelineRequest,
                                      [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            auto timelineResponse = std::static_pointer_cast<DeviceGetTimelineResponse>(response);

            /*for(auto& channelMap : timelineResponse->result.channelTimelineList)
            {
                for(auto& timelineMap : channelMap.second)
                {
                    for(auto& timeline : timelineMap.second)
                    {
                        SPDLOG_DEBUG("test_RecordingTimeline::GetTimeline : Channel({}) OverlappedId({}) Type({}) Start({}) End({})",
                                     channelMap.first, timelineMap.first, timeline.type,
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.startTime),
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.endTime));
                    }
                }
            }*/

            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "Timeline Request failed");

    /* Device Disconnect */
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}


void Test__SunapiDeviceClient::test_RecordingTimeline()
{
    SPDLOG_DEBUG("test_RecordingTimeline");

    /* DeviceConnect */
    QVERIFY2(connectInternal(), "Can not connect to the device..");


    /* Overlapped Recording */
    InitializeNotify();

    DeviceGetOverlappedIdRequestSharedPtr overlappedIdRequest = std::make_shared<DeviceGetOverlappedIdRequest>();
    overlappedIdRequest->param.ChannelIDList.push_back("15");
    overlappedIdRequest->param.ChannelIDList.push_back("16");

    std::string strFromDate = "2021-10-03T11:24:42Z";
    std::string strToDate = "2022-05-18T04:35:17Z";
    overlappedIdRequest->param.FromDate = Wisenet::Common::utcMsecsFromIsoString(strFromDate);
    overlappedIdRequest->param.ToDate = Wisenet::Common::utcMsecsFromIsoString(strToDate);
    m_deviceClient->DeviceGetOverlappedId(overlappedIdRequest,
                                          [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            std::string overlappedIdList = "";
            auto overlappedIdResponse = std::static_pointer_cast<DeviceGetOverlappedIdResponse>(response);
            for(auto id : overlappedIdResponse->result.overlappedIDList)
            {
                overlappedIdList += std::to_string(id) + ",";
            }
            SPDLOG_DEBUG("test_RecordingTimeline::Overlapped Recording : OverlappedIdList={}", overlappedIdList);
            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "Get OverlappedId Request failed");


    /* Calendar Search */
    InitializeNotify();

    DeviceCalendarSearchRequestSharedPtr caneldarsearchRequest = std::make_shared<DeviceCalendarSearchRequest>();
    caneldarsearchRequest->param.ChannelIDList.push_back("15");
    caneldarsearchRequest->param.ChannelIDList.push_back("16");
    //caneldarsearchRequest->param.IgnoreChannelBasedResults = true;
    std::string strMonth = "2021-03-01T00:00:00Z";
    caneldarsearchRequest->param.Month = Wisenet::Common::utcMsecsFromIsoString(strMonth);

    m_deviceClient->DeviceCalendarSearch(caneldarsearchRequest,
                                         [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            auto caneldarsearchResponse = std::static_pointer_cast<DeviceCalendarSearchResponse>(response);

            std::string allChannelResult = "All Channel Result : ";
            for(bool recordExist : caneldarsearchResponse->result.result.dayRecordExist)
            {
                if(recordExist)
                    allChannelResult += "1";
                else
                    allChannelResult += "0";
            }
            SPDLOG_DEBUG("test_RecordingTimeline::Calendar Search : {}", allChannelResult);

            for(auto kvp : caneldarsearchResponse->result.channelReslut)
            {
                std::string oneChannelResult = "";
                for(bool recordExist : kvp.second.dayRecordExist)
                {
                    if(recordExist)
                        oneChannelResult += "1";
                    else
                        oneChannelResult += "0";
                }
                SPDLOG_DEBUG("test_RecordingTimeline::Calendar Search : Channel {} Result : {}", kvp.first, oneChannelResult);
            }

            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "Calendarsearch Request failed");


    /* Timeline */
    InitializeNotify();

    DeviceGetTimelineRequestSharedPtr timelineRequest = std::make_shared<DeviceGetTimelineRequest>();

    timelineRequest->param.FromDate = Wisenet::Common::utcMsecsFromIsoString(strFromDate);
    timelineRequest->param.ToDate = Wisenet::Common::utcMsecsFromIsoString(strToDate);
    timelineRequest->param.OverlappedID = 100;
    timelineRequest->param.ChannelIDList.push_back("15");
    timelineRequest->param.ChannelIDList.push_back("16");
    //timelineRequest->param.PrimaryDeviceIPAddress = "192.168.1.1";

    m_deviceClient->DeviceGetTimeline(timelineRequest,
                                      [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            auto timelineResponse = std::static_pointer_cast<DeviceGetTimelineResponse>(response);

            for(auto& channelMap : timelineResponse->result.channelTimelineList)
            {
                for(auto& timelineMap : channelMap.second)
                {
                    for(auto& timeline : timelineMap.second)
                    {
                        SPDLOG_DEBUG("test_RecordingTimeline::GetTimeline : Channel({}) OverlappedId({}) Type({}) Start({}) End({})",
                                     channelMap.first, timelineMap.first, timeline.type,
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.startTime),
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.endTime));
                    }
                }
            }

            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "Timeline Request failed");


    /* Recording Period */
    InitializeNotify();

    DeviceGetRecordingPeriodRequestSharedPtr recordingPeriodRequest = std::make_shared<DeviceGetRecordingPeriodRequest>();

    m_deviceClient->DeviceGetRecordingPeriod(recordingPeriodRequest,
                                             [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            auto recordingPeriodResponse = std::static_pointer_cast<DeviceGetRecordingPeriodResponse>(response);
            SPDLOG_DEBUG("test_RecordingTimeline::GetRecordingPeriod : StartTime({}) EndTime({})",
                         Wisenet::Common::utcMsecsToUtcIsoString(recordingPeriodResponse->result.startTime),
                         Wisenet::Common::utcMsecsToUtcIsoString(recordingPeriodResponse->result.endTime));

            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "RecordingPeriod Request failed");


    /* All Period Timeline */
    InitializeNotify();

    DeviceGetAllPeriodTimelineRequestSharedPtr allPeriodTimelineRequest = std::make_shared<DeviceGetAllPeriodTimelineRequest>();
    allPeriodTimelineRequest->ChannelIDList.push_back("1");
    allPeriodTimelineRequest->ChannelIDList.push_back("2");
    allPeriodTimelineRequest->ChannelIDList.push_back("3");
    allPeriodTimelineRequest->ChannelIDList.push_back("4");
    allPeriodTimelineRequest->ChannelIDList.push_back("5");

    m_deviceClient->DeviceGetAllPeriodTimeline(allPeriodTimelineRequest,
                                               [=](const ResponseBaseSharedPtr& response)
    {
        if(response->isFailed())
        {
            Notify(false);
        }
        else
        {
            auto allPeriodTimelineResponse = std::static_pointer_cast<DeviceGetAllPeriodTimelineResponse>(response);
            SPDLOG_DEBUG("test_RecordingTimeline::GetAllPeriodTimeline : StartTime({}) EndTime({})",
                         Wisenet::Common::utcMsecsToUtcIsoString(allPeriodTimelineResponse->recordingPeriod.startTime),
                         Wisenet::Common::utcMsecsToUtcIsoString(allPeriodTimelineResponse->recordingPeriod.endTime));

            int count = 0;
            for(auto& channelMap : allPeriodTimelineResponse->timeline.channelTimelineList)
            {
                for(auto& timelineMap : channelMap.second)
                {
                    for(auto& timeline : timelineMap.second)
                    {
                        SPDLOG_DEBUG("test_RecordingTimeline::GetAllPeriodTimeline : Channel({}) OverlappedId({}) Type({}) Start({}) End({})",
                                     channelMap.first, timelineMap.first, timeline.type,
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.startTime),
                                     Wisenet::Common::utcMsecsToUtcIsoString(timeline.endTime));
                        count++;
                    }
                }
            }
            SPDLOG_DEBUG("test_RecordingTimeline::GetAllPeriodTimeline : Total Count({})", count);

            Notify(true);
        }
    });

    QVERIFY2(Wait(600), "AllPeriodTimeline Request failed");


    /* Device Disconnect */
    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

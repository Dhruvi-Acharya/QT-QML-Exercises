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

void Test__SunapiDeviceClient::test_SmartSearch()
{
    SPDLOG_INFO("test_SmartSearch");
    QVERIFY2(connectInternal(), "Can not connect to the device..");

    //set alarmOutput on
    auto searchRequest = std::make_shared<DeviceSmartSearchRequest>();
    std::string strStartTime = "2022-05-30T12:50:14Z";
    std::string strEndTime = "2022-05-30T13:03:14Z";

    searchRequest->fromDate = Wisenet::Common::utcMsecsFromIsoString(strStartTime);
    searchRequest->toDate = Wisenet::Common::utcMsecsFromIsoString(strEndTime);
    searchRequest->OverlappedID = 100;
    searchRequest->channel = "10";

    SmartSearchArea area;

    area.index = 1;
    area.areaType = SmartSearchAreaType::inside;
    area.eventType.push_back(SmartSearchAreaEventType::motion);
    area.eventType.push_back(SmartSearchAreaEventType::enter);
    area.eventType.push_back(SmartSearchAreaEventType::exit);

    area.coordinates.push_back(-0.903226f);
    area.coordinates.push_back(0.870504f);
    area.coordinates.push_back(-0.903226f);
    area.coordinates.push_back(-0.877698f);
    area.coordinates.push_back(0.903226f);
    area.coordinates.push_back(-0.877698f);
    area.coordinates.push_back(0.903226f);
    area.coordinates.push_back(0.870504f);

    searchRequest->areas.push_back(area);

    InitializeNotify();


    m_deviceClient->DeviceSmartSearch(searchRequest,[this] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceSmartSearch() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceSmartSearch() success");
            SPDLOG_INFO("=======================================");
            auto smartResponse = std::static_pointer_cast<DeviceSmartSearchResponse>(response);
            SPDLOG_DEBUG("DeviceSmartSearch() totalCount={}", smartResponse->totalCount);


            for(auto & result : smartResponse->results){
                SPDLOG_DEBUG("DeviceSmartSearch() index={} time={} type={}",
                             result.index, result.dateTime, result.type);
            }
            SPDLOG_INFO("=======================================");
            SPDLOG_INFO("DeviceSmartSearch() success end");
            if(smartResponse->isContinue == false)
                Notify(true);
        }
    });

    Wait(120);
    SPDLOG_INFO("=======================================");
    auto deviceRequest = std::make_shared<DeviceRequestBase>();
    m_deviceClient->DeviceSmartSearchCancel(deviceRequest,[this] (const ResponseBaseSharedPtr& response)
    {
         SPDLOG_INFO("DeviceSmartSearchCancel() ={}", response->errorCode);
    });

    QVERIFY2(Wait(60), "DeviceSmartSearch() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

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

void Test__SunapiDeviceClient::test_OcrSearch()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    SPDLOG_INFO("test_OcrSearch");

    //set alarmOutput on
    auto searchRequest = std::make_shared<DeviceMetaAttributeSearchRequest>();
    std::string strStartTime = "2022-02-01T21:03:14Z";
    std::string strEndTime = "2022-02-20T21:03:14Z";
    searchRequest->classType = AiClassType::ocr;
    searchRequest->fromDate = Wisenet::Common::utcMsecsFromIsoString(strStartTime);
    searchRequest->toDate = Wisenet::Common::utcMsecsFromIsoString(strEndTime);

    for(int i = 0; i < 64 ; i++)
        searchRequest->channelIDList.push_back(std::to_string(i+1));

    InitializeNotify();

   auto urlRequest = std::make_shared<DeviceUrlSnapShotRequest>();

    m_deviceClient->DeviceOcrSearch(searchRequest,[this, &urlRequest] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceOcrSearch() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceOcrSearch() success");
            SPDLOG_INFO("=======================================");
            auto ocrResponse = std::static_pointer_cast<DeviceMetaAttributeSearchResponse>(response);

            SPDLOG_DEBUG("DeviceOcrSearch() totalCount={}", ocrResponse->totalCount);


            for(auto & result : ocrResponse->results){
                SPDLOG_DEBUG("DeviceOcrSearch() index={} url={}", result.aiBaseAttribute.index, result.aiBaseAttribute.imageUrl);

                urlRequest->urlList.push_back(result.aiBaseAttribute.imageUrl);
            }

            SPDLOG_INFO("=======================================");
            SPDLOG_INFO("DeviceOcrSearch() success end");
            if(ocrResponse->isContinue == false)
                Notify(true);
        }
    });
    QVERIFY2(Wait(5), "DeviceOcrSearch() failed");


    for(auto &item : urlRequest->urlList){
        SPDLOG_DEBUG("DeviceUrlSnapShot() url = {} ", item);
    }

    InitializeNotify();

    m_deviceClient->DeviceUrlSnapShot(urlRequest,[this] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceUrlSnapShot() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceUrlSnapShot() success");
            auto snapResponse = std::static_pointer_cast<DeviceUrlSnapShotResponse>(response);

            SPDLOG_INFO("DeviceUrlSnapShot() success size={}", snapResponse->snapShots.size());

            int i = 0;
            for(auto &item : snapResponse->snapShots){
                std::string fileName = + "./snapShot" + std::to_string(i++) + ".jpg";
                std::ofstream outputfile(fileName, std::ostream::binary | std::ostream::app);
                std::copy(item.snapshotData.begin(),
                           item.snapshotData.end(),
                           std::ostreambuf_iterator<char>(outputfile));
                 outputfile.close();

            }

            Notify(true);
        }
    });

    QVERIFY2(Wait(5), "DeviceUrlSnapShot() failed");

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // wait 2seconds
    std::this_thread::sleep_for(std::chrono::seconds(2));

    QVERIFY2(disconnectInternal(), "Can not disconnect to the device..");
}

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

void Test__SunapiDeviceClient::test_MetaAttributeSearch()
{
    QVERIFY2(connectInternal(), "Can not connect to the device..");
    SPDLOG_INFO("test_MetaAttributeSearch");

    //set alarmOutput on
    auto searchRequest = std::make_shared<DeviceMetaAttributeSearchRequest>();
    std::string strStartTime = "2022-02-24T09:50:14Z";
    std::string strEndTime = "2022-02-24T12:03:14Z";
    searchRequest->fromDate = Wisenet::Common::utcMsecsFromIsoString(strStartTime);
    searchRequest->toDate = Wisenet::Common::utcMsecsFromIsoString(strEndTime);

    for(int i = 0; i < 64 ; i++)
        searchRequest->channelIDList.push_back(std::to_string(i+1));

    //searchRequest->classType = AiClassType::person;
    searchRequest->classType = AiClassType::person;

    InitializeNotify();

    auto urlRequest = std::make_shared<DeviceUrlSnapShotRequest>();

    m_deviceClient->DeviceMetaAttributeSearch(searchRequest,[this, &urlRequest] (const ResponseBaseSharedPtr& response)
    {
        if (response->isFailed()) {
            SPDLOG_WARN("DeviceMetaAttributeSearch() failed, error_string={}", response->errorString());
            Notify(false);
        }
        else {
            SPDLOG_INFO("DeviceMetaAttributeSearch() success");
            SPDLOG_INFO("=======================================");
            auto metaResponse = std::static_pointer_cast<DeviceMetaAttributeSearchResponse>(response);
            SPDLOG_DEBUG("DeviceMetaAttributeSearch() totalCount={}", metaResponse->totalCount);


            for(auto & result : metaResponse->results){
                SPDLOG_DEBUG("DeviceMetaAttributeSearch() index={} url={}", result.aiBaseAttribute.index, result.aiBaseAttribute.imageUrl);

                urlRequest->urlList.push_back(result.aiBaseAttribute.imageUrl);
#pragma region personAttribute
                for(auto& gender : result.personGender){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() personGender={} ", gender);
                }

                for(auto& color : result.personClothingTopsColor){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() personClothingTopsColor={} ", color);
                }
                for(auto& color : result.personClothingBottomsColor){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() personClothingBottomsColor={} ", color);
                }

                for(auto& belonging : result.personBelongingBag){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() belonging={} ", belonging);
                }
#pragma endregion
#pragma region faceAttribute
                for(auto& gender : result.faceGender){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() faceGender={} ", gender);
                }
                for(auto& ageType : result.faceAgeType){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() faceAgeType={} ", ageType);
                }
                for(auto& hat : result.faceHat){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() faceHat={} ", hat);
                }
                for(auto& optical : result.faceOpticals){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() faceOpticals={} ", optical);
                }
                for(auto& mask : result.faceMask){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() faceMask={} ", mask);
                }
#pragma endregion
#pragma region vehicleAttribute
                for(auto& type : result.vehicleType){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() vehicleType={} ", type);
                }
                for(auto& color : result.vehicleColor){
                   SPDLOG_DEBUG("DeviceMetaAttributeSearch() vehicleColor={} ", color);
                }
#pragma endregion
            }

            SPDLOG_INFO("=======================================");
            SPDLOG_INFO("DeviceMetaAttributeSearch() success end");
            if(metaResponse->isContinue == false)
                Notify(true);
        }
    });

    Wait(1);
    SPDLOG_INFO("=======================================");
    auto deviceRequest = std::make_shared<DeviceRequestBase>();
    m_deviceClient->DeviceAiSearchCancel(deviceRequest,[this] (const ResponseBaseSharedPtr& response)
    {
         SPDLOG_INFO("DeviceMetaAttributeSearchCancel() ={}", response->errorCode);
    });

    QVERIFY2(Wait(10), "DeviceMetaAttributeSearch() failed");

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

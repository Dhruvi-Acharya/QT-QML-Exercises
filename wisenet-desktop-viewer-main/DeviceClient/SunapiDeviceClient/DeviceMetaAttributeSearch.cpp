/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/

#include "SunapiDeviceClientImpl.h"

namespace Wisenet
{
namespace Device
{

class AiMetaAttributeSearchControl : public BaseCommand, public JsonParser
{
public:
    explicit AiMetaAttributeSearchControl(const std::string& logPrefix,
                                          DeviceMetaAttributeSearchRequestSharedPtr request)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceMetaAttributeSearchResponse>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ai","metaattributesearch", "control");
        builder.AddParameter("Mode","Start");
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->fromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->toDate));

        if(m_request->channelIDList.size() > 0)
        {
            std::string channelIdList = "";
            for(auto channelId : m_request->channelIDList)
            {
                channelIdList += std::to_string(toSunapiChannel(channelId)) + ",";
            }
            channelIdList.erase(channelIdList.size()-1);
            builder.AddParameter("ChannelIDList", channelIdList);
        }

        std::string classType;

        if(m_request->classType == AiClassType::person)
            classType = "Person";
        else if(m_request->classType == AiClassType::face)
            classType = "Face";
        else if(m_request->classType == AiClassType::vehicle)
            classType = "Vehicle";

        builder.AddParameter("ClassType",classType);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {

        auto response = std::static_pointer_cast<DeviceMetaAttributeSearchResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("SearchToken");
        if (itr != jsonDoc.MemberEnd())
            m_searchToken = itr->value.GetString();
        itr = jsonDoc.FindMember("TotalCount");
        if (itr != jsonDoc.MemberEnd())
           m_totalCount = itr->value.GetInt();

        return true;
    }
    DeviceMetaAttributeSearchRequestSharedPtr m_request;

public:
    std::string m_searchToken;
    int m_totalCount = 0;
};

class AiMetaAttributeSearchView : public BaseCommand, public JsonParser
{
public:
    explicit AiMetaAttributeSearchView(const std::string& logPrefix,
                                       DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                       std::string searchToken, bool isResult, int resultIndex, int maxResults, uuid_string deviceId)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceMetaAttributeSearchResponse>(),false,false),
            m_finalResponse(finalResponse),
            m_searchToken(searchToken),
            m_isResult(isResult),
            m_resultIndex(resultIndex),
            m_maxResults(maxResults),
            m_deviceId(deviceId)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ai","metaattributesearch", "view");
        builder.AddParameter("Mode","Start");
        builder.AddParameter("Type", m_isResult?"Results":"Status");
        builder.AddParameter("SearchToken", m_searchToken);
        if(m_isResult){
            builder.AddParameter("ResultFromIndex", m_resultIndex);
            builder.AddParameter("MaxResults", m_maxResults);
        }

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        auto response = std::static_pointer_cast<DeviceMetaAttributeSearchResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("Status");
        if (itr != jsonDoc.MemberEnd()){
            std::string status = itr->value.GetString();
            if(status == "Completed")
                m_isComplete = true;
        }
        SLOG_DEBUG("AiMetaAttributeSearchView(), m_isResult={} ", m_isResult);
        if(m_isResult){
            itr = jsonDoc.FindMember("Results");
            if (itr != jsonDoc.MemberEnd()){
               auto & results = itr->value;
               for(auto & result : results.GetArray()){
                   MetaAttribute metaAttribute;

                   metaAttribute.aiBaseAttribute.deviceID = m_deviceId;
                   metaAttribute.aiBaseAttribute.index = m_resultIndex + m_finalResponse->results.size();
                   auto itrResult = result.FindMember("DateTime");
                   if(itrResult != result.MemberEnd())
                       metaAttribute.aiBaseAttribute.dateTime = Common::utcMsecsFromIsoString(itrResult->value.GetString());
                   itrResult = result.FindMember("Channel");
                   if(itrResult != result.MemberEnd())
                       metaAttribute.aiBaseAttribute.channelId = itrResult->value.GetInt() +1;

                   //SLOG_DEBUG("AiMetaAttributeSearchView(), index={} channel={}",
                    //          metaAttribute.index, metaAttribute.channelId);

                   itrResult = result.FindMember("Attributes");
                   if(itrResult != result.MemberEnd()){
                       auto & attribute = itrResult->value;

                       auto itrPerson = attribute.FindMember("Person");
                       auto itrFace = attribute.FindMember("Face");
                       auto itrVehicle = attribute.FindMember("Vehicle");

#pragma region personAttribute
                       if(itrPerson != attribute.MemberEnd()){
                           metaAttribute.attributeType = AiClassType::person;
                           auto & person = itrPerson->value;
                           auto itrGender = person.FindMember("Gender");
                           if(itrGender != person.MemberEnd()){
                               for(auto & gender : itrGender->value.GetArray()){
                                   metaAttribute.personGender.push_back(gender.GetString());
                               }
                           }
                            auto itrClothing = person.FindMember("Clothing");
                            if(itrClothing != person.MemberEnd()){
                                auto & clotihing = itrClothing->value;
                                auto itrTops = clotihing.FindMember("Tops");
                                if(itrTops != clotihing.MemberEnd()){
                                    auto & tops = itrTops->value;
                                    auto itrColorString = tops.FindMember("ColorString");
                                    if(itrColorString != tops.MemberEnd()){
                                        for(auto & color : itrColorString->value.GetArray()){
                                            metaAttribute.personClothingTopsColor.push_back(color.GetString());
                                        }
                                    }
                                }
                                auto itrBottoms = clotihing.FindMember("Bottoms");
                                if(itrBottoms != clotihing.MemberEnd()){
                                    auto & bottoms = itrBottoms->value;
                                    auto itrColorString = bottoms.FindMember("ColorString");
                                    if(itrColorString != bottoms.MemberEnd()){
                                        for(auto & color : itrColorString->value.GetArray()){
                                            metaAttribute.personClothingBottomsColor.push_back(color.GetString());
                                        }
                                    }
                                }
                            }
                            auto itrBelonging = person.FindMember("Belonging");
                            if(itrBelonging != person.MemberEnd()){
                                auto & belonging = itrBelonging->value;
                                auto itrBag = belonging.FindMember("Bag");
                                if(itrBag != belonging.MemberEnd()){
                                    for(auto & bag : itrBag->value.GetArray()){
                                        metaAttribute.personBelongingBag.push_back(bag.GetString());
                                    }
                                }
                            }
                       }
#pragma endregion
#pragma region faceAttribute
                       if(itrFace != attribute.MemberEnd()){
                           metaAttribute.attributeType = AiClassType::face;
                           auto & face = itrFace->value;
                           auto itrGender = face.FindMember("Gender");
                           if(itrGender != face.MemberEnd()){
                               for(auto & gender : itrGender->value.GetArray()){
                                   metaAttribute.faceGender.push_back(gender.GetString());
                               }
                           }
                           auto itrAgeType = face.FindMember("AgeType");
                           if(itrAgeType != face.MemberEnd()){
                               for(auto & ageType : itrAgeType->value.GetArray()){
                                   metaAttribute.faceAgeType.push_back(ageType.GetString());
                               }
                           }
                           auto itrHat = face.FindMember("Hat");
                           if(itrHat != face.MemberEnd()){
                               for(auto & hat : itrHat->value.GetArray()){
                                   metaAttribute.faceHat.push_back(hat.GetString());
                               }
                           }
                           auto itrOpticals = face.FindMember("Opticals");
                           if(itrOpticals != face.MemberEnd()){
                               for(auto & opticals : itrOpticals->value.GetArray()){
                                   metaAttribute.faceOpticals.push_back(opticals.GetString());
                               }
                           }
                           auto itrMask = face.FindMember("Mask");
                           if(itrMask != face.MemberEnd()){
                               for(auto & mask : itrMask->value.GetArray()){
                                   metaAttribute.faceMask.push_back(mask.GetString());
                               }
                           }
                       }
#pragma endregion
#pragma region vehicleAttribute
                       if(itrVehicle != attribute.MemberEnd()){
                           metaAttribute.attributeType = AiClassType::vehicle;
                           auto & vehicle = itrVehicle->value;
                           auto itrType = vehicle.FindMember("Type");
                           if(itrType != vehicle.MemberEnd()){
                               for(auto & type : itrType->value.GetArray()){
                                   metaAttribute.vehicleType.push_back(type.GetString());
                               }
                           }
                           auto itrColorString = vehicle.FindMember("ColorString");
                           if(itrColorString != vehicle.MemberEnd()){
                               for(auto & colorString : itrColorString->value.GetArray()){
                                   metaAttribute.vehicleColor.push_back(colorString.GetString());
                               }
                           }
                       }
#pragma endregion

                   }


                   itrResult = result.FindMember("ImageURL");
                   if(itrResult != result.MemberEnd())
                       metaAttribute.aiBaseAttribute.imageUrl = itrResult->value.GetString();

                   SLOG_DEBUG("AiMetaAttributeSearchView(), url={}",
                               metaAttribute.aiBaseAttribute.imageUrl);

                   itrResult = result.FindMember("Resolution");
                   if(itrResult != result.MemberEnd()){
                       std::string resolution = itrResult->value.GetString();
                       std::vector<std::string> tokens;
                       boost::split(tokens, resolution, boost::is_any_of("x"));

                       if (!try_stoi(tokens.at(0), &metaAttribute.aiBaseAttribute.imageWidth))
                           continue;
                       if (!try_stoi(tokens.at(1), &metaAttribute.aiBaseAttribute.imageHeight))
                           continue;
                   }

                   itrResult = result.FindMember("ObjectID");
                   if(itrResult != result.MemberEnd())
                       metaAttribute.objectId = itrResult->value.GetInt();

                   m_finalResponse->results.push_back(metaAttribute);
               }
            }
        }
        return true;
    }

public:
    DeviceMetaAttributeSearchResponseSharedPtr m_finalResponse;
    bool m_isComplete = false;
    std::string m_searchToken;
    bool m_isResult = false;
    int m_resultIndex = 0;
    int m_maxResults = 0;
    uuid_string m_deviceId;
};

void SunapiDeviceClientImpl::StartAiSearchStatusTimer(std::string searchToken,
                                                      DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                                      ResponseBaseHandler const& responseHandler, long long durationMsec)
{
    m_aiSearchStatusTimer.cancel();
    m_aiSearchStatusTimer.expires_after(std::chrono::milliseconds(durationMsec));
    m_aiSearchStatusTimer.async_wait( WeakCallback(shared_from_this(),
                                              [this, searchToken, finalResponse, responseHandler](const boost::system::error_code& ec) {
        if (ec) {
            m_runningAiSearch = false;
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, searchToken, finalResponse, responseHandler]() {
            SLOG_DEBUG("StartAiSearchStatusTimer");
            metaAttributeSearchViewStatus(searchToken, finalResponse, responseHandler);
        }));
    }));
}

void SunapiDeviceClientImpl::StopAiSearchStatusTimer()
{
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_aiSearchStatusTimer.cancel();
    }));
}

void SunapiDeviceClientImpl::DeviceAiSearchCancel( DeviceRequestBaseSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler )
{
     asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
     {
         auto response = std::make_shared<ResponseBase>();
         response->errorCode = Wisenet::ErrorCode::NoError;
         m_runningAiSearch = false;
         responseHandler(response);
     }));
}

void SunapiDeviceClientImpl::DeviceMetaAttributeSearch( DeviceMetaAttributeSearchRequestSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler )
{
    m_runningAiSearch = true;
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        SLOG_DEBUG("DeviceMetaAttributeSearch(), fromDate={} toDate={}  classType={}",
                   request->fromDate, request->toDate,  (int)request->classType);

        if(request->channelIDList.size() == 0 ||
                request->fromDate == 0 ||
                request->toDate == 0){
            SLOG_DEBUG("DeviceMetaAttributeSearch() channelSize={} from={} to={}",
                       request->channelIDList.size(),
                       request->fromDate, request->toDate);
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            m_runningAiSearch = false;
            responseHandler(response);
            return;
        }

        auto metaAttributeSearchControl = std::make_shared<AiMetaAttributeSearchControl>(
                    m_logPrefix,
                    request);


        asyncRequest(m_httpSearchMetaDataSession,
                     metaAttributeSearchControl,
                     [this, request, metaAttributeSearchControl, responseHandler](const ResponseBaseSharedPtr& response)

        {
            auto finalResponse = std::make_shared<DeviceMetaAttributeSearchResponse>();
            if(response->isSuccess()){
                SLOG_DEBUG("DeviceMetaAttributeSearch() AiMetaAttributeSearchControl isSuccess totalCount={} ",
                           metaAttributeSearchControl->m_totalCount);
                if(metaAttributeSearchControl->m_totalCount == 0){
                    finalResponse->totalCount = 0;
                    finalResponse->errorCode = Wisenet::ErrorCode::NoError;
                     m_runningAiSearch = false;
                    responseHandler(finalResponse);
                    return;
                }

                finalResponse->totalCount = metaAttributeSearchControl->m_totalCount;

                SLOG_DEBUG("DeviceMetaAttributeSearch() AiMetaAttributeSearchControl isSuccess 1 totalCount={} ",
                           metaAttributeSearchControl->m_totalCount);

                metaAttributeSearchViewStatus(metaAttributeSearchControl->m_searchToken, finalResponse, responseHandler);


            }
            else{
                m_runningAiSearch = false;
                responseHandler(finalResponse);
            }

        }, nullptr, AsyncRequestType::HTTPGET,false);

    }));
}

void SunapiDeviceClientImpl::metaAttributeSearchViewStatus(std::string searchToken,
                                                     DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                                     ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("metaAttributeSearchViewStatus() seachToken={}",
               searchToken);
    if(!m_runningAiSearch) return;

    auto metaAttributeSearch = std::make_shared<AiMetaAttributeSearchView>(
                m_logPrefix, finalResponse, searchToken, false, 1, 100, m_deviceUUID);

    asyncRequest(m_httpSearchMetaDataSession,
                 metaAttributeSearch,
                 [this, metaAttributeSearch, searchToken, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("metaAttributeSearchViewStatus() isSuccess isResult={} ",
                       metaAttributeSearch->m_isComplete);
            if(metaAttributeSearch->m_isResult == false && metaAttributeSearch->m_isComplete == false){
               StartAiSearchStatusTimer(searchToken, finalResponse, responseHandler);
            }
            else{
               int maxResult = m_repos->attributes().aiCgi.metaAttributeSearchMaxResults;
               if(maxResult > 100)
                   maxResult = 100;

               metaAttributeSearchViewResult(searchToken, 1, maxResult, finalResponse, responseHandler);
            }
        }
        else{
            m_runningAiSearch = false;
            responseHandler(finalResponse);
        }
    }, nullptr, AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::metaAttributeSearchViewResult(std::string searchToken,
                             int startIndex, int maxCount,
                             DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                   ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("metaAttributeSearchViewResult() seachToken={} startIndex={} maxCount={}",
               searchToken, startIndex, maxCount);
    if(!m_runningAiSearch) return;

    auto metaAttributeSearch = std::make_shared<AiMetaAttributeSearchView>(
                m_logPrefix, finalResponse, searchToken, true, startIndex, maxCount, m_deviceUUID);
    asyncRequest(m_httpSearchMetaDataSession,
                 metaAttributeSearch,
                 [this, metaAttributeSearch, searchToken, startIndex, maxCount, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("metaAttributeSearchViewResult() response seachToken={} startIndex={} maxCount={}",
                       searchToken, startIndex, maxCount);

            finalResponse->deviceId = m_deviceUUID;
            finalResponse->errorCode = Wisenet::ErrorCode::NoError;
            if(finalResponse->totalCount == startIndex + (int)finalResponse->results.size() - 1){
                SLOG_DEBUG("metaAttributeSearchViewResult() final");
                m_runningAiSearch = false;
                responseHandler(finalResponse);
            }
            else{
                auto newFinalResponse = std::make_shared<DeviceMetaAttributeSearchResponse>();
                newFinalResponse->totalCount = finalResponse->totalCount;
                finalResponse->isContinue = true;
                responseHandler(finalResponse);
                metaAttributeSearchViewResult(searchToken, startIndex + finalResponse->results.size(), maxCount, newFinalResponse, responseHandler);
            }
        }
        else{
            m_runningAiSearch = false;
            responseHandler(finalResponse);
        }
    }, nullptr, AsyncRequestType::HTTPGET,false);
}

}
}

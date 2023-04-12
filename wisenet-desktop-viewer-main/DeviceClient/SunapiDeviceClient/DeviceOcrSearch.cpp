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

class AiOcrSearchControl : public BaseCommand, public JsonParser
{
public:
    explicit AiOcrSearchControl(const std::string& logPrefix,
                                          DeviceMetaAttributeSearchRequestSharedPtr request)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceMetaAttributeSearchResponse>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("ai","ocrsearch", "control");
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

        if(m_request->ocrText.length() > 0)
            builder.AddParameter("SearchText",m_request->ocrText);

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

class AiOcrSearchView : public BaseCommand, public JsonParser
{
public:
    explicit AiOcrSearchView(const std::string& logPrefix,
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
        builder.CreateCgi("ai","ocrsearch", "view");
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
        SLOG_DEBUG("AiOcrSearchView(), m_isResult={} ", m_isResult);
        if(m_isResult){
            itr = jsonDoc.FindMember("Results");
            if (itr != jsonDoc.MemberEnd()){
               auto & results = itr->value;
               for(auto & result : results.GetArray()){
                   MetaAttribute ocrAttribute;
                   ocrAttribute.attributeType = AiClassType::ocr;
                   ocrAttribute.aiBaseAttribute.index = m_resultIndex + m_finalResponse->results.size();
                   ocrAttribute.aiBaseAttribute.deviceID = m_deviceId;
                   auto itrResult = result.FindMember("DateTime");
                   if(itrResult != result.MemberEnd())
                       ocrAttribute.aiBaseAttribute.dateTime = Common::utcMsecsFromIsoString(itrResult->value.GetString());
                   itrResult = result.FindMember("Channel");
                   if(itrResult != result.MemberEnd())
                       ocrAttribute.aiBaseAttribute.channelId = itrResult->value.GetInt() + 1;

                   itrResult = result.FindMember("Text");
                   if(itrResult != result.MemberEnd())
                       ocrAttribute.ocrText = itrResult->value.GetString();

                   itrResult = result.FindMember("ImageURL");
                   if(itrResult != result.MemberEnd())
                       ocrAttribute.aiBaseAttribute.imageUrl = itrResult->value.GetString();

                   itrResult = result.FindMember("ObjectID");
                   if(itrResult != result.MemberEnd())
                       ocrAttribute.objectId = itrResult->value.GetInt();

                   SLOG_DEBUG("AiMetaAttributeSearchView(), url={}",
                               ocrAttribute.aiBaseAttribute.imageUrl);

                   itrResult = result.FindMember("Resolution");
                   if(itrResult != result.MemberEnd()){
                       std::string resolution = itrResult->value.GetString();
                       std::vector<std::string> tokens;
                       boost::split(tokens, resolution, boost::is_any_of("x"));

                       if (!try_stoi(tokens.at(0), &ocrAttribute.aiBaseAttribute.imageWidth))
                           continue;
                       if (!try_stoi(tokens.at(1), &ocrAttribute.aiBaseAttribute.imageHeight))
                           continue;
                   }

                   m_finalResponse->results.push_back(ocrAttribute);
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

void SunapiDeviceClientImpl::StartOcrSearchStatusTimer(std::string searchToken,
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
            ocrSearchViewStatus(searchToken, finalResponse, responseHandler);
        }));
    }));
}

void SunapiDeviceClientImpl::DeviceOcrSearch( DeviceMetaAttributeSearchRequestSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler )
{
    m_runningAiSearch = true;
    asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        SLOG_DEBUG("DeviceOcrSearch(), fromDate={} toDate={}",
                   request->fromDate, request->toDate);

        if(request->channelIDList.size() == 0 ||
                request->fromDate == 0 ||
                request->toDate == 0){
            SLOG_DEBUG("DeviceOcrSearch() channelSize={} from={} to={}",
                       request->channelIDList.size(),
                       request->fromDate, request->toDate);
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            m_runningAiSearch = false;
            responseHandler(response);
            return;
        }

        auto ocrSearchControl = std::make_shared<AiOcrSearchControl>(
                    m_logPrefix,
                    request);


        asyncRequest(m_httpSearchMetaDataSession,
                     ocrSearchControl,
                     [this, request, ocrSearchControl, responseHandler](const ResponseBaseSharedPtr& response)

        {
            auto finalResponse = std::make_shared<DeviceMetaAttributeSearchResponse>();
            if(response->isSuccess()){
                SLOG_DEBUG("DeviceOcrSearch() AiOcrSearchControl isSuccess totalCount={} ",
                           ocrSearchControl->m_totalCount);
                if(ocrSearchControl->m_totalCount == 0){
                    finalResponse->totalCount = 0;
                    finalResponse->errorCode = Wisenet::ErrorCode::NoError;
                    m_runningAiSearch = false;
                    responseHandler(finalResponse);
                    return;
                }

                finalResponse->totalCount = ocrSearchControl->m_totalCount;

                SLOG_DEBUG("DeviceOcrSearch() AiOcrSearchControl isSuccess 1 totalCount={} ",
                           ocrSearchControl->m_totalCount);

                ocrSearchViewStatus(ocrSearchControl->m_searchToken, finalResponse, responseHandler);

            }
            else{
                m_runningAiSearch = false;
                responseHandler(finalResponse);
            }
        }, nullptr, AsyncRequestType::HTTPGET,false);
    }));
}

void SunapiDeviceClientImpl::ocrSearchViewStatus(std::string searchToken,
                                                     DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                                     ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("ocrSearchViewStatus() seachToken={}",
               searchToken);
    if(!m_runningAiSearch) return;

    auto ocrSearch = std::make_shared<AiOcrSearchView>(
                m_logPrefix, finalResponse, searchToken, false, 1, 100, m_deviceUUID);

    asyncRequest(m_httpSearchMetaDataSession,
                 ocrSearch,
                 [this, ocrSearch, searchToken, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("ocrSearchViewStatus() isSuccess isResult={} ",
                       ocrSearch->m_isComplete);
            if(ocrSearch->m_isResult == false && ocrSearch->m_isComplete == false){
               StartOcrSearchStatusTimer(searchToken, finalResponse, responseHandler);
            }
            else{
                int maxResult = m_repos->attributes().aiCgi.ocrSearchMaxResults;
                if(maxResult > 100)
                    maxResult = 100;
               ocrSearchViewResult(searchToken, 1, maxResult, finalResponse, responseHandler);
            }
        }
        else{
            m_runningAiSearch = false;
            responseHandler(finalResponse);
        }
    }, nullptr, AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::ocrSearchViewResult(std::string searchToken,
                             int startIndex, int maxCount,
                             DeviceMetaAttributeSearchResponseSharedPtr finalResponse,
                                   ResponseBaseHandler const& responseHandler)
{
    SLOG_DEBUG("ocrSearchViewResult() seachToken={} startIndex={} maxCount={}",
               searchToken, startIndex, maxCount);
    if(!m_runningAiSearch) return;

    auto ocrSearch = std::make_shared<AiOcrSearchView>(
                m_logPrefix, finalResponse, searchToken, true, startIndex, maxCount, m_deviceUUID);
    asyncRequest(m_httpSearchMetaDataSession,
                 ocrSearch,
                 [this, ocrSearch, searchToken, startIndex, maxCount, finalResponse, responseHandler](const ResponseBaseSharedPtr& response)

    {
        if(response->isSuccess()){
            SLOG_DEBUG("ocrSearchViewResult() response seachToken={} startIndex={} maxCount={}",
                       searchToken, startIndex, maxCount);

            finalResponse->errorCode = Wisenet::ErrorCode::NoError;
            finalResponse->deviceId = m_deviceUUID;

            if(finalResponse->totalCount == startIndex + (int)finalResponse->results.size() - 1){
                SLOG_DEBUG("ocrSearchViewResult() final");
                m_runningAiSearch = false;
                responseHandler(finalResponse);
            }
            else{
                auto newFinalResponse = std::make_shared<DeviceMetaAttributeSearchResponse>();
                newFinalResponse->totalCount = finalResponse->totalCount;
                finalResponse->isContinue = true;
                responseHandler(finalResponse);

                ocrSearchViewResult(searchToken, startIndex + finalResponse->results.size(), maxCount, newFinalResponse, responseHandler);
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

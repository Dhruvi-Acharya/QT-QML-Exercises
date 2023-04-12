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
#pragma once
#include <string>
#include <unordered_map>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "SunapiDeviceClientUtil.h"
#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"
#include "TimeUtil.h"


namespace Wisenet
{
namespace Device
{

//************************* RecordingGeneralView ******************************//

struct RecordingGeneralViewResult
{
    struct General
    {
        std::string NormalMode;
        std::string EventMode;
        bool subStreamEnable = false;
    };
    std::unordered_map<int, General> generals;
};

class RecordingGeneralView : public BaseCommand, public IniParser
{
public:
    explicit RecordingGeneralView(const std::string& logPrefix,
                                  ResponseBaseSharedPtr response,
                                  bool ignoreHttpErrorResponse = false,
                                  bool ignoreParsingErrorResponse = false)
        : BaseCommand(this,logPrefix,response,
                      ignoreHttpErrorResponse,
                      ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","general", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    RecordingGeneralViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        auto isCamera = iniDoc.getIntValue("Channel");
        if (isCamera) { // TODO MULTIPLE CHANNEL CAMERA
            int channel = isCamera.value();
            RecordingGeneralViewResult::General general;
            general.NormalMode = iniDoc.getString("NormalMode");
            general.EventMode = iniDoc.getString("EventMode");

            parseResult.generals[channel] = general;
            return true;
        }

        // Recorder
        // Parse full response
        for (auto &item : iniDoc.m_listValue) {
            std::vector<std::string> tokens;
            int channelIndex = 0;

            boost::split(tokens, item.first, boost::is_any_of("."), boost::token_compress_on);
            if (tokens.size() != 3)
                continue;

            if (tokens[0] != "Channel")
                continue;

            if (!try_stoi(tokens[1], &channelIndex))
                continue;

            std::string& key = tokens[2];

            // Channel.0.EventMode = Off
            if (key == "EventMode")
                parseResult.generals[channelIndex].EventMode = item.second;
            else if (key == "NormalMode")
                parseResult.generals[channelIndex].EventMode = item.second;
            else if (key == "SubStreamEnable") {
                if (boost::iequals(item.second, "True"))
                    parseResult.generals[channelIndex].subStreamEnable = true;
                else
                    parseResult.generals[channelIndex].subStreamEnable = false;
            }
        }
        return true;
    }
};

//************************* RecordingStorageViewResult ******************************//


struct RecordingStorageViewResult
{
    bool enable = false;
    bool overWrite = false;
    bool autoDeleteEnable = false;
};

class RecordingStorageView : public BaseCommand, public IniParser
{
public:
    explicit RecordingStorageView(const std::string& logPrefix,
                                  ResponseBaseSharedPtr response,
                                  bool ignoreHttpErrorResponse = false,
                                  bool ignoreParsingErrorResponse = false)
        :BaseCommand(this,logPrefix,response,
                     ignoreHttpErrorResponse,
                     ignoreParsingErrorResponse)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","storage", "view");
        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        //응답처리를 하지 않으므로 nullptr로 리턴
        return nullptr;
    }

    RecordingStorageViewResult parseResult;

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        parseResult.enable = iniDoc.getBool("Enable", false);
        parseResult.overWrite = iniDoc.getBool("OverWrite", false);
        parseResult.autoDeleteEnable = iniDoc.getBool("AutoDeleteEnable", false);
        return true;
    }
};

class RecordingSmartSearchControlCancel : public BaseCommand, public JsonParser
{
public:
    explicit RecordingSmartSearchControlCancel(const std::string& logPrefix,
                                          const std::string& token)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceSmartSearchResponse>(),false,false),
          m_searchToken(token)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","smartsearch", "control");
        builder.AddParameter("Mode","Cancel");
        builder.AddParameter("SearchToken",m_searchToken);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        Q_UNUSED(jsonDoc);
        return true;
    }
    DeviceSmartSearchRequestSharedPtr m_request;

public:
    std::string m_searchToken;
};

class RecordingSmartSearchControl : public BaseCommand, public JsonParser
{
public:
    explicit RecordingSmartSearchControl(const std::string& logPrefix,
                                          DeviceSmartSearchRequestSharedPtr request)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceSmartSearchResponse>(),false,false),
          m_request(request)
    {
    }

    std::string convertAreaEventType(SmartSearchAreaEventType type){
        if(type == SmartSearchAreaEventType::motion)    return "Motion";
        if(type == SmartSearchAreaEventType::enter)    return "Enter";
        else   return "Exit";
    }
    std::string convertAIType(SmartSearchAIType type){
        if(type == SmartSearchAIType::person)    return "Person";
        if(type == SmartSearchAIType::vehicle)    return "Vehicle";
        else   return "Unknown";
    }
    std::string convertAreaType(SmartSearchAreaType type){
        if(type == SmartSearchAreaType::outside) return "Outside";
        else    return "Inside";
    }
    std::string convertLineEventType(SmartSearchLineEventType type){
        if(type == SmartSearchLineEventType::leftDirection)    return "Left";
        if(type == SmartSearchLineEventType::rightDirection)    return "Right";
        else   return "BothDirections";
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","smartsearch", "control");
        builder.AddParameter("Mode","Start");
        builder.AddParameter("Channel",toSunapiChannel(m_request->channel));
        builder.AddParameter("OverlappedID",m_request->OverlappedID);
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->fromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->toDate));

        for(auto& area : m_request->areas){
            std::string prefix = "Area." + boost::lexical_cast<std::string>(area.index) + ".";

            builder.AddParameter(prefix + "Type", convertAreaType(area.areaType));

            if(!area.eventType.empty()) {
                std::string eventType;
                for(auto& event : area.eventType){
                    eventType += (convertAreaEventType(event) + ",");
                }
                eventType.erase(eventType.length()-1);
                builder.AddParameter(prefix + "EventType", eventType);
            }

            if(!area.aiType.empty()) {
                std::string aiType = "";
                for(auto& ai : area.aiType){
                    aiType += (convertAIType(ai) + ",");
                }
                aiType.erase(aiType.length()-1);
                builder.AddParameter(prefix + "Filter", aiType);
            }

            std::string coordinates;
            for(auto& coordinate : area.coordinates){
                coordinates +=  (boost::lexical_cast<std::string>(coordinate) + ",");
            }
            coordinates.erase(coordinates.length()-1);
            builder.AddParameter(prefix + "Coordinates", coordinates);
        }
        for(auto& line : m_request->lines){
            std::string prefix = "Line." + boost::lexical_cast<std::string>(line.index) + ".";

            builder.AddParameter(prefix + "EventType", convertLineEventType(line.eventType));

            std::string coordinates;
            for(auto& coordinate : line.coordinates){
                coordinates +=  (boost::lexical_cast<std::string>(coordinate) + ",");
            }
            coordinates.erase(coordinates.length()-1);
            builder.AddParameter(prefix + "Coordinates", coordinates);
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
        auto response = std::static_pointer_cast<DeviceSmartSearchResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("SearchToken");
        if (itr != jsonDoc.MemberEnd())
            m_searchToken = itr->value.GetString();
        itr = jsonDoc.FindMember("TotalCount");
        if (itr != jsonDoc.MemberEnd())
           m_totalCount = itr->value.GetInt();

        return true;
    }
    DeviceSmartSearchRequestSharedPtr m_request;

public:
    std::string m_searchToken;
    int m_totalCount = 0;
};

class RecordingSmartSearchView : public BaseCommand, public JsonParser
{
public:
    explicit RecordingSmartSearchView(const std::string& logPrefix,
                                       DeviceSmartSearchResponseSharedPtr finalResponse,
                                       std::string searchToken, bool isResult)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceSmartSearchResponse>(),false,false),
            m_finalResponse(finalResponse),
            m_searchToken(searchToken),
            m_isResult(isResult)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","smartsearch", "view");
        builder.AddParameter("Type", m_isResult?"Results":"Status");
        builder.AddParameter("SearchToken", m_searchToken);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    SmartSearchResultType convertResultEventType(std::string type){
        if(type == "Motion")    return SmartSearchResultType::motion;
        if(type == "Enter")    return SmartSearchResultType::enter;
        if(type == "Exit")    return SmartSearchResultType::exit;
        else   return SmartSearchResultType::pass;
    }

    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        auto response = std::static_pointer_cast<DeviceSmartSearchResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("Status");
        if (itr != jsonDoc.MemberEnd()){
            std::string status = itr->value.GetString();
            if(status == "Completed")
                m_isComplete = true;    
        }

        if(m_isResult){
            m_finalResponse->searchToken = m_searchToken;
            itr = jsonDoc.FindMember("TotalCount");
            if (itr != jsonDoc.MemberEnd())
               m_finalResponse->totalCount = itr->value.GetInt();
            SLOG_DEBUG("RecordingSmartSearchView(), m_isResult={} total={}", m_isResult, m_finalResponse->totalCount);
            itr = jsonDoc.FindMember("SmartSearchResults");
            if (itr != jsonDoc.MemberEnd()){
               auto & results = itr->value;
               for(auto & result : results.GetArray()){

                    auto itrResult = result.FindMember("Channel");
                    int channel = 0;
                    if(itrResult != result.MemberEnd())
                        channel = itrResult->value.GetInt() +1;

                    itrResult = result.FindMember("Results");
                    if(itrResult != result.MemberEnd()){
                        for(auto &subResult : itrResult->value.GetArray()){
                            SmartSearchResult searchResult;
                            searchResult.channel = channel;
                            auto itrResult2 = subResult.FindMember("Result");
                            if(itrResult2 != subResult.MemberEnd())
                                searchResult.index = itrResult2->value.GetInt() +1;

                            itrResult2 = subResult.FindMember("EventTime");
                            if(itrResult2 != subResult.MemberEnd())
                                searchResult.dateTime = Common::utcMsecsFromIsoString(itrResult2->value.GetString());

                            itrResult2 = subResult.FindMember("EventType");
                            if(itrResult2 != subResult.MemberEnd())
                                searchResult.type = convertResultEventType(itrResult2->value.GetString());
                            //SLOG_DEBUG("RecordingSmartSearchView(), index={} time={} type={}",
                             //          searchResult.index, searchResult.dateTime, (int)searchResult.type);
                            m_finalResponse->results.push_back(searchResult);
                        }
                    }

               }
            }
        }
        return true;
    }

public:
    DeviceSmartSearchResponseSharedPtr m_finalResponse;
    bool m_isComplete = false;
    std::string m_searchToken;
    bool m_isResult = false;
    int m_maxResults = 0;
    uuid_string m_deviceId;
};

// 2022.08.29. added
class RecordingTextSearchControl : public BaseCommand, public JsonParser
{
public:
    explicit RecordingTextSearchControl(const std::string& logPrefix,
                                          DeviceTextSearchRequestSharedPtr request)
        :BaseCommand(this, logPrefix,
                     std::make_shared<DeviceTextSearchResponse>(), false, false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","metadata", "control");
        builder.AddParameter("Mode", m_request->mode);
        builder.AddParameter("MetadataType", m_request->metaDataType);
        builder.AddParameter("DeviceIDList", toSunapiChannel(m_request->deviceIDList));
        builder.AddParameter("OverlappedID", m_request->overlappedID);
        builder.AddParameter("Keyword", m_request->keyword);
        builder.AddParameter("IsWholeWord", m_request->isWholeword);
        builder.AddParameter("IsCaseSensitive", m_request->isCaseSensitive);
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->fromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->toDate));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        auto response = std::static_pointer_cast<DeviceTextSearchResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("SearchToken");
        if (itr != jsonDoc.MemberEnd())
           m_searchToken = itr->value.GetString();

        return true;
    }
    DeviceTextSearchRequestSharedPtr m_request;

public:
    std::string m_searchToken;
};

// 2022.08.29. added
class RecordingTextSearchView: public BaseCommand, public JsonParser
{
public:
    explicit RecordingTextSearchView(const std::string& logPrefix,
                                     DeviceTextSearchResponseSharedPtr finalResponse,
                                     std::string deviceID,
                                     std::string searchToken,
                                     std::string status)
        :BaseCommand(this, logPrefix,
                     std::make_shared<DeviceTextSearchResponse>(), false, false),
          m_finalResponse(finalResponse),
          m_deviceID(deviceID),
          m_searchToken(searchToken),
          m_status(status)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","metadata", "view");
        builder.AddParameter("Type", "Status");
        builder.AddParameter("SearchToken", m_searchToken);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        auto response = std::static_pointer_cast<DeviceTextSearchStatusResponse>(m_responseBase);

        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("SearchToken");
        if (itr != jsonDoc.MemberEnd())
            m_searchToken = itr->value.GetString();

        itr = jsonDoc.FindMember("Status");
        if (itr != jsonDoc.MemberEnd())
           m_status = itr->value.GetString();

        itr = jsonDoc.FindMember("TotalCount");
        if (itr != jsonDoc.MemberEnd())
           m_totalCount = itr->value.GetInt();

        return true;
    }

public:
    DeviceTextSearchResponseSharedPtr m_finalResponse;
    std::string m_deviceID;
    std::string m_searchToken;
    std::string m_status;
    int m_totalCount = 0;
};

// 2022.08.29. added
class RecordingTextSearchResultView: public BaseCommand, public IniParser
{
public:
    explicit RecordingTextSearchResultView(const std::string& logPrefix,
                                     DeviceTextSearchResponseSharedPtr finalResponse,
                                     std::string deviceID, std::string searchToken, std::string requestCount)
        :BaseCommand(this, logPrefix,
                     std::make_shared<DeviceTextSearchResponse>(), false, false),
          m_finalResponse(finalResponse),
          m_requestCount(requestCount),
          m_deviceID(deviceID),
          m_searchToken(searchToken)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","metadata", "view");
        builder.AddParameter("Type", "Results");
        builder.AddParameter("SearchToken", m_searchToken);
        builder.AddParameter("ResultFromIndex", "1");
        builder.AddParameter("MaxResults", m_requestCount);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        m_intervalFrom = "";
        m_intervalTo = "";
        m_searchTokenExpirytime = "";
        m_totalResultsFound = 0;
        m_totalCount = 0;

        for (auto& itr : iniDoc.m_listValue)
        {
            if (itr.first == "SearchTokenExpirytime")
                m_searchTokenExpirytime = itr.second;
            else if (itr.first == "TotalResultsFound")
                m_totalResultsFound = std::stoi(itr.second);
            else if (itr.first == "TotalCount")
                m_totalCount = std::stoi(itr.second);
            else if (itr.first == "IntervalFrom")
                m_intervalFrom = itr.second;
            else if (itr.first == "IntervalTo")
                m_intervalTo = itr.second;
        }

        auto textDataList = new TextSearchDataResult[m_totalResultsFound];

        for (auto& itr : iniDoc.m_value)
        {
            std::vector<std::string> tokens;

            boost::split(tokens, itr.first, boost::is_any_of("."));
            std::sort(tokens.begin(), tokens.end());

            if (3 > tokens.size())
                continue;

            int chNum = 0;
            try_stoi(tokens[0], &chNum, 0);

            std::string& key = tokens[1];

            textDataList[chNum - 1].uuid = m_deviceID;

            if (key == "DeviceID") {
                textDataList[chNum - 1].deviceID = try_stoi(itr.second);
            }
            else if (key == "Date") {
                textDataList[chNum - 1].date = itr.second;
            }
            else if (key == "PlayTime") {
                textDataList[chNum - 1].playTime = itr.second;
            }
            else if (key == "ChannelIDList") {
                textDataList[chNum - 1].channelIDList = itr.second;
            }
            else if (key == "KeywordsMatched") {
                textDataList[chNum - 1].keywordsMatched = itr.second;
            }
            else if (key == "Result" || key == "TextData") {
                textDataList[chNum - 1].textData = itr.second;;
            }
        }

        for (int i=0; i<m_totalResultsFound; i++)
        {
            TextSearchDataResult textData;
            textData = textDataList[i];
            m_textData.push_back(textData);
        }

        delete[] textDataList;

        return true;
    }

public:
    DeviceTextSearchResponseSharedPtr m_finalResponse;

    std::vector<TextSearchDataResult> m_textData;

    std::string m_requestCount;
    std::string m_deviceID;
    std::string m_searchToken;
    std::string m_searchTokenExpirytime;
    std::string m_intervalFrom;
    std::string m_intervalTo;

    int m_totalResultsFound = 0;
    int m_totalCount = 0;
};

// 2022.09.01. added
class RecordingTextPosConfView: public BaseCommand, public IniParser
{
public:
    explicit RecordingTextPosConfView(const std::string& logPrefix,
                                      ResponseBaseSharedPtr response,
                                      bool ignoreHttpErrorResponse = false,
                                      bool ignoreParsingErrorResponse = false)
        :BaseCommand(this,logPrefix,response,
                     ignoreHttpErrorResponse,
                     ignoreParsingErrorResponse)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","posconf", "view");

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        if(m_responseBase)
            m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        std::map<std::string, TextPosConfResult> textPosConfResultMap;

        for (auto& itr : iniDoc.m_listValue)
        {
            std::vector<std::string> tokens;

            boost::split(tokens, itr.first, boost::is_any_of("."));

            if (3 > tokens.size())
                continue;

            textPosConfResultMap[tokens[1]].deviceId = tokens[1];
            if (tokens[2] == "DeviceName") {
                textPosConfResultMap[tokens[1]].deviceName = itr.second;
            }
            else if (tokens[2] == "Enable") {
                textPosConfResultMap[tokens[1]].enable = itr.second == "True";
            }
            else if (tokens[2] == "EncodingType") {
                textPosConfResultMap[tokens[1]].encodingType = itr.second;
            }
            else if (tokens[2] == "ChannelIDList") {
                textPosConfResultMap[tokens[1]].channelIDList = itr.second;
            }
        }

        for(auto& itr : textPosConfResultMap) {
            m_confData.push_back(itr.second);
        }

        return true;
    }

public:
    DeviceTextPosConfResponseSharedPtr m_finalResponse;

    std::vector<TextPosConfResult> m_confData;
};

// 2022.10.31. added
class RecordingTextSearchControlCancel : public BaseCommand, public JsonParser
{
public:
    explicit RecordingTextSearchControlCancel(const std::string& logPrefix,
                                          const std::string& token)
        :BaseCommand(this,logPrefix,
                     std::make_shared<DeviceTextSearchResponse>(),false,false),
          m_searchToken(token)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording","metadata", "control");
        builder.AddParameter("Mode","Cancel");
        builder.AddParameter("SearchToken",m_searchToken);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        Q_UNUSED(jsonDoc);
        return true;
    }
    DeviceTextSearchRequestSharedPtr m_request;

public:
    std::string m_searchToken;
};

}
}

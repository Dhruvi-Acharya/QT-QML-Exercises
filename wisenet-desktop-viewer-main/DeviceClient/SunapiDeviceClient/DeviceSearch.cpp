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

//*************************** DeviceGetOverlappedId *****************************************//

class RecordingOverlappedView : public BaseCommand, public IniParser
{
public:
    explicit RecordingOverlappedView(const std::string& logPrefix, DeviceGetOverlappedIdRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetOverlappedIdResponse>(),false,false),
          m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording", "overlapped", "view");
        if(m_request->param.ChannelIDList.size() > 0)
        {
            std::string channelIdList = "";
            for(auto channelId : m_request->param.ChannelIDList)
            {
                channelIdList += std::to_string(toSunapiChannel(channelId)) + ",";
            }
            channelIdList.erase(channelIdList.size()-1);
            builder.AddParameter("ChannelIDList", channelIdList);
        }
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->param.FromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->param.ToDate));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        auto response = std::static_pointer_cast<DeviceGetOverlappedIdResponse>(m_responseBase);
        // Overlapped recording ID list
        std::vector<std::string> trackList;
        boost::split(trackList, iniDoc.getString("OverlappedIDList"), boost::is_any_of(","), boost::token_compress_on);
        for(auto strTrackId : trackList)
        {
            int trackId = 0;
            if (try_stoi(strTrackId, &trackId))
                response->result.overlappedIDList.push_back(trackId);
        }

        /*
        Overlapped recording ID list for each Channel
        MULTI DIRECTIONAL CAMERA ONLY
        */
        for (auto item : iniDoc.m_value)
        {
            if (item.first.find("Channel") == std::string::npos)
                continue;

            std::vector<std::string> channelTokens;
            boost::split(channelTokens, item.first, boost::is_any_of("."), boost::token_compress_on);

            int curChannel = 0;
            if (channelTokens.size() != 3 || !try_stoi(channelTokens.at(1), &curChannel))
                continue;

            std::string serviceChannel = fromSunapiChannel(curChannel);

            std::vector<std::string> channelTracks;
            boost::split(channelTracks, item.second, boost::is_any_of(","), boost::token_compress_on);
            for(auto strTrackId : channelTracks)
            {
                int trackId = 0;
                if (try_stoi(strTrackId, &trackId))
                {
                    if(response->result.channelOverlappedIDList.find(serviceChannel) == response->result.channelOverlappedIDList.end())
                    {
                        std::vector<int> channelTrackList;
                        response->result.channelOverlappedIDList.emplace(serviceChannel, channelTrackList);
                    }

                    response->result.channelOverlappedIDList[serviceChannel].push_back(trackId);
                }
            }
        }

        return true;
    }

    DeviceGetOverlappedIdRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetOverlappedId( DeviceGetOverlappedIdRequestSharedPtr const& request,
                                                    ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<RecordingOverlappedView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

//*************************** DeviceCalendarsearch *****************************************//

class RecordingCalendarsearchView : public BaseCommand, public IniParser
{
public:
    explicit RecordingCalendarsearchView(const std::string& logPrefix,DeviceCalendarSearchRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceCalendarSearchResponse>(),false,false),
          m_request(request)
    {

    }
    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording", "calendarsearch", "view");

        if(m_request->param.Month != 0)
        {
            std::string strDateTime = Wisenet::Common::utcMsecsToUtcIsoString(m_request->param.Month).substr(0, 7);
            builder.AddParameter("Month", strDateTime);
        }

        if(m_request->param.PrimaryDeviceIPAddress != "")
            builder.AddParameter("PrimaryDeviceIPAddress", m_request->param.PrimaryDeviceIPAddress);

        if(m_request->param.IgnoreChannelBasedResults == true)
        {
            builder.AddParameter("IgnoreChannelBasedResults", m_request->param.IgnoreChannelBasedResults);
        }
        else
        {
            if(m_request->param.ChannelIDList.size() > 0)
            {
                std::string channelIdList = "";
                for(auto channelId : m_request->param.ChannelIDList)
                {
                    channelIdList += std::to_string(toSunapiChannel(channelId)) + ",";
                }
                channelIdList.erase(channelIdList.size()-1);
                builder.AddParameter("ChannelIDList", channelIdList);
            }
        }

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }


private:
    bool parseINI(NameValueText& iniDoc) override
    {
        auto response = std::static_pointer_cast<DeviceCalendarSearchResponse>(m_responseBase);

        // Result for all channel (when IgnoreChannelBasedResults == true, NVR Only)
        std::string strResult = iniDoc.getString("Result");
        for(unsigned long long i=0 ; i<strResult.length() ; i++)
        {
            if(strResult[i] == '1')
                response->result.result.dayRecordExist[i] = true;
            else
                response->result.result.dayRecordExist[i] = false;
        }

        // Result for each Channel
        for (auto& item : iniDoc.m_value)
        {
            if (item.first.find("Channel") == std::string::npos)
                continue;

            std::vector<std::string> channelTokens;
            boost::split(channelTokens, item.first, boost::is_any_of("."), boost::token_compress_on);

            int curChannel = 0;
            if (channelTokens.size() != 3 || !try_stoi(channelTokens[1], &curChannel) || channelTokens[2] != "Result")
                continue;

            std::string serviceChannel = fromSunapiChannel(curChannel);

            MonthResultArray channelResult;
            for(unsigned long long i=0 ; i<item.second.length() ; i++)
            {
                if(item.second[i] == '1')
                    channelResult.dayRecordExist[i] = true;
                else
                    channelResult.dayRecordExist[i] = false;
            }

            response->result.channelReslut[serviceChannel] = channelResult;
        }

        return true;
    }

    DeviceCalendarSearchRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceCalendarSearch(DeviceCalendarSearchRequestSharedPtr const& request,
                                                  ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<RecordingCalendarsearchView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

//*************************** DeviceGetTimeline *****************************************//
class RecordingTimelineView : public BaseCommand, public IniParser
{
public:
    explicit RecordingTimelineView(const std::string& logPrefix, DeviceGetTimelineRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetTimelineResponse>(),false,false),
          m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording", "timeline", "view");
        builder.AddParameter("FromDate", Common::utcMsecsToUtcIsoString(m_request->param.FromDate));
        builder.AddParameter("ToDate", Common::utcMsecsToUtcIsoString(m_request->param.ToDate));
        builder.AddParameter("Type", "All");    // 장비에는 항상 All로 요청

        if(m_request->param.OverlappedID != -1)
            builder.AddParameter("OverlappedID", std::to_string(m_request->param.OverlappedID));

        if(m_request->param.ChannelIDList.size() > 0)
        {
            std::string channelIdList = "";
            for(auto channelId : m_request->param.ChannelIDList)
            {
                channelIdList += std::to_string(toSunapiChannel(channelId)) + ",";
            }
            channelIdList.erase(channelIdList.size()-1);
            builder.AddParameter("ChannelIDList", channelIdList);
        }

        if(m_request->param.PrimaryDeviceIPAddress != "")
            builder.AddParameter("PrimaryDeviceIPAddress", m_request->param.PrimaryDeviceIPAddress);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }
private:
    bool parseINI(NameValueText& iniDoc) override
    {
        auto response = std::static_pointer_cast<DeviceGetTimelineResponse>(m_responseBase);

        // 타임라인 검색 요청한 각 채널의 빈 결과 Response 생성
        int overlappedID = m_request->param.OverlappedID;
        for(auto& channelId : m_request->param.ChannelIDList)
        {
            std::unordered_map<int, std::vector<ChannelTimeline>> trackResultMap;
            std::vector<ChannelTimeline> channelTimelineList;
            trackResultMap[overlappedID] = channelTimelineList;
            response->result.channelTimelineList[channelId] = trackResultMap;
        }

        // 결과 Parsing용 Map (Primary key : Channel ID, Second key : Result Number, value : Timeline Data)
        std::unordered_map<std::string, std::map<int, ChannelTimeline>> resultMap;

        for(auto item : iniDoc.m_value)
        {
            if(item.first.find("Channel") == std::string::npos || item.first.find("Result") == std::string::npos)
                continue;

            std::vector<std::string> channelTokens;
            boost::split(channelTokens, item.first, boost::is_any_of("."), boost::token_compress_on);

            if(channelTokens.size() != 5)
                continue;

            int curChannel = -1;
            int curResultNo = -1;
            try_stoi(channelTokens.at(1), &curChannel, -1);
            try_stoi(channelTokens.at(3), &curResultNo, -1);

            if(curChannel == -1 || curResultNo == -1)
                continue;

            std::string serviceChannel = fromSunapiChannel(curChannel);

            if(resultMap.find(serviceChannel) == resultMap.end())
            {
                std::map<int, ChannelTimeline> channelTimelineList;
                resultMap.emplace(serviceChannel, channelTimelineList);
            }

            if(resultMap[serviceChannel].find(curResultNo) == resultMap[serviceChannel].end())
            {
                ChannelTimeline channelTimeline;
                resultMap[serviceChannel].emplace(curResultNo, channelTimeline);
            }

            if(channelTokens.at(4) == "StartTime")
            {
                int64_t startTime = Common::utcMsecsFromIsoString(item.second);
                resultMap[serviceChannel][curResultNo].startTime = startTime;
            }
            else if(channelTokens.at(4) == "EndTime")
            {
                int64_t endTime = Common::utcMsecsFromIsoString(item.second);
                resultMap[serviceChannel][curResultNo].endTime = endTime;
            }
            else if(channelTokens.at(4) == "Type")
            {
                resultMap[serviceChannel][curResultNo].type = convertRecordingType(item.second);
            }
        }

        // 결과 Parsing Map에서 Response로 값 복사
        for(auto channelMap : resultMap)
        {
            for(auto timelineMap : channelMap.second)
            {
                response->result.channelTimelineList[channelMap.first][overlappedID].push_back(timelineMap.second);
            }
        }

        return true;
    }

    DeviceGetTimelineRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetTimeline( DeviceGetTimelineRequestSharedPtr const& request,
                                                ResponseBaseHandler const& responseHandler )
{
    if(request->param.OverlappedID != -1) {
        asyncRequest(m_httpCommandSession,
                     std::make_shared<RecordingTimelineView>(m_logPrefix,request),
                     responseHandler,
                     nullptr,AsyncRequestType::HTTPBIGGET,false, nullptr, 60);
    }
    else {
        // OverlappedID 미지정(-1)이면 OverlappedID List 획득 후 각 OverlappedID별 타임라인 로드
        getAllOverlappedIdTimeline(request, responseHandler);
    }
}

//*************************** DeviceGetRecordingPeriod *****************************************//

class RecordingSearchRecordingPeriodView : public BaseCommand, public IniParser
{
public:
    explicit RecordingSearchRecordingPeriodView(const std::string& logPrefix, DeviceGetRecordingPeriodRequestSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetRecordingPeriodResponse>(),false,false),
          m_request(request)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("recording", "searchrecordingperiod", "view");
        builder.AddParameter("ResultsInUTC", true);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    bool parseINI(NameValueText& iniDoc) override
    {
        auto response = std::static_pointer_cast<DeviceGetRecordingPeriodResponse>(m_responseBase);

        response->result.startTime = Common::utcMsecsFromIsoString(iniDoc.getString("StartTime"));
        response->result.endTime = Common::utcMsecsFromIsoString(iniDoc.getString("EndTime"));

        return true;
    }

    DeviceGetRecordingPeriodRequestSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetRecordingPeriod( DeviceGetRecordingPeriodRequestSharedPtr const& request,
                                                       ResponseBaseHandler const& responseHandler )
{
    asyncRequest(m_httpCommandSession,
                 std::make_shared<RecordingSearchRecordingPeriodView>(m_logPrefix, request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}


//*************************** DeviceGetAllTimeline *****************************************//

void SunapiDeviceClientImpl::DeviceGetAllPeriodTimeline( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                         ResponseBaseHandler const& responseHandler )
{
    if(m_repos->attributes().recordingAttribute.searchPeriod)
    {
        getAllPeriodTimeline(request, responseHandler);
    }
    else
    {
        auto finalResponse = std::make_shared<DeviceGetAllPeriodTimelineResponse>();
        getRecordingPeriodFromCalendarSearch(request, responseHandler, finalResponse, 0);
    }
}

void SunapiDeviceClientImpl::getAllPeriodTimeline( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                   ResponseBaseHandler const& responseHandler )
{
    // Recording Period 먼저 획득
    auto recordingPeriodView = std::make_shared<RecordingSearchRecordingPeriodView>(m_logPrefix, std::make_shared<DeviceGetRecordingPeriodRequest>());

    asyncRequest(m_httpCommandSession,
                 recordingPeriodView,
                 [this, request, responseHandler](const ResponseBaseSharedPtr& response)
    {
        auto finalResponse = std::make_shared<DeviceGetAllPeriodTimelineResponse>();
        auto recordingPeriodResponse = std::static_pointer_cast<DeviceGetRecordingPeriodResponse>(response);

        if(recordingPeriodResponse->errorCode != Wisenet::ErrorCode::NoError)
        {
            // 실패 응답
            finalResponse->errorCode = recordingPeriodResponse->errorCode;
            responseHandler(finalResponse);
            return;
        }

        // Response에 Recording Period 저장
        finalResponse->recordingPeriod = recordingPeriodResponse->result;

        // Overlapped Id 요청
        getAllPeriodOverlappedId(request, responseHandler, finalResponse);
    }, nullptr,AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::getAllPeriodOverlappedId( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                       ResponseBaseHandler const& responseHandler,
                                                       DeviceGetAllPeriodTimelineResponseSharedPtr const& finalResponse )
{
    // OverlappedId 획득 Request 생성
    DeviceGetOverlappedIdRequestSharedPtr overlappedIdRequest = std::make_shared<DeviceGetOverlappedIdRequest>();
    for(auto& channelId : request->ChannelIDList)
    {
        overlappedIdRequest->param.ChannelIDList.push_back(channelId);
    }
    overlappedIdRequest->param.FromDate = finalResponse->recordingPeriod.startTime;
    overlappedIdRequest->param.ToDate = finalResponse->recordingPeriod.endTime;

    auto overlappedIdView = std::make_shared<RecordingOverlappedView>(m_logPrefix, overlappedIdRequest);

    // OverlappedId 획득 Request
    asyncRequest(m_httpCommandSession,
                 overlappedIdView,
                 [this, request, responseHandler, finalResponse](const ResponseBaseSharedPtr& response)
    {
        auto overlappedIdResponse = std::static_pointer_cast<DeviceGetOverlappedIdResponse>(response);

        if(overlappedIdResponse->errorCode != Wisenet::ErrorCode::NoError)
        {
            // 실패 응답
            finalResponse->errorCode = overlappedIdResponse->errorCode;
            responseHandler(finalResponse);
            return;
        }

        // Response에 Overlapped Id 저장
        finalResponse->overlappedId = overlappedIdResponse->result;

        // Overlapped Id가 하나도 없으면 각 채널 별로 빈 결과를 생성하여 리턴
        if(finalResponse->overlappedId.overlappedIDList.size() == 0)
        {
            for(auto& channelId : request->ChannelIDList)
            {
                std::unordered_map<int, std::vector<ChannelTimeline>> trackResultMap;
                std::vector<ChannelTimeline> channelTimelineList;
                trackResultMap[-1] = channelTimelineList;
                finalResponse->timeline.channelTimelineList[channelId] = trackResultMap;
            }

            responseHandler(finalResponse);
            return;
        }

        // Overlapped Id 별 Timeline 획득 요청
        getAllPeriodOverlappedTimeline(request, responseHandler, finalResponse, 0, 0,
                finalResponse->recordingPeriod.startTime, finalResponse->recordingPeriod.endTime);

    }, nullptr,AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::getAllPeriodOverlappedTimeline( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                             ResponseBaseHandler const& responseHandler,
                                                             DeviceGetAllPeriodTimelineResponseSharedPtr const& finalResponse,
                                                             unsigned int overlappedIdIndex,
                                                             unsigned int channelIdIndex,
                                                             int64_t startTime,
                                                             int64_t endTime )
{
    // 모든 OverlappedId의 Timeline 획득 한 경우 Response
    if(overlappedIdIndex >= finalResponse->overlappedId.overlappedIDList.size())
    {
        responseHandler(finalResponse);
        return;
    }

    // 마지막 Channel Timeline 획득 한 경우 다음 OverlappedId Timeline 요청
    if(channelIdIndex >= request->ChannelIDList.size())
    {
        getAllPeriodOverlappedTimeline(request, responseHandler, finalResponse, overlappedIdIndex+1, 0,
                finalResponse->recordingPeriod.startTime, finalResponse->recordingPeriod.endTime);
        return;
    }

    // Timeline 획득 Request 생성
    DeviceGetTimelineRequestSharedPtr timelineRequest = std::make_shared<DeviceGetTimelineRequest>();
    timelineRequest->param.FromDate = startTime;//finalResponse->recordingPeriod.startTime;
    timelineRequest->param.ToDate = endTime;//finalResponse->recordingPeriod.endTime;
    timelineRequest->param.OverlappedID = finalResponse->overlappedId.overlappedIDList[overlappedIdIndex];
    timelineRequest->param.ChannelIDList.push_back(request->ChannelIDList[channelIdIndex]);
    /*
    for(auto& channelId : request->ChannelIDList)
    {
        timelineRequest->param.ChannelIDList.push_back(channelId);
    }
    */

    auto timelineView = std::make_shared<RecordingTimelineView>(m_logPrefix, timelineRequest);

    // OverlappedId에 해당하는 Timeline 획득 Request
    asyncRequest(m_httpCommandSession,
                 timelineView,
                 [this, request, responseHandler, finalResponse, overlappedIdIndex, channelIdIndex](const ResponseBaseSharedPtr& response)
    {
        auto timelineResponse = std::static_pointer_cast<DeviceGetTimelineResponse>(response);
        int count = 0;
        int64_t lastRecordEndTime = 0;

        // 성공 시 Response에 Timeline 저장
        if(timelineResponse->errorCode == Wisenet::ErrorCode::NoError)
        {
            for(auto& channelMap : timelineResponse->result.channelTimelineList)
            {
                if(finalResponse->channelOverlappedIdList.find(channelMap.first) == finalResponse->channelOverlappedIdList.end()) {
                    // 채널 별 OverlappedId List vector 생성
                    std::vector<int> channelOverlappedIdList;
                    finalResponse->channelOverlappedIdList[channelMap.first] = channelOverlappedIdList;
                }

                for(auto& timelineMap : channelMap.second)
                {
                    if(std::find(finalResponse->channelOverlappedIdList[channelMap.first].begin(),
                                 finalResponse->channelOverlappedIdList[channelMap.first].end(),
                                 timelineMap.first) == finalResponse->channelOverlappedIdList[channelMap.first].end()) {
                        finalResponse->channelOverlappedIdList[channelMap.first].push_back(timelineMap.first);  // 채널 별 OverlappedId List 업데이트
                    }

                    count = timelineMap.second.size();
                    for(auto& timeline : timelineMap.second)
                    {
                        finalResponse->timeline.channelTimelineList[channelMap.first][timelineMap.first].push_back(timeline);
                        if(lastRecordEndTime < timeline.endTime)
                            lastRecordEndTime = timeline.endTime;
                    }
                }
            }
        }

        if(count >= 5000 && lastRecordEndTime != 0)
        {
            // 타임라인 최대 수 만큼 로딩 된 경우, 같은 채널의 뒷 부분 추가 로딩
            getAllPeriodOverlappedTimeline(request, responseHandler, finalResponse, overlappedIdIndex, channelIdIndex,
                    lastRecordEndTime, finalResponse->recordingPeriod.endTime);
        }
        else
        {
            // 다음 채널 타임라인 로딩
            getAllPeriodOverlappedTimeline(request, responseHandler, finalResponse, overlappedIdIndex, channelIdIndex+1,
                    finalResponse->recordingPeriod.startTime, finalResponse->recordingPeriod.endTime);
        }

        // 다음 OverlappedId의 Timeline 요청
        //getAllPeriodOverlappedTimeline(request, responseHandler, finalResponse, overlappedIdIndex+1);
    }, nullptr,AsyncRequestType::HTTPBIGGET,false, nullptr, 60);
}

void SunapiDeviceClientImpl::getRecordingPeriodFromCalendarSearch( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                                   ResponseBaseHandler const& responseHandler,
                                                                   DeviceGetAllPeriodTimelineResponseSharedPtr const& finalResponse,
                                                                   unsigned int periodIndex )
{
    // target month가 없으면 실패 응답
    if(request->Month == 0)
    {
        finalResponse->errorCode = Wisenet::ErrorCode::InvalidRequest;
        responseHandler(finalResponse);
        return;
    }

    // Period 내의 모든 CalendarSearch 완료 시,
    if(periodIndex >= request->Period)
    {
        // CalendarSearch로 획득 한 녹화 구간 내의 OverlappedId 리스트 요청
        getAllPeriodOverlappedId(request, responseHandler, finalResponse);
        return;
    }

    // Calendar Search를 통해 Recording Period획득
    DeviceCalendarSearchRequestSharedPtr calendarSearchRequest = std::make_shared<DeviceCalendarSearchRequest>();
    for(auto& channelId : request->ChannelIDList)
    {
        calendarSearchRequest->param.ChannelIDList.push_back(channelId);
    }
    boost::posix_time::ptime p = boost::posix_time::from_time_t(request->Month / 1000);
    p = p - boost::gregorian::months(periodIndex);
    int64_t monthMsec = boost::posix_time::to_time_t(p) * 1000;
    calendarSearchRequest->param.Month = monthMsec;

    auto calendarsearchView = std::make_shared<RecordingCalendarsearchView>(m_logPrefix, calendarSearchRequest);
    asyncRequest(m_httpCommandSession,
                 calendarsearchView,
                 [this, request, responseHandler, finalResponse, periodIndex, monthMsec](const ResponseBaseSharedPtr& response)
    {
        auto calendarSearchResponse = std::static_pointer_cast<DeviceCalendarSearchResponse>(response);

        if(calendarSearchResponse->errorCode != Wisenet::ErrorCode::NoError)
        {
            // 실패 응답
            finalResponse->errorCode = calendarSearchResponse->errorCode;
            responseHandler(finalResponse);
            return;
        }

        // Response에 Recording Period 저장
        updateRecordingPeriodFromCalendarSearch(finalResponse, calendarSearchResponse, monthMsec);

        // 이전 달의 CalendarSearch 추가 요청
        getRecordingPeriodFromCalendarSearch(request, responseHandler, finalResponse, periodIndex+1);
    }, nullptr,AsyncRequestType::HTTPGET,false);
}

void SunapiDeviceClientImpl::updateRecordingPeriodFromCalendarSearch(DeviceGetAllPeriodTimelineResponseSharedPtr const& finalResponse,
                                                                     DeviceCalendarSearchResponseSharedPtr const calendarResponse,
                                                                     int64_t monthMsec)
{
    boost::gregorian::date targetMonth = boost::posix_time::from_time_t(monthMsec / 1000).date();
    int64_t startTime = 0;
    int64_t endTime = 0;

    for(auto& channel : calendarResponse->result.channelReslut)
    {
        for(int i=0 ; i<31 ; i++)
        {
            bool hasRecord = channel.second.dayRecordExist[i];
            if(!hasRecord)
                continue;

            if(startTime == 0)
            {
                boost::posix_time::ptime p(boost::gregorian::date(targetMonth.year(), targetMonth.month(), i+1), boost::posix_time::time_duration(0,0,0));
                startTime = boost::posix_time::to_time_t(p) * 1000;
            }

            boost::posix_time::ptime p(boost::gregorian::date(targetMonth.year(), targetMonth.month(), i+1), boost::posix_time::time_duration(23, 59, 59));
            int64_t tempEndTime = boost::posix_time::to_time_t(p) * 1000 + 999;

            if(endTime < tempEndTime)
                endTime = tempEndTime;
        }
    }

    if(startTime != 0 && endTime != 0)
    {
        if(finalResponse->recordingPeriod.startTime == 0 || finalResponse->recordingPeriod.startTime > startTime)
            finalResponse->recordingPeriod.startTime = startTime;

        if(finalResponse->recordingPeriod.endTime == 0 || finalResponse->recordingPeriod.endTime < endTime)
            finalResponse->recordingPeriod.endTime = endTime;
    }
}

void SunapiDeviceClientImpl::getRecentTimeline( DeviceGetAllPeriodTimelineRequestSharedPtr const& request,
                                                ResponseBaseHandler const& responseHandler )
{
    /*
    // Calendar Search를 통해 Recording Period획득
    DeviceCalendarSearchRequestSharedPtr calendarSearchRequest = std::make_shared<DeviceCalendarSearchRequest>();
    for(auto& channelId : request->ChannelIDList)
    {
        calendarSearchRequest->param.ChannelIDList.push_back(channelId);
    }
    calendarSearchRequest->param.Month = request->Month;

    auto calendarsearchView = std::make_shared<RecordingCalendarsearchView>(m_logPrefix, calendarSearchRequest);

    asyncRequest(m_httpCommandSession,
                 calendarsearchView,
                 [this, request, responseHandler](const ResponseBaseSharedPtr& response)
    {
        auto finalResponse = std::make_shared<DeviceGetAllPeriodTimelineResponse>();
        auto calendarSearchResponse = std::static_pointer_cast<DeviceCalendarSearchResponse>(response);

        if(calendarSearchResponse->errorCode != Wisenet::ErrorCode::NoError)
        {
            // 실패 응답
            finalResponse->errorCode = calendarSearchResponse->errorCode;
            responseHandler(finalResponse);
            return;
        }

        // Response에 Recording Period 저장
        finalResponse->recordingPeriod = calendarSearchResponse->result;

        // Overlapped Id 요청
        getAllPeriodOverlappedId(request, responseHandler, finalResponse);
    }, nullptr);
    */
}

void SunapiDeviceClientImpl::getAllOverlappedIdTimeline(DeviceGetTimelineRequestSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler)
{
    // OverlappedId 획득 Request 생성
    DeviceGetOverlappedIdRequestSharedPtr overlappedIdRequest = std::make_shared<DeviceGetOverlappedIdRequest>();
    for(auto& channelId : request->param.ChannelIDList)
    {
        overlappedIdRequest->param.ChannelIDList.push_back(channelId);
    }
    overlappedIdRequest->param.FromDate = request->param.FromDate;
    overlappedIdRequest->param.ToDate = request->param.ToDate;

    auto overlappedIdView = std::make_shared<RecordingOverlappedView>(m_logPrefix, overlappedIdRequest);

    // OverlappedId 획득 Request
    asyncRequest(m_httpCommandSession,
                 overlappedIdView,
                 [this, request, responseHandler](const ResponseBaseSharedPtr& response)
    {
        DeviceGetTimelineResponseSharedPtr finalResponse = std::make_shared<DeviceGetTimelineResponse>();
        DeviceGetOverlappedIdResponseSharedPtr overlappedIdResponse = std::static_pointer_cast<DeviceGetOverlappedIdResponse>(response);

        if(overlappedIdResponse->errorCode != Wisenet::ErrorCode::NoError)
        {
            // 실패 응답
            finalResponse->errorCode = overlappedIdResponse->errorCode;
            responseHandler(finalResponse);
            return;
        }

        // Overlapped Id가 하나도 없으면 각 채널 별로 빈 결과를 생성하여 리턴
        if(overlappedIdResponse->result.overlappedIDList.size() == 0)
        {
            for(auto& channelId : request->param.ChannelIDList)
            {
                std::unordered_map<int, std::vector<ChannelTimeline>> trackResultMap;
                std::vector<ChannelTimeline> channelTimelineList;
                trackResultMap[-1] = channelTimelineList;
                finalResponse->result.channelTimelineList[channelId] = trackResultMap;
            }

            responseHandler(finalResponse);
            return;
        }

        // Overlapped Id 별 Timeline 획득 요청
        for(auto& overlappedId : overlappedIdResponse->result.overlappedIDList) {
            if(overlappedId == -1)
                continue;   // overlappedId가 -1일 수 없으나, 무한루프 방지용 코드.

            DeviceGetTimelineRequestSharedPtr timelineRequest = std::make_shared<DeviceGetTimelineRequest>();
            timelineRequest->param = request->param;
            timelineRequest->param.OverlappedID = overlappedId;
            DeviceGetTimeline(timelineRequest, responseHandler);
        }
    }, nullptr,AsyncRequestType::HTTPGET,false);
}


}
}

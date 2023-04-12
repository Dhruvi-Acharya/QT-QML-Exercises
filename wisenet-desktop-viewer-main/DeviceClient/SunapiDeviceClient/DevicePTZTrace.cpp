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

class PTZConfigTraceView : public BaseCommand, public JsonParser
{
public:
    explicit PTZConfigTraceView(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetTraceResponse>(),false,false),
         m_request(request)
    {
        SPDLOG_DEBUG("DevicePTZTrace::PTZConfigTraceView()");
    }
    std::string RequestUri() override {
        SPDLOG_DEBUG("DevicePTZTrace::PTZConfigTraceView::RequestUri()");
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "trace", "view")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        SPDLOG_DEBUG("DevicePTZTrace::PTZConfigTraceView::ProcessPost()");
        auto response = std::static_pointer_cast<DeviceGetTraceResponse>(m_responseBase);
        response->tracePresets = parseResult;

        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

   std::vector<int> parseResult; // preset list of trace 1 ~ 6
private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        SPDLOG_DEBUG("DevicePTZTrace::PTZConfigTraceView::parseJson()");
        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("PTZTraces");
        if (itr != jsonDoc.MemberEnd())
        {
            auto & results = itr->value;
            for(auto & result : results.GetArray())
            {
                auto tracesResult = result.FindMember("Traces");

                if(tracesResult != result.MemberEnd())
                {
                    auto & resultTraceList = tracesResult->value;
                    for(auto &resultTrace : resultTraceList.GetArray())
                    {
                        auto traceResult = resultTrace.FindMember("Trace");
                        if(traceResult != resultTrace.MemberEnd())
                        {
                            parseResult.push_back(traceResult->value.GetInt());
                            SPDLOG_DEBUG("PTZConfigTraceView::parseJson() trace={}", traceResult->value.GetInt());
                        }
                    }
                }
            }
        }

        return true;
    }

    ChannelRequestBaseSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetTrace(ChannelRequestBaseSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("DevicePTZTrace::DeviceGetTrace() start");
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigTraceView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
    SPDLOG_DEBUG("DevicePTZTrace::DeviceGetTrace() end");
}

// move to trace
class PTZControlMoveTrace : public BaseCommand
{
public:
    explicit PTZControlMoveTrace(const std::string& logPrefix, DeviceMoveTraceRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "trace", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Trace", m_request->traceNumber)
                .AddParameter("Mode", "Start");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceMoveTraceRequestSharedPtr m_request;
};

// move to trace
void SunapiDeviceClientImpl::DeviceMoveTrace(DeviceMoveTraceRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlMoveTrace>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

// stop this position
class PTZControlStopTrace : public BaseCommand
{
public:
    explicit PTZControlStopTrace(const std::string& logPrefix, DeviceStopTraceRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "trace", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Trace", m_request->traceNumber)
                .AddParameter("Mode", "Stop");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceStopTraceRequestSharedPtr m_request;
};

// stop this position
void SunapiDeviceClientImpl::DeviceStopTrace(DeviceStopTraceRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlStopTrace>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}

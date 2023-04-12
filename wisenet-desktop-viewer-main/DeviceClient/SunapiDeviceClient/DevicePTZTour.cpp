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

class PTZConfigTourView : public BaseCommand, public JsonParser
{
public:
    explicit PTZConfigTourView(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetTourResponse>(),false,false),
         m_request(request)
    {
        SPDLOG_DEBUG("DevicePTZTour::PTZConfigTourView()");
    }
    std::string RequestUri() override {
        SPDLOG_DEBUG("DevicePTZTour::PTZConfigTourView::RequestUri()");
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "tour", "view")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        SPDLOG_DEBUG("DevicePTZTour::PTZConfigTourView::ProcessPost()");
        auto response = std::static_pointer_cast<DeviceGetTourResponse>(m_responseBase);
        response->tourPresets = parseResult;

        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

   std::vector<int> parseResult; // preset list of tour 1 ~ 6
private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        SPDLOG_DEBUG("DevicePTZTour::PTZConfigTourView::parseJson()");
        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("PTZTours");
        if (itr != jsonDoc.MemberEnd())
        {
            auto & results = itr->value;
            for(auto & result : results.GetArray())
            {
                auto toursResult = result.FindMember("Tours");

                if(toursResult != result.MemberEnd())
                {
                    auto & resultTourList = toursResult->value;
                    for(auto &resultTour : resultTourList.GetArray())
                    {
                        auto presetSeqResult = resultTour.FindMember("GroupSequences");
                        if(presetSeqResult != resultTour.MemberEnd())
                        {
                            auto & resultTourPresetSeq = presetSeqResult->value;

                            for(auto &resultTourPreset : resultTourPresetSeq.GetArray())
                            {
                                auto presetNumberResult = resultTourPreset.FindMember("Group");
                                if(presetNumberResult != resultTourPreset.MemberEnd())
                                {
                                    parseResult.push_back(presetNumberResult->value.GetInt());
                                    SPDLOG_DEBUG("PTZConfigTourView::parseJson() group={}", presetNumberResult->value.GetInt());
                                }
                            }
                        }
                    }
                }
            }
        }

        return true;
    }

    ChannelRequestBaseSharedPtr m_request;
};

void SunapiDeviceClientImpl::DeviceGetTour(ChannelRequestBaseSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("DevicePTZTour::DeviceGetTour() start");
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigTourView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
    SPDLOG_DEBUG("DevicePTZTour::DeviceGetTour() end");
}

// move to tour
class PTZControlMoveTour : public BaseCommand
{
public:
    explicit PTZControlMoveTour(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "tour", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Tour", 1)
                .AddParameter("Mode", "Start");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    ChannelRequestBaseSharedPtr m_request;
};

// move to tour
void SunapiDeviceClientImpl::DeviceMoveTour(ChannelRequestBaseSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlMoveTour>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

// stop this position
class PTZControlStopTour : public BaseCommand
{
public:
    explicit PTZControlStopTour(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
          m_request(request)
    {
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "tour", "control")
                .AddParameter("Channel", toSunapiChannel(m_request->channelID))
                .AddParameter("Tour", 1)
                .AddParameter("Mode", "Stop");

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    ChannelRequestBaseSharedPtr m_request;
};

// stop this position
void SunapiDeviceClientImpl::DeviceStopTour(ChannelRequestBaseSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlStopTour>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}

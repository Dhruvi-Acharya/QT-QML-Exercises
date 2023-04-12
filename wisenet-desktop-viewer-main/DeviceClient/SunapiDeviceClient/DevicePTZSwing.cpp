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

class PTZConfigSwingView : public BaseCommand, public JsonParser
{
public:
    explicit PTZConfigSwingView(const std::string& logPrefix, ChannelRequestBaseSharedPtr request)
        :BaseCommand(this,logPrefix,std::make_shared<DeviceGetSwingResponse>(),false,false),
         m_request(request)
    {
        SPDLOG_DEBUG("DevicePTZSwing::PTZConfigSwingView()");
    }
    std::string RequestUri() override {
        SPDLOG_DEBUG("DevicePTZSwing::PTZConfigSwingView::RequestUri()");
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzconfig", "swing", "view")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID));

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override {
        SPDLOG_DEBUG("DevicePTZSwing::PTZConfigSwingView::ProcessPost()");
        auto response = std::static_pointer_cast<DeviceGetSwingResponse>(m_responseBase);
        response->swingPanStartPreset = m_swingPanStartPreset;
        response->swingPanEndPreset = m_swingPanEndPreset;
        response->swingTiltStartPreset = m_swingTiltStartPreset;
        response->swingTiltEndPreset = m_swingTiltEndPreset;
        response->swingPanTiltStartPreset = m_swingPanTiltStartPreset;
        response->swingPanTiltEndPreset = m_swingPanTiltEndPreset;
        response->errorCode = Wisenet::ErrorCode::NoError;
        return response;
    }

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("PTZSwing");
        if (itr != jsonDoc.MemberEnd())
        {
            auto & results = itr->value;
            for(auto & result : results.GetArray())
            {
                auto itrResult = result.FindMember("SwingSequence");

                if(itrResult != result.MemberEnd())
                {
                    auto & resultSwingSeq = itrResult->value;
                    for(auto &resultSwing : resultSwingSeq.GetArray())
                    {
                        auto swingMode = resultSwing.FindMember("Mode");
                        if(swingMode != resultSwing.MemberEnd())
                        {
                            auto swingFromPreset = resultSwing.FindMember("FromPreset");
                            auto swingToPreset = resultSwing.FindMember("ToPreset");

                            if(swingFromPreset != resultSwing.MemberEnd() && swingToPreset != resultSwing.MemberEnd())
                            {
                                std::string swingModeString = swingMode->value.GetString();
                                int fromPreset = swingFromPreset->value.GetInt();
                                int toPreset = swingToPreset->value.GetInt();

                                SPDLOG_DEBUG("DevicePTZSwing::PTZConfigSwingView::parseJson() - swingModeString={} fromPreset={} toPreset={}", swingModeString, fromPreset, toPreset);

                                if(swingModeString == "Pan") {
                                    m_swingPanStartPreset = fromPreset;
                                    m_swingPanEndPreset = toPreset;
                                }
                                else if(swingModeString == "Tilt") {
                                    m_swingTiltStartPreset = fromPreset;
                                    m_swingTiltEndPreset = toPreset;
                                }
                                else if(swingModeString == "PanTilt") {
                                    m_swingPanTiltStartPreset = fromPreset;
                                    m_swingPanTiltEndPreset = toPreset;
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
    int m_swingPanStartPreset = 0;
    int m_swingPanEndPreset = 0;
    int m_swingTiltStartPreset = 0;
    int m_swingTiltEndPreset = 0;
    int m_swingPanTiltStartPreset = 0;
    int m_swingPanTiltEndPreset = 0;
};

void SunapiDeviceClientImpl::DeviceGetSwing(ChannelRequestBaseSharedPtr const& request,
                                         ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("DevicePTZSwing::DeviceGetSwing() start");
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZConfigSwingView>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
    SPDLOG_DEBUG("DevicePTZSwing::DeviceGetSwing() end");
}

class PTZControlMoveStopSwing : public BaseCommand
{
public:
    explicit PTZControlMoveStopSwing(const std::string& logPrefix, DeviceMoveStopSwingRequestSharedPtr request)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request)
    {
        swingActionMap[Wisenet::Device::SwingActionType::Pan] = "Pan";
        swingActionMap[Wisenet::Device::SwingActionType::Tilt] = "Tilt";
        swingActionMap[Wisenet::Device::SwingActionType::PanTilt] = "PanTilt";
        swingActionMap[Wisenet::Device::SwingActionType::Stop] = "Stop";
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;

        builder.CreateCgi("ptzcontrol", "swing", "control")
               .AddParameter("Channel", toSunapiChannel(m_request->channelID))
               .AddParameter("Mode", swingActionMap[m_request->swingActionType]);

        return builder.GetUrl();
    };

    ResponseBaseSharedPtr ProcessPost() override {
        m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }

private:
    DeviceMoveStopSwingRequestSharedPtr m_request;
    std::map <Wisenet::Device::SwingActionType, std::string> swingActionMap;
};

// move to preset or stop this position
void SunapiDeviceClientImpl::DeviceMoveStopSwing(DeviceMoveStopSwingRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    asyncRequest(m_httpPtzSession,
                 std::make_shared<PTZControlMoveStopSwing>(m_logPrefix,request),
                 responseHandler,
                 nullptr,AsyncRequestType::HTTPGET,false);
}

}
}

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

void SunapiDeviceClientImpl::DeviceTalk( DeviceTalkRequestSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler )
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        std::string channel = request->channelID;
        auto currentChannelTalkClientItor = m_TalkClientMap.find(channel);

        // Talk Start
        if(request->talkStart)
        {
            if(currentChannelTalkClientItor != m_TalkClientMap.end())
            {
                // 이미 Talk 중이면 성공 응답
                auto response = std::make_shared<ResponseBase>();
                responseHandler(response);
                return;
            }

            // Talk client 생성
            Rtsp::RtspClientPtr talkClient = std::make_shared<Rtsp::RtspClient>(m_streamIoc);
            m_TalkClientMap.emplace(channel, talkClient);

            DeviceType deviceType = m_repos->device().deviceType;
            std::string rtspUrl = "";
            MediaRequestInfo mediaRequest = {}; // 2023.01.11. coverity (ubuntu)
            mediaRequest.streamType = StreamType::liveHigh; // live rtsp url을 사용.
            mediaRequest.profileId = "1";

            if(deviceType == DeviceType::SunapiCamera)
                rtspUrl = getCameraRtspUrl(mediaRequest, Rtsp::TransportMethod::RTP_RTSP, channel);
            else if(deviceType == DeviceType::SunapiRecorder)
                rtspUrl = getNvrRtspUrl(mediaRequest, Rtsp::TransportMethod::RTP_RTSP, channel);

            // Talk client 초기화
            talkClient->Initialize( Rtsp::RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL,
                                    rtspUrl,
                                    Rtsp::TransportMethod::RTP_RTSP,
                                    m_connectionInfo.user,
                                    m_connectionInfo.password,
                                    Rtsp::kDefaultRtspConnectionTimeout);

            // Setup BackChannel

            talkClient->SetupBackChannel( WeakCallback(shared_from_this(), [this, channel, talkClient, responseHandler](const Rtsp::RtspResponseSharedPtr& rtspResponse)
            {
                boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, channel, talkClient, responseHandler, rtspResponse]()
                {
                    auto response = std::make_shared<ResponseBase>();
                    response->errorCode = rtspResponse->IsSuccess ? Wisenet::ErrorCode::NoError
                                                                  : convertRtspStatusToErrorcode(rtspResponse->StatusCode);
                    if(rtspResponse->StatusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_ServiceUnavailable)
                        response->errorCode = Wisenet::ErrorCode::UserFull;

                    if(rtspResponse->IsSuccess){
                        boost::optional<Rtsp::RtspPlaybackParam> playParam;
                        talkClient->Play(playParam,
                                         [=](const Rtsp::RtspResponseSharedPtr& rtspResponse)
                        {
                            response->errorCode = rtspResponse->IsSuccess ? Wisenet::ErrorCode::NoError
                                                                            : convertRtspStatusToErrorcode(rtspResponse->StatusCode);
                            responseHandler(response);
                            if (!rtspResponse->IsSuccess){
                                SPDLOG_INFO("DeviceTalk() Play failed");
                                talkClient->Close();
                                m_TalkClientMap[channel] = nullptr;
                                m_TalkClientMap.erase(channel);
                            }
                        });
                    }
                    else
                    {
                        responseHandler(response);  // response to CoreService

                        // 실패 시 정리
                        SPDLOG_INFO("DeviceTalk() Setup failed");
                        talkClient->Close();
                        m_TalkClientMap[channel] = nullptr;
                        m_TalkClientMap.erase(channel);
                    }
                }));
            }));
        }
        // Talk Stop
        else
        {
            if(currentChannelTalkClientItor != m_TalkClientMap.end())
            {
                // Talk Client 정리
                currentChannelTalkClientItor->second->Close();
                m_TalkClientMap[channel] = nullptr;
                m_TalkClientMap.erase(channel);
            }

            auto response = std::make_shared<ResponseBase>();
            responseHandler(response);
        }

    }));
}

void SunapiDeviceClientImpl::DeviceSendTalkData( DeviceSendTalkDataRequestSharedPtr const& request )
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request]()
    {
        std::string channel = request->channelID;
        auto currentChannelTalkClientItor = m_TalkClientMap.find(channel);

        if(currentChannelTalkClientItor == m_TalkClientMap.end())
            return; // 해당 Channel에 TalkClient가 없으면 리턴

        currentChannelTalkClientItor->second->SendBackChannelData(
                    request->audioFrame->getDataPtr(),
                    request->audioFrame->getDataSize());
    }));
}


}
}

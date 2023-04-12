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

#include "MediaAgent.h"
#include "LogSettings.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[M-AGENT] "}, level, __VA_ARGS__)

namespace Wisenet
{
namespace Device
{


MediaAgent::MediaAgent(boost::asio::io_context& ioc,
                       std::string& deviceID,
                       std::string& channelID,
                       Wisenet::Device::StreamType streamType,
                       std::string& profileId,
                       std::string playbackSessionId)
    : m_rtspClient(std::make_shared<Wisenet::Rtsp::RtspClient>(ioc))
    , m_deviceID(deviceID)
    , m_channelID(channelID)
    , m_streamType(streamType)
    , m_profileId(profileId)
    , m_playbackSessionId(playbackSessionId)
    , m_transportMethod(Rtsp::TransportMethod::RTP_RTSP)
    , m_lastControlType(ControlType::pause)
    , m_lastControlTime(0)
{
    SPDLOG_DEBUG("MediaAgent() new instance :: deviceID={}, channelID={}, profileId={}, streamType={}, sessionID={}",
                 m_deviceID, m_channelID, m_profileId, m_streamType, m_playbackSessionId);
}

void MediaAgent::close()
{
    SPDLOG_DEBUG("close() :: deviceID={}, channelID={}, profileId={}, streamType={}, sessionID={}",
                 m_deviceID, m_channelID, m_profileId, m_streamType, m_playbackSessionId);

    ClearMediaStreamHandler();

    if (m_rtspClient) {
        m_rtspClient->Close();
    }
}

MediaAgent::~MediaAgent()
{
    SPDLOG_DEBUG("~MediaAgent() destroy instance :: deviceID={}, channelID={}, profileId={}, streamType={}, sessionID={}",
                 m_deviceID, m_channelID, m_profileId, m_streamType, m_playbackSessionId);

}

const Wisenet::Rtsp::RtspClientPtr MediaAgent::GetRtspClient()
{
    return m_rtspClient;
}

void MediaAgent::SetMediaDisconnectedHandler(const MediaDisconnectedHandler &disconnHandler)
{
    m_disconnectedHandler = disconnHandler;
}

bool MediaAgent::IsSameMedia(const std::string& channelID,
                             const Wisenet::Device::StreamType streamType,
                             const std::string& profileId,
                             const std::string& playbackSessionId)
{
    // channelID, streamType, profilId, playbackSessionId가 모두 같은 MediaAgent인지 리턴
    return m_channelID == channelID
            && m_streamType == streamType
            && m_profileId == profileId
            && m_playbackSessionId == playbackSessionId;
}

void MediaAgent::AddMediaStreamHandler(std::string mediaUUID, Wisenet::Media::MediaSourceFrameHandler& mediaStreamHandler)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    if(m_mediaStreamHandlerMap.find(mediaUUID) != m_mediaStreamHandlerMap.end())
    {
        // 동일한 mediaUUID의 callback이 이미 있으면 제거
        m_mediaStreamHandlerMap[mediaUUID] = nullptr;
        m_mediaStreamHandlerMap.erase(mediaUUID);
    }

    SPDLOG_DEBUG("AddMediaStreamHandler() deviceID={}, channelID={}, profileID={}, mediaUUID={}, sessionID={}",
                 m_deviceID, m_channelID, m_profileId, mediaUUID, m_playbackSessionId);
    m_mediaStreamHandlerMap.emplace(mediaUUID, mediaStreamHandler); // callback 함수 추가
}

void MediaAgent::RtspClientInitPlay(const Rtsp::RtspPlayControlType rtspPlayControlType,
                                    const Rtsp::TransportMethod transportMethod,
                                    const std::string rtspUrl,
                                    const std::string &userName,
                                    const std::string &password,
                                    const bool supportQuickPlay,
                                    const boost::optional<Rtsp::RtspPlaybackParam> playParam,
                                    Rtsp::RtspResponseHandler const& responseHandler)
{
    m_transportMethod = transportMethod;

    SPDLOG_DEBUG("RtspClientInitPlay supportQuickPlay={} transportMethod={}",
                 supportQuickPlay, (int)transportMethod);

    // Initialize
    m_rtspClient->Initialize(rtspPlayControlType,
                             rtspUrl,
                             transportMethod,
                             userName,
                             password,
                             Rtsp::kDefaultRtspConnectionTimeout);

    // Set Callback
    m_rtspClient->SetIntermediateCallback(// event callback
                                          WeakCallback(shared_from_this(),
                                                       [=](const Rtsp::RtspEventSharedPtr& e)
    {
        SPDLOG_DEBUG("RTSP EVENT OCCURRED :: deviceID={}, channelID={}, profileID={}, sessionID={}, eventType={}, url={}",
                     m_deviceID, m_channelID, m_profileId, m_playbackSessionId,
                     e->EventType, rtspUrl);
        if (e->EventType == Rtsp::RtspEventType::SOCKET_CLOSED) {
            OnDisconnected();
        }
    }),

                                          // media callback
                                          WeakCallback(shared_from_this(),
                                                       [this](const Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        CallbackMediaStream(mediaSourceFrame);
    }));

    // Setup
    Rtsp::RtspSetupParam setupParam;

    if (rtspUrl.find("PosPlayback") != std::string::npos || rtspUrl.find("PosBackup") != std::string::npos) {
        setupParam.AudioOn = false;
        setupParam.VideoOn = false;
        setupParam.OnvifMetaDataOn = true;
    }
    else {
        setupParam.AudioOn = true;
        setupParam.VideoOn = true;
        setupParam.OnvifMetaDataOn = false;
    }

    setupParam.MetaImageOn = false;

    if(supportQuickPlay)
    {
        m_rtspClient->QuickPlay(
                    setupParam,
                    playParam,
                    WeakCallback(shared_from_this(),
                                 [responseHandler](const Rtsp::RtspResponseSharedPtr& quickPlayResponse)
        {
            SPDLOG_DEBUG("MediaAgent::RtspClientInitPlay support quickplay success={}", quickPlayResponse->IsSuccess);
            responseHandler(quickPlayResponse);
        }));
    }
    else
    {
        m_rtspClient->Setup(
                    setupParam,
                    WeakCallback(shared_from_this(),
                                 [this, responseHandler, playParam](const Rtsp::RtspResponseSharedPtr& setupResponse)
        {
            if (!setupResponse->IsSuccess)
            {
                responseHandler(setupResponse);
            }
            else
            {
                // Play
                m_rtspClient->Play(
                            playParam,
                            WeakCallback(shared_from_this(),
                                         [responseHandler](const Rtsp::RtspResponseSharedPtr& playResponse)
                {
                    responseHandler(playResponse);
                }));
            }
        }));
    }
}

void MediaAgent::CallbackMediaStream(const Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    for(auto& mediaStreamHandler : m_mediaStreamHandlerMap)
    {
        mediaStreamHandler.second(mediaSourceFrame);
    }
}

void MediaAgent::OnDisconnected()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    if (m_disconnectedHandler) {
        for(auto& mediaStreamHandler : m_mediaStreamHandlerMap) {
            std::string mediaID = mediaStreamHandler.first;
            m_disconnectedHandler(mediaID);
        }
    }
}

void MediaAgent::RemoveMediaStreamHandler(const std::string mediaUUID, bool& mediaAgentRemoved)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    mediaAgentRemoved = false;

    if(m_mediaStreamHandlerMap.find(mediaUUID) != m_mediaStreamHandlerMap.end())
    {
        // 동일한 mediaUUID의 callback이 이미 있으면 제거
        SPDLOG_DEBUG("RemoveMediaStreamHandler() deviceID={}, channelID={}, profileID={}, sessionID={}, mediaUUID={}",
                     m_deviceID, m_channelID, m_profileId, m_playbackSessionId, mediaUUID);
        m_mediaStreamHandlerMap[mediaUUID] = nullptr;
        m_mediaStreamHandlerMap.erase(mediaUUID);
    }

    if(m_mediaStreamHandlerMap.size() == 0)
    {
        mediaAgentRemoved = true;
    }
}

void MediaAgent::ClearMediaStreamHandler()
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);
    m_mediaStreamHandlerMap.clear();
    SPDLOG_DEBUG("ClearMediaStreamHandler(), deviceID={}, channelID={}, profileID={}, sessionID={}",
                 m_deviceID, m_channelID, m_profileId, m_playbackSessionId);
}

void MediaAgent::AddMediaOpenResponseHandler(ResponseBaseHandler const& responseHandler)
{
    m_responseHandlerList.push_back(responseHandler);
}

int MediaAgent::GetMediaOpenResponseHandlerCount()
{
    return m_responseHandlerList.size();
}

void MediaAgent::CallbackMediaOpenResponse(Wisenet::ErrorCode errorCode)
{
    // 대기 중인 모든 responseHandler에 응답 전달
    auto response = std::make_shared<ResponseBase>();
    response->errorCode = errorCode;

    for(auto& responseHandler : m_responseHandlerList)
    {
        responseHandler(response);
    }

    m_responseHandlerList.clear();
}

bool MediaAgent::CheckMediaUUID(const std::string& mediaUUID)
{
    std::lock_guard<std::recursive_mutex> lock(m_mutex);

    if(m_mediaStreamHandlerMap.find(mediaUUID) != m_mediaStreamHandlerMap.end())
    {
        // 동일한 mediaUUID의 callback이 있으면 True 리턴
        return true;
    }

    return false;
}

void MediaAgent::RtspClientPlayControl(const ControlType controlType,
                                       Rtsp::RtspPlaybackParam playParam,
                                       Rtsp::RtspResponseHandler const& responseHandler)
{
    if(controlType == ControlType::pause)
    {
        SPDLOG_DEBUG("RtspClientPlayControl()::PAUSE, deviceID={}, channelID={}, profileID={}, sessionID={}",
                     m_deviceID, m_channelID, m_profileId, m_playbackSessionId);

        // Send Rtsp Pause
        m_rtspClient->Pause(
                    WeakCallback(shared_from_this(),
                                 [responseHandler](const Rtsp::RtspResponseSharedPtr& pauseResponse)
        {
            responseHandler(pauseResponse);
        }));
    }
    else
    {
        SPDLOG_DEBUG("RtspClientPlayControl()::PLAY, deviceID={}, channelID={}, profileID={}, sessionID={}",
                     m_deviceID, m_channelID, m_profileId, m_playbackSessionId);
        if(controlType == m_lastControlType)
        {
            // 이전과 동일한 Step 명령이 내려오면 마지막 시간에서 1초 보정
            if(controlType == ControlType::stepForward)
            {
                m_lastControlTime++;
                playParam.StartTime = m_lastControlTime;
            }
            else if(controlType == ControlType::stepBackward)
            {
                m_lastControlTime--;
                playParam.StartTime = m_lastControlTime;
            }
        }

        // Send Rtsp Play
        m_rtspClient->Play(
                    playParam,
                    WeakCallback(shared_from_this(),
                                 [responseHandler](const Rtsp::RtspResponseSharedPtr& playResponse)
        {
            responseHandler(playResponse);
        }));
    }

    m_lastControlType = controlType;
    m_lastControlTime = playParam.StartTime;
}

bool MediaAgent::IsPlayback()
{
    return (m_streamType == StreamType::playbackHigh || m_streamType == StreamType::playbackLow);
}


}
}

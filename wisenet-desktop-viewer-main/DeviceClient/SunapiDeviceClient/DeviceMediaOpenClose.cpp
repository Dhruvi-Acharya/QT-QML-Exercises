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

void SunapiDeviceClientImpl::DeviceMediaOpen( DeviceMediaOpenRequestSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler )
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {      
        MediaRequestInfo mediaRequest = request->mediaRequestInfo;
        std::string channelID = request->channelID;

        if(m_isFwupdating){
            SLOG_DEBUG("DeviceMediaOpen() FwupdateMediaError channelID={}", channelID);
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::FwupdateMediaError;
            responseHandler(response);
            return;
        }

        SLOG_DEBUG("DeviceMediaOpen(), channelID={}, mediaID={}, profileID={}, sessionID={}, isLive={}, isBackup={}, isPosPlayback={}",
                   request->channelID, request->mediaID,
                   mediaRequest.profileId, mediaRequest.playbackSessionId,
                   mediaRequest.isLive(), mediaRequest.isBackup(), mediaRequest.isPosPlayback());

        for(auto& mediaAgent : m_mediaAgentList)
        {
            bool sameMediaExist = mediaAgent->IsSameMedia(channelID,
                                                          mediaRequest.streamType,
                                                          mediaRequest.profileId,
                                                          mediaRequest.playbackSessionId);

            // 2022.10.17. 플레이백과 같은 세션 아이디가 요청되어도 PosPlayback의 경우에는 제외하도록 한다.
            if(sameMediaExist && !mediaRequest.isPosPlayback() /* PosPlayback */ )
            {
                // 동일한 MediaAgent가 이미 있으면 callback 함수만 추가
                mediaAgent->AddMediaStreamHandler(request->mediaID, mediaRequest.streamHandler);

                if(mediaAgent->GetMediaOpenResponseHandlerCount() == 0)
                {
                    // 응답 대기 중인 요청이 없으면 바로 성공 응답
                    auto response = std::make_shared<ResponseBase>();
                    responseHandler(response);
                }
                else
                {
                    // 응답 대기 중인 요청이 있으면 ResponseHandler만 추가
                    mediaAgent->AddMediaOpenResponseHandler(responseHandler);
                }

                return;
            }
        }

        bool isSessionFull = false;

        // NVR sessionKey 획득 실패하면 MediaOpen 실패 처리
        if(m_repos->device().deviceType == DeviceType::SunapiRecorder
                && getSessionKey(mediaRequest.streamType, mediaRequest.playbackSessionId) == "")
        {
            SLOG_DEBUG("DeviceMediaOpen() SessionFull getSessionKey() failed");
            isSessionFull = true;
        }

        // Playback 요청이고, maxPlaybackChannels 초과 시 실패 처리
        if(mediaRequest.isPlayback())
        {
            int maxPlaybackChannels = m_repos->attributes().recordingAttribute.maxPlaybackChannels;
            int playbackChannelCount = getPlaybackChannelCount();

            if(playbackChannelCount >= maxPlaybackChannels)
            {
                SLOG_DEBUG("DeviceMediaOpen() SessionFull maxPlaybackChannels={} playbackChannelCount={}", maxPlaybackChannels, playbackChannelCount);
                isSessionFull = true;
            }
        }

        if(isSessionFull)
        {
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::UserFull;
            responseHandler(response);
            return;
        }

        // 신규 MediaAgent 생성
        MediaAgentPtr mediaAgent = std::make_shared<MediaAgent>(
                    m_streamIoc,
                    m_deviceUUID,
                    channelID,
                    mediaRequest.streamType,
                    mediaRequest.profileId,
                    mediaRequest.playbackSessionId);

        mediaAgent->SetMediaDisconnectedHandler(WeakCallback(shared_from_this(), [this, channelID](const std::string& mediaUUID) {
            SLOG_DEBUG("MEDIA Streaming Disconnected, channelID={}, mediaID={}", channelID, mediaUUID);
            auto mediaDisconnectedEvent = std::make_shared<MediaDisconnectedEvent>();
            mediaDisconnectedEvent->deviceID = m_deviceUUID;
            mediaDisconnectedEvent->channelID = channelID;
            mediaDisconnectedEvent->mediaID = mediaUUID;
            asio::post(m_strand, WeakCallback(shared_from_this(), [this, mediaDisconnectedEvent]()
            {
                eventHandler(mediaDisconnectedEvent);
            }));
        }));

        mediaAgent->AddMediaStreamHandler(request->mediaID, mediaRequest.streamHandler);
        mediaAgent->AddMediaOpenResponseHandler(responseHandler);

        m_mediaAgentList.push_back(mediaAgent);


        // RtspClient 초기화, Setup, Play까지 수행
        rtspMediaOpen(mediaRequest, channelID, mediaAgent);
    }));
}

bool supportQuickPlayTransportMethod(Rtsp::TransportMethod type)
{
    if(type == Rtsp::TransportMethod::RTP_UDP_UNICAST || type == Rtsp::TransportMethod::RTP_UDP_MULTICAST)
        return false;
    return true;
}

void SunapiDeviceClientImpl::rtspMediaOpen(MediaRequestInfo mediaRequest, std::string& channel, MediaAgentPtr const& mediaAgent)
{
    DeviceType deviceType = m_repos->device().deviceType;
    Rtsp::RtspPlaybackParam playParam;

    Rtsp::RtspPlayControlType rtspPlayControlType = Rtsp::RtspPlayControlType::RTSP_PLAY_LIVE;
    if(mediaRequest.streamType == StreamType::playbackHigh
            || mediaRequest.streamType == StreamType::playbackLow
            || mediaRequest.streamType == StreamType::backupHigh
            || mediaRequest.streamType == StreamType::backupLow
            || mediaRequest.streamType == StreamType::posPlayback
            || mediaRequest.streamType == StreamType::posBackup)
    {
        if(deviceType == DeviceType::SunapiCamera)
            rtspPlayControlType = Rtsp::RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA;
        else if(deviceType == DeviceType::SunapiRecorder)
            rtspPlayControlType = Rtsp::RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR;

        playParam.StartTime = mediaRequest.startTime / 1000;
        playParam.EndTime = mediaRequest.endTime / 1000;
        playParam.Immidiate = false;
        if(mediaRequest.isPausedOpen) {
            playParam.Scale = Rtsp::RTSP_PLAY_PB_SUNAPI_NVR_F1;
        }
        else {
            playParam.Scale = mediaRequest.speed;
        }
        if(mediaRequest.streamType == StreamType::backupHigh || mediaRequest.streamType == StreamType::backupLow)
        {
            playParam.RateControl = false;
        }
    }

    Rtsp::TransportMethod transportMethod = Rtsp::TransportMethod::RTP_RTSP;    // 프로그램 설정값 연동 필요

    if (MediaProtocolType::RTP_UDP == m_connectionInfo.mediaProtocolType) {
        transportMethod = Rtsp::TransportMethod::RTP_UDP_UNICAST;
    }
    else if(MediaProtocolType::RTSP_MULTICAST == m_connectionInfo.mediaProtocolType) {
        transportMethod = Rtsp::TransportMethod::RTP_UDP_MULTICAST;
    }
    else if (MediaProtocolType::RTSP_HTTP == m_connectionInfo.mediaProtocolType) {
        if (m_connectionInfo.isSSL) {
            transportMethod = Rtsp::TransportMethod::RTP_RTSP_HTTPS;
        }
        else {
            transportMethod = Rtsp::TransportMethod::RTP_RTSP_HTTP;
        }
    }
    else {
        transportMethod = Rtsp::TransportMethod::RTP_RTSP;
    }

    std::string rtspUrl = "";
    if(deviceType == DeviceType::SunapiCamera)
        rtspUrl = getCameraRtspUrl(mediaRequest, transportMethod, channel);
    else if(deviceType == DeviceType::SunapiRecorder)
        rtspUrl = getNvrRtspUrl(mediaRequest, transportMethod, channel);

    std::string userName = m_connectionInfo.user;
    std::string password = m_connectionInfo.password;
    bool supportQuickPlay = m_repos->device().deviceCapabilities.quickPlay && supportQuickPlayTransportMethod(transportMethod);

    mediaAgent->RtspClientInitPlay(rtspPlayControlType,
                                   transportMethod,
                                   rtspUrl,
                                   userName,
                                   password,
                                   supportQuickPlay,
                                   playParam,
                                   WeakCallback(shared_from_this(), [this, mediaAgent](const Rtsp::RtspResponseSharedPtr& rtspResponse)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, mediaAgent, rtspResponse]()
        {
            Wisenet::ErrorCode errorCode = rtspResponse->IsSuccess ? Wisenet::ErrorCode::NoError
                                                                   : convertRtspStatusToErrorcode(rtspResponse->StatusCode);
            mediaAgent->CallbackMediaOpenResponse(errorCode);

            if (!rtspResponse->IsSuccess)
            {
                // RtspClient 초기화, Setup, Play 도중 실패하면 정리
                bool mediaAgentRemoved = false;
                int removedIndex = -1;
                std::string playbackSessionId = mediaAgent->GetPlaybackSessionId();

                for(auto& agent : m_mediaAgentList)
                {
                    removedIndex++;

                    if(agent == mediaAgent)
                    {
                        mediaAgentRemoved = true;
                        agent = nullptr;
                        break;
                    }
                }

                if(mediaAgentRemoved)
                {
                    SLOG_DEBUG("DeviceMediaOpen() MediaAgent Removed");
                    mediaAgent->close();
                    m_mediaAgentList.erase(m_mediaAgentList.begin()+removedIndex);
                    checkSessionKeyRelease(playbackSessionId);
                }
            }
        }));
    }));
}

std::string SunapiDeviceClientImpl::getCameraRtspUrl(MediaRequestInfo mediaRequest,
                                                 Rtsp::TransportMethod transportMethod,
                                                 std::string& channel)
{
    /*
     * LIVE
     *  (1) rtsp://<Device IP>/profile<no>/media.smp
     *  (2) rtsp://<Device IP>/multicast/profile<no>/media.smp
     *
     * SEARCH, BACKUP
     *  (1) rtsp://<Device IP>/recording/play.smp
    */

    std::string url = "";

    StreamType streamType = mediaRequest.streamType;
    int channelIndex = toSunapiChannel(channel);

    const auto& conn = m_connectionInfo;
    if (transportMethod == Rtsp::TransportMethod::RTP_RTSP_HTTP)
        url = "http://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.port);
    else if(transportMethod == Rtsp::TransportMethod::RTP_RTSP_HTTPS)
        url = "https://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.sslPort);
    else
        url = "rtsp://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.rtspPort);

    if (streamType == StreamType::liveHigh || streamType == StreamType::liveLow)
    {
        if (m_repos->device().channels.size() > 1)
            url += "/" + std::to_string(channelIndex);    // 멀티 채널 카메라 조건 확인 필요. channelIndex를 0부터 시작하도록 변환.

        if (transportMethod == Rtsp::RTP_UDP_MULTICAST)
            url += "/multicast";

        url += "/profile" + mediaRequest.profileId + "/media.smp";
    }
    else if(streamType == StreamType::playbackHigh
            || streamType == StreamType::playbackLow
            || streamType == StreamType::backupHigh
            || streamType == StreamType::backupLow)
    {
        /* 멀티 채널 카메라 조건 확인 필요
        if (m_deviceInfo.IsMultiDirectionalCamera && (xnsBody->m_nMediaType == XNSE_MTYPE_SEARCH_NET || xnsBody->m_nMediaType == XNSE_MTYPE_BACKUP_NET))
            url += "/" + std::to_string(xnsBody->m_OID[1] - 1);
        */

        url += "/recording";

        if (mediaRequest.trackId > 0)
            url += "/OverlappedID=" + std::to_string(mediaRequest.trackId);

        url += "/play.smp";
    }

    return url;
}

std::string SunapiDeviceClientImpl::getNvrRtspUrl(MediaRequestInfo mediaRequest,
                                              Rtsp::TransportMethod transportMethod,
                                              std::string& channel)
{
    /*
    LIVE
    (1) rtsp://<Device IP>:558/LiveChannel/<chid>/media.smp
    (2) rtsp://<Device IP>:558/LiveChannel/<chid>/media.smp/session=<sid>
    (3) rtsp://<Device IP>:558/LiveChannel/<chid>/media.smp/multicast&session=<sid>

    SEARCH
    (1) rtsp://<Device IP>:558/PlaybackChannel/<chid>/media.smp/overlap=<id>&session=<sid>

    BACKUP
    (1) rtsp://<Device IP>:558/BackupChannel/<chid>/media.smp/overlap=<id>&session=<sid>
    */

    /*
    POS LIVE
    (1) rtsp://<Device IP>:558/PosLive/<chid>/media.smp
    (2) rtsp://<Device IP>:558/PosLive/<chid>/media.smp/session=<sid>
    (3) rtsp://<Device IP>:558/PosLive/<chid>/media.smp/multicast&session=<sid>

    POS SEARCH
    (1) rtsp://<Device IP>:558/PosPlayback/<chid>/media.smp/overlap=<id>&session=<sid>

    POS BACKUP
    (1) rtsp://<Device IP>:558/PosBackup/<chid>/media.smp/overlap=<id>&session=<sid>
    */

    std::string url = "";

    StreamType streamType = mediaRequest.streamType;
    int channelIndex = toSunapiChannel(channel);

    const auto& conn = m_connectionInfo;
    if (transportMethod == Rtsp::TransportMethod::RTP_RTSP_HTTP)
        url = "http://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.port);
    else if(transportMethod == Rtsp::TransportMethod::RTP_RTSP_HTTPS)
        url = "https://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.sslPort);
    else
        url = "rtsp://" + std::string(conn.ip) + ":" + boost::lexical_cast<std::string>(conn.rtspPort);

    if (streamType == StreamType::liveHigh || streamType == StreamType::liveLow)
    {
        // Live
        url += ("/LiveChannel/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");

        if (transportMethod == Rtsp::RTP_UDP_MULTICAST)
            url += "multicast&";

        if (!mediaRequest.profileId.empty())
            url += "profile=" + mediaRequest.profileId + "&";
    }
    else if (streamType == StreamType::playbackHigh || streamType == StreamType::playbackLow)
    {
        // Playback
        url += ("/PlaybackChannel/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");
    }
    else if (streamType == StreamType::backupHigh || streamType == StreamType::backupLow)
    {
        // Backup
        url += ("/BackupChannel/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");
    }
    else if (streamType == StreamType::posLive)
    {
        // POS LIVE
        url += ("/PosLive/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");

        if (transportMethod == Rtsp::RTP_UDP_MULTICAST)
            url += "multicast&";
    }
    else if (streamType == StreamType::posPlayback)
    {
        // POS SEARCH
        url += ("/PosPlayback/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");
    }
    else if (streamType == StreamType::posBackup)
    {
        // POS BACKUP
        url += ("/PosBackup/" + boost::lexical_cast<std::string>(channelIndex) + "/media.smp/");
    }

    std::string sessionKey = getSessionKey(streamType, mediaRequest.playbackSessionId);
    if (sessionKey != "")
    {
        url += "session=" + boost::lexical_cast<std::string>(sessionKey) + "&";

        if (mediaRequest.trackId >= 0)
            url += "overlap=" + boost::lexical_cast<std::string>(mediaRequest.trackId) + "&";

        if ((streamType == StreamType::playbackLow || streamType == StreamType::backupLow)
                && m_repos->attributes().recordingAttribute.dualTrackRecording)
        {
            url += "substream&";
        }
    }

    url.pop_back();
    SLOG_DEBUG("RTSP URL for NVR : {}", url);
    return url;
}

Wisenet::ErrorCode SunapiDeviceClientImpl::convertRtspStatusToErrorcode(unsigned short statusCode)
{
    if(statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_OK)
        return Wisenet::ErrorCode::NoError;
    else if(statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_UserOperationWasTimedout)
        return Wisenet::ErrorCode::NoResponse;
    else if(statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_Unauthorized ||
            statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_Forbidden) {
        return Wisenet::ErrorCode::PermissionDenied;
    }
    else if (statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_MaxConcurrentStreamsForCodecReached ||
             statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_MaxConcurrentStreamsReached ||
             statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_MaxUserHasExceeded) {
        return Wisenet::ErrorCode::UserFull;
    }
    else if (statusCode == Wisenet::Rtsp::ResponseStatusCode::RTSP_UserConnectionWasClosed) {
        return Wisenet::ErrorCode::NetworkError;
    }
    return Wisenet::ErrorCode::NetworkError;
}

std::string SunapiDeviceClientImpl::getSessionKey(StreamType streamType, std::string playbackSessionId)
{
    const std::vector<DevcieSessionKey>& sessionKeyList = m_repos->sessionKeys();
    int sessionKeyCount = sessionKeyList.size();
    int keyIndex = -1;
    int emptyKeyIndex = -1;

    if(sessionKeyCount < 5)
        return "";  // 장비에서 획득한 sessionkey가 없으면 리턴

    if(streamType == StreamType::liveLow || streamType == StreamType::liveHigh)
    {
        // live는 첫 번째 sessionkey 반환
        keyIndex = 0;
    }
    else if(streamType == StreamType::backupHigh || streamType == StreamType::backupLow)
    {
        // backup은 마지막 sessionkey 반환
        keyIndex = sessionKeyCount - 1;
    }
    else if(streamType == StreamType::playbackHigh || streamType == StreamType::playbackLow)
    {
        for(int i=1 ; i<sessionKeyCount - 1 ; i++)
        {
            if(sessionKeyList[i].playbackSessionId == playbackSessionId)
                keyIndex = i;   // 동일한 playbackSessionId에 할당 된 sessionKey가 있는 경우
            else if(sessionKeyList[i].playbackSessionId == "")
                emptyKeyIndex = i;  // 할당 되지 않은 sessionKey index
        }

        if(keyIndex == -1 && emptyKeyIndex >= 0)
        {
            // 동일한 playbackSessionId에 할당 된 sessionKey가 없으면 비어있는 sessionKey 할당
            keyIndex = emptyKeyIndex;
            DevcieSessionKey deviceSessionKey(sessionKeyList[keyIndex].sessionKey, playbackSessionId);
            m_repos->setSessionKey(deviceSessionKey);
        }
    }
    else if (streamType == StreamType::posLive)
    {
        keyIndex = 0;
    }
    else if (streamType == StreamType::posPlayback)
    {
        for(int i=1 ; i<sessionKeyCount - 1 ; i++)
        {
            if(sessionKeyList[i].playbackSessionId == playbackSessionId)
                keyIndex = i;   // 동일한 playbackSessionId에 할당 된 sessionKey가 있는 경우
            else if(sessionKeyList[i].playbackSessionId == "")
                emptyKeyIndex = i;  // 할당 되지 않은 sessionKey index
        }

        if(keyIndex == -1 && emptyKeyIndex >= 0)
        {
            // 동일한 playbackSessionId에 할당 된 sessionKey가 없으면 비어있는 sessionKey 할당
            keyIndex = emptyKeyIndex;
            DevcieSessionKey deviceSessionKey(sessionKeyList[keyIndex].sessionKey, playbackSessionId);
            m_repos->setSessionKey(deviceSessionKey);
        }
    }
    else if (streamType == StreamType::posBackup)
    {
        ;
    }

    if(keyIndex >= 0)
        return sessionKeyList[keyIndex].sessionKey;
    else
        return "";
}

void SunapiDeviceClientImpl::checkSessionKeyRelease(std::string playbackSessionId)
{
    if(playbackSessionId != "")
    {
        bool releaseSessionKey = true;

        for(auto& mediaAgent : m_mediaAgentList)
        {
            if(playbackSessionId == mediaAgent->GetPlaybackSessionId())
            {
                releaseSessionKey = false;
                break;
            }
        }

        if(releaseSessionKey)
        {
            SLOG_DEBUG("checkSessionKeyRelease() SessionKey Released");
            m_repos->releaseSessionKey(playbackSessionId);
        }
    }
}

MediaAgentPtr SunapiDeviceClientImpl::getMediaAgent(const std::string& mediaUUID)
{
    for(auto& mediaAgent : m_mediaAgentList)
    {
        if(mediaAgent->CheckMediaUUID(mediaUUID))
        {
            return mediaAgent;
        }
    }

    return nullptr;
}

int SunapiDeviceClientImpl::getPlaybackChannelCount()
{
    int count = 0;

    for(auto& mediaAgent : m_mediaAgentList)
    {
        if(mediaAgent->IsPlayback())
            count++;
    }

    return count;
}

void SunapiDeviceClientImpl::DeviceMediaClose( DeviceMediaCloseRequestSharedPtr const& request,
                                           ResponseBaseHandler const& responseHandler)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        int removedIndex = -1;
        bool mediaAgentRemoved = false;
        MediaAgentPtr removedMediaAgent = nullptr;

        SLOG_DEBUG("DeviceMediaClose(), channelID={}, mediaID={}",
                   request->channelID, request->mediaID);
        for(auto& mediaAgent : m_mediaAgentList)
        {
            removedIndex++;

            mediaAgent->RemoveMediaStreamHandler(request->mediaID, mediaAgentRemoved);

            if(mediaAgentRemoved)
            {
                removedMediaAgent = mediaAgent;
                break;
            }
        }

        if(!removedMediaAgent) {
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::NoError;
            responseHandler(response);
            return;
        }

        std::string playbackSessionId = removedMediaAgent->GetPlaybackSessionId();

        if(removedMediaAgent->GetTransportMethod() == Rtsp::TransportMethod::RTP_UDP_MULTICAST)
        {
            // Multicast인 경우 Teardown 후 MediaAgent 정리
            Rtsp::RtspClientPtr removedClient = removedMediaAgent->GetRtspClient();
            if(removedClient)
            {
                removedClient->Teardown([this, playbackSessionId, removedMediaAgent, responseHandler](const Rtsp::RtspResponseSharedPtr& response)
                {
                    if (!response->IsSuccess)
                        SLOG_DEBUG("DeviceMediaClose() Teardown failed");
                    else
                        SLOG_DEBUG("DeviceMediaClose() Teardown succeeded");

                    // Teardown 응답 대기 중 MediaAgentList 변동이 있을 수 있으므로, 삭제 할 index를 다시 체크 후 삭제
                    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, removedMediaAgent, playbackSessionId, responseHandler]()
                    {
                        int removedIndex = -1;
                        bool mediaAgentRemoved = false;

                        for(auto& agent : m_mediaAgentList)
                        {
                            removedIndex++;

                            if(agent == removedMediaAgent)
                            {
                                mediaAgentRemoved = true;
                                agent = nullptr;
                                break;
                            }
                        }

                        if(mediaAgentRemoved)
                        {
                            removedMediaAgent->close();
                            m_mediaAgentList.erase(m_mediaAgentList.begin()+removedIndex);
                            checkSessionKeyRelease(playbackSessionId);
                        }

                        auto response = std::make_shared<ResponseBase>();
                        response->errorCode = Wisenet::ErrorCode::NoError;
                        responseHandler(response);
                    }));
                });
            } else {
                auto response = std::make_shared<ResponseBase>();
                response->errorCode = Wisenet::ErrorCode::NoError;
                responseHandler(response);
            }
        }
        else
        {
            // Multicast가 아닌 경우 Teardown 없이 MediaAgent
            removedMediaAgent->close();
            m_mediaAgentList.erase(m_mediaAgentList.begin()+removedIndex);
            checkSessionKeyRelease(playbackSessionId);

            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::NoError;
            responseHandler(response);
        }
    }));
}

void SunapiDeviceClientImpl::DeviceMediaControl( DeviceMediaControlRequestSharedPtr const& request,
                                             ResponseBaseHandler const& responseHandler )
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        MediaAgentPtr mediaAgent = getMediaAgent(request->mediaID);
        if(mediaAgent == nullptr)
        {
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            responseHandler(response);

            return;
        }

        const MediaControlInfo& mediaControlInfo = request->mediaControlInfo;
        Rtsp::RtspPlaybackParam playParam;

        if(mediaControlInfo.controlType == ControlType::pause)
        {
            SLOG_DEBUG("DeviceMediaControl() Pause, channelID={}", request->channelID);
        }
        else
        {
            playParam.StartTime = mediaControlInfo.time / 1000;

            if (mediaControlInfo.controlType == ControlType::stepForward
                    || mediaControlInfo.controlType == ControlType::stepBackward)
            {
                if (mediaControlInfo.controlType == ControlType::stepForward)
                {
                    SLOG_DEBUG("DeviceMediaControl() Step Forward, channelID={}", request->channelID);
                    playParam.StartTime++;
                    playParam.Scale = Rtsp::RTSP_PLAY_PB_SUNAPI_NVR_F1;
                }
                else
                {
                    SLOG_DEBUG("DeviceMediaControl() Step Backward, channelID={}", request->channelID);
                    playParam.StartTime--;
                    playParam.Scale = Rtsp::RTSP_PLAY_PB_SUNAPI_NVR_B1;
                }

                playParam.Immidiate = true;
                playParam.Resume = false;
            }
            else if(mediaControlInfo.controlType == ControlType::play)
            {
                SLOG_DEBUG("DeviceMediaControl() Play, channelID={}", request->channelID);
                playParam.Scale = mediaControlInfo.speed;
                playParam.Immidiate = false;
                playParam.Resume = true;
            }
        }

        mediaAgent->RtspClientPlayControl(mediaControlInfo.controlType,
                                          playParam,
                                          WeakCallback(shared_from_this(),[this, responseHandler](const Rtsp::RtspResponseSharedPtr& rtspResponse)
        {
            auto response = std::make_shared<ResponseBase>();
            response->errorCode = rtspResponse->IsSuccess ? Wisenet::ErrorCode::NoError
                                                          : convertRtspStatusToErrorcode(rtspResponse->StatusCode);
            responseHandler(response);

            return;
        }));
    }));
}


}
}

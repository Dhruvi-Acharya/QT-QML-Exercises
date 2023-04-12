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

#include <memory>
#include <map>

#include "RtspClient.h"
#include "DeviceClient/DeviceRequestResponse.h"
#include "WeakCallback.h"


namespace Wisenet
{
namespace Device
{

typedef std::function <void(const std::string& mediaUUID)>  MediaDisconnectedHandler;

class MediaAgent : public std::enable_shared_from_this<MediaAgent>
{
public:
    explicit MediaAgent(boost::asio::io_context& ioc,
                        std::string& deviceID,
                        std::string& channelID,
                        Wisenet::Device::StreamType streamType,
                        std::string& profileId,
                        std::string playbackSessionId);
    void close();
    ~MediaAgent();

    const Rtsp::RtspClientPtr GetRtspClient();
    void SetMediaDisconnectedHandler(MediaDisconnectedHandler const& disconnHandler);
    bool IsSameMedia(const std::string& channelID,
                     const Wisenet::Device::StreamType streamType,
                     const std::string& profileId,
                     const std::string& playbackSessionId);
    void AddMediaStreamHandler(std::string mediaUUID, Wisenet::Media::MediaSourceFrameHandler& mediaStreamHandler);
    void CallbackMediaStream(const Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame);
    void OnDisconnected();
    void RtspClientInitPlay(const Rtsp::RtspPlayControlType rtspPlayControlType,
                            const Rtsp::TransportMethod transportMethod,
                            const std::string rtspUrl,
                            const std::string &userName,
                            const std::string &password,
                            const bool supportQuickPlay,
                            const boost::optional<Rtsp::RtspPlaybackParam> playParam,
                            Rtsp::RtspResponseHandler const& responseHandler);
    void RemoveMediaStreamHandler(const std::string mediaUUID, bool& mediaAgentRemoved);
    void ClearMediaStreamHandler();
    void AddMediaOpenResponseHandler(ResponseBaseHandler const& responseHandler);
    int GetMediaOpenResponseHandlerCount();
    void CallbackMediaOpenResponse(Wisenet::ErrorCode errorCode);    
    bool CheckMediaUUID(const std::string& mediaUUID);
    void RtspClientPlayControl(const ControlType controlType,
                               Rtsp::RtspPlaybackParam playParam,
                               Rtsp::RtspResponseHandler const& responseHandler);

    const std::string& GetPlaybackSessionId(){return m_playbackSessionId;}
    const Rtsp::TransportMethod& GetTransportMethod(){return m_transportMethod;}
    bool IsPlayback();

private:
    Wisenet::Rtsp::RtspClientPtr    m_rtspClient;
    std::string                     m_deviceID;
    std::string                     m_channelID;
    Wisenet::Device::StreamType     m_streamType;
    std::string                     m_profileId;
    std::string                     m_playbackSessionId;
    Rtsp::TransportMethod           m_transportMethod;
    ControlType                     m_lastControlType;
    int64_t                         m_lastControlTime;


    std::recursive_mutex m_mutex;
    std::map<std::string, Wisenet::Media::MediaSourceFrameHandler> m_mediaStreamHandlerMap;  // key:MediaUUID
    std::vector<ResponseBaseHandler> m_responseHandlerList;
    MediaDisconnectedHandler    m_disconnectedHandler = nullptr;
};

typedef std::shared_ptr<MediaAgent> MediaAgentPtr;

}
}

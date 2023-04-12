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
#include <boost/asio.hpp>
#include <boost/optional.hpp>

#include "RtspRequestResponse.h"
#include "Media/MediaSourceFrame.h"
#include "RtspSocket.h"


namespace Wisenet
{
namespace Rtsp
{

const std::string kOnvifBackChannelRequireTag = "www.onvif.org/ver20/backchannel";
const std::string kSamsungReplayRequireTag = "samsung-replay-timezone";
const std::string kOnvifReplayRequireTag = "onvif-replay";
const std::string kSSMReplayRequireTag = "ssm-replay";
const std::string kBestshotRequireTag = "Bestshot";

struct BackChannelData
{
    BackChannelData()
        : Data(nullptr), DataSize(0)
    {}

    BackChannelData(const unsigned char *data, const unsigned int dataSize)
        : DataSize(dataSize)
    {
        Data = new unsigned char[dataSize];
        std::memcpy(Data, data, dataSize);
    }
    ~BackChannelData()
    {
        if (Data != nullptr)
            delete[] Data;
    }
    unsigned char *Data;
    unsigned int DataSize;
};
typedef std::shared_ptr<BackChannelData> BackChannelDataPtr;


enum RtspCommandType
{
    RTSP_COMMAND_SETUP,
    RTSP_COMMAND_SETUP_BACKCHANNEL,
    RTSP_COMMAND_PLAY,
    RTSP_COMMAND_PAUSE,
    RTSP_COMMAND_TEARDOWN,
    RTSP_COMMAND_GET_PARAMETER,
    RTSP_COMMAND_QUICK_PLAY
};

// RTSP Client로부터 전달된 명령 정보를 저장하는 구조체
struct RtspCommandItem
{
    RtspCommandType                     Command = RTSP_COMMAND_SETUP;
    RtspResponseHandler                 ResponseHandler;

    boost::optional<RtspPlaybackParam>  PlaybackParam;
    boost::optional<RtspSetupParam>     SetupParam;
};
typedef std::shared_ptr<RtspCommandItem> RtspCommandItemPtr;

struct SunapiCameraPlabackInfo
{
    std::string		LastURL;
    std::string		BaseUrl;
    std::string		OverlappedID;
};


class RtspSession : public std::enable_shared_from_this<RtspSession>
{
public:
    explicit RtspSession(boost::asio::io_context& ioc);
    ~RtspSession();

    bool Initialize(
            const RtspPlayControlType playControlType,
            const std::string &rtspUrl,
            const TransportMethod transportMethod,
            const std::string &userName,
            const std::string &password,
            const unsigned int timeoutSec,
            const std::string &bindIp);

    void SetDisableGetParameter(bool disable) { m_disableGetParameter = disable; }

    void SetIntermediateCallback(const RtspEventCallbackFunctor& eventCallback,
                                 const Media::MediaSourceFrameHandler& mediaCallback);

    void ExecuteRtspCommand(const RtspCommandItemPtr& commandItem,
                            const RtspResponseHandler& responseCallback);

    void TransferBackChannelData(BackChannelDataPtr data);


    void Close();

private:
    std::string GetBaseUrl(std::string& requestUrl, std::string& content_base);

    void CloseInternal();

    // Execute and send RTSP command from RtspClient
    void ExecuteSetup(const RtspCommandItemPtr& commandItem,
                      const RtspResponseHandler& responseCallback);
    void ExecutePlay(const RtspCommandItemPtr& commandItem,
                     const RtspResponseHandler& responseCallback);
    void ExecuteQuickPlay(const RtspCommandItemPtr& commandItem,
                             const RtspResponseHandler& responseCallback);
    void ExecutePause(const RtspCommandItemPtr& commandItem,
                      const RtspResponseHandler& responseCallback);
    void ExecuteTeardown(const RtspResponseHandler& responseCallback);

    // Make RTSP Command message
    std::string  MakeRtspDescribeMessage(const RtspCommandItemPtr& commandItem);
    std::string  MakeRtspSetupMessage(const RtpSessionPtr& session, const RtspCommandItemPtr& commandItem);
    std::string  MakeRtspPlayMessage(const RtspCommandItemPtr& commandItem, std::string &retUrl);
    std::string  MakeRtspPauseMessage();
    std::string  MakeRtspTeardownMessage();
    std::string  MakeRtspQuickPlayMessage(const RtspCommandItemPtr& commandItem, std::string &retUrl);

    // RTSP socket response handler
    void RtspDescribeResponse(const RtspCommandItemPtr& commandItem,
                              const RtspResponseHandler& responseCallback,
                              const RtspSocketResponsePtr& response);

    bool ParseDescribeResponse(const RtspCommandItemPtr &commandItem,
                               const RtspSocketResponsePtr& response);

    bool ParseSessionField(const RtspSocketResponsePtr& response);

    void RtspSetupResponse(const int setupIndex,
                           const RtspCommandItemPtr& commandItem,
                           const RtspResponseHandler& responseCallback,
                           const RtspSocketResponsePtr& response);

    bool ParseSetupResponse(const int setupIndex,
                            const RtspCommandItemPtr &commandItem,
                            const RtspSocketResponsePtr& response);

    void RtspPlayResponse(const RtspCommandItemPtr& commandItem,
                          const RtspResponseHandler& responseCallback,
                          const RtspSocketResponsePtr& response);

    void RtspQuickPlayResponse(const RtspCommandItemPtr& commandItem,
                                   const RtspResponseHandler& responseCallback,
                                   const RtspSocketResponsePtr& response);

    void RtspPauseResponse(const RtspCommandItemPtr& commandItem,
                           const RtspResponseHandler& responseCallback,
                           const RtspSocketResponsePtr& response);

    void RtspTeardownResponse(const RtspResponseHandler& responseCallback,
                              const RtspSocketResponsePtr& response);

    // I/O Complete handler
    void Complete(const RtspResponseHandler& responseCallback,
                  const RtspSocketResponsePtr& response, bool isSuccess);

    // Send RTSP GET PARAMTER periodically to kepp alive
    void StartKeepAliveTimer();
    void RtspSendGetParameter();

private:
    boost::asio::io_context&                m_ioContext;
    boost::asio::strand<boost::asio::io_context::executor_type>    m_strand;

    TransportMethod							m_transportMethod;
    std::string								m_url;
    std::string								m_baseUrl;
    std::string								m_contentBase;

    unsigned int							m_sessionTimeout;
    RtspPlayControlType						m_playControlType;

    // rtsp socket
    RtspSocketPtr							m_rtspSocket;

    // callback
    Media::MediaSourceFrameHandler         m_mediaCallbacks;

    SunapiCameraPlabackInfo					m_sunapiCameraInfo;
    //-------------------------------------------------
    // SETUP이 다시 내려올 경우 초기화 되어야 하는 변수들
    //-------------------------------------------------
    // SDP Parser
    SdpParser								m_sdpParser;

    // rtp sessions
    std::vector<RtpSessionPtr>				m_rtpSessions;
    RtpSessionPtr							m_rtpBackChannel;

    std::string								m_lastSessionID;

    bool									m_disableGetParameter;
    boost::asio::steady_timer               m_keepAliveTimer;
    bool                                    m_useBlockSize;
    std::chrono::steady_clock::time_point   m_startTimePoint;

    // close flag
    std::atomic<bool>                       m_isClosed;
};

}
}

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

#include <memory>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "LogSettings.h"
#include "RtspSession.h"
#include "WeakCallback.h"
#include "RtspHttpSocket.h"
#include "RtspHttpSslSocket.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-SESS] "}, level, __VA_ARGS__)

namespace Wisenet
{
namespace Rtsp
{


bool LoadSunapiCameraUrl(const std::string& baseUrl, SunapiCameraPlabackInfo& camInfo)
{
    /*
    [Type1] rtsp://<Device IP>/recording/<Start Time>/play.smp
    [Type2] rtsp://<Device IP>/recording/<Start Time>-<End Time>/play.smp
    [Type3] rtsp://<Device IP>/recording/<Start Time>-<End Time>/OverlappedID=1/play.smp
    */
    try
    {
        auto pfound = baseUrl.rfind("/play.smp");
        if (pfound == std::string::npos)
            return false;

        auto rfound = baseUrl.find("/recording/");
        if (rfound == std::string::npos || rfound > pfound)
            return false;

        camInfo.BaseUrl = baseUrl.substr(0, rfound);

        auto ofound = baseUrl.find("/OverlappedID=");
        if (ofound != std::string::npos
                && ofound > rfound
                && ofound < pfound)
        {
            camInfo.OverlappedID = baseUrl.substr(ofound, pfound - ofound);
        }
        camInfo.LastURL = baseUrl;
    }
    catch (std::exception& e)
    {
        camInfo = SunapiCameraPlabackInfo();
        SPDLOG_DEBUG("{}", e.what());
        return false;
    }

    return true;
}


RtspSession::RtspSession(boost::asio::io_context& ioc)
    : m_ioContext(ioc)    
    , m_strand(ioc.get_executor())
    , m_sessionTimeout(60)
    , m_disableGetParameter(false)
    , m_keepAliveTimer(ioc)
    , m_useBlockSize(false)
    , m_transportMethod(TransportMethod::RTP_RTSP)
    , m_playControlType(RtspPlayControlType::RTSP_PLAY_LIVE)
    , m_isClosed(false)
{
    SPDLOG_DEBUG("RtspSession()");
    m_startTimePoint = std::chrono::steady_clock::now();
}

RtspSession::~RtspSession()
{
    SPDLOG_DEBUG("~RtspSession()");
}

bool RtspSession::Initialize(
        const RtspPlayControlType playControlType,
        const std::string &rtspUrl,
        const TransportMethod transportMethod,
        const std::string &userName,
        const std::string &password,
        const unsigned int timeoutSec,
        const std::string &bindIp)
{
    m_playControlType = playControlType;
    m_url = rtspUrl;
    //m_url = "rtsp://192.168.255.21:554/profile3/media.smp";
    m_baseUrl = m_url;
    m_transportMethod = transportMethod;

    // check rtsp url
    if (boost::istarts_with(m_baseUrl, "http://"))
        boost::ireplace_first(m_baseUrl, "http://", "rtsp://");
    else if (boost::istarts_with(m_baseUrl, "https://"))
        boost::ireplace_first(m_baseUrl, "https://", "rtsp://");
    if (!boost::istarts_with(m_baseUrl, "rtsp://"))
        m_baseUrl = "rtsp://" + m_baseUrl;

    if (m_transportMethod == RTP_RTSP_HTTP)
    {
        m_rtspSocket = std::make_shared<RtspHttpSocket>(
                    m_ioContext, m_baseUrl, userName, password);
        m_rtspSocket->Initialize(timeoutSec, 80, m_transportMethod);
    }
    else if (m_transportMethod == RTP_RTSP_HTTPS)
    {
        m_rtspSocket = std::make_shared<RtspHttpSslSocket>(
                    m_ioContext, m_baseUrl, userName, password);
        m_rtspSocket->Initialize(timeoutSec, 443, m_transportMethod);
    }
    else
    {
        m_rtspSocket = std::make_shared<RtspSocket>(
                    m_ioContext, m_baseUrl, userName, password);
        m_rtspSocket->Initialize(timeoutSec, 554, m_transportMethod);
    }

    // enable optional RTSP "Blocksize"
    /* 장비 버그로 추후 적용 예정
    m_useBlockSize =  (m_transportMethod == RTP_RTSP ||
                       m_transportMethod == RTP_RTSP_HTTP ||
                       m_transportMethod == RTP_RTSP_HTTPS);
     */

//    m_rtspSocket->SetUserAgentString(userAgentString);
    m_rtspSocket->SetBindIp(bindIp);

    SPDLOG_DEBUG("RtspSession initialized :: url={}, baseUrl={}", m_url, m_baseUrl);

    return true;
}

// 외부 콜백 연결
// RTP SESSION과 RTP SOURCE에서 해당 콜백을 함께 사용한다.
void RtspSession::SetIntermediateCallback(
        const RtspEventCallbackFunctor& eventCallback,
        const Media::MediaSourceFrameHandler& mediaCallback)
{
    m_mediaCallbacks = mediaCallback;
    m_rtspSocket->SetEventCallback(eventCallback);
}

void RtspSession::ExecuteRtspCommand(const RtspCommandItemPtr &commandItem,
                                     const RtspResponseHandler& responseCallback)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem, responseCallback]()
    {
        auto commandType = commandItem->Command;
        if (commandType == RTSP_COMMAND_SETUP)
        {
            SPDLOG_INFO("Setup command, url={}", m_url);
            ExecuteSetup(commandItem, responseCallback);
        }
        else if (commandType == RTSP_COMMAND_SETUP_BACKCHANNEL)
        {
            SPDLOG_INFO("Setup backchannel command, url={}", m_url);
            m_rtspSocket->SetBackChannel(true);
            ExecuteSetup(commandItem, responseCallback);
        }
        else if (commandType == RTSP_COMMAND_PLAY)
        {
            SPDLOG_INFO("Play command, url={}", m_url);
            ExecutePlay(commandItem, responseCallback);
        }
        else if (commandType == RTSP_COMMAND_PAUSE)
        {
            SPDLOG_INFO("Pause command, url={}", m_url);
            ExecutePause(commandItem, responseCallback);
        }
        else if (commandType == RTSP_COMMAND_TEARDOWN)
        {
            SPDLOG_INFO("Teardown command, url={}", m_url);
            ExecuteTeardown(responseCallback);
        }
        else if (commandType == RTSP_COMMAND_QUICK_PLAY)
        {
            SPDLOG_DEBUG("QuickPlay command, url={]", m_url);
            ExecuteQuickPlay(commandItem, responseCallback);
        }
        else
        {
            SPDLOG_INFO("Invalid commandType:: {}, url={}", commandType, m_url);
        }
    }));
}

void RtspSession::Close()
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_keepAliveTimer.cancel();

        CloseInternal();
    }));
}

void RtspSession::CloseInternal()
{
    SPDLOG_DEBUG("RtspSession::CloseInternal() START url={}----", m_url);
    m_isClosed = true;
    m_rtspSocket->Close();

    for(auto& rtpSession : m_rtpSessions)
    {
        rtpSession->Close();
    }
    m_rtpSessions.clear();

    if (m_rtpBackChannel)
    {
        m_rtpBackChannel->Close();
        m_rtpBackChannel.reset();
    }

    m_mediaCallbacks = nullptr;

    SPDLOG_DEBUG("CloseInternal() END url={}----", m_url);
}

void RtspSession::ExecuteSetup(const RtspCommandItemPtr& commandItem,
                               const RtspResponseHandler& responseCallback)
{
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA ||
            m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC)
    {
        if (!LoadSunapiCameraUrl(m_baseUrl, m_sunapiCameraInfo))
        {
            SPDLOG_ERROR("Failed to tokenize sunapi camera url for playback, url={}", m_url);
            auto response = std::make_shared<RtspSocketResponse>();
            response->IsSuccess = false;
            response->StatusCode = 0;
            response->ErrorDetails = "Invalid Sunapi Camera RTSP URL:" + m_url;
            Complete(responseCallback, response, response->IsSuccess);
            return;
        }
    }

    // RTSP DESCRIBE Command
    auto header = MakeRtspDescribeMessage(commandItem);
    m_rtspSocket->Write("DESCRIBE", m_baseUrl, header,
                        WeakCallback(shared_from_this(), [this, commandItem, responseCallback]
                                     (const RtspSocketResponsePtr& response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, commandItem, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspDescribeResponse(commandItem, responseCallback, response);
        }));
    }));
}

void RtspSession::RtspDescribeResponse(const RtspCommandItemPtr& commandItem,
                                           const RtspResponseHandler& responseCallback,
                                           const RtspSocketResponsePtr& response)
{
    SPDLOG_DEBUG("RtspDescribeResponse(), url={}", m_url);

    if (!response->IsSuccess) {
        Complete(responseCallback, response, response->IsSuccess);
        return;
    }

    if (!ParseDescribeResponse(commandItem, response)) {
        Complete(responseCallback, response, false);
        return;
    }

    // 연결할 RTP SESSION이 없는 경우
    if (m_rtpSessions.size() == 0) {
        SPDLOG_WARN("There is no rtp sessions to connect, url={}", m_url);
        Complete(responseCallback, response, false);
        return;
    }

    // RTSP Setup processing
    const int setupIndex = 0;
    auto rtpSession = m_rtpSessions[setupIndex];
    auto header = MakeRtspSetupMessage(rtpSession, commandItem);


    m_rtspSocket->Write("SETUP", rtpSession->Url(), header,
                        WeakCallback(shared_from_this(), [this, commandItem, setupIndex, responseCallback]
                                     (const RtspSocketResponsePtr& response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, commandItem, setupIndex, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspSetupResponse(setupIndex, commandItem, responseCallback, response);
        }));
    }));
}



void RtspSession::RtspSetupResponse(const int setupIndex,
        const RtspCommandItemPtr &commandItem,
        const RtspResponseHandler &responseCallback,
        const RtspSocketResponsePtr& response)
{
    SPDLOG_DEBUG("RtspSetupResponse(), Index={}, url={}", setupIndex, m_url);
    if (!response->IsSuccess) {
        Complete(responseCallback, response, response->IsSuccess);
        return;
    }

    // parse and rtp setup
    if (!ParseSetupResponse(setupIndex, commandItem, response)) {
        Complete(responseCallback, response, false);
        return;
    }


    // continue to next setup
    const int nextSetupIndex = setupIndex+1;
    if (m_rtpSessions.size() > nextSetupIndex) {
        auto rtpSession = m_rtpSessions[nextSetupIndex];
        auto header = MakeRtspSetupMessage(rtpSession, commandItem);

        m_rtspSocket->Write("SETUP", rtpSession->Url(), header,
                            WeakCallback(shared_from_this(), [this, commandItem, nextSetupIndex, responseCallback]
                                         (const RtspSocketResponsePtr& response)
        {
            boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                [this, commandItem, responseCallback, nextSetupIndex, response]()
            {
                if (m_isClosed) return;
                RtspSetupResponse(nextSetupIndex, commandItem, responseCallback, response);
            }));
        }));
        return;
    }

    // finish all setup processing
    for(auto& session : m_rtpSessions)
    {
        session->StartPlaying();
    }

    // 접속유지를 위한 HEARTBEAT 타이머를 등록한다.
    if (m_sessionTimeout > 0)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            StartKeepAliveTimer();
        }));
    }

    Complete(responseCallback, response, response->IsSuccess);
    return;
}

void RtspSession::ExecutePlay(const RtspCommandItemPtr& commandItem,
                              const RtspResponseHandler& responseCallback)
{
    // RTSP PLAY Command
    std::string retUrl;
    auto header = MakeRtspPlayMessage(commandItem, retUrl);

    m_rtspSocket->Write("PLAY", retUrl, header,
                        WeakCallback(shared_from_this(), [this, commandItem, responseCallback]
                                     (const RtspSocketResponsePtr& response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, commandItem, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspPlayResponse(commandItem, responseCallback, response);
        }));
    }));
}


void RtspSession::RtspPlayResponse(const RtspCommandItemPtr &commandItem,
                                       const RtspResponseHandler &responseCallback,
                                       const RtspSocketResponsePtr &response)
{

    SPDLOG_DEBUG("RtspPlayResponse, url={}", m_url);

    if (!response->IsSuccess) {
        Complete(responseCallback, response, response->IsSuccess);
        return;
    }

    // Live 일때만 Play응답을 확인한다.
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE)
    {
        /*
        In order that clients may immediately begin synchronizing video and audio streams, and
        computing absolute UTC timestamps for incoming packets for recording purposes, a
        transmitter should include the following header fields in the RTSP PLAY response:

        - Range ([RFC 2326] section 12.29). This SHALL include a start time in clock units
        ([RFC 2326] section 3.7), not SMPTE or NPT units.

        - RTP-Info ([RFC 2326] section 12.33). This SHALL include an rtptime value which
        corresponds to the start time specified in the Range header.

        Example:

        client->server: PLAY rtsp://example.com/onvif_camera/video RTSP/1.0
        Cseq: 4
        Range: npt=now-
        Session: 12345678

        server->client: RTSP/1.0 200 OK
        Cseq: 4
        Session: 12345678
        Range: clock=20100217T143720.257z-
        RTP-Info: url=rtsp://example.com/onvif_camera/video;seq=1234;rtptime=3450012
        */

        auto dateItr = response->Fields.find("date");
        if (dateItr != response->Fields.end())
        {
            for(auto& session : m_rtpSessions)
            {
                session->SetStartTimeWithPlayResponse(dateItr->second);
            }
        }
        else
        {
            SPDLOG_WARN("Can not find Date field in PLAY response, url={}", m_url);
        }
    }

    Complete(responseCallback, response, response->IsSuccess);
}


void RtspSession::ExecuteQuickPlay(const RtspCommandItemPtr& commandItem,
                              const RtspResponseHandler& responseCallback)
{
    std::string retUrl;
    auto header = MakeRtspQuickPlayMessage(commandItem, retUrl);

    m_rtspSocket->Write("QUICKPLAY", retUrl, header,
                        WeakCallback(shared_from_this(), [this, commandItem, responseCallback]
                                     (const RtspSocketResponsePtr& response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, commandItem, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspQuickPlayResponse(commandItem, responseCallback, response);
        }));
    }));
}


void RtspSession::RtspQuickPlayResponse(const RtspCommandItemPtr &commandItem,
                                        const RtspResponseHandler &responseCallback,
                                        const RtspSocketResponsePtr &response)
{

    SPDLOG_DEBUG("RtspQuickPlayResponse, url={}", m_url);

    if (!response->IsSuccess) {
        Complete(responseCallback, response, response->IsSuccess);
        return;
    }

    if (!ParseSessionField(response)) {
        SPDLOG_WARN("There is no session field in QUICKPLAY response!, continue anyway");
    }

    // Parse SDP
    if (!ParseDescribeResponse(commandItem, response)) {
        Complete(responseCallback, response, false);
        return;
    }

    // 연결할 RTP SESSION이 없는 경우
    if (m_rtpSessions.size() == 0) {
        SPDLOG_WARN("There is no rtp sessions to connect, url={}", m_url);
        Complete(responseCallback, response, false);
        return;
    }

    // RTSP Setup processing
    for (auto& rtpSession : m_rtpSessions) {
        rtpSession->SetSessionID(m_lastSessionID);
        m_rtspSocket->SetInterleavedRtpSessionPtr(rtpSession);
        rtpSession->StartPlaying();
    }

    // Live 일때만 Play응답을 확인한다.
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE) {
        auto dateItr = response->Fields.find("date");
        if (dateItr != response->Fields.end())
        {
            for(auto& session : m_rtpSessions)
            {
                session->SetStartTimeWithPlayResponse(dateItr->second);
            }
        }
        else
        {
            SPDLOG_WARN("Can not find Date field in PLAY response, url={}", m_url);
        }
    }

    // 접속유지를 위한 HEARTBEAT 타이머를 등록한다.
    if (m_sessionTimeout > 0)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            StartKeepAliveTimer();
        }));
    }

    Complete(responseCallback, response, response->IsSuccess);
}


void RtspSession::ExecutePause(const RtspCommandItemPtr& commandItem,
                               const RtspResponseHandler& responseCallback)
{
    // RTSP PAUSE Command
    auto header = MakeRtspPauseMessage();
    auto baseUrl = GetBaseUrl(m_baseUrl, m_contentBase);
    m_rtspSocket->Write("PAUSE", baseUrl, header,
                        WeakCallback(shared_from_this(), [this, commandItem, responseCallback]
                                     (const RtspSocketResponsePtr & response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, commandItem, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspPauseResponse(commandItem, responseCallback, response);
        }));
    }));
}


void RtspSession::RtspPauseResponse(const RtspCommandItemPtr &commandItem,
                                        const RtspResponseHandler &responseCallback,
                                        const RtspSocketResponsePtr &response)
{
    SPDLOG_DEBUG("RtspSendPauseResponse, url={}", m_url);

    Complete(responseCallback, response, response->IsSuccess);
}


void RtspSession::ExecuteTeardown(const RtspResponseHandler& responseCallback)
{
    // RTSP TEARDOWN Command
    std::string baseUrl;
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA ||
            m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC)
        baseUrl = m_sunapiCameraInfo.LastURL;
    else
        baseUrl = GetBaseUrl(m_baseUrl, m_contentBase);
    auto header = MakeRtspTeardownMessage();

    // TEARDOWN은 100ms만 응답을 기다린다.
    m_rtspSocket->Write("TEARDOWN", baseUrl, header,
                        WeakCallback(shared_from_this(), [this, responseCallback]
                                     (const RtspSocketResponsePtr & response)
    {
        boost::asio::post(m_strand, WeakCallback(shared_from_this(),
            [this, responseCallback, response]()
        {
            if (m_isClosed) return;
            RtspTeardownResponse(responseCallback, response);
        }));
    }));

}

void RtspSession::RtspTeardownResponse(const RtspResponseHandler &responseCallback,
                                       const RtspSocketResponsePtr &response)
{
    SPDLOG_DEBUG("RtspSendTeardownResponse, url={}", m_url);

    Complete(responseCallback, response, response->IsSuccess);
}


void RtspSession::Complete(const RtspResponseHandler& responseCallback,
                           const RtspSocketResponsePtr& sockResponse, bool isSuccess)
{

    auto response = std::make_shared<RtspResponse>();

    response->IsSuccess = isSuccess;
    response->CSeq = sockResponse->CSeq;
    response->ErrorDetails = sockResponse->ErrorDetails;
    response->StatusCode = sockResponse->StatusCode;
    if (!response->IsSuccess) {
        SPDLOG_INFO("Complete(), rtsp responose is failed, statusCode={}, details={}",
                    response->StatusCode, response->ErrorDetails);
    }
    if (responseCallback)
        responseCallback(response);
}

void RtspSession::StartKeepAliveTimer()
{
    if (m_sessionTimeout > 0 && !m_disableGetParameter)
    {
        //p2p 연결시 5초이상 지연될 수 있다고 해서, 절반 주기로 보냄.
        unsigned int keepAliveSec = m_sessionTimeout <= 5 ? 1 : (m_sessionTimeout/2);

        m_keepAliveTimer.expires_after(std::chrono::seconds(keepAliveSec));
        m_keepAliveTimer.async_wait( WeakCallback(shared_from_this(),
                                                  [this](const boost::system::error_code& ec)
        {

            if (ec) {
                if (ec == boost::asio::error::operation_aborted)
                    SPDLOG_DEBUG("KeepAlive timer aborted, url={}", m_url);
                return;
            }

            boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
            {
                // Send RTSP GET PARAMETER Message to keep alive
                RtspSendGetParameter();

                // Restart keep alive timer
                StartKeepAliveTimer();
            }));
        }));
    }

}


void RtspSession::RtspSendGetParameter()
{
    std::string baseUrl = GetBaseUrl(m_baseUrl, m_contentBase);
    SPDLOG_DEBUG("RtspSession::RtspSendGetParameter(), url={}", m_url);
    std::ostringstream extraHeader;
    if (!m_lastSessionID.empty())
        extraHeader << "Session: " << m_lastSessionID << "\r\n";

    m_rtspSocket->Write("GET_PARAMETER", baseUrl, extraHeader.str(),
                        WeakCallback(shared_from_this(), [this]
                                     (const RtspSocketResponsePtr& response)
    {
        SPDLOG_DEBUG("RtspSendGetParameter() response, url={}", m_url);
    }));
}

void RtspSession::TransferBackChannelData(BackChannelDataPtr data)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, data]()
    {
        if (m_rtpBackChannel)
        {
            auto rtpSink = m_rtpBackChannel->RtpSink();
            unsigned char channel = m_rtpBackChannel->RtpChannel();
            rtpSink->BuildPackets(data->Data, data->DataSize);

            for(auto& packet : rtpSink->Packets())
            {
                if (m_rtpBackChannel->IsTcpInterleaved())
                {
                    // RTSP Interleaved frame
                    m_rtspSocket->WriteBackChannelData(channel, packet->Buffer(), packet->BufferSize());
                }
                else
                {
                    // RTP/UDP
                    m_rtpBackChannel->RtpSocket()->WriteBackChannelData(packet->Buffer(), packet->BufferSize());
                }
            }

            rtpSink->ClearPakcets();
        }

        // //*data.reset();
    }));
}


}
}

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

#include "RtspRequestResponse.h"
#include "LogSettings.h"
#include "WeakCallback.h"
#include "RtspClient.h"
#include <functional>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-CLNT] "}, level, __VA_ARGS__)
namespace Wisenet
{
namespace Rtsp
{


RtspClient::RtspClient(boost::asio::io_context& ioc)

    : m_ioContext(ioc)
    , m_strand(boost::asio::make_strand(ioc.get_executor()))
    , m_rtspSession(std::make_shared<RtspSession>(m_ioContext))
{
    SPDLOG_DEBUG("RtspClient():: Create a instance");
}

RtspClient::~RtspClient()
{
    SPDLOG_DEBUG("~RtspClient():: Destroy a instance");
}

void RtspClient::Initialize(const RtspPlayControlType playControlType,
                            const std::string &url,
                            const TransportMethod transportMethod,
                            const std::string &userName,
                            const std::string &password,
                            const unsigned int timeoutSec,
                            const std::string &bindIp)
{
    m_rtspSession->Initialize(playControlType, url, transportMethod,
                              userName, password, timeoutSec, bindIp);
}

void RtspClient::SetIntermediateCallback(const RtspEventCallbackFunctor &eventCallback, const Media::MediaSourceFrameHandler &mediaCallback)
{
    m_rtspSession->SetIntermediateCallback(eventCallback, mediaCallback);
}

void RtspClient::SetDisableGetParameter(bool disable)
{
    m_rtspSession->SetDisableGetParameter(disable);
}

void RtspClient::Setup(const RtspSetupParam& setupParam,
                       const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("Setup()");

    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_SETUP;
    commandItem->ResponseHandler = responseCallback;
    commandItem->SetupParam = setupParam;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

void RtspClient::Play(const boost::optional<RtspPlaybackParam> playParam,
                      const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("Play()");

    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_PLAY;
    commandItem->ResponseHandler = responseCallback;
    commandItem->PlaybackParam = playParam;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

void RtspClient::QuickPlay(const RtspSetupParam& setupParam,
               const boost::optional<RtspPlaybackParam> playParam,
               const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("QuickPlay()::startTime={}/{}", playParam->StartTime);
    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_QUICK_PLAY;
    commandItem->ResponseHandler = responseCallback;
    commandItem->SetupParam = setupParam;
    commandItem->PlaybackParam = playParam;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

void RtspClient::Pause(const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("Pause()");

    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_PAUSE;
    commandItem->ResponseHandler = responseCallback;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

void RtspClient::Teardown(const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("Teardown()");

    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_TEARDOWN;
    commandItem->ResponseHandler = responseCallback;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

void RtspClient::SetupBackChannel(const RtspResponseHandler& responseCallback)
{
    SPDLOG_DEBUG("SetupBackChannel()");

    RtspCommandItemPtr commandItem = std::make_shared<RtspCommandItem>();
    commandItem->Command = RTSP_COMMAND_SETUP_BACKCHANNEL;
    commandItem->ResponseHandler = responseCallback;

    boost::asio::post(m_strand, WeakCallback(shared_from_this(),
                                             [this, commandItem]()
    {
        pushRequest(commandItem);
    }));
}

bool RtspClient::SendBackChannelData(const unsigned char *data, const unsigned int dataSize)
{
    SPDLOG_TRACE("SendBackChannelData()");

    std::shared_ptr<BackChannelData> sdata = std::make_shared<BackChannelData>(data, dataSize);
    m_rtspSession->TransferBackChannelData(sdata);

    return true;
}

void RtspClient::Close()
{
    SPDLOG_DEBUG("Close()");
    // close() 호출시에는 shared_ptr이 삭제되더라도 정상적으로 close()가 완료되도록
    // weak_ptr을 사용하지 않는다.
    auto self = shared_from_this();
    boost::asio::post(m_strand, [self, this]()
    {
        SPDLOG_DEBUG("CloseInternal()");
        cancelAllRequest();

        m_rtspSession->Close();
    });

    /* OLD CODE
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        SPDLOG_DEBUG("CloseInternal()");
        cancelAllRequest();

        m_rtspSession->Close();
    }));
    */
}


///////////////////////////////// request queue ////////////////////////////////
void RtspClient::pushRequest(const RtspCommandItemPtr &request)
{
    bool doStart = (m_requests.size() == 0);
    m_requests.push(request);
    SPDLOG_DEBUG("RTSP push request:: queue count = {}", m_requests.size());

    if (doStart) {
        executeRequest(request);
    }
}

void RtspClient::executeRequest(const RtspCommandItemPtr &request)
{
    SPDLOG_DEBUG("RTSP execute request");
    m_rtspSession->ExecuteRtspCommand(
                request, WeakCallback(shared_from_this(), [this, request]
                                      (const RtspResponseSharedPtr& response)
    {
        completeRequest();
        if (request->ResponseHandler) {
            request->ResponseHandler(response);
        }
    }));
}

void RtspClient::completeRequest()
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        if (m_requests.size() > 0) {
            m_requests.pop();
            SPDLOG_DEBUG("RTSP complete request:: queue count = {}", m_requests.size());

            // go to next request processing
            if (m_requests.size() > 0) {
                auto& request = m_requests.front();
                executeRequest(request);
            }
        }
    }));
}

void RtspClient::cancelAllRequest()
{
    if (m_requests.size() > 0) {
        SPDLOG_DEBUG("Clear all RTSP request queue, count={}", m_requests.size());
        std::queue<RtspCommandItemPtr> empty;
        std::swap(m_requests, empty);
    }
}


}
}

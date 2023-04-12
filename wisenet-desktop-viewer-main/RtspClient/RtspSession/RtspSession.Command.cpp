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

#include <iostream>
#include <boost/date_time.hpp>

#include "LogSettings.h"
#include "RtspSocket.h"
#include "RtspSession.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-SESS] "}, level, __VA_ARGS__)

namespace Wisenet
{
namespace Rtsp
{

#define EPSILON 0.00001  //정확도.

bool float_compare_same(float a, float b)
{
    return fabs(a - b) < EPSILON; // 앞서 정의한 0.00001 이하는 비교하지 않습니다.
}

static bool isAbsoluteURL(const std::string& url)
{
    for (std::string::size_type i = 0; i < url.size(); i++)
    {
        if (url[i] == '/' && url[i] == '\0')
        {
            break;
        }
        else if (url[i] == ':' )
        {
            return true;
        }
    }
    return false;
}


std::string GetAbsTimeString(const time_t t, const bool isUtc)
{
    boost::posix_time::ptime start;
    std::stringstream ss;
    try {
        start = boost::posix_time::from_time_t(t);
        ss << std::setw(4) << std::setfill('0') << start.date().year();
        ss << std::setw(2) << std::setfill('0') << start.date().month().as_number();
        ss << std::setw(2) << std::setfill('0') << start.date().day();
        if (isUtc)	ss << "T";
        ss << std::setw(2) << std::setfill('0') << start.time_of_day().hours();
        ss << std::setw(2) << std::setfill('0') << start.time_of_day().minutes();
        ss << std::setw(2) << std::setfill('0') << start.time_of_day().seconds();
        if (isUtc) ss << "Z";
    }
    catch (std::exception& e) {
        const static std::string defDateStr = "1970-01-01T12:00:00Z";
        SPDLOG_ERROR("BAD TIME_T VALUE, USE 1970 TIME");
        return defDateStr;
    }
    return ss.str();
}

void GetPortFromTransportParams(const std::string& input,
                                unsigned short &port1_ret, unsigned short &port2_ret)
{
    std::istringstream is(input);
    std::string token;
    if (std::getline(is, token, '-'))
    {
        boost::trim(token);
        boost::conversion::try_lexical_convert<unsigned short>(token, port1_ret);
        port2_ret = port1_ret;

        if (std::getline(is, token, '-'))
        {
            boost::trim(token);
            boost::conversion::try_lexical_convert<unsigned short>(token, port2_ret);
        }
    }
}

std::string RtspSession::GetBaseUrl(std::string& requestUrl, std::string& content_base)
{
    std::string baseUrl;
    if (content_base.empty() || m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR)
    {
        baseUrl = requestUrl;
    }
    else
    {
        if (boost::ends_with(content_base, "/"))
            baseUrl = content_base.substr(0, content_base.size() - 1);
        else
            baseUrl = content_base;
    }
    return baseUrl;
}

std::string RtspSession::MakeRtspDescribeMessage(const RtspCommandItemPtr& commandItem)
{
    SPDLOG_TRACE("MakeRtspDescribeMessage(), url={}", m_url);
    std::ostringstream extraHeader;

    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL)
    {
        extraHeader << "Require: " << kOnvifBackChannelRequireTag << "\r\n";
    }
    else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM)
    {
        extraHeader << "Require: " << kSSMReplayRequireTag << "\r\n";
    }
    else if ((m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE || m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR)
             && commandItem->SetupParam->MetaImageOn)
    {
        extraHeader << "Require: " << kBestshotRequireTag << "\r\n";
    }
    extraHeader << "Accept: application/sdp\r\n";
    return extraHeader.str();
}

std::string RtspSession::MakeRtspSetupMessage(const RtpSessionPtr& session,
                                              const RtspCommandItemPtr& commandItem)
{
    SPDLOG_TRACE("MakeRtspSetupMessage(), url={}", m_url);

    std::ostringstream extraHeader;
    // seperate rtp
    if (m_transportMethod == RTP_UDP_MULTICAST ||
        m_transportMethod == RTP_UDP_UNICAST) {
        extraHeader << "Transport: " << "RTP/AVP;";

        // TODO :: 추후 c=IN IP4 .., c=IN IP6 파싱하여 엔드포인트가 멀티캐스트 주소인지 확인 필요함.
        if (m_transportMethod == RTP_UDP_MULTICAST)
            extraHeader << "multicast;";
        else
            extraHeader << "unicast;";

        unsigned short rtpPort = session->RtpPort();
        unsigned short rtcpPort = session->RtcpPort();

        extraHeader << "client_port=" << rtpPort << "-" << rtcpPort << "\r\n";
    }

    // interleaved
    else {
        extraHeader << "Transport: RTP/AVP/TCP;unicast;interleaved=";
        extraHeader << session->RtpChannel() << "-" << session->RtcpChannel() << "\r\n";
    }

    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL) {
        extraHeader << "Require: " << kOnvifBackChannelRequireTag << "\r\n";
    }
    else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_ONVIF ||
             m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR) {
        extraHeader << "Require: " << kOnvifReplayRequireTag << "\r\n";
    }
    else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA ||
             m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC) {
        extraHeader << "Require: " << kSamsungReplayRequireTag << "\r\n";
    }
    else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM) {
        extraHeader << "Require: " << kSSMReplayRequireTag << "\r\n";
    }

    if (!m_lastSessionID.empty()) {
        extraHeader << "Session: " << m_lastSessionID << "\r\n";
    }
    // use blocksize
    if (m_useBlockSize) {
        extraHeader << "Blocksize: 64000\r\n";
    }

    return extraHeader.str();
}


std::string RtspSession::MakeRtspPlayMessage(const RtspCommandItemPtr& commandItem, std::string &retUrl)
{
    SPDLOG_TRACE("MakeRtspPlayMessage(), url={}", m_url);

    boost::optional<RtspPlaybackParam> &playbackParam = commandItem->PlaybackParam;
    std::ostringstream extraHeader;
    if (!m_lastSessionID.empty())
        extraHeader << "Session: " << m_lastSessionID << "\r\n";

    ////////////////////////////////////////////////////////////////////////////
    // LIVE, BACKCHANNEL
    ////////////////////////////////////////////////////////////////////////////

    // LIVE 또는 BACKCHANNEL일 경우
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE || m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL)
    {
        if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE)
            extraHeader << "Range: npt=0.000-\r\n";

        else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL)
            extraHeader << "Require: " << kOnvifBackChannelRequireTag << "\r\n";

        retUrl = GetBaseUrl(m_baseUrl, m_contentBase);
        return extraHeader.str();
    }


    ////////////////////////////////////////////////////////////////////////////
    // PLAYBACK
    ////////////////////////////////////////////////////////////////////////////

    std::stringstream requestURL;
    // SUNAPI CAMERA PLAYBACK 인 경우
    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA ||
            m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC)
    {
        if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA)
        {
            extraHeader << "Require: " << kOnvifReplayRequireTag << "\r\n";

            requestURL << m_sunapiCameraInfo.LastURL;

            if (playbackParam->StartTime == 0)
            {
                extraHeader << "Range: npt=0.000-\r\n";
            }
            else
            {
                extraHeader << "Range: clock=" << GetAbsTimeString(playbackParam->StartTime, true) << "-";
                if (playbackParam->EndTime != 0)
                    extraHeader << GetAbsTimeString(playbackParam->EndTime, true);
                extraHeader << "\r\n";
            }
        }
        else // RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC
        {
            extraHeader << "Require: " << kSamsungReplayRequireTag << "\r\n";

            if (playbackParam->Resume || playbackParam->StartTime == 0)
            {
                requestURL << m_sunapiCameraInfo.LastURL;
            }
            else
            {
                requestURL << m_sunapiCameraInfo.BaseUrl << "/recording/" <<
                    GetAbsTimeString(playbackParam->StartTime, false);
                if (playbackParam->EndTime != 0)
                    requestURL << "-" << GetAbsTimeString(playbackParam->EndTime, false);
                if (!m_sunapiCameraInfo.OverlappedID.empty())
                    requestURL << m_sunapiCameraInfo.OverlappedID;
                requestURL << "/play.smp";
                m_sunapiCameraInfo.LastURL = requestURL.str();
            }

            extraHeader << "Range: npt=0.000-\r\n";
        }

        std::stringstream stream;
        stream << std::fixed << std::setprecision(3) << playbackParam->Scale;
        extraHeader << "Scale: " << stream.str() << "\r\n";
    }

    // SUNAPI NVR, ONVIF PLAYBACK 인 경우
    else if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR ||
        m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_ONVIF ||
        m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM)
    {
        requestURL << GetBaseUrl(m_baseUrl, m_contentBase);

        if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM)
            extraHeader << "Require: " << kSSMReplayRequireTag << "\r\n";
        else
            extraHeader << "Require: " << kOnvifReplayRequireTag << "\r\n";

        if (playbackParam->StartTime == 0)
        {
            extraHeader << "Range: npt=0.000-\r\n";
        }
        else
        {
            extraHeader << "Range: clock=" << GetAbsTimeString(playbackParam->StartTime, true) << "-";
            if (playbackParam->EndTime != 0)
                extraHeader << GetAbsTimeString(playbackParam->EndTime, true);
            extraHeader << "\r\n";
        }

        if (playbackParam->FrameType == FRAME_ALL)
            extraHeader << "Frames: all\r\n";
        else if (playbackParam->FrameType == FRAME_INTRA)
            extraHeader << "Frames: intra\r\n";
        else if (playbackParam->FrameType == FRAME_PREDICTED)
            extraHeader << "Frames: predicted\r\n";
    }


    if (playbackParam->RateControl)
    {
        extraHeader << "Rate-Control: yes\r\n";

        if ((m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR || m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM)
                && float_compare_same(playbackParam->Scale, RTSP_PLAY_PB_SUNAPI_NVR_F1))
        {
            extraHeader << "Scale: +0.0\r\n";
        }
        else if ((m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR || m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SSM)
                 && float_compare_same(playbackParam->Scale, RTSP_PLAY_PB_SUNAPI_NVR_B1))
        {
            extraHeader << "Scale: -0.0\r\n";
        }
        else
        {
            std::stringstream stream;
            stream << std::fixed << std::setprecision(3) << playbackParam->Scale;
            extraHeader << "Scale: " << stream.str() << "\r\n";
        }

    }
    else
    {
        extraHeader << "Rate-Control: no\r\n";
        if (playbackParam->Scale > 0)
            extraHeader << "Scale: 1.0\r\n";
        else if (playbackParam->Scale < 0)
            extraHeader << "Scale: -1.0\r\n";
    }

    if (playbackParam->Immidiate)
        extraHeader << "Immediate: yes\r\n";

    retUrl = requestURL.str();
    return extraHeader.str();
}

// DESCRIBE + SETUP + PLAY
// Media-Type: video/audio/metadata/metaimage/backchannel
std::string RtspSession::MakeRtspQuickPlayMessage(const RtspCommandItemPtr& commandItem, std::string &retUrl)
{
    std::string playExtra = MakeRtspPlayMessage(commandItem, retUrl);

    std::ostringstream extraHeader;
    boost::optional<RtspSetupParam> &setupParam = commandItem->SetupParam;
    std::vector<std::string> mediaTypes;
    if (setupParam->VideoOn) {
        mediaTypes.push_back("video");
    }
    if (setupParam->AudioOn) {
        mediaTypes.push_back("audio");
    }
    if (setupParam->OnvifMetaDataOn) {
        mediaTypes.push_back("metadata");
    }
    if (setupParam->MetaImageOn) {
        mediaTypes.push_back("metaimage");
    }
    extraHeader << "Media-Type: ";
    for (int i = 0; i < mediaTypes.size(); i++) {
        extraHeader << mediaTypes[i];
        if (i+1 < mediaTypes.size()) {
            extraHeader << "/";
        }
    }
    extraHeader << "\r\n";
    extraHeader << "Accept: application/sdp\r\n";

    return playExtra + extraHeader.str();
}

std::string RtspSession::MakeRtspPauseMessage()
{
    SPDLOG_TRACE("MakeRtspPauseMessage()");
    std::ostringstream extraHeader;
    if (!m_lastSessionID.empty())
        extraHeader << "Session: " << m_lastSessionID << "\r\n";

    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL)
    {
        extraHeader << "Require: " << kOnvifBackChannelRequireTag << "\r\n";
    }

    return extraHeader.str();
}

std::string RtspSession::MakeRtspTeardownMessage()
{
    SPDLOG_TRACE("MakeRtspTeardownMessage()");

    std::ostringstream extraHeader;
    if (!m_lastSessionID.empty())
        extraHeader << "Session: " << m_lastSessionID << "\r\n";

    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL)
        extraHeader << "Require: " << kOnvifBackChannelRequireTag << "\r\n";

    return extraHeader.str();
}


bool RtspSession::ParseDescribeResponse(const RtspCommandItemPtr &commandItem,
                                        const RtspSocketResponsePtr& response)
{
    auto content_base = response->Fields.find("content-base");
    if (content_base != response->Fields.end())
        m_contentBase = content_base->second;

    m_sdpParser.Parse(response->ResponseContent);

    // RTP 세션 생성
    unsigned int channel = 0;
    for (auto& stream : m_sdpParser.Streams())
    {
        // 백채널인 경우, audio / sendonly일 경우에만 SETUP을 수행한다.
        if (commandItem->Command == RTSP_COMMAND_SETUP_BACKCHANNEL)
        {
            if (stream.Media != "audio")
                continue;

            if (!stream.SendOnly)
                continue;
        }
        else
        {
            if (!commandItem->SetupParam)
            {
                SPDLOG_WARN("ParseDescribeResponse() SetupParam is nullptr, url={}", m_url);
                continue;
            }

            if (stream.Media == "video" && !commandItem->SetupParam->VideoOn && !stream.MetaImageSession)
            {
                SPDLOG_DEBUG("ParseDescribeResponse() Video is off, url={}", m_url);
                continue;
            }

            if (stream.Media == "audio" && !commandItem->SetupParam->AudioOn)
            {
                SPDLOG_DEBUG("ParseDescribeResponse() Audio is off, url={}", m_url);
                continue;
            }
            if (stream.Media == "application" && !commandItem->SetupParam->OnvifMetaDataOn)
            {
                SPDLOG_DEBUG("ParseDescribeResponse() Metadata is off, url={}", m_url);
                continue;
            }
        }

        std::string subSessionUrl = stream.Control;
        if (!isAbsoluteURL(subSessionUrl))
        {
            std::string baseUrl;
            if (m_contentBase.empty())
            {
                baseUrl = m_baseUrl;
            }
            else
            {
                baseUrl = m_contentBase;
            }

            if (boost::ends_with(baseUrl, "/"))
                subSessionUrl = baseUrl + subSessionUrl;
            else
                subSessionUrl = baseUrl + "/" + subSessionUrl;
        }

        // RTP 세션을 생성한다.
        RtpSessionPtr session = std::make_shared<RtpSession>(m_ioContext);
        session->Initialize(subSessionUrl, m_playControlType, stream, m_mediaCallbacks);

        // UDP 통신인 경우, 소켓도 미리 준비시킨다.
        if (m_transportMethod == RTP_UDP_UNICAST || m_transportMethod == RTP_UDP_MULTICAST)
        {
            session->CreateUDPSocket(stream.IsRtcpMux, 0);
        }
        else if (stream.HasTransport) {
            SPDLOG_DEBUG("QUICKPLAY set tcp interrleaved channel, rtp={}, rtcp={}",
                         stream.InterleavedRtpChannel, stream.InterleavedRtcpChannel);
            session->SetupTcpInterleaved(stream.InterleavedRtpChannel, stream.InterleavedRtcpChannel);
        }
        else
        {
            session->SetupTcpInterleaved(channel, channel + 1);
            channel += 2;
        }

        if (stream.SendOnly)
        {
            m_rtpBackChannel = session;
            session->CreateRtpSink();
        }
        else
        {
            if (!session->CreateRtpSource(m_startTimePoint))
            {
                SPDLOG_INFO("Create RTP Source failed, Skip this rtsp source... codec={}, url={}", stream.CodecName, m_url);
                continue;
            }
        }

        m_rtpSessions.push_back(session);
    }

    if(m_rtpSessions.size() == 0)
    {
        response->IsSuccess = false;
        response->ErrorDetails = "Create RTP Source failed";
        return false;
    }

    return true;
}

bool RtspSession::ParseSessionField(const RtspSocketResponsePtr& response)
{
    //  Session  = "Session" ":" session-id [ ";" "timeout" "=" delta-seconds ]
    auto session = response->Fields.find("session");
    if (session == response->Fields.end()) {
        return false;
    }

    std::vector<std::string> tokens;
    boost::split(tokens, session->second, boost::is_any_of(";"), boost::token_compress_on);
    if (tokens.size() == 2)
    {
        m_lastSessionID = tokens[0];
        std::string timeoutVal = boost::trim_copy(tokens[1]);
        if (boost::starts_with(timeoutVal, "timeout="))
        {
            unsigned int timeout = 20;
            auto tval = timeoutVal.substr(8);
            boost::trim(tval);
            boost::conversion::try_lexical_convert<unsigned int>(tval, timeout);
            if (timeout < m_sessionTimeout)
                m_sessionTimeout = timeout;
        }
    }
    else
    {
        m_lastSessionID = session->second;
    }
    return true;
}

bool RtspSession::ParseSetupResponse(const int setupIndex,
                                     const RtspCommandItemPtr &commandItem,
                                     const RtspSocketResponsePtr& response)
{
    auto currentRtpSession = m_rtpSessions.at(setupIndex);

    if (ParseSessionField(response)) {
        currentRtpSession->SetSessionID(m_lastSessionID);
    }

    // We have a valid "Transport:" header if any of the following are true:
    //   - We saw a "interleaved=" field, indicating RTP/RTCP-over-TCP streaming, or
    //   - We saw a "server_port=" field, or
    //   - We saw a "client_port=" field.
    //     If we didn't also see a "server_port=" field, then the server port is assumed to be the same as the client port.

    auto transport = response->Fields.find("transport");
    if (transport == response->Fields.end())
    {
        SPDLOG_WARN("Can not find Transport field in setup response, url={}", m_url);
        response->IsSuccess = false;
        response->ErrorDetails = "Can not find Transport field";
        return false;
    }

    std::string transportValue = transport->second;
    std::istringstream is(transportValue);
    std::string token;
    bool isMulticast = false;
    std::string destination;
    std::string interleaved;
    std::string serverPort;
    std::string clientPort;
    std::string multicastPort;
    while (std::getline(is, token, ';'))
    {
        if (boost::equals(token, "unicast"))
        {
            isMulticast = false;
        }
        else if (boost::equals(token, "multicast"))
        {
            isMulticast = true;
        }
        else if (boost::starts_with(token, "server_port="))
        {
            serverPort = token.substr(12);
        }
        else if (boost::starts_with(token, "port="))
        {
            multicastPort = token.substr(12);
        }
        else if (boost::starts_with(token, "client_port="))
        {
            clientPort = token.substr(12);
        }
        else if (boost::starts_with(token, "interleaved="))
        {
            interleaved = token.substr(12);
        }
        else if (boost::starts_with(token, "destination="))
        {
            destination = token.substr(12);
        }
    }

    SPDLOG_DEBUG("ParseSetupResponse isMulticast={} clientPort={} interleaved={} destination={}",
                 isMulticast, clientPort, interleaved, destination);

    // INTERLEAVED
    if (!interleaved.empty())
    {
        std::istringstream is(interleaved);
        std::string token;
        unsigned int rtpChannel = 0, rtcpChannel = 0;
        if (std::getline(is, token, '-'))
        {
            boost::trim(token);
            boost::conversion::try_lexical_convert<unsigned int>(token, rtpChannel);

            if (std::getline(is, token, '-'))
            {
                // TODO : RTCP mux 이면 어떻게 하나?
                boost::trim(token);
                boost::conversion::try_lexical_convert<unsigned int>(token, rtcpChannel);
                currentRtpSession->SetupTcpInterleaved(rtpChannel, rtcpChannel);

                // INTERELAVED인 경우 RTSP로부터 RTP데이터를 받아서 RTP세션으로 전달하기 위해서
                // RTSP SOCKET에 RTP세션 포인터를 할당한다.
                m_rtspSocket->SetInterleavedRtpSessionPtr(currentRtpSession);
            }
        }
    }
    // UDP
    else
    {
        std::string ip;

        unsigned short clientRtpPort = currentRtpSession->RtpPort();
        unsigned short clientRtcpPort = currentRtpSession->RtcpPort();

        unsigned short rtpPort = clientRtpPort, rtcpPort = clientRtcpPort;

        if (!clientPort.empty())
        {
            GetPortFromTransportParams(clientPort, clientRtpPort, clientRtcpPort);
        }

        if (isMulticast && !destination.empty())
        {
            ip = destination;
            if (!multicastPort.empty())
                GetPortFromTransportParams(multicastPort, rtpPort, rtcpPort);
        }
        else
        {
            ip = m_rtspSocket->GetIp();
            if (!serverPort.empty())
                GetPortFromTransportParams(serverPort, rtpPort, rtcpPort);
            else
            {
                rtpPort = clientRtpPort;
                rtcpPort = clientRtcpPort;
            }
        }

        // RTP/UDP 인 경우 UDP 소켓을 별도로 생성한다.
        currentRtpSession->SetupUDPSocket(
                    isMulticast,
                    clientRtpPort == clientRtcpPort,
                    ip,
                    clientRtpPort,
                    clientRtcpPort,
                    rtpPort,
                    rtcpPort
                    );
    }

    return true;
}

}
}

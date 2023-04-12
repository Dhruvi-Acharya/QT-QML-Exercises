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

#include <iomanip>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/date_time/local_time/local_time.hpp>
#include <boost/date_time/local_time/local_time_io.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "RtpSource/RtpSource.h"
#include "RtpSource/RtpAudioSource.hpp"
#include "RtpSource/AacRtpSource.hpp"
#include "RtpSource/H264RtpSource.hpp"
#include "RtpSource/H265RtpSource.hpp"
#include "RtpSource/MjpegRtpSource.hpp"
#include "RtpSource/RtpMetaDataSource.hpp"
#include "RtpSource/RtpMetaImageSource.hpp"

#include "LogSettings.h"
#include "Media/MediaSourceFrame.h"
#include "WeakCallback.h"
#include "RtpSession.h"


namespace Wisenet
{
namespace Rtsp
{


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTP-SESS] "}, level, __VA_ARGS__)


RtpSession::RtpSession(boost::asio::io_context& ioc)
    : m_ioContext(ioc)
    , m_isTcpInterleaved(false)
    , m_isRtcpMux(false)
    , m_isMulticast(false)
    , m_rtpChannel(0)
    , m_rtcpChannel(0)
    , m_playControlType(RtspPlayControlType::RTSP_PLAY_LIVE)
{
    SPDLOG_DEBUG("RtpSession()");
}


RtpSession::~RtpSession()
{
    SPDLOG_DEBUG("~RtpSession(), URL={}", m_url);
}

void RtpSession::Initialize(std::string &url,
                            RtspPlayControlType playControlType,
                            MediaStreamInfo streamInfo,
                            Media::MediaSourceFrameHandler& callback)
{
    m_url = url;
    m_playControlType = playControlType;
    m_streamInfo = streamInfo;
    m_mediaCallbacks = callback;
}

void RtpSession::Close()
{
    SPDLOG_DEBUG("RtpSession::Close(), url={}", m_url);

    if (!m_isTcpInterleaved)
    {
        if (m_rtpSocket)
        {
            m_rtpSocket->Close();
            m_rtpSocket.reset();
        }
        if (m_rtcpSocket && !m_isRtcpMux)
        {
            m_rtcpSocket->Close();
            m_rtcpSocket.reset();
        }
    }

    //*m_rtpSink.reset();
    if (m_rtpSource)
        m_rtpSource->Close();

    m_mediaCallbacks = nullptr;

}

void RtpSession::SetupTcpInterleaved(const unsigned int rtpChannel,
                                     const unsigned int rtcpChannel)
{
    SPDLOG_DEBUG("SetupTcpInterleaved(), url={}", m_url);
    m_rtpChannel = rtpChannel;
    m_rtcpChannel = rtcpChannel;
    m_isTcpInterleaved = true;
}

void RtpSession::CreateUDPSocket(const bool isRtcpMux,
                                 const unsigned short recommandClientPort)
{
    SPDLOG_DEBUG("CreateUDPSocket(), url={}", m_url);
    m_isRtcpMux = isRtcpMux;
    // https://tools.ietf.org/html/rfc3551
    // As specified in the RTP protocol definition, RTP data SHOULD be
    // carried on an even UDP port number and the corresponding RTCP packets
    // SHOULD be carried on the next higher(odd) port number.
    if (recommandClientPort != 0)
    {
        // use an even-numbered port for RTP, and the next (odd-numbered) port for RTCP
        unsigned short port = recommandClientPort & ~1;
        m_rtpSocket = std::make_shared<RtpUdpSocket>(*this, m_ioContext, port);
        if (!m_isRtcpMux)
            m_rtcpSocket = std::make_shared<RtpUdpSocket>(*this, m_ioContext, port | 1);
        else
            m_rtcpSocket = m_rtpSocket;

        //SPDLOG_DEBUG("CreateUDPSocket(), recommandClientPort={}, rtpPort={}, rtcpPort={}", recommandClientPort, m_rtpSocket->GetPortNumber(), m_rtcpSocket->GetPortNumber());
    }
    else
    {
        std::vector<RtpUdpSocketPtr> socketList;

        // Create sockets until we get a port-number pair (even: RTP, even+1: RTCP)
        bool isSuccess = false;
        while (!isSuccess)
        {
            RtpUdpSocketPtr rtpSockPtr = std::make_shared<RtpUdpSocket>(*this, m_ioContext);
            unsigned short rtpPort = rtpSockPtr->GetPortNumber();

            if ((rtpPort & 1) != 0)
            {
                socketList.push_back(rtpSockPtr);
                continue;
            }
            
            if (!m_isRtcpMux)
            {
                RtpUdpSocketPtr rtcpSockPtr = std::make_shared<RtpUdpSocket>(*this, m_ioContext, rtpPort | 1);
                unsigned short rtcpPort = rtcpSockPtr->GetPortNumber();
                
                if (rtcpPort != (rtpPort | 1)) {
                    socketList.push_back(rtcpSockPtr);
                    continue;
                }

                //SPDLOG_DEBUG("CreateUDPSocket(), url={}, rtpPort={}, rtcpPort={}", m_url, rtpPort, rtcpPort);
                m_rtpSocket = rtpSockPtr;
                m_rtcpSocket = rtcpSockPtr;
            }
            else
            {
                m_rtpSocket = m_rtcpSocket = rtpSockPtr;
            }

            isSuccess = true;
            break;
        }
    }
}

void RtpSession::SetupUDPSocket(
        const bool isMulticast,
        const bool isRtcpMux,
        const std::string &serverAddress,
        const unsigned short rtpClientPort,
        const unsigned short rtcpClientPort,
        const unsigned short rtpServerPort,
        const unsigned short rtcpServerPort)
{
    SPDLOG_TRACE("SetupUDPSocket(), url={}", m_url);

    bool needToReset = false;
    if (isRtcpMux != m_isRtcpMux)
        needToReset = true;
    else if (rtpClientPort != m_rtpSocket->GetPortNumber())
        needToReset = true;
    else if (rtcpClientPort != m_rtcpSocket->GetPortNumber())
        needToReset = true;

    if (needToReset)
    {
        SPDLOG_INFO("SDP information and SETUP response have different information, remake udp socket.., url={}", m_url);
        m_isRtcpMux = isRtcpMux;

        m_rtpSocket.reset();
        m_rtcpSocket.reset();

        m_rtpSocket = std::make_shared<RtpUdpSocket>(*this, m_ioContext, rtpClientPort);
        if (!m_isRtcpMux)
            m_rtcpSocket = std::make_shared<RtpUdpSocket>(*this, m_ioContext, rtcpClientPort | 1);
        else
            m_rtcpSocket = m_rtpSocket;
    }

    m_isMulticast = isMulticast;
    m_rtpSocket->SetDestination(serverAddress, rtpServerPort, isMulticast);
    if (!isRtcpMux)
    {
        m_rtcpSocket->SetDestination(serverAddress, rtcpServerPort, isMulticast);
        m_rtcpSocket->SetRtcp();
    }
    else
    {
        m_rtpSocket->SetRtcpMux();
    }
}


bool RtpSession::CreateRtpSource(const std::chrono::steady_clock::time_point& rtspOpenTimePoint)
{
    SPDLOG_DEBUG("RtpSession::CreateRtpSource(), url={} codec={}", m_url, m_streamInfo.CodecName);

    size_t expectedFrameSize = 10240;
    if (m_streamInfo.MetaImageSession)
    {
        m_rtpSource = std::make_shared<RtpMetaImageSource>();
    }
    else if (m_streamInfo.CodecName == "H264")
    {
        m_rtpSource = std::make_shared<H264RtpSource>();
        expectedFrameSize = 512000;
    }
    else if (m_streamInfo.CodecName == "H265")
    {
        m_rtpSource = std::make_shared<H265RtpSource>();
        expectedFrameSize = 512000;
    }
    else if (m_streamInfo.CodecName == "JPEG" || m_streamInfo.PayloadFormat == 26)
    {
        m_rtpSource = std::make_shared<MjpegRtpSource>();
        expectedFrameSize = 512000;
    }
    // AAC
    else if (m_streamInfo.CodecName == "MPEG4-GENERIC")
    {
        m_rtpSource = std::make_shared<AacRtpSource>();
    }

    else if (m_streamInfo.CodecName == "PCMU" ||
             m_streamInfo.CodecName == "DVI4" ||		// ADPCM
             m_streamInfo.CodecName == "G726-16" ||
             m_streamInfo.CodecName == "G726-24" ||
             m_streamInfo.CodecName == "G726-32" ||
             m_streamInfo.CodecName == "G726-40" )
    {
        m_rtpSource = std::make_shared<RtpAudioSource>();
    }
    else if (m_streamInfo.CodecName == "VND.ONVIF.METADATA")
    {
        m_rtpSource = std::make_shared<RtpMetaDataSource>();
    }

    if (m_rtpSource)
    {
        m_rtpSource->Initialize(m_url,
                                m_playControlType,
                                m_mediaCallbacks,
                                expectedFrameSize);
        m_rtpSource->Setup(m_streamInfo);
        m_rtpSource->setStartTimePoint(rtspOpenTimePoint);
    }
    else
    {
        SPDLOG_DEBUG("RtpSession::CreateRtpSource() Not supported CODEC, url={}", m_url);
        return false;
    }

    return true;
}

bool RtpSession::CreateRtpSink()
{
    SPDLOG_DEBUG("RtpSession::CreateRtpSink(), codec={}, url={}", m_streamInfo.CodecName, m_url);
    m_rtpSink = std::make_shared<BackChannelRtpSink>((unsigned char)(m_streamInfo.PayloadFormat), m_streamInfo.ClockRate);

    return true;
}

void RtpSession::StartPlaying()
{
    // RTP 세션을 INTERLEAVED로 맺지 않는 경우에는
    // 해당 소켓의 수신동작을 시작시킨다.

    if (!m_rtpSocket)
        return;

    m_rtpSocket->StartPlaying();

    if (!m_isRtcpMux && m_rtcpSocket)
        m_rtcpSocket->StartPlaying();
}

void RtpSession::ReadRawRtpData(const unsigned char* bufPtr, size_t byteTransferred)
{
    if(m_rtpSource)
        m_rtpSource->ReadData(bufPtr, byteTransferred);
}

// RTCP packet types:
const unsigned char RTCP_PT_SR = 200;
const unsigned char RTCP_PT_RR = 201;
const unsigned char RTCP_PT_SDES = 202;
const unsigned char RTCP_PT_BYE = 203;
const unsigned char RTCP_PT_APP = 204;
#define PACKET_UNKNOWN_TYPE 0
#define PACKET_RTP 1
#define PACKET_RTCP_REPORT 2
#define PACKET_BYE 3
#define PACKET_RTCP_APP 4

void RtpSession::ReadRawRtcpData(const unsigned char* bufPtr, size_t byteTransferred)
{
    unsigned char* ptr = (unsigned char*)bufPtr;
    size_t packetSize = byteTransferred;

    unsigned int rtcpHdr = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)bufPtr);

    //PLOG2_TRACE << "RTCP packet: header " << std::hex << std::setw(8) << rtcpHdr;

    if ((rtcpHdr & 0xE0FE0000) != (0x80000000 | (RTCP_PT_SR << 16)) &&
            (rtcpHdr & 0xE0FF0000) != (0x80000000 | (RTCP_PT_APP << 16)))
    {

        SPDLOG_DEBUG("ReadRawRtcpData() not supported RTCP packet: header {}, url={}", rtcpHdr, m_url);
    }

    // Process each of the individual RTCP 'subpackets' in (what may be)
    // a compound RTCP packet.
    int typeOfPacket = PACKET_UNKNOWN_TYPE;
    unsigned int reportSenderSSRC = 0;
    bool packetOK = false;
    bool callByeHandler = false;

    while (1)
    {
        uint8_t rc = (rtcpHdr >> 24) & 0x1F;
        uint8_t pt = (rtcpHdr >> 16) & 0xFF;
        unsigned length = 4 * (rtcpHdr & 0xFFFF); // doesn't count hdr

        ptr += 4; // skip over the header
        packetSize -= 4;

        if (length > packetSize)
            break;

        if (length < 4)
            break;

        length -= 4;
        reportSenderSSRC = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)ptr); ptr += 4; packetSize -= 4;

        bool subPacketOK = false;

        if (pt == RTCP_PT_SR)
        {
            //PLOG2_TRACE << "RTCP SR Report";
            if (length >= 20)
            {
                length -= 20;

                // Extract the NTP timestamp, and note this:
                unsigned int NTPmsw = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)ptr);  ptr += 4; packetSize -= 4;
                unsigned int NTPlsw = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)ptr);  ptr += 4; packetSize -= 4;
                unsigned int rtpTimestamp = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)ptr);  ptr += 4; packetSize -= 4;

                ptr += 8; packetSize -= 8;

                unsigned int reportBlocksSize = rc*(6 * 4);
                if (length >= reportBlocksSize && reportBlocksSize > 0)
                {
                    length -= reportBlocksSize;
                    packetSize -= reportBlocksSize;
                    ptr += reportBlocksSize;
                }
                subPacketOK = true;
                typeOfPacket = PACKET_RTCP_REPORT;

                ProcessSenderReport(reportSenderSSRC, NTPmsw, NTPlsw, rtpTimestamp);
            }

        }
        else if (pt == RTCP_PT_APP)
        {
            //PLOG2_TRACE << "RTCP APP";
            uint8_t& subtype = rc;
            uint32_t nameBytes = (ptr[0] << 24) | (ptr[1] << 16) | (ptr[2] << 8) | (ptr[3]);
            ptr += 4; packetSize -= 4;

            subPacketOK = true;
            typeOfPacket = PACKET_RTCP_APP;
        }
        else if (pt == RTCP_PT_BYE)
        {
            //PLOG2_TRACE << "RTCP BYE";
            subPacketOK = true;
            typeOfPacket = PACKET_BYE;
            callByeHandler = true;
        }
        else if (pt == RTCP_PT_SDES)
        {
            subPacketOK = true;
        }
        else
        {
            subPacketOK = true;
        }

        if (!subPacketOK)
            break;

        ptr += length;
        packetSize -= length;

        if (packetSize == 0)
        {
            packetOK = true;
            break;
        }
        else if (packetSize < 4)
        {
            //PLOG2_TRACE << "extraneous" << packetSize << " bytes at end of RTCP packet!";
            break;
        }
        rtcpHdr = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)ptr);
        if ((rtcpHdr & 0xC0000000) != 0x80000000)
        {
            break;
        }
    }


    if (!packetOK)
    {
        SPDLOG_DEBUG("ReadRawRtcpData() rejected bad RTCP subpacket: header {}, url={}", rtcpHdr, m_url);
    }
}


void RtpSession::ProcessSenderReport(unsigned int SSRC,
                                     unsigned int NTPmsw, unsigned int NTPlsw,
                                     unsigned int rtpTimestamp)
{
    double usec = ((double)NTPlsw * 15625.0) / 0x04000000; // 10^6/2^32
    time_t seconds = NTPmsw - 2208988800; // 1/1/1900 -> 1/1/1970
    unsigned int useconds = (unsigned int)(usec + 0.5);

    time_t tv = static_cast<long long>(seconds)* 1000 + (useconds/1000);    // convert to millisecond

    SPDLOG_TRACE("RTCP Sender REPORT::SSRC={}, tv={}, rtpTimestamp={}", SSRC, tv, rtpTimestamp);

    if (m_rtpSource && m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE)
    {
        m_rtpSource->SetSynchronizedUsingRTCP(tv, rtpTimestamp);
    }
}

#ifdef WIN32
#define timegm _mkgmtime
#endif

// TODO :: ONVIF 연동시에는 ONVIF 스펙 지원 및 해당 응답 메시지안의 시간정보를 사용하도록 할지, 안할지를 결정할 것.
void RtpSession::SetStartTimeWithPlayResponse(std::string& dateStr)
{
    /*
    HTTP applications have historically allowed three different formats
    for the representation of date/time stamps:

    Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
    Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
    Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format
    Sun, Nov 06 2016 04:40:55 GMT  ; LIVE555 bug
    */

    std::istringstream ss(dateStr);
    std::tm stm = {0};
    // Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
    ss >> std::get_time(&stm, "%a, %d %b %Y %H:%M:%S GMT");
    if (ss.fail())
    {
        // Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format
        std::istringstream ss1(dateStr);
        ss1 >> std::get_time(&stm, "%a %b %d %H:%M:%S %Y");
        if (ss1.fail())
        {
            // Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
            std::istringstream ss2(dateStr);
            ss2 >> std::get_time(&stm, "%A, %d-%b-%y %H:%M:%S GMT");
            if (ss2.fail())
            {
                // LIVE55 Server bug
                std::istringstream ss3(dateStr);
                ss3 >> std::get_time(&stm, "%a, %b %d %Y %H:%M:%S GMT");
                if (ss3.fail())
                {
                    SPDLOG_WARN("DATE Parse failed={}", dateStr);
                    return;
                }
            }
        }
    }

    time_t t = timegm(&stm);
    //SPDLOG_DEBUG("Play Response time_t={}, url={}", t, m_url);

    if (m_rtpSource && m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE)
    {
        m_rtpSource->SetCurrentTimveVal(t*1000);    // convert to millisecond and set
    }
}

}
}

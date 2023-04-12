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

#include <boost/asio/detail/socket_ops.hpp>

#include "LogSettings.h"
#include "RtpSource.h"

namespace Wisenet
{
namespace Rtsp
{

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-RTPS] "}, level, __VA_ARGS__)


using namespace boost::asio::detail::socket_ops;


RtpSource::RtpSource()
    : m_currentPacketBeginFrame(true)
    , m_currentPacketCompleteFrame(true)
    , m_markerBit(false)
    , m_currentPacketPtr(nullptr)
    , m_currentPacketSize(0)
    , m_currentExtensionSize(0), m_currentExtensionOffset(0)
    , m_currentRtpTimeStamp(0)
    , m_currentSeq(0)
    , m_currentSSRC(0)
    , m_lastTimeStamp(0), m_currentTimeStamp(0)
    , m_RtpExtensionSeq(0), m_lastRtpExtensionSeq(0)
    , m_currentFrameSequence(0)
    , m_checkExtensionHeader(true)
    , m_currentTimeVal(0)
    , m_payloadFormat(0)
    , m_clockRate(0)
    , m_playControlType(RtspPlayControlType::RTSP_PLAY_LIVE)
{
    SPDLOG_DEBUG("RtpSource()");
    m_isClosed = false;
}

RtpSource::~RtpSource()
{
    SPDLOG_DEBUG("~RtpSource(), url={}", m_url);
}

void RtpSource::Close()
{
    SPDLOG_DEBUG("Close(), url={}", m_url);
    if (m_isClosed) return;
    m_isClosed = true;
    // 쓰레드 동시접근으로 비정상종료 문제 발생 가능성 있음.
    // MediaAgent에서 weak_ptr로 처리하므로 여기서는 nullptr 처리부분을 뺌.
//    m_mediaCallbacks = nullptr;
}

void RtpSource::Initialize(
    const std::string &url,
    const RtspPlayControlType playControlType,
    Media::MediaSourceFrameHandler& callback,
    const size_t expectedFrameSize)
{
    m_url = url;
    m_playControlType = playControlType;
    m_mediaCallbacks = callback;
    m_mediaSourceDataPtr = std::make_shared<MediaFrame>(expectedFrameSize);
}

void RtpSource::Setup(MediaStreamInfo &streamInfo)
{
    if (streamInfo.CodecName == "")
    {
        if (streamInfo.PayloadFormat == 26)
            streamInfo.CodecName = "JPEG";
    }
    m_codecName = streamInfo.CodecName;

    m_payloadFormat = streamInfo.PayloadFormat;
    m_clockRate = streamInfo.ClockRate;

    SPDLOG_DEBUG("Setup(), url={}, codec={}, payload={}, clockrate={}", m_url, m_codecName, m_payloadFormat, m_clockRate);
}

void RtpSource::ReadData(const unsigned char* bufPtr, size_t byteTransferred)
{
    // Check for the 12-byte RTP header:
    if (byteTransferred < 12)
    {
        SPDLOG_DEBUG("ReadData() Rtp should be bigger than 12 bytes:: bytes={}, url={}", byteTransferred, m_url);
        return;
    }


    /* parse rtp header
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |V=2|P|X|  CC   |M|     PT      |       sequence number         |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                           timestamp                           |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |           synchronization source (SSRC) identifier            |
    +=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+=+
    |            contributing source (CSRC) identifiers             |
    |                             ....                              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    */
    size_t rtpPacketSize = byteTransferred;
    size_t offset = 0;
    uint32_t rtpHeader = network_to_host_long(*(uint32_t*)(bufPtr + offset)); offset += 4;
    m_currentRtpTimeStamp = network_to_host_long(*(uint32_t*)(bufPtr + offset)); offset += 4;
    m_currentSSRC = network_to_host_long(*(uint32_t*)(bufPtr + offset)); offset += 4;

    m_currentPacketPtr = (unsigned char*)(bufPtr);
    m_currentPacketSize = byteTransferred;


    // check version (2)
    if ((rtpHeader & 0xC0000000) != 0x80000000)
    {
        SPDLOG_DEBUG("ReadData() RTP version is not 2, url={}", m_url);
        return;
    }

    // marker bit
    m_markerBit = (rtpHeader & 0x00800000) != 0;

    // payload type
    unsigned char payloadType = (unsigned char)((rtpHeader & 0x007F0000) >> 16);

    // sequence number
    uint16_t expectedSeq = m_currentSeq + 1;
    m_currentSeq = (uint16_t)(rtpHeader & 0xFFFF);

    if (expectedSeq != m_currentSeq)
    {
        SPDLOG_DEBUG("ReadData() Initialize buffer for RTP SEQ NO...expectedSeq={}, currentSeq={}, url={}",
                     expectedSeq, m_currentSeq, m_url);
        m_mediaSourceDataPtr->Reset();
        m_currentFrameSequence++;
    }

    // CSRC count
    uint32_t cc = (rtpHeader >> 24) & 0x0F;
    offset += (cc * 4); // skip CSRC identifiers

    /* RTP header extension (X)
    0                   1                   2                   3
    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |      defined by profile       |           length              |
    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    |                        header extension                       |
    |                             ....                              |
    */
    m_currentExtensionSize = 0;
    m_currentExtensionOffset = 0;


    if (rtpHeader & 0x10000000)
    {
        m_currentExtensionOffset = offset;
        uint32_t rtpExtensionHeader = network_to_host_long(*(uint32_t*)(bufPtr + offset)); offset += 4;
        m_currentExtensionSize = 4 * (rtpExtensionHeader & 0xFFFF);

        bool checkHeader = true;
        if (m_checkExtensionHeader == true ) //video
        {
            // 저장장치에서 garbage extension을 올려주는 경우가 있음.
            if (m_currentExtensionSize > byteTransferred)
            {
                SPDLOG_DEBUG("ReadData() Extension header size is too big, ignore it... ex_size={}, url={}",
                             m_currentExtensionSize, m_url);
                m_currentExtensionSize = 0;
                offset = m_currentExtensionOffset;
                m_currentExtensionOffset = 0;
                checkHeader = false;
            }
        }
        else if (m_currentExtensionSize != kRtpPlaybackExtHeaderSize)	//audio, meta
        {
            m_currentExtensionSize = 0;
            offset = m_currentExtensionOffset;
            m_currentExtensionOffset = 0;
            checkHeader = false;
        }

        if (checkHeader)
        {
            offset += m_currentExtensionSize;

            if (m_currentExtensionSize >= kRtpPlaybackExtHeaderSize &&
                (m_playControlType != RtspPlayControlType::RTSP_PLAY_LIVE &&
                m_playControlType != RtspPlayControlType::RTSP_PLAY_AUDIO_BACKCHANNEL))
            {
                unsigned char *extPtr = m_currentPacketPtr + m_currentExtensionOffset;
                uint32_t profile = (rtpExtensionHeader >> 16);

                extPtr += 4;
                // ONVIF EXTENSION : 0xabac 요즘
                // SUNAPI EXTENSION : 0xabad 옜날 + timezone
                if (profile == 0xabac || profile == 0xabad || profile == 0x4D48)
                {
                    unsigned int NTPmsw = network_to_host_long(*(unsigned int *)extPtr);
                    unsigned int NTPlsw = network_to_host_long(*(unsigned int *)(extPtr + 4));
                    double usec = ((double)NTPlsw * 15625.0) / 0x04000000; // 10^6/2^32

                    m_RtpExtensionSeq = *(char *)(extPtr + 9);

                    int64_t seconds = NTPmsw - 2208988800; // 1/1/1900 -> 1/1/1970
                    int useconds = (unsigned int)(usec + 0.5);
                    m_currentTimeVal = static_cast<long long>(seconds)*1000 + (useconds/1000);

                     //SUNAPI EXTENSION
                    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA ||
                            m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_CAMERA_LC /* &&  profile == 0xabad*/)
                    {
                        // Timezone offset : 녹화된 시점의 timezone 정보를 전달해준다.
                        signed short timezone_offset = network_to_host_short(*(short *)(extPtr + 10));
                        m_currentTimeVal += (timezone_offset * 60 * 1000);
                    }
                    else if (profile == 0x4D48)
                    {
                        m_currentTimeVal = static_cast<long long>(seconds)*1000 + (NTPlsw/1000);
                    }
                }
            }
        }
    }

    // discard padding bytes
    if (rtpHeader & 0x20000000)
    {
        // The last octet of the padding contains a count of how
        // many padding octets should be ignored, including itself.
        unsigned char paddingBytes = bufPtr[byteTransferred - 1];
        rtpPacketSize -= paddingBytes;
    }

    //SPDLOG_DEBUG("RTP byteTransferred={}, RTP Size={}, Seq={}, Timestamp={}, SSRC={}, MARKER={}",
    //    byteTransferred, rtpPacketSize, m_currentSeq, m_currentRtpTimeStamp, m_currentSSRC, m_markerBit);

    if (offset >= rtpPacketSize)
    {
        //PLOG_DEBUG << "Invalid RTP Packet...offset=" << offset << "rtpPacketSize=" << rtpPacketSize;
        return;
    }

    unsigned char *p = m_currentPacketPtr + offset;
    size_t psize = rtpPacketSize - offset;
    size_t skipBytes = 0;
    size_t remainBytes = 0;

    if (ProcessSpecialHeader(p, psize, skipBytes) == false)
    {
        SPDLOG_DEBUG("RtpSource::ReadData() ProcessSpecialHeader() fail.. skip it. url={}", m_url);
        return;
    }

    if (m_mediaSourceDataPtr->FrameSize() == 0 && !m_currentPacketBeginFrame)
    {
        SPDLOG_DEBUG("RtpSource::ReadData() It's not begine frame.. skip it. url={}", m_url);
        return;
    }

    if (psize <= skipBytes)
    {
        SPDLOG_DEBUG("RtpSource::ReadData() skipBytes is too big.. skip, psize={}, skipbytes={}, url={}",
                     psize, skipBytes, m_url);
        return;
    }

    p += skipBytes;
    psize -= skipBytes;

    do {
        if (AddData(p, psize, remainBytes) == true)
        {
            double ms = (double)(m_currentRtpTimeStamp) / (m_clockRate / 1000);

            // RTP TIME STAMP를 그대로 사용한다.
            m_currentTimeStamp = (unsigned int)(ms + 0.5);

            if (m_lastTimeStamp == 0)
                m_lastTimeStamp = m_currentTimeStamp;

            if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE)
            {
                if (!m_rtcp.IsSet)
                {
                    long long diffTimestamp = (long long)m_currentTimeStamp - (long long)m_lastTimeStamp;
                    m_currentTimeVal += diffTimestamp;
                }
                else
                {
                    long long rtpTimeStampDiff = (long long)m_currentRtpTimeStamp - (long long)m_rtcp.RtpTimeStamp;
                    double us = (double)((rtpTimeStampDiff) / (m_clockRate / 1000)) * 1000;
                    long long x = (long long)(us + 0.5);

                    m_currentTimeVal = static_cast<long long>(m_rtcp.NTP) + (x/1000);

                    /*if(payloadType == 'b')
                    PLOG_DEBUG << "PAYLOAD[" << payloadType << "] rtpTimeStampDiff[" << rtpTimeStampDiff << "] m_currentRtpTimeStamp[" << m_currentRtpTimeStamp
                        << "] m_rtcp.RtpTimeStamp[" << m_rtcp.RtpTimeStamp << "]";*/
                }
            }

            if (!m_isClosed)
            {
                m_currentFrameSequence++;
                if (m_mediaCallbacks) {
                    CallbackData();
                }
            }

            m_mediaSourceDataPtr->Reset();
            m_lastTimeStamp = m_currentTimeStamp;
            m_lastRtpExtensionSeq = m_RtpExtensionSeq;
        }

        if (remainBytes > 0)
        {
            //PLOG_DEBUG << "remain bytes=" << remainBytes << ", psize=" << psize;
            if (remainBytes >= psize)
                break;
            size_t nextOffset = psize - remainBytes;
            p += nextOffset;
            psize = remainBytes;
        }
    } while (remainBytes > 0);
}

bool RtpSource::ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes)
{
    m_currentPacketCompleteFrame = true;
    retSkipBytes = 0;
    return true;
}

bool RtpSource::AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes)
{
    remainBytes = 0;
    m_mediaSourceDataPtr->AddData(dataPtr, dataSize);
    if (m_currentPacketCompleteFrame)
    {
        return true;
    }
    return false;
}

void  RtpSource::SetCurrentTimveVal(const int64_t& t)
{
    m_currentTimeVal = t;
    m_currentTimeStamp = 0;
    m_lastTimeStamp = 0;
}

void RtpSource::SetSynchronizedUsingRTCP(const int64_t& ntp,
    const unsigned int rtpTimeStamp)
{
    m_rtcp.IsSet = true;
    m_rtcp.NTP = ntp;
    m_rtcp.RtpTimeStamp = rtpTimeStamp;
}

}
}

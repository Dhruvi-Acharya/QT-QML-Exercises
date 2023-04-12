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

#include "memory"
#include "vector"
#include "boost/asio/detail/socket_ops.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


namespace Wisenet
{
namespace Rtsp
{


struct RtpPacket
{
public:
    RtpPacket(unsigned char payloadType, bool markerBit,
        unsigned short seq,
        unsigned int timestamp, unsigned int ssrc,
        unsigned char* src, size_t srcSize)
    {
        m_seq = seq;
        // RTP version 2; marker ('M') bit not set (by default; it can be set later)
        unsigned int rtpHdr = 0x80000000;
        rtpHdr |= (payloadType << 16);
        rtpHdr |= seq;
        if (markerBit)	rtpHdr |= 0x00800000;

        m_bufferSize = 12 + srcSize;
        m_buffer = new unsigned char[m_bufferSize];

        uint32_t word = boost::asio::detail::socket_ops::host_to_network_long(rtpHdr);
        std::memcpy(m_buffer, &word, sizeof(uint32_t));

        word = boost::asio::detail::socket_ops::host_to_network_long(timestamp);
        std::memcpy(m_buffer + 4, &word, sizeof(uint32_t));

        word = boost::asio::detail::socket_ops::host_to_network_long(ssrc);
        std::memcpy(m_buffer + 8, &word, sizeof(uint32_t));

        std::memcpy(m_buffer + 12, src, srcSize);
    }

    ~RtpPacket()
    {
        delete[] m_buffer;
    }

    unsigned char* Buffer()
    {
        return m_buffer;
    }
    size_t BufferSize()
    {
        return m_bufferSize;
    }
    unsigned short Seq()
    {
        return m_seq;
    }

private:
    unsigned short	m_seq;
    unsigned char * m_buffer;
    size_t			m_bufferSize;
};

typedef std::shared_ptr<RtpPacket> RtpPacketPtr;

class BackChannelRtpSink
{
public:
    BackChannelRtpSink(unsigned char payloadType, unsigned int clockRate);
    ~BackChannelRtpSink();

    const static int kMaxRtpPayloadSize = 1456;
    const static int kMaxRtpDataSize = 1444; // -12

    void ClearPakcets();
    void BuildPackets(unsigned char *buffer, size_t bufferSize);
    std::vector<RtpPacketPtr>& Packets();

private:
    std::vector<RtpPacketPtr> m_packets;

    unsigned char m_payloadType;
    unsigned int m_clockRate;

    unsigned int m_currentTimestamp;
    unsigned int m_currentSeq;
    unsigned int m_ssrc;

    bool m_isFirst;
    boost::posix_time::ptime m_lastTime;
};

typedef std::shared_ptr<BackChannelRtpSink> BackChannelRtpSinkPtr;


}
}

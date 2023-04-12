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

#include "RtpSession/BackChannelRtpSink.h"


namespace Wisenet
{
namespace Rtsp
{


BackChannelRtpSink::BackChannelRtpSink(unsigned char payloadType, unsigned int clockRate)
    : m_payloadType(payloadType)
    , m_clockRate(clockRate)
    , m_currentTimestamp(10000)
    , m_currentSeq(2000)
    , m_ssrc(0x481ad8d6)
    , m_isFirst(true)
{

}

BackChannelRtpSink::~BackChannelRtpSink()
{

}


void BackChannelRtpSink::ClearPakcets()
{
    m_packets.clear();
}

std::vector<RtpPacketPtr>& BackChannelRtpSink::Packets()
{
    return m_packets;
}

void BackChannelRtpSink::BuildPackets(unsigned char *buffer, size_t bufferSize)
{
    if (m_isFirst)
    {
        m_isFirst = false;
        m_lastTime = boost::posix_time::microsec_clock::local_time();
    }
    else
    {
        boost::posix_time::ptime now = boost::posix_time::microsec_clock::local_time();
        boost::posix_time::time_duration msdiff = now - m_lastTime;

        auto millSec = msdiff.total_milliseconds();
        double timestamp = m_clockRate * millSec / 1000;
        m_currentTimestamp += (unsigned int)(timestamp+0.5);

        m_lastTime = now;
    }

    size_t remainSize = bufferSize;
    unsigned char* offset = buffer;
    //m_currentTimestamp += 2000;
    while (remainSize > 0)
    {
        size_t sendBytes = 0;
        bool markerbit = false;
        if (remainSize > kMaxRtpDataSize)
        {
            sendBytes = kMaxRtpDataSize;
        }
        else
        {
            sendBytes = remainSize;
            markerbit = true;
        }

        //OLOG_INFO << "sequence=" << m_currentSeq;
        RtpPacketPtr rtpPacket = std::make_shared<RtpPacket>(
                    m_payloadType,
                    markerbit,
                    m_currentSeq,
                    m_currentTimestamp,
                    m_ssrc,
                    offset,
                    sendBytes);

        m_currentSeq++;

        offset += sendBytes;
        m_packets.push_back(rtpPacket);
        remainSize -= sendBytes;
    }
}


}
}

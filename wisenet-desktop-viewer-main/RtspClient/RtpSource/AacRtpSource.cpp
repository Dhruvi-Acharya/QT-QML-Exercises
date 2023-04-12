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

#include <fstream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>

#include "LogSettings.h"
#include "AacRtpSource.hpp"


namespace Wisenet
{
namespace Rtsp
{

AacRtpSource::AacRtpSource()
    : m_sizeLength(0)
    , m_indexLength(0)
    , m_indexDeltaLength(0)
{

}

void AacRtpSource::Setup(MediaStreamInfo &streamInfo)
{
    RtpAudioSource::Setup(streamInfo);

    m_formatParams = streamInfo.FormatParams;
    auto slitr = m_formatParams.find("sizelength");
    auto ilitr = m_formatParams.find("indexlength");
    auto idlitr = m_formatParams.find("indexdeltalength");
    auto modeitr = m_formatParams.find("mode");
    auto bitrateitr = m_formatParams.find("bitrate");

    if (slitr != m_formatParams.end())
    {
        boost::trim(slitr->second);
        boost::conversion::try_lexical_convert<unsigned int>(slitr->second, m_sizeLength);
    }
    if (ilitr != m_formatParams.end())
    {
        boost::trim(ilitr->second);
        boost::conversion::try_lexical_convert<unsigned int>(ilitr->second, m_indexLength);
    }
    if (idlitr != m_formatParams.end())
    {
        boost::trim(idlitr->second);
        boost::conversion::try_lexical_convert<unsigned int>(idlitr->second, m_indexDeltaLength);
    }
    if (modeitr != m_formatParams.end())
    {
        m_mode = modeitr->second;
    }
    if (bitrateitr != m_formatParams.end())
    {
        boost::trim(bitrateitr->second);
        boost::conversion::try_lexical_convert<unsigned int>(bitrateitr->second, m_bitRate);
    }
}

AacRtpSource::~AacRtpSource()
{

}


bool AacRtpSource::ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes)
{
    unsigned char *ptr = dataPtr;
    size_t resultSpecialHeaderSize = 0;

    // whether the *previous* packet ended a frame
    m_currentPacketBeginFrame = m_currentPacketCompleteFrame;

    // The RTP "M" (marker) bit indicates the last fragment of a frame:
    m_currentPacketCompleteFrame = m_markerBit;


    if (m_sizeLength > 0)
    {
        // The packet begins with a "AU Header Section".  Parse it, to
        // determine the "AU-header"s for each frame present in this packet:
        resultSpecialHeaderSize += 2;

        if (rtpDataSize < resultSpecialHeaderSize)
        {
            SPDLOG_DEBUG("[AAC][{}] Invalid rtp data size={}", m_url, rtpDataSize);
            return false;
        }

        unsigned int AU_headers_length = (ptr[0] << 8) | ptr[1];
        unsigned int AU_headers_length_bytes = (AU_headers_length + 7) / 8;
        resultSpecialHeaderSize += AU_headers_length_bytes;

        // TODO..LIVE555에서 구현한 부분들을 추가할 것인가?

    }

    retSkipBytes = resultSpecialHeaderSize;
    return true;
}


}
}

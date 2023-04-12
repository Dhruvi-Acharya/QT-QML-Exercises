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

#ifndef OSCAR_MJPEG_RTP_SOURCE_HPP
#define OSCAR_MJPEG_RTP_SOURCE_HPP

#pragma once

#include "RtpVideoSource.hpp"

namespace Wisenet
{
namespace Rtsp
{

class MjpegRtpSource : public RtpVideoSource
{
public:
    MjpegRtpSource();
    ~MjpegRtpSource();

    void Setup(MediaStreamInfo &streamInfo);

private:
    bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);
    bool AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes);

private:
    enum { kRtpPlaybackJpegExtHeaderSize = 16 };
    enum { kSofMarkerHeaderSize = 19 };
    enum { kSosSegmentHeaderSize = 12 };
    enum { kMaxJpegHeaderSize = 1024 };

    bool m_useSofMarker;
    bool m_useSosSegment;
    unsigned char	m_sofMarker[kSofMarkerHeaderSize];
    unsigned char	m_sosSegment[kSosSegmentHeaderSize];
    unsigned char	m_jpegHeader[kMaxJpegHeaderSize];
protected:
    uint32_t	m_miObjectID;
};

}
}

#endif

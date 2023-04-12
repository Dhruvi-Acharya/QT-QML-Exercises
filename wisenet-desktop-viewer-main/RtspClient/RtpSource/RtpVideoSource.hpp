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

#ifndef OSCAR_RTP_VIDEO_SOURCE_HPP
#define OSCAR_RTP_VIDEO_SOURCE_HPP

#include "RtpSource.h"


namespace Wisenet
{
namespace Rtsp
{


class RtpVideoSource : public RtpSource
{
public:
	RtpVideoSource();
	virtual ~RtpVideoSource();

	virtual void Setup(MediaStreamInfo &streamInfo);

protected:
    Media::VideoCodecType m_videoCodec;
    Media::VideoFrameType m_currentVideoFrameType;
	int m_currentVideoWidth;
	int m_currentVideoHeight;
    float m_frameRate;

private:
    // 통계 측정 값
    std::chrono::steady_clock::time_point m_videoStatClock;
    std::chrono::steady_clock::time_point m_lastVideoClock;
    size_t m_statVideoReceivedCnt = 0;
    float m_avgVideoGap = 0;
    size_t m_maxVideoGapMsec = 0;
    size_t m_minVideoGapMsec = 0;
    bool m_isFirstVideoFrame = true;

	virtual void CallbackData();
    void Statistics();
    void InitStatistics();

};


}
}

#endif

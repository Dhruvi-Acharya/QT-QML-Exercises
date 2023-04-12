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

#include "RtpVideoSource.hpp"
#include "LogSettings.h"

namespace Wisenet
{
namespace Rtsp
{

RtpVideoSource::RtpVideoSource()
    : m_frameRate(0)
    , m_currentVideoFrameType(Media::VideoFrameType::UNKNOWN)
    , m_videoCodec(Media::VideoCodecType::UNKNOWN)
    , m_currentVideoWidth(0)
    , m_currentVideoHeight(0)
{

}


RtpVideoSource::~RtpVideoSource()
{
}

void RtpVideoSource::Setup(MediaStreamInfo &streamInfo)
{
    RtpSource::Setup(streamInfo);
    if (m_clockRate == 0)
    {
        //PLOG_WARN << "Clockrate is 0, assume 90000, codecName=" << m_codecName;
        m_clockRate = 90000;
    }

    m_currentVideoWidth = streamInfo.VideoWidth;
    m_currentVideoHeight = streamInfo.VideoHeight;
    m_frameRate = streamInfo.FrameRate;

    if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE) {
        InitStatistics();
    }

    //PLOG_DEBUG << "Default vide width=" << m_currentVideoWidth << ", height=" << m_currentVideoHeight;
}

void RtpVideoSource::CallbackData()
{
    if (m_mediaCallbacks)
    {
        if (m_playControlType == RtspPlayControlType::RTSP_PLAY_LIVE) {
            Statistics();
        }

        Media::VideoSourceFrameSharedPtr videoFrame = std::make_shared<Media::VideoSourceFrame>(m_mediaSourceDataPtr->Ptr(), m_mediaSourceDataPtr->FrameSize());
        videoFrame->sourceType = Media::MediaSourceType::RTP;

        videoFrame->frameTime.ptsTimestampMsec = m_currentTimeVal;
        videoFrame->frameTime.rtpTimestampMsec = m_currentTimeStamp;

        // NVR playback 재생시, p프레임부터 데이터가 날아온 경우
        if (m_playControlType == RtspPlayControlType::RTSP_PLAY_PB_SUNAPI_NVR &&
            m_lastRtpExtensionSeq != m_RtpExtensionSeq &&
            m_currentVideoFrameType != Media::VideoFrameType::I_FRAME)
        {
            //PLOG_DEBUG << "+++ m_lastRtpExtensionSeq" << (unsigned int)m_lastRtpExtensionSeq << ", m_RtpExtensionSeq =" << (unsigned int)m_RtpExtensionSeq;
            m_currentFrameSequence++;
        }

        videoFrame->frameSequence = m_currentFrameSequence;
        videoFrame->commandSeq = m_RtpExtensionSeq;

        videoFrame->videoFrameType = m_currentVideoFrameType;
        videoFrame->videoCodecType = m_videoCodec;
        videoFrame->videoWidth = m_currentVideoWidth;
        videoFrame->videoHeight = m_currentVideoHeight;
        videoFrame->frameRate = m_frameRate;

        m_mediaCallbacks(videoFrame);
    }
}

void RtpVideoSource::InitStatistics()
{
    m_isFirstVideoFrame = true;
    m_statVideoReceivedCnt = 0;
    m_maxVideoGapMsec = 0;
    m_minVideoGapMsec = 0;
}

void RtpVideoSource::Statistics()
{
    if (!Wisenet::Common::statLoggerRaw()->should_log(spdlog::level::info)) {
        return;
    }

    int m_statClockPeriodMsec = 300000; // 5min for info level (default)
    if (Wisenet::Common::statLoggerRaw()->should_log(spdlog::level::debug)) {
        m_statClockPeriodMsec = 1000; // 1sec for debug level
    }

    auto now = std::chrono::steady_clock::now();
    if (m_isFirstVideoFrame) {
        m_isFirstVideoFrame = false;
        auto openGapMsec = (size_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - m_startPoint).count();
        STATLOG_INFO("[RTP-V LIVE] url={}, first video time={} ms elapsed, {}, {}x{}",
                     m_url, openGapMsec, m_codecName, m_currentVideoWidth, m_currentVideoHeight);
        m_videoStatClock = now;
        m_lastVideoClock = now;
        return;
    }

    m_statVideoReceivedCnt++;
    auto frameGapMsec = (size_t)std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastVideoClock).count();
    if (frameGapMsec > m_maxVideoGapMsec) {
        m_maxVideoGapMsec = frameGapMsec;
    }
    if (frameGapMsec < m_minVideoGapMsec || m_minVideoGapMsec == 0) {
        m_minVideoGapMsec = frameGapMsec;
    }
    //STATLOG_DEBUG("[RTP-V LIVE] gap={}, min={}, max={}", frameGapMsec, m_minVideoGapMsec, m_maxVideoGapMsec);
    m_lastVideoClock = now;

    auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_videoStatClock).count();
    if (elapsedMsec > m_statClockPeriodMsec) {
        STATLOG_INFO("[RTP-V LIVE] url={}, elasped={} ms, {}, {}x{}, {} frames, max={}, min={}",
                    m_url, elapsedMsec, m_codecName, m_currentVideoWidth, m_currentVideoHeight,
                    m_statVideoReceivedCnt, m_maxVideoGapMsec, m_minVideoGapMsec);

        m_videoStatClock = now;
        m_statVideoReceivedCnt = 0;
        m_maxVideoGapMsec = 0;
        m_minVideoGapMsec = 0;
    }
}

}
}

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

#include "FFmpegStreamInfo.h"

FFmpegStreamInfo::FFmpegStreamInfo()
    : m_stream(nullptr)
    , m_streamType(StreamType::STREAM_UNKNOWN)
    , m_currentTimeStamp(0), m_previousTimeStamp(0), m_previousDuration(0)
    , m_dts(0)
    , m_isFirstTimeStamp(true)
{

}

void FFmpegStreamInfo::Reset(AVStream *stream, StreamType type)
{
    Reset();
    m_stream = stream;
    m_streamType = type;

    m_stream->start_time = 0;
}

void FFmpegStreamInfo::Reset()
{
    m_stream = nullptr;
    m_streamType = StreamType::STREAM_UNKNOWN;
    m_currentTimeStamp = m_previousTimeStamp = m_previousDuration = 0;
    m_dts = 0;
    m_isFirstTimeStamp = true;
}

void FFmpegStreamInfo::set_time_base(const int num, const int den)
{
    if (m_stream)
    {
        m_stream->time_base.num = num;
        m_stream->time_base.den = den;
        m_stream->start_time = 0;
    }
}

int64_t FFmpegStreamInfo::UpdateTimeStamp(int64_t currentTimeStamp)
{
    if (m_isFirstTimeStamp)
    {
        m_previousTimeStamp = currentTimeStamp;
        m_currentTimeStamp = currentTimeStamp;
        m_isFirstTimeStamp = false;
    }
    else
    {
        m_previousTimeStamp = m_currentTimeStamp;
        m_currentTimeStamp = currentTimeStamp;
    }

    return m_currentTimeStamp;
}

int64_t FFmpegStreamInfo::UpdateDts(int64_t duration)
{
    m_previousDuration = duration;
    m_dts += duration;

    return m_dts;
}

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
#include "FFmpegStruct.h"

class FFmpegStreamInfo
{
public:
    enum class StreamType
    {
        STREAM_UNKNOWN,
        STREAM_VIDEO,
        STREAM_AUDIO,
        STREAM_SUBTITLE
    };

    FFmpegStreamInfo();

    void Reset(AVStream *stream, StreamType type);
    void Reset();
    void set_time_base(const int num, const int den);
    void SetDts(int64_t dts) { m_dts = dts; }
    int64_t UpdateTimeStamp(int64_t currentTimeStamp);
    int64_t UpdateDts(int64_t duration);

    bool IsFirstTimeStamp() const { return m_isFirstTimeStamp; }
    int64_t GetCurrentTimeStamp() { return m_currentTimeStamp; }
    int64_t GetPreviousTimeStamp() { return m_previousTimeStamp; }
    int64_t GetPreviousDuration() { return m_previousDuration; }
    int64_t GetDurationTimeStamp() { return m_currentTimeStamp - m_previousTimeStamp; }
    int64_t GetDts() { return m_dts; }
    AVStream* GetStream() { return m_stream; }
    StreamType GetType() { return m_streamType; }

private:
    bool m_isFirstTimeStamp;
    AVStream* m_stream;
    StreamType m_streamType;
    int64_t m_currentTimeStamp;
    int64_t m_previousTimeStamp;
    int64_t m_previousDuration;
    int64_t m_dts;
};

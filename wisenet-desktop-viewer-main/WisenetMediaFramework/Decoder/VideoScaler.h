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

#include "BaseDecoder.h"

struct SwsContextInfo
{
    void reset()
    {
        SourceWidth = 0;
        SourceHeight = 0;
        SourceFormat = AVPixelFormat::AV_PIX_FMT_NONE;
        OutWidth = 0;
        OutHeight = 0;
        OutFormat = AVPixelFormat::AV_PIX_FMT_NONE;
    }
    bool equal(const int srcWidth, const int srcHeight, const AVPixelFormat srcFormat,
               const int outWidth, const int outHeight, const AVPixelFormat outFormat)
    {
        return (srcFormat == SourceFormat &&
                srcWidth == SourceWidth &&
                srcHeight == SourceHeight &&
                outFormat == OutFormat &&
                outWidth == OutWidth &&
                outHeight == OutHeight );

    }
    void reset(const int srcWidth, const int srcHeight, const AVPixelFormat srcFormat,
               const int outWidth, const int outHeight, const AVPixelFormat outFormat)
    {
        SourceWidth = srcWidth;
        SourceHeight = srcHeight;
        SourceFormat = srcFormat;
        OutWidth = outWidth;
        OutHeight = outHeight;
        OutFormat = outFormat;
    }

    int             SourceWidth = 0;
    int             SourceHeight = 0;
    AVPixelFormat   SourceFormat = AVPixelFormat::AV_PIX_FMT_NONE;
    int             OutWidth = 0;
    int             OutHeight = 0;
    AVPixelFormat   OutFormat = AVPixelFormat::AV_PIX_FMT_NONE;

};

class VideoScaler
{
public:
    VideoScaler();
    ~VideoScaler();

    VideoOutputFrameSharedPtr getScaledData(AVFrame* srcFrame, const VideoOutParam &outParam);
    VideoOutputFrameSharedPtr getData(AVFrame *srcFrame, const VideoOutParam &outParam);
private:
    void release();
private:
    SwsContext* m_swsContext = nullptr;
    SwsContextInfo m_swsContextInfo;

};


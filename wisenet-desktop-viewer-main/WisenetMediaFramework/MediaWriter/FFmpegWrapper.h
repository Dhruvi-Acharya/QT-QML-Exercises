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

class AVFrameWrapper
{
public:
    AVFrameWrapper()
        : m_frame(av_frame_alloc())
    {

    }

    ~AVFrameWrapper()
    {
        av_frame_free(&m_frame);
    }

    AVFrame* get() { return m_frame; }

private:
    AVFrame* m_frame;
};

typedef std::shared_ptr<AVFrameWrapper> AVFramePtr;


class AVCodecContextWrapper
{
public:
    AVCodecContextWrapper(const AVCodec *codec)
        : m_codecContext(avcodec_alloc_context3(codec))
    {

    }

    ~AVCodecContextWrapper()
    {
        avcodec_free_context(&m_codecContext);
    }

    AVCodecContext* get() { return m_codecContext; }

private:
    AVCodecContext* m_codecContext;
};

typedef std::shared_ptr<AVCodecContextWrapper> AVCodecContextPtr;


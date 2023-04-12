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
#include <QAudioOutput>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswresample/swresample.h"
#include "libavutil/samplefmt.h"
}

class AudioDecoder : public BaseDecoder
{
    Q_OBJECT
public:
    explicit AudioDecoder(QObject *parent = nullptr);
    ~AudioDecoder();

    Wisenet::Media::MediaType getMediaType() override
    { return Wisenet::Media::MediaType::AUDIO; }

    DecodeResult openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr& codecParam);
    DecodeResult decodeStart(const Wisenet::Media::AudioSourceFrameSharedPtr& audioSourceFrame,
                             AudioOutputFrameSharedPtr& outFrame);
    void flushBuffers() override;
    void release();
private:
    int setupAudioFormat();

    int openInternal(const Wisenet::Media::AudioSourceFrameSharedPtr& audioSourceFrame,
                     const FFmpegCodecParamSharedPtr& codecParam);
    bool checkFrame(const Wisenet::Media::AudioSourceFrameSharedPtr& audioSourceFrame);
    AudioOutputFrameSharedPtr getResampledData(AVFrame *srcFrame);
private:
    AVCodecContext*             m_codecContext;
    SwrContext*                 m_swrContext;
    AVFrame*                    m_frame;
    AVPacket*                   m_packet;
    AVCodecID                   m_lastCodecId = AV_CODEC_ID_NONE;
    int                         m_lastAudioChannels = 0;
    int                         m_lastAudioSampleRate = 0;
    int                         m_lastAudioBitrate = 0;
    bool                        m_useFFmpegCodecParam;

    QAudioFormat                m_audioFormat;
    AVSampleFormat              m_targetSampleFormat = AVSampleFormat::AV_SAMPLE_FMT_NONE;
    int                         m_targetSampleRate = 0;
    uint64_t                    m_targetChannelLayout = AV_CH_LAYOUT_MONO;
    int                         m_targetChannel = 1;
};


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
#include "VideoScaler.h"



class VideoDecoder: public BaseVideoDecoder
{
    Q_OBJECT
public:
    explicit VideoDecoder(QObject *parent = nullptr, const bool lowDelay=false, bool isSequence = false);
    ~VideoDecoder();
    DecodeResult openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr& codecParam) override;
    DecodeResult decodeStart(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame,
                             VideoOutputFrameSharedPtr& outFrame,
                             const bool keepOriginalSize,
                             const int outWidth, const int outHeight,
                             const bool noDelay, std::string& errMsg) override;

    int decodeInternalHw(bool& gotPicture);
    int decodeInternalSw(const bool noDelay, const bool isKeyFrame, const bool eagainFlag, bool& gotPicture);

    void flushBuffers() override;
    void release() override;

    VideoOutputFrameSharedPtr getLastOriginalVideoImage() override;

private:
    const AVCodec* initAvCodec(AVCodecID codecID, const int videoWidth, const int videoHeight, int decoderNotAvailable = 0);
    int openInternal(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame,
                     const FFmpegCodecParamSharedPtr& codecParam, int decoderNotAvailable = 0);

    bool checkFrame(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame);

private:
    VideoScaler                 m_scaler;
    AVCodecContext*             m_codecContext = nullptr;
    AVFrame*                    m_frame = nullptr;
    AVFrame*                    m_swFrame = nullptr;
    AVPacket*                   m_packet = nullptr;
    AVBufferRef*                m_hwDeviceContext = nullptr;

    bool                        m_useFFmpegCodecParam = false;
    int                         m_lastVideoWidth = 0;
    int                         m_lastVideoHeight = 0;
    AVCodecID                   m_lastCodecId = AV_CODEC_ID_NONE;
    bool                        m_noScaler = true;

    FFmpegCodecParamSharedPtr   m_lastCodecParam;
    bool                        m_delayedCodec = false;
    bool                        m_isSequence = false;

    VideoOutputFrameSharedPtr m_lastVideoOutputPtr = nullptr;

    // H/W DECODER BUFFER QUEUE
    std::queue<VideoOutputFrameSharedPtr> m_hwOutputQueue;
};


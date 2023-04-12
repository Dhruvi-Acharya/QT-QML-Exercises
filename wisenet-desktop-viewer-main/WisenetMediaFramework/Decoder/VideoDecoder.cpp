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
#include <iostream>
#include <queue>
#include "VideoDecoder.h"
#include "LogSettings.h"
#include "DecoderManager.h"
#include <QDebug>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-HWV] "}, level, __VA_ARGS__)

#define HW_DECODE_TRACE 0

static int calculate_thread_count(const size_t resolution, const AVCodecID codecId)
{
    int thrCount = 1;

    if (codecId == AV_CODEC_ID_HEVC)
    {
        thrCount = (resolution >= 16000000) ? 8 : (resolution >= 8000000) ? 6 : (resolution >= 5000000) ? 4 :  (resolution >= 2000000) ? 2 : 1;
    }
    else
    {
        thrCount = (resolution >= 24000000) ? 6 : (resolution >= 16000000) ? 5 : (resolution >= 12000000) ? 4 : (resolution >= 8000000) ? 3 : (resolution >= 2000000) ? 2 : 1;
    }
    return thrCount;
}

//#define FFMPEG_TRACE
#define PERF_TRACE
VideoDecoder::VideoDecoder(QObject *parent, const bool lowDelay, bool isSequence)
    : BaseVideoDecoder(parent, lowDelay), m_isSequence(isSequence)
{
    if (Wisenet::Common::statLoggerRaw()->should_log(spdlog::level::debug)) {
        m_checkFrameCount = 300;
    }
#ifdef PERF_TRACE
    m_checkFrameCount = 60;
#endif

    SPDLOG_DEBUG("VideoDecoder()");
    /* just test */
#ifdef FFMPEG_TRACE
    av_log_set_level(AV_LOG_TRACE);
#endif
}

VideoDecoder::~VideoDecoder()
{
    SPDLOG_DEBUG("~VideoDecoder()");
    release();
}

void VideoDecoder::release()
{
    // singleton object에서 resolution count 감소
    DecoderManager::getInstance()->DecoderClosed(m_decodeMethod, m_lastVideoWidth * m_lastVideoHeight, m_isSequence);

    if (m_codecContext) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }

    if(m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }

    if(m_swFrame) {
        av_frame_free(&m_swFrame);
        m_swFrame = nullptr;
    }

    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }

    if (m_hwDeviceContext) {
        av_buffer_unref(&m_hwDeviceContext);
        m_hwDeviceContext = nullptr;
    }

    m_lastVideoWidth = 0;
    m_lastVideoHeight = 0;
    m_lastCodecId = AV_CODEC_ID_NONE;
    m_lastVideoOutputPtr = nullptr;
    m_decodeMethod = VideoDecodeMethod::SW;
    std::queue<VideoOutputFrameSharedPtr> empty;
    std::swap(m_hwOutputQueue, empty);
}

const AVCodec* VideoDecoder::initAvCodec(AVCodecID codecID, const int videoWidth, const int videoHeight, int decoderNotAvailable)
{
    // 일단 SW Decoder가 가능한 지 판단.
    const AVCodec* avCodec = avcodec_find_decoder(codecID);
    if (!avCodec) {
        SPDLOG_WARN("Unavailable video codec, codecID={}/{}", codecID, avcodec_get_name(codecID));
        return nullptr;
    }

    std::string codecName = avcodec_get_name(codecID);
    SPDLOG_DEBUG("initAvCodec() START, codec={}, res={}x{}", codecName, videoWidth, videoHeight);

    // 4x, -1x, step --> SW decoder 사용
    if (m_lowDelay)
        m_decodeMethod = VideoDecodeMethod::SW;
    else
        m_decodeMethod = DecoderManager::getInstance()->GetAvaliableCodecMethod(videoWidth * videoHeight, decoderNotAvailable, m_isSequence);

    int ret_hwdevice_ctx_create = 0;
    if (m_decodeMethod != VideoDecodeMethod::SW && m_decodeMethod != VideoDecodeMethod::QSV)
    {
        // HW context를 연다.
        AVHWDeviceType deviceType = m_decodeMethod == VideoDecodeMethod::CUVID ? AVHWDeviceType::AV_HWDEVICE_TYPE_CUDA :
                                                                                 m_decodeMethod == VideoDecodeMethod::QSV ? AVHWDeviceType::AV_HWDEVICE_TYPE_QSV : AVHWDeviceType::AV_HWDEVICE_TYPE_VIDEOTOOLBOX;

        std::string deviceString = m_decodeMethod == VideoDecodeMethod::CUVID ? "CUVID" :
                                                                                m_decodeMethod == VideoDecodeMethod::QSV ? "QSV" : "VIDEOTOOLBOX";

        ret_hwdevice_ctx_create = av_hwdevice_ctx_create(&m_hwDeviceContext, deviceType, NULL, NULL, 0);

        if (ret_hwdevice_ctx_create < 0)
        {
            SPDLOG_INFO("Can not create hardware device context, use s/w decoding. mediaName={}, hwdeviceType={}, ret={}/{}",
                        m_mediaName, deviceString, ret_hwdevice_ctx_create, av_make_error_stdstring(ret_hwdevice_ctx_create));

            DecoderManager::getInstance()->DecoderClosed(m_decodeMethod, videoWidth * videoHeight, m_isSequence);
            m_decodeMethod = VideoDecodeMethod::SW;
        }
        else
        {
            SPDLOG_INFO("Create hardware device context, mediaName={}, hwdeviceType={}", m_mediaName, deviceString);

            /*
            if (m_decodeMethod == VideoDecodeMethod::CUVID || m_decodeMethod == VideoDecodeMethod::QSV)
            {
                if (m_hwDeviceContext)
                {
                    av_buffer_unref(&m_hwDeviceContext);
                    m_hwDeviceContext = nullptr;
                }
            }*/
        }
    }

    if(m_decodeMethod == VideoDecodeMethod::QSV)
    {
        avCodec = nullptr;
        codecName += "_qsv";
        avCodec = avcodec_find_decoder_by_name(codecName.c_str());
    }
    else if(m_decodeMethod == VideoDecodeMethod::CUVID)
    {
        avCodec = nullptr;
        codecName += "_cuvid";
        avCodec = avcodec_find_decoder_by_name(codecName.c_str());
    }

    if (!avCodec)
    {
        SPDLOG_WARN("avcodec_find_decoder_by_name() faield, codecName={}", codecName);
        DecoderManager::getInstance()->DecoderClosed(m_decodeMethod, videoWidth * videoHeight, m_isSequence);
        return nullptr;
    }

    // Allocate codec context
    m_codecContext = avcodec_alloc_context3(avCodec);

    if (!m_codecContext)
    {
        SPDLOG_INFO("Failed to allocate the video codec context.");
        DecoderManager::getInstance()->DecoderClosed(m_decodeMethod, videoWidth * videoHeight, m_isSequence);
        return nullptr;
    }

    // VideoToolbox H/W context
    if (m_decodeMethod != VideoDecodeMethod::SW && m_decodeMethod != VideoDecodeMethod::QSV)
    {
        m_codecContext->hw_device_ctx = av_buffer_ref(m_hwDeviceContext);
        m_codecContext->get_format = avcodec_default_get_format;
    }

    // CUVID Set output resolution
    if (m_decodeMethod == VideoDecodeMethod::CUVID)
    {
        if (m_outParam.needToResize(videoWidth, videoHeight))
        {
            int outWidth = videoWidth;
            int outHeight = videoHeight;

            if(videoWidth >= videoHeight) {
                outWidth = m_outParam.maximumOutSize == 0 ? m_outParam.outWidth : m_outParam.maximumOutSize;
                outWidth = Align16(outWidth);
                outHeight = outWidth * videoHeight / videoWidth;
                outHeight = outHeight%2 == 0 ? outHeight : outHeight+1;
            }
            else {
                outHeight = m_outParam.maximumOutSize == 0 ? m_outParam.outHeight : m_outParam.maximumOutSize;
                outHeight = Align16(outHeight);
                outWidth = outHeight * videoWidth / videoHeight;
                outWidth = outWidth%2 == 0 ? outWidth : outWidth+1;
            }

            std::stringstream ss;
            ss << outWidth << "x" << outHeight;
            int ret = av_opt_set(m_codecContext->priv_data, "resize", ss.str().c_str(), 0);

            SPDLOG_DEBUG("set resize option::{}, ret={}", ss.str(), ret);
        }
        else
        {
            SPDLOG_DEBUG("no need to set resize option::src={}x{}, dst={}x{}",
                         videoWidth, videoHeight, m_outParam.outWidth, m_outParam.outHeight);
        }
    }

    m_delayedCodec = (avCodec->capabilities & AV_CODEC_CAP_DELAY);

    if (m_lowDelay)
    {
        SPDLOG_DEBUG("Force low delay if possible!");
        m_codecContext->flags |= AV_CODEC_FLAG_LOW_DELAY;
    }

    SPDLOG_DEBUG("initAvCodec(), codecName={}", codecName);
    return avCodec;
}

DecodeResult VideoDecoder::openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr &codecParam)
{
    SPDLOG_DEBUG("openWithFFmpegCodecParam(), mediaName={}", m_mediaName);

    if (openInternal(nullptr, codecParam) < 0)
        return DecodeResult::OpenFail;
    return DecodeResult::Success;
}

int VideoDecoder::openInternal(const Wisenet::Media::VideoSourceFrameSharedPtr& sourceFrame,
                               const FFmpegCodecParamSharedPtr& codecParam, int decoderNotAvailable)
{
    int ret = -1;
    SPDLOG_DEBUG("openInternal(), mediaName={}", m_mediaName);

    if (m_codecContext != NULL)
    {
        SPDLOG_DEBUG("openInternal() release opened decoder context first.");
        release();
    }

    AVCodecParameters *videoCodecParameters = nullptr;

    if (codecParam)
    {
        // reset을 위해서 마지막 codecParam을 저장하고 있는다.
        if (m_lastCodecParam != codecParam)
        {
            m_lastCodecParam = codecParam;
        }

        videoCodecParameters = codecParam->getParam();
        m_useFFmpegCodecParam = true;
    }
    else
    {
        m_useFFmpegCodecParam = false;
    }

    int videoWidth = videoCodecParameters ? videoCodecParameters->width : sourceFrame->videoWidth;
    int videoHeight = videoCodecParameters ? videoCodecParameters->height : sourceFrame->videoHeight;

    AVCodecID codecID = videoCodecParameters ? videoCodecParameters->codec_id : get_vcodec_id(sourceFrame->videoCodecType);

    // Allocate an AVFrame and set its fields to default values
    m_frame = av_frame_alloc();
    if (!m_frame)
    {
        SPDLOG_WARN("Could not allocate frame");
        release();
        return ret;
    }

    m_swFrame = av_frame_alloc();
    if (!m_swFrame)
    {
        SPDLOG_WARN("Could not allocate swFrame");
        release();
        return ret;
    }

    if (codecID == AV_CODEC_ID_NONE)
    {
        SPDLOG_WARN("unknown codecID");
        release();
        return ret;
    }

    // Initialize h/w or s/w decoder context
    auto videoCodec = initAvCodec(codecID, videoWidth, videoHeight, decoderNotAvailable);
    if (!videoCodec)
    {
        release();
        return ret;
    }

    // Copy codec parameters from input stream to output codec context
    if (videoCodecParameters)
    {
        ret = avcodec_parameters_to_context(m_codecContext, videoCodecParameters);
        if (ret < 0)
        {
            SPDLOG_WARN("Failed to copy video codec parameters to decoder context, ret={}", ret);
            release();
            return ret;
        }
    }

    // set thread count for decoding speed (need to calculate with resolution and fps...)
    size_t resolution = videoWidth * videoHeight;
    m_codecContext->thread_count = m_lowDelay ? 1 : calculate_thread_count(resolution, videoCodec->id);

    SPDLOG_DEBUG("Try with thread_count={}", m_codecContext->thread_count);

    m_lastVideoWidth = videoWidth;
    m_lastVideoHeight = videoHeight;

    // Init the decoder
    ret = avcodec_open2(m_codecContext, videoCodec, NULL);
    if (ret < 0)
    {
        int hwNotAvailable = (int)m_decodeMethod;

        SPDLOG_WARN("Failed to open video codec decoder, ret={}, error decoder={}", ret, (int)m_decodeMethod);
        release();

        if (hwNotAvailable == 1 || hwNotAvailable == 2)
        {
            SPDLOG_WARN("Try to open video codec decoder again, error decoder={}", (int)m_decodeMethod);
            return openInternal(sourceFrame, codecParam, hwNotAvailable);
        }

        return ret;
    }

    // initialize packet, set data to NULL
    m_packet = av_packet_alloc();
    m_packet->data = NULL;
    m_packet->size = 0;

    m_lastCodecId = codecID;

    SPDLOG_DEBUG("Success to open video decoder, name={}, codec={}, thread_count={}, "
                 "res={}x{}, low_delay={}, delayedCodec={}, decodeMethod={}",
                 m_mediaName,
                 avcodec_get_name(codecID),
                 m_codecContext->thread_count,
                 m_lastVideoWidth, m_lastVideoHeight, m_lowDelay, m_delayedCodec,
                 getMethodName()
                 );

    return 0;
}


bool VideoDecoder::checkFrame(const Wisenet::Media::VideoSourceFrameSharedPtr &videoSourceFrame)
{
    if (!m_useFFmpegCodecParam)
    {
        AVCodecID codecID = get_vcodec_id(videoSourceFrame->videoCodecType);
        if(codecID != m_lastCodecId)
        {
            SPDLOG_DEBUG("video codec changed. name={}, codec={}, {}",
                         m_mediaName, avcodec_get_name(codecID), avcodec_get_name(m_lastCodecId));
            return false;
        }

        if ((videoSourceFrame->videoWidth != m_lastVideoWidth) ||
                (videoSourceFrame->videoHeight != m_lastVideoHeight))
        {
            SPDLOG_DEBUG("video resolution changed. name={}, new={}x{}, before={}x{}",
                         m_mediaName,
                         videoSourceFrame->videoWidth, videoSourceFrame->videoHeight,
                         m_lastVideoWidth, m_lastVideoHeight);
            return false;
        }
    }
    return true;
}


DecodeResult VideoDecoder::decodeStart(const Wisenet::Media::VideoSourceFrameSharedPtr &videoSourceFrame,
                                       VideoOutputFrameSharedPtr &outFrame,
                                       const bool keepOriginalSize,
                                       const int outWidth, const int outHeight,
                                       const bool noDelay,
                                       std::string &errMsg)
{
    int ret = -1;
    bool gotPicture = false;
    bool isKeyFrame = videoSourceFrame->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME;
    bool retry = true;
    AVFrame *targetFrame = m_frame;
    bool isReopen = false;

    startDecodeTimer();
#if 0
    SPDLOG_DEBUG("decodeStart(), keyFrame={}, seq={}, pSeq={}, size={}",
                 isKeyFrame, videoSourceFrame->frameSequence,
                 videoSourceFrame->commandSeq, videoSourceFrame->getDataSize());
#endif

    bool checkResize = true;
    bool outSizeChanged = false;
    if (m_decodeMethod == VideoDecodeMethod::CUVID && !isKeyFrame) {
        checkResize = false;
    }

    if(checkResize)
        outSizeChanged = setVideoOutSize(outWidth, outHeight, keepOriginalSize);

    if (m_codecContext == NULL || m_frame == NULL)
    {
        if (m_useFFmpegCodecParam)
        {
            SPDLOG_DEBUG("decodeStart() codec context or frame is nullptr");
            goto __EmitReturn;
        }

        if (!isKeyFrame)
        {
            SPDLOG_DEBUG("Not ready to init with key frame.. skip it.");
            goto __EmitReturn;
        }

        ret = openInternal(videoSourceFrame, nullptr);
        if (ret < 0)
        {
            errMsg = "openInternal() failed";
            goto __EmitReturn;
        }
        outSizeChanged = false;
    }

    // 프레임 해상도, 코덱이 변경되었는지 확인해서 변경되었으면 재생성한다.
    if (!checkFrame(videoSourceFrame))
    {
        SPDLOG_DEBUG("Video Frame is changed, reset decoder!, isKeyFrame={}", isKeyFrame);
        if (!isKeyFrame)
        {
            release();
            goto __EmitReturn;
        }

        ret = openInternal(videoSourceFrame, nullptr);
        if (ret < 0)
        {
            errMsg = "openInternal() failed";
            goto __EmitReturn;
        }
        isReopen = true;

    }

    // delay option이 변경되었으면 코덱 옵션을 변경하여 초기화한다.
    // cuvid인 경우 outputsize를 h/w decoder에서 재설정하기 위해서 다시 오픈한다.
    if (!isReopen && ( (outSizeChanged && m_decodeMethod == VideoDecodeMethod::CUVID) || (noDelay != m_lowDelay && isKeyFrame) ) )
    {
        SPDLOG_DEBUG("reset decoder for outsize changes or delay option chagnes, lowDelay={}/{}, outSizeChanged={}",
                     m_lowDelay, noDelay, outSizeChanged);

        m_lowDelay = noDelay;
        if (m_useFFmpegCodecParam)
            ret = openInternal(nullptr, m_lastCodecParam);
        else
            ret = openInternal(videoSourceFrame, nullptr);

        if (ret < 0)
        {
            errMsg = "openInternal() failed";
            goto __EmitReturn;
        }

        isReopen = true;
    }

    m_packet->size = (int)videoSourceFrame->getDataSize();
    m_packet->data = videoSourceFrame->getDataPtr();
    m_packet->pts = videoSourceFrame->frameTime.ptsTimestampMsec;

    m_lastVideoOutputPtr = nullptr;

    if(DecoderManager::getInstance()->isIframeMode() && !isKeyFrame)
        goto __EmitReturn;

    if (m_decodeMethod != VideoDecodeMethod::SW)
    {
        ret = decodeInternalHw(gotPicture);

        if (ret == -542398533 || ret == -17)
        {
            SPDLOG_CRITICAL("decodeInternalHw() failed ret={}", ret);
            VideoDecodeMethod hwNotAvailable = m_decodeMethod;
            release();
            if (m_useFFmpegCodecParam)
                ret = openInternal(nullptr, m_lastCodecParam, (int)hwNotAvailable);
            else
                ret = openInternal(videoSourceFrame, nullptr, (int)hwNotAvailable);

            if (ret < 0)
            {
                errMsg = "openInternal() failed";
                goto __EmitReturn;
            }

            isReopen = true;
        }
    }
    else
    {
        ret = decodeInternalSw(noDelay, isKeyFrame, false, gotPicture);
    }

    if (ret == 0 && gotPicture)
    {
        outFrame = m_lastVideoOutputPtr;
    }

__EmitReturn:
    if (gotPicture)
        ret = 0;

    DecodeResult result = (ret==0) ? DecodeResult::Success : DecodeResult::DecodeFail;

    // Statistics for video decoding time
    bool expired = endDecodeTimer();
    if (expired)
    {
        STATLOG_DEBUG("Decoding/Scaling {} frames avg elapsed MediaName={}, AVG={:.2f} / MAX={} ms, CODEC={}, RES={}x{}",
                      m_checkFrameCount, m_mediaName, m_frameAvg, m_maxDecodeTimeMsec,
                      avcodec_get_name(m_lastCodecId), m_lastVideoWidth, m_lastVideoHeight);
        resetTimer();
    }

    return result;
}

void VideoDecoder::flushBuffers()
{
    if (m_codecContext)
    {
        avcodec_flush_buffers(m_codecContext);
    }
}

int VideoDecoder::decodeInternalSw(const bool noDelay, const bool isKeyFrame, const bool eagainFlag, bool& gotPicture)
{
    if (!m_codecContext) {
        SPDLOG_CRITICAL("decodeInternalSw():: m_codecContext is null!!");
        return -1;
    }

    gotPicture = false;
    int ret = avcodec_send_packet(m_codecContext, m_packet);
    if (ret < 0) {
        SPDLOG_INFO("avcodec_send_packet(SW) FAILED, error={}/{}", ret, av_make_error_stdstring(ret));
        return ret;
    }

    // get all the available frames from the decoder
    ret = avcodec_receive_frame(m_codecContext, m_frame);
    if (ret < 0) {
        // those two return values are special and mean there is no output
        // frame available, but there were no errors during decoding
        if (ret == AVERROR_EOF) {
            SPDLOG_DEBUG("decodeInternalSw():: VIDEO DECODE EOF, SHOULD NOT HAPPEN");
            return 0;
        }
        else if (ret == AVERROR(EAGAIN)) {
            // lowDelay 설정을 해도 바로 나오지 않는 영상들에 대한 예외 처리
            if (!gotPicture && noDelay && isKeyFrame && !eagainFlag) {
                SPDLOG_DEBUG("decodeInternalSw:: VIDEO DECODE EAGAIN, TRY just one onmore time");
                return decodeInternalSw(noDelay, isKeyFrame, true, gotPicture);
            }
            return 0;
        }
        else {
            SPDLOG_DEBUG("avcodec_receive_frame(SW) failed::ret={}/{}", ret, av_make_error_stdstring(ret));
            return ret;
        }
    }
    m_lastVideoOutputPtr = m_scaler.getScaledData(m_frame, m_outParam);
    if (!m_lastVideoOutputPtr) {
        return -1;
    }

    ret = 0;
    gotPicture = true;
    m_lastVideoOutputPtr->ptsTimeMsec = m_frame->pts;
    //m_lastVideoOutputPtr->setPtsTimeMsec(m_frame->pts);

#if DECODE_TRACE
    SPDLOG_DEBUG("[SW_DEC] VIDEO OUT PTS={}, seq={}, ret={}",
                 m_lastVideoOutputPtr->ptsTimeMsec(), m_lastVideoOutputPtr->frameSequence, ret);
#endif
    return 0;
}

/**
 * H/W Decoding
 * @param gotPicture
 * @return
 */
int VideoDecoder::decodeInternalHw(bool& gotPicture)
{
    gotPicture = false;
    if (!m_codecContext) {
        SPDLOG_CRITICAL("decodeInternalHw():: m_hwCodecContext is null!!");
        return -1;
    }

#if HW_DECODE_TRACE
    SPDLOG_DEBUG("decodeInternalHw() START");
#endif
    int loopCount = 0;
    int recvloopCount = 0;
    int ret = 0;
    bool pktConsumed = false;
    AVFrame *hwFrame = NULL;

    while(!pktConsumed)
    {
        loopCount++;
        ret = avcodec_send_packet(m_codecContext, m_packet);
        if (ret == AVERROR(EAGAIN))
        {
            ret = 0;
        }
        else if (ret < 0)
        {
            SPDLOG_ERROR("avcodec_send_packet(HW) fail:: loopCount={} ret={}/{}",
                         loopCount, ret, av_make_error_stdstring(ret));;
            goto __END;
        }
        else
        {
            pktConsumed = 1;
        }

        while (ret >= 0)
        {
            //av_frame_unref(m_frame);

            hwFrame = av_frame_alloc();
            if (!hwFrame)
            {
                SPDLOG_ERROR("av_frame_alloc(HW) failed");
                ret =  AVERROR(ENOMEM);
                goto __END;
            }

            ret = avcodec_receive_frame(m_codecContext, hwFrame);
            if (ret < 0 && ret != AVERROR(EAGAIN) && ret != AVERROR_EOF)
            {
                SPDLOG_DEBUG("avcodec_receive_frame(H/W) decoding failed:: ret={}/{}", ret, av_make_error_stdstring(ret));
                av_frame_free(&hwFrame);
                goto __END;
            }

            if (ret >= 0) {
                recvloopCount++;
#if HW_DECODE_TRACE
                SPDLOG_DEBUG("GOT HW GOT_PICTURE, pktConsumed::{}", pktConsumed);
#endif
                if (m_hwDeviceContext)
                {
                    AVFrame* temp = nullptr;
                    // retrieve data from GPU to CPU
                    av_frame_unref(temp);
                    av_frame_unref(m_swFrame);
                    ret = av_hwframe_transfer_data(m_swFrame, hwFrame, 0);
                    if (ret < 0)
                    {
                        SPDLOG_DEBUG("av_hwframe_transfer_data() failed, ret={}/{}",
                                     ret, av_make_error_stdstring(ret));
                        break;
                    }
                    m_swFrame->pts = hwFrame->pts;
                    temp = m_swFrame;

                    auto outPtr = m_scaler.getScaledData(temp, m_outParam);
                    if (!outPtr) {
                        av_frame_free(&temp);
                        ret = -1;
                        goto __END;
                    }

                    outPtr->ptsTimeMsec = temp->pts;
                    //outPtr->setPtsTimeMsec(hwFrame->pts);
                    m_hwOutputQueue.push(outPtr);
                } 
                else
                {
                    auto outPtr = m_scaler.getScaledData(hwFrame, m_outParam);
                    if (!outPtr)
                    {
                        av_frame_free(&hwFrame);
                        ret = -1;
                        goto __END;
                    }

                    outPtr->ptsTimeMsec = hwFrame->pts;
                    //outPtr->setPtsTimeMsec(hwFrame->pts);
                    m_hwOutputQueue.push(outPtr);

                    av_frame_unref(m_swFrame);
                    av_frame_move_ref(m_swFrame, hwFrame);
                }
            }
            av_frame_free(&hwFrame);
        }
    }

__END:
    if (m_hwOutputQueue.size() > 0)
    {
        m_lastVideoOutputPtr = m_hwOutputQueue.front();
        m_hwOutputQueue.pop();
        ret = 0;
    }

    if (ret == AVERROR(EAGAIN))
    {
        ret = 0;
    }

    if (m_lastVideoOutputPtr)
    {
        gotPicture = true;
    }

#if HW_DECODE_TRACE
    SPDLOG_DEBUG("decodeInternalHw() END::  gotPicture={}, loopCount={}, recvLoopCount={}, queueCnt={}",
                 gotPicture, loopCount, recvloopCount, m_hwOutputQueue.size());
#endif
    return ret;
}

VideoOutputFrameSharedPtr VideoDecoder::getLastOriginalVideoImage()
{
    VideoOutputFrameSharedPtr videoOutputPtr = nullptr;
    if(m_decodeMethod == VideoDecodeMethod::SW) {
        if(m_frame) {
            SPDLOG_DEBUG("getLastOriginalVideoImage() m_frame width={}, height={}",
                         m_frame->width, m_frame->height);
            videoOutputPtr = m_scaler.getData(m_frame, m_outParam);
        }
    }
    else {
        if(m_swFrame) {
            SPDLOG_DEBUG("getLastOriginalVideoImage() m_swFrame width={}, height={}",
                         m_swFrame->width, m_swFrame->height);
            videoOutputPtr = m_scaler.getData(m_swFrame, m_outParam);
        }
    }

    return videoOutputPtr;
}

/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "VideoEncoder.h"
#include "LogSettings.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-VENC] "}, level, __VA_ARGS__)
#define ENCODE_TRACE 0

VideoEncoder::VideoEncoder()
{
    SPDLOG_DEBUG("VideoEncoder()");
}

VideoEncoder::~VideoEncoder()
{
    SPDLOG_DEBUG("~VideoEncoder()");
    release();
}

int VideoEncoder::open(const VideoOutputFrameSharedPtr& decodedFrame)
{
    if (m_codecContext != nullptr) {
        SPDLOG_DEBUG("VideoEncoder::open() release opened encoder context first.");
        release();
    }

    // find H264 encoder
    //const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
    const AVCodec *codec = avcodec_find_encoder_by_name("libopenh264");
    if (!codec) {
        SPDLOG_ERROR("Can not find video encoder, libopenh264");
        return -1;
    }

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        SPDLOG_ERROR("Failed to alloc video encoder codec context.");
        release();
        return -1;
    }

    // put sample parameters
    m_codecContext->bit_rate = 4096000;    
    // resolution must be a multiple of two
    m_codecContext->width = decodedFrame->width();
    m_codecContext->height = decodedFrame->height();    
    // frames per second
    m_codecContext->time_base.num = 1;
    m_codecContext->time_base.den = 30;
    m_codecContext->framerate.num = 30;
    m_codecContext->framerate.den = 1;    
    /* check frame pict_type before passing frame to encoder,
     * if frame->pict_type is AV_PICTURE_TYPE_I
     * then gop_size is ignored and the output of encoder
     * will always be I frame irrespective to gop_size
     */
    m_codecContext->gop_size = 30;    
    m_codecContext->max_b_frames = 0;    
    m_codecContext->pix_fmt = decodedFrame->format();

    av_opt_set(m_codecContext->priv_data, "profile", "high", AV_OPT_SEARCH_CHILDREN);
    av_opt_set(m_codecContext->priv_data, "level", "4.0", AV_OPT_SEARCH_CHILDREN);
    /*
    av_opt_set(m_codecContext->priv_data, "preset", "slow", AV_OPT_SEARCH_CHILDREN);
    av_opt_set(m_codecContext->priv_data, "crf",  "18", AV_OPT_SEARCH_CHILDREN);
    */

    int ret = avcodec_open2(m_codecContext, codec, NULL);
    if (ret < 0) {
        SPDLOG_WARN("Failed to open video codec encoder, ret={}, err={}", ret, av_make_error_stdstring(ret));
        release();
        return ret;
    }
    SPDLOG_DEBUG("Success to open h264 encoder, width={}, height={}, codec_id={}, pix_fmt={}",
                 m_codecContext->width, m_codecContext->height, m_codecContext->codec_id, m_codecContext->pix_fmt);

    // initialize packet, set data to NULL
    m_packet = av_packet_alloc();

    // frame containing input raw audio
    m_frame = av_frame_alloc();
    if (!m_frame) {
        SPDLOG_WARN("Could not allocate video frame");
        release();
        return -1;
    }
    m_frame->format = m_codecContext->pix_fmt;
    m_frame->width  = m_codecContext->width;
    m_frame->height = m_codecContext->height;

    ret = av_frame_get_buffer(m_frame, 0);
    if (ret < 0) {
        SPDLOG_ERROR("av_frame_get_buffer() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        release();
        return -1;
    }
    SPDLOG_DEBUG("av_frame_get_buffer() success. format={}, width={}, height={}", m_frame->format, m_frame->width, m_frame->height);

    return 0;
}

bool VideoEncoder::encode(const VideoOutputFrameSharedPtr& decodedFrame,
                          Wisenet::Media::VideoSourceFrameSharedPtr& encodedFrame)
{
    if (!m_codecContext) {
        if (open(decodedFrame) < 0) {
            SPDLOG_ERROR("open() failed");
            return false;
        }
    }

    /* Make sure the frame data is writable.
       On the first round, the frame is fresh from av_frame_get_buffer()
       and therefore we know it is writable.
       But on the next rounds, encode() will have called
       avcodec_send_frame(), and the codec may have kept a reference to
       the frame in its internal structures, that makes the frame
       unwritable.
       av_frame_make_writable() checks that and allocates a new buffer
       for the frame only if necessary.
     */
    int ret = av_frame_make_writable(m_frame);
    if (ret < 0) {
        SPDLOG_WARN("av_frame_make_writable() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }

    // fill frame data
    uint8_t *src_data[4] = {NULL};
    uint8_t *dst_data[4] = {NULL};
    int src_linesize[4] = {0};
    int dst_linesize[4] = {0};

    src_data[0] = decodedFrame->data(0);
    src_data[1] = decodedFrame->data(1);
    src_data[2] = decodedFrame->data(2);
    src_data[3] = decodedFrame->data(3);
    src_linesize[0] = decodedFrame->lineSize(0);
    src_linesize[1] = decodedFrame->lineSize(1);
    src_linesize[2] = decodedFrame->lineSize(2);
    src_linesize[3] = decodedFrame->lineSize(3);

    dst_data[0] = m_frame->data[0];
    dst_data[1] = m_frame->data[1];
    dst_data[2] = m_frame->data[2];
    dst_data[3] = m_frame->data[3];
    dst_linesize[0] = m_frame->linesize[0];
    dst_linesize[1] = m_frame->linesize[1];
    dst_linesize[2] = m_frame->linesize[2];
    dst_linesize[3] = m_frame->linesize[3];

    av_image_copy(dst_data, dst_linesize,
                  (const uint8_t **)src_data, src_linesize,
                  decodedFrame->format(), decodedFrame->width(), decodedFrame->height());

    m_frame->pts = decodedFrame->ptsTimeMsec;


    // send frame to encoder
    ret = avcodec_send_frame(m_codecContext, m_frame);
    if (ret < 0) {
        SPDLOG_WARN("avcodec_send_frame() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }
    av_packet_unref(m_packet);
    while (ret >= 0) {
        // receivce packet from encoder
        ret = avcodec_receive_packet(m_codecContext, m_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
#if ENCODE_TRACE
            SPDLOG_DEBUG("avcodec_receive_packet() RET={}, EAGAIN={}", ret, AVERROR(EAGAIN));
#endif
            break;
        }
        else if (ret < 0) {
            SPDLOG_WARN("avcodec_receive_packet() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
            break;
        }

        if (m_packet->size > 0) {
            encodedFrame = std::make_shared<Wisenet::Media::VideoSourceFrame>(m_packet->data, m_packet->size);
            encodedFrame->videoCodecType = get_wisenet_vcodec_type(m_codecContext->codec_id);
            encodedFrame->frameSequence = m_frameSequence++;
            encodedFrame->frameRate = 30;
            encodedFrame->videoWidth = decodedFrame->width();
            encodedFrame->videoHeight = decodedFrame->height();
            encodedFrame->videoFrameType = (m_packet->flags & AV_PKT_FLAG_KEY) ?
                        Wisenet::Media::VideoFrameType::I_FRAME :
                        Wisenet::Media::VideoFrameType::P_FRAME ;
            encodedFrame->frameTime.ptsTimestampMsec = decodedFrame->ptsTimeMsec;
            /*
            SPDLOG_DEBUG("encode video. size={} codecType={} width={} height={}",
                         m_packet->size, encodedFrame->videoCodecType, encodedFrame->videoWidth, encodedFrame->videoHeight);
                         */

            av_packet_unref(m_packet);
        }
    }
    if (encodedFrame->getDataSize() > 0) {
#if ENCODE_TRACE
        SPDLOG_DEBUG("encode video success, ret={}, dataSize={}\n{:X}", ret, audioEncodeData->dataSize(),
                     spdlog::to_hex(audioEncodeData->data(), audioEncodeData->data() + audioEncodeData->dataSize()));
#endif
        return true;
    }
    SPDLOG_WARN("encode video failed, encoded packet size is invalid, size={}", m_packet->size);
    return false;
}


void VideoEncoder::release()
{
    if (m_codecContext != nullptr) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = nullptr;
    }
    if(m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
    m_frameSequence = 0;
}

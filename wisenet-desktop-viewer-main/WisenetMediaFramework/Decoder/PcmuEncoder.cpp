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
#include "PcmuEncoder.h"
#include "LogSettings.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-AENC] "}, level, __VA_ARGS__)
#define ENCODE_TRACE 0

PcmuEncoder::PcmuEncoder()
{
    SPDLOG_DEBUG("PcmuEncoder()");
}

PcmuEncoder::~PcmuEncoder()
{
    SPDLOG_DEBUG("~PcmuEncoder()");
    release();
}

int PcmuEncoder::open(const AudioInputFrameSharedPtr &audioSourceData)
{
    if (m_codecContext != nullptr) {
        SPDLOG_DEBUG("PcmuEncoder::open() release opened encoder context first.");
        release();
    }

    // find pcmu encoder
    const AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_PCM_MULAW);
    if (!codec) {
        SPDLOG_ERROR("Can not find audio encoder, AV_CODEC_ID_PCM_MULAW");
        return -1;
    }

    m_codecContext = avcodec_alloc_context3(codec);
    if (!m_codecContext) {
        SPDLOG_ERROR("Failed ato alloc audio encoder codec context.");
        release();
        return -1;
    }

    // put sample parameter
    m_codecContext->bit_rate = 64000;
    m_codecContext->sample_rate = 8000;
    m_codecContext->channels = 1;
    m_codecContext->sample_fmt = AV_SAMPLE_FMT_S16;
    m_codecContext->channel_layout = AV_CH_LAYOUT_MONO;

    int ret = avcodec_open2(m_codecContext, codec, NULL);
    if (ret < 0) {
        SPDLOG_WARN("Failed to open audio codec encoder, ret={}, err={}", ret, av_make_error_stdstring(ret));
        release();
        return ret;
    }

    SPDLOG_DEBUG("Success to open pcmu encoder, codec={}, sample_fmt={}, bit_rate={}, sampleRate={}, channels={}, channelLayout={}",
                 avcodec_get_name(AV_CODEC_ID_PCM_MULAW),
                 av_get_sample_fmt_name(m_codecContext->sample_fmt),
                 m_codecContext->bit_rate, m_codecContext->sample_rate,
                 m_codecContext->channels, m_codecContext->channel_layout);

    // initialize packet, set data to NULL
    m_packet = av_packet_alloc();

    // frame containing input raw audio
    m_frame = av_frame_alloc();
    if (!m_frame) {
        SPDLOG_WARN("Could not allocate frame");
        release();
        return -1;
    }
    m_frame->channels = 1;
    m_frame->channel_layout = AV_CH_LAYOUT_MONO;
    m_frame->format = AV_SAMPLE_FMT_S16;
    m_frame->sample_rate = 8000;
    m_frame->nb_samples = audioSourceData->getDataSize()/2;

    ret = av_frame_get_buffer(m_frame, 1);
    if (ret < 0) {
        SPDLOG_ERROR("av_frame_get_buffer() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        release();
        return -1;
    }
    SPDLOG_DEBUG("av_frame_get_buffer() success, nb_samples={}", m_frame->nb_samples);


    return 0;
}

bool PcmuEncoder::encode(const AudioInputFrameSharedPtr &audioSourceData,
                         Wisenet::Media::AudioSourceFrameSharedPtr  &audioEncodeData)
{
#if ENCODE_TRACE
    // SPDLOG_DEBUG("encode start, source={}\n{:X}", audioSourceData->dataSize(),
    //              spdlog::to_hex(audioSourceData->data(), audioSourceData->data()+320));
#endif
    if (!m_codecContext) {
        if (open(audioSourceData) < 0) {
            SPDLOG_ERROR("open() failed");
            return false;
        }
    }
    int ret = av_frame_make_writable(m_frame);
    if (ret < 0) {
        SPDLOG_WARN("av_frame_make_writable() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }
    //uint16_t *samples = (uint16_t*)m_frame->data[0];
    //std::memcpy(samples, audioSourceData->data(), audioSourceData->dataSize());
    ret = avcodec_fill_audio_frame(m_frame, 1, AV_SAMPLE_FMT_S16,
                                   audioSourceData->getDataPtr(), audioSourceData->getDataSize(), 1);
    if (ret < 0) {
        SPDLOG_WARN("avcodec_fill_audio_frame() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }

    ret = avcodec_send_frame(m_codecContext, m_frame);
    if (ret < 0) {
        SPDLOG_WARN("avcodec_send_frame() failed, ret={}, err={}", ret, av_make_error_stdstring(ret));
        return false;
    }
    av_packet_unref(m_packet);
    while (ret >= 0) {
        ret = avcodec_receive_packet(m_codecContext, m_packet);
        if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
#if ENCODE_TRACE
            SPDLOG_DEBUG("avcodec_receive_packet() RET={}, EAGAIN={}", ret, AVERROR(EAGAIN));
#endif
            break;
        } else if (ret < 0) {
            SPDLOG_WARN("avcodec_receive_packet() failed, ret={}, err={}", ret,
                        av_make_error_stdstring(ret));
            break;
        }

        if (m_packet->size > 0) {
            audioEncodeData = std::make_shared<Wisenet::Media::AudioSourceFrame>(m_packet->data, m_packet->size);
            av_packet_unref(m_packet);
        }
    }
    if (audioEncodeData->getDataSize() > 0) {
#if ENCODE_TRACE
        SPDLOG_DEBUG("encode audio success, ret={}, dataSize={}\n{:X}", ret, audioEncodeData->dataSize(),
                     spdlog::to_hex(audioEncodeData->data(), audioEncodeData->data() + audioEncodeData->dataSize()));
#endif
        return true;
    }
    SPDLOG_WARN("encode audio failed, encoded packet size is invalid, size={}", m_packet->size);
    return false;
}


void PcmuEncoder::release()
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
}

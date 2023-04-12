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
#include "AudioDecoder.h"
#include "LogSettings.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFMPEG-A] "}, level, __VA_ARGS__)




AudioDecoder::AudioDecoder(QObject *parent)
    : BaseDecoder(parent)
    , m_codecContext(nullptr)
    , m_swrContext(nullptr)
    , m_frame(nullptr)
    , m_packet(nullptr)
    , m_useFFmpegCodecParam(false)
{
    SPDLOG_DEBUG("AudioDecoder::AudioDecoder()");
}

AudioDecoder::~AudioDecoder()
{
    SPDLOG_DEBUG("AudioDecoder::~AudioDecoder()");
    release();
}

void AudioDecoder::release()
{
    if (m_codecContext != NULL) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = NULL;
    }
    if(m_frame) {
        av_frame_free(&m_frame);
        m_frame = nullptr;
    }
    if (m_packet) {
        av_packet_free(&m_packet);
        m_packet = nullptr;
    }
    if (m_swrContext) {
        swr_free(&m_swrContext);
        m_swrContext = nullptr;
    }
}


int AudioDecoder::setupAudioFormat()
{
    m_targetSampleFormat = av_get_packed_sample_fmt(m_codecContext->sample_fmt);

    SPDLOG_DEBUG("setupAudioFormat():: targetSampleFormat = {}, channel_layout={}",
                 m_targetSampleFormat, m_codecContext->channel_layout);

    int getBytesPerSample = av_get_bytes_per_sample(m_targetSampleFormat);
    SPDLOG_DEBUG("setupAudioFormat():: bytesPerSample={}", getBytesPerSample);
    if (getBytesPerSample >= 4) {
        m_targetSampleFormat = AV_SAMPLE_FMT_S32;
    }

    if (m_codecContext->channel_layout == 0) {
        SPDLOG_DEBUG("setupAudioFormat():: warning!, channel_layout is 0");
        m_codecContext->channel_layout = AV_CH_LAYOUT_MONO;
    }

    m_targetSampleRate = m_codecContext->sample_rate;
    m_targetChannelLayout = m_codecContext->channel_layout;
    m_targetChannel = av_get_channel_layout_nb_channels(m_targetChannelLayout);

    QAudioFormat audioFormat;
    audioFormat.setSampleRate(m_targetSampleRate);
    audioFormat.setChannelCount(m_targetChannel);
    audioFormat.setSampleSize(8*av_get_bytes_per_sample(m_targetSampleFormat));
    audioFormat.setCodec("audio/pcm");
    audioFormat.setByteOrder(AV_NE(QAudioFormat::BigEndian, QAudioFormat::LittleEndian));
    if (m_targetSampleFormat == AV_SAMPLE_FMT_U8)
        audioFormat.setSampleType(QAudioFormat::UnSignedInt);
    else if (m_targetSampleFormat == AV_SAMPLE_FMT_FLT)
        audioFormat.setSampleType(QAudioFormat::Float);
    else
        audioFormat.setSampleType(QAudioFormat::SignedInt);
    m_audioFormat = audioFormat;

    SPDLOG_DEBUG("QAudioFormat info::sampleRate={}, channel={}, sampleSize={}, codec={}, sampleType={}",
                 audioFormat.sampleRate(), audioFormat.channelCount(), audioFormat.sampleSize(),
                 audioFormat.codec().toStdString(), audioFormat.sampleType());
    SPDLOG_DEBUG("swr_alloc_set_opts() param, t_layout={}, t_format={}, t_sampleRate={},t_channel={}, s_layout={}, s_format={}, s_sampleRate={}",
                 m_targetChannelLayout, m_targetSampleFormat, m_targetSampleRate, m_targetChannel,
                 m_codecContext->channel_layout, m_codecContext->sample_fmt, m_codecContext->sample_rate);

    m_swrContext = swr_alloc_set_opts(nullptr,
                                      m_targetChannelLayout, m_targetSampleFormat, m_targetSampleRate,
                                      m_codecContext->channel_layout, m_codecContext->sample_fmt, m_codecContext->sample_rate,
                                      0, nullptr);
    if (!m_swrContext) {
        SPDLOG_WARN("Failed to alloc resampler context");
        return -1;
    }
#if 0 // only for testing
    double a = 1.0;
    av_opt_get_double(m_swrContext, "cutoff", 0, &a);
    SPDLOG_DEBUG("cutoff={}", a);
    if (av_opt_set_double(m_swrContext, "cutoff", 0.001,   0) < 0) {
        SPDLOG_WARN("av_opt_set_double(cutoff) failed");
    }
    av_opt_get_double(m_swrContext, "cutoff", 0, &a);
    SPDLOG_DEBUG("(2)cutoff={}", a);
#endif

    int ret = swr_init(m_swrContext);
    if (ret < 0) {
        SPDLOG_WARN("Failed to initialize the resampling context, ret={},{}", ret, av_make_error_stdstring(ret));
        return ret;
    }

    return 0;
}

DecodeResult AudioDecoder::openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr& codecParam)
{
    SPDLOG_DEBUG("AudioDecoder::openWithFFmpegCodecParam()");

    if (openInternal(nullptr, codecParam) < 0)
        return DecodeResult::OpenFail;
    return DecodeResult::Success;
}

int AudioDecoder::openInternal(const Wisenet::Media::AudioSourceFrameSharedPtr& sourceFrame,
                               const FFmpegCodecParamSharedPtr& codecParam)
{
    int ret = -1;
    SPDLOG_DEBUG("AudioDecoder::openInternal()");
    if (m_codecContext != NULL) {
        SPDLOG_DEBUG("AudioDecoder::openInternal() release opened decoder context first.");
        release();
    }

    AVCodecParameters *param = nullptr;
    AVCodecID codecID = AV_CODEC_ID_NONE;

    if (codecParam) {
        param = codecParam->getParam();
        m_useFFmpegCodecParam = true;
    }
    else {
        m_useFFmpegCodecParam = false;
    }

    // Allocate an AVFrame and set its fields to default values
    m_frame = av_frame_alloc();
    if (!m_frame) {
        SPDLOG_WARN("Could not allocate frame");
        release();
        return ret;
    }

    codecID = param ? param->codec_id : get_acodec_id(sourceFrame->audioCodecType);
    if (codecID == AV_CODEC_ID_NONE) {
        SPDLOG_INFO("Not support codec::{}", sourceFrame->audioCodecType);
        release();
        return ret;
    }
    SPDLOG_DEBUG("Audio Codec ID={}", avcodec_get_name(codecID));

    // find decoder for the stream
    auto dec = avcodec_find_decoder(codecID);
    if (!dec) {
        SPDLOG_INFO("Can not find audio decoder");
        release();
        return ret;
    }

    // Allocate a codec context for the decoder
    m_codecContext = avcodec_alloc_context3(dec);
    if (!m_codecContext) {
        SPDLOG_WARN("Failed to allocate the audio codec context");
        release();
        return ret;
    }

    // Copy codec parameters from input stream to output codec context
    if (param) {
        ret = avcodec_parameters_to_context(m_codecContext, param);
        if (ret < 0) {
            SPDLOG_WARN("Failed to copy audio codec parameters to decoder context, ret={}", ret);
            release();
            return ret;
        }
        SPDLOG_DEBUG("channelLayout={}, channels={}, sample_rate={}, bits_per_coded_sample={}",
                     param->channel_layout, m_codecContext->channels, m_codecContext->sample_rate,
                     m_codecContext->bits_per_coded_sample);
        SPDLOG_DEBUG("param, bits_per_coded_sample={}, channels={}, sample_rate={}",
                     param->bits_per_coded_sample, param->channels, param->sample_rate);

    }
    else {
        SPDLOG_DEBUG("sourceFrame info, channels={}, sampleRate={}, bitRate={}, bitsPerSample={}",
                     sourceFrame->audioChannels, sourceFrame->audioSampleRate,
                     sourceFrame->audioBitrate, sourceFrame->audioBitPerSample);

        m_codecContext->channels = sourceFrame->audioChannels;
        m_codecContext->sample_rate = sourceFrame->audioSampleRate;
        if (codecID != AV_CODEC_ID_AAC) {
            m_codecContext->bits_per_coded_sample = sourceFrame->audioBitPerSample;
        }

        m_lastCodecId = codecID;
        m_lastAudioChannels = sourceFrame->audioChannels;
        m_lastAudioSampleRate = sourceFrame->audioSampleRate;
        m_lastAudioBitrate = sourceFrame->audioBitrate;
    }

    if (m_codecContext->channel_layout == 0) {
        m_codecContext->channel_layout = av_get_default_channel_layout(m_codecContext->channels);
    }

    // Init the decoder
    ret = avcodec_open2(m_codecContext, dec, NULL);
    if (ret < 0) {
        SPDLOG_WARN("Failed to open audio codec decoder, ret={}, err={}", ret, av_make_error_stdstring(ret));
        release();
        return ret;
    }

    // check resample format
    if (setupAudioFormat() < 0) {
        release();
        return -1;
    }

    // initialize packet, set data to NULL
    m_packet = av_packet_alloc();
    m_packet->data = NULL;
    m_packet->size = 0;

    SPDLOG_DEBUG("Success to open audio decoder, codec={}, sample_fmt={}",
                 avcodec_get_name(codecID), av_get_sample_fmt_name(m_codecContext->sample_fmt));
    return 0;
}

bool AudioDecoder::checkFrame(const Wisenet::Media::AudioSourceFrameSharedPtr &audioSourceFrame)
{
    if (!m_useFFmpegCodecParam) {
        AVCodecID codecID = get_acodec_id(audioSourceFrame->audioCodecType);
        if(codecID != m_lastCodecId) {
            SPDLOG_DEBUG("audio codec changed={}, {}",
                         avcodec_get_name(codecID), avcodec_get_name(m_lastCodecId));
            return false;
        }

        if (m_lastAudioChannels != audioSourceFrame->audioChannels ||
            m_lastAudioSampleRate != audioSourceFrame->audioSampleRate ||
            m_lastAudioBitrate != audioSourceFrame->audioBitrate) {
            SPDLOG_DEBUG("audio parameter changed, channel:{}=>{}, sampleRate:{}=>{}, bitrate:{}=>{}",
                         m_lastAudioChannels, audioSourceFrame->audioChannels,
                         m_lastAudioSampleRate, audioSourceFrame->audioSampleRate,
                         m_lastAudioBitrate, audioSourceFrame->audioBitrate);
            return false;
        }

    }
    return true;

}

AudioOutputFrameSharedPtr AudioDecoder::getResampledData(AVFrame *srcFrame)
{
    int dst_nb_samples = av_rescale_rnd(swr_get_delay(m_swrContext, m_codecContext->sample_rate) +
                                        srcFrame->nb_samples, m_targetSampleRate, m_codecContext->sample_rate, AV_ROUND_UP);

    int bufferSize = av_samples_get_buffer_size(nullptr, m_targetChannel, dst_nb_samples, m_targetSampleFormat, 0);
    auto outAudioPtr = std::make_shared<AudioOutputFrame>(m_audioFormat, bufferSize);
    uint8_t *buf = outAudioPtr->data();
    int ret = swr_convert(m_swrContext, &buf, dst_nb_samples, (const uint8_t**)(srcFrame->data), srcFrame->nb_samples);
    if (ret < 0) {
        SPDLOG_WARN("swr_convert() Failed, ret={}", ret);
    }
    //SPDLOG_DEBUG("getResampledData(), dst_nb_samples={}, bufferSize={}", dst_nb_samples, bufferSize);
    return outAudioPtr;
}

DecodeResult AudioDecoder::decodeStart(const Wisenet::Media::AudioSourceFrameSharedPtr& audioSourceFrame,
                                       AudioOutputFrameSharedPtr& outFrame)
{
    //SPDLOG_TRACE("decodeStart()");

    int ret = -1;
    bool gotPicture = false;

    if (m_codecContext == NULL || m_frame == NULL) {
        if (m_useFFmpegCodecParam) {
            SPDLOG_ERROR("decodeStart() codec context or frame is nullptr");
            goto __EmitReturn;
        }

        ret = openInternal(audioSourceFrame, nullptr);
        if (ret < 0) {
            goto __EmitReturn;
        }
    }
    if (!checkFrame(audioSourceFrame)) {
        SPDLOG_DEBUG("Audio Frame is changed, reset decoder!");
        ret = openInternal(audioSourceFrame, nullptr);
        if (ret < 0) {
            goto __EmitReturn;
        }
    }


    m_packet->size = (int)audioSourceFrame->getDataSize();
    m_packet->data = audioSourceFrame->getDataPtr();
    m_packet->pts = audioSourceFrame->frameTime.ptsTimestampMsec;
//    m_packet.dts = dataPtr->dts();

    ret = avcodec_send_packet(m_codecContext, m_packet);
    if (ret < 0) {
        SPDLOG_ERROR("Error submitting a packet for decoding, err={}, message={}", ret, av_make_error_stdstring(ret));
        goto __EmitReturn;
    }

    // get all the available frames from the decoder
    while (ret >= 0) {
        ret = avcodec_receive_frame(m_codecContext, m_frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                //SPDLOG_TRACE("DECODE EOF");
                goto __EmitReturn;
            }

            SPDLOG_ERROR("Error during decoding, err={}, message={}", ret, av_make_error_stdstring(ret));
            goto __EmitReturn;
        }

        auto audioOut = getResampledData(m_frame);
        audioOut->ptsTimeMsec = m_frame->pts;
//        SPDLOG_DEBUG("AUDIO OUT pts={}, channels={}, nb_samples={}, size={}, bytes_per_sample={},{}",
//                     m_frame->pts, m_frame->channels, m_frame->nb_samples, audioOut->dataSize(),
//                     av_get_bytes_per_sample(m_targetSampleFormat),
//                     av_get_bytes_per_sample(m_codecContext->sample_fmt));
        outFrame = audioOut;
        gotPicture = true;
    }

__EmitReturn:
    if (gotPicture)
        ret = 0;

    DecodeResult result = (ret==0) ? DecodeResult::Success : DecodeResult::DecodeFail;
    return result;
}

void AudioDecoder::flushBuffers()
{
    if (m_codecContext) {
        avcodec_flush_buffers(m_codecContext);
    }
}


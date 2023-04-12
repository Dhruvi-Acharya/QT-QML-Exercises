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

#include "LogSettings.h"
#include "FFmpegFileOutContext.h"
#include <QDir>
#include <QDateTime>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "QLocaleManager.h"

#define STREAM_PIX_FMT    AV_PIX_FMT_YUV420P /* default pix_fmt */

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFmpeg-FOC] "}, level, __VA_ARGS__)

FFmpegFileOutContext::~FFmpegFileOutContext()
{
    Finalize();
}

void FFmpegFileOutContext::Finalize()
{
    CloseFile();
    ClearQueue();
}

void FFmpegFileOutContext::ReleaseContext()
{
    if(m_avOutFormatContext)
    {
        if (m_avOutFormatContext->oformat)
        {
            if (!(m_avOutFormatContext->flags & AVFMT_NOFILE))
                avio_closep(&m_avOutFormatContext->pb);
        }

        avformat_free_context(m_avOutFormatContext);
        m_avOutFormatContext = nullptr;
    }

    m_avOutVideoStream.Reset();
    m_avOutAudioStream.Reset();
    m_avOutSubtitleStream.Reset();

    m_avAudioDecoderContext.reset();
    m_avSubripDecoderContext.reset();
    m_avAudioEncoderContext.reset();

    if (m_avAudioResampleContext)
    {
        swr_free(&m_avAudioResampleContext);
        m_avAudioResampleContext = nullptr;
    }

    if (m_subTitleOutBuffer)
    {
        av_free(m_subTitleOutBuffer);
        m_subTitleOutBuffer = nullptr;
    }
    avsubtitle_free(&m_subTitle);
}

bool FFmpegFileOutContext::AllocContext()
{
    ReleaseContext();

    // check dir
    if(!QDir(m_fileDir).exists())
    {
        if(!QDir().mkpath(m_fileDir))
        {
            SPDLOG_ERROR("AllocContext() Failed to create a directory. path={}", m_fileDir.toLocal8Bit().toStdString());
            return false;
        }
    }

    // check file
    int extensionIndex = m_fileName.lastIndexOf(".");
    if(extensionIndex == -1 || extensionIndex == m_fileName.length() - 1)
    {
        SPDLOG_ERROR("AllocContext() File extension error fileName={}", m_fileName.toLocal8Bit().toStdString());
        return false;
    }

    QString name = m_fileName.left(extensionIndex);
    QString extension = m_fileName.right(m_fileName.length() - extensionIndex - 1);
    QString currentOutFilePath = m_fileNo == 1
            ? m_fileDir + "/" + m_fileName
            : m_fileDir + "/" + name + " (" + QString::number(m_fileNo) + ")." + extension;
    while (QFile(currentOutFilePath).exists())
        currentOutFilePath = m_fileDir + "/" + name + " (" + QString::number(++m_fileNo) + ")." + extension;

    m_currentOutFilePath = currentOutFilePath.toUtf8().toStdString();   // 영문 OS에서 한글 파일, 폴더 안열리는 문제 수정
    const char *out_filename = m_currentOutFilePath.c_str();

    // make formatcontext
    int ret = avformat_alloc_output_context2(&m_avOutFormatContext, NULL, NULL, out_filename);
    if (ret < 0)
    {
        emit writeFailed("avformat_alloc_output_context2 failed : " + ret);
        return false;
    }

    return true;
}

// open file and write header
bool FFmpegFileOutContext::OpenFile()
{
    if (!m_avOutFormatContext)
    {
        emit writeFailed("Interal error");
        return false;
    }

    int ret = -1;

    if (!(m_avOutFormatContext->oformat->flags & AVFMT_NOFILE))
    {
        ret = avio_open(&m_avOutFormatContext->pb, m_currentOutFilePath.c_str(), AVIO_FLAG_WRITE);
        if (ret == 0)
        {
            m_avOutFormatContext->pb->seekable |= AVIO_SEEKABLE_NORMAL;
            ret = avformat_write_header(m_avOutFormatContext, NULL);
            if (ret < 0) {
                SPDLOG_ERROR("OpenFile() avformat_write_header() failed. ret:{},{}", ret, av_make_error_stdstring(ret));
            }
        }
    }

    if (ret < 0)
    {
        SPDLOG_ERROR("OpenFile() Could not open output file. ret:{},{}", ret, av_make_error_stdstring(ret));
        emit writeFailed("Could not open output file");
        return false;
    }

    emit newFileCreated(QString::fromStdString(m_currentOutFilePath));
    return true;
}

// close output file
void FFmpegFileOutContext::CloseFile()
{
    // write readyQueue data
    if (!m_isReady)
    {
        QueueSetup();
        m_isReady = true;
    }
    bool m_videoFrameChecked = false;
    bool m_audioFrameChecked = false;

    // write trailer
    int ret = -1;
    if (m_avOutFormatContext && m_avOutFormatContext->pb != NULL)
    {
        ret = av_write_trailer(m_avOutFormatContext);
        if(ret != 0)
        {
            emit writeFailed("CloseFile error : " + QString::number(ret));
        }
    }

    ReleaseContext();

    if(ret == 0)
    {
        int fileNo = 1;
        int extensionIndex = m_fileName.lastIndexOf(".");
        QString name = m_fileName.left(extensionIndex);
        QString extension = m_fileName.right(m_fileName.length() - extensionIndex - 1);

        QDateTime startDatetime = QDateTime::fromMSecsSinceEpoch(m_startTime);
        QDateTime endDatetime = QDateTime::fromMSecsSinceEpoch(m_endTime);
        QString dateTime = "_" + startDatetime.toString("yyyyMMdd") + "_" + startDatetime.toString("hhmmss") + "_" + endDatetime.toString("hhmmss");

        QString newOutFilePath = m_fileDir + "/" + name + dateTime + "." + extension;
        while (QFile(newOutFilePath).exists())
            newOutFilePath = m_fileDir + "/" + name + dateTime + " (" + QString::number(++fileNo) + ")." + extension;

        if(!QFile::rename(QString::fromUtf8(m_currentOutFilePath.c_str()), newOutFilePath))
        {
            emit writeFailed("Rename error");
        }
    }
}

void FFmpegFileOutContext::ClearQueue()
{
    m_readyQueue.clear();
}

bool FFmpegFileOutContext::GenericSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData, bool &isVideoDecodingFail)
{
    if (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO)
    {
        if(m_avOutVideoStream.GetStream())
            return true;    // already created..

        return GenericVideoSetup(mediaData, isVideoDecodingFail);
    }

    else if (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO)
    {
        if(m_avOutAudioStream.GetStream())
            return true;    // already created..

        return GenericAudioSetup(mediaData);
    }

    return true;
}

bool FFmpegFileOutContext::GenericVideoSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData, bool &isVideoDecodingFail)
{
    AVPacket *pkt = NULL;
    AVCodecParserContext *parser = NULL;
    AVFrame *frame = NULL;
    AVCodecContext *codecContext = NULL;
    unsigned int dataSize = 0;
    unsigned char *dataPtr = NULL;
    QString errMessage;
    bool isSuccess = true;
    int bytes = 0;
    int ret = 0;
    AVStream *out_stream = NULL;
    AVCodecParameters *par = NULL;

    AVBSFContext *bsf = NULL;

    if(!m_avOutFormatContext) {
        //SPDLOG_DEBUG("m_avOutFormatContext not ready!!");
        return false;
    }

    if (mediaData->getMediaType() != Wisenet::Media::MediaType::VIDEO) {
        //SPDLOG_DEBUG("m_avOutFormatContext not video!!");
        return false;
    }

    // i frame이 아니면 패스한다.
    auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
    if (videoData->videoFrameType != Wisenet::Media::VideoFrameType::I_FRAME) {
        SPDLOG_DEBUG("m_avOutFormatContext not key frame!!");
        return true;
    }

    dataSize = videoData->getDataSize();
    dataPtr = videoData->getDataPtr();

    if (m_avOutFormatContext->oformat->flags & AVFMT_GLOBALHEADER) {
        SPDLOG_DEBUG("GenericVideoSetup() AVFMT_GLOBALHEADER flag set");
    }

    SPDLOG_DEBUG("GenericVideoSetup() START dataSize={}", dataSize);
    AVCodecID ffmpegCodecID = get_vcodec_id(videoData->videoCodecType);
    auto *av_codec = avcodec_find_decoder(ffmpegCodecID);
    if (av_codec == NULL) {
        SPDLOG_WARN("GenericVideoSetup() avcodec_find_decoder() failed");
        errMessage = "Not support codec type";
        isSuccess = false;
        goto __END;
    }

    // aloc codec context
    codecContext = avcodec_alloc_context3(av_codec);
    if (codecContext == NULL) {
        SPDLOG_WARN("GenericVideoSetup() avcodec_alloc_context3 failed");
        errMessage = "Can not create VIDEO codec context";
        isSuccess = false;
        goto __END;
    }

    // codec open
    ret = avcodec_open2(codecContext, av_codec, NULL);
    if(ret < 0) {
        SPDLOG_WARN("OpenFile() avcodec_open2 ret:{}", av_make_error_stdstring(ret));
        errMessage = "Can not open VIDEO codec context";
        isSuccess = false;
        goto __END;
    }

    // decode test i-frame to get video information
    pkt = av_packet_alloc();
    pkt->data = dataPtr;
    pkt->size = dataSize;
    pkt->flags |= AV_PKT_FLAG_KEY;

    ret = avcodec_send_packet(codecContext, pkt);
    if (ret < 0 ) {
        SPDLOG_WARN("OpenFile() avcodec_send_packet ret:{}/{}", ret, av_make_error_stdstring(ret));
        errMessage = "Can not decode VIDEO codec context";
        isSuccess = false;
        goto __END;
    }
    avcodec_send_packet(codecContext, NULL);

    frame = av_frame_alloc();
    ret = avcodec_receive_frame(codecContext, frame);
    if (ret < 0 && ret != AVERROR_EOF ) {
        SPDLOG_WARN("OpenFile() avcodec_receive_frame ret:{}/{}", ret, av_make_error_stdstring(ret));
        errMessage = "Can not decode(2) VIDEO codec context";
        isSuccess = false;
        goto __END;
    }
    SPDLOG_DEBUG("GenericVideoSetup() decode video 1 frame test success!!");

    // create av stream
    out_stream = avformat_new_stream(m_avOutFormatContext, NULL);
    if (out_stream == NULL)
    {
        SPDLOG_WARN("GenericVideoSetup() avformat_new_stream failed");
        errMessage = "Can not create video out stream";
        isSuccess = false;
        goto __END;
    }
    out_stream->id = m_avOutFormatContext->nb_streams-1;

    /* timebase: This is the fundamental unit of time (in seconds) in terms
     * of which frame timestamps are represented. For fixed-fps content,
     * timebase should be 1/framerate and timestamp increments should be
     * identical to 1. */
    out_stream->time_base.num = 1;
    out_stream->time_base.den = 100;

    // copy the stream parameters to the muxer
    ret = avcodec_parameters_from_context(out_stream->codecpar, codecContext);
    if (ret < 0) {
        SPDLOG_WARN("GenericVideoSetup() avcodec_parameters_from_context failed");
        errMessage = "Can not copy VIDEO codec context parameter";
        isSuccess = false;
        goto __END;
    }

    // parser init
    if(av_codec->id == AVCodecID::AV_CODEC_ID_H264 || av_codec->id == AVCodecID::AV_CODEC_ID_HEVC) {
        const AVBitStreamFilter *filter = av_bsf_get_by_name("remove_extra");
        ret = av_bsf_alloc(filter, &bsf);
        if (ret < 0) {
            SPDLOG_WARN("GenericVideoSetup() av_bsf_alloc() failed, ret:{}/{}",
                        ret, av_make_error_stdstring(ret));
            errMessage = "Can not create bitstream filter context";
            isSuccess = false;
            goto __END;
        }
        ret = avcodec_parameters_from_context(bsf->par_in, codecContext);
        if (ret < 0) {
            SPDLOG_WARN("GenericVideoSetup() set bsf parameters from context failed, ret:{}/{}",
                        ret, av_make_error_stdstring(ret));
            errMessage = "Can not copy VIDEO codec context parameter to bsf";
            isSuccess = false;
            goto __END;
        }
        av_bsf_init(bsf);
        if (av_bsf_send_packet(bsf, pkt) == 0) {
            if (av_bsf_receive_packet(bsf, pkt) == 0) {
                if (pkt->size < dataSize) {
                    bytes = dataSize - pkt->size;
                    SPDLOG_DEBUG("GenericVideoSetup() make extra data for H264/H265, extra_size={}", bytes);
                    par = out_stream->codecpar;
                    par->extradata = (uint8_t *)av_mallocz(bytes + AV_INPUT_BUFFER_PADDING_SIZE);
                    par->extradata_size = bytes;
                    std::memcpy(par->extradata, dataPtr, bytes);
                }
            }
        }

        /* parser->split is deprecated(null) since FFmpeg v5.0 */
#if 0 // FFmpeg v4 logic
        parser = av_parser_init(av_codec->id);
        if (!parser) {
            SPDLOG_DEBUG("GenericVideoSetup() av_parser_init() failed");
            errMessage = "Can not init VIDEO parser";
            isSuccess = false;
            goto __END;
        }
        par = out_stream->codecpar;
        bytes = parser->parser->split(codecContext, dataPtr, dataSize);
        if (bytes > 0 && par->extradata_size == 0) {
            SPDLOG_DEBUG("GenericVideoSetup() make extra data for H264/H265, extra_size={}", bytes);
            par->extradata = (uint8_t *)av_mallocz(bytes + AV_INPUT_BUFFER_PADDING_SIZE);
            std::memcpy(par->extradata, dataPtr, bytes);
            par->extradata_size = bytes;
        }
#endif
    }

#if 0
    SPDLOG_DEBUG("info(1), codec_type={}, codec_id={}, codec_tag={}, bit_rate={}, bits_per_coded_sample={}, bits_per_raw_sample={}, profile={}, level={},format={}, width={}, height={}",
                 par->codec_type, par->codec_id, par->codec_tag, par->bit_rate, par->bits_per_coded_sample, par->bits_per_raw_sample, par->profile, par->level,
                 par->format, par->width, par->height);
    SPDLOG_DEBUG("info(2), field_order={}, color_range={}, color_primaries={}, color_trc={}, color_space={}, chroma_location={}, sample_aspect_ratio={}/{}, video_delay={}, extraSize={}",
                 (int)par->field_order, (int)par->color_range, (int)par->color_primaries, (int)par->color_trc, (int)par->color_space, (int)par->chroma_location,
                 par->sample_aspect_ratio.num, par->sample_aspect_ratio.den,
                 par->video_delay, par->extradata_size);
#endif

    SPDLOG_DEBUG("GenericVideoSetup()() completed!!");
    m_avOutVideoStream.Reset(out_stream, FFmpegStreamInfo::StreamType::STREAM_VIDEO);

__END:
    if (!isSuccess)
        emit writeFailed(errMessage);
    if (pkt) {
        av_packet_free(&pkt);
    }
    if (parser) {
        av_parser_close(parser);
    }
    if (frame) {
        av_frame_free(&frame);
    }
    if (codecContext) {
        avcodec_free_context(&codecContext);
    }
    if (bsf) {
        av_bsf_free(&bsf);
    }
    return isSuccess;
}

bool FFmpegFileOutContext::GenericAudioSetup(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    if(!m_avOutFormatContext)
        return false;

    if (mediaData->getMediaType() != Wisenet::Media::MediaType::AUDIO)
        return false;

    auto audioData = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(mediaData);

    unsigned int dataSize = audioData->getDataSize();
    unsigned char *dataPtr = audioData->getDataPtr();
    AVCodecID ffmpegCodecID = get_acodec_id(audioData->audioCodecType);

    // find decoder
    auto *av_codec = avcodec_find_decoder(ffmpegCodecID);
    if (av_codec == NULL)
    {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_find_decoder failed");
        emit writeFailed("Can not find audio codec");
        return false;
    }

    // aloc codec context;
    m_avAudioDecoderContext = std::make_shared<AVCodecContextWrapper>(av_codec);
    if (m_avAudioDecoderContext->get() == NULL)
    {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_alloc_context3 failed");
        emit writeFailed("Can not create codec context");
        return false;
    }

    // codec open
    m_avAudioDecoderContext->get()->sample_rate = audioData->audioSampleRate;
    if (audioData->audioSampleRate == 0)
        m_avAudioDecoderContext->get()->sample_rate = 8000;

    m_avAudioDecoderContext->get()->channels = 1;// audioData->Channels;
    m_avAudioDecoderContext->get()->channel_layout =
            av_get_default_channel_layout(m_avAudioDecoderContext->get()->channels);

    if (ffmpegCodecID == AV_CODEC_ID_ADPCM_G726)
    {
        SPDLOG_DEBUG("GenericAudioSetup() G726 bits_per_coded_sample={}, bitRate={}",
                     audioData->audioBitPerSample, audioData->audioBitrate);
        m_avAudioDecoderContext->get()->bit_rate = audioData->audioBitrate;
        m_avAudioDecoderContext->get()->bits_per_coded_sample = audioData->audioBitPerSample;
    }
    else if(ffmpegCodecID == AV_CODEC_ID_AAC)
    {
        m_avAudioDecoderContext->get()->bit_rate = audioData->audioBitrate;
        if(audioData->audioSampleRate != 0)
        {
            m_avAudioDecoderContext->get()->bits_per_coded_sample = audioData->audioBitrate / audioData->audioSampleRate;
            m_avAudioDecoderContext->get()->sample_rate = audioData->audioSampleRate;
        }
    }

    if (m_avOutFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
        m_avAudioDecoderContext->get()->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

    if (avcodec_open2(m_avAudioDecoderContext->get(), av_codec, NULL) < 0)
    {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_open2 failed");
        emit writeFailed("Can not create audio decoding codec context");
        return false;
    }

    AVFrameWrapper frame;
    AVPacket pkt;
    int ret = 0;
    av_init_packet(&pkt);
    pkt.data = dataPtr;
    pkt.size = dataSize;

    // decode one frame
    ret = avcodec_send_packet(m_avAudioDecoderContext->get(), &pkt);
    if (ret < 0) {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_send_packet(AUDIO) failed, err={}, message={}", ret, av_make_error_stdstring(ret));
        emit writeFailed("Can not decode audio");
        return false;
    }
    avcodec_send_packet(m_avAudioDecoderContext->get(), NULL);
    ret = avcodec_receive_frame(m_avAudioDecoderContext->get(), frame.get());
    if (ret < 0) {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_receive_frame(AUDIO) failed, err={}, message={}", ret, av_make_error_stdstring(ret));
        emit writeFailed("Can not decode audio");
        return false;
    }

    AVStream *out_stream = avformat_new_stream(m_avOutFormatContext, NULL);
    if (out_stream == NULL)
    {
        SPDLOG_DEBUG("GenericAudioSetup() avformat_new_stream failed");
        emit writeFailed("Can not create out stream");
        return false;
    }
    out_stream->time_base = m_avAudioDecoderContext->get()->time_base;

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(out_stream->codecpar, m_avAudioDecoderContext->get());
    if (ret < 0) {
        SPDLOG_DEBUG("GenericAudioSetup() avcodec_parameters_from_context failed");
        emit writeFailed("Failed to copy context from input to output stream codec context");
        return false;
    }
    SPDLOG_DEBUG("GenericAudioSetup() param bits_per_coded_sample={}, channels={}, sampleRate={}, bitRate={}",
                 out_stream->codecpar->bits_per_coded_sample,
                 out_stream->codecpar->channels,
                 out_stream->codecpar->sample_rate,
                 out_stream->codecpar->bit_rate
                 );

    // AAC
    if (ffmpegCodecID == AVCodecID::AV_CODEC_ID_AAC)
    {
        out_stream->codecpar->extradata = (uint8_t*)av_mallocz(16 + AV_INPUT_BUFFER_PADDING_SIZE);
        out_stream->codecpar->extradata_size = 16;
        // object id, sampling rate, channel
        unsigned char frequencyIndex = 0;

        switch (audioData->audioSampleRate)
        {
        case 96000:
            frequencyIndex = 0;
            break;
        case 88200:
            frequencyIndex = 1;
            break;
        case 64000:
            frequencyIndex = 2;
            break;
        case 48000:
            frequencyIndex = 3;
            break;
        case 44100:
            frequencyIndex = 4;
            break;
        case 32000:
            frequencyIndex = 5;
            break;
        case 24000:
            frequencyIndex = 6;
            break;
        case 22050:
            frequencyIndex = 7;
            break;
        case 16000:
            frequencyIndex = 8;
            break;
        case 12000:
            frequencyIndex = 9;
            break;
        case 11025:
            frequencyIndex = 10;
            break;
        case 8000:
            frequencyIndex = 11;
            break;
        case 7350:
            frequencyIndex = 12;
            break;
        default:
            frequencyIndex = 8;
            break;
        }
        // reference site: http://wiki.multimedia.cx/index.php?title=ADTS
        // Structure : CCDEEFFF FGHHH000
        // Letter Length(bits) Description
        //   C        2        Layer: always 0
        //   D        1        protection absent, Warning, set to 1 if there is no CRC and 0 if there is CRC
        //   E        2        profile, the MPEG-4 Audio Object Type minus 1 (2: AAC LC(Low Complexity)
        //   F        4        MPEG-4 Sampling Frequency Index(15 is forbidden)
        //   G        1        private bit, guaranteed never to be used by MPEG, set to 0 when encoding, ignore when decoding
        //   H        3        MPEG-4 Channel Configuration (in the case of 0, the channel configuration is sent via an inband PCE)

        out_stream->codecpar->extradata[0] = 0x10 | ((frequencyIndex & 0xE) >> 1);
        out_stream->codecpar->extradata[1] = ((frequencyIndex & 0x01) << 7) | ((1/*audioData->audioChannels*/ & 0x07) << 3);
        //out_stream->codec->extradata[0] = 0x14;
        //out_stream->codec->extradata[1] = 0x08;
    }

    m_avOutAudioStream.Reset(out_stream, FFmpegStreamInfo::StreamType::STREAM_AUDIO);

    return true;
}

bool FFmpegFileOutContext::TimedTextSetup()
{
    if(!m_avOutFormatContext)
        return false;

    if (!m_isTimedText && !m_avOutVideoStream.GetStream())
        return false;

    // srt decoder
    auto *decoderCodec = avcodec_find_decoder(AV_CODEC_ID_SUBRIP);
    m_avSubripDecoderContext = std::make_shared<AVCodecContextWrapper>(decoderCodec);
    if (m_avSubripDecoderContext->get() == NULL)
    {
        SPDLOG_DEBUG("TimedTextSetup() avcodec_alloc_context3 failed");
        return false;
    }
    int ret = avcodec_open2(m_avSubripDecoderContext->get(), decoderCodec, NULL);
    if (ret < 0)
    {
        SPDLOG_DEBUG("TimedTextSetup() Failed to create AV_CODEC_ID_SUBRIP codec context");
        return false;
    }

    // find encoder
    auto *av_codec = avcodec_find_encoder(AV_CODEC_ID_SUBRIP);
    if (av_codec == NULL)
    {
        SPDLOG_DEBUG("TimedTextSetup() avcodec_find_encoder failed");
        emit writeFailed("Can not find codec");
        return false;
    }

    // new stream
    AVStream *out_stream = avformat_new_stream(m_avOutFormatContext, NULL);
    if (out_stream == NULL)
    {
        SPDLOG_DEBUG("TimedTextSetup() avformat_new_stream failed");
        emit writeFailed("Can not create out stream");
        return false;
    }
    out_stream->id = m_avOutFormatContext->nb_streams-1;

    // aloc codec context
    AVCodecContextWrapper codecContext(av_codec);
    if (codecContext.get() == NULL)
    {
        SPDLOG_DEBUG("TimedTextSetup() avcodec_alloc_context3 failed");
        emit writeFailed("Can not create codec context");
        return false;
    }
    codecContext.get()->codec_id = AV_CODEC_ID_SUBRIP;
    codecContext.get()->subtitle_header = (uint8_t*)av_strdup((char*)m_avSubripDecoderContext->get()->subtitle_header);
    codecContext.get()->subtitle_header_size = m_avSubripDecoderContext->get()->subtitle_header_size;
    out_stream->time_base.num = 1;
    out_stream->time_base.den = 1000;
    codecContext.get()->time_base = out_stream->time_base;

    /* Some formats want stream headers to be separate. */
    if (m_avOutFormatContext->oformat->flags & AVFMT_GLOBALHEADER) {
        codecContext.get()->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
    }

    // codec open
    ret = avcodec_open2(codecContext.get(), av_codec, NULL);
    if(ret < 0)
    {
        SPDLOG_ERROR("TimedTextSetup() avcodec_open2 ret:{}", av_make_error_stdstring(ret));
        emit writeFailed("Can not open codec context");
        return false;
    }

    /* copy the stream parameters to the muxer */
    ret = avcodec_parameters_from_context(out_stream->codecpar, codecContext.get());
    if (ret < 0) {
        SPDLOG_DEBUG("TimedTextSetup() avcodec_parameters_from_context failed");
        emit writeFailed("Can not copy codec context");
        return false;
    }

    m_avOutSubtitleStream.Reset(out_stream, FFmpegStreamInfo::StreamType::STREAM_SUBTITLE);

    m_subTitleOutBuffer = (uint8_t*)av_malloc(kKaxSubTitleOutSize);

    return true;
}

bool FFmpegFileOutContext::SetupFirst()
{
    m_startTime = 0;
    m_endTime = 0;
    m_prevSeconds = 0;

    for(Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaData : m_readyQueue)
    {
        if (mediaData->frameTime.ptsTimestampMsec != 0)
        {
            m_startTime = mediaData->frameTime.ptsTimestampMsec;
            break;
        }
    }

    // alloc OutFormatContext
    if (AllocContext() == false)
        return false;

    int retryDecodingCount = 3;
    for(Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaData : m_readyQueue)
    {
        if ((mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO && m_avOutVideoStream.GetStream()) ||
                (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO && m_avOutAudioStream.GetStream()))
        {
            continue;
        }

        bool isVideoDecodingFail = false;
        if (GenericSetup(mediaData, isVideoDecodingFail) == false)
        {
            // retry 3 times..
            if (isVideoDecodingFail && retryDecodingCount > 0)
            {
                retryDecodingCount--;
                continue;
            }
            else
            {
                emit writeFailed("Can not get video information : decoding of I frame failed.");
                return false;
            }
        }

        if (m_avOutVideoStream.GetStream() && m_avOutAudioStream.GetStream())
        {
            break;
        }
    }

    if (m_avOutVideoStream.GetStream() == NULL)
    {
        SPDLOG_DEBUG("SetupFirst() video stream is not set...");
        return false;
    }

    /*
    if(m_avOutAudioStream.GetStream() == NULL)
    {
        AVStream* outAVStream = avformat_new_stream(m_avOutFormatContext, m_avOutFormatContext->audio_codec);
        if(outAVStream != NULL)
        {
            outAVStream->codec->codec_tag = 0;
            if (m_avOutFormatContext->oformat->flags & AVFMT_GLOBALHEADER)
                outAVStream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;

            m_avOutAudioStream.Reset(outAVStream, FFmpegStreamInfo::StreamType::STREAM_AUDIO);
        }
    }

    if(m_avOutSubtitleStream.GetStream() == NULL)
    {

    }
    */

    // file open
    if (m_avOutVideoStream.GetStream() || m_avOutAudioStream.GetStream())
    {
        if (m_isTimedText && m_avOutVideoStream.GetStream())
        {
            // subtitle setup
            if(TimedTextSetup() == false && m_avSubripDecoderContext)
            {
                m_avSubripDecoderContext.reset();
            }
        }

        if (OpenFile() == false)
        {
            SPDLOG_DEBUG("SetupFirst() open file() failed...");
            return false;
        }
    }

    return true;
}

bool FFmpegFileOutContext::IsCodecChagned(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    AVStream *avStream = (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO ?
                              m_avOutVideoStream.GetStream():
                              m_avOutAudioStream.GetStream());

    if (!avStream)
        return true;

    if (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO)
    {
        auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
        if (get_vcodec_id(videoData->videoCodecType) != avStream->codecpar->codec_id)
            return true;

        if (videoData->videoWidth != avStream->codecpar->width ||
                videoData->videoHeight != avStream->codecpar->height)
            return true;
    }
    else if (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO)
    {
        auto audioData = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(mediaData);
        if (get_acodec_id(audioData->audioCodecType) != avStream->codecpar->codec_id)
        {
            if (m_avAudioDecoderContext->get())
            {
                if (m_avAudioDecoderContext->get()->codec_id != get_acodec_id(audioData->audioCodecType))
                    return true;
                return false;
            }
            return true;
        }

    }

    return false;
}

bool FFmpegFileOutContext::QueueSetup(void)
{
    if (m_isReady)
        return true;

    // setup out
    if (SetupFirst() == false)
    {
        ClearQueue();
        return false;
    }

    // write file
    for(Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaData : m_readyQueue)
    {
        if (WriteFile(mediaData) == false)
        {
            return false;
        }
    }

    ClearQueue();
    return true;
}

void FFmpegFileOutContext::SetOutFileInfo(QString fileDir, QString fileName)
{
    m_fileDir = fileDir;
    m_fileName = fileName;
}

bool FFmpegFileOutContext::DoWrite(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    if (!m_isReady)
    {
        bool isAudioFrame = (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO);
        bool isVideoFrame = (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO);

        if (!m_audioFrameChecked && isAudioFrame) {
            m_audioFrameChecked = true;
        }

        if (!m_videoFrameChecked && isVideoFrame) {
            auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
            if (videoData->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME) {
                // i-frame을 받고 1초간 대기하면서 audio가 들어오는 지 추가 확인한다.
                m_videoFrameChecked = true;
                m_lastVideoFrameCheckedClock = std::chrono::steady_clock::now();
            }
        }

        if (isVideoFrame && !m_videoFrameChecked) {
            // 초기화되기전 키프레임이 아닌 프레임들은 모두 버린다.
            return true;
        }

        m_readyQueue.push_back(mediaData);

        // 1. video&audio프레임이 모두 들어온 경우에는 즉시 SETUP
        bool doSetup = m_videoFrameChecked && m_audioFrameChecked;
        // 2. video프레임만 들어오고 1초가 지난 경우
        if (m_videoFrameChecked && !doSetup) {
            auto end = std::chrono::steady_clock::now();
            auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_lastVideoFrameCheckedClock).count();
            if (elapsedMsec >= kBufferWaitTimeMsec) {
                doSetup = true;
            }
        }

        // push queue until check audio/video codec status
        if (doSetup) {
            SPDLOG_DEBUG("Try to QueueSetup(), video={}, audio={}", m_videoFrameChecked, m_audioFrameChecked);
            if (QueueSetup() == false) {
                m_audioFrameChecked = false;
                m_videoFrameChecked = false;
                return false;
            }

            m_isReady = true;
            return true;
        }

        return true;
    }

    if(!m_avOutFormatContext)
        return false;

    bool isNew = false;
    int64_t fsize = avio_size(m_avOutFormatContext->pb);
    if (IsCodecChagned(mediaData) == true)
    {
        SPDLOG_DEBUG("DoWrite() Codec infomation is changed...");
        isNew = true;
    }
    else if (fsize >= kMaxFileSizeByte)
    {
        SPDLOG_DEBUG("DoWrite() file size is too big...");
        isNew = true;
    }

    if (isNew)
    {
        CloseFile();
        m_fileNo++;
        m_readyQueue.push_back(mediaData);
        m_isReady = false;
        m_videoFrameChecked = false;
        m_audioFrameChecked = false;
        return true;
    }

    bool ret = WriteFile(mediaData);
    if (ret == false)
    {
        emit writeFailed("Could not write data..");
    }
    return ret;
}

bool FFmpegFileOutContext::WriteFile(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    if (mediaData->getDataSize() == 0)
        return true;

    AVStream *outStream = nullptr;
    FFmpegStreamInfo *streamInfo = nullptr;

    if (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO)
        streamInfo = &m_avOutVideoStream;
    else if (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO)
        streamInfo = &m_avOutAudioStream;
    /* TODO : SUBTITLE
    else if (mediaData->Type == oscar::media::SUBTITLE)
        streamInfo = &m_avOutSubtitleStream;
    */

    if (!streamInfo)
        return false;

    outStream = streamInfo->GetStream();
    if (!outStream)
        return false;

    AVPacket pkt = { 0 };
    av_init_packet(&pkt);
    pkt.data = mediaData->getDataPtr();
    pkt.size = mediaData->getDataSize();
    pkt.stream_index = outStream->index;
    pkt.pos = -1;

    AVRational msecTimebase;
    msecTimebase.den = 1000;
    msecTimebase.num = 1;

    if (mediaData->getMediaType() == Wisenet::Media::MediaType::VIDEO)
    {
        auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaData);
        if(videoData->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME)
            pkt.flags |= AV_PKT_FLAG_KEY;

        WriteVideo(mediaData, outStream, streamInfo, &pkt, msecTimebase);
    }
    else if (mediaData->getMediaType() == Wisenet::Media::MediaType::AUDIO)
    {
        WriteAudio(mediaData, outStream, streamInfo, &pkt, msecTimebase);
    }

    //std::cout << "write frame=" << pkt.size << std::endl;

    int ret = av_interleaved_write_frame(m_avOutFormatContext, &pkt);
    if (ret < 0)
    {
        SPDLOG_DEBUG("WriteFile() interleaved_write_frame() failed");
#ifndef IGNORE_WRITE_ERROR
        av_packet_unref(&pkt);
        return false;
#endif
    }
    av_packet_unref(&pkt);

    m_endTime = mediaData->frameTime.ptsTimestampMsec;

    // SUBTITLE WRITE
    if (outStream == m_avOutVideoStream.GetStream() &&
            m_isTimedText &&
            m_avOutSubtitleStream.GetStream() &&
            m_prevSeconds != mediaData->frameTime.ptsTimestampMsec / 1000 &&
            m_avOutVideoStream.GetDts() - m_avOutSubtitleStream.GetDts() >= 1000)
    {
        WriteSubtitle(mediaData);
    }

    return true;
}

bool FFmpegFileOutContext::WriteVideo(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData,
                                      AVStream *outStream,
                                      FFmpegStreamInfo *streamInfo,
                                      AVPacket *pkt,
                                      AVRational msecTimebase)
{
    streamInfo->UpdateTimeStamp(mediaData->frameTime.ptsTimestampMsec);

    int64_t diff = streamInfo->GetDurationTimeStamp();
    int64_t prevDuration = streamInfo->GetPreviousDuration();

    if (diff == 0 && streamInfo->GetDts() == 0)
    {
        diff = 0; // first
    }
    else if (diff >= 3000)
    {
        if (prevDuration > 0 && prevDuration < 3000)
            diff = prevDuration;
        else
            diff = 10;
    }
    else if (diff < 10)
    {
        if (prevDuration > 10)
            diff = prevDuration;
        else
            diff = 10;
    }

    int64_t dts = streamInfo->UpdateDts(diff);

    pkt->pts = pkt->dts = dts;
    pkt->duration = 0;

    pkt->pts = av_rescale_q(pkt->pts, msecTimebase, outStream->time_base);
    pkt->dts = av_rescale_q(pkt->dts, msecTimebase, outStream->time_base);
    pkt->duration = (int)av_rescale_q(pkt->duration, msecTimebase, outStream->time_base);
    //pkt.pts = av_rescale_q_rnd(pkt.pts, msecTimebase, outStream->time_base,
    //	(enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
    //pkt.dts = av_rescale_q_rnd(pkt.dts, msecTimebase, outStream->time_base,
    //	(enum AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));

    //fprintf(stderr, "sec=%"PRId64", ms=%"PRId64"d, vdts=%"PRId64", diff=%"PRId64", pkt.dts=%"PRId64"\n",
    //	data->PresentationTime.Seconds, data->PtsMsec, dts, diff, pkt.dts);

    return true;
}

bool FFmpegFileOutContext::WriteAudio(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData,
                                      AVStream *outStream,
                                      FFmpegStreamInfo *streamInfo,
                                      AVPacket *pkt,
                                      AVRational msecTimebase)
{
    bool isFirstTime = streamInfo->IsFirstTimeStamp();

    streamInfo->UpdateTimeStamp(mediaData->frameTime.ptsTimestampMsec);

    int64_t diff = streamInfo->GetDurationTimeStamp();
    int64_t prevDuration = streamInfo->GetPreviousDuration();
    int64_t dts = 0;

    if (isFirstTime)
    {
        dts = streamInfo->UpdateDts(0);
    }
    else if (diff == 0 &&
             m_avOutVideoStream.GetStream() != NULL &&
             m_avOutVideoStream.GetDts() != 0)
    {
        dts = m_avOutVideoStream.GetDts();
        if (dts > streamInfo->GetDts())
            streamInfo->SetDts(dts);
        else
        {
            dts = streamInfo->UpdateDts(10);
        }
    }
    else if (diff >= 3000)
    {
        if (prevDuration > 0 && prevDuration < 3000)
            diff = prevDuration;
        else
            diff = 10;

        dts = streamInfo->UpdateDts(diff);
    }
    else if (diff < 10)
    {
        if (prevDuration > 10)
            diff = prevDuration;
        else
            diff = 10;
        dts = streamInfo->UpdateDts(diff);
    }
    else
    {
        dts = streamInfo->UpdateDts(diff);
    }


    pkt->pts = pkt->dts = dts;
    pkt->duration = 0;

    pkt->pts = av_rescale_q(pkt->pts, msecTimebase, outStream->time_base);
    pkt->dts = av_rescale_q(pkt->dts, msecTimebase, outStream->time_base);
    pkt->duration = (int)av_rescale_q(pkt->duration, msecTimebase, outStream->time_base);

    //std::cout << "data=>" << data->PtsMsec << ",packet.dts ==>" << pkt.dts << ", audio DTS==>" << dts << std::endl;

    return true;
}

bool FFmpegFileOutContext::WriteSubtitle(Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData)
{
    m_avOutSubtitleStream.SetDts(m_avOutVideoStream.GetDts());

    AVRational msecTimebase;
    msecTimebase.den = 1000;
    msecTimebase.num = 1;

    m_prevSeconds = mediaData->frameTime.ptsTimestampMsec / 1000;
    std::string timedText = "";
    timedText = QLocaleManager::Instance()->getDateTime24h(mediaData->frameTime.ptsTimestampMsec).toStdString();

    AVPacket sub_pkt_out;
    av_init_packet(&sub_pkt_out);
    int subOutSize = 0;

    if (!m_avSubripDecoderContext)
    {
        SPDLOG_DEBUG("WriteFile() interleaved_write_frame() failed");
        fprintf(stderr, "m_avSubripDecoderContext is null.. skip\n");
        av_packet_unref(&sub_pkt_out);
        return true;
    }
    else
    {
        subOutSize = timedText.length();
        std::strncpy((char*)m_subTitleOutBuffer, timedText.c_str(), subOutSize);
    }

    sub_pkt_out.data = m_subTitleOutBuffer;;
    sub_pkt_out.size = subOutSize;
    sub_pkt_out.pts =  av_rescale_q(m_avOutSubtitleStream.GetDts(), msecTimebase, m_avOutSubtitleStream.GetStream()->time_base);
    sub_pkt_out.dts = sub_pkt_out.pts;
    sub_pkt_out.duration = (int)av_rescale_q(1000, msecTimebase, m_avOutSubtitleStream.GetStream()->time_base);
    sub_pkt_out.stream_index = m_avOutSubtitleStream.GetStream()->index;

    int ret = av_interleaved_write_frame(m_avOutFormatContext, &sub_pkt_out);
    if (ret < 0)
    {
        SPDLOG_DEBUG("WriteFile() Error av_interleaved_write_frame(sub_title) failed");
    }
    av_packet_unref(&sub_pkt_out);

    return true;
}

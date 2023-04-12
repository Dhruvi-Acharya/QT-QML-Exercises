/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "VideoDecoder.h"
#include "LogSettings.h"
#include <chrono>

static const std::string av_make_error_string(int errnum)
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE];
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return (std::string)errbuf;
}

static const std::string av_make_pix_fmt_string(int pix_fmt)
{
    const char* pix_name = av_get_pix_fmt_name((AVPixelFormat)pix_fmt);
    return (std::string)pix_name;
}

static int Align16(int value)
{
    return (((value + 15) >> 4) << 4);
}

VideoDecoder::VideoDecoder(ThreadSafeQueue<RawVideoDataPtr>& decodeQueue,
                           ThreadSafeQueue<DecodedVideoDataPtr>& renderQueue,
                           bool useYuvTexture)
    : m_codecContext(NULL)
    , m_frame(NULL)
    , m_swsContext(NULL)
    , m_decodeQueue(decodeQueue)
    , m_renderQueue(renderQueue)
    , m_bUseYuv(useYuvTexture)
{
    SPDLOG_DEBUG("VideoDecoder::VideoDecoder()");
}

VideoDecoder::~VideoDecoder()
{
    SPDLOG_DEBUG("VideoDecoder::~VideoDecoder()");
    Release();
}

void VideoDecoder::initializeWithCodecContext(FFmpegCodecParamWrapperPtr contextPtr)
{
    int ret = 0;

    SPDLOG_DEBUG("VideoDecoder::InitializeWithCodecContext()");
    if (m_codecContext != NULL) {
        SPDLOG_INFO("VideoDecoder::InitializeWithCodecContext() release opened decoder context first.");
        Release();
    }

    AVCodecParameters *param = contextPtr->getParam();

    // find decoder for the stream
    AVCodec *dec = avcodec_find_decoder(param->codec_id);
    if (!dec) {
        SPDLOG_ERROR("Can not find video decoder");
        Release();
        return;
    }

    // Allocate a codec context for the decoder
    m_codecContext = avcodec_alloc_context3(dec);
    if (!m_codecContext) {
        SPDLOG_ERROR("Failed to allocate the video codec context");
        Release();
        return;
    }

    // Copy codec parameters from input stream to output codec context
    ret = avcodec_parameters_to_context(m_codecContext, param);
    if (ret < 0) {
        SPDLOG_ERROR("Failed to copy video codec parameters to decoder context, ret={}", ret);
        Release();
        return;
    }

    // set thread count for decoding speed (need to calculate with resolution and fps...)
    m_codecContext->thread_count = 5;

    // Init the decoder
    ret = avcodec_open2(m_codecContext, dec, NULL);
    if (ret < 0) {
        SPDLOG_ERROR("Failed to open video codec decoder, ret={}", ret);
        Release();
        return;
    }

    m_frame = av_frame_alloc();
    if (!m_frame) {
        SPDLOG_ERROR("Could not allocate frame");
        Release();
        return;
    }

    // initialize packet, set data to NULL
    av_init_packet(&m_packet);
    m_packet.data = NULL;
    m_packet.size = 0;

    SPDLOG_INFO("VideoDecoder:: Success to open video codec decoder, thread_count={}", m_codecContext->thread_count);
}

void VideoDecoder::Release()
{
    if (m_codecContext != NULL) {
        avcodec_free_context(&m_codecContext);
        m_codecContext = NULL;
    }
    if(m_frame != NULL) {
        av_frame_free(&m_frame);
        m_frame = NULL;
    }
    if (m_swsContext != NULL) {
        sws_freeContext(m_swsContext);
        m_swsContext = NULL;
    }
    m_lastDts = 0;
}

static bool needToScale(const int dstWidth, const int dstHeight, const int frameWidth, const int frameHeight)
{
    if (dstWidth == 0 || dstHeight == 0)
        return false;

    if (dstWidth >= frameWidth || dstHeight >= frameHeight)
        return false;

    size_t orgResolution = frameWidth * frameHeight;
    size_t calcResolution = dstWidth * dstHeight;

    double scaleFactor = 0.7;
    if ( (orgResolution * scaleFactor) >= calcResolution )
        return true;

    return false;
}

void VideoDecoder::handleVideoDecode(const int param)
{
    SPDLOG_TRACE("VideoDecoder::handleVideoDecode()");

    auto start = std::chrono::steady_clock::now();
    int ret = 0;
    bool gotPicture = false;
    RawVideoDataPtr dataPtr;

    if (m_codecContext == NULL || m_frame == NULL) {
        SPDLOG_CRITICAL("VideoDecoder::handleDecodeTimer() internal data is invalid, codec context or frame is nullptr");
        goto __EMIT_AND_RETURN;
    }

    if (!m_decodeQueue.pop_front(dataPtr)) {
        SPDLOG_INFO("Decode queue is empty...");
        goto __EMIT_AND_RETURN;
    }

    if (spdlog::should_log(spdlog::level::trace)) {
        SPDLOG_TRACE("VideoDecoder::handleDecodeTimer() read video, size={}, queue={}",
                     dataPtr->size(), m_decodeQueue.size());
    }


    m_packet.size = dataPtr->size();
    m_packet.data = dataPtr->data();
    m_packet.dts = dataPtr->dts();

    ret = avcodec_send_packet(m_codecContext, &m_packet);
    if (ret < 0) {
        SPDLOG_ERROR("Error submitting a packet for decoding, err={}, message={}", ret, av_make_error_string(ret));
        goto __EMIT_AND_RETURN;
    }

    // get all the available frames from the decoder

    while (ret >= 0) {
        ret = avcodec_receive_frame(m_codecContext, m_frame);
        if (ret < 0) {
            // those two return values are special and mean there is no output
            // frame available, but there were no errors during decoding
            if (ret == AVERROR_EOF || ret == AVERROR(EAGAIN)) {
                SPDLOG_TRACE("DECODE EOF");
                goto __EMIT_AND_RETURN;
            }

            SPDLOG_ERROR("Error during decoding, err={}, message={}", ret, av_make_error_string(ret));
            goto __EMIT_AND_RETURN;
        }

        SPDLOG_TRACE("Decoding success!!, ret={}, fmt={}", ret, av_make_pix_fmt_string(m_frame->format));

        // check window size..
        int tmpDstWidth = m_frame->width;
        int tmpDstHeight = m_frame->height;

        if (needToScale(m_windowWidth, m_windowHeight, tmpDstWidth, tmpDstHeight)) {
            if (tmpDstWidth > tmpDstHeight) {
                tmpDstWidth = Align16(m_windowWidth);
                tmpDstHeight = Align16(m_windowWidth * m_frame->height / m_frame->width);
            }
            else {
                tmpDstHeight = Align16(m_windowHeight);
                tmpDstWidth = Align16(m_windowHeight * m_frame->width / m_frame->height);
            }
        }

        if ( m_swsContext == NULL ||
             m_srcHeight != m_frame->height || m_srcWidth != m_frame->width ||
             tmpDstWidth != m_dstWidth || tmpDstHeight != m_dstHeight) {
            if (m_swsContext)
                sws_freeContext(m_swsContext);

            SPDLOG_INFO("Create SwsContext, dst_width={}, dst_height={}", tmpDstWidth, tmpDstHeight);
            m_srcWidth = m_frame->width;
            m_srcHeight = m_frame->height;
            m_srcFormat = (AVPixelFormat)m_frame->format;
            m_dstWidth = tmpDstWidth;
            m_dstHeight = tmpDstHeight;
            m_dstFormat = m_bUseYuv ? AV_PIX_FMT_YUV420P : AV_PIX_FMT_RGB24;

            m_swsContext = sws_getContext(m_srcWidth, m_srcHeight, m_srcFormat,
                                          m_dstWidth, m_dstHeight, m_dstFormat,
                                          SWS_BILINEAR, NULL, NULL, NULL);
        }
        /* convert to destination format */
        uint8_t *src_data[4] = {NULL}, *dst_data[4] = {NULL};
        int src_linesize[4] = {0}, dst_linesize[4] = {0};
        src_data[0] = m_frame->data[0];
        src_data[1] = m_frame->data[1];
        src_data[2] = m_frame->data[2];
        src_data[3] = m_frame->data[3];
        src_linesize[0] = m_frame->linesize[0];
        src_linesize[1] = m_frame->linesize[1];
        src_linesize[2] = m_frame->linesize[2];
        src_linesize[3] = m_frame->linesize[3];

        auto decodedData = std::make_shared<DecodedVideoData>(m_dstWidth, m_dstHeight, m_dstFormat);
        dst_data[0] = decodedData->data(0);
        dst_data[1] = decodedData->data(1);
        dst_data[2] = decodedData->data(2);
        dst_data[3] = decodedData->data(3);

        dst_linesize[0] = decodedData->lineSize(0);
        dst_linesize[1] = decodedData->lineSize(1);
        dst_linesize[2] = decodedData->lineSize(2);
        dst_linesize[3] = decodedData->lineSize(3);

        sws_scale(m_swsContext, (const uint8_t * const*)src_data,
                  src_linesize, 0, m_srcHeight, dst_data, dst_linesize);

        SPDLOG_DEBUG("Decode And Scale(SOURCE), width={}, height={}, fmt={}, line0={}, line1={}, line2={}, line=3={}, timestamp={}",
                     m_srcWidth, m_srcHeight, av_make_pix_fmt_string((int)m_srcFormat),
                     src_linesize[0], src_linesize[1], src_linesize[2], src_linesize[3], m_frame->best_effort_timestamp);
        SPDLOG_DEBUG("Decode And Scale(TARGET), width={}, height={}, fmt={}, line0={}, line1={}, line2={}, line=3={}",
                     m_dstWidth, m_dstHeight, av_make_pix_fmt_string((int)m_dstFormat),
                     dst_linesize[0], dst_linesize[1], dst_linesize[2], dst_linesize[3]);

        m_renderQueue.push(decodedData);
        gotPicture = true;
        //emit decodeResultReady(ret, elapsedMsec);
    }

__EMIT_AND_RETURN:
    auto end = std::chrono::steady_clock::now();
    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (gotPicture)
        ret = 0;

    m_decodeCount++;
    m_decodeTimeMsec += elapsedMsec;
    if (m_decodeCount == 100) {
        double frame100Avg = (double)m_decodeTimeMsec / m_decodeCount;
        STATLOG_INFO("Decoding 100 frame avg elapsed Millseconds={}", frame100Avg);
        m_decodeCount = 0;
        m_decodeTimeMsec = 0;
    }

    emit decodeResultReady(ret, elapsedMsec);
}

void VideoDecoder::handleSetVideoOutResolution(const int width, const int height)
{
    SPDLOG_INFO("Set video out resolution {}x{}", width, height);
    m_windowWidth = width;
    m_windowHeight = height;
}

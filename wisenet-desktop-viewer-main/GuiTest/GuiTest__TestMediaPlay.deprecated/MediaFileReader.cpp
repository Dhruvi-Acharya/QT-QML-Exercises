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
#include "MediaFileReader.h"
#include "LogSettings.h"

static void log_callback(void* ptr, int level, const char* fmt, va_list vl)
{
    static int print_prefix = 1;
    char line[1024] = {0};
    av_log_format_line(ptr, level, fmt, vl, line, 1024, &print_prefix);

    SPDLOG_INFO("FFmpeg::{}", line);
}



MediaFileReader::MediaFileReader(ThreadSafeQueue<RawVideoDataPtr>& decodeQueue)
    : m_fmtContext(NULL)
    , m_videoStream(NULL)
    , m_decodeQueue(decodeQueue)
{
    SPDLOG_DEBUG("MediaFileReader::MediaFileReader()");
}

MediaFileReader::~MediaFileReader()
{
    SPDLOG_DEBUG("MediaFileReader::~MediaFileReader()");
    closeInternal();

}

void MediaFileReader::open(const QString &filePath)
{
    std::string path = filePath.toStdString();
    int err = 0;
    SPDLOG_DEBUG("MediaFileReader::open(), file={}", path);

//    av_log_set_callback(log_callback);
//    av_log_set_level(AV_LOG_WARNING);

    if (m_fmtContext) {
        SPDLOG_INFO("MediaFileReader::open(), close opend file first");
        closeInternal();
    }

    AVFormatContext* fmtContext = NULL;
    AVStream *videoStream = NULL;

    err = avformat_open_input(&fmtContext, path.c_str(), NULL, NULL);
    if (err < 0) {
        SPDLOG_ERROR("Can not open media file, path={}", path);
        emit openResultReady(err, 0);
        return;
    }

    err = avformat_find_stream_info(fmtContext, NULL);
    if (err < 0) {
        SPDLOG_ERROR("Can not find media stream infomation, path={}", path);
        emit openResultReady(err, 0);
    }

    err = av_find_best_stream(fmtContext, AVMEDIA_TYPE_VIDEO, -1, -1, NULL, 0);
    if (err < 0) {
        SPDLOG_ERROR("Can not find video stream, path={}", path);
        emit openResultReady(err, 0);
    }

    videoStream = fmtContext->streams[err];

    if (spdlog::should_log(spdlog::level::debug)) {
        av_dump_format(fmtContext, 0, path.c_str(), 0);
    }

    m_fmtContext = fmtContext;
    m_videoStream = videoStream;

    double fps = av_q2d(m_videoStream->avg_frame_rate);

    SPDLOG_INFO("Media file open success..fps={}, {}", fps, av_q2d(m_videoStream->r_frame_rate));
    emit openResultReady(err, fps);

    // send codec context to decoder
    FFmpegCodecParamWrapperPtr paramPtr(new FFmpegCodecParamWrapper);
    paramPtr->CopyParam(videoStream->codecpar);
    emit openCodecContextReady(paramPtr);

    QString message = QString("File=%1, Resolution=%2x%3, fps=%4, Codec=%5")
            .arg(filePath)
            .arg(videoStream->codecpar->width)
            .arg(videoStream->codecpar->height)
            .arg(fps, 0, 'g', 5)
            .arg(avcodec_get_name(videoStream->codecpar->codec_id));
    emit videoInfoChanged(message);
}

void MediaFileReader::read()
{
    if (m_fmtContext == NULL || m_videoStream == NULL) {
        SPDLOG_CRITICAL("invalid internal data, fmtContext or video stream is null");
        emit readResultReady(-1);
        return;
    }

    //SPDLOG_DEBUG("MediaFileReader::read()");
    int ret = av_read_frame(m_fmtContext, &m_packet);
    if (ret < 0) {
        if (ret == AVERROR_EOF || avio_feof(m_fmtContext->pb)) {
            SPDLOG_INFO("av_read_frame() end_of_file, ret={}", ret);
        }
        else {
            SPDLOG_WARN("av_read_frame() failed, ret={}", ret);
        }

        emit readResultReady(ret);
        return;
    }

    if (m_videoStream->index == m_packet.stream_index) {
        //double dts = m_packet.dts * av_q2d(m_videoStream->time_base);

        SPDLOG_TRACE("av_read_frame() success, ret={}, readBytes={}, pts={}, dts={}, conDts={}", ret, m_packet.size, m_packet.pts, m_packet.dts);
        auto videoData = std::make_shared<RawVideoData>(m_packet.size, m_packet.data, m_packet.dts);
        m_decodeQueue.push(videoData);
        emit readVideoResultReady(ret);
    }

    av_packet_unref(&m_packet);
    emit readResultReady(ret);
}

void MediaFileReader::close()
{
    SPDLOG_DEBUG("MediaFileReader::close()");
    closeInternal();
    emit closeResultReady(0);
}

void MediaFileReader::closeInternal()
{
    m_decodeQueue.clear();
    av_packet_unref(&m_packet);

    if (m_fmtContext) {
        avformat_close_input(&m_fmtContext);
        m_fmtContext = NULL;
    }
}

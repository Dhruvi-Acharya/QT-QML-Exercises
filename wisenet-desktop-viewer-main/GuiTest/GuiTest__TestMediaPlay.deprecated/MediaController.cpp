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
#include "MediaController.h"
#include "MediaFileReader.h"
#include "VideoDecoder.h"
#include "LogSettings.h"

MediaController::MediaController(ThreadSafeQueue<RawVideoDataPtr>& decodeVideoQueue,
                                 ThreadSafeQueue<DecodedVideoDataPtr>& renderVideoQueue,
                                 bool useYuvTexture)
    : m_decodeQueue(decodeVideoQueue), m_renderVideoQueue(renderVideoQueue)
{
    // read timer
    m_readTimer = new QTimer(this);
    m_readTimer->setSingleShot(true);
    m_readTimer->setInterval(8);
    connect(m_readTimer, &QTimer::timeout, this, &MediaController::handleReadTimeout);

    // decode timer
    m_decodeTimer = new QTimer(this);
    m_decodeTimer->setSingleShot(true);
    m_decodeTimer->setInterval(8);
    m_decodeTimer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(m_decodeTimer, &QTimer::timeout, this, &MediaController::handleDecodeTimeout);

    // file reader thread setup
    MediaFileReader *reader = new MediaFileReader(decodeVideoQueue);

    // move to thread
    reader->moveToThread(&readThread);

    // deleteLater signal
    connect(&readThread, &QThread::finished, reader, &QObject::deleteLater);

    // controller --> reader
    connect(this, &MediaController::mediaPlayStart, reader, &MediaFileReader::open);
    connect(this, &MediaController::mediaReadContinue, reader, &MediaFileReader::read);

    // reader --> controller
    connect(reader, &MediaFileReader::openResultReady, this, &MediaController::handleOpenResult);
    connect(reader, &MediaFileReader::readResultReady, this, &MediaController::handleReadResult);
    connect(reader, &MediaFileReader::eofResultReady, this, &MediaController::handleEofResult);
    // signal to signal
    connect(reader, &MediaFileReader::videoInfoChanged, this, &MediaController::videoInfoChanged);

    // video decoder thread setup
    VideoDecoder *decoder = new VideoDecoder(decodeVideoQueue, renderVideoQueue, useYuvTexture);

    // move to thread
    decoder->moveToThread(&decodeThread);

    // deleteLater signal
    connect(&decodeThread, &QThread::finished, decoder, &QObject::deleteLater);

    // reader --> decoder
    connect(reader, &MediaFileReader::openCodecContextReady, decoder, &VideoDecoder::initializeWithCodecContext);

    // reader --> decoder
    //connect(reader, &MediaFileReader::readVideoResultReady, decoder, &VideoDecoder::handleVideoDecode);

    // controller --> decoder
    connect(this, &MediaController::videoDecode, decoder, &VideoDecoder::handleVideoDecode);
    connect(this, &MediaController::setVideoOutResolution, decoder, &VideoDecoder::handleSetVideoOutResolution);

    // decoder --> controller
    connect(decoder, &VideoDecoder::decodeResultReady, this, &MediaController::handleVideoDecodeResult, Qt::QueuedConnection);

    readThread.start();
    decodeThread.start();
}

MediaController::~MediaController()
{
    // reader thread stop
    readThread.quit();
    readThread.wait();

    // decoder thread stop
    decodeThread.quit();
    decodeThread.wait();
}

void MediaController::mediaOpen(const QString &filePath, IVideoRenderer* renderer)
{
    m_renderer = renderer;
    m_readTimer->stop();
    m_decodeTimer->stop();

    SPDLOG_INFO("MediaController::mediaOpen() start.. filePath={}", filePath.toStdString());
    emit mediaPlayStart(filePath);
}

void MediaController::setVideoOutSize(const int width, const int height)
{
    SPDLOG_INFO("MediaController::setVideoOutSize():: width={}, height={}", width, height);
    emit setVideoOutResolution(width, height);
}

void MediaController::handleOpenResult(const int ret, const double fps)
{
    SPDLOG_INFO("MediaController:: openResult={}", ret);
    if (ret < 0) {
        /* do nothing */
        return;
    }

    if (fps > 0)
        m_decodeSleepMsec = (int)(1000 / fps + 0.5);
    else
        m_decodeSleepMsec = 33;

    emit mediaReadContinue();

    // start decode timer
    if (m_decodeTimer->isActive())
        m_decodeTimer->stop();

    m_decodeTimer->start(8);
}

void MediaController::handleReadResult(const int ret)
{
    const int maxQ = 10;

    SPDLOG_DEBUG("MediaController:: readResult={}", ret);
    if (ret >= 0) {
        auto queueSize = m_decodeQueue.size();

        if (queueSize  >= maxQ) {
            SPDLOG_TRACE("MediaController:: Queue wait..size={}", queueSize);

            // start wait timer
            if (m_readTimer->isActive())
                m_readTimer->stop();

            m_readTimer->start();
            return;
        }
        emit mediaReadContinue();
    }
}

void MediaController::handleEofResult(const int ret)
{
    SPDLOG_INFO("MediaController:: handleEofResult={}", ret);
}

void MediaController::handleVideoDecodeResult(const int ret, const int elapsedMsec)
{
    SPDLOG_DEBUG("MediaController::handleVideoDecodeResult(), ret={}, elapsedMsec={}", ret, elapsedMsec);
    DecodedVideoDataPtr video;
    if (m_renderVideoQueue.pop_front(video)) {
        if (m_renderer) {
            m_renderer->updateVideoOut(video);
        }
    }

    if (elapsedMsec >= 0 && elapsedMsec < m_decodeSleepMsec) {
        m_decodeTimer->start(m_decodeSleepMsec-elapsedMsec);
    }
    else {
        m_decodeTimer->start(4);
    }

}

void MediaController::handleReadTimeout()
{
    SPDLOG_TRACE("MediaController:: handleReadTimeout()");
    handleReadResult(0);
}

void MediaController::handleDecodeTimeout()
{
    SPDLOG_DEBUG("MediaController:: handleDecodeTimeout()");

    if (m_decodeQueue.size() == 0) {
        SPDLOG_DEBUG("MediaController:: handleDecodeTimeout(), reset timer..");
        m_decodeTimer->start(8);
        return;
    }

    emit videoDecode(0);
}

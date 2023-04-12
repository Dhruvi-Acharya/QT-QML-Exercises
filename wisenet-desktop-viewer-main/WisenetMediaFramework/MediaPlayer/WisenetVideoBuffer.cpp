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
#include "WisenetVideoBuffer.h"
#include "LogSettings.h"

WisenetVideoBuffer::WisenetVideoBuffer()
    : QAbstractPlanarVideoBuffer(NoHandle)
    , m_mapMode(NotMapped)
{

}

void WisenetVideoBuffer::initVideoParam(const VideoOutputFrameSharedPtr &ffVideoFrame)
{
    m_ptsTimeMsec = ffVideoFrame->ptsTimeMsec;
    if (ffVideoFrame->format()==AVPixelFormat::AV_PIX_FMT_YUV420P)
        m_format = QVideoFrame::PixelFormat::Format_YUV420P;
    else if (ffVideoFrame->format()==AVPixelFormat::AV_PIX_FMT_NV12)
        m_format = QVideoFrame::PixelFormat::Format_NV12;
    else if (ffVideoFrame->format()==AVPixelFormat::AV_PIX_FMT_BGRA)
        m_format = QVideoFrame::PixelFormat::Format_BGRA32;
    else if (ffVideoFrame->format()==AVPixelFormat::AV_PIX_FMT_YUVJ420P) {
        m_format = QVideoFrame::PixelFormat::Format_YUV420P;
        m_useJpegColorSpace = true;
    }
}

void WisenetVideoBuffer::copyDataFrom(const VideoOutputFrameSharedPtr &ffVideoFrame)
{
    initVideoParam(ffVideoFrame);

    m_planeCount = ffVideoFrame->planeCount();
    m_numBytes = av_image_alloc(m_data, m_bytesPerLine, ffVideoFrame->width(), ffVideoFrame->height(),
                   ffVideoFrame->format(), 1);

    uint8_t*    srcDataPtr[4] = {nullptr};
    int         srcLineSize[4] = {0};
    for (int i = 0 ; i < 4 ; i++) {
        srcDataPtr[i] = ffVideoFrame->data(i);
        srcLineSize[i] = ffVideoFrame->lineSize(i);
    }
    av_image_copy(m_data, m_bytesPerLine,
                  (const uint8_t **)srcDataPtr, srcLineSize,
                  ffVideoFrame->format(), ffVideoFrame->width(), ffVideoFrame->height());
}

void WisenetVideoBuffer::moveDataFrom(const VideoOutputFrameSharedPtr &ffVideoFrame)
{
    //SPDLOG_DEBUG("WisenetVideoBuffer::moveDataFrom() start");
    initVideoParam(ffVideoFrame);

    m_planeCount = ffVideoFrame->planeCount();
    m_numBytes = (int)ffVideoFrame->dataSize();

    for (int i = 0 ; i < m_planeCount ; i++) {
        m_data[i] = ffVideoFrame->data(i);
        m_bytesPerLine[i] = ffVideoFrame->lineSize(i);
    }
    ffVideoFrame->setBufferMoved();
    //SPDLOG_DEBUG("WisenetVideoBuffer::moveDataFrom() end");
}

WisenetVideoBuffer::~WisenetVideoBuffer()
{
    //SPDLOG_DEBUG("WisenetVideoBuffer::~WisenetVideoBuffer()");
    if (m_data[0] != nullptr) {
        //SPDLOG_DEBUG("WisenetVideoBuffer::~WisenetVideoBuffer() av_freep() called");
        av_freep(&m_data[0]);
    }
}

QAbstractVideoBuffer::MapMode WisenetVideoBuffer::mapMode() const
{
    return m_mapMode;
}

void WisenetVideoBuffer::unmap()
{
    m_mapMode = NotMapped;
}

int WisenetVideoBuffer::map(MapMode mode, int *numBytes, int bytesPerLine[], uchar *data[])
{
    m_mapMode = mode;
    if (numBytes) {
        *numBytes = m_numBytes;
    }
    for (int i = 0 ; i < m_planeCount ; i++) {
        data[i] = m_data[i];
        bytesPerLine[i] = m_bytesPerLine[i];
    }
    return m_planeCount;
}

QVideoFrame::PixelFormat WisenetVideoBuffer::pixelFormat() const
{
    return m_format;
}

qint64 WisenetVideoBuffer::ptsTimeMsec() const
{
    return m_ptsTimeMsec;
}

bool WisenetVideoBuffer::useJpegColorSpace() const
{
    return m_useJpegColorSpace;
}


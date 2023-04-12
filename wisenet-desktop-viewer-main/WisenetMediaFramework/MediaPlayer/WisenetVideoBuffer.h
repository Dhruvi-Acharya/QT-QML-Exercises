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

#include <QAbstractPlanarVideoBuffer>
#include <QVideoFrame>
#include "FFmpegStruct.h"

class WisenetVideoBuffer : public QAbstractPlanarVideoBuffer
{
public:
    explicit WisenetVideoBuffer();
    virtual ~WisenetVideoBuffer();

    MapMode mapMode() const override;
    void unmap() override;
    int map(MapMode mode, int *numBytes, int bytesPerLine[], uchar *data[]) override;

    QVideoFrame::PixelFormat pixelFormat() const;

    qint64 ptsTimeMsec() const;
    bool useJpegColorSpace() const ;

    void copyDataFrom(const VideoOutputFrameSharedPtr& ffVideoFrame);
    void moveDataFrom(const VideoOutputFrameSharedPtr& ffVideoFrame);

private:
    void initVideoParam(const VideoOutputFrameSharedPtr& ffVideoFrame);

private:
    uchar *m_data[4] = {nullptr};
    int m_bytesPerLine[4] = {0};
    int m_planeCount = 0;
    int m_numBytes = 0;
    MapMode m_mapMode;
    QVideoFrame::PixelFormat m_format = QVideoFrame::PixelFormat::Format_Invalid;
    qint64 m_ptsTimeMsec = 0;
    bool m_useJpegColorSpace = false; // for j420p
};


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
#include <QVideoFrame>
#include "FFmpegStruct.h"
#include "MaskItem.h"

#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#endif
#endif


class CvMaskPainter
{
public:
    explicit CvMaskPainter();
    ~CvMaskPainter();

    // public functions
    void paint(const QVideoFrame& frame, const QList<NormalMaskRect>& maskRectList);
    void paint(const VideoOutputFrameSharedPtr& frame, const QList<NormalMaskRect>& maskRectList);

    void setPaintType(WisenetMaskType::MaskPaintType paintType);
    WisenetMaskType::MaskPaintType getPaintType();
    void setInclude(bool include);
    bool getInclude();
    void setRGBColor(uchar r, uchar g, uchar b);
    void getRGBColor(uchar& r, uchar& g, uchar& b);
    void setMosaicCellSize(uchar size);
    uchar getMosaicCellSize();
    void setPixelRatio(float pixelRatio);

private:
    // private functions
    void _paint(int numBytes, int bytesPerLine[4], uchar *data[4], int planeCount, int videoWidth, int videoHeight, const QList<NormalMaskRect>& maskRectList);

private:
    // private variables
    WisenetMaskType::MaskPaintType m_paintType = WisenetMaskType::MaskPaintType::Solid;
    bool m_include = true;
    uchar m_rgbColor[3] = {255, 255, 255};
    uchar m_mosaicCellSize = 5;
    float m_pixelRatio = 1;
#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
    cv::Scalar m_yuvColor[3];
#endif
#endif
};

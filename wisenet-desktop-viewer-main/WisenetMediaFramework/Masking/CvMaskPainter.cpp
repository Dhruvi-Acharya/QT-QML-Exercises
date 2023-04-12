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
#include "CvMaskPainter.h"
#include "WisenetVideoBuffer.h"

CvMaskPainter::CvMaskPainter()
{
    setRGBColor(m_rgbColor[0], m_rgbColor[1], m_rgbColor[2]);   // set color to initialize yuv color scalar
}

CvMaskPainter::~CvMaskPainter()
{

}

void CvMaskPainter::setPaintType(WisenetMaskType::MaskPaintType paintType)
{
    m_paintType = paintType;
}

WisenetMaskType::MaskPaintType CvMaskPainter::getPaintType()
{
    return m_paintType;
}

void CvMaskPainter::setInclude(bool include)
{
    m_include = include;
}

bool CvMaskPainter::getInclude()
{
    return m_include;
}

void CvMaskPainter::setRGBColor(uchar r, uchar g, uchar b)
{
    m_rgbColor[0] = r;
    m_rgbColor[1] = g;
    m_rgbColor[2] = b;

#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
    // convert rgb to yuv
    float R = (float)m_rgbColor[0];
    float G = (float)m_rgbColor[1];
    float B = (float)m_rgbColor[2];
    m_yuvColor[0] = cv::Scalar((0.257 * R) + (0.504 * G) + (0.098 * B) + 16);
    m_yuvColor[1] = cv::Scalar(- (0.148 * R) - (0.291 * G) + (0.439 * B) + 128);
    m_yuvColor[2] = cv::Scalar((0.439 * R) - (0.368 * G) - (0.071 * B) + 128);
#endif
#endif
}

void CvMaskPainter::getRGBColor(uchar& r, uchar& g, uchar& b)
{
    r = m_rgbColor[0];
    g = m_rgbColor[1];
    b = m_rgbColor[2];
}

/**
 * @brief setMosaicCellSize
 * @param size : one mosaic's pixel size
 */
void CvMaskPainter::setMosaicCellSize(uchar size)
{
    if(size < 5)
        m_mosaicCellSize = 5;    // min
    else if(size > 50)
        m_mosaicCellSize = 50;   // max
    else
        m_mosaicCellSize = size;
}

uchar CvMaskPainter::getMosaicCellSize()
{
    return m_mosaicCellSize;
}

/**
 * @brief CvMaskPainter::setPixelRatio
 * @param pixelRatio : videoOutputSize / videoInputSize
 */
void CvMaskPainter::setPixelRatio(float pixelRatio)
{
    if(pixelRatio <= 0 || pixelRatio > 1)
        pixelRatio = 1;

    m_pixelRatio = pixelRatio;
}

void CvMaskPainter::paint(const QVideoFrame& frame, const QList<NormalMaskRect>& maskRectList)
{
    if(frame.pixelFormat() != QVideoFrame::PixelFormat::Format_YUV420P)
        return; // unsupported pixel format

    QAbstractVideoBuffer* buffer = frame.buffer();
    WisenetVideoBuffer* wisenetBuffer = (WisenetVideoBuffer*)buffer;
    int numBytes = 0;
    int bytesPerLine[4] = {0};
    uchar *data[4] = {nullptr};
    int planeCount = wisenetBuffer->map(QAbstractVideoBuffer::MapMode::ReadWrite, &numBytes, bytesPerLine, data);
    //SPDLOG_DEBUG("CvMaskPainter::paint() numBytes:{}, bytesPerLine:{},{},{},{}", numBytes, bytesPerLine[0],bytesPerLine[1],bytesPerLine[2],bytesPerLine[3]);
    _paint(numBytes, bytesPerLine, data, planeCount, frame.width(), frame.height(), maskRectList);
    wisenetBuffer->unmap();
}

void CvMaskPainter::paint(const VideoOutputFrameSharedPtr& frame, const QList<NormalMaskRect>& maskRectList)
{
    if(frame->format() != AVPixelFormat::AV_PIX_FMT_YUV420P)
        return; // unsupported pixel format

    int numBytes = frame->dataSize();
    int bytesPerLine[4] = {0};
    uchar *data[4] = {nullptr};
    int planeCount = frame->planeCount();
    for(int i=0 ; i<planeCount ; i++) {
        bytesPerLine[i] = frame->lineSize(i);
        data[i] = frame->data(i);
    }
    //SPDLOG_DEBUG("CvMaskPainter::paint() numBytes:{}, bytesPerLine:{},{},{},{}", numBytes, bytesPerLine[0],bytesPerLine[1],bytesPerLine[2],bytesPerLine[3]);
    _paint(numBytes, bytesPerLine, data, planeCount, frame->width(), frame->height(), maskRectList);
}

void CvMaskPainter::_paint(int numBytes, int bytesPerLine[4], uchar *data[4], int planeCount, int videoWidth, int videoHeight, const QList<NormalMaskRect>& maskRectList)
{
#ifdef Q_OS_WINDOWS
#ifndef MEDIA_FILE_ONLY
    if(numBytes == 0 || planeCount != 3)
        return; // invalid frame

    int uvHeight = numBytes/bytesPerLine[0] - videoHeight;

    // get YUV matrix of original video data
    cv::Mat mY(videoHeight, bytesPerLine[0], CV_8UC1, data[0]);
    cv::Mat mU(uvHeight, bytesPerLine[1], CV_8UC1, data[1]);
    cv::Mat mV(uvHeight, bytesPerLine[2], CV_8UC1, data[2]);
    cv::Rect boundsY(0, 0, mY.cols, mY.rows);
    cv::Rect boundsU(0, 0, mU.cols, mU.rows);
    cv::Rect boundsV(0, 0, mV.cols, mV.rows);

    // exclude result matrix (same size as original video)
    cv::Mat yExclude;
    cv::Mat uExclude;
    cv::Mat vExclude;
    float mosaicCellSize = (float)m_mosaicCellSize * m_pixelRatio;
    if(!m_include) {
        if(m_paintType == WisenetMaskType::MaskPaintType::Solid) {
            // paint solid color to entire exclude matrix
            yExclude = cv::Mat(videoHeight, bytesPerLine[0], CV_8UC1, m_yuvColor[0]);
            uExclude = cv::Mat(uvHeight, bytesPerLine[1], CV_8UC1, m_yuvColor[1]);
            vExclude = cv::Mat(uvHeight, bytesPerLine[2], CV_8UC1, m_yuvColor[2]);
        }
        else {
            // mosaic entire exclude matrix
            int yResizeWidth = bytesPerLine[0]/mosaicCellSize >= 2 ? bytesPerLine[0]/mosaicCellSize : 2;
            int yResizeHeight = videoHeight/mosaicCellSize >= 2 ? videoHeight/mosaicCellSize : 2;
            int uvResizeWidth = bytesPerLine[1]/mosaicCellSize >= 2 ? bytesPerLine[1]/mosaicCellSize : 2;
            int uvResizeHeight = uvHeight/mosaicCellSize >= 2 ? uvHeight/mosaicCellSize : 2;
            cv::resize(mY, yExclude, cv::Size(yResizeWidth, yResizeHeight), 0, 0, cv::INTER_LINEAR);
            cv::resize(mU, uExclude, cv::Size(uvResizeWidth, uvResizeHeight), 0, 0, cv::INTER_LINEAR);
            cv::resize(mV, vExclude, cv::Size(uvResizeWidth, uvResizeHeight), 0, 0, cv::INTER_LINEAR);
            cv::resize(yExclude, yExclude, cv::Size(bytesPerLine[0], videoHeight), 0, 0, cv::INTER_NEAREST);
            cv::resize(uExclude, uExclude, cv::Size(bytesPerLine[1], uvHeight), 0, 0, cv::INTER_NEAREST);
            cv::resize(vExclude, vExclude, cv::Size(bytesPerLine[2], uvHeight), 0, 0, cv::INTER_NEAREST);
        }
    }

    // handle each maskRect
    for(auto& maskRect : maskRectList) {
        // convert to real size
        int x = maskRect.normalX * videoWidth;
        int y = maskRect.normalY * videoHeight;
        int width = maskRect.normalWidth * videoWidth;
        int height = maskRect.normalHeight * videoHeight;

        // make rect data to even number
        x = x%2 == 0 ? x : x-1;
        y = y%2 == 0 ? y : y-1;
        width = width%2 == 0 ? width : width-1;
        height = height%2 == 0 ? height : height-1;

        cv::Rect yMaskRect(x, y, width, height);
        cv::Rect uvMaskRect(x/2, y/2, width/2, height/2);

        cv::Mat yMask(mY, yMaskRect & boundsY);
        cv::Mat uMask(mU, uvMaskRect & boundsU);
        cv::Mat vMask(mV, uvMaskRect & boundsV);

        if(m_include) {
            if(m_paintType == WisenetMaskType::MaskPaintType::Solid) {
                // paint solid color to maskRect
                yMask = m_yuvColor[0];
                uMask = m_yuvColor[1];
                vMask = m_yuvColor[2];
            }
            else {
                // mosaic maskRect
                int yResizeWidth = yMask.cols/mosaicCellSize >= 2 ? yMask.cols/mosaicCellSize : 2;
                int yResizeHeight = yMask.rows/mosaicCellSize >= 2 ? yMask.rows/mosaicCellSize : 2;
                int uvResizeWidth = uMask.cols/mosaicCellSize >= 2 ? uMask.cols/mosaicCellSize : 2;
                int uvResizeHeight = uMask.rows/mosaicCellSize >= 2 ? uMask.rows/mosaicCellSize : 2;
                cv::Mat yTemp;
                cv::Mat uTemp;
                cv::Mat vTemp;
                cv::resize(yMask, yTemp, cv::Size(yResizeWidth, yResizeHeight), 0, 0, cv::INTER_LINEAR);
                cv::resize(uMask, uTemp, cv::Size(uvResizeWidth, uvResizeHeight), 0, 0, cv::INTER_LINEAR);
                cv::resize(vMask, vTemp, cv::Size(uvResizeWidth, uvResizeHeight), 0, 0, cv::INTER_LINEAR);
                cv::resize(yTemp, yTemp, cv::Size(yMask.cols, yMask.rows), 0, 0, cv::INTER_NEAREST);
                cv::resize(uTemp, uTemp, cv::Size(uMask.cols, uMask.rows), 0, 0, cv::INTER_NEAREST);
                cv::resize(vTemp, vTemp, cv::Size(vMask.cols, vMask.rows), 0, 0, cv::INTER_NEAREST);
                yTemp.copyTo(yMask);
                uTemp.copyTo(uMask);
                vTemp.copyTo(vMask);
            }
        }
        else {
            // copy croped original image of maskRect to exclude result matrix
            cv::Mat yInclude(yExclude, yMaskRect & boundsY);
            cv::Mat uInclude(uExclude, uvMaskRect & boundsU);
            cv::Mat vInclude(vExclude, uvMaskRect & boundsV);
            yMask.copyTo(yInclude);
            uMask.copyTo(uInclude);
            vMask.copyTo(vInclude);
        }
    }

    if(!m_include) {
        // copy exclude result matrix to original video matrix
        yExclude.copyTo(mY);
        uExclude.copyTo(mU);
        vExclude.copyTo(mV);
    }
#endif
#endif
}

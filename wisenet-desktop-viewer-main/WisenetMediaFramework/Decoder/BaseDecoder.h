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

#include <QObject>
#include <QSize>
#include <chrono>
#include <QImage>
#include "FFmpegStruct.h"

struct VideoOutParam
{
    bool    keepOriginalSize = false;
    int     outWidth = 0;
    int     outHeight = 0;
    int     maximumOutSize = 0; // maximum width or height
    VideoOutPixelFormat pixelFormat = VideoOutPixelFormat::Unknown;
    bool needToResize(const int srcWidth, const int srcHeight)
    {
        if (keepOriginalSize)
            return false;
        if(maximumOutSize == 0) {
            if (srcWidth <= outWidth || srcHeight <= outHeight) {
                return false;
            }
        }
        else {
            if((srcWidth >= srcHeight && srcWidth <= maximumOutSize) ||
                    (srcWidth < srcHeight && srcHeight <= maximumOutSize)) {
                return false;
            }
        }
        return true;
    }
};


enum class DecodeResult
{ Success = 0, InvalidParam, DecodeFail, OpenFail };

class BaseDecoder : public QObject
{
    Q_OBJECT
public:
    explicit BaseDecoder(QObject *parent = nullptr)
        : QObject(parent)
    {}
    virtual ~BaseDecoder()
    {}

    virtual Wisenet::Media::MediaType getMediaType() = 0;
    virtual void flushBuffers(){}
};


class BaseVideoDecoder : public QObject
{
    Q_OBJECT
public:
    explicit BaseVideoDecoder(QObject *parent, const bool lowDelay)
        : QObject(parent), m_lowDelay(lowDelay)
    {}
    virtual ~BaseVideoDecoder()
    {}

    Wisenet::Media::MediaType getMediaType()
    { return Wisenet::Media::MediaType::VIDEO; }

    void setMediaName(const QString& name) {
        m_mediaName = name.toStdString();
    }
    std::string getMethodName() {
        return videoDecodeMethodName(m_decodeMethod);
    }
    VideoDecodeMethod getMethod() {
        return m_decodeMethod;
    }

    bool setVideoOutSize(const int width, const int height,
                         const bool keepOriginalSize)
    {
        bool isChanged = false;
        if (m_outParam.outWidth != width) {
            m_outParam.outWidth = width;
            isChanged = true;
        }
        if (m_outParam.outHeight != height) {
            m_outParam.outHeight = height;
            isChanged = true;
        }
        if (m_outParam.keepOriginalSize != keepOriginalSize) {
            m_outParam.keepOriginalSize = keepOriginalSize;
            isChanged = true;
        }
        return  isChanged;
    }

    void setPixelFormat(const VideoOutPixelFormat pixelFormat)
    {
        m_outParam.pixelFormat = pixelFormat;
    }

    void setMaximumVideoOutSize(const int size)
    {
        m_outParam.maximumOutSize = size;
    }

    virtual DecodeResult openWithFFmpegCodecParam(const FFmpegCodecParamSharedPtr& codecParam) = 0;
    virtual DecodeResult decodeStart(const Wisenet::Media::VideoSourceFrameSharedPtr& videoSourceFrame,
                                     VideoOutputFrameSharedPtr& outFrame,
                                     const bool keepOriginalSize,
                                     const int outWidth, const int outHeight,
                                     const bool noDelay, std::string& errMsg) = 0;

    virtual void release() = 0;
    virtual void flushBuffers(){}

    virtual VideoOutputFrameSharedPtr getLastOriginalVideoImage() = 0;

protected:
    void startDecodeTimer()
    {
        m_startPoint = std::chrono::steady_clock::now();
    }
    bool endDecodeTimer()
    {
        auto end = std::chrono::steady_clock::now();
        int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(end - m_startPoint).count();
        if (m_maxDecodeTimeMsec < elapsedMsec)
            m_maxDecodeTimeMsec = elapsedMsec;
        m_decodeCount++;
        m_decodeTimeMsec += elapsedMsec;
        if (m_decodeCount >= m_checkFrameCount) {
            m_frameAvg = (float)m_decodeTimeMsec / m_decodeCount;
            return true;
        }
        return false;
    }
    void resetTimer()
    {
        m_decodeCount = 0;
        m_decodeTimeMsec = 0;
        m_maxDecodeTimeMsec = 0;
        m_frameAvg = 0;
    }
protected:
    std::string     m_mediaName;
    VideoOutParam   m_outParam;
    bool            m_lowDelay = false;
    // 시간측정
    std::chrono::steady_clock::time_point m_startPoint;
    size_t          m_decodeCount = 0;
    size_t          m_decodeTimeMsec = 0;
    size_t          m_maxDecodeTimeMsec = 0;
    float           m_frameAvg = 0.0f;
    int             m_checkFrameCount = 1800; // 30fps 기준 1분 (info level)
    VideoDecodeMethod m_decodeMethod = VideoDecodeMethod::SW;

};


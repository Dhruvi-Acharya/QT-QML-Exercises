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
#pragma once

#include <QObject>
#include <QtCore>
#include <memory>
#include "ThreadSafeQueue.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libavformat/avformat.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
}

struct RawVideoData
{
public:
    RawVideoData()
        : m_dataSize(0), m_dataPtr(nullptr)
    {}

    RawVideoData(const size_t dataSize, const unsigned char* dataPtr, const int64_t dts)
    {
        m_dataSize = dataSize;
        m_dataPtr = new unsigned char[m_dataSize];
        m_dts = dts;
        std::copy(dataPtr, dataPtr+dataSize, m_dataPtr);
    }

    ~RawVideoData()
    {
        if (m_dataSize > 0 && m_dataPtr != nullptr) {
            m_dataSize = 0;
            delete[] m_dataPtr;
            m_dataPtr = nullptr;
        }
    }

    size_t size()
    { return m_dataSize; }

    unsigned char* data()
    { return m_dataPtr; }

    int64_t dts()
    { return m_dts; }

private:
    int64_t m_dts;
    size_t  m_dataSize;
    unsigned char* m_dataPtr;
};
typedef std::shared_ptr<RawVideoData> RawVideoDataPtr;


struct DecodedVideoData
{
public:
    DecodedVideoData()
    {}

    DecodedVideoData(const int width, const int height, AVPixelFormat format)
        : m_width(width)
        , m_height(height)
        , m_format(format)
    {
        av_image_alloc(m_dataPtr, m_lineSize,
                       m_width, m_height, m_format, 8);
    }

    ~DecodedVideoData()
    {
        if (m_dataPtr[0] != NULL) {
            av_freep(&m_dataPtr[0]);
        }
    }

    uint8_t* data(int index)
    { return m_dataPtr[index]; }

    int lineSize(int index)
    { return m_lineSize[index]; }

    int width() {return m_width;}
    int height() {return m_height;}

    AVPixelFormat format() {return m_format;}

private:
    int             m_width = 0;
    int             m_height = 0;
    AVPixelFormat   m_format;
    uint8_t*        m_dataPtr[4] = {NULL};
    int             m_lineSize[4] = {0};
};
typedef std::shared_ptr<DecodedVideoData> DecodedVideoDataPtr;

class FFmpegCodecParamWrapper;
typedef QSharedPointer<FFmpegCodecParamWrapper> FFmpegCodecParamWrapperPtr;

class FFmpegCodecParamWrapper : public QObject
{
    Q_OBJECT
public:
    FFmpegCodecParamWrapper()
        : m_codecParam(NULL)
    {
    }

    ~FFmpegCodecParamWrapper()
    {
        if (m_codecParam != NULL) {
            avcodec_parameters_free(&m_codecParam);
            m_codecParam = NULL;
        }
    }

    void CopyParam(AVCodecParameters* srcCodecParam)
    {
        m_codecParam = avcodec_parameters_alloc();
        avcodec_parameters_copy(m_codecParam, srcCodecParam);
    }

    AVCodecParameters* getParam()
    {
        return m_codecParam;
    }

private:
    AVCodecParameters * m_codecParam;
};

class VideoDecoder : public QObject
{
    Q_OBJECT
    AVCodecContext* m_codecContext;
    AVFrame*        m_frame;
    AVPacket        m_packet;
    SwsContext*     m_swsContext;
    ThreadSafeQueue<RawVideoDataPtr>& m_decodeQueue;
    ThreadSafeQueue<DecodedVideoDataPtr>& m_renderQueue;

    AVPixelFormat   m_srcFormat = AV_PIX_FMT_NONE;
    int             m_srcWidth = 0;
    int             m_srcHeight = 0;
    AVPixelFormat   m_dstFormat = AV_PIX_FMT_NONE;
    int             m_windowWidth = 0;
    int             m_windowHeight = 0;
    int             m_dstWidth = 0;
    int             m_dstHeight = 0;

    double          m_lastDts = 0;
    bool            m_bUseYuv = false;

    size_t          m_decodeCount = 0;
    size_t          m_decodeTimeMsec = 0;

public:
    VideoDecoder(ThreadSafeQueue<RawVideoDataPtr>& decodeQueue,
                 ThreadSafeQueue<DecodedVideoDataPtr>& renderQueue,
                 bool useYuvTexture = false);
    ~VideoDecoder();

public slots:
    void initializeWithCodecContext(FFmpegCodecParamWrapperPtr contextPtr);
    void handleVideoDecode(const int param);
    void handleSetVideoOutResolution(const int width, const int height);

signals:
    void decodeResultReady(const int ret, const int expectedSleepMsec);

private:
    void Release();
};

Q_DECLARE_METATYPE(FFmpegCodecParamWrapperPtr)


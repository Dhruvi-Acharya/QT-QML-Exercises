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
#include <QSharedPointer>
#include <QAudioFormat>
#include <string>
#include <memory>
#include <cstdint>
#include "MediaSourceFrameQueue.h"
#include "Media/MediaSourceFrame.h"

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavcodec/bsf.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/frame.h"
#include "libavutil/pixdesc.h"
#include "libavutil/imgutils.h"
#include "libswresample/swresample.h"
#include "libavutil/opt.h"
#include "libavutil/avassert.h"
}


inline int Align16(int value)
{
    return (((value + 15) >> 4) << 4);
}


inline AVCodecID get_vcodec_id(Wisenet::Media::VideoCodecType vcodec)
{
    if (vcodec == Wisenet::Media::VideoCodecType::H264)
        return AV_CODEC_ID_H264;
    else if (vcodec == Wisenet::Media::VideoCodecType::H265)
        return AV_CODEC_ID_HEVC;
    else if (vcodec == Wisenet::Media::VideoCodecType::MJPEG)
        return AV_CODEC_ID_MJPEG;
    else if (vcodec == Wisenet::Media::VideoCodecType::MPEG4)
        return AV_CODEC_ID_MPEG4;
    else if (vcodec == Wisenet::Media::VideoCodecType::VP8)
        return AV_CODEC_ID_VP8;
    else if (vcodec == Wisenet::Media::VideoCodecType::VP9)
        return AV_CODEC_ID_VP9;
    else if (vcodec == Wisenet::Media::VideoCodecType::AV1)
        return AV_CODEC_ID_AV1;

    return AV_CODEC_ID_NONE;
}

inline Wisenet::Media::VideoCodecType get_wisenet_vcodec_type(AVCodecID avCodecid)
{
    if (avCodecid == AV_CODEC_ID_H264)
        return Wisenet::Media::VideoCodecType::H264;
    else if (avCodecid == AV_CODEC_ID_HEVC)
        return Wisenet::Media::VideoCodecType::H265;
    else if (avCodecid == AV_CODEC_ID_MJPEG)
        return Wisenet::Media::VideoCodecType::MJPEG;
    else if (avCodecid == AV_CODEC_ID_MPEG4)
        return Wisenet::Media::VideoCodecType::MPEG4;
    else if (avCodecid == AV_CODEC_ID_VP8)
        return Wisenet::Media::VideoCodecType::VP8;
    else if (avCodecid == AV_CODEC_ID_VP9)
        return Wisenet::Media::VideoCodecType::VP9;
    else if (avCodecid == AV_CODEC_ID_AV1)
        return Wisenet::Media::VideoCodecType::AV1;

    return Wisenet::Media::VideoCodecType::UNKNOWN;
}

inline AVCodecID get_acodec_id(Wisenet::Media::AudioCodecType acodec)
{
    if (acodec == Wisenet::Media::AudioCodecType::AAC)
        return AV_CODEC_ID_AAC;
    else if (acodec == Wisenet::Media::AudioCodecType::G711U)
        return AV_CODEC_ID_PCM_MULAW;
    else if (acodec == Wisenet::Media::AudioCodecType::G711A)
        return AV_CODEC_ID_PCM_ALAW;
    else if (acodec == Wisenet::Media::AudioCodecType::G726)
        return AV_CODEC_ID_ADPCM_G726;
    return AV_CODEC_ID_NONE;
}

inline Wisenet::Media::AudioCodecType get_wisenet_acodec_type(AVCodecID avCodecid)
{
    if (avCodecid == AV_CODEC_ID_AAC)
        return Wisenet::Media::AudioCodecType::AAC;
    else if (avCodecid == AV_CODEC_ID_PCM_MULAW)
        return Wisenet::Media::AudioCodecType::G711U;
    else if (avCodecid == AV_CODEC_ID_PCM_ALAW)
        return Wisenet::Media::AudioCodecType::G711A;
    else if (avCodecid == AV_CODEC_ID_ADPCM_G726)
        return Wisenet::Media::AudioCodecType::G726;
    return Wisenet::Media::AudioCodecType::UNKNOWN;
}


inline const std::string av_make_error_stdstring(int errnum)
{
    char errbuf[AV_ERROR_MAX_STRING_SIZE] = { 0x00, };
    av_strerror(errnum, errbuf, AV_ERROR_MAX_STRING_SIZE);
    return (std::string)errbuf;
}


inline const std::string av_make_pix_fmt_stdstring(int pix_fmt)
{
    const char* pix_name = av_get_pix_fmt_name((AVPixelFormat)pix_fmt);
    return (std::string)pix_name;
}

enum class VideoOutPixelFormat
{
    Unknown = 0,
    YUV420P,
    YUVJ420P,
    NV12,
    BGRA
};

enum class VideoDecodeMethod {
    SW,
    QSV,
    CUVID,
    VIDEOTOOLBOX
};

inline const std::string videoDecodeMethodName(const VideoDecodeMethod method)
{
    if (method == VideoDecodeMethod::SW)
        return "SW";
    else if (method == VideoDecodeMethod::QSV)
        return "QSV";
    else if (method == VideoDecodeMethod::CUVID)
        return "CUDA";
    else if (method == VideoDecodeMethod::VIDEOTOOLBOX)
        return "TBX";
    return "Unknown";
}

inline const std::string makeVideoCodecDescription(Wisenet::Media::VideoCodecType codecType,
                                                   const VideoDecodeMethod method)
{
    return Wisenet::Media::VideoCodecTypeString(codecType)+"_"+videoDecodeMethodName(method);
}

struct FFmpegCodecParam : public Wisenet::Media::MediaSourceFrameBase
{
public:
    FFmpegCodecParam(Wisenet::Media::MediaType mediaType)
        : MediaSourceFrameBase(nullptr, 0)
        , m_codecParam(nullptr)
        , m_mediaType(mediaType)
    {
    }

    ~FFmpegCodecParam()
    {
        if (m_codecParam != nullptr) {
            avcodec_parameters_free(&m_codecParam);
            m_codecParam = nullptr;
        }
    }

    void allocParam(AVCodecParameters* srcCodecParam)
    {
        m_codecParam = avcodec_parameters_alloc();
        avcodec_parameters_copy(m_codecParam, srcCodecParam);
    }

    AVCodecParameters* getParam() const
    {
        return m_codecParam;
    }

    Wisenet::Media::MediaType getMediaType() override
    {
        return m_mediaType;
    }

private:
    AVCodecParameters * m_codecParam;
    Wisenet::Media::MediaType m_mediaType = Wisenet::Media::MediaType::VCODEC_PARAM;
};
typedef std::shared_ptr<FFmpegCodecParam> FFmpegCodecParamSharedPtr;


struct MediaOutputFrameBase
{
public:
    virtual ~MediaOutputFrameBase(){}

    virtual uint8_t* data(int index) const = 0;
    virtual size_t dataSize() const = 0;
    int64_t ptsTimeMsec = 0;
};

struct MetaFrame : MediaOutputFrameBase
{
public :
    MetaFrame(){}
    MetaFrame(const std::string metaData, const size_t dataSize)
    {
        m_metaData = metaData;
        m_dataSize = dataSize;
        m_dataPtr = new uint8_t[m_dataSize];
    }
    ~MetaFrame()
    {
        if (m_dataPtr)
            delete[] m_dataPtr;
    }
    uint8_t* data(int index = 0) const override
    {
        Q_UNUSED(index);
        return m_dataPtr;
    }
    size_t dataSize() const override
    {
        return m_dataSize;
    }

    std::string metaData()
    {
        return m_metaData;
    }

private:
    std::string m_metaData = "";
    size_t      m_dataSize = 0;
    uint8_t*    m_dataPtr = nullptr;
};

struct AudioOutputFrame : MediaOutputFrameBase
{
public :
    AudioOutputFrame(){}
    AudioOutputFrame(const QAudioFormat& audioFormat, const size_t dataSize)
    {
        m_audioFormat = audioFormat;
        m_dataSize = dataSize;
        m_dataPtr = new uint8_t[m_dataSize];
    }
    ~AudioOutputFrame()
    {
        if (m_dataPtr)
            delete[] m_dataPtr;
    }
    uint8_t* data(int index = 0) const override
    {
        Q_UNUSED(index);
        return m_dataPtr;
    }
    size_t dataSize() const override
    {
        return m_dataSize;
    }
    const QAudioFormat& audioFormat() const
    {
        return m_audioFormat;
    }
    void setSourceID(const QString& sourceID) {
        m_sourceID = sourceID;
    }
    QString sourceID() const {
        return m_sourceID;
    }

private:
    QString     m_sourceID;
    QAudioFormat m_audioFormat;
    size_t      m_dataSize = 0;
    uint8_t*    m_dataPtr = nullptr;
};

struct VideoOutputFrame : MediaOutputFrameBase
{
public:
    VideoOutputFrame(){}
    VideoOutputFrame(const int videoWidth,
                     const int videoHeight,
                     AVPixelFormat pixelFormat)
        : m_width(videoWidth)
        , m_height(videoHeight)
        , m_pixelFormat(pixelFormat)
    {
        m_dataSize = av_image_alloc(m_dataPtr, m_lineSize, m_width, m_height, m_pixelFormat, 1);
        for (int i = 0; i < 4 ; i++) {
            if (m_dataPtr[i] == nullptr)
                break;
            m_planeCount++;
        }
    }

    ~VideoOutputFrame()
    {
        if (m_dataPtr[0] != nullptr) {
            av_freep(&m_dataPtr[0]);
        }
    }

    void setBufferMoved() {
        m_dataPtr[0] = nullptr;
    }

    uint8_t* data(int index) const override
    {
        if ((index < 0) || (index > 3))
            return nullptr;

        return m_dataPtr[index];
    }
    size_t dataSize() const override
    {
        return m_dataSize;
    }

    int lineSize(int index) const
    {
        return m_lineSize[index];
    }

    AVPixelFormat format() const
    {
        return m_pixelFormat;
    }

    int width() const
    {
        return m_width;
    }

    int height() const
    {
        return m_height;
    }

    int planeCount() const
    {
        return m_planeCount;
    }


private:
    uint8_t*    m_dataPtr[4] = {nullptr};
    int         m_lineSize[4] = {0};
    int         m_width = 0;
    int         m_height = 0;
    size_t      m_dataSize = 0;
    int8_t      m_planeCount = 0;
    AVPixelFormat m_pixelFormat = AVPixelFormat::AV_PIX_FMT_NONE;
};

struct AudioInputFrame
{
    explicit AudioInputFrame(const unsigned char *dataPtr = nullptr,
                    const size_t dataSize = 0)
    {
        if (dataSize > 0 && dataPtr != nullptr) {
            m_dataPtr = new unsigned char[dataSize];
            m_dataSize = dataSize;
            std::copy(dataPtr, dataPtr+dataSize, m_dataPtr);
        }
    }

    virtual ~AudioInputFrame()
    {
        if (m_dataPtr != nullptr)
            delete[] m_dataPtr;

        m_dataPtr = nullptr;
        m_dataSize = 0;
    }

    size_t getDataSize() const
    {
        return m_dataSize;
    }

    unsigned char * getDataPtr() const
    {
        return m_dataPtr;
    }

protected:
    size_t          m_dataSize = 0;
    unsigned char * m_dataPtr = nullptr;
};

typedef std::shared_ptr<MediaOutputFrameBase>   MediaOutputFrameBaseSharedPtr;
typedef std::shared_ptr<VideoOutputFrame>       VideoOutputFrameSharedPtr;
typedef std::shared_ptr<AudioOutputFrame>       AudioOutputFrameSharedPtr;
typedef std::shared_ptr<MetaFrame>              MetaFrameSharedPtr;

typedef std::shared_ptr<MediaSourceFrameQueue>   SourceFrameQueueSharedPtr;
typedef std::shared_ptr<AudioInputFrame>    AudioInputFrameSharedPtr;



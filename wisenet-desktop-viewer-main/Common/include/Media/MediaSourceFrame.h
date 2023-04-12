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

#include <memory>
#include <cstdint>
#include <string>
#include <functional>
#include <algorithm>

namespace Wisenet
{
namespace Media
{

enum class MediaSourceType
{
    UNKNOWN,
    RTP,
    FILE
};

enum class MediaType
{
    UNKNOWN,
    VIDEO,
    AUDIO,
    META_DATA,
    VCODEC_PARAM, // FFMPEG CODEC PARAMETER
    ACODEC_PARAM  // FFMPEG CODEC PARAMETER
};

enum class VideoCodecType
{
    UNKNOWN = 0,
    H265,
    H264,
    MJPEG,
    MPEG4,
    VP8,
    VP9,
    AV1
};

inline std::string VideoCodecTypeString(VideoCodecType vcodec)
{
    if (vcodec == VideoCodecType::H264)
        return "H.264";
    else if (vcodec == VideoCodecType::H265)
        return "H.265";
    else if (vcodec == VideoCodecType::MJPEG)
        return "MJPEG";
    else if (vcodec == VideoCodecType::MPEG4)
        return "MPEG4";
    else if (vcodec == VideoCodecType::VP8)
        return "VP8";
    else if (vcodec == VideoCodecType::VP9)
        return "VP9";
    else if (vcodec == VideoCodecType::AV1)
        return "AV1";
    return "UNKNOWN";
}

enum class VideoFrameType
{
    UNKNOWN = 0,
    I_FRAME,
    P_FRAME
};


enum class AudioCodecType
{
    UNKNOWN = 0,
    G711U,
    G711A,
    G723,
    G726,
    AAC
};

inline std::string AudioCodecTypeString(AudioCodecType acodec)
{
    if (acodec == AudioCodecType::G711U)
        return "G.711U";
    else if (acodec == AudioCodecType::G711A)
        return "G.711A";
    else if (acodec == AudioCodecType::G723)
        return "G.723";
    else if (acodec == AudioCodecType::G726)
        return "G.726";
    else if (acodec == AudioCodecType::AAC)
        return "AAC";
    return "UNKNOWN";
}


enum class MetaDataCodecType
{
    UNKNOWN = 0,
    XML,
    JPEG,
    TEXT
};


struct MediaTime
{
    // presentation timestamp
    // the number of milliseconds since
    // 1970-01-01T00:00:00 Universal Coordinated Time
    int64_t     ptsTimestampMsec = 0;

    // if RTP, rtp timestamp / clock rate / 1000
    uint32_t    rtpTimestampMsec = 0;
};

struct MediaSourceFrameBase
{
    MediaSourceType sourceType = MediaSourceType::UNKNOWN;
    MediaTime       frameTime;          // frame time
    unsigned int    frameSequence = 0;  // continous increment while continous media
    unsigned char   commandSeq = 0;     // e.g. RTP : CSeq and RTP extension when playback

    explicit MediaSourceFrameBase(const unsigned char *dataPtr = nullptr,
                    const size_t dataSize = 0)
    {
        if (dataSize > 0 && dataPtr != nullptr) {
            m_dataPtr = new unsigned char[dataSize];
            m_dataSize = dataSize;
            std::copy(dataPtr, dataPtr+dataSize, m_dataPtr);
        }
    }

    virtual ~MediaSourceFrameBase()
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

    virtual MediaType getMediaType() = 0;

protected:
    size_t          m_dataSize = 0;
    unsigned char * m_dataPtr = nullptr;
};


struct VideoSourceFrame : MediaSourceFrameBase
{
    VideoFrameType videoFrameType = VideoFrameType::UNKNOWN;
    VideoCodecType videoCodecType = VideoCodecType::UNKNOWN;
    int videoWidth = 0;     // video resolution width
    int videoHeight = 0;    // video resolution height
    float frameRate = 0;    // frame rate (30.0f)
    int64_t dtsTimestampMsec = -1; // special field for video file when pts does not same to dts
    bool immediate = false; // ignore dtsTimestampMsec, decode immediately

    explicit VideoSourceFrame(const unsigned char *dataPtr = nullptr,
                         const size_t dataSize = 0)
        : MediaSourceFrameBase(dataPtr, dataSize)
    {}

    MediaType getMediaType() override
    {
        return MediaType::VIDEO;
    }
};

struct AudioSourceFrame : MediaSourceFrameBase
{
    AudioCodecType audioCodecType = AudioCodecType::UNKNOWN;
    int audioChannels = 0;      // number of audio channels (1:mono, 2:stereo)
    int audioSampleRate = 0;   // Samples per seconds (Hz: 16000, 32000, 44100...)
    int audioBitPerSample = 0;  // Bit per Sample (bits)
    int audioBitrate = 0;       // Bitrate (bits)

    explicit AudioSourceFrame(const unsigned char *dataPtr = nullptr,
                         const size_t dataSize = 0)
        : MediaSourceFrameBase(dataPtr, dataSize)
    {}

    MediaType getMediaType() override
    {
        return MediaType::AUDIO;
    }
};

struct MetadataSourceFrame : MediaSourceFrameBase
{
    MetaDataCodecType metaDataCodecType = MetaDataCodecType::UNKNOWN;
    uint32_t refID = 0;

    explicit MetadataSourceFrame(const unsigned char *dataPtr = nullptr,
                         const size_t dataSize = 0)
        : MediaSourceFrameBase(dataPtr, dataSize)
    {}

    MediaType getMediaType() override
    {
        return MediaType::META_DATA;
    }
};


typedef std::shared_ptr<MediaSourceFrameBase>    MediaSourceFrameBaseSharedPtr;

typedef std::shared_ptr<VideoSourceFrame>        VideoSourceFrameSharedPtr;
typedef std::shared_ptr<AudioSourceFrame>        AudioSourceFrameSharedPtr;
typedef std::shared_ptr<MetadataSourceFrame>     MetadataSourceFrameSharedPtr;


typedef std::function <void(const MediaSourceFrameBaseSharedPtr&)> MediaSourceFrameHandler;

/* media frame handler example */
/*
void frameHandler(const MediaSourceFrameBaseSharedPtr& frame)
{
    auto mediaType = frame->getMediaType();
    if (mediaType  == MediaType::VIDEO) {
        auto videoFrame = std::static_pointer_cast<VideoSourceFrame>(frame);
    }
    else if (mediaType == MediaType::AUDIO) {
        auto audioFrame = std::static_pointer_cast<AudioSourceFrame>(frame);
    }
    else if (mediaType  == MediaType::META_DATA) {
        auto metaFrame = std::static_pointer_cast<MetadataSourceFrame>(frame);
    }
    else {
        // invalid source Type
    }
}
*/

}
}

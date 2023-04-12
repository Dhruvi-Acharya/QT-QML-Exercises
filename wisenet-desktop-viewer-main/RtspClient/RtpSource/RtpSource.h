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
#include <atomic>
#include <chrono>

#include "SdpParser.h"
#include "Media/MediaSourceFrame.h"
#include "RtspRequestResponse.h"
#include "SecurityUtil.h"
#include <cstring>

namespace Wisenet
{
namespace Rtsp
{

extern bool seqNumLT(uint16_t s1, uint16_t s2);

/*
struct MediaSessionCallbacks
{
    MediaSessionCallbacks()
        : VideoCallback(nullptr)
        , AudioCallback(nullptr)
        , MetaCallback(nullptr)
        , MetaImageCallback(nullptr)
    {}
    oscar::media::PushVideoFunctor			VideoCallback;
    oscar::media::PushAudioFunctor			AudioCallback;
    oscar::media::PushOnvifMetaFunctor		MetaCallback;
    oscar::media::PushMetaImageFunctor		MetaImageCallback;
};
*/

struct MediaFrame
{
    MediaFrame(const size_t frameSize)
    {
        m_bufferSize = frameSize;
        m_buffer = new unsigned char[frameSize];

        m_offset = m_buffer;
        m_currentSize = 0;
    }

    void AddData(const unsigned char *dataPtr, const size_t dataSize)
    {
        // 버퍼크기를 초과한 경우
        if ((dataSize + m_currentSize) >= m_bufferSize)
        {
            m_bufferSize = (dataSize + m_currentSize) * 2;
            unsigned char *tmp = new unsigned char[m_bufferSize];

            std::memcpy(tmp, m_buffer, m_currentSize);
            delete[] m_buffer;
            m_buffer = tmp;
            m_offset = m_buffer + m_currentSize;
        }

        std::memcpy(m_offset, dataPtr, dataSize);
        m_offset += dataSize;
        m_currentSize += dataSize;
    }

    size_t FrameSize()
    {
        return m_currentSize;
    }

    unsigned char* Ptr()
    {
        return m_buffer;
    }

    void Reset()
    {
        m_offset = m_buffer;
        m_currentSize = 0;
    }

    ~MediaFrame()
    {
        if (m_buffer)
            delete[] m_buffer;
    }

private:
    unsigned char *m_buffer;
    size_t m_bufferSize;

    unsigned char *m_offset;
    size_t m_currentSize;
};

typedef std::shared_ptr<MediaFrame> MediaFramePtr;

struct RtcpInfo
{
    RtcpInfo()
        : IsSet(false)
        , NTP(0)
        , RtpTimeStamp(0)
    {}

    bool IsSet;
    int64_t NTP;
    unsigned int RtpTimeStamp;
};


class RtpSource : public std::enable_shared_from_this<RtpSource>
{
public:
    RtpSource();
    virtual ~RtpSource();

    void Initialize(
            const std::string &url,
            const RtspPlayControlType playControlType,
            Media::MediaSourceFrameHandler& callback,
            const size_t expectedFrameSize);

    void Close();

    virtual void Setup(MediaStreamInfo &streamInfo);

    void ReadData(const unsigned char* bufPtr, size_t byteTransferred);

    void SetCurrentTimveVal(const int64_t& t);

    void SetSynchronizedUsingRTCP(const int64_t& ntp,
                                  const unsigned int rtpTimeStamp);

    bool HasBeenSynchronizedUsingRTCP()
    {
        return m_rtcp.IsSet;
    }
    void setStartTimePoint(const std::chrono::steady_clock::time_point& rtspOpenTimePoint)
    {
        m_startPoint = rtspOpenTimePoint;
    }

protected:
    unsigned int m_payloadFormat;
    unsigned int m_clockRate;

    std::string m_codecName;
    std::string m_url;
    RtspPlayControlType m_playControlType;

    uint32_t m_currentSSRC;
    uint32_t m_currentRtpTimeStamp;
    uint16_t m_currentSeq;

    unsigned char *m_currentPacketPtr;
    size_t m_currentPacketSize;
    size_t m_currentExtensionSize;
    size_t m_currentExtensionOffset;

    bool m_checkExtensionHeader;
    bool m_markerBit;
    bool m_currentPacketBeginFrame;
    bool m_currentPacketCompleteFrame;

    MediaFramePtr m_mediaSourceDataPtr;
    Media::MediaSourceFrameHandler m_mediaCallbacks;
    std::atomic<bool>		m_isClosed;
    unsigned char			m_RtpExtensionSeq;
    char					m_lastRtpExtensionSeq;
    int64_t                 m_currentTimeVal;
    uint32_t				m_currentTimeStamp;
    uint32_t				m_lastTimeStamp;
    uint32_t				m_currentFrameSequence;

    RtcpInfo				m_rtcp;
    std::chrono::steady_clock::time_point m_startPoint;

    enum { kRtpPlaybackExtHeaderSize = 12 };
private:
    virtual bool ProcessSpecialHeader(unsigned char *dataPtr, size_t rtpDataSize, size_t &retSkipBytes);

    // 1프레임이 완성되면 true를 그렇지 않으면 false를 리턴한다.
    virtual bool AddData(unsigned char *dataPtr, size_t dataSize, size_t &remainBytes);

    // 어플리케이션 콜백을 호출하여 데이터를 전달한다.
    // 콜백 호출은 데이터타입(비디오/오디오/메타)에 따라서 다르게 호출되어야 한다.
    virtual void CallbackData() = 0;


};


typedef std::shared_ptr<RtpSource> RtpSourcePtr;

}
}

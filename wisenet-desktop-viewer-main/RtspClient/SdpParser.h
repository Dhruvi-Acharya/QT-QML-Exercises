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

#include "string"
#include "map"
#include "list"

namespace Wisenet
{
namespace Rtsp
{

struct MediaStreamInfo
{
    MediaStreamInfo()
        : Bandwidth(0)
        , Port(0)
        , FrameRate(0)
        , VideoWidth(0), VideoHeight(0)
        , IsRtcpMux(false)
        , PayloadFormat(0)
        , ClockRate(0)
        , NumChannels(1)
        , SendOnly(false)
        , MetaImageSession(false)
        , HasTransport(false)
        , InterleavedRtpChannel(0)
        , InterleavedRtcpChannel(0)
    {}
    bool		SendOnly;
    std::string Media;
    std::string ProtocolName;
    std::string RangeStr;
    std::string CodecName;
    std::string Control;
    unsigned int PayloadFormat;
    unsigned int ClockRate;
    unsigned int NumChannels;
    unsigned short Port;
    unsigned int Bandwidth;
    float FrameRate;
    unsigned short VideoWidth;
    unsigned short VideoHeight;
    bool IsRtcpMux;
    bool MetaImageSession;
    bool HasTransport;
    unsigned int InterleavedRtpChannel;
    unsigned int InterleavedRtcpChannel;
    std::map<std::string, std::string> FormatParams;
};

class SdpParser
{
public:
    SdpParser();
    ~SdpParser();

    void Clear()
    {
        m_sessionName = "";
        m_sessionInformation = "";
        m_control = "";
        m_rangeStr = "";
        m_streams.clear();
    }

    bool Parse(std::string& sdp);
    const std::list<MediaStreamInfo>& Streams()
    {
        return m_streams;
    }

private:
    std::string m_sessionName;
    std::string m_sessionInformation;
    std::string m_connectionInformation;
    std::string m_control;
    std::string m_rangeStr;

    std::list<MediaStreamInfo> m_streams;
};

}
}

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

#include <string>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/unordered_map.hpp>
#include "LogSettings.h"
#include "SdpParser.h"

namespace Wisenet
{
namespace Rtsp
{

SdpParser::SdpParser()
{

}

SdpParser::~SdpParser()
{

}

bool SdpParser::Parse(std::string& result)
{
    Clear();
    std::string line;
    std::istringstream stream(result);

    while (std::getline(stream, line) && line != "\r")
    {
        boost::trim(line);
        if (boost::starts_with(line, "s="))
        {
            m_sessionName = line.substr(2);
        }
        else if (boost::starts_with(line, "i="))
        {
            m_sessionInformation = line.substr(2);
        }
        else if (boost::starts_with(line, "c=")) // connection information
        {
            m_connectionInformation = line.substr(2);

            // TODO : 추후 필요시 구현...현 사양에서는 MULTICAST 자동 확인 루틴 필요하지 않음.
            // Check for "c=IN IP4 <connection-endpoint>"
            // or "c=IN IP4 <connection-endpoint>/<ttl+numAddresses>"
            // (Later, do something with <ttl+numAddresses> also #####)
            /*
            5.2.1.1.4 Multicast streaming
            A device shall include a valid multicast address in the "c=" field of a DESCRIBE response
            according to RFC 4566.
            Remark: the optional dynamic multicast address assignment exception described in appendix
            C.1.7 of RFC 2326 allowing 0.0.0.0 addresses does not apply.

            Chapter 10.7 TEARDOWN of [RFC 2326] states that a device shall stop the stream delivery
            for the given URI on tear down. This needs to be clarified in case of multicast: for a multicast
            stream the device shall stop sending packets for a multicast configuration when no more
            RTSP sessions are using the same multicast configuration nor its AutoStart flag has been set.
            */
        }
        else if (boost::starts_with(line, "t=")) // type
        {
            // 필요시 구현..
        }
        else if (boost::starts_with(line, "a=control:"))
        {
            m_control = line.substr(10);
        }
        else if (boost::starts_with(line, "a=range:npt="))
        {
            m_rangeStr = line.substr(12);
        }
        else if (boost::starts_with(line, "a=range:clock="))
        {
            m_rangeStr = line.substr(14);
        }

        // TODO :: 리팩토링... 코드가 너무 길어...
        else if (boost::starts_with(line, "m="))
        {
            bool found_m = true;
            while (found_m)
            {
                std::vector<std::string> tokens;
                boost::split(tokens, line.substr(2), boost::is_any_of(" "), boost::token_compress_on);
                if (tokens.size() != 4)
                {
                    SPDLOG_DEBUG("Invalid media stream info : {}", line);
                    return false;
                }
                //m=<media> <port> <proto> <fmt>
                MediaStreamInfo streamInfo;
                streamInfo.Media = tokens[0];

                //"m=%s %hu/%*u RTP/AVP %u"

                unsigned int localPort = 0;
                if (boost::contains(tokens[1], "/"))
                {
                    std::vector<std::string> pts;
                    boost::split(pts, tokens[1], boost::is_any_of("/"), boost::token_compress_on);
                    boost::trim(pts[0]);
                    boost::conversion::try_lexical_convert<unsigned int>(pts[0], localPort);
                }
                else
                {
                    boost::trim(tokens[1]);
                    boost::conversion::try_lexical_convert<unsigned int>(tokens[1], localPort);
                }

                streamInfo.Port = localPort;

                streamInfo.ProtocolName = tokens[2];
                boost::trim(tokens[3]);
                boost::conversion::try_lexical_convert<unsigned int>(tokens[3], streamInfo.PayloadFormat);
                found_m = false;

                while (std::getline(stream, line) && line != "\r")
                {
                    boost::trim(line);
                    if (boost::starts_with(line, "m="))
                    {
                        // 다음 미디어 정보 읽기
                        m_streams.push_back(streamInfo);
                        found_m = true;
                        break;
                    }
                    else if (boost::starts_with(line, "b=AS:"))
                    {
                        auto tline = line.substr(5);
                        boost::trim(tline);
                        boost::conversion::try_lexical_convert<unsigned int>(tline, streamInfo.Bandwidth);
                    }
                    else if (boost::starts_with(line, "a=rtpmap:"))
                    {
                        // "a=rtpmap:<fmt> <codec>/<freq>"
                        std::stringstream in(line.substr(9));
                        std::string payload, next;
                        if (in >> payload)
                        {
                            unsigned int payloadFmt;
                            boost::trim(payload);
                            boost::conversion::try_lexical_convert<unsigned int>(payload, payloadFmt);
                            if (payloadFmt == streamInfo.PayloadFormat)
                            {
                                if (in >> next)
                                {
                                    std::istringstream is(next);
                                    std::string token;
                                    size_t tokCount = 0;
                                    while (std::getline(is, token, '/'))
                                    {
                                        boost::trim(token);
                                        ++tokCount;
                                        if (tokCount == 1)
                                        {
                                            streamInfo.CodecName = boost::to_upper_copy(token);
                                        }
                                        else if (tokCount == 2)
                                        {
                                            boost::conversion::try_lexical_convert<unsigned int>(token, streamInfo.ClockRate);
                                        }
                                        else if (tokCount == 3)
                                        {
                                            boost::conversion::try_lexical_convert<unsigned int>(token, streamInfo.NumChannels);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (boost::equals(line, "a=rtcp-mux"))
                    {
                        streamInfo.IsRtcpMux = true;
                    }
                    else if (boost::equals(line, "a=sendonly"))
                    {
                        streamInfo.SendOnly = true;
                    }
                    else if (boost::starts_with(line, "a=fmtp:"))
                    {
                        std::stringstream in(line.substr(7));
                        std::string payload, next;
                        if (in >> payload)
                        {
                            unsigned int payloadFmt;
                            boost::trim(payload);
                            boost::conversion::try_lexical_convert<unsigned int>(payload, payloadFmt);
                            if (payloadFmt == streamInfo.PayloadFormat)
                            {
                                if (std::getline(in, next))
                                    //if (in >> next)
                                {
                                    std::istringstream is(next);
                                    std::string token;
                                    while (std::getline(is, token, ';'))
                                    {
                                        const auto eq_idx = token.find_first_of('=');
                                        if (std::string::npos != eq_idx)
                                        {
                                            std::string key = token.substr(0, eq_idx);
                                            std::string value = token.substr(eq_idx + 1);
                                            boost::trim(key);
                                            boost::trim(value);
                                            boost::to_lower(key);
                                            //boost::to_lower(value);
                                            streamInfo.FormatParams[key] = value;
                                        }
                                        else
                                        {
                                            boost::trim(token);
                                            boost::to_lower(token);
                                            streamInfo.FormatParams[token] = "";
                                        }
                                    }
                                }
                            }
                        }
                    }
                    else if (boost::starts_with(line, "a=control:"))
                    {
                        streamInfo.Control = line.substr(10);
                    }
                    else if (boost::starts_with(line, "a=range:npt="))
                    {
                        streamInfo.RangeStr = line.substr(12);
                    }
                    else if (boost::starts_with(line, "a=range:clock="))
                    {
                        streamInfo.RangeStr = line.substr(14);
                    }
                    else if (boost::starts_with(line, "a=framerate:"))
                    {
                        auto tline = line.substr(12);
                        boost::trim(tline);
                        boost::conversion::try_lexical_convert<float>(tline, streamInfo.FrameRate);
                    }
                    else if (boost::starts_with(line, "a=x-framerate:"))
                    {
                        auto tline = line.substr(14);
                        boost::trim(tline);
                        boost::conversion::try_lexical_convert<float>(tline, streamInfo.FrameRate);
                    }
                    else if (boost::starts_with(line, "a=framesize:"))
                    {
                        std::stringstream in(line.substr(12));
                        std::string payload, wh;
                        if (in >> payload)
                        {
                            if (in >> wh)
                            {
                                std::istringstream is(wh);
                                std::string width, height;
                                if (std::getline(is, width, '-'))
                                {
                                    if (std::getline(is, height, '-'))
                                    {
                                        boost::trim(width);
                                        boost::trim(height);
                                        boost::conversion::try_lexical_convert<unsigned short>(width, streamInfo.VideoWidth);
                                        boost::conversion::try_lexical_convert<unsigned short>(height, streamInfo.VideoHeight);
                                    }
                                }
                            }
                        }
                    }
                    // only valid when QUICKPLAY
                    else if (boost::starts_with(line, "a=transport:"))
                    {
                        streamInfo.HasTransport = true;
                        std::istringstream is(line.substr(12));
                        std::string token;
                        while (std::getline(is, token, ';')) {
                            if (boost::starts_with(token, "interleaved=")) {
                                std::string interleaved = token.substr(12);
                                std::istringstream isInterleaved(interleaved);
                                std::string chStr;

                                if (std::getline(isInterleaved, chStr, '-')) {
                                    boost::trim(chStr);
                                    boost::conversion::try_lexical_convert<unsigned int>(chStr, streamInfo.InterleavedRtpChannel);
                                    if (std::getline(isInterleaved, chStr, '-')) {
                                        boost::trim(chStr);
                                        boost::conversion::try_lexical_convert<unsigned int>(chStr, streamInfo.InterleavedRtcpChannel);

                                    }
                                }
                                SPDLOG_DEBUG("QUICKPLAY media={}, control={}, interleaved={}, RTP/{}-RTCP/{}",
                                             streamInfo.Media,
                                             streamInfo.Control,
                                             token,
                                             streamInfo.InterleavedRtpChannel, streamInfo.InterleavedRtcpChannel);
                            }
                        }
                    }
                    else if (boost::starts_with(line, "a=x-dimensions:"))
                    {
                        std::istringstream is(line.substr(15));
                        std::string width, height;
                        if (std::getline(is, width, ','))
                        {
                            if (std::getline(is, height, ','))
                            {
                                boost::trim(width);
                                boost::trim(height);
                                boost::conversion::try_lexical_convert<unsigned short>(width, streamInfo.VideoWidth);
                                boost::conversion::try_lexical_convert<unsigned short>(height, streamInfo.VideoHeight);
                            }
                        }
                    }
                    else if (boost::starts_with(line, "i=MetaImageSession"))
                    {
                        streamInfo.MetaImageSession = true;
                    }
                }
                if (!found_m)
                    m_streams.push_back(streamInfo);
            }

        }

    }
    return true;
}


}
}

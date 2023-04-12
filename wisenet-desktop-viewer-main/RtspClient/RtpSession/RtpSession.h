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
#include <string>
#include <boost/asio.hpp>

#include "Media/MediaSourceFrame.h"
#include "RtspRequestResponse.h"
#include "RtpSession/BackChannelRtpSink.h"
#include "RtpUdpSocket.h"
#include "RtpSource/RtpSource.h"


namespace Wisenet
{
namespace Rtsp
{

class RtpSession : public std::enable_shared_from_this<RtpSession>
{
public:
    RtpSession(boost::asio::io_context& ioc);
    ~RtpSession();

    void Initialize(std::string &url,
                    RtspPlayControlType playControlType,
                    MediaStreamInfo streamInfo,
                    Media::MediaSourceFrameHandler& callback);
    void Close();

    // Call after receive DESCRIBE response
    // rtcp always rtp + 1 or same
    void CreateUDPSocket(
        const bool isRtcpMux,
        const unsigned short recommandClientPort);

    void SetupUDPSocket(
        const bool isMulticast,
        const bool isRtcpMux,
        const std::string &serverAddress,
        const unsigned short rtpClientPort,
        const unsigned short rtcpClientPort,
        const unsigned short rtpServerPort,
        const unsigned short rtcpServerPort
        );


    // Call after receive SETUP response
    void SetSessionID(const std::string& sessionID)
    {
        m_sessionID = sessionID;
    }

    void SetupTcpInterleaved(const unsigned int rtpChannel,
                             const unsigned int rtcpChannel);

    unsigned int RtpChannel() { return m_rtpChannel; }
    unsigned int RtcpChannel() { return m_rtcpChannel; }
    unsigned short RtpPort()
    {
        if (m_rtpSocket)
            return m_rtpSocket->GetPortNumber();
        return 0;
    }
    unsigned short RtcpPort()
    {
        if (m_rtcpSocket)
            return m_rtcpSocket->GetPortNumber();
        return 0;
    }

    std::string SessionID() { return m_sessionID; }

    void SetupUdpDestination(const std::string &ipAddress,
                             const unsigned short rtpServerPort,
                             const unsigned short rtcpServerPort);


    bool CreateRtpSource(const std::chrono::steady_clock::time_point& rtspOpenTimePoint);
    bool CreateRtpSink();


    // todo : set rtcp bye handler..
    // for test
    void StartPlaying();

    void ReadRawRtpData(const unsigned char* buff, size_t byteTransferred);
    void ReadRawRtcpData(const unsigned char* buff, size_t byteTransferred);

    const MediaStreamInfo& StreamInfo()
    {
        return m_streamInfo;
    }

    BackChannelRtpSinkPtr& RtpSink()
    {
        return m_rtpSink;
    }

    RtpUdpSocketPtr& RtpSocket()
    {
        return m_rtpSocket;
    }
    void SetStartTimeWithPlayResponse(std::string& dateStr);

    const std::string& Url() { return m_url; }
    const bool IsTcpInterleaved() { return m_isTcpInterleaved; }

private:
    boost::asio::io_context&        m_ioContext;
    Media::MediaSourceFrameHandler  m_mediaCallbacks;

    std::string		m_url;
    std::string		m_sessionID;
    RtspPlayControlType m_playControlType;

    bool			m_isTcpInterleaved;
    bool			m_isRtcpMux;
    bool			m_isMulticast;
    unsigned int	m_rtpChannel;
    unsigned int	m_rtcpChannel;

    RtpUdpSocketPtr	m_rtpSocket;
    RtpUdpSocketPtr m_rtcpSocket;

    RtpSourcePtr	m_rtpSource;
    MediaStreamInfo m_streamInfo;

    void ProcessSenderReport(unsigned int SSRC,
        unsigned int NTPmsw, unsigned int NTPlsw,
        unsigned int rtpTimestamp);

    BackChannelRtpSinkPtr m_rtpSink;
};


typedef std::shared_ptr<RtpSession> RtpSessionPtr;

}
}

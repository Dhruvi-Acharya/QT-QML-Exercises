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
#include <queue>
#include <boost/asio.hpp>


namespace Wisenet
{
namespace Rtsp
{


class RtpSession;
typedef std::shared_ptr<boost::asio::ip::udp::socket> UdpSocketPtr;

struct ReorderingBuffer
{
    ReorderingBuffer()
        : Seq(0), Buffer(nullptr), BufferSize(0)
    {
    }
    ReorderingBuffer(uint16_t seq, const unsigned char* buffer, size_t bufferSize)
        : Seq(seq)
    {
        Buffer = new unsigned char[bufferSize];
        BufferSize = bufferSize;
        std::memcpy(Buffer, buffer, bufferSize);
    }

    ~ReorderingBuffer()
    {
        if (Buffer)
            delete[] Buffer;
    }

    uint16_t		Seq;
    unsigned char *	Buffer;
    size_t			BufferSize;
};

typedef std::shared_ptr<ReorderingBuffer>ReorderingBufferPtr;

struct ReorderingComp
{
    bool operator()(const ReorderingBufferPtr& lhs, const ReorderingBufferPtr& rhs)
    {
        int diff = lhs->Seq - rhs->Seq;
        if (diff > 0)
        {
            return (diff < 0x8000);
        }
        else if (diff < 0)
        {
            return (diff < -0x8000);
        }
        else
        {
            return false;
        }
    }
};

struct _AsioStreamBuf
{
    _AsioStreamBuf();
    ~_AsioStreamBuf();
    boost::asio::streambuf Buffer;
};
typedef std::shared_ptr<_AsioStreamBuf> _AsioStreamBufPtr;

class RtpUdpSocket : public std::enable_shared_from_this<RtpUdpSocket>
{
public:
    RtpUdpSocket(RtpSession& rtpSession, boost::asio::io_context& ioc, const unsigned short portNo=0);

    void Close();
    void MakeBindSocket(const unsigned short portNo);
    unsigned short GetPortNumber()const
    {
        return m_socket->local_endpoint().port();
    }

    int SetReceiveBufferSize(const int bufferSize)
    {
        // todo
        m_socket->set_option(boost::asio::socket_base::receive_buffer_size(bufferSize));
        return GetReceiveBufferSize();
    }

    int GetReceiveBufferSize() const
    {
        boost::asio::socket_base::receive_buffer_size option;
        m_socket->get_option(option);
        return option.value();
    }

    void SetRtcp()
    {
        m_isRtcp = true;
    }
    void SetRtcpMux()
    {
        m_isRtcpMux = true;
    }
    void SetDestination(const std::string &serverAddress,
        const unsigned short serverPort,
        const bool isMulticast);

    void StartPlaying();

    void WriteBackChannelData(unsigned char*data, size_t dataSize);
    void HandleAsyncWriteBackChannelData(const boost::system::error_code& error,
        size_t byteTransferred);

    virtual ~RtpUdpSocket();

private:
    enum { kMaxPacketSize = 65535 };
    enum { kReceiveBufferSize = 1024000};
    enum { kRtpHeaderSize = 12 };
    void HandleReadPacket(const boost::system::error_code& error, size_t byteTransferred);
    void continuePlaying();
    void BufferingRtpData(const unsigned char* buff, size_t byteTransferred);
    void startBufferingTimer();
    void resetBuffering();

protected:
    bool m_isRtcp;
    bool m_isRtcpMux;
    std::string m_serverAddress;
    unsigned short m_serverPort;
    bool m_isMulticast;

    boost::asio::io_context& m_ioContext;
    boost::asio::strand<boost::asio::io_context::executor_type>    m_strand;

    UdpSocketPtr m_socket;
    boost::asio::ip::udp::endpoint m_senderEndpoint;
    boost::asio::ip::udp::endpoint m_receiverEndpoint;

    RtpSession& m_rtpSession;

    // buffering udp rtp packets
    boost::asio::steady_timer               m_bufferingTimer;
    unsigned int                            m_bufferingTimeMs;
    bool                                    m_bufferingCompleted;
    std::chrono::steady_clock::time_point   m_bufferingStartClock;
    std::chrono::steady_clock::time_point   m_bufferingLastClock;
    _AsioStreamBufPtr                       m_recvPackets;
    std::priority_queue<ReorderingBufferPtr, std::vector<ReorderingBufferPtr>, ReorderingComp>	m_bufferingQueue;
};

typedef std::shared_ptr<RtpUdpSocket> RtpUdpSocketPtr;


}
}

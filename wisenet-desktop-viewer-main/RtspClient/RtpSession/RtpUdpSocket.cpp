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

#include <boost/bind.hpp>
#include <boost/asio/detail/socket_ops.hpp>
#include <boost/lexical_cast.hpp>

#include "LogSettings.h"
#include "WeakCallback.h"
#include "RtpUdpSocket.h"
#include "RtpSession.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTP-UDP] "}, level, __VA_ARGS__)


namespace Wisenet
{
namespace Rtsp
{

using boost::asio::ip::udp;
using namespace boost::asio::detail::socket_ops;

void RtpUdpSocket::MakeBindSocket(const unsigned short portNo)
{
    m_socket = std::make_shared<udp::socket>(m_ioContext);
    boost::asio::ip::udp::endpoint listen_endpoint(udp::v4(), portNo);
    try {
        m_socket->open(listen_endpoint.protocol());
        m_socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
        m_socket->bind(listen_endpoint);
        if (portNo == 0)
            SPDLOG_WARN("RtpUdpSocket::MakeBindSocket() new port port:{} end", m_socket->local_endpoint().port());
    }
    catch (std::exception) {
        SPDLOG_WARN("RtpUdpSocket::MakeBindSocket() exception2...use anoymous port:{}", portNo);
        MakeBindSocket(0);
    }
}

RtpUdpSocket::RtpUdpSocket(RtpSession& rtpSession,
                           boost::asio::io_context& ioc,
                           const unsigned short portNo)
    : m_ioContext(ioc)
    , m_strand(ioc.get_executor())
    , m_isMulticast(false)
    , m_serverPort(0)
    , m_rtpSession(rtpSession)
    , m_isRtcp(false)
    , m_isRtcpMux(false)
    , m_bufferingTimer(ioc)
    , m_bufferingTimeMs(100)
    , m_bufferingCompleted(false)
    , m_recvPackets(std::make_shared<_AsioStreamBuf>())
{
    m_recvPackets->Buffer.prepare(kMaxPacketSize);

    if (portNo != 0) {
        MakeBindSocket(portNo);
    }
    else {
        m_socket = std::make_shared<udp::socket>(m_ioContext, udp::endpoint(udp::v4(), 0));
        //auto tmp = m_socket->local_endpoint().port();
        m_socket->set_option(boost::asio::ip::udp::socket::reuse_address(true));
    }

    // buffer size ->1MB
    boost::asio::socket_base::receive_buffer_size sockBuffSize(kReceiveBufferSize);
    boost::system::error_code ec;
    m_socket->set_option(boost::asio::socket_base::receive_buffer_size(sockBuffSize), ec);
    if (!ec) {
        m_socket->get_option(sockBuffSize, ec);
        SPDLOG_DEBUG("RtpUdpSocket::RtpUdpSocket(), bufferSize={}", sockBuffSize.value());
    }
    
    SPDLOG_DEBUG("RtpUdpSocket::RtpUdpSocket()");
}

RtpUdpSocket::~RtpUdpSocket()
{
    SPDLOG_DEBUG("RtpUdpSocket::~RtpUdpSocket()");
}

void RtpUdpSocket::Close()
{
    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(),
                                                 [this]()
    {
        if (m_socket) {
            boost::system::error_code ec;
            m_socket->cancel(ec);
            m_socket->shutdown(boost::asio::socket_base::shutdown_both, ec);
            m_socket->close(ec);
        }
        m_bufferingTimer.cancel();
    }));
}

void RtpUdpSocket::SetDestination(const std::string &serverAddress,
                                  const unsigned short serverPort,
                                  const bool isMulticast)
{
    m_serverAddress = serverAddress;
    m_serverPort = serverPort;
    m_isMulticast = isMulticast;

    if (isMulticast)
    {
        boost::asio::ip::address multicastAddress(
                    boost::asio::ip::address::from_string(serverAddress));
        // Join the multicast group.
        m_socket->set_option(
                    boost::asio::ip::multicast::join_group(multicastAddress));
    }
    try
    {
        udp::resolver resolver(m_ioContext);
        std::string strPort;

        if (boost::conversion::try_lexical_convert<std::string>(m_serverPort, strPort) != false)
        {
            udp::resolver::query query(udp::v4(), m_serverAddress, strPort);
            m_receiverEndpoint = *resolver.resolve(query);
        }
    }
    catch (std::exception)
    {
        SPDLOG_ERROR("can not resolve server ip::{}, port::{}", m_serverAddress, m_serverPort);
    }
}

void RtpUdpSocket::StartPlaying()
{
    resetBuffering();
    startBufferingTimer();
    continuePlaying();
}

void RtpUdpSocket::continuePlaying()
{
    auto streamPtr = m_recvPackets;
    m_socket->async_receive_from(m_recvPackets->Buffer.prepare(kMaxPacketSize),
                                 m_senderEndpoint,
                                 WeakCallback(shared_from_this(),
                                              [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand,
                          WeakCallback(shared_from_this(),
                                       [this, streamPtr, error, byteTransferred]()
        {
            //SPDLOG_INFO("RTP PACKET ARRIVE={}, RTCP={}", byteTransferred, m_isRtcp);
            HandleReadPacket(error, byteTransferred);
        }));
    }));
}

void Wisenet::Rtsp::RtpUdpSocket::resetBuffering()
{
    m_bufferingStartClock = std::chrono::steady_clock::now();
    m_bufferingLastClock = m_bufferingStartClock;
    m_bufferingCompleted = false;
}

void RtpUdpSocket::startBufferingTimer()
{
    if (m_bufferingTimeMs == 0 || m_isRtcp)
        return;

    //SPDLOG_DEBUG("RtpUdpSocket::startBufferingTimer() :: {}", m_bufferingTimeMs);
    m_bufferingTimer.expires_after(std::chrono::milliseconds(m_bufferingTimeMs));
    m_bufferingTimer.async_wait(
                WeakCallback(shared_from_this(),
                             [this](const boost::system::error_code& ec)
    {
        if (ec)
            return;

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            // check buffering queue interval
            auto now = std::chrono::steady_clock::now();
            auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_bufferingLastClock).count();

            if (elapsedMsec > m_bufferingTimeMs) {
                // when no packets in buffering time limits, clear buffering queue directlry.
                //SPDLOG_DEBUG("No packets in buffering time!! clear queue, {}", elapsedMsec);
                while (!m_bufferingQueue.empty()) {
                    auto first = m_bufferingQueue.top();
                    m_bufferingQueue.pop();
                    m_rtpSession.ReadRawRtpData(first->Buffer, first->BufferSize);
                }

                // reset start time point for buffering
                resetBuffering();
            }

            startBufferingTimer();
        }));
    }));
}


void RtpUdpSocket::BufferingRtpData(const unsigned char* buff, size_t byteTransferred)
{
    if (m_bufferingTimeMs == 0) {
        m_rtpSession.ReadRawRtpData(buff, byteTransferred);
        return;
    }

    m_bufferingLastClock = std::chrono::steady_clock::now();
    uint32_t rtpHeader = boost::asio::detail::socket_ops::network_to_host_long(*(uint32_t*)(buff));
    uint16_t sequenceNum = (uint16_t)(rtpHeader & 0xFFFF);

    auto bufferedPacket = std::make_shared<ReorderingBuffer>(sequenceNum, buff, byteTransferred);
    m_bufferingQueue.push(bufferedPacket);

    if (!m_bufferingCompleted) {
        auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(m_bufferingLastClock - m_bufferingStartClock).count();
        if (elapsedMsec <= m_bufferingTimeMs) {
            return;
        }
        m_bufferingCompleted = true;
    }

    // pop first packet and push
    if (!m_bufferingQueue.empty()) {
        auto first = m_bufferingQueue.top();
        m_bufferingQueue.pop();
        //SPDLOG_DEBUG("CALL RTP DATA:: CH={}, SEQUENCE={}, QUEUE_SIZE={}, BYTES={}",
        //             m_rtpSession.RtpChannel(),
        //             first->Seq, m_bufferingQueue.size(), byteTransferred);
        m_rtpSession.ReadRawRtpData(first->Buffer, first->BufferSize);
    }
}

void RtpUdpSocket::HandleReadPacket(const boost::system::error_code& error, size_t byteTransferred)
{
    if (error) {
        SPDLOG_INFO("Read error! error={}, message={}", error.value(), error.message());
        if (error == boost::asio::error::eof) {
            SPDLOG_ERROR("Disconnected by host...");
        }
        return;
    }

    // check RTP header size
    if (byteTransferred < kRtpHeaderSize) {
        SPDLOG_ERROR("RTP size is too small, size={}", byteTransferred);
    }
    else {
        const unsigned char* bufPtr = boost::asio::buffer_cast<const unsigned char*>(m_recvPackets->Buffer.data());
        if (m_isRtcp) {
            m_rtpSession.ReadRawRtcpData(bufPtr, byteTransferred);
        }
        else if (m_isRtcpMux){
            // https://tools.ietf.org/html/rfc5761
            // New RTCP packet types may be registered in the future and will
            // further reduce the RTP payload types that are available when
            // multiplexing RTP and RTCP onto a single port.To allow this
            // multiplexing, future RTCP packet type assignments SHOULD be made
            // after the current assignments in the range 209 - 223, then in the range
            // 194 - 199, so that only the RTP payload types in the range 64 - 95 are
            // blocked.RTCP packet types in the ranges 1 - 191 and 224 - 254 SHOULD
            // only be used when other values have been exhausted.
            uint32_t rtpHeader = network_to_host_long(*(uint32_t*)(bufPtr));
            unsigned char payloadType = (unsigned char)((rtpHeader & 0x007F0000) >> 16);
            if (payloadType >= 64 && payloadType <= 95) {
                // RTCP MULTIPLEXED PACKET
                m_rtpSession.ReadRawRtcpData(bufPtr, byteTransferred);
            }
            else {
                BufferingRtpData(bufPtr, byteTransferred);
            }
        }
        else {
            BufferingRtpData(bufPtr, byteTransferred);
        }
    }

    continuePlaying();
}


void RtpUdpSocket::WriteBackChannelData(unsigned char*data, size_t dataSize)
{
    auto buffer = std::make_shared<std::string>((char*)data, dataSize);

    m_socket->async_send_to(boost::asio::buffer(*buffer),
                            m_receiverEndpoint,
                            WeakCallback(shared_from_this(), [buffer, this]
                                         (const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (error) {
            SPDLOG_INFO("RtpUdpSocket::WriteBackChannelData() error! error={}, message={}", error.value(), error.message());
            return;
        }
        //SPDLOG_TRACE("RtpUdpSocket::WriteBackChannelData() write::{}", bytes_transferred);
    }));
}


_AsioStreamBuf::_AsioStreamBuf()
{
    SPDLOG_DEBUG("_AsioStreamBuf()");
}

_AsioStreamBuf::~_AsioStreamBuf()
{
    SPDLOG_DEBUG("~_AsioStreamBuf()");
}



}
}

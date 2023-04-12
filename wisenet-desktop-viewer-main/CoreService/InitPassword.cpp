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
#include "InitPassword.h"
#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"
#include "DigestAuthenticator.h"
#include "TextEncodeUtil.h"
#include <boost/algorithm/string.hpp>
#include <boost/core/ignore_unused.hpp>
#include "SecurityManager.h"
#include "TextEncodeUtil.h"

namespace Wisenet
{
namespace Core
{

using boost::asio::ip::udp;

InitPassword::InitPassword(asio::io_context& ioContext)
    : m_ioContext(ioContext)
    , m_strand(ioContext.get_executor())
{
    SPDLOG_INFO("Create a new Init Password instance");
    m_recvPackets.resize(4096);
}

InitPassword::~InitPassword()
{
    SPDLOG_INFO("Destroy a Init Password instance");
}

void InitPassword::Set(InitDevicePasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("Start IpSetting Change");
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        cancel();
        // send/recv discovery message
        getRsaKey(request, responseHandler);
    }));
}

void InitPassword::cancel()
{
    SPDLOG_DEBUG("Cancel all task");

    for (auto& s : m_sendSockets) {
        s->sock.close();
    }
    m_sendSockets.clear();

    if (m_recvSocket) {
        m_recvSocket->close();
        m_recvSocket.reset();
    }
}

void InitPassword::getRsaKey(InitDevicePasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    // recv
    m_recvSocket.reset(new udp::socket(m_ioContext));

    boost::asio::socket_base::receive_buffer_size sockBuffSize(102400);
    boost::asio::ip::udp::endpoint listen_endpoint(udp::v4(), kSunapiDiscoveryRecvPort);
    m_recvSocket->open(listen_endpoint.protocol());
    m_recvSocket->set_option(udp::socket::reuse_address(true));
    m_recvSocket->set_option(sockBuffSize);
    m_recvSocket->bind(listen_endpoint);

    asyncRecvFrom(request, responseHandler);

    // send broadcast
    m_sendSockets.clear();
    auto addrs = GetDiscoverySources();
    boost::system::error_code ec;

    for (auto const& addr : addrs) {
        std::string ip = addr.ip().toString().toStdString();

        boost::asio::ip::address_v4 local_interface = boost::asio::ip::make_address_v4(ip, ec);
        if (ec) {
            SPDLOG_WARN("Can not make ipv4 address from {}. skip it.", ip);
            continue;
        }

        auto sender(std::make_shared<SendSocket>(m_ioContext));

        auto sendPoint(udp::endpoint(local_interface, 0));
        sender->sock.open(sendPoint.protocol(), ec);
        if (ec) {
            SPDLOG_WARN("Can not open send socket. ip={}, what={}", ip, ec.message());
            continue;
        }
        sender->sock.bind(sendPoint, ec);
        if (ec) {
            SPDLOG_WARN("Can not bind send socket, ip={}, what={}", ip, ec.message());
            continue;
        }
        sender->sock.set_option(boost::asio::socket_base::broadcast(true), ec);
        if (ec) {
            SPDLOG_WARN("Set broadcast send socket, ip={}, what={}", ip, ec.message());
            continue;
        }

        m_sendSockets.push_back(sender);
        udp::endpoint broadcastEndpoint(boost::asio::ip::address_v4::broadcast(), kSunapiDiscoverySendPort);

        SUNAPI_DATA_PACKET packet;
        const size_t packetSize = sizeof(packet);
        sender->sendPacket.resize(packetSize);

        std::memset(&packet, 0x00, packetSize);

        packet.Mode = DEF_REQ_SCAN_RSA;

        unsigned char* sendPtr = &sender->sendPacket[0];
        std::memcpy(sendPtr, (unsigned char*)&packet, sizeof(packet));

        SPDLOG_DEBUG("Send ip rsa key for sunapi device with ip={}", ip);
        sender->sock.async_send_to(boost::asio::buffer(sender->sendPacket, packetSize), broadcastEndpoint,
            WeakCallback(shared_from_this(), [ip](const boost::system::error_code& error, size_t byteTransferred)
        {
            if (error) {
                SPDLOG_WARN("Send ipSetting message failed, ip={}, ec={}", ip, error.message());
                return;
            }
            SPDLOG_TRACE("Send ipSetting message success={}, byteTransffered={}", ip, byteTransferred);
        }));
    }
}

void InitPassword::setPassword(InitDevicePasswordRequestSharedPtr const& request, InitPasswordParam const& param, ResponseBaseHandler const& responseHandler)
{    
    m_recvSocket.reset(new udp::socket(m_ioContext));

    boost::asio::socket_base::receive_buffer_size sockBuffSize(102400);
    boost::asio::ip::udp::endpoint listen_endpoint(udp::v4(), kSunapiDiscoveryRecvPort);
    m_recvSocket->open(listen_endpoint.protocol());
    m_recvSocket->set_option(udp::socket::reuse_address(true));
    m_recvSocket->set_option(sockBuffSize);
    m_recvSocket->bind(listen_endpoint);

    asyncRecvFrom(request, responseHandler);

    m_sendSockets.clear();
    auto addrs = GetDiscoverySources();
    boost::system::error_code ec;

    for (auto const& addr : addrs) {
        std::string ip = addr.ip().toString().toStdString();

        boost::asio::ip::address_v4 local_interface = boost::asio::ip::make_address_v4(ip, ec);
        if (ec) {
            SPDLOG_WARN("Can not make ipv4 address from {}. skip it.", ip);
            continue;
        }

        auto sender(std::make_shared<SendSocket>(m_ioContext));

        auto sendPoint(udp::endpoint(local_interface, 0));
        sender->sock.open(sendPoint.protocol(), ec);
        if (ec) {
            SPDLOG_WARN("Can not open send socket. ip={}, what={}", ip, ec.message());
            continue;
        }
        sender->sock.bind(sendPoint, ec);
        if (ec) {
            SPDLOG_WARN("Can not bind send socket, ip={}, what={}", ip, ec.message());
            continue;
        }
        sender->sock.set_option(boost::asio::socket_base::broadcast(true), ec);
        if (ec) {
            SPDLOG_WARN("Set broadcast send socket, ip={}, what={}", ip, ec.message());
            continue;
        }

        m_sendSockets.push_back(sender);
        udp::endpoint broadcastEndpoint(boost::asio::ip::address_v4::broadcast(), kSunapiDiscoverySendPort);

        SUNAPI_DATA_PACKET_PASSWORD packet;
        const size_t packetSize = sizeof(packet);
        sender->sendPacket.resize(packetSize);
        std::memset(&packet, 0x00, packetSize);

        packet.Mode = DEF_REQ_APPLY_PASSWORD;
        std::string password = Wisenet::Library::SecurityManager::EncryptPassword(param.payload, param.password);
        std::string base64 = Wisenet::Common::Base64Encode(password);

        std::copy(base64.begin(), base64.end(), packet.Payload);
        packet.Payload[base64.size()] = '\0';
        std::copy(param.macAddress.begin(), param.macAddress.end(), packet.MacAddress);
        packet.MacAddress[param.macAddress.size()] = '\0';
        packet.payloadSize = (unsigned short)(base64.size());

        unsigned char* sendPtr = &sender->sendPacket[0];
        std::memcpy(sendPtr, (unsigned char*)&packet, sizeof(packet));

        SPDLOG_DEBUG("Send ip password setting for sunapi device with ip={}", ip);
        sender->sock.async_send_to(boost::asio::buffer(sender->sendPacket, packetSize), broadcastEndpoint,
            WeakCallback(shared_from_this(), [ip](const boost::system::error_code& error, size_t byteTransferred)
        {
            if (error) {
                SPDLOG_WARN("Send password setting message failed, ip={}, ec={}", ip, error.message());
                return;
            }
            SPDLOG_TRACE("Send password setting message success={}, byteTransffered={}", ip, byteTransferred);
        }));
    }
}

void InitPassword::asyncRecvFrom(InitDevicePasswordRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    if (!m_recvSocket)
        return;

    m_recvSocket->async_receive_from(boost::asio::buffer(m_recvPackets, m_recvPackets.size()), m_senderEndpoint,
        WeakCallback(shared_from_this(), [this, request, responseHandler](const boost::system::error_code& ec, size_t byteTransferred)
    {
        boost::ignore_unused(byteTransferred);
        if (ec) {
            if (ec == boost::asio::error::operation_aborted)
                SPDLOG_DEBUG("rsa key recv aborted");
            else
                SPDLOG_WARN("Failed to receive sunapi rsa key message, what={}", ec.message());
            return;
        }

        unsigned char* ptr = &m_recvPackets[0];
        SUNAPI_DATA_PACKET_RSA* pkt = (SUNAPI_DATA_PACKET_RSA*)ptr;

        if (pkt->MacAddress == request->macAddress && pkt->Mode == DEF_RES_SCAN_RSA){
            InitPasswordParam param;
            param.macAddress = request->macAddress;
            param.password = request->password;
            param.payload = std::string(pkt->Payload);
            param.PayloadSize = pkt->PayloadSize;

            asio::post(m_strand, WeakCallback(shared_from_this(), [this, request, param, responseHandler]()
            {
                setPassword(request, param, responseHandler);
            }));
        }
        else if (pkt->MacAddress == request->macAddress && 
            (pkt->Mode == DEF_RES_APPLY_PASSWORD_ERR  || pkt->Mode == DEF_RES_APPLY_PASSWORD)){
            auto response = std::make_shared<ResponseBase>();

            if (pkt->Mode == DEF_RES_APPLY_PASSWORD_ERR) {
                response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            }
            SPDLOG_DEBUG("Password Setting {}", GetErrorString(response->errorCode));
            responseHandler(response);
        }       
        else {
            asyncRecvFrom(request, responseHandler);
        }
    }));
}

}
}

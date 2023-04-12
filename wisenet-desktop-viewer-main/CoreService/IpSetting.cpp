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
#include "IpSetting.h"
#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"
#include "DigestAuthenticator.h"
#include <boost/algorithm/string.hpp>

namespace Wisenet
{
namespace Core
{

using boost::asio::ip::udp;

IpSetting::IpSetting(asio::io_context& ioContext)
    : m_ioContext(ioContext)
    , m_strand(ioContext.get_executor())
{
    SPDLOG_INFO("Create a new Ip Setting instance");
    m_recvPackets.resize(4096);
}

IpSetting::~IpSetting()
{
    SPDLOG_INFO("Destroy a IpSetting instance");
}

void IpSetting::Change(IpSettingDeviceRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("Start IpSetting Change");
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, request, responseHandler]()
    {
        cancel();
        // send/recv discovery message
        changeInternal(request, responseHandler);
    }));
}

void IpSetting::cancel()
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

void IpSetting::changeInternal(IpSettingDeviceRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
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

        packet.Mode = DEF_REQ_APPLY_EX;

        std::copy(request->ipSetting.macAddress.begin(), request->ipSetting.macAddress.end(), packet.MacAddress);
        packet.MacAddress[request->ipSetting.macAddress.size()] = '\0';

        std::copy(request->ipSetting.ipAddress.begin(), request->ipSetting.ipAddress.end(), packet.IPAddress);
        packet.IPAddress[request->ipSetting.ipAddress.size()] = '\0';


        std::copy(request->ipSetting.subnetMaskAddress.begin(), request->ipSetting.subnetMaskAddress.end(), packet.SubnetMask);
        packet.SubnetMask[request->ipSetting.subnetMaskAddress.size()] = '\0';

        std::copy(request->ipSetting.gatewayAddress.begin(), request->ipSetting.gatewayAddress.end(), packet.Gateway);
        packet.Gateway[request->ipSetting.gatewayAddress.size()] = '\0';

        if (request->ipSetting.supportPasswordEncryption) {           
            std::stringstream ss; 
            ss << request->ipSetting.id << ":"  << request->ipSetting.password << ":" << request->ipSetting.passwordNonce;    
            std::string out = DigestAuthenticator::hashSha256(ss.str());
            memcpy(packet.Password, out.c_str(), sizeof(packet.Password));
            packet.Version = 0x08;
        }
        else
        {
            std::copy(request->ipSetting.password.begin(), request->ipSetting.password.end(), packet.Password);
            packet.Password[request->ipSetting.password.size()] = '\0';
        }

        packet.Port = request->ipSetting.httpPort;
        packet.HttpPort = request->ipSetting.httpPort;
        packet.DevicePort = request->ipSetting.devicePort;

        if (request->ipSetting.addressType== DiscoverAddressType::StaticIP)
            packet.NetworkMode = SUNAPI_NETWORK_MODE_STATIC;
        else if (request->ipSetting.addressType == DiscoverAddressType::DHCP)
            packet.NetworkMode = SUNAPI_NETWORK_MODE_DHCP;
        else if (request->ipSetting.addressType == DiscoverAddressType::PPPoE)
            packet.NetworkMode = SUNAPI_NETWORK_MODE_PPPOE;

        unsigned char* sendPtr = &sender->sendPacket[0];
        std::memcpy(sendPtr, (unsigned char*)&packet, sizeof(packet));

        SPDLOG_DEBUG("Send ip broadcast for sunapi device with ip={}", ip);
        sender->sock.async_send_to(boost::asio::buffer(sender->sendPacket, packetSize), broadcastEndpoint, 
            WeakCallback(shared_from_this(), [ip] (const boost::system::error_code& error, size_t byteTransferred)
        {
            if (error) {
                SPDLOG_WARN("Send ipSetting message failed, ip={}, ec={}", ip, error.message());
                return;
            }
            SPDLOG_TRACE("Send ipSetting message success={}, byteTransffered={}", ip, byteTransferred);
        }));
    }
}

void IpSetting::asyncRecvFrom(IpSettingDeviceRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler)
{
    if (!m_recvSocket)
        return;

    m_recvSocket->async_receive_from(boost::asio::buffer(m_recvPackets, m_recvPackets.size()), m_senderEndpoint, 
        WeakCallback(shared_from_this(), [this, request, responseHandler] (const boost::system::error_code& ec, size_t byteTransferred)
    {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted)
                SPDLOG_DEBUG("Discovery recv aborted");
            else
                SPDLOG_WARN("Failed to receive sunapi discovery message, what={}", ec.message());
            return;
        }

        if (byteTransferred != sizeof(SUNAPI_DATA_PACKET)) {
            asyncRecvFrom(request, responseHandler);
            return;
        }

        unsigned char* ptr = &m_recvPackets[0];
        SUNAPI_DATA_PACKET* pkt = (SUNAPI_DATA_PACKET*)ptr;
        if (pkt->Mode != DEF_RES_APPLYERR && pkt->Mode != DEF_RES_PASSERR &&
            pkt->Mode != DEF_RES_APPLY && pkt->Mode != DEF_RES_APPLY_EX) {
            asyncRecvFrom(request, responseHandler);
            return;
        }

        IpSettingInfo deviceInfo;
        auto response = std::make_shared<IpSettingDeviceResponse>();
        
        if (pkt->Mode == DEF_RES_PASSERR)
            response->errorCode = Wisenet::ErrorCode::UnAuthorized;
        else if (pkt->Mode == DEF_RES_APPLYERR)
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
        else if (std::string(pkt->MacAddress) != request->ipSetting.macAddress){
            asyncRecvFrom(request, responseHandler);
            return;
        }
        else{
            response->ipInstall.macAddress = std::string(pkt->MacAddress);
            response->ipInstall.ipAddress = std::string(pkt->IPAddress);
            response->ipInstall.subnetMaskAddress = std::string(pkt->SubnetMask);
            response->ipInstall.gatewayAddress = std::string(pkt->Gateway);
            response->ipInstall.devicePort = pkt->DevicePort;
            response->ipInstall.httpPort = pkt->HttpPort;
            if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_STATIC)
                response->ipInstall.addressType = DiscoverAddressType::StaticIP;
            else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_DHCP)
                response->ipInstall.addressType = DiscoverAddressType::DHCP;
            else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_PPPOE)
                response->ipInstall.addressType = DiscoverAddressType::PPPoE;

            if (pkt->Version & VERSION_SUPPORT_PASSWORD_VERIFICATION) {
                response->ipInstall.supportPasswordEncryption = true;
                response->ipInstall.passwordNonce = std::string(pkt->Password);
                response->ipInstall.passwordNonce.resize(20);
            }
        }
        responseHandler(response);        
    }));
}

}
}

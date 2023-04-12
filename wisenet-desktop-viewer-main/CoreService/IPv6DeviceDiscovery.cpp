/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "IPv6DeviceDiscovery.h"
#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"

#include <boost/core/ignore_unused.hpp>
#include <boost/algorithm/string.hpp>
#include <string.h>


namespace Wisenet
{
namespace Core
{

using boost::asio::ip::udp;

IPv6DeviceDiscovery::IPv6DeviceDiscovery(asio::io_context& ioContext)
    : m_ioContext(ioContext)
    , m_strand(ioContext.get_executor())
    , m_waitTimer(ioContext)
    , m_dataCollectTimer(ioContext)
{
    SPDLOG_INFO("Create a new IPv6DeviceDiscovery instance");
    m_recvPackets.resize(4096);
}


IPv6DeviceDiscovery::~IPv6DeviceDiscovery()
{
    SPDLOG_INFO("Destroy a IPv6DeviceDiscovery instance");
}

void IPv6DeviceDiscovery::StartDiscoveryIPv6(DiscoverDevicesRequestSharedPtr const& request,
    ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("Start discovery IPv6");
    asio::dispatch(m_strand,
        WeakCallback(shared_from_this(),
            [this, request, responseHandler]()
    {
        // cancel all previous actions
        cancel();

        // start wait timer
        wait(responseHandler);

        // start collect timer
        aggregate(responseHandler);

        // send/recv discovery message
        discoverSunapiIPv6(request);
    }));
}

void IPv6DeviceDiscovery::wait(ResponseBaseHandler const& responseHandler)
{
    m_waitTimer.expires_after(std::chrono::seconds(5));
    m_waitTimer.async_wait(
                WeakCallback(shared_from_this(),
                             [this, responseHandler](const boost::system::error_code& ec)
    {
        if (ec)
            return;

        SPDLOG_DEBUG("Discovery timed out! close all socket i/o.");

        // transfer final response
        asio::dispatch(m_strand,
                       WeakCallback(shared_from_this(), [this, responseHandler]()
        {
            onResponse(responseHandler, boost::system::error_code(), false);
            cancel();
        }));
    }));
}


void IPv6DeviceDiscovery::onResponse(ResponseBaseHandler const& responseHandler,
                                 const boost::system::error_code& ec, bool isContinue)
{
    if (responseHandler == nullptr)
        return;

    auto response = std::make_shared<DiscoverDevicesResponse>();
    if (ec) {
        response->errorCode = Wisenet::ErrorCode::NetworkError;
        responseHandler(response);
        return;
    }

    response->isContinue = isContinue;
    response->discoveredDevices = m_discoveredDevices;
    responseHandler(response);
}


void IPv6DeviceDiscovery::cancel()
{
    SPDLOG_DEBUG("Cancel all discovery task");

    for(auto& s : m_sendSockets) {
        s->sock.close();
    }
    m_sendSockets.clear();

    if (m_recvSocket) {
        m_recvSocket->close();
        m_recvSocket.reset();
    }

    m_waitTimer.cancel();
    m_dataCollectTimer.cancel();
    m_discoveredDevices.clear();

}

void IPv6DeviceDiscovery::aggregate(ResponseBaseHandler const& responseHandler)
{
    m_dataCollectTimer.expires_after(std::chrono::milliseconds(50));

    m_dataCollectTimer.async_wait( WeakCallback(shared_from_this(),
                                                [this, responseHandler](const boost::system::error_code& ec)
    {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted)
                SPDLOG_DEBUG("Discovery aggregation aborted");

            return;
        }

        asio::post(m_strand,
                   WeakCallback(shared_from_this(), [this, responseHandler]()
        {
            if (m_discoveredDevices.size() > 0) {
                SPDLOG_DEBUG("== SUNAPI Discovered aggregation count={}", m_discoveredDevices.size());
                if (spdlog::get_level() == spdlog::level::trace) {
                    for (auto &deviceInfo : m_discoveredDevices) {
                        SPDLOG_TRACE("SUNAPI Discovery response, type={}, model={}, ipAddress={}, httpPort={}, passwordReset={}",
                                     deviceInfo.deviceType, deviceInfo.model, deviceInfo.ipAddress, deviceInfo.httpPort, deviceInfo.needToPasswordConfig);
                    }
                }

                onResponse(responseHandler, boost::system::error_code(), true);
                m_discoveredDevices.clear();
            }

            // restart aggregate timer
            aggregate(responseHandler);
        }));

    }));
}

void IPv6DeviceDiscovery::discoverSunapiIPv6(DiscoverDevicesRequestSharedPtr const& request)
{
    boost::ignore_unused(request);
    // find local interface IPv6 address
    std::string ip = GetIPv6Address();

    boost::system::error_code ec;    
    // recv
    if (m_recvSocket && m_recvSocket->is_open())
    {
        m_recvSocket->cancel();
        m_recvSocket->close();
    }

    m_recvSocket.reset(new udp::socket(m_ioContext));

    boost::asio::ip::address local_interface_v6 = boost::asio::ip::address::from_string(ip);
    boost::asio::socket_base::receive_buffer_size sockBuffSizeV6(102400);
    boost::asio::ip::udp::endpoint listen_endpointV6(local_interface_v6, kSunapiDiscoveryRecvPort);
    m_recvSocket->open(listen_endpointV6.protocol(), ec);
    if (ec) {
        SPDLOG_WARN("Can not open recv socket. what={}", ec.message());
        return;
    }
    m_recvSocket->set_option(udp::socket::reuse_address(true));
    m_recvSocket->set_option(sockBuffSizeV6);
   
    m_recvSocket->bind(listen_endpointV6, ec);
    if (ec) {
        SPDLOG_WARN("Can not bind recv socket. what={}", ec.message());
        return;
    }
   
    asyncRecvFrom();

    // send broadcast
    m_sendSockets.clear();
    
    auto senderV6(std::make_shared<SendSocket>(m_ioContext));
    auto sendPointV6(udp::endpoint(local_interface_v6, kSunapiDiscoveryRecvPort));
    
    senderV6->sock.open(sendPointV6.protocol(), ec);
    if (ec) {
        SPDLOG_WARN("Can not open send socket. what={}", ec.message());
        return;
    }
    senderV6->sock.set_option(udp::socket::reuse_address(true));

    boost::asio::ip::multicast::hops hopOption(16);
    senderV6->sock.set_option(hopOption);


    senderV6->sock.bind(sendPointV6, ec);
    if (ec) {
        SPDLOG_WARN("Can not bind send socket, what={}", ec.message());
        return;
    }

    m_sendSockets.push_back(senderV6);
    udp::endpoint multicastEndpoint(boost::asio::ip::make_address_v6("ff02::1111"), kSunapiDiscoverySendPort);
  

    SUNAPI_DATA_PACKET_V6 packet;
    const size_t packetSize = sizeof(packet);
    senderV6->sendPacket.resize(packetSize);

    std::memset(&packet, 0x00, packetSize);
    packet.Mode = DEF_REQ_SCAN_EX;

    unsigned char* sendPtr = &senderV6->sendPacket[0];
    std::memcpy(sendPtr, (unsigned char*)&packet, sizeof(packet));

    SPDLOG_DEBUG("Send discovery multicast for sunapi device");
    senderV6->sock.async_send_to(boost::asio::buffer(senderV6->sendPacket, packetSize),
        multicastEndpoint,
        WeakCallback(shared_from_this(),
            []
            (const boost::system::error_code& error, size_t byteTransferred)
            {
                if (error) {
                    SPDLOG_WARN("Send discovery message failed, ec={}", error.message());
                    return;
                }
                SPDLOG_WARN("Send IPv6 discovery message success, byteTransffered={}", byteTransferred);
            }));
}

std::string Wisenet::Core::IPv6DeviceDiscovery::GetIPv6Address()
{
    std::string ip = "";
    std::string tempIp = "";

    auto addrs = GetDiscoveryIPv6Sources();
    for (auto const& addr : addrs) {
        tempIp = addr.ip().toString().toStdString();
        SPDLOG_WARN("IPv6 address with Scope ID from {}.", ip);
        if (tempIp.length() > 0)
        {
#ifdef Q_OS_WINDOWS
            //remove scope id
            auto itr = tempIp.find("%");

            if (itr != std::string::npos) {
                tempIp.erase(itr, tempIp.size());
            }
#endif
            if (ip == "")
                ip = tempIp;
            else if (ip.length() < tempIp.length())
                ip = tempIp;
        }
    }
    SPDLOG_WARN("IPv6 address from {}.", ip);
    return ip;
}

void IPv6DeviceDiscovery::asyncRecvFrom()
{
    if (!m_recvSocket)
        return;

    m_recvSocket->async_receive_from(boost::asio::buffer(m_recvPackets, m_recvPackets.size()),
                                     m_senderEndpoint,
                                   WeakCallback(shared_from_this(), [this]
                                                  (const boost::system::error_code& ec, size_t byteTransferred)
    {
        if (ec) {
            if (ec == boost::asio::error::operation_aborted)
                SPDLOG_DEBUG("Discovery recv aborted");
            else
                SPDLOG_WARN("Failed to receive sunapi discovery message, what={}", ec.message());
            return;
        }

        if (byteTransferred != sizeof(SUNAPI_DATA_PACKET_V6)) {
            asyncRecvFrom();
            return;
        }

        unsigned char* ptr = &m_recvPackets[0];
        SUNAPI_DATA_PACKET_V6* pkt = (SUNAPI_DATA_PACKET_V6*)ptr;
        if (pkt->Mode != DEF_RES_SCAN_EX) {
            asyncRecvFrom();
            return;
        }

        // no compatible sunapi version
        if (!(pkt->SupportedProtocol & 0x08)) {
            // except SNP-6320(H)
            if (!boost::starts_with(pkt->DeviceName, "SNP-632")) {
                    asyncRecvFrom();
                    return;
            }
        }
#ifndef SUNAPI_CAMERA_SUPPORT
        // Device Type (0x00: Camera, 0x01: Encoder, 0x02: Decoder, 0x03: Recorder, 0x04: IOBox, 0x05: NetworkSpeaker/Mic
        if (pkt->ModelType != 0x03) {
            asyncRecvFrom();
            return;
        }
#endif

        DiscoveryDeviceInfo deviceInfo;
        deviceInfo.ipVersion = Wisenet::Core::InternetProtocolVersion::IPv6;
        if (pkt->ModelType == 0x03)
            deviceInfo.deviceType = Wisenet::Device::DeviceType::SunapiRecorder;
        else
            deviceInfo.deviceType = Wisenet::Device::DeviceType::SunapiCamera;

        if (pkt->Version & VERSION_SUPPORT_NEW_MODELNAME) {
            deviceInfo.model = std::string(pkt->NewModelName);
        }

        if (deviceInfo.model.empty())
            deviceInfo.model = std::string(pkt->DeviceName);

        if (pkt->Version & VERSION_SUPPORT_PASSWORD_VERIFICATION) {

            if (pkt->PasswordStatus == 0x01)
                deviceInfo.needToPasswordConfig = true;
            deviceInfo.supportPasswordEncryption = true;


            deviceInfo.passwordNonce = std::string(pkt->Password);

            //SPDLOG_WARN("Nonce #1={} --- {}", std::string(pkt->IPAddress), deviceInfo.passwordNonce);

            if(deviceInfo.passwordNonce.size() > 20){
                deviceInfo.passwordNonce.resize(20);
                 //SPDLOG_WARN("Nonce #2={} --- {}", std::string(pkt->IPAddress), deviceInfo.passwordNonce);
            }
        }

        if (pkt->isOnlySunapiProtocol)
            deviceInfo.supportSunapiOnly = true;

        if (pkt->ModelType == 0x03)
            deviceInfo.deviceType = Wisenet::Device::DeviceType::SunapiRecorder;
        else
            deviceInfo.deviceType = Wisenet::Device::DeviceType::SunapiCamera;

        // todo :: byte ordering
        deviceInfo.httpPort = pkt->HttpPort;
        deviceInfo.httpsPort = pkt->HttpsPort;

        if (pkt->HttpMode != 0) {
            deviceInfo.isHttpsEnable = true;
        }

        deviceInfo.macAddress = std::string(pkt->MacAddress);
        deviceInfo.ipAddress = std::string(pkt->IPv6Address);
        deviceInfo.subnetMaskAddress = std::string(pkt->SubnetMask);
        deviceInfo.gatewayAddress = std::string(pkt->Gateway);
        deviceInfo.devicePort = pkt->DevicePort;

        if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_STATIC)
            deviceInfo.addressType = DiscoverAddressType::StaticIP;
        else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_DHCP)
            deviceInfo.addressType = DiscoverAddressType::DHCP;
        else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_PPPOE)
            deviceInfo.addressType = DiscoverAddressType::PPPoE;
        else {
            SPDLOG_WARN("DeviceDiscovery::asyncRecvFromIPv6 - Unknown NetworkMode");
            asyncRecvFrom();
            return;
        }

        deviceInfo.supportIpConfig = true;
        deviceInfo.supportDhcpConfig = true;
        deviceInfo.supportPasswordConfig = true;
        deviceInfo.manufacturer = "Hanwha Vision";

       SPDLOG_WARN("SUNAPI IPv6 Discovery response, type={}, model={}, ipAddress={}, httpPort={}, passwordReset={}",
                     deviceInfo.deviceType, deviceInfo.model, deviceInfo.ipAddress, deviceInfo.httpPort, deviceInfo.needToPasswordConfig);

        asio::post(m_strand,
                   WeakCallback(shared_from_this(), [this, deviceInfo] ()
        {
            if(deviceInfo.model != "" && deviceInfo.ipAddress != "")
                m_discoveredDevices.push_back(deviceInfo);
        }));

        asyncRecvFrom();
    }));
}


}
}

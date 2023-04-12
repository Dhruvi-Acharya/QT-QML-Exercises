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
#include "DeviceDiscovery.h"
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

DeviceDiscovery::DeviceDiscovery(asio::io_context& ioContext)
    : m_ioContext(ioContext)
    , m_strand(ioContext.get_executor())
    , m_waitTimer(ioContext)
    , m_dataCollectTimer(ioContext)
{
    SPDLOG_INFO("Create a new DeviceDiscovery instance");
     m_recvPackets.resize(4096);
}


DeviceDiscovery::~DeviceDiscovery()
{
    SPDLOG_INFO("Destroy a DeviceDiscovery instance");
}

void DeviceDiscovery::StartDiscovery(DiscoverDevicesRequestSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler)
{
    SPDLOG_DEBUG("Start discovery");
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
        discoverSunapi(request);
    }));
}

void DeviceDiscovery::wait(ResponseBaseHandler const& responseHandler)
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


void DeviceDiscovery::onResponse(ResponseBaseHandler const& responseHandler,
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


void DeviceDiscovery::cancel()
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

void DeviceDiscovery::aggregate(ResponseBaseHandler const& responseHandler)
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

void DeviceDiscovery::discoverSunapi(DiscoverDevicesRequestSharedPtr const& request)
{
    boost::ignore_unused(request);
    // recv
    m_recvSocket.reset(new udp::socket(m_ioContext));

    boost::asio::socket_base::receive_buffer_size sockBuffSize(102400);
    boost::asio::ip::udp::endpoint listen_endpoint(udp::v4(), kSunapiDiscoveryRecvPort);
    m_recvSocket->open(listen_endpoint.protocol());
    m_recvSocket->set_option(udp::socket::reuse_address(true));
    m_recvSocket->set_option(sockBuffSize);
    m_recvSocket->bind(listen_endpoint);

    asyncRecvFrom();

    // send broadcast
    m_sendSockets.clear();
    auto addrs =  GetDiscoverySources();
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

        // pushback in sender socket list
        m_sendSockets.push_back(sender);

        udp::endpoint broadcastEndpoint(boost::asio::ip::address_v4::broadcast(), kSunapiDiscoverySendPort);
        SUNAPI_DATA_PACKET packet;
        const size_t packetSize = sizeof(packet);
        sender->sendPacket.resize(packetSize);

        std::memset(&packet, 0x00, packetSize );
        packet.Mode = DEF_REQ_SCAN_EX;
        packet.Version = 0x08;

        unsigned char * sendPtr = &sender->sendPacket[0];
        std::memcpy(sendPtr, (unsigned char*)&packet, sizeof(packet));

        SPDLOG_DEBUG("Send discovery broadcast for sunapi device with ip={}", ip);
        sender->sock.async_send_to(boost::asio::buffer(sender->sendPacket, packetSize),
                                   broadcastEndpoint,
                                   WeakCallback(shared_from_this(),
                                                [ip]
                                                (const boost::system::error_code& error, size_t byteTransferred)
        {
            if (error) {
                SPDLOG_WARN("Send discovery message failed, ip={}, ec={}", ip, error.message());
                return;
            }
            SPDLOG_TRACE("Send discovery message success={}, byteTransffered={}", ip, byteTransferred);
        }));
    }
}

void DeviceDiscovery::asyncRecvFrom()
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

        if (byteTransferred != sizeof(SUNAPI_DATA_PACKET)) {
            asyncRecvFrom();
            return;
        }

        unsigned char* ptr = &m_recvPackets[0];
        SUNAPI_DATA_PACKET* pkt = (SUNAPI_DATA_PACKET*)ptr;
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
        deviceInfo.ipVersion = Wisenet::Core::InternetProtocolVersion::IPv4;
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
        deviceInfo.ipAddress = std::string(pkt->IPAddress);
        deviceInfo.subnetMaskAddress = std::string(pkt->SubnetMask);
        deviceInfo.gatewayAddress = std::string(pkt->Gateway);
        deviceInfo.devicePort = pkt->DevicePort;

//        boost::system::error_code ecc;
//        boost::asio::ip::address_v4 addr = boost::asio::ip::make_address_v4(pkt->IPAddress, ecc);
//        if (!ecc)
//            deviceInfo.ipV4ToUint = addr.to_uint();

        if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_STATIC)
            deviceInfo.addressType = DiscoverAddressType::StaticIP;
        else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_DHCP)
            deviceInfo.addressType = DiscoverAddressType::DHCP;
        else if (pkt->NetworkMode == SUNAPI_NETWORK_MODE_PPPOE)
            deviceInfo.addressType = DiscoverAddressType::PPPoE;
        else {
            SPDLOG_WARN("DeviceDiscovery::asyncRecvFrom - Unknown NetworkMode");
            asyncRecvFrom();
            return;
        }

        deviceInfo.supportIpConfig = true;
        deviceInfo.supportDhcpConfig = true;
        deviceInfo.supportPasswordConfig = true;
        deviceInfo.manufacturer = "Hanwha Vision";

        //SPDLOG_TRACE("SUNAPI Discovery response, type={}, model={}, ipAddress={}, httpPort={}, passwordReset={}",
        //             deviceInfo.deviceType, deviceInfo.model, deviceInfo.ipAddress, deviceInfo.httpPort, deviceInfo.needToPasswordConfig);

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

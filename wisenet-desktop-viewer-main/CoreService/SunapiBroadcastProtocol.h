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
#include <boost/asio.hpp>
#include <QNetworkInterface>

namespace Wisenet
{
namespace Core
{
struct SendSocket
{
    explicit SendSocket(boost::asio::io_context& ioContext)
        : sock(ioContext)
    {}
    boost::asio::ip::udp::socket sock;
    std::vector<unsigned char> sendPacket;
};

typedef std::shared_ptr<SendSocket> SendSocketPtr;


const unsigned short kSunapiDiscoverySendPort = 7701;
const unsigned short kSunapiDiscoveryRecvPort = 7711;

const unsigned short kSvpDvrDiscoverySendPort = 7601;
const unsigned short kSvpDvrDiscoveryRecvPort = 7611;

const unsigned short kSvpNvrDiscoverySendPort = 7603;
const unsigned short kSvpNvrDiscoveryRecvPort = 7613;


enum PACKET_MODE
{
    DEF_REQ_SCAN = 1,
    DEF_REQ_APPLY = 2,
    DEF_REQ_REBOOT = 3,
    DEF_REQ_PORTMAPPING = 4,
    DEF_REQ_SCAN_EX = 6,
    DEF_REQ_APPLY_EX = 7,
    DEF_REQ_SCAN_RSA = 8,
    DEF_REQ_APPLY_PASSWORD = 9,

    DEF_RES_SCAN = 11,
    DEF_RES_SCAN_EX = 12,
    DEF_RES_SCAN_RSA = 13,
    DEF_RES_APPLY = 22,
    DEF_RES_APPLY_EX = 23,
    DEF_RES_APPLY_PASSWORD_ERR = 24,
    DEF_RES_APPLY_PASSWORD = 25,
    DEF_RES_PASSERR = 33,
    DEF_RES_PORTMAPPING = 44,
    DEF_RES_PORTMAPPINGERR = 55,
    DEF_RES_ROUTERCONERR = 66,
    DEF_RES_APPLYERR = 77
};

enum SUNAPI_NETWORK_MODE
{
    SUNAPI_NETWORK_MODE_STATIC = 0x00,
    SUNAPI_NETWORK_MODE_DHCP = 0x01,
    SUNAPI_NETWORK_MODE_PPPOE = 0x02,
};

enum SUNAPI_DISCOVERY_VERSION
{
    VERSION_CANT_CHANGE_HTTPS = 0x01,
    VERSION_CAN_CHANGE_HTTPS = 0x02,
    VERSION_SUPPORT_NEW_MODELNAME = 0x04,
    VERSION_SUPPORT_PASSWORD_VERIFICATION = 0x08
};

struct DATA_PACKET
{
    unsigned char	Mode;
    char	PacketID[18];
    char	MacAddress[18];
    char	IPAddress[16];
    char	SubnetMask[16];
    char	Gateway[16];
    char	Password[20];
    char	isOnlySunapiProtocol; /* new */
    unsigned short	Port;
    unsigned char	Status;
    char	DeviceName[10];
    unsigned short	HttpPort;
    unsigned short	DevicePort;
    unsigned short	TcpPort;
    unsigned short	UdpPort;
    unsigned short	UploadPort;
    unsigned short	MulticastPort;
    unsigned char	NetworkMode;
    char	DDNS[128];
};

struct SUNAPI_DATA_PACKET
{
    unsigned char	Mode;
    char	PacketID[18];
    char	MacAddress[18];
    char	IPAddress[16];
    char	SubnetMask[16];
    char	Gateway[16];
    char	Password[20];
    char	isOnlySunapiProtocol; /* new */
    unsigned short	Port;
    unsigned char	Status;
    char	DeviceName[10];
    unsigned short	HttpPort;
    unsigned short	DevicePort;
    unsigned short	TcpPort;
    unsigned short	UdpPort;
    unsigned short	UploadPort;
    unsigned short	MulticastPort;
    unsigned char	NetworkMode;
    char	DDNS[128];
    /*----------------------------*/
    char Alias[32];
    char NewModelName[32];
    unsigned char	ModelType; //0x00 ~ 0x03, camera, encoder, decoder, NVR
    unsigned short	Version;
    unsigned char	HttpMode; // http or https
    unsigned short	HttpsPort;
    unsigned char	SupportedProtocol;
    unsigned char	PasswordStatus;
};

struct SUNAPI_DATA_PACKET_V6
{
    unsigned char	Mode;
    char	PacketID[18];
    char	MacAddress[18];
    char	IPAddress[16];
    char	SubnetMask[16];
    char	Gateway[16];
    char    IPv6Address[40];
    char	Password[20];
    char	isOnlySunapiProtocol; /* new */
    unsigned short	Port;
    unsigned char	Status;
    char	DeviceName[10];
    unsigned short	HttpPort;
    unsigned short	DevicePort;
    unsigned short	TcpPort;
    unsigned short	UdpPort;
    unsigned short	UploadPort;
    unsigned short	MulticastPort;
    unsigned char	NetworkMode;
    char	DDNS[128];
    /*----------------------------*/
    char Alias[32];
    char NewModelName[32];
    unsigned char	ModelType; //0x00 ~ 0x03, camera, encoder, decoder, NVR
    unsigned short	Version;
    unsigned char	HttpMode; // http or https
    unsigned short	HttpsPort;
    unsigned char	SupportedProtocol;
    unsigned char	PasswordStatus;
};

struct SUNAPI_DATA_PACKET_RSA
{
    unsigned char	Mode;
    char			PacketID[18];
    char			MacAddress[18];
    char			IPAddress[16];
    char			SubnetMask[16];
    char			Gateway[16];
    char			Reserved[10];
    unsigned short	PayloadSize;
    char			Payload[512];
};

struct SUNAPI_DATA_PACKET_PASSWORD
{
    unsigned char	Mode;
    char			PacketID[18];
    char			MacAddress[18];
    char			Reserved[10];
    unsigned short	payloadSize;
    char			Payload[512];
};


QList<QNetworkAddressEntry> GetDiscoverySources();
QList<QNetworkAddressEntry> GetDiscoveryIPv6Sources();

}
}

#include "SipConnector.h"
#include "MacConverter.h"
#include "ExceptionCode.h"
#include <algorithm>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <LogSettings.h>
#include <boost/iostreams/stream.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

SipConnector::SipConnector()
{
    m_currentLocalPort = DEFAULT_START_LOCAL_PORT;

    std::string localIp = "127.0.0.1";
    if (getAvailableLocalIP(localIp))
    {
        config.emplace("local_ip", localIp);
        SPDLOG_INFO("Local IP has been set successfully: {}", localIp);
    }

    config.emplace("turn_server", PRT_TURN_SERVER);
    config.emplace("sip_server", PRT_SIP_SERVER);

    SIPPROXY_SetLogLevel("", 0x08, [](char* log) {
        SPDLOG_DEBUG("[SIPPROXY] ---- {}", log);
        });
}

SipConnector::~SipConnector()
{
    std::for_each(deviceMap.begin(), deviceMap.end(),
        [](std::pair<std::string, std::shared_ptr<SipDevice>> kvp)
        {
            SIPPROXY_StopServer(kvp.second->proxyHandle);
            SIPPROXY_StopServer(kvp.second->httpsProxyHandle);
            SIPPROXY_Close(kvp.second->ecHandle);
        });

    deviceMap.clear();
}

bool SipConnector::ConnectDevice(const std::string& macAddress, unsigned short& localPort, unsigned short& localHttpsPort, short& connectionType)
{
    // Check duplication
    if (this->deviceMap.find(macAddress) != this->deviceMap.end())
    {
        auto existDevice = deviceMap[macAddress];
        localPort = existDevice->localPort;
        localHttpsPort = existDevice->localHttpsPort;

        SPDLOG_INFO("Exist proxy mac: {}, port: {}", macAddress, localPort, localHttpsPort);
        restartServer(existDevice, connectionType);

        return true;
    }

    // Make new session
    std::shared_ptr<SipDevice> device = std::make_shared<SipDevice>();
    device->macAddress = macAddress;

    // Generate UID
    std::string uid = convertUid(device->macAddress);

    try
    {
        device->ecHandle = SIPPROXY_Open(uid, config);
    }
    catch (const legosip::Exception_t& e)
    {
        SPDLOG_ERROR("SIPPROXY_Open ERROR : {}", e);
        return false;
    }

    if (device->ecHandle == nullptr)
    {
        SPDLOG_ERROR("SIPPROXY_Open failed");
        return false;
    }

    connectionType = (short)SIPPROXY_GetConnectionType(device->ecHandle);
    SPDLOG_INFO("ConnectionType: {}", connectionType);

    std::string deviceInfo;
    std::string deviceID = "";

    SIPPROXY_GetDeviceInfo(uid, deviceID, deviceInfo);

    SPDLOG_INFO("DeviceInfo: {}", deviceInfo);

    localPort = getNextPort();
    localHttpsPort = getNextPort();

    unsigned short remotePort = 0;
    unsigned short remoteHttpsPort = 0;

    if (deviceInfo == "")
    {
        remotePort = DEFAULT_S1_DEVICE_PORT;
        remoteHttpsPort = DEFAULT_S1_HTTPS_PORT;
    }
    else
        parseDeviceInfoJson(deviceInfo, remotePort, remoteHttpsPort);

    device->proxyHandle = SIPPROXY_StartServer(device->ecHandle, deviceID, localPort, remotePort);
    if (device->proxyHandle == nullptr)
    {
        SPDLOG_ERROR("SIPPROXY_StartServer failed (HTTP)");
        return false;
    }

    device->httpsProxyHandle = SIPPROXY_StartServer(device->ecHandle, deviceID, localHttpsPort, remoteHttpsPort);
    if (device->httpsProxyHandle == nullptr)
    {
        SPDLOG_ERROR("SIPPROXY_StartServer failed (HTTPS)");
        return false;
    }

    device->localPort = localPort;
    device->localHttpsPort = localHttpsPort;
    SPDLOG_INFO("Proxy started - local port: {}, local https: {}, http port: {}, https port: {}", localPort, localHttpsPort, remotePort, remoteHttpsPort);

    this->deviceMap.emplace(device->macAddress, device);

    return true;
}

bool SipConnector::DIsconnectDevice(const std::string& macAddress)
{
    if (this->deviceMap.find(macAddress) != this->deviceMap.end())
    {
        auto device = deviceMap[macAddress];
        SIPPROXY_StopServer(device->proxyHandle);
        SIPPROXY_StopServer(device->httpsProxyHandle);
        SIPPROXY_Close(device->ecHandle);

        deviceMap.erase(macAddress);

        return true;
    }
    else
    {
        return false;
    }
}

bool SipConnector::restartServer(std::shared_ptr<SipDevice> device, short& connectionType)
{
    SIPPROXY_StopServer(device->proxyHandle);
    SIPPROXY_StopServer(device->httpsProxyHandle);
    SIPPROXY_Close(device->ecHandle);

    // Generate UID
    std::string uid = convertUid(device->macAddress);

    try
    {
        device->ecHandle = SIPPROXY_Open(uid, config);
    }
    catch (const legosip::Exception_t& e)
    {
        SPDLOG_ERROR("restartServer() SIPPROXY_Open ERROR : {}", e);
        return false;
    }

    if (device->ecHandle == nullptr)
    {
        SPDLOG_ERROR("restartServer() SIPPROXY_Open failed");
        return false;
    }

    connectionType = (short)SIPPROXY_GetConnectionType(device->ecHandle);
    SPDLOG_INFO("restartServer() ConnectionType: {}", connectionType);

    std::string deviceInfo;
    std::string deviceID = "";

    SIPPROXY_GetDeviceInfo(uid, deviceID, deviceInfo);

    SPDLOG_INFO("restartServer() DeviceInfo: {}", deviceInfo);

    unsigned short remotePort = 0;
    unsigned short remoteHttpsPort = 0;

    if (deviceInfo == "")
        remotePort = DEFAULT_S1_DEVICE_PORT;
    else
        parseDeviceInfoJson(deviceInfo, remotePort, remoteHttpsPort);

    device->proxyHandle = SIPPROXY_StartServer(device->ecHandle, deviceID, device->localPort, remotePort);
    if (device->proxyHandle == nullptr)
    {
        SPDLOG_ERROR("restartServer() SIPPROXY_StartServer failed (HTTP)");
        return false;
    }

    device->httpsProxyHandle = SIPPROXY_StartServer(device->ecHandle, deviceID, device->localHttpsPort, remoteHttpsPort);
    if (device->httpsProxyHandle == nullptr)
    {
        SPDLOG_ERROR("restartServer() SIPPROXY_StartServer failed (HTTPS)");
        return false;
    }

    SPDLOG_INFO("Proxy restarted - local port: {}, local https: {}, http port: {}, https port: {}", device->localPort, device->localHttpsPort, remotePort, remoteHttpsPort);
}

unsigned short SipConnector::getNextPort()
{
    unsigned short retPort = m_currentLocalPort++;

    if (m_currentLocalPort > DEFAULT_END_LOCAL_PORT)
    {
        SPDLOG_INFO("Local port overflow: {}", m_currentLocalPort);
        m_currentLocalPort = DEFAULT_START_LOCAL_PORT;
    }

    return retPort;
}

std::string SipConnector::convertUid(std::string& macAddress) const
{
    std::string serialNumber = generateSerialNumber(macAddress);
    std::string domainID = serialNumber + ".NVR.TECHWIN";

    return "sips:" + domainID + "@s1.co.kr";
}

bool SipConnector::getAvailableLocalIP(std::string& address) const
{
    const char* google_dns_server = "8.8.8.8";
    int dns_port = 53;
    bool ret = false;
    char buffer[100] = { 0 };
    struct sockaddr_in serv;
    struct sockaddr_in name;
    socklen_t namelen;
    SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock == INVALID_SOCKET)
    {
        goto ErrBlock;
    }

    memset(&serv, 0, sizeof(serv));
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr(google_dns_server);
    serv.sin_port = htons(dns_port);

    if (connect(sock, (const struct sockaddr*)&serv, sizeof(serv)) == SOCKET_ERROR)
    {
        goto ErrBlock;
    }

    namelen = sizeof(name);
    if (getsockname(sock, (struct sockaddr*)&name, &namelen) == SOCKET_ERROR)
    {
        goto ErrBlock;
    }

    if (inet_ntop(AF_INET, &name.sin_addr, buffer, 80) != nullptr)
    {
        address = buffer;
        ret = true;
    }

ErrBlock:
    closesocket(sock);
    return ret;
}

std::string SipConnector::generateSerialNumber(std::string& macAddress) const
{
    alllink::CMacConverter converter;
    macAddress.erase(std::remove(macAddress.begin(), macAddress.end(), ':'), macAddress.end());  // remove ":" from "11:aa:22:bb:33:cc" string.

    return converter.ConvertToSN(macAddress.c_str());
}

bool SipConnector::parseDeviceInfoJson(const std::string& deviceInfo, unsigned short& devicePort, unsigned short& httpsPort)
{
    try
    {
        boost::property_tree::ptree props;
        boost::iostreams::stream<boost::iostreams::array_source> stream(deviceInfo.c_str(), deviceInfo.size());
        boost::property_tree::read_json(stream, props);

        devicePort = props.get<unsigned short>("http_port", 1835);
        httpsPort = props.get<unsigned short>("https_port", 1836);
    }
    catch (std::exception& e)
    {
        SPDLOG_ERROR("parseDeviceInfoJson exception: {}", e.what());
        return false;
    }

    return true;
}
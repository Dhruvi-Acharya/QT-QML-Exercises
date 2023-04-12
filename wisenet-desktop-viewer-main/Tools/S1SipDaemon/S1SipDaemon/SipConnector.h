#pragma once

#include "sipproxy.h"
#include <memory>

#define PRT_TURN_SERVER "melbsip.s1.co.kr:443"	// get real commercial turn servier via melbsip server
#define PRT_NS_SERVER	"mnssip.s1.co.kr:9091"
#define PRT_SIP_SERVER	"13.124.64.153:5061"	// Don't use PRT_SIP_SERVER directly. You should get SIP_SERVER Address from NS_SERVER.
#define DEFAULT_S1_DEVICE_PORT 1835
#define DEFAULT_S1_HTTPS_PORT 1836
#define DEFAULT_START_LOCAL_PORT 18000
#define DEFAULT_END_LOCAL_PORT 19999
struct SipDevice {
    std::string macAddress;
    EC_HANDLE ecHandle;
    SIPPROXY_HANDLE proxyHandle;
    SIPPROXY_HANDLE httpsProxyHandle;
    unsigned short localPort;
    unsigned short localHttpsPort;
};

class SipConnector
{
public:
    SipConnector();
    ~SipConnector();

    bool ConnectDevice(const std::string& macAddress, unsigned short& localPort, unsigned short& localHttpsPort, short& connectionType);
    bool DIsconnectDevice(const std::string& macAddress);
private:
    unsigned short getNextPort();
    std::string convertUid(std::string& macAddress) const;
    std::string generateSerialNumber(std::string& macAddress) const;
    bool parseDeviceInfoJson(const std::string& deviceInfo, unsigned short& devicePort, unsigned short& httpsPort);
    bool getAvailableLocalIP(std::string& address)const;
    bool restartServer(std::shared_ptr<SipDevice> sipDevice, short& connectionType);

private:
    std::map<std::string, std::string> config;
    std::map<std::string, std::shared_ptr<SipDevice>> deviceMap;

    unsigned short m_currentLocalPort = 0;
};


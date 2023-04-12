#pragma once
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <deque>
#include <ThreadPool.h>

const int MAX_RECEIVE_BUFFER_LEN = 512;
const int MAX_MESSAGE_LEN = 129;
const int MAC_LENGTH = 6;

struct PACKET_HEADER
{
    short nCommand;
    short nSize;
};

const short REQ_SIP_CONNECT = 6;
const short RES_SIP_CONNECT = 7;
const short REQ_SIP_DISCONNECT = 8;
const short REQ_STOP_SERVICE = 9;

struct PKT_REQ_SIP_CONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_SIP_CONNECT;
        nSize = sizeof(PKT_REQ_SIP_CONNECT);
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned char szMac[MAC_LENGTH];
};

struct PKT_REQ_SIP_DISCONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_SIP_DISCONNECT;
        nSize = sizeof(PKT_REQ_SIP_DISCONNECT);
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned char szMac[MAC_LENGTH];
};

struct PKT_RES_SIP_CONNECT : public PACKET_HEADER
{
    void Init()
    {
        nCommand = RES_SIP_CONNECT;
        nSize = sizeof(PKT_RES_SIP_CONNECT);
        nPort = 0;
        memset(szMac, 0, MAC_LENGTH);
    }

    unsigned short nPort;
    unsigned short nHttpsPort;
    short nConnectionType;
    unsigned char szMac[MAC_LENGTH];
};

struct PKT_REQ_STOP_SERVICE : public PACKET_HEADER
{
    void Init()
    {
        nCommand = REQ_STOP_SERVICE;
        nSize = sizeof(PKT_REQ_STOP_SERVICE);
    }
};

class S1Sip : public std::enable_shared_from_this<S1Sip>
{
public:
    S1Sip(boost::asio::io_context& ioContext, unsigned short port);
    ~S1Sip();

    void GetPort(std::string& mac, unsigned short& http, unsigned short& https, short& connectionType);
    void Disconnect();
    void StopService();

private:
    void connect(boost::asio::ip::tcp::endpoint endpoint);
    void close();
    void processPacket(const char* pData, unsigned short& http, unsigned short& https, short& connectionType);

    void convertMacAddress(const char* src, unsigned char* target);
    int char2int(char input);

private:
    boost::asio::io_context& m_ioContext;
    boost::asio::ip::tcp::socket m_socket;
    std::string m_macAddress;

    std::deque<char*> m_SendDataQueue;
    unsigned short m_s1DaemonPort = 1881;
};

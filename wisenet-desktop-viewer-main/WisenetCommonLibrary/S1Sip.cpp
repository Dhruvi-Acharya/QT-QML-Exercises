#include "S1Sip.h"
#include "LogSettings.h"
#include "WeakCallback.h"
#include <QDebug>

S1Sip::S1Sip(boost::asio::io_context& ioContext, unsigned short port)
    : m_ioContext(ioContext),
      m_socket(ioContext),
      m_s1DaemonPort(port)
{
}

S1Sip::~S1Sip()
{
}

void S1Sip::GetPort(std::string& mac, unsigned short& http, unsigned short& https, short& connectionType)
{
    auto endpoint = boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), m_s1DaemonPort);


    boost::system::error_code connect_error;
    m_socket.connect(endpoint, connect_error);

    if (connect_error)
        SPDLOG_ERROR("Connection failed - error No: {}, Message: {}", connect_error.value(), connect_error.message());
    else
        SPDLOG_INFO("Connection success");


    SPDLOG_INFO("SIP connnect 1");
    PKT_REQ_SIP_CONNECT sendPkt;
    sendPkt.Init();
    convertMacAddress(mac.c_str(), sendPkt.szMac);
    char* pSendData = nullptr;
    pSendData = new char[sendPkt.nSize];
    memcpy(pSendData, &sendPkt, sendPkt.nSize);

    boost::system::error_code ignored_error;
    m_socket.write_some(boost::asio::buffer(pSendData, sendPkt.nSize), ignored_error);
    SPDLOG_INFO("SIP connnect 2");

    char recv_buf[1024];
    boost::system::error_code recv_error;
    std::size_t len = m_socket.read_some(boost::asio::buffer(recv_buf), recv_error);
    SPDLOG_INFO("SIP connnect 3");

    if (recv_error)
    {
        if (recv_error == boost::asio::error::eof)
            SPDLOG_ERROR("Disconnected with server");
        else
            SPDLOG_ERROR("Receive error - error No: {}, Message: {}", recv_error.value(), recv_error.message());

        close();
        return;
    }

    m_macAddress = mac;
    processPacket(recv_buf, http, https, connectionType);
    close();

    SPDLOG_INFO("S1Sip::GetPort: mac{}, http{}, https{}", mac, http, https);
}

void S1Sip::Disconnect()
{
    auto endpoint = boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), m_s1DaemonPort);


    boost::system::error_code connect_error;
    m_socket.connect(endpoint, connect_error);

    if (connect_error)
        SPDLOG_ERROR("Connection failed - error No: {}, Message: {}", connect_error.value(), connect_error.message());
    else
        SPDLOG_INFO("Connection success");


    SPDLOG_INFO("SIP disconnect 1");
    PKT_REQ_SIP_DISCONNECT sendPkt;
    sendPkt.Init();
    convertMacAddress(m_macAddress.c_str(), sendPkt.szMac);
    char* pSendData = nullptr;
    pSendData = new char[sendPkt.nSize];
    memcpy(pSendData, &sendPkt, sendPkt.nSize);

    boost::system::error_code ignored_error;
    m_socket.write_some(boost::asio::buffer(pSendData, sendPkt.nSize), ignored_error);
    SPDLOG_INFO("SIP disconnect 2");

    close();
}

void S1Sip::StopService()
{
    auto endpoint = boost::asio::ip::tcp::endpoint(
                boost::asio::ip::address::from_string("127.0.0.1"), m_s1DaemonPort);


    boost::system::error_code connect_error;
    m_socket.connect(endpoint, connect_error);

    if (connect_error)
        SPDLOG_ERROR("Connection failed - error No: {}, Message: {}", connect_error.value(), connect_error.message());
    else
        SPDLOG_INFO("Connection success");


    SPDLOG_INFO("SIP stop service 1");
    PKT_REQ_STOP_SERVICE sendPkt;
    sendPkt.Init();
    char* pSendData = nullptr;
    pSendData = new char[sendPkt.nSize];
    memcpy(pSendData, &sendPkt, sendPkt.nSize);

    boost::system::error_code ignored_error;
    m_socket.write_some(boost::asio::buffer(pSendData, sendPkt.nSize), ignored_error);
    SPDLOG_INFO("SIP stop service 2");

    close();
}


void S1Sip::close()
{
    if (m_socket.is_open())
    {
        m_socket.close();
    }
}


void S1Sip::convertMacAddress(const char* src, unsigned char* target)
{
    while (*src && src[1])
    {
        *(target++) = char2int(*src) * 16 + char2int(src[1]);
        src += 2;
    }
}

int S1Sip::char2int(char input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

void S1Sip::processPacket(const char* pData, unsigned short& http, unsigned short& https, short& connectionType)
{
    PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

    switch (pheader->nCommand)
    {
    case RES_SIP_CONNECT:
    {
        PKT_RES_SIP_CONNECT* pPacket = (PKT_RES_SIP_CONNECT*)pData;

        SPDLOG_INFO("SIP connect response: {}, {}, {}", pPacket->nPort, pPacket->nHttpsPort, pPacket->nConnectionType);
        http = pPacket->nPort;
        https = pPacket->nHttpsPort;
        connectionType = pPacket->nConnectionType;
    }
        break;
    }
}

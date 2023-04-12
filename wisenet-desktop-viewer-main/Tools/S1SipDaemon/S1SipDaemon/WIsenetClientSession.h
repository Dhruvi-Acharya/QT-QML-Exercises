#pragma once
#include <deque>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "Protocol.h"

class SipControlServer;

class WIsenetClientSession
{
public:
    WIsenetClientSession(std::string& sessionID, boost::asio::io_context& io_context, SipControlServer* pServer);
    ~WIsenetClientSession();

    std::string SessionID() { return m_sessionID; }

    boost::asio::ip::tcp::socket& Socket() { return m_socket; }

    void Init();

    void PostReceive();
    void PostSend(const bool bImmediately, const int nSize, char* pData);

private:
    void handle_write(const boost::system::error_code& error, size_t bytes_transferred);
    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred);

    std::string m_sessionID;
    boost::asio::ip::tcp::socket m_socket;

    std::array<char, MAX_RECEIVE_BUFFER_LEN> m_ReceiveBuffer;

    int m_nPacketBufferMark;
    char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];

    std::deque<char*> m_SendDataQueue;
    SipControlServer* m_pServer;
};
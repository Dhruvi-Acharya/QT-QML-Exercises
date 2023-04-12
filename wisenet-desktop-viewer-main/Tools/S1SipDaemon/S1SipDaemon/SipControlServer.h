#pragma once 

#include <map>
#include <memory>
#include <boost/core/ignore_unused.hpp>
#include "WIsenetClientSession.h"
#include "SipConnector.h"
#include "Protocol.h"

class SipControlServer
{
public:
    SipControlServer(boost::asio::io_context& io_context, unsigned short portNumber);
    ~SipControlServer();

    void Start();

    void CloseSession(const std::string& sessionID);
    void ProcessPacket(const std::string& sessionID, const char* pData);

private:
    bool PostAccept();
    void handle_accept(std::shared_ptr<WIsenetClientSession> pSession, const boost::system::error_code& error);
    std::string generateUuid();

private:
    boost::asio::ip::tcp::acceptor m_acceptor;

    std::map<std::string, std::shared_ptr<WIsenetClientSession>> m_sessionMap;
    std::deque<int> m_SessionQueue;

    SipConnector m_sipConnector;
    unsigned short m_portNumber = 1881;
};
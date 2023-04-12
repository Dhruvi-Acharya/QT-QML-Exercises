#include <LogSettings.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "SipControlServer.h"

SipControlServer::SipControlServer(boost::asio::io_context& io_context, unsigned short portNumber)
    : m_acceptor(io_context, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), portNumber))
    , m_portNumber(portNumber)
{
    SPDLOG_INFO("SipControlServer() Server port: {}", portNumber);
}

SipControlServer::~SipControlServer()
{
    std::for_each(m_sessionMap.begin(), m_sessionMap.end(),
        [](std::pair<std::string, std::shared_ptr<WIsenetClientSession>> kvp)
        {
            if (kvp.second->Socket().is_open())
            {
                kvp.second->Socket().close();
            }
        });

    m_sessionMap.clear();
    SPDLOG_INFO("~SipControlServer()");
}

std::string SipControlServer::generateUuid()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();

    return boost::uuids::to_string(uuid);
}

void SipControlServer::Start()
{
    SPDLOG_INFO("Server started....");

    PostAccept();
}

void SipControlServer::CloseSession(const std::string& sessionID)
{
    SPDLOG_INFO("Client connection ended. Session ID: {}", sessionID);

    m_sessionMap[sessionID]->Socket().close();
    m_sessionMap.erase(sessionID);
}

void SipControlServer::ProcessPacket(const std::string& sessionID, const char* pData)
{
    PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

    switch (pheader->nCommand)
    {
    case REQ_SIP_CONNECT:
    {
        PKT_REQ_SIP_CONNECT* pPacket = (PKT_REQ_SIP_CONNECT*)pData;

        char cMac[13];
        std::string mac;
        snprintf(cMac, sizeof(cMac), "%02X%02X%02X%02X%02X%02X", pPacket->szMac[0], pPacket->szMac[1], pPacket->szMac[2], pPacket->szMac[3], pPacket->szMac[4], pPacket->szMac[5]);
        mac = cMac;

        unsigned short port = 0;
        unsigned short httpsPort = 0;
        short connectionType = 0;

        bool ret = this->m_sipConnector.ConnectDevice(mac, port, httpsPort, connectionType);
        SPDLOG_INFO("SIP Connect - MAC: {}, Port: {}, Https: {}, ConnectionType: {}, ret: {}", mac, port, httpsPort, connectionType, ret);

        PKT_RES_SIP_CONNECT SendPkt;
        SendPkt.Init();
        memcpy_s(SendPkt.szMac, MAC_LENGTH, pPacket->szMac, MAC_LENGTH);
        SendPkt.nPort = port;
        SendPkt.nHttpsPort = httpsPort;
        SendPkt.nConnectionType = connectionType;

        if (ret == false)
        {
            SendPkt.nPort = 0;
            SendPkt.nHttpsPort = 0;
            SendPkt.nConnectionType = -1;
        }

        m_sessionMap[sessionID]->PostSend(false, SendPkt.nSize, (char*)&SendPkt);
        break;
    }
    case REQ_SIP_DISCONNECT:
    {
        PKT_REQ_SIP_DISCONNECT* pPacket = (PKT_REQ_SIP_DISCONNECT*)pData;

        char cMac[13];
        std::string mac;
        snprintf(cMac, sizeof(cMac), "%02X%02X%02X%02X%02X%02X", pPacket->szMac[0], pPacket->szMac[1], pPacket->szMac[2], pPacket->szMac[3], pPacket->szMac[4], pPacket->szMac[5]);
        mac = cMac;

        bool ret = this->m_sipConnector.DIsconnectDevice(mac);
        SPDLOG_INFO("SIP Disconnect - MAC: {}, ret: {}", mac, ret);
        break;
    }
    case REQ_STOP_SERVICE:
    {
        SPDLOG_INFO("STOP SERVICE");
        m_acceptor.close();
        break;
    }
    }

    return;
}

bool SipControlServer::PostAccept()
{
    std::string uuid = generateUuid();

    auto session = std::make_shared<WIsenetClientSession>(uuid, (boost::asio::io_context&)(m_acceptor).get_executor().context(), this);
    m_sessionMap.emplace(uuid, session);

    m_acceptor.async_accept(session->Socket(),
        boost::bind(&SipControlServer::handle_accept,
            this,
            session,
            boost::asio::placeholders::error)
    );

    return true;
}

void SipControlServer::handle_accept(std::shared_ptr<WIsenetClientSession> pSession, const boost::system::error_code& error)
{
    if (!error)
    {
        SPDLOG_INFO("handle_accept() Client connect success. SessionID: {}", pSession->SessionID());

        pSession->Init();
        pSession->PostReceive();

        PostAccept();
    }
    else
    {
        SPDLOG_INFO("handle_accept() Error No: {}  error Message: {}", error.value(), error.message());
    }
}
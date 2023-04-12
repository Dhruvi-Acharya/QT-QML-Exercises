#include "WIsenetClientSession.h"
#include "SipControlServer.h"
#include <LogSettings.h>

WIsenetClientSession::WIsenetClientSession(std::string& sessionID, boost::asio::io_context& io_context, SipControlServer* pServer)
    : m_socket(io_context)
    , m_sessionID(sessionID)
    , m_pServer(pServer)
{
    SPDLOG_INFO("WIsenetClientSession() {}", sessionID);
}

WIsenetClientSession::~WIsenetClientSession()
{
    while (m_SendDataQueue.empty() == false)
    {
        delete[] m_SendDataQueue.front();
        m_SendDataQueue.pop_front();
    }

    SPDLOG_INFO("~WIsenetClientSession() {}", m_sessionID);
}

void WIsenetClientSession::Init()
{
    m_nPacketBufferMark = 0;
}

void WIsenetClientSession::PostReceive()
{
    m_socket.async_read_some
    (
        boost::asio::buffer(m_ReceiveBuffer),
        boost::bind(&WIsenetClientSession::handle_receive, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
    );
}

void WIsenetClientSession::PostSend(const bool bImmediately, const int nSize, char* pData)
{
    char* pSendData = nullptr;

    if (bImmediately == false)
    {
        pSendData = new char[nSize];
        memcpy(pSendData, pData, nSize);

        m_SendDataQueue.push_back(pSendData);
    }
    else
    {
        pSendData = pData;
    }

    if (bImmediately == false && m_SendDataQueue.size() > 1)
    {
        return;
    }

    boost::asio::async_write(m_socket, boost::asio::buffer(pSendData, nSize),
        boost::bind(&WIsenetClientSession::handle_write, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred)
    );
}

void WIsenetClientSession::handle_write(const boost::system::error_code& error, size_t bytes_transferred)
{
    delete[] m_SendDataQueue.front();
    m_SendDataQueue.pop_front();

    if (m_SendDataQueue.empty() == false)
    {
        char* pData = m_SendDataQueue.front();

        PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;

        PostSend(true, pHeader->nSize, pData);
    }
}

void WIsenetClientSession::handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (error)
    {
        if (error == boost::asio::error::eof)
        {
            SPDLOG_INFO("Client disconnected");
        }
        else
        {
            SPDLOG_INFO("error No: {}  error Message : {}", error.value(), error.message());
        }

        m_pServer->CloseSession(m_sessionID);
    }
    else
    {
        memcpy(&m_PacketBuffer[m_nPacketBufferMark], m_ReceiveBuffer.data(), bytes_transferred);

        int nPacketData = m_nPacketBufferMark + bytes_transferred;
        int nReadData = 0;

        while (nPacketData > 0)
        {
            if (nPacketData < sizeof(PACKET_HEADER))
            {
                break;
            }

            PACKET_HEADER* pHeader = (PACKET_HEADER*)&m_PacketBuffer[nReadData];

            if (pHeader->nSize <= nPacketData)
            {
                m_pServer->ProcessPacket(m_sessionID, &m_PacketBuffer[nReadData]);

                nPacketData -= pHeader->nSize;
                nReadData += pHeader->nSize;
            }
            else
            {
                break;
            }
        }

        if (nPacketData > 0)
        {
            char TempBuffer[MAX_RECEIVE_BUFFER_LEN] = { 0, };
            memcpy(&TempBuffer[0], &m_PacketBuffer[nReadData], nPacketData);
            memcpy(&m_PacketBuffer[0], &TempBuffer[0], nPacketData);
        }

        m_nPacketBufferMark = nPacketData;
        PostReceive();
    }
}


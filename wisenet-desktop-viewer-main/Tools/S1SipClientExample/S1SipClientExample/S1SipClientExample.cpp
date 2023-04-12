#include <deque>
#include <boost/bind/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <Protocol.h>

using namespace std;

class S1SipClient
{
public:
    S1SipClient(boost::asio::io_service& io_service)
        : m_IOService(io_service),
        m_Socket(io_service)
    {
        m_bIsLogin = false;
        InitializeCriticalSectionAndSpinCount(&m_lock, 4000);
    }

    ~S1SipClient()
    {
        EnterCriticalSection(&m_lock);

        while (m_SendDataQueue.empty() == false)
        {
            delete[] m_SendDataQueue.front();
            m_SendDataQueue.pop_front();
        }

        LeaveCriticalSection(&m_lock);

        DeleteCriticalSection(&m_lock);
    }

    bool IsConnecting() { return m_Socket.is_open(); }

    void LoginOK() { m_bIsLogin = true; }
    bool IsLogin() { return m_bIsLogin; }

    void Connect(boost::asio::ip::tcp::endpoint endpoint)
    {
        m_nPacketBufferMark = 0;

        m_Socket.async_connect(endpoint,
            boost::bind(&S1SipClient::handle_connect, this,
                boost::asio::placeholders::error)
        );
    }

    void Close()
    {
        if (m_Socket.is_open())
        {
            m_Socket.close();
        }
    }

    void PostSend(const bool bImmediately, const int nSize, char* pData)
    {
        char* pSendData = nullptr;

        EnterCriticalSection(&m_lock);		// 락 시작

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

        if (bImmediately || m_SendDataQueue.size() < 2)
        {
            boost::asio::async_write(m_Socket, boost::asio::buffer(pSendData, nSize),
                boost::bind(&S1SipClient::handle_write, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)
            );
        }

        LeaveCriticalSection(&m_lock);		// 락 완료
    }

    void ConvertMacAddress(const char* src, unsigned char* target)
    {
        while (*src && src[1])
        {
            *(target++) = char2int(*src) * 16 + char2int(src[1]);
            src += 2;
        }
    }

    int char2int(char input)
    {
        if (input >= '0' && input <= '9')
            return input - '0';
        if (input >= 'A' && input <= 'F')
            return input - 'A' + 10;
        if (input >= 'a' && input <= 'f')
            return input - 'a' + 10;
        throw std::invalid_argument("Invalid input string");
    }

private:

    void PostReceive()
    {
        memset(&m_ReceiveBuffer, '\0', sizeof(m_ReceiveBuffer));

        m_Socket.async_read_some
        (
            boost::asio::buffer(m_ReceiveBuffer),
            boost::bind(&S1SipClient::handle_receive, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred)

        );
    }

    void handle_connect(const boost::system::error_code& error)
    {
        if (!error)
        {
            std::cout << "Connected succesfully" << std::endl;
            std::cout << "Enter commands" << std::endl;

            PostReceive();
        }
        else
        {
            std::cout << "Cannot connect to server. error No: " << error.value() << " error Message: " << error.message() << std::endl;
        }
    }

    void handle_write(const boost::system::error_code& error, size_t bytes_transferred)
    {
        EnterCriticalSection(&m_lock);			// 락 시작

        delete[] m_SendDataQueue.front();
        m_SendDataQueue.pop_front();

        char* pData = nullptr;

        if (m_SendDataQueue.empty() == false)
        {
            pData = m_SendDataQueue.front();
        }

        LeaveCriticalSection(&m_lock);			// 락 완료


        if (pData != nullptr)
        {
            PACKET_HEADER* pHeader = (PACKET_HEADER*)pData;
            PostSend(true, pHeader->nSize, pData);
        }
    }

    void handle_receive(const boost::system::error_code& error, size_t bytes_transferred)
    {
        if (error)
        {
            if (error == boost::asio::error::eof)
            {
                std::cout << "Disconnected" << std::endl;
            }
            else
            {
                std::cout << "error No: " << error.value() << " error Message: " << error.message() << std::endl;
            }

            Close();
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
                    ProcessPacket(&m_PacketBuffer[nReadData]);

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

    void ProcessPacket(const char* pData)
    {
        PACKET_HEADER* pheader = (PACKET_HEADER*)pData;

        switch (pheader->nCommand)
        {
        case RES_SIP_CONNECT:
        {
            PKT_RES_SIP_CONNECT* pPacket = (PKT_RES_SIP_CONNECT*)pData;

            std::cout << "SIP connect response: " << pPacket->nPort << " " << pPacket->nHttpsPort << " " << pPacket->nConnectionType << std::endl;
        }
        break;
        }
    }

private:
    boost::asio::io_service& m_IOService;
    boost::asio::ip::tcp::socket m_Socket;

    std::array<char, 512> m_ReceiveBuffer;

    int m_nPacketBufferMark;
    char m_PacketBuffer[MAX_RECEIVE_BUFFER_LEN * 2];

    CRITICAL_SECTION m_lock;
    std::deque< char* > m_SendDataQueue;

    bool m_bIsLogin;
};

int main()
{
    boost::asio::io_service io_service;

    auto endpoint = boost::asio::ip::tcp::endpoint(
        boost::asio::ip::address::from_string("127.0.0.1"),
        PORT_NUMBER);

    S1SipClient Client(io_service);
    Client.Connect(endpoint);

    boost::thread thread(boost::bind(&boost::asio::io_service::run, &io_service));

    char szInputMessage[MAX_MESSAGE_LEN * 2] = { 0, };

    while (std::cin.getline(szInputMessage, MAX_MESSAGE_LEN))
    {
        if (strnlen_s(szInputMessage, MAX_MESSAGE_LEN) == 0)
        {
            break;
        }

        if (Client.IsConnecting() == false)
        {
            std::cout << "Can't connet with server" << std::endl;
            continue;
        }

        std::string command = szInputMessage;

        std::string mac = "0009184C0AAE";
        //std::string mac = "0009184C0B08";
        //std::string mac = "007F334F619B";
        //std::string mac = "0009184C0B70";

        if (command == "1")
        {
            PKT_REQ_SIP_CONNECT SendPkt;
            SendPkt.Init();

            Client.ConvertMacAddress(mac.c_str(), SendPkt.szMac);
            Client.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
        }
        else if (command == "2")
        {
            PKT_REQ_SIP_DISCONNECT SendPkt;
            SendPkt.Init();

            Client.ConvertMacAddress(mac.c_str(), SendPkt.szMac);
            Client.PostSend(false, SendPkt.nSize, (char*)&SendPkt);
        }
    }

    io_service.stop();
    Client.Close();

    thread.join();

    std::cout << "Exit client" << std::endl;

    return 0;
}
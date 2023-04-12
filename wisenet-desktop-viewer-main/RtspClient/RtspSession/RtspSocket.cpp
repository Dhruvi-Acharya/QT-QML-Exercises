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

#include <iostream>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "LogSettings.h"
#include "WeakCallback.h"
#include "RtspSocket.h"


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-SOCK] "}, level, __VA_ARGS__)

namespace Wisenet
{
namespace Rtsp
{

namespace status_codes
{
#define _RTSP_PHRASES
#define DAT(a,b,c) const static unsigned short a = b;
#include "RtspConstants.ipp"
#undef _RTSP_PHRASES
#undef DAT
}

const bool SocketWasClosed(const boost::system::error_code& error)
{
    return ((boost::asio::error::eof == error)
            || (boost::asio::error::connection_reset == error)
            || (boost::asio::error::connection_aborted == error)
            || (boost::asio::error::connection_refused == error));
}

const bool OperationAborted(const boost::system::error_code& error)
{
    return (boost::asio::error::operation_aborted == error);
}


void RtspSocket::WriteRequestLog(const std::string& request)
{
    SPDLOG_DEBUG("[RTSP REQUEST]::{}\n{}\n", m_url, request);
}

void RtspSocket::WriteResponseLog(const std::string& response)
{
    SPDLOG_DEBUG("[RTSP RESPONSE]::{}\n{}\n",  m_url, response) ;
}


RtspSocket::RtspSocket(boost::asio::io_context& ioc,
                       const std::string &rtspUrl,
                       const std::string &userName,
                       const std::string &password)
    : m_ioContext(ioc)
    , m_strand(ioc.get_executor())
    , m_url(rtspUrl)
    , m_userName(userName)
    , m_password(password)
    , m_timeoutTimer(ioc)
    , m_currentRequest(0)
    , m_eventCallback (nullptr)
    , m_userAgentString("Wisenet viewer")
    , m_responseStream(std::make_shared<AsioStreamBuf>())
    , m_timeoutSec(10)
    , m_port(0)
    , m_cSeq(0)
    , m_isSsl(false)
    , m_isBackChannel(false)
    , m_transportMethod(TransportMethod::RTP_RTSP)
    , m_isClosed(false)
{
    SPDLOG_DEBUG("RtspSocket() : {}", m_url);
}

RtspSocket::~RtspSocket()
{
    SPDLOG_DEBUG("~RtspSocket() : {}", m_url);
    //m_eventCallback = nullptr;
}

void RtspSocket::Initialize(const unsigned int timeoutSec,
                            const unsigned short defaultPort,
                            const TransportMethod transportMethod)
{
    m_timeoutSec = timeoutSec;
    m_transportMethod = transportMethod;
    m_cSeq = 1;
    m_isSsl = false;
    m_bindIp = "";
    m_responseStream->Buffer.prepare(64000);

    ParseUrl(m_url, defaultPort, m_strIp, m_port, m_extraUrl);
}

bool RtspSocket::ParseUrl(const std::string &srcUrl, const unsigned short defaultPort,
              std::string &retIp, unsigned short &retPort,
              std::string &retExtraUrl)
{
    retPort = defaultPort;

    // Parse the URL as "rtsp://<server-address-or-name>[:<port>][/<stream-name>]"
    std::string url = srcUrl;
    boost::ierase_first(url, "rtsp://");

    // Parse the IPv6 URL as "rtsp://[<server-address-or-name>]:<port>/<stream-name>"
    boost::char_separator<char> sep("[]");
    boost::tokenizer<boost::char_separator<char> > tok(url, sep);
    std::vector<std::string> ipv6(tok.begin(), tok.end());

    if(ipv6.size() == 2) // IPv6 Case
    {
        retIp = "[" + ipv6[0] + "]";
        boost::ierase_first(url, retIp);

        boost::char_separator<char> sep(":/");
        boost::tokenizer<boost::char_separator<char> > tok(url, sep);
        std::vector<std::string> ports(tok.begin(), tok.end());

        SPDLOG_DEBUG("ports size : {}", ports.size());
        if (ports.size() > 0)
        {
            if(std::count_if(ports[0].begin(), ports[0].end(),
                        [](unsigned char c){ return std::isdigit(c);}
                        ))
            {
                boost::conversion::try_lexical_convert<unsigned short>(ports[0], retPort);
            }
            else
            {
                SPDLOG_ERROR("Invalid retPort {}", ports[0]);
            }
        }
        SPDLOG_DEBUG("retPort {}", retPort);
    }
    else // IPv4 Case
    {
        std::vector<std::string>addresses;
        boost::split(addresses, url, boost::is_any_of("/"));
        if (addresses.size() == 0)
        {
            // invalid address
            return false;
        }

        for (size_t i = 1; i < addresses.size(); i++)
        {
            retExtraUrl += "/" + addresses[i];
        }

        retIp = addresses[0];

        std::vector<std::string> ports;
        boost::split(ports, retIp, boost::is_any_of(":"), boost::token_compress_on);
        if (ports.size() >= 2)
        {
            try {
                boost::trim(ports[1]);
                boost::conversion::try_lexical_convert<unsigned short>(ports[1], retPort);
                retIp = ports[0];
            }
            catch (const boost::bad_lexical_cast&)
            {
                ;
            }
        }
    }

    SPDLOG_DEBUG("Source Url : {}  retIp : {}", srcUrl, retIp);

    return true;
}

void RtspSocket::SetUserAgentString(const std::string& userAgent)
{
    if (!userAgent.empty())
        m_userAgentString = userAgent;
}

void RtspSocket::SetBindIp(const std::string& bindIp)
{
    if (!bindIp.empty())
        m_bindIp = bindIp;
}

void RtspSocket::SetEventCallback(const RtspEventCallbackFunctor& eventCallback)
{
    m_eventCallback = eventCallback;
}

void RtspSocket::SetInterleavedRtpSessionPtr(RtpSessionPtr& rtpSession)
{
    m_rtpMap[rtpSession->RtpChannel()] = rtpSession;
    m_rtpMap[rtpSession->RtcpChannel()] = rtpSession;
}

void RtspSocket::SetBackChannel(bool isBackChannel)
{
    SPDLOG_DEBUG("RtspSocket::SetBackChannel = {}", isBackChannel);
    m_isBackChannel = isBackChannel;
}

bool RtspSocket::IsConnected()
{
    if (!m_sendSocket)
        return false;

    return (m_sendSocket->is_open());
}

/**
 * @brief RtspSocket::Write
 * RTSP SOCKET을 통해 메시지를 전송한다.
 *
 * @param rtspCommand (DESCRIBE, SETUP, PLAY,...)
 * @param url
 * @param extraHeaders
 * @param callback
 */
void RtspSocket::Write(const std::string& rtspCommand,
                       const std::string& url,
                       const std::string& extraHeaders,
                       RtspSocketCallbackFunctor callback)
{
    boost::asio::post(m_strand,
                      WeakCallback(shared_from_this(),
                                   [this, rtspCommand, url, extraHeaders, callback]()
    {
        if (m_isClosed) return;

        /* GET_PARAMETER 명령인 경우에는 connect 된 상태에서만 전송한다. */
        if (rtspCommand == "GET_PARAMETER" && !IsConnected()) {
            SPDLOG_INFO("GET_PARAMETER will be ignored because socket already closed.., url={}", m_url);
            return;
        }

        m_currentRequest = std::make_shared<RtspSocketRequest>(rtspCommand, url, extraHeaders, callback);
        this->WriteStart(m_currentRequest);
    }));
}


void RtspSocket::WriteStart(RtspSocketRequestPtr& request)
{
    std::ostringstream stream;
    MakeRequest(request, stream);

    /* 타이머 시작 */
    StartTimer(request);

    if (!IsConnected())
    {
        Connect();
        return;
    }

    auto buffer = std::make_shared<std::string>(stream.str());
    SPDLOG_TRACE("Write() -- async_write(), REQUEST size={}", buffer->size());
    async_write(boost::asio::buffer(*buffer),
                WeakCallback(shared_from_this(),
                             [buffer, this](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            SPDLOG_TRACE("Write() -- async_write() byteTransferred={}", byteTransferred);
            if (error)
            {
                SPDLOG_INFO("Write() -- async_write() error! error={}, message={}, url={}", error.value(), error.message(), m_url);
                ErrorComplete(error);
                return;
            }
        }));
    }));
}

void RtspSocket::StartTimer(RtspSocketRequestPtr& request)
{
    unsigned int timeoutMsec = (request->Command == "TEARDOWN") ? 500 : m_timeoutSec*1000;

    m_timeoutTimer.expires_after(std::chrono::milliseconds(timeoutMsec));
    m_timeoutTimer.async_wait(
                WeakCallback(shared_from_this(),
                             [this, timeoutMsec](const boost::system::error_code& error)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, timeoutMsec]()
        {
            if (m_isClosed) return;

            if (OperationAborted(error))
            {
                // it is called by timer.cancel();
                SPDLOG_TRACE("Write() Timer operation was canceled...do nothing");
                return;
            }

            SPDLOG_INFO("Write() HandleTimeOut:: msec={}, url={}", timeoutMsec, m_url);
            auto response = std::make_shared<RtspSocketResponse>();
            response->IsSuccess = false;
            response->StatusCode = status_codes::UserOperationWasTimedout;
            Complete(response);
        }));
    }));
}


void RtspSocket::Connect()
{
    SPDLOG_TRACE("Connect()--START");

    try
    {
        m_sendSocket = std::make_shared<boost::asio::ip::tcp::socket>(m_ioContext);
        m_recvSocket = m_sendSocket;

        boost::system::error_code ec;
        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        std::string strPort;
        boost::conversion::try_lexical_convert<std::string>(m_port, strPort);
        boost::asio::ip::tcp::resolver::query query(m_strIp, strPort);
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query, ec);
        if (ec) {
            SPDLOG_WARN("resolve failed() url={}, ip={}, port={}, ec={}/{}",
                        m_url, m_strIp, m_port, ec.value(), ec.message());

            auto response = std::make_shared<RtspSocketResponse>();
            response->IsSuccess = false;
            response->StatusCode = 0;
            response->ErrorDetails = "connect exception!!(resolve failed)";
            Complete(response);
            return;
        }

        if (m_bindIp.length())
        {
            boost::asio::ip::tcp::endpoint remote_endpoint(
                        boost::asio::ip::address::from_string(m_strIp), m_port); // server address
            m_recvSocket->open(boost::asio::ip::tcp::v4());
            try
            {
                SPDLOG_DEBUG("Connect(), set bind ip, url={}, bind_ip={}", m_url, m_bindIp);
                m_recvSocket->bind(boost::asio::ip::tcp::endpoint(
                                       boost::asio::ip::address::from_string(m_bindIp), // your local address
                                       0));
            }
            catch (boost::system::system_error e)
            {

            }
        }

        SPDLOG_DEBUG("async_connect() start, url={}, ip={}, port={}", m_url, m_strIp, m_port);
        boost::asio::async_connect(*m_sendSocket,
                                   iterator,
                                   WeakCallback(shared_from_this(),
                                                [this](const boost::system::error_code& error, boost::asio::ip::tcp::resolver::iterator iterator)
        {
            boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
            {
                if (m_isClosed) return;
                HandleAsyncConnect(error);
            }));
        }));
    }
    catch (std::exception& e)
    {
        SPDLOG_ERROR("Connect() url={}, exception = {}", m_url, e.what());

        auto response = std::make_shared<RtspSocketResponse>();
        response->IsSuccess = false;
        response->StatusCode = 0;
        response->ErrorDetails = "connect exception!!";
        Complete(response);
    }
}

void RtspSocket::HandleAsyncConnect(const boost::system::error_code& error)
{
    SPDLOG_TRACE("HandleAsyncConnect()--START");

    if (error)
    {
        SPDLOG_DEBUG("HandleAsyncConnect() : Connect error! error={} message={} url={}", error.value(), error.message(), m_url);
        ErrorComplete(error);
        return;
    }

    // buffer size 조정
    boost::system::error_code ec;
    m_sendSocket->set_option(boost::asio::socket_base::receive_buffer_size(kReceiveBufferSize), ec);
    if (ec) {
        SPDLOG_DEBUG("HandleAsyncConnect() : socket set_option(receive_buffer_size) failed() ignore it, url={}", m_url);
    }

    m_sendSocket->set_option(boost::asio::ip::tcp::no_delay(true), ec);
    // mac os에서 ddns로 인터넷망을 통해 ARN-1610 접속시 windows size 237byte ACK를 받을 때
    // 5초씩 write delay 발생하는 문제점 수정.
    // send buffer사이즈를 200byte로 줄여서 window size가 작더라도 패킷을 내보내기 위함.
#ifndef WIN32
    SPDLOG_DEBUG("RtspSocket::HandleAsyncConnect() : m_isBackChannel = {}", m_isBackChannel);
    if(!m_isBackChannel) {
        m_sendSocket->set_option(boost::asio::socket_base::send_buffer_size(200), ec);
        if (ec) {
            SPDLOG_DEBUG("HandleAsyncConnect() : socket set_option(send_buffer_size) failed() ignore it, url={}", m_url);
        }
    }
#endif

    auto buffer = std::make_shared<std::string>(m_currentWriteMessage);
    SPDLOG_TRACE("RtspSocket::HandleAsyncConnect()--async_write() REQUEST, size={}", buffer->size());

    async_write(boost::asio::buffer(*buffer),
                WeakCallback(shared_from_this(),
                             [buffer, this](const boost::system::error_code& error, size_t byteTransferred)
    {
        SPDLOG_TRACE("RtspSocket::HandleAsyncConnect()--async_write() RESPONSE, byte={}", byteTransferred);
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
        {
            if (m_isClosed) return;
            if (error)
            {
                SPDLOG_DEBUG("RtspSocket::HandleAsyncConnect()--async_write() error! error={} message={} url={}",
                    error.value(), error.message(), m_url);
                ErrorComplete(error);
                return;
            }
        }));
    }));

    // Start receive RTSP Response or interleaved data...
    ReadStart();
}

void RtspSocket::MakeRequest(
        RtspSocketRequestPtr& request,
        std::ostringstream& stream)
{
    stream << request->Command << " " << request->Url << " RTSP/1.0\r\n";
    stream << "CSeq: " << (unsigned int)(m_cSeq) << "\r\n";
    m_cSeq++;
    if (m_auth.hasAuthorization())
    {
        m_auth.UpdateAuthorization(request->Command, m_url);
        std::string authLine = m_auth.authorization();
        stream << "Authorization: " << authLine << "\r\n";
    }

    stream << "User-Agent: " << m_userAgentString << "\r\n";
    stream << request->ExtraHeaders;
    stream << "\r\n";

    m_currentWriteMessage = stream.str();
    WriteRequestLog(m_currentWriteMessage);
}

bool RtspSocket::ParseResponse(std::istream& stream, ResponseMap& resMap)
{
    std::string line;

    while (std::getline(stream, line) && line != "\r")
    {
        if (line.substr(0, 5) == "RTSP/" || line.substr(0, 5) == "HTTP/")
        {
            std::vector<std::string> tokens;
            boost::split(tokens, line, boost::is_any_of("\t "), boost::token_compress_on);
            if (tokens.size() > 2)
            {
                //PLOG_TRACE << "Reply:" << tokens[1];
                resMap.insert(ResponseMap::value_type("reply", tokens[1]));
            }
        }
        else
        {
            const auto equals_idx = line.find_first_of(':');
            if (std::string::npos != equals_idx)
            {
                std::string key = line.substr(0, equals_idx);
                std::string value = line.substr(equals_idx + 1);
                boost::trim(key);
                boost::trim(value);
                boost::to_lower(key);
                //PLOG_TRACE << key << ":" << value;
                resMap.insert(ResponseMap::value_type(key, value));
            }
        }
    }

    return true;
}

void RtspSocket::ReadStart()
{
    auto streamPtr = m_responseStream;
    async_read(m_responseStream->Buffer,
               boost::asio::transfer_at_least(1),
               WeakCallback(shared_from_this(),
                            [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            HandleAsyncReadStart(error, byteTransferred);
        }));
    }));
}

void RtspSocket::HandleAsyncReadStart(const boost::system::error_code& error,
                                      size_t byteTransferred)
{
    SPDLOG_TRACE("HandleAsyncReadStart::byteTransferred={}, buffer={}", byteTransferred, m_responseStream->Buffer.size());
    if (error)
    {
        SPDLOG_DEBUG("RtspSocket::HandleAsyncReadStart() error! error={} message={}, url={}", error.value(), error.message(), m_url);
        ErrorComplete(error);
        return;
    }

    bool checkNext = true;
    while (checkNext)
    {
        const char* pkt = boost::asio::buffer_cast<const char*>(m_responseStream->Buffer.data());
        size_t streamSize = m_responseStream->Buffer.size();

        checkNext = false;

        SPDLOG_TRACE("HandleAsyncReadStart::do next!!", streamSize);

        /*
        CHECK RTP Interleaved DATA
        S->C: $\000{2 byte length}{"length" bytes data, w/RTP header}
        S->C: $\000{2 byte length}{"length" bytes data, w/RTP header}
        S->C: $\001{2 byte length}{"length" bytes  RTCP packet}
        */
        if (pkt[0] == '$')
        {
            if (streamSize >= 4)
            {
                // RTP Interleaved 데이터인 경우, 버퍼에 이미 도착한 경우가 많이 있으므로,
                // 여기서 루프로 처리하도록 한다.
                // 계속 타면, function recursive call로 인해서 스택오버플로우가 발생할 것 같다.
                checkNext = HandleAsyncReadRtpInterleavedHeader(true, boost::system::error_code(), 4);
            }
            else
            {
                auto streamPtr = m_responseStream;
                async_read(m_responseStream->Buffer,
                           boost::asio::transfer_at_least(4 - streamSize),
                           WeakCallback(shared_from_this(),
                                        [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
                {
                    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
                    {
                        if (m_isClosed) return;
                        HandleAsyncReadRtpInterleavedHeader(false, error, byteTransferred);
                    }));
                }));
            }
        }

        /*
        CHECK RTSP Response DATA
        if pkt[0] != '$', assume it is RTSP message.
        */
        else
        {
            SPDLOG_TRACE("Read start RTSP response..");

            bool completeRtspResponse = false;
            if (pkt[0] != 'R')
            {
                SPDLOG_ERROR("RtspSocket::RTSP RESPONSE did not start with 'R', continue anyway..., url={}", m_url);
            }
            if (streamSize >= 12) // RTSP/1.0 + \r\n\r\n
            {
                size_t i = 0;
                for (i = 0; i < streamSize - 4; i++)
                {
                    if (pkt[i] == '\r' && pkt[i + 1] == '\n' && pkt[i + 2] == '\r' && pkt[i + 3] == '\n')
                    {
                        SPDLOG_TRACE("RtspSocket::HandleAsyncReadStart() :: read RTSP RESPONSE from buffer");
                        auto readByte = i + 4;
                        checkNext = HandleAsyncReadRtspResponse(true, error, readByte);
                        completeRtspResponse = true;
                        break;
                    }
                }
            }
            if (!completeRtspResponse)
            {
                SPDLOG_TRACE("RtspSocket::HandleAsyncReadStart() :: async_read_until NEW LINE");
                auto streamPtr = m_responseStream;
                async_read_until(m_responseStream->Buffer,
                                 "\r\n\r\n",
                                 WeakCallback(shared_from_this(),
                                              [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
                {
                    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
                    {
                        if (m_isClosed) return;
                        HandleAsyncReadRtspResponse(false, error, byteTransferred);
                    }));
                }));
            }
        }
        
    }

    //PLOG_DEBUG << "HandleAsyncReadStart() -- EXIT";
}


bool RtspSocket::HandleAsyncReadRtpInterleavedHeader(
        const bool isDirectCall,
        const boost::system::error_code& error,
        size_t byteTransferred)
{
    SPDLOG_TRACE("HandleAsyncReadRtpInterleavedHeader::byteTransferred={}, buffer={}", byteTransferred, m_responseStream->Buffer.size());
    if (error)
    {
        SPDLOG_DEBUG("RtspSocket::HandleAsyncReadRtpInterleavedHeader error! error={} message={}, url={}", error.value(), error.message(), m_url);
        ErrorComplete(error);
        return false;
    }

    const unsigned char* pkt = boost::asio::buffer_cast<const unsigned char*>(m_responseStream->Buffer.data());
    unsigned int channel = pkt[1];
    // The byte that we read is the first (high) byte of the 16-bit RTP or RTCP packet 'size'.
    // The byte that we read is the second (low) byte of the 16-bit RTP or RTCP packet 'size'.
    unsigned short dataSize = (((unsigned short)pkt[2]) << 8) | pkt[3];

    m_responseStream->Buffer.consume(4);

    //SPDLOG_DEBUG("RTP Channel={}, rtpDataSize={}, streamSize={}", channel, dataSize, m_responseStream->size());

    if (m_responseStream->Buffer.size() >= dataSize)
    {
        return HandleAsyncReadRtpData(isDirectCall, channel,
                                      dataSize, boost::system::error_code(), dataSize);
    }
    else
    {
        auto streamPtr = m_responseStream;
        async_read(m_responseStream->Buffer,
                   boost::asio::transfer_at_least(dataSize-m_responseStream->Buffer.size()),
                   WeakCallback(shared_from_this(),
                                [this, streamPtr, channel, dataSize, isDirectCall](const boost::system::error_code& error, size_t byteTransferred)
        {
            boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, channel, dataSize, error, byteTransferred]()
            {
                if (m_isClosed) return;
                HandleAsyncReadRtpData(false, channel, dataSize, error, byteTransferred);
            }));
        }));
    }
    return false;
}


bool RtspSocket::HandleAsyncReadRtpData(
        const bool isDirectCall,
        const unsigned int channel,
        const size_t dataSize,
        const boost::system::error_code& error,
        size_t byteTransferred)
{
    SPDLOG_TRACE("HandleAsyncReadRtpData::{}, buffer::{}, channel={}, dataSize={}", byteTransferred, m_responseStream->Buffer.size(), channel, dataSize);
    if (error)
    {
        SPDLOG_DEBUG("RtspSocket::HandleAsyncReadRtpData() error! error={}, message={}, url={}", error.value(), error.message(), m_url);
        ErrorComplete(error);
        return false;
    }

#if 0
    /* DISCONNECTED TEST */
    boost::system::error_code e = boost::asio::error::eof;
    ErrorComplete(error);
    return false;
#endif


    auto rtpSessionItr = m_rtpMap.find(channel);
    if (rtpSessionItr == m_rtpMap.end())
    {
        SPDLOG_DEBUG("RtspSocket::Critical Error!!, check please.. Invalid RTP Channel::{}, url={}", channel, m_url);
    }
    else
    {
        const unsigned char* bufPtr = boost::asio::buffer_cast<const unsigned char*>(m_responseStream->Buffer.data());
        // RTP
        if (channel % 2 == 0)
        {
            m_rtpMap[channel]->ReadRawRtpData(bufPtr, dataSize);
        }
        // RTCP
        else
        {
            m_rtpMap[channel]->ReadRawRtcpData(bufPtr, dataSize);
        }
    }


    m_responseStream->Buffer.consume(dataSize);

    if (m_responseStream->Buffer.size() > 0 && isDirectCall)
    {
        // 여기서는 HandleAsyncReadStart()를 호출하지 않는다.
        // 응답을 true로 올려줘서, HandleAsyncReadStart() 안에서 루프를 반복하도록 한다.
        return true;
    }

    if (m_responseStream->Buffer.size() > 0)
        HandleAsyncReadStart(error, m_responseStream->Buffer.size());
    else
        ReadStart();
    return false;
}


bool RtspSocket::HandleAsyncReadRtspResponse(
        const bool isDirectCall,
        const boost::system::error_code& error,
        size_t byteTransferred)
{
    if (error)
    {
        SPDLOG_INFO("RtspSocket::HandleAsyncReadRtspResponse() error! url={}, error={}, message={}", m_url, error.value(), error.message());
        ErrorComplete(error);
        return false;
    }

    SPDLOG_TRACE("RtspSocket::HandleAsyncReadRtspResponse(), size={}", byteTransferred);

    boost::asio::streambuf::const_buffers_type resBuf = m_responseStream->Buffer.data();
    std::string responseData = std::string(
                boost::asio::buffers_begin(resBuf),
                boost::asio::buffers_begin(resBuf) + byteTransferred);

    m_responseStream->Buffer.consume(byteTransferred);
    WriteResponseLog(responseData);

    ResponseMap Fields;
    std::istringstream stream(responseData);
    ParseResponse(stream, Fields);

    auto itRes = Fields.equal_range("reply");

    std::string statusCode = "";
    for (auto it = itRes.first; it != itRes.second; it++)
    {
        statusCode = it->second;
        break;
    }

    unsigned short nStatusCode = 0;
    unsigned char nCseq = 1;

    if (!statusCode.empty())
        nStatusCode = boost::lexical_cast<unsigned short>(statusCode);

    auto itCseq = Fields.equal_range("cseq");
    std::string cseq = "";
    for (auto it = itCseq.first; it != itCseq.second; it++)
    {
        cseq = it->second;
        break;
    }

    if (!cseq.empty())
        nCseq = (unsigned char)(boost::lexical_cast<unsigned int>(cseq));

    if (nStatusCode != status_codes::OK)
    {
        if (nStatusCode == status_codes::Unauthorized)
        {
            // 인증실패시 1회 재시도한다.
            if (m_auth.GetRetryCount() == 0)
            {
                std::string authLine;

                auto iterPair = Fields.equal_range("www-authenticate");

                for (auto iter = iterPair.first; iter != iterPair.second; ++iter)
                {
                    authLine = iter->second;
                    if (authLine.find("SHA-256") != std::string::npos)
                        break;
                }

                m_auth.ResetAuthorization();
                m_auth.GenerateAuthorization(authLine, m_userName, m_password);
                DigestLogin();

                if (m_responseStream->Buffer.size() > 0 && isDirectCall)
                {
                    return true;
                }
                else
                {
                    SPDLOG_TRACE("HandleAsyncReadRtspResponse():: Continue ReadStart()..");
                    ReadStart();
                    return false;
                }
            }
        }

        // 에러 처리한다.
        auto response = std::make_shared<RtspSocketResponse>();
        response->CSeq = nCseq;
        response->IsSuccess = false;
        response->StatusCode = nStatusCode;
        Complete(response);

        // 에러가 발생하더라도, 연결을 끊어야 하는 경우가 아니면, 수신은 계속 수행해야 한다.
        ReadStart();
        return false;
    }

    // STATUS 200 OK
    m_auth.ResetRetryCount();

    auto response = std::make_shared<RtspSocketResponse>();
    response->CSeq = nCseq;
    response->IsSuccess = true;
    response->StatusCode = nStatusCode;
    response->Fields = Fields;

    static const std::string kContentLengthStr = "content-length";
    auto clItr = Fields.find(kContentLengthStr);
    if (clItr != Fields.end())
    {
        size_t contentLength;
        boost::trim(clItr->second);
        boost::conversion::try_lexical_convert<size_t>(clItr->second, contentLength);
        size_t streamSize = m_responseStream->Buffer.size();

        SPDLOG_TRACE("RtspSocket::HandleAsyncReadRtspResponse():: contentLength={}, streamsize={}", contentLength, streamSize);
        if (contentLength >= streamSize)
        {
            contentLength -= streamSize;
            if (contentLength > 0) {
                SPDLOG_DEBUG("HandleAsyncReadRtspResponse()::Read more content data, contentLength={}", contentLength);
            }

            if (streamSize  > 0) {
                std::stringstream	content;
                content << &m_responseStream->Buffer;
                response->ResponseContent = content.str();
            }
            auto streamPtr = m_responseStream;
            async_read(m_responseStream->Buffer,
                       boost::asio::transfer_at_least(contentLength),
                       WeakCallback(shared_from_this(),
                                    [this, response, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
            {
                boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, response, error, byteTransferred]()
                {
                    if (m_isClosed) return;
                    HandleAsyncReadRtspContent(response, false, error, byteTransferred);
                }));
            }));
            return false;
        }
        else
        {
            // contentLength까지만 받도록 하자.
            return HandleAsyncReadRtspContent(response, isDirectCall, error, contentLength);
        }
    }

    SPDLOG_TRACE("RtspSocket::HandleAsyncReadRtspResponse() call TransferCallback()");
    Complete(response);

    if (m_responseStream->Buffer.size() > 0 && isDirectCall)
    {
        return true;
    }
    else
    {
        ReadStart();
        return false;
    }
}


void RtspSocket::DigestLogin()
{
    boost::asio::post(m_strand,
                      WeakCallback(shared_from_this(),
                                   [this]()
    {
        if (m_currentRequest) {
            std::ostringstream stream;
            MakeRequest(m_currentRequest, stream);

            auto buffer = std::make_shared<std::string>(stream.str());

            SPDLOG_TRACE("RtspSocket::DigestLogin()::async_write() for AUTH, byte = {}, url={}", buffer->size(), m_url);
            async_write(boost::asio::buffer(*buffer),
                        WeakCallback(shared_from_this(),
                            [this, buffer](const boost::system::error_code& error, size_t byteTransferred)
            {
                boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
                {
                    if (m_isClosed) return;
                    SPDLOG_TRACE("RtspSocket::DigestLogin()::async_write() success for AUTH, byteTransferred={}", byteTransferred);
                    if (error)
                    {
                        SPDLOG_INFO("RtspSocket::DigestLogin()::async_write() for AUTH error! error={}, message={}, url={}",
                            error.value(), error.message(), m_url);
                        ErrorComplete(error);
                        return;
                    }
                }));
                
            }));
        }
    }));
}

bool RtspSocket::HandleAsyncReadRtspContent(
        const RtspSocketResponsePtr& response,
        const bool isDirectCall,
        const boost::system::error_code& error,
        size_t byteTransferred)
{
    if (error)
    {
        SPDLOG_DEBUG("RtspSocket::HandleAsyncReadRtspContent() error! error={}, message={}, url={}", error.value(), error.message(), m_url);
        ErrorComplete(error);
        return false;
    }

    if (byteTransferred > 0)
    {
        boost::asio::streambuf::const_buffers_type resBuf = m_responseStream->Buffer.data();
        std::string responseData = std::string(
                    boost::asio::buffers_begin(resBuf),
                    boost::asio::buffers_begin(resBuf) + byteTransferred);

        m_responseStream->Buffer.consume(byteTransferred);
        response->ResponseContent += responseData;
    }

    if (response->ResponseContent.length() > 0) {
        SPDLOG_DEBUG("RTSP RESPONSE(DESCRIBE) Content\n{}\n", response->ResponseContent);
    }

    Complete(response);

    if (m_responseStream->Buffer.size() > 0 && isDirectCall)
    {
        return true;
    }
    else
    {
        ReadStart();
        return false;
    }
}


void RtspSocket::Complete(const RtspSocketResponsePtr &response)
{
    SPDLOG_TRACE("RtspSocket::Complete()");
    boost::asio::post(m_strand,
                      WeakCallback(shared_from_this(),
                                   [this, response]()
    {
        m_timeoutTimer.cancel();

        if (m_currentRequest) {
            m_currentRequest->Callback(response);
            m_currentRequest = nullptr;
        }
    }));
}

void RtspSocket::ErrorComplete(const boost::system::error_code &error)
{
    //SPDLOG_DEBUG("RtspSocket::ErrorCompete()");
    if (OperationAborted(error))
    {
        SPDLOG_DEBUG("RtspSocket::operation was aborted..., url={}", m_url);
        m_timeoutTimer.cancel();
        return;
    }
    boost::asio::post(m_strand,
                      WeakCallback(shared_from_this(),
                                   [this, error]()
    {
        bool socketClosed = SocketWasClosed(error);
        if (m_currentRequest) {
            auto response = std::make_shared<RtspSocketResponse>();
            response->IsSuccess = false;
            response->StatusCode = 0;

            if (socketClosed) {
                SPDLOG_DEBUG("RtspSocket::Socket was closed..., url={}", m_url);
                response->StatusCode = status_codes::UserConnectionWasClosed;
            }

            response->ErrorDetails = error.message();
            m_currentRequest->Callback(response);
            m_currentRequest = nullptr;
        }
        else if (m_eventCallback) {
            // call event callback if there is no request.
            auto evt = std::make_shared<RtspEvent>();
            evt->EventType = SOCKET_CLOSED;
            m_eventCallback(evt);
        }

        if (socketClosed) {
            Close();
        }
    }));
}

void RtspSocket::Close()
{
    SPDLOG_DEBUG("RtspSocket::Close(), url={}", m_url);
    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_isClosed = true;
        // SEND SOCKET과 RECV SOCKET이 동일하다.
        if (IsConnected())
        {
            SPDLOG_DEBUG("RtspSocket::CloseInternal(), url={}", m_url);
            boost::system::error_code ec;
            m_sendSocket->shutdown(boost::asio::socket_base::shutdown_both, ec);
            m_sendSocket->close(ec);
        }
        m_timeoutTimer.cancel();
        m_currentRequest = nullptr;
        //m_eventCallback = nullptr;
    }));
}

void RtspSocket::WriteBackChannelData(unsigned char rtpChannel, unsigned char* data, size_t dataSize)
{
    unsigned int packetSize = (unsigned int)(dataSize);
    unsigned char hdr[4] = { 0 };
    hdr[0] = '$';
    hdr[1] = rtpChannel;
    hdr[2] = (unsigned char)((packetSize & 0xFF00) >> 8);
    hdr[3] = (unsigned char)(packetSize & 0xFF);

    std::ostringstream ostream;
    ostream.write((char*)hdr, 4);
    ostream.write((char*)data, dataSize);

    std::shared_ptr<std::string> buffer;

    if(m_transportMethod == RTP_RTSP_HTTP || m_transportMethod == RTP_RTSP_HTTPS)
        buffer = std::make_shared<std::string>(SecurityUtil::Base64Encoding(ostream.str()));
    else
        buffer = std::make_shared<std::string>(ostream.str());

    async_write(boost::asio::buffer(*buffer),
                WeakCallback(shared_from_this(),
                             [buffer, this](const boost::system::error_code& error, std::size_t bytes_transferred)
    {
        if (error)
        {
            SPDLOG_INFO("HandleAsyncWriteBackChannelData error! error={}, message={}, url={}", error.value(), error.message(), m_url);
            return;
        }
    }));
}

AsioStreamBuf::AsioStreamBuf()
{
    SPDLOG_DEBUG("AsioStreamBuf()");
}

AsioStreamBuf::~AsioStreamBuf()
{
    SPDLOG_DEBUG("~AsioStreamBuf()");
}


}
}

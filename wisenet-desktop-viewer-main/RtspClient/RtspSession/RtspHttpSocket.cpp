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
#include <boost/bind.hpp>
#include <boost/make_shared.hpp>

#include "RtspHttpSocket.h"
#include "LogSettings.h"
#include "WeakCallback.h"
#include "SecurityUtil.h"


namespace Wisenet
{
namespace Rtsp
{

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-HTTP] "}, level, __VA_ARGS__)

RtspHttpSocket::RtspHttpSocket(boost::asio::io_context& ioContext,
                               const std::string &rtspUrl,
                               const std::string &userName,
                               const std::string &password)
    : RtspSocket(ioContext, rtspUrl, userName, password)
{
    SPDLOG_DEBUG("RtspHttpSocket() : {}", rtspUrl);
}

RtspHttpSocket::~RtspHttpSocket()
{
    SPDLOG_DEBUG("~RtspHttpSocket(), url={}", m_url);
}

void RtspHttpSocket::Initialize(const unsigned int timeoutSec,
                                const unsigned short defaultPort,
                                const TransportMethod transportMethod)
{
    m_timeoutSec = timeoutSec;
    m_transportMethod = transportMethod;
    m_cSeq = 2;
    m_isSsl = false;
    m_bindIp = "";
    m_responseStream->Buffer.prepare(64000);

    ParseUrl(m_url, defaultPort, m_strIp, m_port, m_extraUrl);
}

void RtspHttpSocket::Close()
{
    SPDLOG_DEBUG("Close(), url={}", m_url);
    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_isClosed = true;
        if (IsConnected())
        {
            SPDLOG_DEBUG("CloseInternal(), url={}", m_url);
            boost::system::error_code ec;
            if (m_sendSocket)
            {
                m_sendSocket->shutdown(boost::asio::socket_base::shutdown_both, ec);
                m_sendSocket->close(ec);
            }
            if (m_recvSocket)
            {
                m_recvSocket->shutdown(boost::asio::socket_base::shutdown_both, ec);
                m_recvSocket->close(ec);
            }
        }
        m_timeoutTimer.cancel();
        m_currentRequest = nullptr;
        m_eventCallback = nullptr;
    }));
}

bool RtspHttpSocket::IsConnected()
{
    if (!m_sendSocket)
        return false;
    if (!m_recvSocket)
        return false;

    return (m_sendSocket->is_open() && m_recvSocket->is_open());
}

void RtspHttpSocket::Connect()
{
    SPDLOG_DEBUG("Connect(), url={}", m_url);
    try
    {
        // m_sendSocket :: HTTP POST
        // m_recvSocket :: HTTP GET

        // 1. HTTP GET CONNECTION
        m_recvSocket = std::make_shared<boost::asio::ip::tcp::socket>(m_ioContext);

        // 2. HTTP POST CONNECTION
        m_sendSocket = std::make_shared<boost::asio::ip::tcp::socket>(m_ioContext);

        boost::asio::ip::tcp::resolver resolver(m_ioContext);
        std::string strPort;
        boost::conversion::try_lexical_convert<std::string>(m_port, strPort);
        boost::asio::ip::tcp::resolver::query query(m_strIp, strPort);
        boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

        if (m_bindIp.length())
        {
            boost::asio::ip::tcp::endpoint remote_endpoint(
                        boost::asio::ip::address::from_string(m_strIp), m_port); // server address
            m_recvSocket->open(boost::asio::ip::tcp::v4());
            try
            {
                m_recvSocket->bind(boost::asio::ip::tcp::endpoint(
                                       boost::asio::ip::address::from_string(m_bindIp), // your local address
                                       0));
            }
            catch (boost::system::system_error)
            {

            }
        }

        boost::asio::async_connect(*m_recvSocket, iterator, WeakCallback(
                                       shared_from_this(),
                                       [this](const boost::system::error_code& error,
                                       boost::asio::ip::tcp::resolver::iterator iterator)
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
        SPDLOG_ERROR("Connect() exception = {}, url={}", e.what(), m_url);

        auto response = std::make_shared<RtspSocketResponse>();
        response->IsSuccess = false;
        response->StatusCode = 0;
        response->ErrorDetails = "connect exception!!";
        Complete(response);
    }
}

void RtspHttpSocket::HandleAsyncConnect(const boost::system::error_code& error)
{
    SPDLOG_TRACE("RtspHttpSocket::HandleAsyncConnect(), url={}", m_url);
    if (error)
    {
        SPDLOG_INFO("RtspHttpSocket::HandleAsyncConnect() error! url={}, error={}, message={}", m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    // SEND HTTP GET Message
    // set receive buffer size
    m_recvSocket->set_option(boost::asio::ip::tcp::no_delay(true));
    m_recvSocket->set_option(boost::asio::socket_base::receive_buffer_size(kReceiveBufferSize));

    boost::asio::socket_base::receive_buffer_size option;
    m_recvSocket->get_option(option);
    size_t current_size = option.value();

    m_sessionCookie = SecurityUtil::GetRandomStringMD5();
    m_sessionCookie.resize(22);
    std::ostringstream stream;
    MakeHttpGetMessage(stream);

    auto buffer = std::make_shared<std::string>(stream.str());
    auto streamPtr = m_responseStream;
    boost::asio::async_write(*m_recvSocket, boost::asio::buffer(*buffer), WeakCallback(
                                 shared_from_this(),
                                 [this, buffer, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;

            if (error)
            {
                SPDLOG_WARN("async_write(recv) failed.. url={}, error={}, message={}", m_url, error.value(), error.message());
                ErrorComplete(error);
                return;
            }

            auto streamPtr = m_responseStream;
            boost::asio::async_read_until(*m_recvSocket, m_responseStream->Buffer,
                "\r\n\r\n",
                [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
            {
                boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
                {
                    if (m_isClosed) return;

                    if (error)
                    {
                        SPDLOG_WARN("RtspHttpSocket::async_read_until(recv) failed.. url={}, error={}, message={}",
                            m_url, error.value(), error.message());
                        ErrorComplete(error);
                        return;
                    }

                    ResponseMap Fields;
                    std::istream stream(&m_responseStream->Buffer);
                    ParseResponse(stream, Fields);

                    auto itRes = Fields.equal_range("reply");

                    std::string statusCode = "";
                    for (auto it = itRes.first; it != itRes.second; it++)
                    {
                        statusCode = it->second;
                        break;
                    }

                    unsigned short nStatusCode = 0;
                    boost::trim(statusCode);
                    if (!statusCode.empty())
                        nStatusCode = boost::lexical_cast<unsigned short>(statusCode);

                    if (nStatusCode == RTSP_Unauthorized && m_auth.GetRetryCount() == 0)
                    {
                        SPDLOG_DEBUG("HandleAsyncConnect() nStatusCode == RTSP_Unauthorized && m_auth.GetRetryCount() == 0, url={}", m_url);

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
                        Connect();

                        return;
                    }
                    else if (nStatusCode != 200)
                    {
                        auto response = std::make_shared<RtspSocketResponse>();
                        response->IsSuccess = false;
                        response->StatusCode = nStatusCode;
                        response->ErrorDetails = "invalid http response";
                        Complete(response);
                        return;
                    }
                    m_auth.ResetRetryCount();

                    boost::asio::ip::tcp::resolver resolver(m_ioContext);
                    std::string strPort;
                    boost::conversion::try_lexical_convert<std::string>(m_port, strPort);
                    boost::asio::ip::tcp::resolver::query query(m_strIp, strPort);
                    boost::asio::ip::tcp::resolver::iterator iterator = resolver.resolve(query);

                    if (m_bindIp.length())
                    {
                        boost::asio::ip::tcp::endpoint remote_endpoint(
                            boost::asio::ip::address::from_string(m_strIp), m_port); // server address
                        m_recvSocket->open(boost::asio::ip::tcp::v4());
                        try
                        {
                            m_recvSocket->bind(boost::asio::ip::tcp::endpoint(
                                boost::asio::ip::address::from_string(m_bindIp), // your local address
                                0));
                        }
                        catch (boost::system::system_error)
                        {

                        }
                    }

                    boost::asio::async_connect(*m_sendSocket, iterator, WeakCallback(
                        shared_from_this(),
                        [this](const boost::system::error_code& error,
                            boost::asio::ip::tcp::resolver::iterator iterator)
                    {
                        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
                            {
                                if (m_isClosed) return;
                                HandleAsyncConnect2(error);
                            }));
                    }));
                }));
            });
        }));
        
    }));
}


void RtspHttpSocket::HandleAsyncConnect2(const boost::system::error_code& error)
{
    SPDLOG_TRACE("RtspHttpSocket::HandleAsyncConnect2()");

    if (error)
    {
        SPDLOG_INFO("RtspHttpSocket::HandleAsyncConnect2 error! url={}, error={}, message={}", m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    std::ostringstream stream;
    MakeHttpPostMessage(stream);

    auto buffer = std::make_shared<std::string>(stream.str());
    boost::asio::async_write(*m_sendSocket, boost::asio::buffer(*buffer), WeakCallback(
                                 shared_from_this(),
                                 [this, buffer](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            if (error)
            {
                SPDLOG_INFO("RtspHttpSocket::async_write(send) failed.. url={} error={}, message={}", m_url, error.value(), error.message());
                ErrorComplete(error);
                return;
            }

            SPDLOG_DEBUG("RtspHttpSocket::HTTP connection has been established...url={}", m_url);
            SendCurrentRequest();

            // Start receive RTSP Response or interleaved data...
            ReadStart();
        }));
        
    }));
}

void RtspHttpSocket::SendCurrentRequest()
{
    SPDLOG_TRACE("RtspHttpSocket::SendCurrentRequest() : START");
    auto buffer = std::make_shared<std::string>(m_currentWriteMessage);
    async_write(boost::asio::buffer(*buffer), WeakCallback(
                    shared_from_this(),
                    [this, buffer](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            if (error) {
                SPDLOG_INFO("RtspHttpSocket::SendCurrentRequest() : async_write() error! url={}, error={}, message={}", m_url, error.value(), error.message());
                ErrorComplete(error);
                return;
            }
        }));
    }));
}

void RtspHttpSocket::MakeHttpGetMessage(std::ostringstream& stream)
{
    stream << "GET " << m_extraUrl << " HTTP/1.1\r\n";
    stream << "CSeq: << " << (unsigned int)m_cSeq << "\r\n";
    m_cSeq++;
    stream << "User-Agent: " << m_userAgentString << "\r\n";
    stream << "Host: " << m_strIp << "\r\n";
    stream << "x-sessioncookie: " << m_sessionCookie << "\r\n";
    stream << "Accept: application/x-rtsp-tunnelled\r\n";
    if (m_auth.hasAuthorization())
    {
        m_auth.UpdateAuthorization("Get", m_extraUrl);
        std::string authLine = m_auth.authorization();
        stream << "Authorization: " << authLine << "\r\n";
    }
    stream << "Pragma: no-cache\r\n";
    stream << "Cache-Control: no-cache\r\n";
    stream << "\r\n";
}

void RtspHttpSocket::MakeHttpPostMessage(std::ostringstream& stream)
{
    stream << "POST " << m_extraUrl << " HTTP/1.1\r\n";
    stream << "CSeq: << " << (unsigned int)m_cSeq << "\r\n"; // no count sequence??
    m_cSeq++;
    stream << "User-Agent: " << m_userAgentString << "\r\n";
    stream << "Host: " << m_strIp << "\r\n";
    stream << "x-sessioncookie: " << m_sessionCookie << "\r\n";
    stream << "Content-Type: application/x-rtsp-tunnelled\r\n";

    if (m_auth.hasAuthorization())
    {
        m_auth.UpdateAuthorization("Get", m_extraUrl);
        std::string authLine = m_auth.authorization();
        stream << "Authorization: " << authLine << "\r\n";
    }

    stream << "Pragma: no-cache\r\n";
    stream << "Cache-Control: no-cache\r\n";
    stream << "Content-Length: 32767\r\n";
    stream << "Expires: Sun, 9 Jan 1972 00:00:00 GMT\r\n";
    stream << "\r\n";
}

void RtspHttpSocket::MakeRequest(
        RtspSocketRequestPtr& request,
        std::ostringstream& stream)
{
    std::ostringstream ostream;
    RtspSocket::MakeRequest(request, ostream);

    std::string b64 = SecurityUtil::Base64Encoding(ostream.str());
    stream << b64;
    m_currentWriteMessage = b64;
}


}
}

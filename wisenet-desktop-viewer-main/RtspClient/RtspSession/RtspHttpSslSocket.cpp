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
#include <functional>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/make_shared.hpp>
#include <boost/core/ignore_unused.hpp>

#include "RtspHttpSslSocket.h"
#include "LogSettings.h"
#include "WeakCallback.h"

using boost::asio::ip::tcp;
namespace ssl = boost::asio::ssl;
typedef ssl::stream<tcp::socket> ssl_socket;

namespace Wisenet
{
namespace Rtsp
{


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[RTSP-HTTPS] "}, level, __VA_ARGS__)

RtspHttpSslSocket::RtspHttpSslSocket(boost::asio::io_context& ioContext,
                                     const std::string &rtspUrl,
                                     const std::string &userName,
                                     const std::string &password)
    : RtspHttpSocket(ioContext, rtspUrl, userName, password)
    , m_sslSendContext(ssl::context::tls)
    , m_sslRecvContext(ssl::context::tls)
{
    SPDLOG_DEBUG("RtspHttpSslSocket : {}", rtspUrl);
}

RtspHttpSslSocket::~RtspHttpSslSocket()
{
    SPDLOG_DEBUG("~RtspHttpSslSocket(), url={}", m_url);
}

void RtspHttpSslSocket::Initialize(const unsigned int timeoutSec,
                                   const unsigned short defaultPort,
                                   const TransportMethod transportMethod)
{
    m_timeoutSec = timeoutSec;
    m_transportMethod = transportMethod;
    m_cSeq = 1;
    m_isSsl = true;
    m_bindIp = "";
    m_responseStream->Buffer.prepare(64000);

    ParseUrl(m_url, defaultPort, m_strIp, m_port, m_extraUrl);
}

void RtspHttpSslSocket::Close()
{
    SPDLOG_DEBUG("Close(), url={}", m_url);
    boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_isClosed = true;
        if (IsConnected())
        {
            SPDLOG_DEBUG("CloseInternal(), url={}", m_url);
            boost::system::error_code ec;
            if (m_sslRecvSock)
            {
                m_sslRecvSock->lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, ec);
                m_sslRecvSock->lowest_layer().close(ec);
            }
            if (m_sslSendSock)
            {
                m_sslSendSock->lowest_layer().shutdown(boost::asio::socket_base::shutdown_both, ec);
                m_sslSendSock->lowest_layer().close(ec);
            }
        }
        m_timeoutTimer.cancel();
        m_currentRequest = nullptr;
        m_eventCallback = nullptr;
    }));
}

bool RtspHttpSslSocket::IsConnected()
{
    if (!m_sslSendSock)
        return false;
    if (!m_sslRecvSock)
        return false;

    return (m_sslSendSock->lowest_layer().is_open() && m_sslRecvSock->lowest_layer().is_open());
}

void RtspHttpSslSocket::Connect()
{
    std::string sslVersion = OpenSSL_version(OPENSSL_VERSION);
    SPDLOG_DEBUG("Connect(), SSL VER::{}, url={}", sslVersion, m_url);

    try
    {
        m_sslRecvContext.set_verify_mode(ssl::verify_none);
        m_sslSendContext.set_verify_mode(ssl::verify_none);

        m_sslSendContext.set_default_verify_paths();
        m_sslRecvContext.set_default_verify_paths();

        // m_sendSocket :: HTTP POST
        // m_recvSocket :: HTTP GET

        // 1. HTTP GET CONNECTION
        m_sslRecvSock = std::make_shared < ssl::stream<boost::asio::ip::tcp::socket>>(m_ioContext, m_sslRecvContext);

        // 2. HTTP POST CONNECTION
        m_sslSendSock = std::make_shared < ssl::stream<boost::asio::ip::tcp::socket>>(m_ioContext, m_sslSendContext);

        tcp::resolver resolver(m_ioContext);
        std::string strPort;
        boost::conversion::try_lexical_convert<std::string>(m_port, strPort);
        tcp::resolver::query query(m_strIp, strPort);
        tcp::resolver::iterator iterator = resolver.resolve(query);

        if (m_bindIp.length())
        {
            boost::asio::ip::tcp::endpoint remote_endpoint(
                        boost::asio::ip::address::from_string(m_strIp), m_port); // server address
            m_sslRecvSock->lowest_layer().open(boost::asio::ip::tcp::v4());
            try
            {
                m_sslRecvSock->lowest_layer().bind(boost::asio::ip::tcp::endpoint(
                                                       boost::asio::ip::address::from_string(m_bindIp), // your local address
                                                       0));
            }
            catch (boost::system::system_error e)
            {

            }
        }

        boost::asio::async_connect(m_sslRecvSock->lowest_layer(), iterator, WeakCallback(
                                       shared_from_this(),
                                       [this](const boost::system::error_code& error, tcp::resolver::iterator iterator)
        {
            boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, iterator]()
            {
                if (m_isClosed) return;
                m_sslRecvSock->set_verify_callback(std::bind(&RtspHttpSslSocket::GetCertificate, this, std::placeholders::_1, std::placeholders::_2));
                HandleAsyncConnect(error);
            }));
        }));
    }
    catch (std::exception& e)
    {
        SPDLOG_WARN("Connect() exception!, url={}, exception = {}", m_url, e.what());
    }
}

bool RtspHttpSslSocket::GetCertificate(bool preverified, boost::asio::ssl::verify_context& ctx)
{
    boost::ignore_unused(ctx);
    //*m_cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    preverified = true;
    return preverified;
}

bool RtspHttpSslSocket::VerifyCertificate(bool preverified,
                                          boost::asio::ssl::verify_context& ctx)
{
    boost::ignore_unused(ctx);
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // In this example we will simply print the certificate's subject name.
    /* //*
    char subject_name[256] = { 0 };
    X509* cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());
    X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
    std::cout << "Verifying " << subject_name << "\n";
    */
    return true || preverified;
}

// 수신 소켓 연결 콜백
void RtspHttpSslSocket::HandleAsyncConnect(const boost::system::error_code& error)
{
    if (error)
    {
        SPDLOG_INFO("HandleAsyncConnect error! url={}, error={}, message={}", m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    SPDLOG_TRACE("Connected");

    // 수신소켓 HTTPS SETUP
    m_sslRecvSock->lowest_layer().set_option(tcp::no_delay(true));
    // buffer size 조정
    m_sslRecvSock->lowest_layer().set_option(boost::asio::socket_base::receive_buffer_size(kReceiveBufferSize));

    boost::asio::socket_base::receive_buffer_size option;
    m_sslRecvSock->lowest_layer().get_option(option);
    //size_t current_size = option.value();

    m_sslRecvSock->async_handshake(ssl_socket::client, WeakCallback(
                                       shared_from_this(),
                                       [this](const boost::system::error_code& error)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
        {
            if (m_isClosed) return;
            HandleAsyncReadForConnect2(error);
        }));
    }));
}

void RtspHttpSslSocket::HandleAsyncReadForConnect2(const boost::system::error_code& error)
{
    if (error)
    {
        SPDLOG_WARN("HandleAsyncReadForConnect2() failed.. url={}, error={}, message={}",
                    m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    m_sessionCookie = SecurityUtil::GetRandomStringMD5();
    m_sessionCookie.resize(22);

    std::ostringstream stream;
    MakeHttpGetMessage(stream);
    auto buffer = std::make_shared<std::string>(stream.str());
    boost::asio::async_write(*m_sslRecvSock, boost::asio::buffer(*buffer), WeakCallback(
                                 shared_from_this(),
                                 [buffer, this](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            HandleAsyncWriteForConnect2(error, byteTransferred);
        }));
    }));
}

void RtspHttpSslSocket::HandleAsyncWriteForConnect2(const boost::system::error_code& error, size_t byteTransferred)
{
    boost::ignore_unused(byteTransferred);
    if (error)
    {
        SPDLOG_WARN("HandleAsyncWriteForConnect2() failed.. url={}, error={}, message={}",
                    m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    auto streamPtr = m_responseStream;
    boost::asio::async_read_until(*m_sslRecvSock, m_responseStream->Buffer, "\r\n\r\n", WeakCallback(
                                      shared_from_this(),
                                      [this, streamPtr](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error, byteTransferred]()
        {
            if (m_isClosed) return;
            if (error)
            {
                SPDLOG_WARN("HandleAsyncWriteForConnect2()::async_read_until(recv) failed.. url={}, error={}, message={}",
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
            if (!statusCode.empty())
                boost::conversion::try_lexical_convert<unsigned short>(statusCode, nStatusCode);

            if (nStatusCode != 200)
            {
                auto response = std::make_shared<RtspSocketResponse>();
                response->IsSuccess = false;
                response->StatusCode = nStatusCode;
                response->ErrorDetails = "invalid http response";
                Complete(response);
                return;
            }

            tcp::resolver resolver(m_ioContext);
            std::string strPort;
            boost::conversion::try_lexical_convert<std::string>(m_port, strPort);
            tcp::resolver::query query(m_strIp, strPort);
            tcp::resolver::iterator iterator = resolver.resolve(query);

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
                catch (boost::system::system_error e)
                {

                }
            }

            boost::asio::async_connect(m_sslSendSock->lowest_layer(), iterator, WeakCallback(
                shared_from_this(),
                [=](const boost::system::error_code& error, tcp::resolver::iterator iterator)
            {
                boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
                {
                    if (m_isClosed) return;
                    m_sslSendSock->set_verify_callback(std::bind(&RtspHttpSslSocket::GetCertificate, this, std::placeholders::_1, std::placeholders::_2));
                    HandleAsyncConnect2(error);
                }));
            }));
        }));
    }));
}

// 송신 소켓 연결 콜백
void RtspHttpSslSocket::HandleAsyncConnect2(const boost::system::error_code& error)
{
    SPDLOG_TRACE("HandleAsyncConnect2()");

    if (error)
    {
        SPDLOG_INFO("HandleAsyncConnect2 error! url={} error={}, message={}", m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    m_sslSendSock->lowest_layer().set_option(tcp::no_delay(true));
#ifndef WIN32
    boost::system::error_code ec;
    m_sslSendSock->lowest_layer().set_option(boost::asio::socket_base::send_buffer_size(200), ec);
    if (ec) {
        SPDLOG_DEBUG("HandleAsyncConnect2() : socket set_option(send_buffer_size) failed() ignore it, url={}", m_url);
    }
#endif

    m_sslSendSock->async_handshake(ssl_socket::client, WeakCallback(
                                       shared_from_this(),
                                       [this](const boost::system::error_code& error)
    {
        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
        {
            if (m_isClosed) return;
            HandleAsyncConnect3(error);
        }));
    }));
}

void RtspHttpSslSocket::HandleAsyncConnect3(const boost::system::error_code& error)
{
    SPDLOG_TRACE("HandleAsyncConnect3()");

    if (error)
    {
        SPDLOG_INFO("HandleAsyncConnect3() failed.. url={}, error={}, message={}",
                    m_url, error.value(), error.message());
        ErrorComplete(error);
        return;
    }

    std::ostringstream stream;
    MakeHttpPostMessage(stream);
    auto buffer = std::make_shared<std::string>(stream.str());

    boost::asio::async_write(*m_sslSendSock, boost::asio::buffer(*buffer), WeakCallback(
                                 shared_from_this(),
                                 [this, buffer](const boost::system::error_code& error, size_t byteTransferred)
    {
        boost::ignore_unused(byteTransferred);

        boost::asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this, error]()
        {
            if (m_isClosed) return;

            if (error) {
                SPDLOG_INFO("async_write(send) failed.. url={}, error={}, message={}",
                    m_url, error.value(), error.message());
                ErrorComplete(error);
                return;
            }

            SPDLOG_DEBUG("HTTPS connection has been established...url={}", m_url);

            SendCurrentRequest();

            // Start receive RTSP Response or interleaved data...
            ReadStart();
        }));
    }));
}


}
}

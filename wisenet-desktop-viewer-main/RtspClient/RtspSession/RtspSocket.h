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
#pragma once

#include <memory>
#include <atomic>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/asio/strand.hpp>
#include <boost/unordered_map.hpp>
#include "RtspRequestResponse.h"
#include "DigestAuthenticator.h"
#include "RtpSession/RtpSession.h"

namespace Wisenet
{
namespace Rtsp
{

typedef boost::unordered_multimap<std::string, std::string> ResponseMap;

struct RtspSocketRequest;

struct RtspSocketResponse
{
    RtspSocketResponse()
        : CSeq(1), IsSuccess(true), StatusCode(200)
    {}
    unsigned char	CSeq;
    bool			IsSuccess;

    std::string		ErrorDetails;

    std::string		ResponseContent;
    unsigned short	StatusCode;
    ResponseMap		Fields;
};
typedef std::shared_ptr<RtspSocketResponse> RtspSocketResponsePtr;

typedef std::function<void(const RtspSocketResponsePtr& response)> RtspSocketCallbackFunctor;

struct RtspSocketRequest
{
    RtspSocketRequest(const std::string& rtspCommand,
        const std::string& url,
        const std::string& extraHeaders,
        const RtspSocketCallbackFunctor& callback)
        : Command(rtspCommand)
        , Url(url), ExtraHeaders(extraHeaders)
        , Callback(callback)
    {

    }

    const std::string				Command;
    const std::string				Url;
    const std::string				ExtraHeaders;
    const RtspSocketCallbackFunctor Callback;
};

typedef std::shared_ptr<RtspSocketRequest> RtspSocketRequestPtr;
typedef std::shared_ptr<boost::asio::ip::tcp::socket> TcpSocketPtr;
typedef std::shared_ptr<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> SslSocketPtr;

struct AsioStreamBuf
{
    AsioStreamBuf();
    ~AsioStreamBuf();
    boost::asio::streambuf Buffer;
};
typedef std::shared_ptr<AsioStreamBuf> AsioStreamBufPtr;

class RtspSocket : public std::enable_shared_from_this<RtspSocket>
{
public:
    RtspSocket(boost::asio::io_context& ioc,
               const std::string &rtspUrl,
               const std::string &userName,
               const std::string &password);

    virtual ~RtspSocket();

    virtual void Initialize(const unsigned int timeoutSec,
                    const unsigned short defaultPort,
                    const TransportMethod transportMethod);

    void SetEventCallback(const RtspEventCallbackFunctor& eventCallback);

    void SetUserAgentString(const std::string& userAgent);
    void SetBindIp(const std::string& bindIp);
    void SetInterleavedRtpSessionPtr(RtpSessionPtr& rtpSession);
    void SetBackChannel(bool isBackChannel);

    const std::string& GetIp() { return m_strIp; }

    void Write(const std::string& rtspCommand,
        const std::string& url,
        const std::string& contents,
        RtspSocketCallbackFunctor callback);

    virtual void Close();
    void WriteBackChannelData(unsigned char rtpChannel, unsigned char* data, size_t dataSize);

protected:
    enum { kReceiveBufferSize = 1024000};
    bool ParseUrl(const std::string &srcUrl, const unsigned short defaultPort,
                  std::string &retIp, unsigned short &retPort,
                  std::string &retExtraUrl);
    bool ParseResponse(std::istream& stream, ResponseMap& resMap);
    virtual bool IsConnected();
    virtual void Connect();

    virtual void HandleAsyncConnect(const boost::system::error_code& error);

    virtual void MakeRequest(
        RtspSocketRequestPtr& request,
        std::ostringstream& stream);

    void WriteStart(RtspSocketRequestPtr& request);
    void StartTimer(RtspSocketRequestPtr& request);
    void ReadStart();

    // READ START POINT
    void HandleAsyncReadStart(
            const boost::system::error_code& error,
            size_t byteTransferred);

    // RTP INTERLEAVED
    bool HandleAsyncReadRtpInterleavedHeader(
            const bool isDirectCall,
            const boost::system::error_code& error,
            size_t byteTransferred);

    // RTP DATA
    bool HandleAsyncReadRtpData(
            const bool isDirectCall,
            const unsigned int channel,
            const size_t dataSize,
            const boost::system::error_code& error,
            size_t byteTransferred);

    // RTSP RESPONSE
    bool HandleAsyncReadRtspResponse(
            const bool isDirectCall,
            const boost::system::error_code& error,
            size_t byteTransferred);

    // RTSP RESPONSE CONTENT
    bool HandleAsyncReadRtspContent(
            const RtspSocketResponsePtr& response,
            const bool isDirectCall,
            const boost::system::error_code& error,
            size_t byteTransferred);

    void DigestLogin();
    void Complete(const RtspSocketResponsePtr &response);
    void ErrorComplete(const boost::system::error_code& error);

    void WriteRequestLog(const std::string& request);
    void WriteResponseLog(const std::string& response);
protected:
    template <typename Allocator, typename CompletionCondition, typename ReadHandler>
    void async_read(boost::asio::basic_streambuf<Allocator>& b,
        CompletionCondition completion_condition,
        BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
    {
        if (m_isSsl)
        {
            if (m_sslRecvSock)
            {
                boost::asio::async_read(*m_sslRecvSock, b,
                    completion_condition, handler);
            }
        }
        else
        {
            if (m_recvSocket)
            {
                boost::asio::async_read(*m_recvSocket, b,
                    completion_condition, handler);
            }
        }
    }
    template <typename Allocator, typename ReadHandler>
    void async_read_until(boost::asio::basic_streambuf<Allocator>& b,
        const std::string& delim,
        BOOST_ASIO_MOVE_ARG(ReadHandler) handler)
    {
        if (m_isSsl)
        {
            if (m_sslRecvSock)
            {
                boost::asio::async_read_until(*m_sslRecvSock, b, delim, handler);
            }
        }
        else
        {
            if (m_recvSocket)
            {
                boost::asio::async_read_until(*m_recvSocket, b, delim, handler);
            }
        }
    }
    template <typename ConstBufferSequence, typename WriteHandler>
    void async_write(const ConstBufferSequence& b,
        BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
    {
        if (m_isSsl)
        {
            if (m_sslSendSock)
            {
                boost::asio::async_write(*m_sslSendSock, b, handler);
            }
        }
        else
        {
            if (m_sendSocket)
            {
                boost::asio::async_write(*m_sendSocket, b, handler);
            }
        }
    }

    template <typename ConstBufferSequence,	typename CompletionCondition, typename WriteHandler>
    void async_write_with_readSock(const ConstBufferSequence& buffers,
        CompletionCondition completion_condition,
        BOOST_ASIO_MOVE_ARG(WriteHandler) handler)
    {
        if (m_isSsl)
        {
            if (m_sslRecvSock)
            {
                boost::asio::async_write(*m_sslRecvSock, buffers, completion_condition, handler);
            }
        }
        else
        {
            if (m_recvSocket)
            {
                boost::asio::async_write(*m_recvSocket, buffers, completion_condition, handler);
            }
        }
    }

protected:
    boost::asio::io_context&    m_ioContext;
    boost::asio::strand<boost::asio::io_context::executor_type>    m_strand;

    const std::string		m_url;
    const std::string		m_userName;
    const std::string		m_password;

    std::string				m_bindIp;
    std::string				m_userAgentString;
    unsigned int			m_timeoutSec;

    std::string				m_strIp;
    std::string				m_extraUrl;
    unsigned short			m_port;

    unsigned char			m_cSeq;

    TcpSocketPtr	m_sendSocket;
    TcpSocketPtr	m_recvSocket;
    SslSocketPtr	m_sslRecvSock;
    SslSocketPtr	m_sslSendSock;
    bool			m_isSsl;
    bool            m_isBackChannel;

    DigestAuthenticator m_auth;

    TransportMethod m_transportMethod;
    AsioStreamBufPtr                                m_responseStream;

    RtspSocketRequestPtr							m_currentRequest;
    RtspEventCallbackFunctor                        m_eventCallback;
    std::string										m_currentWriteMessage;
    std::map<unsigned int, RtpSessionPtr>			m_rtpMap;
    boost::asio::steady_timer                       m_timeoutTimer;

    std::atomic<bool>                               m_isClosed;
};

typedef std::shared_ptr<RtspSocket> RtspSocketPtr;


}
}

/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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

# pragma once

#ifndef OSCAR_RTSP_HTTP_SSL_SOCKET_HPP
#define OSCAR_RTSP_HTTP_SSL_SOCKET_HPP

#include <boost/pointer_cast.hpp>
#include <boost/asio/ssl.hpp>

#include "RtspHttpSocket.h"

namespace Wisenet
{
namespace Rtsp
{


class RtspHttpSslSocket : public RtspHttpSocket
{
public:
    RtspHttpSslSocket(boost::asio::io_context& ioContext,
                   const std::string &rtspUrl,
                   const std::string &userName,
                   const std::string &password);
    virtual ~RtspHttpSslSocket();
    virtual void Initialize(const unsigned int timeoutSec,
                            const unsigned short defaultPort,
                            const TransportMethod transportMethod);
    virtual void Close();

protected:
    bool IsConnected();
    void Connect();

    void HandleAsyncConnect(const boost::system::error_code& error);
    void HandleAsyncConnect2(const boost::system::error_code& error);
    void HandleAsyncConnect3(const boost::system::error_code& error);

private:
    void HandleAsyncReadForConnect2(const boost::system::error_code& error);
    void HandleAsyncWriteForConnect2(const boost::system::error_code& error, size_t byteTransferred);
    bool GetCertificate(bool preverified, boost::asio::ssl::verify_context& ctx);
    bool VerifyCertificate(bool preverified,
                           boost::asio::ssl::verify_context& ctx);
private:
    //*X509* m_cert;
    boost::asio::ssl::context m_sslRecvContext;
    boost::asio::ssl::context m_sslSendContext;
    std::shared_ptr<RtspHttpSslSocket> shared_from_this()
    {
        return std::static_pointer_cast<RtspHttpSslSocket>(RtspSocket::shared_from_this());
    }

};


}
}

#endif // OSCAR_RTSP_HTTP_SSL_SOCKET_HPP

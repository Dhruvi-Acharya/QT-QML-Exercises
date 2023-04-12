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

#ifndef OSCAR_RTSP_HTTP_SOCKET_HPP
#define OSCAR_RTSP_HTTP_SOCKET_HPP

#include <iostream>
#include <boost/pointer_cast.hpp>

#include "RtspSocket.h"

namespace Wisenet
{
namespace Rtsp
{


class RtspHttpSocket : public RtspSocket
{
public:
    RtspHttpSocket(boost::asio::io_context& ioContext,
                   const std::string &rtspUrl,
                   const std::string &userName,
                   const std::string &password);
    virtual ~RtspHttpSocket();
    virtual void Initialize(const unsigned int timeoutSec,
                            const unsigned short defaultPort,
                            const TransportMethod transportMethod);
    virtual void Close();

protected:
    void SendCurrentRequest();
    virtual bool IsConnected();
    virtual void Connect();

    virtual void MakeRequest(
            RtspSocketRequestPtr& request,
            std::ostringstream& stream);

    virtual void HandleAsyncConnect(const boost::system::error_code& error);
    virtual void HandleAsyncConnect2(const boost::system::error_code& error);

    void MakeHttpGetMessage(std::ostringstream& stream);
    void MakeHttpPostMessage(std::ostringstream& stream);

protected:
    std::string m_sessionCookie;

    std::shared_ptr<RtspHttpSocket> shared_from_this()
    {
        return std::static_pointer_cast<RtspHttpSocket>(RtspSocket::shared_from_this());
    }
};


}
}

#endif // OSCAR_RTSP_HTTP_SOCKET_HPP

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
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>

#include "RtspRequestResponse.h"
#include "Media/MediaSourceFrame.h"
#include "RtspSession/RtspSession.h"


namespace Wisenet
{
namespace Rtsp
{

// rtsp_URL  =   "rtsp://host [ ":" port ] [ abs_path ]
// host = A legal Internet host domain name of IP address

class RtspClient : public std::enable_shared_from_this<RtspClient>
{
public:
    explicit RtspClient(boost::asio::io_context& ioc);

    ~RtspClient();

    void Initialize(const RtspPlayControlType playControlType,
                    const std::string &url,
                    const TransportMethod transportMethod,
                    const std::string &userName,
                    const std::string &password,
                    const unsigned int timeoutSec = kDefaultRtspConnectionTimeout,
                    const std::string &bindIp = "");

    void SetIntermediateCallback(
            const RtspEventCallbackFunctor& eventCallback,
            const Media::MediaSourceFrameHandler& mediaCallback);

    void SetDisableGetParameter(bool disable);

    void Setup(const RtspSetupParam& setupParam,
               const RtspResponseHandler& responseCallback);
    void Play(const boost::optional<RtspPlaybackParam> playParam,
              const RtspResponseHandler& responseCallback);
    void QuickPlay(const RtspSetupParam& setupParam,
                       const boost::optional<RtspPlaybackParam> playParam,
                       const RtspResponseHandler& responseCallback);
    void Pause(const RtspResponseHandler& responseCallback);
    void Teardown(const RtspResponseHandler& responseCallback);

    void SetupBackChannel(const RtspResponseHandler& responseCallback);
    bool SendBackChannelData(const unsigned char *data, const unsigned int dataSize);


    void Close();

private:
    void pushRequest(const RtspCommandItemPtr& request);
    void executeRequest(const RtspCommandItemPtr& request);
    void completeRequest();
    void cancelAllRequest();


private:
    //RtspClient(const RtspClient&);
    //RtspClient& operator =(const RtspClient&);

    boost::asio::io_context& m_ioContext;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    std::shared_ptr<RtspSession> m_rtspSession;
    std::queue<RtspCommandItemPtr> m_requests;

};

typedef std::shared_ptr<RtspClient> RtspClientPtr;

}
}

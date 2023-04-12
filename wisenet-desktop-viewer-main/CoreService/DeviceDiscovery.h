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

#include "CoreService/CoreServiceRequestResponse.h"
#include "SunapiBroadcastProtocol.h"
#include <memory>
#include <vector>

namespace asio = boost::asio;
namespace device = Wisenet::Device;

namespace Wisenet
{
namespace Core
{

class DeviceDiscovery : public std::enable_shared_from_this<DeviceDiscovery>
{
public:
    DeviceDiscovery(asio::io_context& ioContext);
    ~DeviceDiscovery();

    void StartDiscovery(DiscoverDevicesRequestSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler);

private:
    void cancel();
    void wait(ResponseBaseHandler const& responseHandler);
    void aggregate(ResponseBaseHandler const& responseHandler);
    void discoverSunapi(DiscoverDevicesRequestSharedPtr const& request);

    void asyncRecvFrom();
    void onResponse(ResponseBaseHandler const& responseHandler,
                    const boost::system::error_code& ec, bool isContinue);

private:
    asio::io_context& m_ioContext;
    asio::strand<boost::asio::io_context::executor_type> m_strand;
    boost::asio::steady_timer m_waitTimer;
    boost::asio::steady_timer m_dataCollectTimer;
    std::vector<SendSocketPtr> m_sendSockets;
    std::unique_ptr<boost::asio::ip::udp::socket> m_recvSocket;
    boost::asio::ip::udp::endpoint m_senderEndpoint;
    std::vector<unsigned char> m_recvPackets;
    std::vector<DiscoveryDeviceInfo> m_discoveredDevices;
};

}
}

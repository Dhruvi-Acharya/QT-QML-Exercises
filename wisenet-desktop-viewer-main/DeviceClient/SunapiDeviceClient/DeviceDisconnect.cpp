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
#include "SunapiDeviceClientImpl.h"


namespace Wisenet
{
namespace Device
{

void SunapiDeviceClientImpl::DeviceDisconnect(DeviceDisconnectRequestSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler)
{
    (void)request;
    (void)responseHandler;
    asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        SLOG_INFO("Disconnect SunapiDeviceClient ");
        disconnect(DeviceStatusType::DisconnectedByService);
    }));
}
void SunapiDeviceClientImpl::disconnect(const DeviceStatusType disconnType)
{
    SLOG_DEBUG("Disconnect SunapiDeviceClient 1");
    closeAll();
    SLOG_DEBUG("Disconnect SunapiDeviceClient 2");
    m_repos->setDisconnected(disconnType);
    SLOG_DEBUG("Disconnect SunapiDeviceClient 3");

    auto deviceStatusEvent = std::make_shared<DeviceStatusEvent>();
    deviceStatusEvent->deviceID = m_deviceUUID;
    deviceStatusEvent->deviceStatus = m_repos->status();
    eventHandler(deviceStatusEvent);
    SLOG_INFO("Disconnect SunapiDeviceClient");
}

void SunapiDeviceClientImpl::closeAll()
{
    SLOG_INFO("SunapiDeviceClientImpl::closeAll start");
    asio::dispatch(m_strand, WeakCallback(shared_from_this(), [this]()
    {
        m_isRunningEventSession = false;
        m_needToMonitorEventSessionStatus = false;
        m_isFwupdating = false;
        m_httpEventSession->Close();
        SLOG_INFO("SunapiDeviceClientImpl::closeAll m_httpEventSession Close");
        m_httpPosEventSession->Close();
        //m_httpEventSession = std::make_shared<HttpSession>(m_deviceIoc);
        m_httpCommandSession->Close();
        m_httpPtzSession->Close();
        m_httpSearchMetaDataSession->Close();
        m_httpConfigSession->Close();
        m_httpChannelUpgradeSession->Close();
        SLOG_INFO("SunapiDeviceClientImpl::closeAll m_httpConfigSession Close");

        StopAiSearchStatusTimer();
        StopChangeConfigUriTimer();
        StopChannelUpdateTimer();

        SLOG_INFO("SunapiDeviceClientImpl::closeAll StopChangeConfigUriTimer end");

        closeAllMediaSession();

        SLOG_INFO("SunapiDeviceClientImpl::closeAll closeAllMediaSession end");

        m_easyConnection.StopP2PService();

        SLOG_INFO("SunapiDeviceClientImpl::closeAll StopP2PService end");

        if(m_s1sip != nullptr)
            m_s1sip->Disconnect();

        m_isReleased = true;
    }));
    SLOG_INFO("SunapiDeviceClientImpl::closeAll end");
}

void SunapiDeviceClientImpl::closeAllMediaSession()
{
    for(auto& mediaAgent : m_mediaAgentList)
        mediaAgent->close();
    m_mediaAgentList.clear();
}


}
}

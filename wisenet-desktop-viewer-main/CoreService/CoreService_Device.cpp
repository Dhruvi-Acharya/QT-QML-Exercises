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

#include <memory>
#include <functional>
#include "CoreService.h"
#include "CoreServiceUtil.h"
#include "UserSession.h"
#include <boost/core/ignore_unused.hpp>

#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"
#include "SunapiDeviceClient.h"
#include "DatabaseManager.h"
#include "PTZLogManager.h"
#include "EventRuleManager.h"

namespace Wisenet
{
namespace Core
{

using namespace std::placeholders;

device::BaseDeviceClientSharedPtr& CoreService::getDevice(const std::string& deviceUUID)
{
    auto itr = m_devices.find(deviceUUID);
    if (itr == m_devices.end()) {
        return m_dummyDevice;
    }
    return itr->second;
}

device::BaseDeviceClientSharedPtr CoreService::createDevice(const std::string& deviceUUID,
                                                            const device::DeviceProtocolType deviceProtocolType)
{
    if (deviceProtocolType == device::DeviceProtocolType::SunapiGeneric) {
        auto clientPtr = std::make_shared<device::SunapiDeviceClient>(deviceIoContext(), streamIoContext(), deviceUUID, m_cloudService,
                                                                        m_config.GetSunapiDeviceClientOption().keepAliveTime,
                                                                        m_config.GetSunapiDeviceClientOption().allowRelay, m_s1DaemonPort);
        clientPtr->SetDeviceEventHandler(WeakCallback(shared_from_this(), [this]
                                                      (const device::DeviceEventBaseSharedPtr& event)
        {
            SPDLOG_DEBUG("Receive a device event from the device={}", event->deviceID);
            processEvent(event);
        }));

        clientPtr->SetDeviceLatestStatusEventHandler(WeakCallback(shared_from_this(), [this]
                                                      (const device::DeviceEventBaseSharedPtr& event)
        {
            SPDLOG_DEBUG("Receive a device latest staus event from the device={}", event->deviceID);
            processEvent(event);
        }));

        return clientPtr;
    }
    return nullptr;
}


void CoreService::byPassToDevice(Wisenet::Device::memFuncType func,
                    device::DeviceRequestBaseSharedPtr const& deviceRequest,
                    ResponseBaseHandler const& responseHandler)
{

    asio::post(m_strand,
               WeakCallback(shared_from_this(),
                            [this, func, deviceRequest, responseHandler] ()
    {
        auto f = std::bind(func, getDevice(deviceRequest->deviceID), _1, _2);
        f(deviceRequest, WeakCallback(shared_from_this(),
                                      [this, responseHandler]
                                      (const ResponseBaseSharedPtr& response)
        {
            safeResponseHandler(response, responseHandler);
        }));
    }));

}

void CoreService::DeviceConnect(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceConnectRequest>(request);
    SPDLOG_DEBUG("DeviceConnect(), deviceUUID={}", deviceRequest->deviceID);

    // UI에서 DDNS 접속을 시도한 경우는 무조건 DDNS부터 접속시도를 하도록 connectedType을 DDNS로 변경한다.
    if ((device::ConnectionType::WisenetDDNS == deviceRequest->connectionInfo.connectionType)
        && deviceRequest->tryToDdnsInDDNSAndP2P) {
        deviceRequest->connectionInfo.connectedType = device::ConnectedType::WisenetDDNS;
        SPDLOG_DEBUG("DeviceConnect() . Changed connectedType to WisenetDDNS , deviceUUID={}", deviceRequest->deviceID);
    }

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, deviceRequest, responseHandler] ()
    {
        getDevice(deviceRequest->deviceID)->DeviceConnect(deviceRequest,
                                                          WeakCallback(shared_from_this(),
                                                                       [this, deviceRequest, responseHandler]
                                                                       (const ResponseBaseSharedPtr& response)
        {

            asio::dispatch(m_strand, WeakCallback(shared_from_this(),
                                                  [this, deviceRequest, response, responseHandler]()
            {
                auto res = std::make_shared<ResponseBase>();
                res->errorCode = response->errorCode;
                if (response->isFailed()) {
                    safeResponseHandler(res, responseHandler);

                    if(!deviceRequest->addDevice){
                        // 장치 추가하는 경우가 아니면 접속 시도하면서 실패가 난 경우 SunapiClient에서 보내지 않으므로
                        // 응답을 받아 시스템 이벤트를 보낸다.
                        Device::DeviceStatusType type = Device::DeviceStatusType::DisconnectedWithError;

                        if(Wisenet::ErrorCode::UnAuthorized == response->errorCode){
                            type = Device::DeviceStatusType::Unauthorized;
                        }else if(Wisenet::ErrorCode::RELAYConnectError == response->errorCode){
                            type = Device::DeviceStatusType::DisconnectedWithRestriction;
                        }
                        m_eventRuleManager->SendDeviceStatusEvent(deviceRequest->deviceID, type);

                    }

                    return;
                }


                Device::DeviceConnectResponseSharedPtr connectResponse =
                        std::static_pointer_cast<Device::DeviceConnectResponse>(response);

                bool bChangeChannel = false;

                if(response->isSuccess()){
                    auto& dbDevice = m_userSession->GetDevice(connectResponse->device.deviceID);

                     SPDLOG_DEBUG("DeviceConnect(), dbDevice.channels.size()={} connectResponse->device.channels.size()={} dbDevice.deviceType={}", dbDevice.channels.size(), connectResponse->device.channels.size(), dbDevice.deviceType);

                     if(dbDevice.channels.size() == connectResponse->device.channels.size() &&
                             dbDevice.deviceType == connectResponse->device.deviceType){
                        // Calculate high/low profile ID
                        SPDLOG_DEBUG("DeviceConnect(), success");

                        // get device name from database and update device&profile info in database
                        res->errorCode = m_userSession->ConnectDevice(connectResponse);
                    }
                    else if(dbDevice.channels.size() == 1 && connectResponse->device.channels.size() > 1){
                        SPDLOG_DEBUG("DeviceConnect(), success rearrange channelDB");
                        AssignChannelProfileInfo(connectResponse->deviceProfileInfo,
                                                 connectResponse->deviceStatus,
                                                 connectResponse->device);

                        std::vector<uuid_string> deviceIDs;
                        deviceIDs.push_back(dbDevice.deviceID);
                        std::map<uuid_string,UserGroup> changedUserGroups;
                        std::map<uuid_string, Layout> changedLayouts;
                        std::vector<uuid_string> removedBookmarks;
                        res->errorCode = m_userSession->RemoveDevices(deviceIDs,
                                                     changedUserGroups,
                                                     changedLayouts,
                                                     removedBookmarks);
                        SPDLOG_WARN("DeviceConnect(), rearrange removeDevice :{}", res->errorCode);
                        if (res->isFailed()) {
                            safeResponseHandler(res, responseHandler);
                            return;
                        }
                        res->errorCode = m_userSession->AddDevice(connectResponse);
                        SPDLOG_WARN("DeviceConnect(), rearrange AddDevice :{}", res->errorCode);
                        if (res->isFailed()) {
                            safeResponseHandler(res, responseHandler);
                            return;
                        }
                        bChangeChannel = true;
                    }

                    else{
                        SPDLOG_DEBUG("DeviceConnect(), DeviceMismatch");
                        res->errorCode = Wisenet::ErrorCode::DeviceMismatch;
                        auto disconnectRequest = std::make_shared<Wisenet::Device::DeviceDisconnectRequest>();
                        disconnectRequest->deviceID = connectResponse->device.deviceID;

                        DeviceDisconnect(disconnectRequest, nullptr);
                        safeResponseHandler(res, responseHandler);
                        return;
                    }
                }
                safeResponseHandler(res, responseHandler);

                if (response->isSuccess()) {
                    SPDLOG_DEBUG("DeviceConnect(), eventSuccess");
                    auto connectEvent = std::make_shared<Device::DeviceStatusEvent>();
                    connectEvent->deviceID = connectResponse->device.deviceID;
                    connectEvent->deviceStatus = connectResponse->deviceStatus;
                    connectEvent->device = connectResponse->device;
                    processEvent(connectEvent);

                    if(connectResponse->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::S1_SIP ||
                            connectResponse->device.connectionInfo.connectedType == Wisenet::Device::ConnectedType::S1_SIP_RELAY)
                    {
                        auto tryConnectingS1DdnsRequest = std::make_shared<Wisenet::RequestBase>();
                        getDevice(deviceRequest->deviceID)->DeviceTryConnectingS1DDNS(tryConnectingS1DdnsRequest,
                                                                                      [=](const ResponseBaseSharedPtr& reply)
                        {
                            if(reply->isSuccess())
                            {
                                SPDLOG_INFO("DeviceConnect(): Initially ConnectedType S1_SIP, but S1DDNS getting success, change connectedType to S1DDNS");
                                auto s1DeviceConnectionTypeChangeEvent = std::make_shared<Wisenet::Device::S1DeviceConnectionTypeChangeEvent>();
                                s1DeviceConnectionTypeChangeEvent->device = connectResponse->device;
                                s1DeviceConnectionTypeChangeEvent->device.connectionInfo.connectedType = Wisenet::Device::ConnectedType::S1_DDNS;
                                processEvent(s1DeviceConnectionTypeChangeEvent);
                            }
                        });

                    }
                }
                if(bChangeChannel){
                    auto event = std::make_shared<SaveChannelsEvent>();
                    for(auto& channel : connectResponse->device.channels)
                        event->channels.push_back(channel.second);
                    processEvent(event);
                }
            }));
        }));
    }));
}


void CoreService::DeviceDisconnect(RequestBaseSharedPtr const & request,
                                   ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceDisconnect(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceDisconnect, deviceRequest, responseHandler);
}


void CoreService::DeviceMediaOpen(RequestBaseSharedPtr const & request,
                                  ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceMediaOpen(), deviceUUID={}", deviceRequest->deviceID);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, deviceRequest, responseHandler] ()
    {

        auto deviceMediaOpenRequest = std::static_pointer_cast<device::DeviceMediaOpenRequest>(deviceRequest);



        auto device = m_databaseManager->GetDevice(deviceMediaOpenRequest->deviceID);
        auto channel = device.channels[deviceMediaOpenRequest->channelID];

        // live이고 profileId가 empty인 경우 liveLow, liveHigh 프로파일의 id를 찾아서 채워준다.
        if ((deviceMediaOpenRequest->mediaRequestInfo.streamType == Device::StreamType::liveHigh ||
                deviceMediaOpenRequest->mediaRequestInfo.streamType == Device::StreamType::liveLow) &&
                deviceMediaOpenRequest->mediaRequestInfo.profileId.empty()) {

            std::string realProfile =  (deviceMediaOpenRequest->mediaRequestInfo.streamType == Device::StreamType::liveHigh) ?  channel.highProfile :  channel.lowProfile;
            if (device.deviceType == Wisenet::Device::DeviceType::SunapiRecorder) {
                realProfile = m_databaseManager->GetRealProfileNumber(channel.deviceID, channel.channelID, realProfile);
            }

            deviceMediaOpenRequest->mediaRequestInfo.profileId = realProfile;
            SPDLOG_DEBUG("Set profile ID for Live streamming, deviceID={}, channelID={}, mediaID={}, profileId={}, high={}, low={}",
                         deviceMediaOpenRequest->deviceID,
                         deviceMediaOpenRequest->channelID,
                         deviceMediaOpenRequest->mediaID,
                         deviceMediaOpenRequest->mediaRequestInfo.profileId,
                         channel.highProfile, channel.lowProfile);

            if(deviceMediaOpenRequest->mediaRequestInfo.profileId == "" || deviceMediaOpenRequest->mediaRequestInfo.profileId .length() < 1){
                asio::post(m_strand,
                           WeakCallback(shared_from_this(), [this, responseHandler] ()
                {
                    SPDLOG_ERROR("Set profile ID for Live streamming, find fail real profile");
                    auto response = std::make_shared<ResponseBase>();
                    response->errorCode = Wisenet::ErrorCode::NetworkError;
                    safeResponseHandler(response, responseHandler);

                }));
                return;
            }
        }

        auto mediaID = deviceMediaOpenRequest->mediaID;
        auto mediaSession = m_userSession->CreateMediaSession(mediaID);
        // MediaSession 초기 설정
        mediaSession->Open(deviceMediaOpenRequest->mediaRequestInfo.streamType,
                           deviceMediaOpenRequest->mediaRequestInfo.trackId,
                           deviceMediaOpenRequest->deviceID,
                           deviceMediaOpenRequest->channelID,
                           deviceMediaOpenRequest->mediaRequestInfo.streamHandler);

        // streamHandler를 MediaSession의 Handler로 변경.
        deviceMediaOpenRequest->mediaRequestInfo.streamHandler = mediaSession->GetMediaSourceFrameHandler();


        getDevice(deviceRequest->deviceID)->DeviceMediaOpen(deviceRequest,
                                                            WeakCallback(shared_from_this(),
                                                                        [this, mediaID, responseHandler]
                                                                        (const ResponseBaseSharedPtr& response)
        {
            asio::post(m_strand,
                       WeakCallback(shared_from_this(), [this, mediaID, response, responseHandler] ()
            {
                // 실패인 경우 MediaSession 삭제
                if(!response->isSuccess()){
                    m_userSession->DeleteMediaSession(mediaID);
                }

                safeResponseHandler(response, responseHandler);
            }));
        }));

    }));


}

void CoreService::DeviceMediaClose(RequestBaseSharedPtr const & request,
                                   ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceMediaClose(), deviceUUID={}", deviceRequest->deviceID);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, deviceRequest, responseHandler] ()
    {

        auto deviceMediaCloseRequest = std::static_pointer_cast<device::DeviceMediaCloseRequest>(deviceRequest);
        auto mediaID = deviceMediaCloseRequest->mediaID;
        getDevice(deviceRequest->deviceID)->DeviceMediaClose(deviceRequest,
                                                            WeakCallback(shared_from_this(),
                                                                        [this, mediaID, responseHandler]
                                                                        (const ResponseBaseSharedPtr& response)
        {

            asio::post(m_strand,
                       WeakCallback(shared_from_this(), [this, mediaID, response, responseHandler] ()
            {
                // MediaSession을 Close하고 MediaSession을 삭제한다.
                auto mediaSession = m_userSession->GetMediaSession(mediaID);
                if(mediaSession) {
                    mediaSession->Close();
                    m_userSession->DeleteMediaSession(mediaID);
                }


                safeResponseHandler(response, responseHandler);
            }));
        }));

    }));
}

void CoreService::DeviceMediaControl(RequestBaseSharedPtr const & request,
                                     ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceMediaControl(), deviceUUID={}", deviceRequest->deviceID);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, deviceRequest, responseHandler] ()
    {


        auto deviceMediaConrolRequest = std::static_pointer_cast<device::DeviceMediaControlRequest>(deviceRequest);

        // SEEK 인경우 시간 변경된 정보를 MEDIASESSION에 알려준다.
        bool isSeeked = false;
        auto mediaID = deviceMediaConrolRequest->mediaID;
        if(device::ControlType::play == deviceMediaConrolRequest->mediaControlInfo.controlType) {
            auto mediaSession = m_userSession->GetMediaSession(mediaID);
            if(mediaSession) {
                mediaSession->Seek(deviceMediaConrolRequest->mediaControlInfo.time);
                isSeeked = true;
            }
        }

        getDevice(deviceRequest->deviceID)->DeviceMediaControl(deviceRequest,
                                                            WeakCallback(shared_from_this(),
                                                                        [this, isSeeked, mediaID, responseHandler]
                                                                        (const ResponseBaseSharedPtr& response)
        {
            if(isSeeked) {
                auto mediaSession = m_userSession->GetMediaSession(mediaID);
                if(mediaSession) {
                    mediaSession->OnSeek();
                }
            }
            safeResponseHandler(response, responseHandler);
        }));

    }));
}

void CoreService::DeviceTalk(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceTalk(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceTalk, deviceRequest, responseHandler);
}

void CoreService::DeviceSendTalkData(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    byPassToDevice(&device::IDeviceClient::DeviceSendTalkData, deviceRequest, responseHandler);
}

void CoreService::DeviceGetOverlappedId(RequestBaseSharedPtr const & request,
                                        ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetOverlappedId(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetOverlappedId, deviceRequest, responseHandler);
}

void CoreService::DeviceCalendarSearch(RequestBaseSharedPtr const & request,
                                       ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceCalendarSearch(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceCalendarSearch, deviceRequest, responseHandler);
}

void CoreService::DeviceGetTimeline(RequestBaseSharedPtr const & request,
                                    ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetTimeline(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetTimeline, deviceRequest, responseHandler);
}

void CoreService::DeviceGetRecordingPeriod(RequestBaseSharedPtr const & request,
                                           ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetRecordingPeriod(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetRecordingPeriod, deviceRequest, responseHandler);
}

void CoreService::DeviceGetAllPeriodTimeline(RequestBaseSharedPtr const & request,
                                             ResponseBaseHandler const & responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetAllPeriodTimeline(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetAllPeriodTimeline, deviceRequest, responseHandler);
}

void CoreService::DeviceChangeUserPassword(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceChangeUserPassword(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceChangeUserPassword, deviceRequest, responseHandler);
}

void CoreService::DeviceSnapshot(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceSnapshot(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSnapshot, deviceRequest, responseHandler);
}

void CoreService::DeviceThumbnail(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceThumbnail(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceThumbnail, deviceRequest, responseHandler);
}

void CoreService::DeviceConfigBackup(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceConfigBackup(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceConfigBackup, deviceRequest, responseHandler);
}

void CoreService::DeviceConfigRestore(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceConfigRestore(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceConfigRestore, deviceRequest, responseHandler);
}


void CoreService::DeviceFirmwareUpdate(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceFirmwareUpdate(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceFirmwareUpdate, deviceRequest, responseHandler);
}

void CoreService::DeviceChannelFirmwareUpdate(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceChannelFirmwareUpdate(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceChannelFirmwareUpdate, deviceRequest, responseHandler);
}

void CoreService::DevicePTZContinuous(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DevicePTZContinuous(), deviceUUID={},channelID={}", channelRequest->deviceID, channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DevicePTZContinuous, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID, channelRequest->channelID);

    }));
}

void CoreService::DevicePTZFocus(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DevicePTZFocus(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DevicePTZFocus, deviceRequest, responseHandler);
}

void CoreService::DeviceImageFocusMode(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceImageFocusMode(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceImageFocusMode, deviceRequest, responseHandler);
}

void CoreService::DevicePTZStop(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DevicePTZStopControl(), deviceUUID={},channelID={}", channelRequest->deviceID,channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DevicePTZStop, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID,channelRequest->channelID);

    }));
}

void CoreService::DeviceGetPreset(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetPreset(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetPreset, deviceRequest, responseHandler);
}

void CoreService::DeviceGetSwing(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetSwing(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetSwing, deviceRequest, responseHandler);
}

void CoreService::DeviceMoveStopSwing(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetSwing(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceMoveStopSwing, deviceRequest, responseHandler);
}

void CoreService::DeviceGetGroup(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetGroup(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetGroup, deviceRequest, responseHandler);
}

void CoreService::DeviceMoveGroup(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetSwing(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceMoveGroup, deviceRequest, responseHandler);
}

void CoreService::DeviceStopGroup(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetSwing(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceStopGroup, deviceRequest, responseHandler);
}

void CoreService::DeviceGetTour(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetTour(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetTour, deviceRequest, responseHandler);
}

void CoreService::DeviceMoveTour(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetTour(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceMoveTour, deviceRequest, responseHandler);
}

void CoreService::DeviceStopTour(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetTour(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceStopTour, deviceRequest, responseHandler);
}

void CoreService::DeviceGetTrace(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetTrace(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetTrace, deviceRequest, responseHandler);
}

void CoreService::DeviceMoveTrace(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceMoveTrace(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceMoveTrace, deviceRequest, responseHandler);
}

void CoreService::DeviceStopTrace(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceStopTrace(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceStopTrace, deviceRequest, responseHandler);
}

void CoreService::DeviceAddPreset(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceAddPreset(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceAddPreset, deviceRequest, responseHandler);
}

void CoreService::DeviceRemovePreset(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceRemovePreset(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceRemovePreset, deviceRequest, responseHandler);
}

void CoreService::DeviceMovePreset(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DeviceMovePreset(), deviceUUID={},channelID={}", channelRequest->deviceID,channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DeviceMovePreset, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID,channelRequest->channelID);

    }));
}

void CoreService::DevicePTZAuxControl(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DevicePTZAuxControl(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DevicePTZAuxControl, deviceRequest, responseHandler);
}

void CoreService::DeviceAreaZoom(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DeviceAreaZoom(), deviceUUID={},channelID={}", channelRequest->deviceID,channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DeviceAreaZoom, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID, channelRequest->channelID);

    }));
}

void CoreService::DeviceGetAutoTracking(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetAutoTracking(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetAutoTracking, deviceRequest, responseHandler);
}

void CoreService::DeviceSetAutoTracking(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DeviceSetAutoTracking(), deviceUUID={},channelID={}", channelRequest->deviceID, channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DeviceSetAutoTracking, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID,channelRequest->channelID);

    }));
}

void CoreService::DeviceSetDigitalAutoTracking(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceSetDigitalAutoTracking(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSetDigitalAutoTracking, deviceRequest, responseHandler);
}

void CoreService::DeviceSetTargetLockCoordinate(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto channelRequest = std::static_pointer_cast<device::ChannelRequestBase>(request);
    SPDLOG_DEBUG("DeviceSetTargetLockCoodinate(), deviceUUID={},channelID={}", channelRequest->deviceID,channelRequest->channelID);

    byPassToDevice(&device::IDeviceClient::DeviceSetTargetLockCoordinate, channelRequest, responseHandler);

    asio::post(m_strand,
               WeakCallback(shared_from_this(), [this, channelRequest, responseHandler] ()
    {
        auto auditLog = m_userSession->GetAuditLog();
        m_ptzLogManager->AddLog(auditLog.sessionID,auditLog.userName,auditLog.host,channelRequest->deviceID,channelRequest->channelID);

    }));
}

void CoreService::DeviceGetDefog(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetDefog(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetDefog, deviceRequest, responseHandler);
}

void CoreService::DeviceSetDefog(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceSetDefog(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSetDefog, deviceRequest, responseHandler);
}

void CoreService::DeviceSetAlarmOutput(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceSetAlarmOutput(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSetAlarmOutput, deviceRequest, responseHandler);
}

void CoreService::DeviceMetaAttributeSearch(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceMetaAttributeSearchRequest>(request);
    SPDLOG_DEBUG("DeviceMetaAttributeSearch(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceMetaAttributeSearch, deviceRequest, responseHandler);
}

void CoreService::DeviceAiSearchCancel(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceAiSearchCancel(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceAiSearchCancel, deviceRequest, responseHandler);
}

void CoreService::DeviceOcrSearch(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceMetaAttributeSearchRequest>(request);
    SPDLOG_DEBUG("DeviceOcrSearch(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceOcrSearch, deviceRequest, responseHandler);
}

void CoreService::DeviceUrlSnapShot(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceUrlSnapShotRequest>(request);
    SPDLOG_DEBUG("DeviceUrlSnapShot(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceUrlSnapShot, deviceRequest, responseHandler);
}

void CoreService::DeviceSmartSearch(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceSmartSearchRequest>(request);
    SPDLOG_DEBUG("DeviceSmartSearch(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSmartSearch, deviceRequest, responseHandler);
}

void CoreService::DeviceSmartSearchCancel(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceSmartSearchCancel(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceSmartSearchCancel, deviceRequest, responseHandler);
}

void CoreService::DeviceTextSearch(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceTextSearchRequest>(request);
    SPDLOG_DEBUG("DeviceTextSearch(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceTextSearch, deviceRequest, responseHandler);
}

void CoreService::DeviceTextPosConf(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceTextPosConfRequest>(request);
    SPDLOG_DEBUG("DeviceTextPosConf(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceTextPosConf, deviceRequest, responseHandler);
}

void CoreService::DeviceStorageInfo(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceStorageInfoRequest>(request);
    SPDLOG_DEBUG("DeviceStorageInfo(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceStorageInfo, deviceRequest, responseHandler);
}

void CoreService::DeviceGetPasswordExpiry(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceGetPasswordExpiry(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceGetPasswordExpiry, deviceRequest, responseHandler);
}

void CoreService::DeviceExtendPasswordExpiration(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceExtendPasswordExpiration(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceExtendPasswordExpiration, deviceRequest, responseHandler);
}

void CoreService::DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<device::DeviceRequestBase>(request);
    SPDLOG_DEBUG("DeviceTryConnectingS1DDNS(), deviceUUID={}", deviceRequest->deviceID);

    byPassToDevice(&device::IDeviceClient::DeviceTryConnectingS1DDNS, deviceRequest, responseHandler);
}


}
}

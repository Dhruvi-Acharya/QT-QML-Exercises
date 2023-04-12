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

#include "SunapiDeviceClient.h"
#include "SunapiDeviceClientImpl.h"


namespace Wisenet
{
namespace Device
{

SunapiDeviceClient::SunapiDeviceClient(asio::io_context &ioc,
                                       asio::io_context &streamIoc,
                                       std::string deviceUUID,
                                       std::shared_ptr<Wisenet::Library::CloudService> cloudService,
                                       long long keepAliveTime,
                                       bool allowRelay,
                                       unsigned short s1DaemonPort)
    : BaseDeviceClient(deviceUUID, DeviceProtocolType::SunapiGeneric)
    , m_impl(std::make_shared<SunapiDeviceClientImpl>(ioc, streamIoc, deviceUUID, cloudService, keepAliveTime, allowRelay, s1DaemonPort))
{

}

SunapiDeviceClient::~SunapiDeviceClient()
{
    m_impl.reset();
}

bool SunapiDeviceClient::IsReleased()
{
    return m_impl->IsReleased();
}

void SunapiDeviceClient::Release()
{
    m_impl->Release();
}

void SunapiDeviceClient::SetDeviceEventHandler(const DeviceEventBaseHandler &eventHandler)
{
    m_impl->SetDeviceEventHandler(eventHandler);
}

void SunapiDeviceClient::SetDeviceLatestStatusEventHandler(const DeviceEventBaseHandler &eventHandler)
{
    m_impl->SetDeviceLatestStatusEventHandler(eventHandler);
}

void SunapiDeviceClient::DeviceConnect(const RequestBaseSharedPtr &request,
                                       const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceConnectRequest>(request);
    m_impl->DeviceConnect(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceDisconnect(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceDisconnectRequest>(request);
    m_impl->DeviceDisconnect(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMediaOpen(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMediaOpenRequest>(request);
    m_impl->DeviceMediaOpen(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMediaClose(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMediaCloseRequest>(request);
    m_impl->DeviceMediaClose(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMediaControl(const RequestBaseSharedPtr &request,
                                            const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMediaControlRequest>(request);
    m_impl->DeviceMediaControl(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceTalk(const RequestBaseSharedPtr &request,
                                    const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceTalkRequest>(request);
    m_impl->DeviceTalk(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSendTalkData(const RequestBaseSharedPtr &request,
                                            const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSendTalkDataRequest>(request);
    m_impl->DeviceSendTalkData(deviceRequest);
}

void SunapiDeviceClient::DeviceGetOverlappedId(const RequestBaseSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetOverlappedIdRequest>(request);
    m_impl->DeviceGetOverlappedId(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceCalendarSearch(const RequestBaseSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceCalendarSearchRequest>(request);
    m_impl->DeviceCalendarSearch(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetTimeline(const RequestBaseSharedPtr &request,
                                           const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetTimelineRequest>(request);
    m_impl->DeviceGetTimeline(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetRecordingPeriod(const RequestBaseSharedPtr &request,
                                                  const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetRecordingPeriodRequest>(request);
    m_impl->DeviceGetRecordingPeriod(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetAllPeriodTimeline(const RequestBaseSharedPtr &request,
                                                    const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetAllPeriodTimelineRequest>(request);
    m_impl->DeviceGetAllPeriodTimeline(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceChangeUserPassword(const RequestBaseSharedPtr &request,
                                                  const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceChangeUserPasswordRequest>(request);
    m_impl->DeviceChangeUserPassword(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSnapshot(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSnapshotRequest>(request);
    m_impl->DeviceSnapshot(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceThumbnail(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceThumbnailRequest>(request);
    m_impl->DeviceThumbnail(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceConfigBackup(const RequestBaseSharedPtr &request,
                                            const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceConfigBackupRequest>(request);
    m_impl->DeviceConfigBackup(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceConfigRestore(const RequestBaseSharedPtr &request,
                                             const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceConfigRestoreRequest>(request);
    m_impl->DeviceConfigRestore(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceFirmwareUpdate(const RequestBaseSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceFirmwareUpdateRequest>(request);
    m_impl->DeviceFirmwareUpdate(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceChannelFirmwareUpdate(const RequestBaseSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceChannelFirmwareUpdateRequest>(request);
    m_impl->DeviceChannelFirmwareUpdate(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DevicePTZContinuous(const RequestBaseSharedPtr &request,
                                             const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DevicePTZContinuousRequest>(request);
    m_impl->DevicePTZContinuous(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DevicePTZFocus(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DevicePTZFocusRequest>(request);
    m_impl->DevicePTZFocus(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DevicePTZStop(const RequestBaseSharedPtr &request,
                                       const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DevicePTZStopRequest>(request);
    m_impl->DevicePTZStop(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceImageFocusMode(const RequestBaseSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceImageFocusModeRequest>(request);
    m_impl->DeviceImageFocusMode(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetPreset(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetPresetRequest>(request);
    m_impl->DeviceGetPreset(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceAddPreset(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceAddPresetRequest>(request);
    m_impl->DeviceAddPreset(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceRemovePreset(const RequestBaseSharedPtr &request,
                                            const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRemovePresetRequest>(request);
    m_impl->DeviceRemovePreset(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMovePreset(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMovePresetRequest>(request);
    m_impl->DeviceMovePreset(deviceRequest, responseHandler);
}

// swing
void SunapiDeviceClient::DeviceGetSwing(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceGetSwing(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMoveStopSwing(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMoveStopSwingRequest>(request);
    m_impl->DeviceMoveStopSwing(deviceRequest, responseHandler);
}

// group
void SunapiDeviceClient::DeviceGetGroup(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceGetGroup(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMoveGroup(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMoveGroupRequest>(request);
    m_impl->DeviceMoveGroup(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceStopGroup(const RequestBaseSharedPtr &request,
                                             const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceStopGroupRequest>(request);
    m_impl->DeviceStopGroup(deviceRequest, responseHandler);
}

// tour
void SunapiDeviceClient::DeviceGetTour(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceGetTour(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMoveTour(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceMoveTour(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceStopTour(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceStopTour(deviceRequest, responseHandler);
}

// trace
void SunapiDeviceClient::DeviceGetTrace(const RequestBaseSharedPtr &request,
                                       const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<ChannelRequestBase>(request);
    m_impl->DeviceGetTrace(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMoveTrace(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMoveTraceRequest>(request);
    m_impl->DeviceMoveTrace(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceStopTrace(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceStopTraceRequest>(request);
    m_impl->DeviceStopTrace(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DevicePTZAuxControl(const RequestBaseSharedPtr &request,
                                             const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DevicePTZAuxControlRequest>(request);
    m_impl->DevicePTZAuxControl(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceAreaZoom(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceAreaZoomRequest>(request);
    m_impl->DeviceAreaZoom(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetAutoTracking(const RequestBaseSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetAutoTrackingRequest>(request);
    m_impl->DeviceGetAutoTracking(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSetAutoTracking(const RequestBaseSharedPtr &request,
                                               const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSetAutoTrackingRequest>(request);
    m_impl->DeviceSetAutoTracking(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSetDigitalAutoTracking(const RequestBaseSharedPtr &request,
                                                      const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSetDigitalAutoTrackingRequest>(request);
    m_impl->DeviceSetDigitalAutoTracking(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSetTargetLockCoordinate(const RequestBaseSharedPtr &request,
                                                      const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSetTargetLockCoordinateRequest>(request);
    m_impl->DeviceSetTargetLockCoordinate(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetDefog(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceGetDefogRequest>(request);
    m_impl->DeviceGetDefog(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSetDefog(const RequestBaseSharedPtr &request,
                                        const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSetDefogRequest>(request);
    m_impl->DeviceSetDefog(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSetAlarmOutput(const RequestBaseSharedPtr &request,
                                              const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSetAlarmOutputRequest>(request);
    m_impl->DeviceSetAlarmOutput(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceMetaAttributeSearch(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMetaAttributeSearchRequest>(request);
    m_impl->DeviceMetaAttributeSearch(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceAiSearchCancel(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
    m_impl->DeviceAiSearchCancel(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceOcrSearch(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceMetaAttributeSearchRequest>(request);
    m_impl->DeviceOcrSearch(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceUrlSnapShot(const RequestBaseSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceUrlSnapShotRequest>(request);
    m_impl->DeviceUrlSnapShot(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSmartSearch(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceSmartSearchRequest>(request);
    m_impl->DeviceSmartSearch(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceSmartSearchCancel(const RequestBaseSharedPtr &request,
                                         const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
    m_impl->DeviceSmartSearchCancel(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceTextSearch(const RequestBaseSharedPtr &request,
                                          const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceTextSearchRequest>(request);
    m_impl->DeviceTextSearch(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceTextPosConf(const RequestBaseSharedPtr &request,
                                           const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceTextPosConfRequest>(request);
    m_impl->DeviceTextPosConf(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceStorageInfo(const RequestBaseSharedPtr &request,
                                           const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceStorageInfoRequest>(request);
    m_impl->DeviceStorageInfo(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceGetPasswordExpiry(const RequestBaseSharedPtr &request,
                                           const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
    m_impl->DeviceGetPasswordExpiry(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceExtendPasswordExpiration(const RequestBaseSharedPtr &request,
                                                 const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
    m_impl->DeviceExtendPasswordExpiration(deviceRequest, responseHandler);
}

void SunapiDeviceClient::DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr &request,
                                                   const ResponseBaseHandler &responseHandler)
{
    auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
    m_impl->DeviceTryConnectingS1DDNS(deviceRequest, responseHandler);
}


}
}

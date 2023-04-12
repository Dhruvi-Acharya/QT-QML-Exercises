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
#include "DeviceClient/IDeviceClient.h"
#include "CloudService.h"

namespace Wisenet
{
namespace Device
{

class SunapiDeviceClientImpl;

class SunapiDeviceClient : public BaseDeviceClient, public std::enable_shared_from_this<SunapiDeviceClient>
{
public:
    SunapiDeviceClient(boost::asio::io_context& ioc,
                       boost::asio::io_context& streamIoc,
                       std::string deviceUUID,
                       std::shared_ptr<Wisenet::Library::CloudService> cloudService,
                       long long keepAliveTime,
                       bool allowRelay,
                       unsigned short s1DaemonPort);
    ~SunapiDeviceClient();

    bool IsReleased() override;

    void Release() override;
    void SetDeviceEventHandler(DeviceEventBaseHandler const& eventHandler) override;
    void SetDeviceLatestStatusEventHandler(DeviceEventBaseHandler const& eventHandler) override;
    void DeviceConnect(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override;
    void DeviceDisconnect(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;
    void DeviceMediaOpen(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler ) override;
    void DeviceMediaClose(RequestBaseSharedPtr const& request ,
                          ResponseBaseHandler const& responseHandler ) override;
    void DeviceMediaControl(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override;
    void DeviceTalk( RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler )override;
    void DeviceSendTalkData(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetOverlappedId(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler ) override;
    void DeviceCalendarSearch(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetTimeline(RequestBaseSharedPtr const& request,
                           ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetRecordingPeriod(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetAllPeriodTimeline(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler ) override;
    void DeviceChangeUserPassword(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) override;
    void DeviceSnapshot(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;
    void DeviceThumbnail(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;
    void DeviceConfigBackup(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override;
    void DeviceConfigRestore(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) override;
    void DeviceFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override;
    void DeviceChannelFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override;
    void DevicePTZContinuous(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override;
    void DevicePTZFocus(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override;
    void DevicePTZStop(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler)  override;
    void DeviceImageFocusMode(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler)  override;
    void DeviceGetPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;
    void DeviceAddPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override;
    void DeviceRemovePreset(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override;
    void DeviceMovePreset(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override;
    // swing
    void DeviceGetSwing(const RequestBaseSharedPtr &request,
                        const ResponseBaseHandler &responseHandler) override;
    void DeviceMoveStopSwing(const RequestBaseSharedPtr &request,
                             const ResponseBaseHandler &responseHandler) override;
    // group
    void DeviceGetGroup(const RequestBaseSharedPtr &request,
                        const ResponseBaseHandler &responseHandler) override;
    void DeviceMoveGroup(const RequestBaseSharedPtr &request,
                             const ResponseBaseHandler &responseHandler) override;
    void DeviceStopGroup(const RequestBaseSharedPtr &request,
                             const ResponseBaseHandler &responseHandler) override;
    // tour
    void DeviceGetTour(const RequestBaseSharedPtr &request,
                        const ResponseBaseHandler &responseHandler) override;
    void DeviceMoveTour(const RequestBaseSharedPtr &request,
                         const ResponseBaseHandler &responseHandler) override;
    void DeviceStopTour(const RequestBaseSharedPtr &request,
                         const ResponseBaseHandler &responseHandler) override;
    // trace
    void DeviceGetTrace(const RequestBaseSharedPtr &request,
                       const ResponseBaseHandler &responseHandler) override;
    void DeviceMoveTrace(const RequestBaseSharedPtr &request,
                        const ResponseBaseHandler &responseHandler) override;
    void DeviceStopTrace(const RequestBaseSharedPtr &request,
                        const ResponseBaseHandler &responseHandler) override;
    void DevicePTZAuxControl(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override;
    void DeviceAreaZoom(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetAutoTracking(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler ) override;
    void DeviceSetAutoTracking(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler ) override;
    void DeviceSetDigitalAutoTracking(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) override;
    void DeviceSetTargetLockCoordinate(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler ) override;
    void DeviceGetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler)  override;
    void DeviceSetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler)  override;
    void DeviceSetAlarmOutput(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceMetaAttributeSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceAiSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceOcrSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceUrlSnapShot(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceSmartSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    void DeviceSmartSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override;
    // 2022.08.29. added
    void DeviceTextSearch(const RequestBaseSharedPtr& request,
                          const ResponseBaseHandler& responseHandler) override;
    // 2022.09.01. added
    void DeviceTextPosConf(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override;

    void DeviceStorageInfo(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override;

    void DeviceGetPasswordExpiry(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override;

    void DeviceExtendPasswordExpiration(const RequestBaseSharedPtr& request,
                                 const ResponseBaseHandler& responseHandler) override;

    void DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr& request,
                                 const ResponseBaseHandler& responseHandler) override;

private:
    std::shared_ptr<SunapiDeviceClientImpl> m_impl;
};

}
}

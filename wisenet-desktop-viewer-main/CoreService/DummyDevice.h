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
#include <boost/core/ignore_unused.hpp>
#include "DeviceClient/IDeviceClient.h"
#include "CoreServiceLogSettings.h"
#include "WeakCallback.h"

namespace Wisenet
{
namespace Device
{

class DummyDevice : public BaseDeviceClient, public std::enable_shared_from_this<DummyDevice>
{
public:
    explicit DummyDevice(boost::asio::io_context& ioc,
                         std::string deviceUUID="",
                         DeviceProtocolType deviceProtocolType = DeviceProtocolType::SunapiGeneric)
        : BaseDeviceClient(deviceUUID, deviceProtocolType)
        , m_strand(boost::asio::make_strand(ioc.get_executor()))
    {}
    ~DummyDevice()
    {}

    void onResponse(const RequestBaseSharedPtr& request,
                    const ResponseBaseHandler& responseHandler,
                    const ResponseBaseSharedPtr& response)
    {
        auto deviceRequest = std::static_pointer_cast<DeviceRequestBase>(request);
        SPDLOG_WARN("DummyDevice > Invalid deviceUUID=({})", deviceRequest->deviceID);

        if (responseHandler == nullptr)
            return;
        boost::asio::post(m_strand,
                          WeakCallback(shared_from_this(), [responseHandler, response] ()
        {
            response->errorCode = Wisenet::ErrorCode::InvalidRequest;
            responseHandler(response);
        }));
    }

    bool IsReleased() override
    {
        return true;
    }

    void Release() override
    {
        SPDLOG_WARN("Release dummy device, deviceUUID={}", m_deviceUUID);
    }

    void SetDeviceEventHandler(DeviceEventBaseHandler const& eventHandler)  override
    {
        boost::ignore_unused(eventHandler);
        SPDLOG_WARN("SetDeviceEventHandler for dummy device, deviceUUID={}", m_deviceUUID);
    }

    void DeviceConnect(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceConnectResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceDisconnect(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMediaOpen( RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMediaClose( RequestBaseSharedPtr const& request ,
                           ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMediaControl( RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceTalk( RequestBaseSharedPtr const& request,
                     ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSendTalkData( RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceChangeUserPassword(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetOverlappedId( RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceGetOverlappedIdResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceCalendarSearch( RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceCalendarSearchResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetTimeline( RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceGetTimelineResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetRecordingPeriod( RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceGetRecordingPeriodResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetAllPeriodTimeline( RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceGetAllPeriodTimelineResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSnapshot(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceSnapshotResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceThumbnail(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceThumbnailResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceConfigBackup(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceConfigRestore(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceFirmwareUpdateResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceChannelFirmwareUpdate(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceChannelFirmwareUpdateResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceAreaZoom(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetPresetResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetSwing(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetSwingResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMoveStopSwing(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetGroup(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetGroupResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMoveGroup(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceStopGroup(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetTour(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetTourResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMoveTour(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceStopTour(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetTrace(RequestBaseSharedPtr const& request,
                       ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetTraceResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMoveTrace(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceStopTrace(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetAutoTracking(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<DeviceGetAutoTrackingResponse>();
        onResponse(request, responseHandler, response);
    }
    void DeviceAddPreset(RequestBaseSharedPtr const& request,
                         ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceRemovePreset(RequestBaseSharedPtr const& request,
                            ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceMovePreset(RequestBaseSharedPtr const& request,
                          ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DevicePTZContinuous(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSetAutoTracking(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DevicePTZFocus(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceSetTargetLockCoordinate(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DevicePTZStop(RequestBaseSharedPtr const& request,
                              ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceImageFocusMode(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DevicePTZAuxControl(RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSetDigitalAutoTracking(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler ) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceGetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetDefogResponse>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSetDefog(RequestBaseSharedPtr const& request,
                        ResponseBaseHandler const& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSetAlarmOutput(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceMetaAttributeSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceAiSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceOcrSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceUrlSnapShot(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceSmartSearch(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }
    void DeviceSmartSearchCancel(const RequestBaseSharedPtr& request,
                              const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    // 2022.08.26. added
    void DeviceTextSearch(const RequestBaseSharedPtr& request,
                          const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    // 2022.09.01. added
    void DeviceTextPosConf(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    // 2022.10.20. added
    void DeviceStorageInfo(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<DeviceStorageInfoResponse>();
        onResponse(request, responseHandler, response);
    }

    // 2022.11.18. added
    void DeviceGetPasswordExpiry(const RequestBaseSharedPtr& request,
                           const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<DeviceGetPasswordExpiryResponse>();
        onResponse(request, responseHandler, response);
    }

    // 2022.11.23. added
    void DeviceExtendPasswordExpiration(const RequestBaseSharedPtr& request,
                                 const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

    void DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr& request,
                                 const ResponseBaseHandler& responseHandler) override
    {
        auto response = std::make_shared<ResponseBase>();
        onResponse(request, responseHandler, response);
    }

private:
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
};

}
}

/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once

#include <functional>
#include <memory>
#include <boost/asio.hpp>
#include <boost/core/ignore_unused.hpp>
#include "DeviceClient/DeviceRequestResponse.h"
#include "DeviceClient/DeviceEvent.h"

namespace Wisenet
{
namespace Device
{


class IDeviceClient
{
public:
    // Connect to device
    // REQ :: DeviceConnectRequest
    // RES :: DeviceConnectResponse
    virtual void DeviceConnect(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) = 0;

    // Disconnect to device
    // REQ :: DeviceDisconnectRequest
    // RES :: ResponseBase
    virtual void DeviceDisconnect(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    // Open media streaming
    // REQ :: DeviceMediaOpenRequest
    // RES :: ResponseBase
    virtual void DeviceMediaOpen(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler ) = 0;

    // Close media streaming
    // REQ :: DeviceMediaCloseRequest
    // RES :: ResponseBase
    virtual void DeviceMediaClose(RequestBaseSharedPtr const& request ,
                                  ResponseBaseHandler const& responseHandler ) = 0;

    // Control playback media streaming
    // REQ :: DeviceMediaControlRequest
    // RES :: ResponseBase
    virtual void DeviceMediaControl(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler ) = 0;

    // Bidirectional Audio (Talk ON/OFF)
    // REQ :: DeviceTalkRequest
    // RES :: ResponseBase
    virtual void DeviceTalk( RequestBaseSharedPtr const& request,
                             ResponseBaseHandler const& responseHandler ) = 0;

    // Bidirectional Audio (Send audio data when Talk ON)
    // REQ :: DeviceSendTalkDataRequest
    // RES :: ResponseBase
    virtual void DeviceSendTalkData(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler ) = 0;

    // Overlapped recording timeline ID
    // REQ :: DeviceGetOverlappedIdRequest
    // RES :: DeviceGetOverlappedIdResponse
    virtual void DeviceGetOverlappedId(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler ) = 0;

    // Search recording month/day
    // REQ :: DeviceCalendarSearchRequest
    // RES :: DeviceCalendarSearchResponse
    virtual void DeviceCalendarSearch(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) = 0;

    // Search recording timeline
    // REQ :: DeviceGetTimelineRequest
    // RES :: DeviceGetTimelineResponse
    virtual void DeviceGetTimeline(RequestBaseSharedPtr const& request,
                                   ResponseBaseHandler const& responseHandler ) = 0;

    // Search recording start-end period
    // REQ :: DeviceGetRecordingPeriodRequest
    // RES :: DeviceGetRecordingPeriodResponse
    virtual void DeviceGetRecordingPeriod(RequestBaseSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler ) = 0;

    // Search recording timeline from all recording period
    // REQ :: DeviceGetAllPeriodTimelineRequest
    // RES :: DeviceGetAllPeriodTimelineResponse
    virtual void DeviceGetAllPeriodTimeline(RequestBaseSharedPtr const& request,
                                            ResponseBaseHandler const& responseHandler ) = 0;

    // Change device login password
    // REQ :: DeviceChangeUserPasswordRequest
    // RES :: ResponseBase
    virtual void DeviceChangeUserPassword(RequestBaseSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler) = 0;

    // Get jpeg snapshot of the camera
    // REQ :: DeviceSnapshotRequest
    // RES :: DeviceSnapshotResponse
    virtual void DeviceSnapshot(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler ) = 0;

    // Get jpeg snapshot of the camera
    // REQ :: DeviceThumbnailRequest
    // RES :: DeviceThumbnailResponse
    virtual void DeviceThumbnail(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler ) = 0;

    // Backup device configuration
    // REQ :: DeviceConfigBackupRequest
    // RES :: ResponseBase
    virtual void DeviceConfigBackup(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler ) = 0;

    // Restore device configuration
    // REQ :: DeviceConfigRestoreRequest
    // RES :: ResponseBase
    virtual void DeviceConfigRestore(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler ) = 0;

    // Update device firmware
    // REQ :: DeviceFirmwareUpdateRequest
    // RES :: DeviceFirmwareUpdateResponse
    virtual void DeviceFirmwareUpdate(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) = 0;

    // Update channel firmware
    // REQ :: DeviceFirmwareUpdateRequest
    // RES :: DeviceFirmwareUpdateResponse
    virtual void DeviceChannelFirmwareUpdate(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler ) = 0;

    // PTZ Continous control
    // REQ :: DevicePTZContinuousRequest
    // RES :: ResponseBase
    virtual void DevicePTZContinuous(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;

    // PTZ Continous control
    // REQ :: DevicePTZFocusRequest
    // RES :: ResponseBase
    virtual void DevicePTZFocus(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;

    // PTZ preset list
    // REQ :: DeviceGetPresetRequest
    // RES :: DeviceGetPresetResponse
    virtual void DeviceGetPreset(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // PTZ Stop control
    // REQ :: DevicePTZStopRequest
    // RES :: ResponseBase
    virtual void DevicePTZStop(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler)  = 0;

    // Image Focus Mode control
    // REQ :: DeviceImageFocusModeRequest
    // RES :: ResponseBase
    virtual void DeviceImageFocusMode(RequestBaseSharedPtr const& request,
                                      ResponseBaseHandler const& responseHandler) = 0;

    // PTZ add a preset
    // REQ :: DeviceAddPresetRequest
    // RES :: ResponseBase
    virtual void DeviceAddPreset(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // PTZ remove a preset
    // REQ :: DeviceRemovePresetRequest
    // RES :: ResponseBase
    virtual void DeviceRemovePreset(RequestBaseSharedPtr const& request,
                                    ResponseBaseHandler const& responseHandler) = 0;

    // PTZ move to the preset
    // REQ :: DeviceMovePresetRequest
    // RES :: ResponseBase
    virtual void DeviceMovePreset(RequestBaseSharedPtr const& request,
                                  ResponseBaseHandler const& responseHandler) = 0;

    /////// new feature of ptz --START--
    // PTZ swing list
    // REQ :: DeviceGetSwingRequest
    // RES :: DeviceGetSwingResponse
    virtual void DeviceGetSwing(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;

    // PTZ swing move or stop
    // REQ :: DeviceGetSwingRequest
    // RES :: DeviceGetSwingResponse
    virtual void DeviceMoveStopSwing(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;
    // PTZ group list
    // REQ :: DeviceGetGroupRequest
    // RES :: DeviceGetGroupResponse
    virtual void DeviceGetGroup(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;
    // PTZ group move or stop
    // REQ :: DeviceGetGroupRequest
    // RES :: DeviceGetGroupResponse
    virtual void DeviceMoveGroup(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;
    // PTZ group move or stop
    // REQ :: DeviceGetGroupRequest
    // RES :: DeviceGetGroupResponse
    virtual void DeviceStopGroup(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;
    // PTZ tour list
    // REQ :: DeviceGetTourRequest
    // RES :: DeviceGetTourResponse
    virtual void DeviceGetTour(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;
    // PTZ tour move
    // REQ :: DeviceGetTourRequest
    // RES :: DeviceGetTourResponse
    virtual void DeviceMoveTour(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;
    // PTZ tour stop
    // REQ :: DeviceGetTourRequest
    // RES :: DeviceGetTourResponse
    virtual void DeviceStopTour(RequestBaseSharedPtr const& request,
                                 ResponseBaseHandler const& responseHandler) = 0;
    // PTZ trace list
    // REQ :: DeviceGetTraceRequest
    // RES :: DeviceGetTraceResponse
    virtual void DeviceGetTrace(RequestBaseSharedPtr const& request,
                               ResponseBaseHandler const& responseHandler) = 0;
    // PTZ trace move
    // REQ :: DeviceGetTraceRequest
    // RES :: DeviceGetTraceResponse
    virtual void DeviceMoveTrace(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;
    // PTZ trace stop
    // REQ :: DeviceGetTraceRequest
    // RES :: DeviceGetTraceResponse
    virtual void DeviceStopTrace(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler) = 0;
    /////// new feature of ptz --END--

    // PTZ Aux command
    // REQ :: DevicePTZAuxControlRequest
    // RES :: ResponseBase
    virtual void DevicePTZAuxControl(RequestBaseSharedPtr const& request,
                                     ResponseBaseHandler const& responseHandler) = 0;


    // PTZ Areazoom
    // REQ :: DeviceAreaZoomRequest
    // RES :: ResponseBase
    virtual void DeviceAreaZoom(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler ) = 0;

    // Get autotracking status
    // REQ :: DeviceGetAutoTrackingRequest
    // RES :: DeviceGetAutoTrackingResponse
    virtual void DeviceGetAutoTracking(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler ) = 0;

    // Set autotracking status
    // REQ :: DeviceSetAutoTrackingRequest
    // RES :: ResponseBase
    virtual void DeviceSetAutoTracking(RequestBaseSharedPtr const& request,
                                       ResponseBaseHandler const& responseHandler ) = 0;

    // Set autotracking status
    // REQ :: DeviceSetDigitalAutoTrackingRequest
    // RES :: ResponseBase
    virtual void DeviceSetDigitalAutoTracking(RequestBaseSharedPtr const& request,
                                              ResponseBaseHandler const& responseHandler ) = 0;
    // Set targetlock coordinate when autotracking
    // REQ :: DeviceSetTargetLockCoordinateRequest
    // RES :: ResponseBase
    virtual void DeviceSetTargetLockCoordinate(RequestBaseSharedPtr const& request,
                                               ResponseBaseHandler const& responseHandler ) = 0;

    // Get defog value
    // REQ :: DeviceGetDefogRequest
    // RES :: DeviceGetDefogResponse
    virtual void DeviceGetDefog(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler)  = 0;

    // Set defog value
    // REQ :: DeviceSetDefogRequest
    // RES :: ResponseBase
    virtual void DeviceSetDefog(RequestBaseSharedPtr const& request,
                                ResponseBaseHandler const& responseHandler)  = 0;

    // Set defog value
    // REQ :: DeviceSetAlarmOutputRequest
    // RES :: ResponseBase
    virtual void DeviceSetAlarmOutput(const RequestBaseSharedPtr& request,
                                      const ResponseBaseHandler& responseHandler) = 0;

    // MetaAttributeSearch value
    // REQ :: DeviceMetaAttributeSearchRequest
    // RES :: ResponseBase
    virtual void DeviceMetaAttributeSearch(const RequestBaseSharedPtr& request,
                                      const ResponseBaseHandler& responseHandler) = 0;

    // MetaAttributeSearchCancel value
    // REQ :: DeviceRequestBase
    // RES :: ResponseBase
    virtual void DeviceAiSearchCancel(const RequestBaseSharedPtr& request,
                                           const ResponseBaseHandler& responseHandler) = 0;

    // OcrSearch value
    // REQ :: DeviceOcrSearchRequest
    // RES :: ResponseBase
    virtual void DeviceOcrSearch(const RequestBaseSharedPtr& request,
                                      const ResponseBaseHandler& responseHandler) = 0;

    // UrlSnapShot value
    // REQ :: DeviceUrlSnapShotRequest
    // RES :: ResponseBase
    virtual void DeviceUrlSnapShot(const RequestBaseSharedPtr& request,
                                      const ResponseBaseHandler& responseHandler) = 0;

    // SmartSearch value
    // REQ :: SmartSearchRequest
    // RES :: ResponseBase
    virtual void DeviceSmartSearch(const RequestBaseSharedPtr& request,
                                      const ResponseBaseHandler& responseHandler) = 0;

    // SmartSearchCancel value
    // REQ :: DeviceRequestBase
    // RES :: ResponseBase
    virtual void DeviceSmartSearchCancel(const RequestBaseSharedPtr& request,
                                           const ResponseBaseHandler& responseHandler) = 0;

    // TextSearch value
    // REQ :: DeviceTextSearchRequest
    // RES :: ResponseBase
    virtual void DeviceTextSearch(const RequestBaseSharedPtr& request,
                                  const ResponseBaseHandler& responseHandler) = 0;

    // TextPosConf value
    // REQ :: DeviceTextSearchRequest
    // RES :: ResponseBase
    virtual void DeviceTextPosConf(const RequestBaseSharedPtr& request,
                                   const ResponseBaseHandler& responseHandler) = 0;

    // Storage
    // REQ :: DeviceStorageInfoRequest
    // RES :: R
    virtual void DeviceStorageInfo(const RequestBaseSharedPtr& request,
                                   const ResponseBaseHandler& responseHandler) = 0;

    // Storage
    // REQ :: DeviceGetPasswordExpiry
    // RES :: R
    virtual void DeviceGetPasswordExpiry(const RequestBaseSharedPtr& request,
                                   const ResponseBaseHandler& responseHandler) = 0;

    // Storage
    // REQ :: DeviceGetPasswordExpiry
    // RES :: R
    virtual void DeviceExtendPasswordExpiration(const RequestBaseSharedPtr& request,
                                         const ResponseBaseHandler& responseHandler) = 0;

    // Try S1DDNS
    // REQ :: DeviceRequestBase
    // RES :: ResponseBase
    virtual void DeviceTryConnectingS1DDNS(const RequestBaseSharedPtr& request,
                                           const ResponseBaseHandler& responseHandler) = 0;
};


class BaseDeviceClient : public IDeviceClient
{
public:
    explicit BaseDeviceClient(std::string deviceUUID="",
                           DeviceProtocolType deviceProtocolType = DeviceProtocolType::SunapiGeneric)
        : m_deviceUUID(deviceUUID), m_deviceProtocolType(deviceProtocolType)
    {}
    virtual ~BaseDeviceClient() = default;

    // It is true if it is released. (It can only be used while using the Release function)
    virtual bool IsReleased()
    {
        return true;
    }

    // Release managed objects like event handler.
    virtual void Release()
    {

    }

    // Set event handler
    virtual void SetDeviceEventHandler(DeviceEventBaseHandler const& eventHandler)
    {
        boost::ignore_unused(eventHandler);
    }

    virtual void SetDeviceLatestStatusEventHandler(DeviceEventBaseHandler const& eventHandler)
    {
        boost::ignore_unused(eventHandler);
    }


    virtual DeviceProtocolType GetDevicePrtocolType() { return m_deviceProtocolType; }
    virtual std::string& GetDeviceUUID() { return m_deviceUUID; }
    virtual bool tryConnectingS1DDNS() { return false; }

protected:
    std::string m_deviceUUID;
    DeviceProtocolType m_deviceProtocolType = DeviceProtocolType::SunapiGeneric;
};

typedef std::shared_ptr<BaseDeviceClient> BaseDeviceClientSharedPtr;

typedef void(IDeviceClient::*memFuncType)(RequestBaseSharedPtr const& request,
                                          ResponseBaseHandler const& responseHandler);

}
}

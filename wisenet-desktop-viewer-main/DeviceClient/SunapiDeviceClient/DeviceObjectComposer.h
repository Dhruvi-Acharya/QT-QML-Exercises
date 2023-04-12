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
#include "ConfigUriComposer.h"
#include "DeviceClient/DeviceStructure.h"
#include "DeviceClient/DeviceEvent.h"
#include "cgis/SystemCgi.h"
#include "cgis/NetworkCgi.h"
#include "cgis/AttributesCgi.h"
#include "cgis/MediaCgi.h"
#include "cgis/RecordingCgi.h"
#include "cgis/EventSourcesCgi.h"
#include "cgis/IoCgi.h"
#include "cgis/EventStatusCgi.h"

namespace Wisenet
{
namespace Device
{

struct DeviceObjectComposerEvent
{
    DeviceStatusEventSharedPtr  deviceStatusEvent;
    ChannelStatusEventSharedPtr channelStatusEvent;
    DeviceProfileInfoEventSharedPtr profileInfoEvent;
};

class DeviceObjectComposer
{
public:
    DeviceObjectComposer();

    // set cgi result into composer
    void setSystemDeviceInfoView(const SystemDeviceInfoViewResult& viewResult);
    void setNetworkInterfaceView(const NetworkInterfaceViewResult& viewResult);
    void setSystemDateView(const SystemDateViewResult& viewResult);
    void setMediaVideoSourceView(const MediaVideoSourceViewResult& viewResult);
    void setMediaVideoProfileView(const MediaVideoProfileViewResult& viewResult);
    void setMediaVideoProfilePolicyView(const MediaVideoProfilePolicyViewResult& viewResult);
    void setRecordingStorageView(const RecordingStorageViewResult& viewResult);
    void setRecordingGeneralView(const RecordingGeneralViewResult& viewResult);
    void setAlarmOutputView(const IoAlarmOutputViewResult& viewResult);
    void setAlarmInputView(const EventSourcesAlarmInputViewResult& viewResult);
    void setNetworkAlarmInputView(const EventSourcesNetworkAlarmInputViewResult& viewResult);
    void setMediaAudioOutputView(const MediaAudioOutputViewResult& viewResult);
    void setMediaCameraRegisterView(const MediaCameraRegisterViewResult& viewResult);
    void setMediaCameraUpgradeView(const MediaCameraUpgradeViewResult &viewResult);
    void setAttributes(const AttributesResult& attributesResult);
    void setSessionKey(const DevcieSessionKey& deviceSessionKey);
    void releaseSessionKey(const std::string& playbackSessionId);
    void setEventStatusSchema(const EventStatusSchemaViewResult& viewResult);
    void setRecordingTextPosConfView(const std::vector<TextPosConfResult> viewResult);

    void setDisconnected(const DeviceStatusType disconnType);

    void setDeviceCertificateResult(bool sameMacAddress, const std::string certResult);

    // make a new device object with cgi results
    const void rebuildDevice(const std::string& deviceUUID,
                             const DeviceConnectInfo& connInfo);
    const void rebuildProfileInfo(const std::string& deviceUUID);
    const DeviceObjectComposerEvent rebuildDeviceWithConfigUri(const ConfigUriComposerPtr &cuc);

    // get cgi result from composer
    const AttributesResult& attributes();
    const Device& device();
    const MediaVideoProfileViewResult& videoProfileView();
    const MediaVideoProfilePolicyViewResult& videoProfilePolicyView();
    const std::vector<DevcieSessionKey>& sessionKeys();
    const SystemDeviceInfoViewResult deviceInfoView();

    const DeviceStatus& status();
    const DeviceProfileInfo& profileInfo();

    bool isRecorder();
    bool isCamera();
    bool isPosEnabled(){return m_posEnabled;}
private:
    void setDeviceCapability(Device::DeviceCapability &deviceCap);
    void setChannelCapability(Device::Channel& channel);

private:
    std::vector<DevcieSessionKey>       m_sessionKeys;
    SystemDeviceInfoViewResult          m_deviceInfoView;
    NetworkInterfaceViewResult          m_networkInterfaceView;
    SystemDateViewResult                m_dateView;
    MediaVideoSourceViewResult          m_videoSourceView;
    MediaVideoProfileViewResult         m_videoProfileView;
    MediaVideoProfilePolicyViewResult   m_videoProfilePolicyView;
    AttributesResult                    m_attributes;
    RecordingStorageViewResult          m_recordingStorageView;
    RecordingGeneralViewResult          m_recordingGeneralView;
    IoAlarmOutputViewResult             m_ioAlarmOutputView;
    EventSourcesAlarmInputViewResult    m_alarmInputView;
    EventSourcesNetworkAlarmInputViewResult m_networkAlarmInputView;
    MediaAudioOutputViewResult          m_audioOutputView;
    MediaCameraRegisterViewResult       m_cameraRegisterView;
    MediaCameraUpgradeViewResult        m_cameraUpgradeView;
    EventStatusSchemaViewResult         m_eventStatusSchemaView;
    std::vector<TextPosConfResult>      m_posConfView;
    bool m_posEnabled = false;

    std::string m_deviceCerticateResult;

    /* Device structure */
    Device                              m_device;

    /* Device status */
    DeviceStatus                        m_status;

    /* Device Profile Info */
    DeviceProfileInfo                   m_profileInfo;
};

typedef std::shared_ptr<DeviceObjectComposer> DeviceObjectComposerPtr;

}
}

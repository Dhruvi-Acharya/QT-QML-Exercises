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
#include "DeviceObjectComposer.h"
#include "SunapiDeviceClientUtil.h"
#include "LogSettings.h"
#include "TimeUtil.h"
#include "FisheyeUtil.h"
#include <boost/algorithm/string.hpp>
#include <map>
#include <cstdlib>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[SunapiClient] "}, level, __VA_ARGS__)


namespace Wisenet
{
namespace Device
{


static ChannelStatusType ConvertChannelStatus(const DeviceType deviceType, const std::string& videoSourceState)
{
    if (boost::iequals(videoSourceState, "Off")) {
        if (deviceType == DeviceType::SunapiRecorder)
            return ChannelStatusType::Disconnected;
        return ChannelStatusType::Deactivated;
    }
    else if (videoSourceState.empty() ||
             boost::iequals(videoSourceState, "On") ||
             boost::iequals(videoSourceState, "Plugin")) {
        return ChannelStatusType::Connected;
    }
    else if (boost::iequals(videoSourceState, "Covert1") ||
             boost::iequals(videoSourceState, "Covert2") ||
             boost::iequals(videoSourceState, "Deactive")) {
        return ChannelStatusType::Deactivated;
    }
    else {
        return ChannelStatusType::Disconnected;
    }
}

static Media::VideoCodecType ConvertVideoCoecType(const std::string& encoding)
{
    if (boost::iequals(encoding, "MJPEG"))
        return Media::VideoCodecType::MJPEG;
    else if (boost::iequals(encoding, "H264"))
        return Media::VideoCodecType::H264;
    else if (boost::iequals(encoding, "H265"))
        return Media::VideoCodecType::H265;
    else if (boost::iequals(encoding, "MPEG4"))
        return Media::VideoCodecType::MPEG4;
    return Media::VideoCodecType::UNKNOWN;
}

static void ConvertVideoProfileViewInfo(
        const MediaVideoProfileViewResult::VideoProfileInfo &info,
        VideoProfile& retProfile)
{
    retProfile.profileName = info.name;
    retProfile.resolution = info.resolution;
    retProfile.codecType = ConvertVideoCoecType(info.encodingType);
    retProfile.framerate = info.framerate;
    retProfile.bitrate = info.bitrate;
    retProfile.compressionLevel = info.compressionLevel;
    retProfile.h26xGovLength = info.h26xGovLength;
    retProfile.h26xProfile = info.h26xProfile;
    retProfile.h26xBitrateControlType = info.h26xBitrateControlType;
}

static void ConvertRecorderVideoProfileViewInfo(
        const int profileID,
        const std::map<int, MediaVideoProfileViewResult::VideoProfileInfo> &infoMap,
        VideoProfile& retProfile)
{
    auto itr = infoMap.find(profileID);
    if (itr == infoMap.end()) {
        SPDLOG_INFO("Can not find profileID={}", profileID);
        return;
    }
    auto& info = itr->second;
    ConvertVideoProfileViewInfo(info, retProfile);
}

static void printProfileInfo(const VideoProfile& profile)
{
    if (spdlog::should_log(spdlog::level::debug)) {
        SPDLOG_DEBUG("DEVICE={}, CHANNEL={}, PROFILE={}, NAME={}, ALIAS({}:{}), CODEC={}, Resolution={}x{}, Framerate={}",
                     profile.deviceID, profile.channelID, profile.profileID,
                     profile.profileName,
                     profile.aliasID, profile.aliasName,
                     profile.codecType,
                     profile.resolution.width, profile.resolution.height,
                     profile.framerate);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
DeviceObjectComposer::DeviceObjectComposer()
{

}

void DeviceObjectComposer::setSystemDeviceInfoView(const SystemDeviceInfoViewResult &viewResult)
{
    m_deviceInfoView = viewResult;
}

void DeviceObjectComposer::setNetworkInterfaceView(const NetworkInterfaceViewResult &viewResult)
{
    m_networkInterfaceView = viewResult;
}

void DeviceObjectComposer::setSystemDateView(const SystemDateViewResult &viewResult)
{
    m_dateView = viewResult;
}


void DeviceObjectComposer::setMediaVideoSourceView(const MediaVideoSourceViewResult &viewResult)
{
   m_videoSourceView = viewResult;
}

void DeviceObjectComposer::setMediaVideoProfileView(const MediaVideoProfileViewResult& viewResult)
{
    m_videoProfileView = viewResult;
}

void DeviceObjectComposer::setMediaVideoProfilePolicyView(const MediaVideoProfilePolicyViewResult& viewResult)
{
    m_videoProfilePolicyView = viewResult;
}

void DeviceObjectComposer::setRecordingStorageView(const RecordingStorageViewResult &viewResult)
{
    m_recordingStorageView = viewResult;
}

void DeviceObjectComposer::setRecordingGeneralView(const RecordingGeneralViewResult &viewResult)
{
    m_recordingGeneralView = viewResult;
}

void DeviceObjectComposer::setAlarmOutputView(const IoAlarmOutputViewResult &viewResult)
{
    m_ioAlarmOutputView = viewResult;
}

void DeviceObjectComposer::setAlarmInputView(const EventSourcesAlarmInputViewResult &viewResult)
{
    m_alarmInputView = viewResult;
}

void DeviceObjectComposer::setNetworkAlarmInputView(const EventSourcesNetworkAlarmInputViewResult &viewResult)
{
    m_networkAlarmInputView = viewResult;
}

void DeviceObjectComposer::setMediaAudioOutputView(const MediaAudioOutputViewResult &viewResult)
{
    m_audioOutputView = viewResult;
}

void DeviceObjectComposer::setMediaCameraRegisterView(const MediaCameraRegisterViewResult &viewResult)
{
    m_cameraRegisterView = viewResult;
}

void DeviceObjectComposer::setMediaCameraUpgradeView(const MediaCameraUpgradeViewResult &viewResult)
{
    m_cameraUpgradeView = viewResult;
}

void DeviceObjectComposer::setAttributes(const AttributesResult& attributesResult)
{
    m_attributes = attributesResult;
}

void DeviceObjectComposer::setSessionKey(const DevcieSessionKey& deviceSessionKey)
{
    for(auto& key : m_sessionKeys) {
        if(key.sessionKey == deviceSessionKey.sessionKey) {
            key.playbackSessionId = deviceSessionKey.playbackSessionId;
            return;
        }
    }

    m_sessionKeys.push_back(deviceSessionKey);
}

void DeviceObjectComposer::releaseSessionKey(const std::string& playbackSessionId)
{
    for(auto& key : m_sessionKeys)
    {
        if(key.playbackSessionId == playbackSessionId)
        {
            key.playbackSessionId = "";
            break;
        }
    }
}

void DeviceObjectComposer::setEventStatusSchema(const EventStatusSchemaViewResult& viewResult)
{
    m_eventStatusSchemaView = viewResult;
}

void DeviceObjectComposer::setDisconnected(const DeviceStatusType disconnType)
{
    m_status.status = disconnType;
    for (auto& channelStatus : m_status.channelsStatus) {
        channelStatus.second.status = ChannelStatusType::Disconnected;
    }
}

void DeviceObjectComposer::setDeviceCertificateResult(bool sameMacAddress, const std::string certResult){
    SPDLOG_DEBUG("DeviceObjectComposer::setDeviceCertificateResult sameMacAddress{} tempResult:{}", sameMacAddress, certResult);

    if(certResult != "Htw device certified")
        m_deviceCerticateResult = certResult;
    else if(!sameMacAddress)
        m_deviceCerticateResult = "Device certicate's macaddress is invalid";
    else
        m_deviceCerticateResult = "Device is certified";

    SPDLOG_DEBUG("DeviceObjectComposer::setDeviceCertificateResult finalResult:{}", m_deviceCerticateResult);
}

void DeviceObjectComposer::setRecordingTextPosConfView(const std::vector<TextPosConfResult> viewResult) {
    /*
    SPDLOG_DEBUG("DeviceObjectComposer::setRecordingTextPosConfView count:{}", viewResult.size());
    SPDLOG_DEBUG("DeviceObjectComposer::setRecordingTextPosConfView ID:{}, name:{}, enable:{}, encodingType:{}, channelIdList:{}",
                    viewResult[1].deviceId, viewResult[1].deviceName, viewResult[1].enable,
                    viewResult[1].encodingType, viewResult[1].channelIDList);
    */
    m_posConfView = viewResult;
    m_posEnabled = false;
    for(auto& result : viewResult) {
        if(result.enable) {
            m_posEnabled = true;
            break;
        }
    }
}

const AttributesResult& DeviceObjectComposer::attributes()
{
    return m_attributes;
}

const Device &DeviceObjectComposer::device()
{
    return m_device;
}

const MediaVideoProfileViewResult &DeviceObjectComposer::videoProfileView()
{
    return m_videoProfileView;
}

const MediaVideoProfilePolicyViewResult &DeviceObjectComposer::videoProfilePolicyView()
{
    return m_videoProfilePolicyView;
}

const std::vector<DevcieSessionKey> &DeviceObjectComposer::sessionKeys()
{
    return m_sessionKeys;
}

const SystemDeviceInfoViewResult DeviceObjectComposer::deviceInfoView()
{
    return m_deviceInfoView;
}

const DeviceStatus &DeviceObjectComposer::status()
{
    return m_status;
}

const DeviceProfileInfo &DeviceObjectComposer::profileInfo()
{
    return m_profileInfo;
}

bool DeviceObjectComposer::isRecorder()
{
    return (m_deviceInfoView.enumDeviceType == DeviceType::SunapiRecorder);
}

bool DeviceObjectComposer::isCamera()
{
    return (m_deviceInfoView.enumDeviceType == DeviceType::SunapiCamera);
}

void DeviceObjectComposer::setDeviceCapability(Device::DeviceCapability &deviceCap)
{
    deviceCap.configBackup = m_attributes.systemAttribute.configBackup;
    deviceCap.configBackupEncrypt = m_attributes.systemCgi.configBackupEncrypt;
    deviceCap.configRestore = m_attributes.systemAttribute.configRestore;
    deviceCap.configRestoreDecrypt = m_attributes.systemCgi.configRestoreDecrypt;
    deviceCap.firmwareUpdate = m_attributes.systemAttribute.fwUpdate;
    deviceCap.changePassword = m_attributes.securityCgi.addUpdateUsers;
    deviceCap.recordingManual = m_attributes.recordingAttribute.manualRecordingStart;
    deviceCap.recordingSearch = m_attributes.recordingAttribute.searchByUTCTime;
    deviceCap.recordingSearchPeriod = m_attributes.recordingAttribute.searchPeriod;
    deviceCap.recordingDualTrack = m_attributes.recordingAttribute.dualTrackRecording;
    deviceCap.playback = m_attributes.recordingAttribute.playbackSpeeds.size() > 0;
    deviceCap.playbackSync = isRecorder();
    deviceCap.playbackChannel = !isRecorder();
    deviceCap.thumbnail = m_attributes.videoCgi.thumbnail;
    deviceCap.quickPlay = m_attributes.systemAttribute.quickPlay;

    for (auto speed : m_attributes.recordingAttribute.playbackSpeeds) {
        boost::erase_all(speed, "x");
        boost::trim(speed);
        float fv = try_stof(speed);
        deviceCap.playbackSpeeds.push_back(fv);
    }

    if (m_eventStatusSchemaView.eventNames.size() > 0) {
        deviceCap.eventNames.resize((int)(m_eventStatusSchemaView.eventNames.size()));
        std::copy(m_eventStatusSchemaView.eventNames.begin(), m_eventStatusSchemaView.eventNames.end(), deviceCap.eventNames.begin());
    }
    else if (m_attributes.eventRuleCgi.dynamicEventName.size() > 0) {
        deviceCap.eventNames.resize((int)(m_attributes.eventRuleCgi.dynamicEventName.size()));
        std::copy(m_attributes.eventRuleCgi.dynamicEventName.begin(), m_attributes.eventRuleCgi.dynamicEventName.end(), deviceCap.eventNames.begin());
    }
    
    deviceCap.recordingTypes = m_attributes.recordingCgi.recordingTypes;
}


void DeviceObjectComposer::setChannelCapability(Device::Channel &channel)
{
    int index = toSunapiChannel(channel.channelID);

    // Media
    auto &system = m_attributes.systemAttribute;
    auto &media = m_attributes.mediaAttribute.channels[index];
    auto &recording = m_attributes.recordingAttribute.channels[index];
    auto &ptz = m_attributes.ptzSupportAttribute.channels[index];
    auto &eventSource = m_attributes.eventSourceAttribute.channels[index];
    auto &image = m_attributes.imageAttribute.channels[index];
    channel.mediaCapabilities.live = media.live;
    channel.mediaCapabilities.audioIn = media.audioIn;
    channel.mediaCapabilities.audioOut = media.channelAudioOut;
    channel.mediaCapabilities.playback = recording.searchTimeline;
    channel.mediaCapabilities.recordingSearch = recording.searchTimeline;
    channel.mediaCapabilities.backup = recording.backup;
    channel.mediaCapabilities.recordingDualTrack = m_attributes.recordingAttribute.dualTrackRecording;
    channel.mediaCapabilities.personSearch = recording.personSearch;
    channel.mediaCapabilities.faceSearch = recording.faceSearch;
    channel.mediaCapabilities.vehicleSearch = recording.vehicleSearch;
    channel.mediaCapabilities.ocrSearch = recording.ocrSearch;
    channel.mediaCapabilities.smartSearch = recording.smartSearch;
    channel.mediaCapabilities.smartSearchAiFilter =  recording.smartSearch & system.aiFeature;


    // PTZ
    channel.ptzCapabilities.continousPanTilt = (ptz.continiousPan && ptz.continiousTilt);
    channel.ptzCapabilities.continousZoom = ptz.continiousZoom;
    channel.ptzCapabilities.continousFocus = ptz.continiousFocus;
    channel.ptzCapabilities.continousIris = ptz.continiousIris;
    channel.ptzCapabilities.home = ptz.home;
    channel.ptzCapabilities.auxCommands = ptz.auxCommands;
    channel.ptzCapabilities.areaZoom = ptz.areaZoom;
    channel.ptzCapabilities.preset = ptz.preset;
    channel.ptzCapabilities.swing = ptz.swing;
    channel.ptzCapabilities.group = ptz.group;
    channel.ptzCapabilities.tour = ptz.tour;
    channel.ptzCapabilities.trace = ptz.trace;
    channel.ptzCapabilities.autoTracking = eventSource.tracking;

    // Image
    channel.imageCapabilities.simpleFocus = image.simpleFocus;
    channel.imageCapabilities.autoFocus = image.autoFocus;
    channel.imageCapabilities.resetFocus = image.resetFocus;
    channel.imageCapabilities.defog = image.defog;


    // Fisheye ----> TODO::
    //camera register
    auto itr =m_cameraRegisterView.cameraRegisters.find(index);
    if (itr != m_cameraRegisterView.cameraRegisters.end())
    {
        channel.channelModelName = itr->second.model;
        channel.ip = itr->second.ip;
        channel.fisheyeSettings.fisheyeEnable = Wisenet::Common::GetFisheyeType(itr->second.model, channel.fisheyeSettings.fisheyeLensType);
        if(channel.fisheyeSettings.fisheyeEnable)
            channel.fisheyeSettings.fisheyeLensLocation = FisheyeLensLocation::Ceiling;
        SPDLOG_DEBUG("[CONFIG] rebuildDevice() START, channel model name={} fisheyeEnable={} fisheyeLensType={}",
                     channel.channelModelName ,channel.fisheyeSettings.fisheyeEnable, channel.fisheyeSettings.fisheyeLensType);
    }

    //camera upgrade
    auto camUpgradeItr = m_cameraUpgradeView.cameraUpagrades.find(index);
    if(camUpgradeItr != m_cameraUpgradeView.cameraUpagrades.end())
    {
        channel.channelCurVersion = camUpgradeItr->second.currentFwVersion;
        channel.isServerUpgradeEnabled = camUpgradeItr->second.isUpgradeEnabled;
    }
}

const void DeviceObjectComposer::rebuildDevice(const std::string& deviceUUID,
                                               const DeviceConnectInfo& connInfo)
{
    spdlog::stopwatch sw;

    SPDLOG_DEBUG("[CONFIG] rebuildDeviceObject() START, uuid={}", deviceUUID);
    Device newDevice;
    DeviceStatus newStatus;

    newDevice.deviceID = deviceUUID;
    newDevice.connectionInfo = connInfo;
    newDevice.modelName = m_deviceInfoView.model;
    newDevice.deviceName = m_deviceInfoView.name;
    newDevice.macAddress = m_deviceInfoView.connectedMacAddress;
    newDevice.deviceType = m_deviceInfoView.enumDeviceType;
    newDevice.firmwareVersion = m_deviceInfoView.firmwareVersion;
    newDevice.deviceCertifcateResult = m_deviceCerticateResult;
    newDevice.supportWebSocket = m_deviceInfoView.supportedWebSocket;

    newDevice.deviceTimeZone.utcTimeBias = m_dateView.utcTimeBias;
    newDevice.deviceTimeZone.useDst = m_dateView.dstEnable;
    if(newDevice.deviceTimeZone.useDst)
    {
        newDevice.deviceTimeZone.dstBias = m_dateView.dstBias;
        newDevice.deviceTimeZone.dstStartMonth = m_dateView.dstStartMonth;
        newDevice.deviceTimeZone.dstStartDay = m_dateView.dstStartDay;
        newDevice.deviceTimeZone.dstStartHour = m_dateView.dstStartHour;
        newDevice.deviceTimeZone.dstStartMinute = m_dateView.dstStartMinute;
        newDevice.deviceTimeZone.dstEndMonth = m_dateView.dstEndMonth;
        newDevice.deviceTimeZone.dstEndDay = m_dateView.dstEndDay;
        newDevice.deviceTimeZone.dstEndHour = m_dateView.dstEndHour;
        newDevice.deviceTimeZone.dstEndMinute = m_dateView.dstEndMinute;
    }

    newStatus.deviceID = deviceUUID;
    newStatus.status = DeviceStatusType::Connected;

    setDeviceCapability(newDevice.deviceCapabilities);

    // create channel information
    for (int i = 0; i < m_attributes.systemAttribute.maxChannel; i++)
    {
        Device::Channel channel;
        ChannelStatus channelStatus;
        std::string channelID = fromSunapiChannel(i);

        channel.deviceID = deviceUUID;
        channel.channelID = channelID;

        channelStatus.deviceID = channel.deviceID;
        channelStatus.channelID = channel.channelID;

        if (isCamera()) {
            channel.channelName = fromSunapiChannel(i);
            auto itr = m_videoProfileView.channels.find(i);
            if (itr != m_videoProfileView.channels.end()) {
                channelStatus.status = ChannelStatusType::Connected;
            }
            else
                channelStatus.status = ChannelStatusType::Disconnected;
        }
        else {
            // video sources is valid for recorder and encoder
            auto itr =m_videoSourceView.videoSources.find(i);
            if (itr !=m_videoSourceView.videoSources.end())
            {
                channel.channelName = itr->second.name;
                channelStatus.status = ConvertChannelStatus(m_deviceInfoView.enumDeviceType, itr->second.status);                

                if(channelStatus.status == ChannelStatusType::Connected)
                {
                    if(m_videoProfileView.channels.find(itr->first) == m_videoProfileView.channels.end())
                        channelStatus.status = ChannelStatusType::Disconnected;
                }
                SPDLOG_DEBUG("[CONFIG] rebuildDevice() START, name={} status={} source={}", channel.channelName, channelStatus.status, itr->second.status);
            }
        }

        setChannelCapability(channel);
        // create network alarm input
        if (isRecorder()) {
            auto itr = m_networkAlarmInputView.networkAlarmInputs.find(i);
            if (itr != m_networkAlarmInputView.networkAlarmInputs.end()) {
                Device::Channel::ChannelInput channelInput;
                channelInput.deviceID = deviceUUID;
                channelInput.channelID = channelID;
                channelInput.inputID = "1";
                channel.channelInputs.emplace(channelInput.inputID, channelInput);
            }
        }


        newDevice.channels.emplace(channelID, std::move(channel));
        newStatus.channelsStatus.emplace(channelID, std::move(channelStatus));
    }

    // create alarm input information
    for (auto& kv : m_alarmInputView.alarmInputs) {
        Device::DeviceInput deviceInput;
        deviceInput.deviceID = deviceUUID;
        deviceInput.deviceInputID = std::to_string(kv.first);
        newDevice.deviceInputs.emplace(deviceInput.deviceInputID, deviceInput);
    }

    // create alarm output information
    for (auto& kv : m_ioAlarmOutputView.alarmOutputs) {
        Device::DeviceOutput deviceOutput;
        deviceOutput.deviceID = deviceUUID;
        deviceOutput.deviceOutputID = std::to_string(kv.first);
        deviceOutput.outputType = (kv.second.type == "Alarmout") ?
                    Device::DeviceOutput::OutputType::AlarmOut :
                    Device::DeviceOutput::OutputType::Beep;
        //SPDLOG_DEBUG("alarm out ={} {} type={}", kv.first, deviceOutput.deviceOutputID, deviceOutput.outputType);
        newDevice.deviceOutputs.emplace(deviceOutput.deviceOutputID, deviceOutput);
    }

    // create pos configuration information
    for (auto& pos : m_posConfView) {
        Device::PosConfiguration posConf;
        posConf.deviceID = deviceUUID;
        posConf.posID = pos.deviceId;
        posConf.posName = pos.deviceName;
        posConf.enable = pos.enable;
        posConf.encodingType = pos.encodingType;

        /*
        std::vector<std::string> channelIDtokens;
        boost::split(channelIDtokens, pos.channelIDList, boost::is_any_of(","));
        for(auto& channelId : channelIDtokens) {
            posConf.channelIDList.push_back(fromSunapiChannel(channelId));
        }
        */

        newDevice.posConfigurations.emplace(posConf.posID, posConf);
    }

    // create profile information
    rebuildProfileInfo(deviceUUID);

    // assign new values
    m_device = newDevice;
    m_status = newStatus;

    spdlog::debug("[CONFIG] rebuildDeviceObject() Elapsed: {:.3} seconds, channel={}, deviceInput={}, deviceOutput={}",
                  sw, m_device.channels.size(), m_device.deviceInputs.size(), m_device.deviceOutputs.size());

}


const void DeviceObjectComposer::rebuildProfileInfo(const std::string& deviceUUID)
{
    SPDLOG_DEBUG("[CONFIG] rebuildProfileInfo(), deviceID={}", deviceUUID);

    DeviceProfileInfo newProfileInfo;
    newProfileInfo.deviceID = deviceUUID;

    // create profile information
    if (isRecorder()) { // recorder : iterate profile policy result
        for (auto& kv : m_videoProfilePolicyView.channels) {
            auto itr = m_videoProfileView.channels.find(kv.first);
            if (itr == m_videoProfileView.channels.end()) {
                SPDLOG_INFO("deviceUUID={}, Can not find profileview in channel={}", deviceUUID, kv.first);
                continue;
            }
            std::string channelID = fromSunapiChannel(kv.first);
            static const std::string recorderProfileNames[3] = {RECORD_PROFILE_RECORER,
                                                                LIVE_PROFILE_RECORER,
                                                                NETWORK_PROFILE_RECORER};
            const int profileIDs[3] = {kv.second.recordProfile,
                                       kv.second.liveProfile,
                                       kv.second.networkProfile};

            for (int i = 0 ; i < 3 ; i++) {
                VideoProfile profile;
                profile.deviceID = deviceUUID;
                profile.channelID = channelID;
                profile.profileID = std::to_string(profileIDs[i]);
                SPDLOG_DEBUG("[CONFIG] rebuildProfileInfo= {} - {}, deviceID={}, channel={}", i, profileIDs[i], deviceUUID, channelID);

                profile.aliasID = std::to_string(i+1);
                profile.aliasName = recorderProfileNames[i];
                ConvertRecorderVideoProfileViewInfo(profileIDs[i], itr->second, profile);
                // use aliasID with Key
                newProfileInfo.videoProfiles[profile.channelID][profile.aliasID] = profile;
                printProfileInfo(profile);
            }

        }
    }
    else { // camera : iterate profile result
        for (auto& kv : m_videoProfileView.channels) {
            std::string channelID = fromSunapiChannel(kv.first);
            for (auto& kv2 : kv.second) {
                VideoProfile profile;
                profile.deviceID = deviceUUID;
                profile.channelID = channelID;
                profile.profileID = std::to_string(kv2.first);
                ConvertVideoProfileViewInfo(kv2.second, profile);
                profile.aliasID = profile.profileID;
                profile.aliasName = profile.profileName;
                newProfileInfo.videoProfiles[profile.channelID][profile.aliasID] = profile;
                printProfileInfo(profile);
            }
        }
    }
    m_profileInfo = newProfileInfo;
}

const DeviceObjectComposerEvent DeviceObjectComposer::rebuildDeviceWithConfigUri(const ConfigUriComposerPtr &cuc)
{
    SPDLOG_DEBUG("[CONFIG] DeviceObjectComposer::rebuildDeviceWithConfigUri() START, uuid={}", m_device.deviceID);

    std::string deviceId = m_device.deviceID;
    DeviceStatusEventSharedPtr deviceStatusEvent = nullptr;
    ChannelStatusEventSharedPtr channelStatusEvent = nullptr;
    DeviceProfileInfoEventSharedPtr profileInfoEvent = nullptr;

    DeviceObjectComposerEvent composerEvent;

    /**
     * TODO:: 추후 카메라 연동시에는 다음 정보도 다시 구성해야 한다.
     * alarm input, alarm output
     */

    if(cuc->m_dateView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] DATE UPDATE, uuid={}", deviceId);
        setSystemDateView(cuc->m_dateView.get());
    }

    if(cuc->m_attributes.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] ATTRIBUTE UPDATE, uuid={}", deviceId);
        setAttributes(cuc->m_attributes.get());
    }

    if (cuc->m_videoSourceView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] VIDEO SOURCE UPDATE, uuid={}", deviceId);
        setMediaVideoSourceView(cuc->m_videoSourceView.get());
    }

    if (cuc->m_videoProfileView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] VIDEO PROFILE UPDATE, uuid={}", deviceId);
        setMediaVideoProfileView(cuc->m_videoProfileView.get());
    }

    if (cuc->m_videoProfilePolicyView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] VIDEO PROFILE POLICY UPDATE, uuid={}", deviceId);
        setMediaVideoProfilePolicyView(cuc->m_videoProfilePolicyView.get());
    }

    if (cuc->m_recordingStorageView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] RECORDING STORAGE UPDATE, uuid={}", deviceId);
        // TODO :: 현재는 녹화상태 정보 관리하지 않음, 상태만 업데이트한다.
        setRecordingStorageView(cuc->m_recordingStorageView.get());
    }

    if (cuc->m_recordingGeneralView.has_value())
    {
        SPDLOG_DEBUG("[CONFIG] RECORDING GENERAL UPDATE, uuid={}", deviceId);
        // TODO :: 현재는 녹화상태 정보 관리하지 않음, 상태만 업데이트한다.
        setRecordingGeneralView(cuc->m_recordingGeneralView.get());
    }

    // date or attribute가 변경된 경우에는 DeviceStatusEvent를 만든다.
    if (cuc->m_attributes.has_value() || cuc->m_dateView.has_value())
    {
        deviceStatusEvent = std::make_shared<DeviceStatusEvent>();

        SPDLOG_DEBUG("[CONFIG] NEW DEVICE STATUS EVENT, uuid={}", deviceId);
        rebuildDevice(deviceId, m_device.connectionInfo);

        deviceStatusEvent->deviceID = deviceId;
        deviceStatusEvent->device = m_device;
        deviceStatusEvent->deviceStatus = m_status;

        if(deviceStatusEvent->deviceStatus.status == DeviceStatusType::Connected)
            deviceStatusEvent->deviceStatus.status = DeviceStatusType::ConnectedAndInfoChanged;

        deviceStatusEvent->deviceProfileInfo = m_profileInfo;
        deviceStatusEvent->serviceUtcTimeMsec = Common::currentUtcMsecs();
        composerEvent.deviceStatusEvent = deviceStatusEvent;

        return composerEvent;
    }

    if (cuc->m_videoProfileView.has_value() || cuc->m_videoProfilePolicyView.has_value())
    {
        profileInfoEvent = std::make_shared<DeviceProfileInfoEvent>();

        SPDLOG_DEBUG("[CONFIG] NEW PROFILE INFO EVENT, uuid={}", deviceId);
        rebuildProfileInfo(deviceId);

        profileInfoEvent->deviceID = deviceId;
        profileInfoEvent->deviceProfileInfo = m_profileInfo;
        profileInfoEvent->serviceUtcTimeMsec = Common::currentUtcMsecs();
        composerEvent.profileInfoEvent = profileInfoEvent;
    }

    if (cuc->m_videoSourceView.has_value())
    {
        channelStatusEvent = std::make_shared<ChannelStatusEvent>();

        for (int i = 0; i < m_attributes.systemAttribute.maxChannel; i++)
        {
            Device::Channel channel;
            ChannelStatus channelStatus;
            std::string channelID = fromSunapiChannel(i);
            channel.deviceID = deviceId;
            channel.channelID = channelID;

            channelStatus.deviceID = channel.deviceID;
            channelStatus.channelID = channel.channelID;

            auto oldChannelStatus = m_status.channelsStatus[channelID];

            if (isCamera()) {
                channelStatus.status = ChannelStatusType::Connected;
            }
            else {
                // video sources is valid for recorder and encoder
                auto itr = m_videoSourceView.videoSources.find(i);
                if (itr != m_videoSourceView.videoSources.end())
                {
                    channelStatus.status = ConvertChannelStatus(m_deviceInfoView.enumDeviceType, itr->second.status);

                    if(profileInfoEvent && channelStatus.status == ChannelStatusType::Connected)
                    {
                        if(m_videoProfileView.channels.find(itr->first) == m_videoProfileView.channels.end())
                            channelStatus.status = ChannelStatusType::Disconnected;
                    }
                }
            }
            if (oldChannelStatus.status != channelStatus.status) {
                if (channelStatus.isGood()) {
                    setChannelCapability(channel);
                    // create network alarm input
                    if (isRecorder()) {
                        channel.channelInputs = m_device.channels[channelID].channelInputs;
                    }
                    channelStatus.channel = channel;
                }
                channelStatusEvent->channelsStatus[channelID] = channelStatus;

                // m_device & m_status 를 업데이트한다.
                if (channelStatus.channel.has_value()) {
                    m_device.channels[channelID] = channel;
                    channelStatus.channel = boost::none;
                }
                m_status.channelsStatus[channelID] = channelStatus;
            }
        }
        channelStatusEvent->deviceID = deviceId;
        channelStatusEvent->serviceUtcTimeMsec = Common::currentUtcMsecs();
        // 변경된 채널 상태 정보가 없으면 굳이 올리지 않는다.
        SPDLOG_DEBUG("[CONFIG] NEW CHANNEL STATUS EVENT, uuid={}, num_channels={}", deviceId, channelStatusEvent->channelsStatus.size());
        if (channelStatusEvent->channelsStatus.size() > 0) {
            composerEvent.channelStatusEvent = channelStatusEvent;
        }
    }

    return composerEvent;

}

}
}

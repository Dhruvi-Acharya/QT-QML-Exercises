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

#include <memory>
#include <string>
#include <functional>
#include <vector>
#include "ErrorCode.h"
#include <boost/optional.hpp>
#include <unordered_map>

namespace Wisenet
{

// 00000000-0000-0000-0000-000000000000 (36 characters UUID)
typedef std::string uuid_string;


// All response structures should be derived from ResponseBase
struct ResponseBase
{
    Wisenet::ErrorCode errorCode = Wisenet::ErrorCode::NoError;
    bool isContinue = false;        // set true if next response will be continued
    bool isFailed()                 // set true if errorCode != NoError
    {
        return (errorCode != Wisenet::ErrorCode::NoError);
    }
    bool isSuccess()
    {
        return !isFailed();
    }
    std::string errorString()       // errorCode to string
    {
        return GetErrorString(errorCode);
    }
};

// All request structures should be derived from RequestBase
struct RequestBase
{
    // 00000000-0000-0000-0000-000000000000 (36 characters UUID)
    uuid_string serviceID;

    // User ID
    std::string userName;
};

// All event structures should be derived from EventBase
enum class EventSourceType
{
    Service = 0,
    Device
};


// SNF-8010/8010VM : C7SST
// PNF-9010R/RV/RVM : C8WWT
// XNF-8010R/RV/RVM : D1SST
// HCF-8010V : D4SST
// QNF-8010/KAN : D9SSV
// QNF-9010 : E3VVV
// XNF-9010RV : E5VVT
// TNF-9010 : E5VVT
namespace FisheyeLensType {
static const std::string SNF_8010 = "SNF-8010";
static const std::string PNF_9010 = "PNF-9010";
static const std::string XNF_8010 = "XNF-8010";
static const std::string HCF_8010 = "HCF-8010";
static const std::string QNF_8010 = "QNF-8010";
static const std::string QNF_9010 = "QNF-9010";
static const std::string XNF_9010 = "XNF-9010";
static const std::string TNF_9010 = "TNF-9010";
}

enum class FisheyeLensLocation
{
    Ceiling = 0,
    Wall,
    Ground
};

enum class FisheyeViewMode
{
    Original = 0,
    Single,
    Quad,
    Panorama
};

namespace AuditLogType{
static const std::string LogIn = "User Login";
static const std::string BackupSettings = "Backup Settings";
static const std::string RestoreSettings = "Restore Settings";
static const std::string InitializeSettings = "Initialize Settings";
static const std::string LogSettings = "Log Settings";
static const std::string UserSettings = "User Settings";
static const std::string UserGroupSettings = "User Group Settings";
static const std::string EventRuleSettings = "Event Rule Settings";
static const std::string ScheduleSettings = "Schedule Settings";
static const std::string DeviceSettings = "Device Settings";
static const std::string ChannelSettings = "Channel Settings";
static const std::string DeviceStatus = "Device Status";
static const std::string PTZControl = "PTZ Control";
static const std::string WatchingLive = "Watching Live";
static const std::string WatchingPlayback = "Watching Playback";
static const std::string ExportingVideo = "Exporting Video";
static const std::string UpdateSoftware = "Software upgrade tried";
}

namespace FixedAlarmType{

//전원관련
static const std::string DualSMPSFail = "SystemEvent.DualSMPSFail";

//설정 관련
static const std::string ConfigChange = "SystemEvent.ConfigChange";
static const std::string TimeChange = "SystemEvent.TimeChange";
static const std::string FactoryReset = "SystemEvent.FactoryReset";
static const std::string ConfigurationRestore = "SystemEvent.ConfigurationRestore";


//팬 관련
static const std::string FanError = "SystemEvent.FanError";
static const std::string CPUFanError = "SystemEvent.CPUFanError";
static const std::string FrameFanError = "SystemEvent.FrameFanError";
static const std::string LeftFanError = "SystemEvent.LeftFanError";
static const std::string RightFanError = "SystemEvent.RightFanError";

//저장매체 관련
static const std::string HDDFull = "SystemEvent.HDDFull";
static const std::string SDFull = "SystemEvent.SDFull";
static const std::string ChannelSDFull = "SDFull";
static const std::string NASFull = "SystemEvent.NASFull";
static const std::string HDDNone = "SystemEvent.HDDNone";
static const std::string HDDFail = "SystemEvent.HDDFail";
static const std::string SDFail = "SystemEvent.SDFail";
static const std::string NASFail = "SystemEvent.NASFail";
static const std::string ChannelSDFail = "SDFail";
static const std::string HDDError = "SystemEvent.HDDError";
static const std::string RAIDDegrade = "SystemEvent.RAIDDegrade";
static const std::string RAIDRebuildStart = "SystemEvent.RAIDRebuildStart";
static const std::string RAIDFail = "SystemEvent.RAIDFail";
static const std::string iSCSIDisconnect = "SystemEvent.iSCSIDisconnect";
static const std::string NASDisconnect = "SystemEvent.NASDisconnect";

//레코딩 관련
static const std::string RecordFiltering = "SystemEvent.RecordFiltering"; //녹화 데이터양 초과
static const std::string RecordingError = "SystemEvent.RecordingError"; //녹화상태 경고

//시스템 관련
static const std::string CpuOverload = "SystemEvent.CpuOverload";
static const std::string NetTxTrafficOverflow = "SystemEvent.NetTxTrafficOverflow";
static const std::string NetCamTrafficOverFlow = "SystemEvent.NetCamTrafficOverFlow";
static const std::string VPUError = "SystemEvent.VPUError";
static const std::string MemoryError = "SystemEvent.MemoryError";
static const std::string LowFps = "SystemEvent.LowFps";
static const std::string ChannelLowFps = "LowFps";
static const std::string NewFWAvailable = "SystemEvent.NewFWAvailable";

//Analytics 관련
static const std::string MotionDetection = "MotionDetection";
static const std::string FaceDetection = "FaceDetection";
static const std::string Videoloss = "Videoloss";
static const std::string VideolossStart = "Videoloss.Start";
static const std::string VideolossEnd = "Videoloss.End";
static const std::string Tampering = "Tampering";
static const std::string AudioDetection = "AudioDetection";
static const std::string VideoAnalytics = "VideoAnalytics";
static const std::string VideoAnalytics_Passing = "VideoAnalytics.Passing";
static const std::string VideoAnalytics_Intrusion = "VideoAnalytics.Intrusion";
static const std::string VideoAnalytics_Entering = "VideoAnalytics.Entering";
static const std::string VideoAnalytics_Exiting = "VideoAnalytics.Exiting";
static const std::string VideoAnalytics_Appearing = "VideoAnalytics.Appearing";
static const std::string VideoAnalytics_Disappearing = "VideoAnalytics.Disappearing";
static const std::string VideoAnalytics_Loitering = "VideoAnalytics.Loitering";
static const std::string Tracking = "Tracking";
static const std::string TrackingStart = "Tracking.Start";
static const std::string TrackingEnd = "Tracking.End";
static const std::string DefocusDetection = "DefocusDetection";
static const std::string FogDetection = "FogDetection";
static const std::string AudioAnalytics = "AudioAnalytics";
static const std::string AudioAnalytics_Scream = "AudioAnalytics.Scream";
static const std::string AudioAnalytics_Gunshot = "AudioAnalytics.Gunshot";
static const std::string AudioAnalytics_Explosion = "AudioAnalytics.Explosion";
static const std::string AudioAnalytics_GlassBreak = "AudioAnalytics.GlassBreak";
static const std::string NetworkAlarmInput = "NetworkAlarmInput";

static const std::string DeviceAlarmInput = "AlarmInput";

//CustomButton
static const std::string CustomButton = "CustomButton";
static const std::string AlertAlarm = "AlertAlarm";

//
static const std::string DynamicEvent = "DynamicEvent";

static const std::string ObjectDetection = "ObjectDetection";
static const std::string ObjectDetection_Face = "ObjectDetection.Face";
static const std::string ObjectDetection_Person = "ObjectDetection.Person";
static const std::string ObjectDetection_Vehicle = "ObjectDetection.Vehicle";
static const std::string ObjectDetection_LicensePlate = "ObjectDetection.LicensePlate";

static const std::string IFrameMode = "SystemEvent.IFrameMode";
static const std::string FullFrameMode = "SystemEvent.FullFrameMode";
}

namespace TitleAlarmType{
static const std::string DeviceAlarmInput = "AlarmInput";
static const std::string DeviceFanStatus = "DeviceFanStatus";
static const std::string DevicePowerStatus = "DevicePowerStatus";
static const std::string DeviceStorageStatus = "DeviceStorageStatus";
static const std::string DeviceRecordingStatus = "DeviceRecordingStatus";
static const std::string DeviceSystemStatus = "DeviceSystemStatus";
static const std::string DeviceSystemConnected = "DeviceSystemConnected";
static const std::string DeviceSystemDisconnected = "DeviceSystemDisconnected";
static const std::string DeviceFirmware = "Firmware";
static const std::string DeviceOverload = "Overload";

static const std::string MotionDetection = "MotionDetection";
static const std::string FaceDetection = "FaceDetection";
static const std::string VideoLoss = "Videoloss";
static const std::string Tampering = "Tampering";
static const std::string AudioDetection = "AudioDetection";
static const std::string VideoAnalytics = "VideoAnalytics";
static const std::string Tracking = "Tracking";
static const std::string DefocusDetection = "DefocusDetection";
static const std::string FogDetection = "FogDetection";
static const std::string AudioAnalytics = "AudioAnalytics";
static const std::string NetworkAlarmInput = "NetworkAlarmInput";
static const std::string ObjectDetection = "ObjectDetection";
static const std::string ObjectDetection_Face = "ObjectDetection.Face";
static const std::string ObjectDetection_Person = "ObjectDetection.Person";
static const std::string ObjectDetection_Vehicle = "ObjectDetection.Vehicle";
static const std::string ObjectDetection_LicensePlate = "ObjectDetection.LicensePlate";
}

namespace AdditionalLogType{
static const std::string FirmwareUpdgrade = "SystemEvent.CoreService.FirmwareUpgrade";
static const std::string DeviceConnected = "SystemEvent.CoreService.DeviceConnected";
static const std::string DeviceDisconnectedByService = "SystemEvent.CoreService.DeviceDisconnectedByService";
static const std::string DeviceDisconnectedWithError = "SystemEvent.CoreService.DeviceDisconnectedWithError";
static const std::string DeviceDisconnectedWithUnauthorized = "SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized";
static const std::string DeviceDisconnectedWithRestriction = "SystemEvent.CoreService.DeviceDisconnectedWithRestriction";
};

enum EventSystemType
{
    Event,
    System
};

static std::unordered_map<std::string, EventSystemType> EventSystemMap ={
    {TitleAlarmType::DeviceAlarmInput , EventSystemType::System},
    {TitleAlarmType::DeviceFanStatus , EventSystemType::System},
    {TitleAlarmType::DevicePowerStatus , EventSystemType::System},
    {TitleAlarmType::DeviceStorageStatus , EventSystemType::System},
    {TitleAlarmType::DeviceRecordingStatus , EventSystemType::System},
    {TitleAlarmType::DeviceSystemConnected , EventSystemType::System},
    {TitleAlarmType::DeviceSystemDisconnected , EventSystemType::System},
    {TitleAlarmType::DeviceFirmware, EventSystemType::System},
    {TitleAlarmType::DeviceOverload , EventSystemType::System},
    {TitleAlarmType::MotionDetection , EventSystemType::Event},
    {TitleAlarmType::FaceDetection , EventSystemType::Event},
    {TitleAlarmType::VideoLoss , EventSystemType::Event},
    {TitleAlarmType::Tampering , EventSystemType::Event},
    {TitleAlarmType::AudioDetection , EventSystemType::Event},
    {TitleAlarmType::VideoAnalytics , EventSystemType::Event},
    {TitleAlarmType::Tracking , EventSystemType::Event},
    {TitleAlarmType::DefocusDetection , EventSystemType::Event},
    {TitleAlarmType::FogDetection , EventSystemType::Event},
    {TitleAlarmType::AudioAnalytics , EventSystemType::Event},
    {TitleAlarmType::ObjectDetection , EventSystemType::Event},
};

static std::map<std::string, std::vector<std::string>> TitleAlarmMap ={
    { TitleAlarmType::DevicePowerStatus,{FixedAlarmType::DualSMPSFail}},

    { TitleAlarmType::DeviceFanStatus, {FixedAlarmType::FanError,
                                        FixedAlarmType::CPUFanError,
                                        FixedAlarmType::FrameFanError,
                                        FixedAlarmType::LeftFanError,
                                        FixedAlarmType::RightFanError}},

    { TitleAlarmType::DeviceStorageStatus, {FixedAlarmType::HDDFull,
                                            FixedAlarmType::HDDFail,
                                            FixedAlarmType::HDDNone,
                                            FixedAlarmType::HDDError,
                                            FixedAlarmType::SDFail,
                                            FixedAlarmType::SDFull,
                                            FixedAlarmType::NASFail,
                                            FixedAlarmType::NASFull,
                                            FixedAlarmType::NASDisconnect,
                                            FixedAlarmType::RAIDDegrade,
                                            FixedAlarmType::RAIDRebuildStart,
                                            FixedAlarmType::RAIDFail,
                                            FixedAlarmType::iSCSIDisconnect}},
    { TitleAlarmType::DeviceRecordingStatus, {FixedAlarmType::RecordFiltering,
                                              FixedAlarmType::RecordingError}},
    { TitleAlarmType::DeviceOverload, {FixedAlarmType::NetCamTrafficOverFlow,
                                           FixedAlarmType::CpuOverload,
                                           FixedAlarmType::NetTxTrafficOverflow,
                                           FixedAlarmType::VPUError,
                                           FixedAlarmType::MemoryError,
                                           FixedAlarmType::LowFps,
                                           FixedAlarmType::ChannelLowFps}},
    { TitleAlarmType::DeviceSystemConnected, {AdditionalLogType::DeviceConnected}},
    { TitleAlarmType::DeviceSystemDisconnected, {AdditionalLogType::DeviceDisconnectedByService,
                                                 AdditionalLogType::DeviceDisconnectedWithError,
                                                 AdditionalLogType::DeviceDisconnectedWithUnauthorized,
                                                 AdditionalLogType::DeviceDisconnectedWithRestriction}},
    { TitleAlarmType::DeviceFirmware, {AdditionalLogType::FirmwareUpdgrade,
                                       FixedAlarmType::NewFWAvailable}},
    { TitleAlarmType::DeviceAlarmInput, {FixedAlarmType::DeviceAlarmInput,
                                         FixedAlarmType::NetworkAlarmInput}},
    { TitleAlarmType::MotionDetection, {FixedAlarmType::MotionDetection}},
    { TitleAlarmType::FaceDetection, {FixedAlarmType::FaceDetection}},
    { TitleAlarmType::VideoLoss, {FixedAlarmType::Videoloss,
                                  FixedAlarmType::VideolossStart,
                                  FixedAlarmType::VideolossEnd}},
    { TitleAlarmType::Tampering, {FixedAlarmType::Tampering}},
    { TitleAlarmType::AudioDetection, {FixedAlarmType::AudioDetection}},
    { TitleAlarmType::VideoAnalytics, {FixedAlarmType::VideoAnalytics,
                                       FixedAlarmType::VideoAnalytics_Passing,
                                       FixedAlarmType::VideoAnalytics_Intrusion,
                                       FixedAlarmType::VideoAnalytics_Entering,
                                       FixedAlarmType::VideoAnalytics_Exiting,
                                       FixedAlarmType::VideoAnalytics_Appearing,
                                       FixedAlarmType::VideoAnalytics_Disappearing,
                                       FixedAlarmType::VideoAnalytics_Loitering}},
    { TitleAlarmType::Tracking, {FixedAlarmType::Tracking,
                                 FixedAlarmType::TrackingStart,
                                 FixedAlarmType::TrackingEnd}},
    { TitleAlarmType::DefocusDetection, {FixedAlarmType::DefocusDetection}},
    { TitleAlarmType::FogDetection, {FixedAlarmType::FogDetection}},
    { TitleAlarmType::AudioAnalytics, {FixedAlarmType::AudioAnalytics,
                                       FixedAlarmType::AudioAnalytics_Scream,
                                       FixedAlarmType::AudioAnalytics_Gunshot,
                                       FixedAlarmType::AudioAnalytics_Explosion,
                                       FixedAlarmType::AudioAnalytics_GlassBreak}}
};

static std::map<std::string, std::string> SubTitleAlarmMap ={
    { FixedAlarmType::DualSMPSFail, TitleAlarmType::DevicePowerStatus},

    { FixedAlarmType::FanError, TitleAlarmType::DeviceFanStatus },
    { FixedAlarmType::CPUFanError, TitleAlarmType::DeviceFanStatus },
    { FixedAlarmType::FrameFanError, TitleAlarmType::DeviceFanStatus },
    { FixedAlarmType::LeftFanError, TitleAlarmType::DeviceFanStatus },
    { FixedAlarmType::RightFanError, TitleAlarmType::DeviceFanStatus },

    { FixedAlarmType::HDDFull, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::HDDFail, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::HDDNone, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::HDDError, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::SDFail, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::SDFull, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::NASFail, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::NASFull, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::NASDisconnect, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::RAIDDegrade, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::RAIDRebuildStart, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::RAIDFail, TitleAlarmType::DeviceStorageStatus },
    { FixedAlarmType::iSCSIDisconnect, TitleAlarmType::DeviceStorageStatus },

    { FixedAlarmType::RecordFiltering, TitleAlarmType::DeviceRecordingStatus },
    { FixedAlarmType::RecordingError, TitleAlarmType::DeviceRecordingStatus },

    { FixedAlarmType::NetCamTrafficOverFlow, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::CpuOverload, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::NetTxTrafficOverflow, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::VPUError, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::MemoryError, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::LowFps, TitleAlarmType::DeviceOverload },
    { FixedAlarmType::ChannelLowFps, TitleAlarmType::DeviceOverload },

    { AdditionalLogType::DeviceConnected, TitleAlarmType::DeviceSystemConnected },
    { AdditionalLogType::DeviceDisconnectedByService,TitleAlarmType::DeviceSystemDisconnected },
    { AdditionalLogType::DeviceDisconnectedWithError,TitleAlarmType::DeviceSystemDisconnected },
    { AdditionalLogType::DeviceDisconnectedWithUnauthorized,TitleAlarmType::DeviceSystemDisconnected },
    { AdditionalLogType::DeviceDisconnectedWithRestriction, TitleAlarmType::DeviceSystemDisconnected },

    { AdditionalLogType::FirmwareUpdgrade,TitleAlarmType::DeviceFirmware},
    { FixedAlarmType::NewFWAvailable,TitleAlarmType::DeviceFirmware},

    { FixedAlarmType::MotionDetection, TitleAlarmType::MotionDetection},
    { FixedAlarmType::FaceDetection, TitleAlarmType::FaceDetection },
    { FixedAlarmType::Videoloss,  TitleAlarmType::VideoLoss },
    { FixedAlarmType::VideolossStart,  TitleAlarmType::VideoLoss },
    { FixedAlarmType::VideolossEnd,  TitleAlarmType::VideoLoss },
    { FixedAlarmType::Tampering, TitleAlarmType::Tampering },
    { FixedAlarmType::AudioDetection, TitleAlarmType::AudioDetection },

    { FixedAlarmType::VideoAnalytics, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Passing, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Intrusion, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Entering, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Exiting, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Appearing, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Disappearing, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::VideoAnalytics_Loitering, TitleAlarmType::VideoAnalytics },
    { FixedAlarmType::Tracking, TitleAlarmType::Tracking },
    { FixedAlarmType::TrackingStart, TitleAlarmType::Tracking },
    { FixedAlarmType::TrackingEnd, TitleAlarmType::Tracking },
    { FixedAlarmType::DefocusDetection, TitleAlarmType::DefocusDetection },
    { FixedAlarmType::FogDetection, TitleAlarmType::FogDetection },
    { FixedAlarmType::AudioAnalytics, TitleAlarmType::AudioAnalytics },
    { FixedAlarmType::AudioAnalytics_Scream, TitleAlarmType::AudioAnalytics },
    { FixedAlarmType::AudioAnalytics_Gunshot, TitleAlarmType::AudioAnalytics },
    { FixedAlarmType::AudioAnalytics_Explosion, TitleAlarmType::AudioAnalytics },
    { FixedAlarmType::AudioAnalytics_GlassBreak, TitleAlarmType::AudioAnalytics },

    { FixedAlarmType::NetworkAlarmInput, TitleAlarmType::DeviceAlarmInput },
    { FixedAlarmType::DeviceAlarmInput, TitleAlarmType::DeviceAlarmInput }
};

static std::vector<std::string> GetSubAlarmType(std::string title){
    auto iter = TitleAlarmMap.find(title);

    if(iter != TitleAlarmMap.end())
        return iter->second;
    else {
        return {title};
    }
}

static std::string GetTitleAlarmType(std::string subType){
    auto iter = SubTitleAlarmMap.find(subType);

    if(iter != SubTitleAlarmMap.end())
        return iter->second;
    else {
        return subType;
    }
}

struct EventBase
{
    uuid_string serviceID;
    // the number of milliseconds since 1970-01-01T00:00:00
    // Universal Coordinated Time
    int64_t     serviceUtcTimeMsec = 0; /** core service utc time */

    virtual std::string EventType() = 0;
    virtual int EventTypeId() = 0;
    virtual EventSourceType EventSource()
    {
        return EventSourceType::Service;
    }
};

typedef std::shared_ptr<RequestBase> RequestBaseSharedPtr;
typedef std::shared_ptr<ResponseBase> ResponseBaseSharedPtr;
typedef std::function <void(const ResponseBaseSharedPtr&)>  ResponseBaseHandler;
typedef std::shared_ptr<EventBase> EventBaseSharedPtr;
typedef std::function <void(const EventBaseSharedPtr&)> EventBaseHandler;

}

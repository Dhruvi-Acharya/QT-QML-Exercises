#pragma once

#include <string>
#include <map>
#include <set>

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/error/en.h>

#include "DeviceClient/DeviceEvent.h"
#include "DeviceObjectComposer.h"

namespace Wisenet
{
namespace Device
{

typedef std::function <void(const std::string&)> ChangeConfigUriHandler;
typedef std::function <void(const std::string&, bool)> ChangeAlarmOutputStatusHandler;
class IEventStatusDispatcher
{
public:
    IEventStatusDispatcher(const std::string& logPrefix)
        :m_logPrefix(logPrefix)
    {
        InitializeMap();
    }
    ~IEventStatusDispatcher()
    {
        m_eventHandler = nullptr;
        m_changeConfigUriHandler = nullptr;
        m_repos = nullptr;
    }
    void SetEventHandler(DeviceEventBaseHandler const& eventHandler, ChangeConfigUriHandler const& changeConfigUriHandler, ChangeAlarmOutputStatusHandler const& changeAlarmOutputStatusHandler)
    {
        m_eventHandler = eventHandler;
        m_changeConfigUriHandler = changeConfigUriHandler;
        m_changeAlarmOutputStatusHandler = changeAlarmOutputStatusHandler;
    }

    void SetDeviceLatestEventStatusHandler(DeviceEventBaseHandler const& deviceLatestStatusHandler)
    {
        m_deviceLatestStatusHandler = deviceLatestStatusHandler;
    }

    void ResetObjectComposer(DeviceObjectComposerPtr& oc)
    {
        m_repos = oc;
    }

    virtual void ParseNewEvent(const std::string& eventString) = 0;

protected:
    void eventCallback(const DeviceEventBaseSharedPtr& event)
    {
        if (m_eventHandler)
            m_eventHandler(event);
    }

    void changeConfigUriCallback(const std::string& uri)
    {
        if (m_changeConfigUriHandler)
            m_changeConfigUriHandler(uri);
    }

    void changeAlarmOutputStatusCallback(const std::string& deviceOutputID, bool on)
    {
        if (m_changeAlarmOutputStatusHandler)
            m_changeAlarmOutputStatusHandler(deviceOutputID, on);
    }

    void deviceLatestEventCallback(const DeviceLatestStatusEventSharedPtr& event)
    {
        if (m_deviceLatestStatusHandler)
            m_deviceLatestStatusHandler(event);
    }

    void InitializeMap()
    {
        //팬 오류: 팬이 정상 작동하지 않습니다. 팬 연결 상태를 확인하십시요.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::FanError);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::CPUFanError);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::FrameFanError);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::LeftFanError);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RightFanError);


        //이중 전원 시스템: 전원에 문제가 발생하였습니다. 전원 LED를 확인하십시오
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::DualSMPSFail);

        //녹화 상태 경고: 카메라 문제로 영상을 녹화하는데 문제가 있습니다. 카메라 상태를 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RecordingError);

        //녹화 데이터양 초과: 채널별 녹화할 수 있는 데이터양이 초과되어 I-프레임만 녹화 됩니다. 녹화설정을 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RecordFiltering);

        //시스템 과부하: 시스템에 과부하가 걸렸습니다. 웹뷰어 또는 VMS에서 원격으로 모티너링 하는 사용자 수를 제한 하거나, 원격 또는 저장장치에서 재생하는 채널 수를 조절하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::CpuOverload);

        //원격전송 중 데이터량이 장비의 전송대역폭을 초과하였습니다. 일부 채널이 I Frame으로 재생됩니다. 모니터링하는 사용자수나 채널수를 줄이시기 바랍니다.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NetTxTrafficOverflow);

        //시스템 과부하: 시스템에 과부하가 걸렸습니다. 웹뷰어 또는 VMS에서 원격으로 모티너링 하는 사용자 수를 제한 하거나, 원격 또는 저장장치에서 재생하는 채널 수를 조절하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::VPUError);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::MemoryError);

        //디스크 가득 참: 디스크가 가득 차서 영상을 녹화할 수 없습니다. 디스크 상태를 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::HDDFull);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::SDFull);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NASFull);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::ChannelSDFull); // NVR의 Schemabased로 올것으로 예상됨.

        //No HDD: 연결되어 있는 HDD가 없습니다. 장치를 확인해 주십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::HDDNone);

        //HDD Error[HDD 오류][하드디스크 실패] 하드디스크 교체가 필요한 경우 표시.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::HDDFail);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::SDFail);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NASFail);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::ChannelSDFail); // NVR의 Schemabased로 올것으로 예상됨.

        //HDD 오류: HDD에 문제가 있습니다. HDD상태를 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::HDDError);

        //RAID 오류: RAID에 장착된 HDD에 문제가 있습니다. HDD 상태를 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RAIDDegrade);

        //RAID 재구성: Degrade 상태의 RAID배열을 복구하고 있습니다.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RAIDRebuildStart);

        //RAID 실패: 디스크 장애 또는 RAID 구성 문제로 인해 RAID 배열을 더 이상 사용할 수 없습니다. RAID상태를 확이하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::RAIDFail);

        //iSCSI 연결 끊김: iSCSI 장치와 연결이 끊겼습니다.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::iSCSIDisconnect);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NASDisconnect);


        //AMD Restriction[AMD 기능저하]: 네트워크 성능 저하(10FPS미만, 해상도 1080P초과)로 인하여 AMD 기능이 제대로 이루어지지 않을 때 표기
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::LowFps);
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::ChannelLowFps); // NVR의 Schemabased로 올것으로 예상됨.

        //네트워크 트래픽 부하: 네트워크에 과부하가 발생하였습니다. 카메라 설정을 확인하십시오.
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NetCamTrafficOverFlow);

        //소프프퉤어 업그레이드: 업데이트 서버에 최신 소프트웨어가 있습니다. 최신 버전으로 업그레이드 하십시오
        m_supportedSystemEvents.emplace(Wisenet::FixedAlarmType::NewFWAvailable);

        //m_supportedSystemEvents.emplace("SystemEvent.PowerOn");
        //m_supportedSystemEvents.emplace("SystemEvent.PowerOff");
        //m_supportedSystemEvents.emplace("SystemEvent.PowerReboot");
        //m_supportedSystemEvents.emplace("SystemEvent.ConfigChange");
        //m_supportedSystemEvents.emplace("SystemEvent.FWUpdate");
        //m_supportedSystemEvents.emplace("SystemEvent.FactoryReset");
        //m_supportedSystemEvents.emplace("SystemEvent.SDFormat");
        //m_supportedSystemEvents.emplace("SystemEvent.SDInsert");
        //m_supportedSystemEvents.emplace("SystemEvent.SDRemove");
        //m_supportedSystemEvents.emplace("SystemEvent.TimeChange");
        //m_supportedSystemEvents.emplace("SystemEvent.ConfigurationRestore");
        //m_supportedSystemEvents.emplace("SystemEvent.NASFormat");
        //m_supportedSystemEvents.emplace("SystemEvent.NASConnect");
        //m_supportedSystemEvents.emplace("SystemEvent.InternalHDDErase");
        //m_supportedSystemEvents.emplace("SystemEvent.OverwriteDecoding");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDEnable");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDSetup");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDBuilding");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDBuildCancel");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDBuildFail");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDRebuildFail");
        //m_supportedSystemEvents.emplace("SystemEvent.BeingUpdate");
        //m_supportedSystemEvents.emplace("SystemEvent.InternalHDDConnect");
        //m_supportedSystemEvents.emplace("SystemEvent.BatteryFail");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDDeviceAdd");
        //m_supportedSystemEvents.emplace("SystemEvent.RAIDRecordRestriction");

        m_unsupportedChannelEvents.emplace("RecordingStatus");
        m_unsupportedChannelEvents.emplace("PriorityRecordingStatus");
        //m_unsupportedChannelEvents.emplace("PTZMotion");
        m_unsupportedChannelEvents.emplace("UserInput");
        m_unsupportedChannelEvents.emplace("NetworkCameraConnect");
        m_unsupportedChannelEvents.emplace("AMDStart");
        //m_unsupportedChannelEvents.emplace("DigitalAutoTracking"); // Dynamic Event임.
        m_unsupportedChannelEvents.emplace("USBWIFIConnect");
        //m_unsupportedChannelEvents.emplace("OpenSDK");
        m_unsupportedChannelEvents.emplace("AlarmOutput");
        m_unsupportedChannelEvents.emplace("POS");

        //기본 지원 이벤트.
        //MotionDetection
        //MotionDetection_RegionID
        //FaceDetection
        //Videoloss (Start, End : 처리과정에서 변환)
        //Tampering (Start, End : 처리과정에서 변환)
        //AudioDetection
        //VideoAnalytics_Passing
        //VideoAnalytics_Passing_LineID
        //VideoAnalytics_Intrusion
        //VideoAnalytics_Intrusion_DefinedAreaID
        //VideoAnalytics_Entering
        //VideoAnalytics_Entering_DefinedAreaID
        //VideoAnalytics_Exiting
        //VideoAnalytics_Exiting_DefinedAreaID
        //VideoAnalytics_Appearing
        //VideoAnalytics_Appearing_DefinedAreaID
        //VideoAnalytics_Disappering
        //VideoAnalytics_Disappearing
        //VideoAnalytics_Disappering_DefinedAreaID
        //VideoAnalytics_Disappearing_DefinedAreaID
        //VideoAnalytics_Loitering
        //VideoAnalytics_Loitering_DefinedAreaID
        //NetworkAlarmInput
        //Tracking
        //NetworkAlarmInput
        //DefocusDetection
        //FogDetection
        //SDFormat
        //SDFail
        //SDFull
        //SDInsert
        //AudioAnalytics_Scream
        //AudioAnalytics_Gunshot
        //AudioAnalytics_Explosion
        //AudioAnalytics_GlassBreak
        //ShockDetection
        //TemperatureChangeDetection
        //BoxTemperatureDetection
        //ObjectDetection
        //ObjectDetection_Person
        //ObjectDetection_Vehicle
        //ObjectDetection_Face
        //ObjectDetection_LicensePlate
        //MaskDetection
        //AlarmInput
    };

    bool IsSystemEvent(const std::string& eventType)
    {
        if((0 == eventType.compare(0, 11 , "SystemEvent"))
            || (0 == eventType.compare(0, 6 , "SDFull"))
            || (0 == eventType.compare(0, 6 , "SDFail"))
            || (0 == eventType.compare(0, 6 , "LowFps"))) {
            return true;
        }

        return false;
    }

    bool IsSupportedEvent(const std::string& eventType)
    {
        if(IsSystemEvent(eventType)){
            auto itorSystemEvent = m_supportedSystemEvents.find(eventType);

            if(itorSystemEvent == m_supportedSystemEvents.end()){
                return false;
            }

        }else {

            auto itorChannelEvent = m_unsupportedChannelEvents.find(eventType);

            if(itorChannelEvent != m_unsupportedChannelEvents.end()){
                return false;
            }
        }

        return true;
    }

    std::set<std::string> m_supportedSystemEvents;
    std::set<std::string> m_unsupportedChannelEvents;

    DeviceObjectComposerPtr m_repos = nullptr;
    DeviceEventBaseHandler  m_eventHandler = nullptr;
    ChangeConfigUriHandler  m_changeConfigUriHandler = nullptr;
    ChangeAlarmOutputStatusHandler m_changeAlarmOutputStatusHandler = nullptr;
    DeviceEventBaseHandler m_deviceLatestStatusHandler = nullptr;
    std::string m_logPrefix;
};

class EventStatusDispatcher : public IEventStatusDispatcher
{
public:
    EventStatusDispatcher(const std::string& logPrefix);
    virtual ~EventStatusDispatcher();//virtual

    void ParseNewEvent(const std::string& eventString) override;

private:
    void ParseAndSendAlarm(const long long& timeStamp, const std::string& category, bool data);
    DeviceEventBaseSharedPtr CreateDeviceEventBase(const std::string& eventType, const long long& timeStamp,
                                                   bool isDevice, bool isChannel,
                                                   const std::string& channelID, const std::string& profileID, const std::string& systemID,
                                                   const std::map<std::string, std::string>& parameters,
                                                   const bool& data);

    void SendDeviceLatestStatusEventBase(const std::string &eventType,
                                         const long long &timeStamp,
                                         const bool &data);
private:
    std::map<std::string, bool> m_eventMap;
};

class SchemaBasedEventStatusDispatcher : public IEventStatusDispatcher
{

public:
    SchemaBasedEventStatusDispatcher(const std::string& logPrefix);
    virtual ~SchemaBasedEventStatusDispatcher();//virtual

    void ParseNewEvent(const std::string& eventString) override;

private:
    void TriggerConfigChange(rapidjson::Value::ConstValueIterator event);
    void TriggerAlarmOutput(const std::string& time, rapidjson::Value::ConstValueIterator event);
    void TriggerSystemEvent(const std::string& eventName, std::string& time, rapidjson::Value::ConstValueIterator event);
    void TriggerAlarmIn(const std::string& eventName, std::string& time, rapidjson::Value::ConstValueIterator event);
    void TriggerChannel(const std::string& eventName, std::string& time, rapidjson::Value::ConstValueIterator event);

    template <typename Encoding, typename Allocator>
    std::string ToStringValue(const rapidjson::GenericValue<Encoding, Allocator>&  value);
};

}
}

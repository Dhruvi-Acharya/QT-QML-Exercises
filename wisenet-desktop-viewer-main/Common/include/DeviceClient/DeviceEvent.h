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

#include <string>
#include <memory>
#include <cstdint>
#include <sstream>
#include <boost/optional.hpp>
#include "DeviceClient/DeviceStructure.h"

namespace Wisenet
{
namespace Device
{

struct DeviceEventBase : EventBase
{
    // 00000000000000000000000000000000 (32 characters UUID)
    std::string deviceID;
    // the number of milliseconds since 1970-01-01T00:00:00
    // Universal Coordinated Time
    int64_t     deviceUtcTimeMsec = 0;
    EventSourceType EventSource() override
    {
        return EventSourceType::Device;
    }
};

struct ChannelEventBase : DeviceEventBase
{
    std::string channelID; // starts with 1 ~ for SUNAPI Device
};

enum DeviceEventType
{
    DeviceEventTypeStart = 0,

    /*** START OF Device Event Type ***/
    DeviceStatusEventType,
    ChannelStatusEventType,
    AlarmOutputStatusEventType,
    DeviceAlarmEventType,
    DeviceSystemEventType,
    DeviceProfileInfoEventType,
    MediaDisconnectedEventType,
    DeviceLatestStatusEventType,
    S1DeviceConnectionTypeChangeEventType,
    /*** END OF Device Event Type ***/

    DeviceEventTypeEnd // end of device event type
};

struct DeviceStatusEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "DeviceStatusEvent";
    }
    int EventTypeId() override
    {
        return DeviceStatusEventType;
    }

    DeviceStatus deviceStatus;

    /* Connected 이벤트시에는 업데이트된 장치정보가 함께 올라온다. */
    boost::optional<Device> device;

    /* Connected 이벤트시에는 업데이트된 장치 프로파일 정보가 함께 올라온다.
       Service내에서만 사용하고 클라이언트로는 전달하지 않는다.
     */
    boost::optional<DeviceProfileInfo> deviceProfileInfo;
};
typedef std::shared_ptr<DeviceStatusEvent> DeviceStatusEventSharedPtr;

struct ChannelStatusEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "ChannelStatusEvent";
    }
    int EventTypeId() override
    {
        return ChannelStatusEventType;
    }
    std::map<std::string, ChannelStatus> channelsStatus;
};
typedef std::shared_ptr<ChannelStatusEvent> ChannelStatusEventSharedPtr;

struct DeviceProfileInfoEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "DeviceProfileInfoEvent";
    }
    int EventTypeId() override
    {
        return DeviceProfileInfoEventType;
    }

    /* Connected 이벤트시에는 업데이트된 장치 프로파일 정보가 함께 올라온다.
       Service내에서만 사용하고 클라이언트로는 전달하지 않는다.
     */
    DeviceProfileInfo deviceProfileInfo;
};
typedef std::shared_ptr<DeviceProfileInfoEvent> DeviceProfileInfoEventSharedPtr;

struct MediaDisconnectedEvent : ChannelEventBase
{
    std::string EventType() override
    {
        return "MediaDisconnectedEvent";
    }
    int EventTypeId() override
    {
        return MediaDisconnectedEventType;
    }
    uuid_string mediaID;
};
typedef std::shared_ptr<MediaDisconnectedEvent> MediaDisconnectedEventSharedPtr;

struct AlarmOutputStatusEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "AlarmOutStatusEvent";
    }
    int EventTypeId() override
    {
        return AlarmOutputStatusEventType;
    }
    uuid_string  deviceOutputID;
    bool         status = false;

    std::string ToString()
    {
        std::stringstream ss;

        ss << "serviceID=" << serviceID << ",";
        ss << "serviceUtcTimeMsec=" << serviceUtcTimeMsec << ",";
        ss << "deviceID=" << deviceID << ",";
        ss << "deviceUtcTimeMsec=" << deviceUtcTimeMsec << ",";
        ss << "eventType=" << EventType() << ",";
        ss << "deviceOutputID=" << deviceOutputID << ",";
        ss << "status=" << status << ",";

        return ss.str();
    }
};
typedef std::shared_ptr<AlarmOutputStatusEvent> AlarmOutputStatusEventSharedPtr;

struct DeviceAlarmEvent : ChannelEventBase
{
    std::string type = "";

    bool isDevice = false;
    bool isChannel = false;
    bool isSystemEvent = false;

    std::map<std::string, std::string> sourceParameters;
    std::map<std::string, std::string> dataParameters;
    bool data = false;

    std::string EventType() override
    {
        return "DeviceAlarmEvent";
    }
    int EventTypeId() override
    {
        return DeviceAlarmEventType;
    }

    std::string ToString()
    {
        std::stringstream ss;

        ss << "eventType=" << EventType() << ",";
        ss << "serviceID=" << serviceID << ",";
        ss << "serviceUtcTimeMsec=" << serviceUtcTimeMsec << ",";
        ss << "deviceID=" << deviceID << ",";
        ss << "deviceUtcTimeMsec=" << deviceUtcTimeMsec << ",";
        ss << "type=" << type << ",";
        ss << "channelID=" << channelID << ",";
        ss << "data=" << data << ",";

        for(auto& itor : sourceParameters){
            ss << "Source Parameter: " << itor.first << "=" << itor.second << ",";
        }

        for(auto& itor : dataParameters){
            ss << "Data Parameter: " << itor.first << "=" << itor.second << ",";
        }

        return ss.str();
    }

};
typedef std::shared_ptr<DeviceAlarmEvent> DeviceAlarmEventSharedPtr;

struct DeviceLatestStatusEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "DeviceLatestStatusEvent";
    }
    int EventTypeId() override
    {
        return DeviceLatestStatusEventType;
    }

    std::string eventKey;
    bool status = false;
};
typedef std::shared_ptr<DeviceLatestStatusEvent> DeviceLatestStatusEventSharedPtr;

struct S1DeviceConnectionTypeChangeEvent : DeviceEventBase
{
    std::string EventType() override
    {
        return "S1DeviceConnectionTypeChangeEvent";
    }
    int EventTypeId() override
    {
        return S1DeviceConnectionTypeChangeEventType;
    }

    Device device;
};
typedef std::shared_ptr<S1DeviceConnectionTypeChangeEvent> S1DeviceConnectionTypeChangeEventSharedPtr;


typedef std::shared_ptr<DeviceEventBase> DeviceEventBaseSharedPtr;
typedef std::function <void(const DeviceEventBaseSharedPtr&)> DeviceEventBaseHandler;

}
}

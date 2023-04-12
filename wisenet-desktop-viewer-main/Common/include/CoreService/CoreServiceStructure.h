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

#include <boost/optional.hpp>
#include <memory>
#include <string>
#include <functional>
#include <vector>
#include <array>
#include <set>
#include <sstream>

#include "DeviceClient/DeviceStructure.h"
#include <JsonArchiver.h>

namespace Wisenet
{
namespace Core
{

namespace SettingsKey{
static const std::string LogRetensionTimeDay = "LOG_RETENTION_TIME_DAY";
}

enum class CoreServicetype
{
    LocalService = 0,
    RemoteService = 1
};

struct OsInfo
{
    std::string platform;
    std::string osVersion;
};

struct UserGroup
{
    struct Permission
    {
        bool playback = false;
        bool exportVideo = false;
        bool ptzControl = false;
        bool deviceControl = false;
        bool localRecording = false;
        bool audio = false;
        bool mic = false;
    };
    struct Resource
    {
        struct Device
        {
            std::set<std::string> channels;
        };
        std::map<uuid_string, Device>   devices;
        std::vector<uuid_string>        webpages;
        std::vector<uuid_string>        layouts;
    };

    uuid_string userGroupID;
    std::string name;
    bool isAdminGroup = false;
    Permission  userPermission;
    bool        accessAllResources = false;
    boost::optional<Resource> assignedResource;
};

struct User
{
    enum class UserType
    {
        Owner = 0,
        Admin,
        Custom
    };


    uuid_string userID; /* must be unique */
    std::string loginID;
    std::string password="";
    uuid_string userGroupID;
    std::string name="";
    UserType    userType = UserType::Admin;
    std::string description="";
    std::string email="";
    bool ldapUser = false;
    std::string dn="";
};

struct LoginBlockedInfo
{
    std::string userID = "";
    long long blockEndTimeStamp = 0;
    short blockedCount = 0;
};

struct LayoutItem
{
    enum class LayoutItemType
    {
        Camera = 0,
        Webpage,
        LocalResource,
        ZoomTarget,
        Image
    };
    struct ZoomTargetParam
    {
        uuid_string zoomTargetID;
        float       normX = 0.0f;
        float       normY = 0.0f;
        float       normWidth = 0.0f;
        float       normHeight = 0.0f;
    };

    struct BrightnessContrastParam
    {
        float   brightness = 0.0f; // -1.0 ~ 1.0
        float   contrast = 0.0f; // -1.0 ~ 1.0
    };

    struct ImageParam
    {
        bool    isBackground = false;
        float   opacity = 1.0f;
    };

    struct FisheyeViewParam
    {
        struct PTZ
        {
            double pan = 0.0;
            double tilt = 0.0;
            double zoom = 0.0;
        };
        std::vector<PTZ> positions;
    };

    struct FisheyeDewarpParam
    {
        bool                dewarpEnable = false;
        FisheyeViewMode     viewMode = FisheyeViewMode::Original;
        FisheyeViewParam    viewParam;
        /* local resource file only */
        bool                fileDewarpEnable = false;
        std::string         fileLensType; /* FisheyeLensType */
        FisheyeLensLocation fileLensLocation = FisheyeLensLocation::Ceiling;
    };

    enum class FillMode
    {
        PreserveAspectRatio = 0,
        Stretch
    };

    enum class StreamProfile
    {
        Auto = 0,
        High,
        Low
    };

    uuid_string     itemID;
    LayoutItemType  itemType = LayoutItemType::Camera;
    uuid_string     webpageID;
    uuid_string     deviceID;
    std::string     channelID;
    std::string     resourcePath;
    int             column = 0;
    int             row = 0;
    int             columnCount = 1;
    int             rowCount = 1;
    int             rotation = 0;
    bool            displayInfo = false;
    ZoomTargetParam zoomTargetParam;
    BrightnessContrastParam   bncParam;
    ImageParam      imageParam;
    FisheyeDewarpParam fisheyeDewarpParam;

    FillMode        fillMode = FillMode::PreserveAspectRatio;
    StreamProfile   profile = StreamProfile::Auto;
};

struct Layout
{
    uuid_string layoutID;
    std::string userName;
    std::string name;
    bool        isLocked = false;
    float       cellWidth = 160;
    float       cellHeight = 90;
    float       cellSpacing = 0.0f;
    LayoutItem::FillMode fillMode = LayoutItem::FillMode::PreserveAspectRatio;
    LayoutItem::StreamProfile profile = LayoutItem::StreamProfile::Auto;
    std::vector<LayoutItem> layoutItems;
};


template <typename Archiver>
Archiver& operator&(Archiver& ar, LayoutItem::FisheyeViewParam& viewParam)
{
    ar.StartObject();

    {
        ar.Member("positions");
        size_t itemCount = viewParam.positions.size();
        ar.StartArray(&itemCount);

        if(ar.IsReader){
            for(size_t i = 0; i < itemCount; i++){
                LayoutItem::FisheyeViewParam::PTZ ptz;

                ar.StartObject();
                ar.Member("pan") & ptz.pan;
                ar.Member("tilt") & ptz.tilt;
                ar.Member("zoom") & ptz.zoom;
                ar.EndObject();

                viewParam.positions.push_back(ptz);
            }
        }else{
            for(auto& ptz : viewParam.positions){
                ar.StartObject();
                ar.Member("pan") & ptz.pan;
                ar.Member("tilt") & ptz.tilt;
                ar.Member("zoom") & ptz.zoom;
                ar.EndObject();
            }
        }
        ar.EndArray();
    }
    return ar.EndObject();
}


struct SequenceLayoutItem
{    
    uuid_string itemID;
    uuid_string layoutID;

    int         delayMs = 0;
    int         order = 0;
};

struct SequenceLayout
{
    uuid_string     sequenceLayoutID;
    uuid_string     userName;
    std::string     name;
    bool            settingsManual = false;

    std::vector<SequenceLayoutItem> sequenceLayoutItems;
};

struct Webpage
{
    uuid_string webpageID;
    std::string name;
    std::string url;
    bool useAuth = false;
    std::string userId;
    std::string userPw;
};

struct Group{
    uuid_string groupID;
    std::string name;
    uuid_string parentGroupID;

    enum class MappingType
    {
        Camera = 0,
        WebPage
    };

    struct MappingItem{
        std::string id;
        MappingType mappingType = MappingType::Camera;
    };

    std::map<std::string, MappingItem> mapping;
};

struct AuditLog
{

    std::string auditLogID;
    std::string sessionID;
    std::string userName;
    std::string host;

    std::string actionType;
    int64_t serviceUtcTimeMsec;

    enum class ItemType
    {
        Service = 0,
        Device,
        Camera,
    };

    ItemType itemType;
    std::string itemID;
    std::string parentID;

    std::string actionDetail;
};

enum class AuditLogOperationType
{
    Updated = 0,
    Added,
    Removed,
    Connect,
    Disconnect,
    Fail,
};

static std::string AuditLogOperationTypeToString(AuditLogOperationType& type)
{
    std::string operationType;
    switch(type)
    {
    case AuditLogOperationType::Added:
        operationType = "Added";
        break;
    case AuditLogOperationType::Updated:
        operationType = "Updated";
        break;
    case AuditLogOperationType::Removed:
        operationType = "Removed";
        break;
    case AuditLogOperationType::Connect:
        operationType = "Connect";
        break;
    case AuditLogOperationType::Disconnect:
        operationType = "Disconnect";
        break;
    case AuditLogOperationType::Fail:
        operationType = "Fail";
    default:
        operationType = "Updated";
        break;
    };

    return operationType;
}

static AuditLogOperationType StringToAuditLogOperationType(std::string & str)
{
    AuditLogOperationType type;
    if(0 == str.compare("Added")) {
        type = AuditLogOperationType::Added;
    } else if(0 == str.compare("Updated")) {
        type = AuditLogOperationType::Updated;
    } else if(0 == str.compare("Removed")) {
        type = AuditLogOperationType::Removed;
    } else if(0 == str.compare("Connect")){
         type = AuditLogOperationType::Connect;
    } else if(0 == str.compare("Disconnect")){
        type = AuditLogOperationType::Disconnect;
    } else if(0 == str.compare("Fail")){
        type = AuditLogOperationType::Fail;
    } else {
        type = AuditLogOperationType::Updated;
    }

    return type;
}

// for Login
struct AuditLogLogInActionDetail
{
    bool isSuccess = false;
    int64_t startUtcTimeMsec = 0;
    int64_t endUtcTimeMsec = 0;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogLogInActionDetail& detail )
{
    ar.StartObject();
    ar.Member("isSuccess") & detail.isSuccess;
    ar.Member("startUtcTimeMsec") & detail.startUtcTimeMsec;
    ar.Member("endUtcTimeMsec") & detail.endUtcTimeMsec;
    return ar.EndObject();
}

// for BackupSettings
struct AuditLogBackupSettingsActionDetail
{
    bool isSuccess = false;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogBackupSettingsActionDetail& detail)
{
    ar.StartObject();
    ar.Member("isSuccess") & detail.isSuccess;
    return ar.EndObject();
}

// for RestoreSettings
struct AuditLogRestoreSettingsActionDetail
{
    bool isSuccess = false;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogRestoreSettingsActionDetail& detail)
{
    ar.StartObject();
    ar.Member("isSuccess") & detail.isSuccess;
    return ar.EndObject();
}


// for InitializeSettings
struct AuditLogInitializeSettingsActionDetail
{
    bool isSuccess = false;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogInitializeSettingsActionDetail& detail)
{
    ar.StartObject();
    ar.Member("isSuccess") & detail.isSuccess;
    return ar.EndObject();
}


// for User
struct AuditLogUserActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
    std::string userName;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogUserActionDetail& detail)
{
    ar.StartObject();
    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    ar.Member("userName") & detail.userName;
    return ar.EndObject();
}

// for UserGroup
struct AuditLogUserGroupActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
    std::string userGroupName;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogUserGroupActionDetail& detail)
{
    ar.StartObject();
    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    ar.Member("userGroupName") & detail.userGroupName;
    return ar.EndObject();
}

// for EventRuleAction
struct AuditLogEventRuleActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
    std::string eventRuleName;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogEventRuleActionDetail& detail)
{
    ar.StartObject();

    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    ar.Member("eventRuleName") & detail.eventRuleName;
    return ar.EndObject();
}


// for ScheduleAction
struct AuditLogScheduleActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
    std::string scheduleName;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogScheduleActionDetail& detail)
{
    ar.StartObject();

    std::string operationType = AuditLogOperationTypeToString(detail.operationType);

    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    ar.Member("scheduleName") & detail.scheduleName;

    return ar.EndObject();
}

// for DeviceAction
struct AuditLogDeviceActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogDeviceActionDetail& detail)
{
    ar.StartObject();

    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    return ar.EndObject();
}

// for ChannelAction
struct AuditLogChannelActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Updated;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogChannelActionDetail& detail)
{
    ar.StartObject();

    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    return ar.EndObject();
}

// for DeviceStatusAction
struct AuditLogDeviceStatusActionDetail
{
    AuditLogOperationType operationType = AuditLogOperationType::Connect;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogDeviceStatusActionDetail& detail)
{
    ar.StartObject();

    std::string operationType = AuditLogOperationTypeToString(detail.operationType);
    ar.Member("operationType") & operationType;

    if(ar.IsReader){
        detail.operationType = StringToAuditLogOperationType(operationType);
    }

    return ar.EndObject();
}

// for PTZControlAction
struct AuditLogPTZControlActionDetail
{
    int64_t startUtcTimeMsec = 0;
    int64_t endUtcTimeMsec = 0;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogPTZControlActionDetail& detail)
{
    ar.StartObject();
    ar.Member("startUtcTimeMsec") & detail.startUtcTimeMsec;
    ar.Member("endUtcTimeMsec") & detail.endUtcTimeMsec;
    return ar.EndObject();
}

// WatchingLive
struct AuditLogWatchingLiveDetail
{
    int64_t startUtcTimeMsec = 0;
    int64_t endUtcTimeMsec = 0;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogWatchingLiveDetail& detail)
{
    ar.StartObject();
    ar.Member("startUtcTimeMsec") & detail.startUtcTimeMsec;
    ar.Member("endUtcTimeMsec") & detail.endUtcTimeMsec;

    return ar.EndObject();
}

// WatchingPlayBack
struct AuditLogWatchingPlayBackDetail
{
    int64_t startUtcTimeMsec = 0;
    int64_t endUtcTimeMsec = 0;
    unsigned int trackID = 0;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogWatchingPlayBackDetail& detail)
{
    ar.StartObject();
    ar.Member("startUtcTimeMsec") & detail.startUtcTimeMsec;
    ar.Member("endUtcTimeMsec") & detail.endUtcTimeMsec;
    ar.Member("trackID") & detail.trackID;

    return ar.EndObject();
}


// ExportingVideo
struct AuditLogExportingVideoDetail
{
    int64_t startUtcTimeMsec = 0;
    int64_t endUtcTimeMsec = 0;
    unsigned int trackID = 0;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogExportingVideoDetail& detail)
{
    ar.StartObject();
    ar.Member("startUtcTimeMsec") & detail.startUtcTimeMsec;
    ar.Member("endUtcTimeMsec") & detail.endUtcTimeMsec;
    ar.Member("trackID") & detail.trackID;

    return ar.EndObject();
}

// for UpdateSoftware
struct AuditLogUpdateSoftwareActionDetail
{
    bool isSuccess = false;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, AuditLogUpdateSoftwareActionDetail& detail)
{
    ar.StartObject();
    ar.Member("isSuccess") & detail.isSuccess;
    return ar.EndObject();
}

struct Bookmark
{
    uuid_string bookmarkID;
    uuid_string deviceID;
    std::string channelID;
    std::string name;
    std::string description;
    std::string tag;
    int64_t startTime = 0;
    int64_t endTime = 0;
    int overlappedID = -1;
    std::vector<unsigned char> image;
    std::string eventLogID;
};

struct EventEmail
{
    std::string sender;
    std::string receiver;
    std::string serverAddress;
    std::string id;
    std::string password;
    std::string port;
    bool isAuthentication = false;
};

struct EventRule
{
    uuid_string eventRuleID;
    std::string name;
    bool isEnabled = false;
    uuid_string scheduleID;

    struct ItemID
    {
        bool isService = false;
        bool isDevice = false;
        bool isChannel = false;
        bool isAlarm = false;
        uuid_string serviceID = "";
        uuid_string deviceID = "";
        uuid_string channelID = "";
        uuid_string alarmID = ""; // AlaramIn ID, AlarmOut ID

        bool operator<(const ItemID& rhs) const
        {            
            int compare = serviceID.compare(rhs.serviceID);
            if(compare > 0){
                return true;
            }else if(compare < 0){
                return false;
            }

            compare = deviceID.compare(rhs.deviceID);
            if(compare > 0){
                return true;
            }else if(compare < 0){
                return false;
            }

            compare = channelID.compare(rhs.channelID);
            if(compare > 0){
                return true;
            }else if(compare < 0){
                return false;
            }

            compare = alarmID.compare(rhs.alarmID);

            if(compare > 0){
                return true;
            }

            return false;
        }
    };

    struct Event
    {
        std::string type;
        bool isAllItem = false;
        std::set<ItemID> itemIDs;
    };

    std::vector<EventRule::Event> events;
    long long waitingSecondTime = 0;


    struct AlertAlarmAction
    {
        bool useAction = false;
        long long intervalSeconds = 0;

        std::set<ItemID> itemIDs;
    };

    struct AlarmOutputAction
    {
        bool useAction = false;
        long long intervalSeconds = 0;
        std::set<ItemID> itemIDs;
    };

    struct LayoutChangeAction
    {
        bool useAction = false;
        long long intervalSeconds = 0;
        uuid_string layoutID = "";
        std::set<ItemID> itemIDs;
    };

    struct EmailAction
    {
        bool useAction = false;
        long long intervalSeconds = 0;
        std::set<ItemID> itemIDs;
    };

    struct Actions {

        EventRule::AlertAlarmAction alertAlarmAction; //alert alarm
        EventRule::AlarmOutputAction alarmOutputAction;
        EventRule::LayoutChangeAction layoutChangeAction;
        EventRule::EmailAction emailAction;
    };

    Actions actions;
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::ItemID& itemID)
{
    ar.StartObject();
    ar.Member("isService") & itemID.isService;
    ar.Member("isDevice") & itemID.isDevice;
    ar.Member("isChannel") & itemID.isChannel;
    ar.Member("isAlarm") & itemID.isAlarm;
    ar.Member("serviceID") & itemID.serviceID;
    ar.Member("deviceID") & itemID.deviceID;
    ar.Member("channelID") & itemID.channelID;
    ar.Member("alarmID") & itemID.alarmID;
    return ar.EndObject();
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::Event& event)
{
    ar.StartObject();
    ar.Member("type") & event.type;
    ar.Member("isAllItem") & event.isAllItem;

    ar.Member("items");
    size_t itemCount = event.itemIDs.size();
    ar.StartArray(&itemCount);

    if(ar.IsReader){
        for(size_t i = 0; i < itemCount; i++){
            EventRule::ItemID itemID;
            ar & itemID;
            event.itemIDs.insert(itemID);
        }
    }else{
        for(auto itemID : event.itemIDs){
            ar & itemID;
        }
    }
    ar.EndArray();

    return ar.EndObject();
};


template <typename Archiver>
Archiver& operator&(Archiver& ar, std::vector<EventRule::Event>& events)
{
    ar.StartObject();
    ar.Member("events");
    size_t eventCount = events.size();
    ar.StartArray(&eventCount);

    if(ar.IsReader){
        events.resize(eventCount);
    }

    for(size_t i=0; i < eventCount; i++) {
        ar & events[i];
    }
    ar.EndArray();

    return ar.EndObject();
};

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::AlertAlarmAction& action)
{
    ar.StartObject();
    ar.Member("useAction") & action.useAction;
    ar.Member("intervalSeconds") & action.intervalSeconds;

    ar.Member("items");
    size_t itemCount = action.itemIDs.size();
    ar.StartArray(&itemCount);

    if(ar.IsReader){
        for(size_t i = 0; i < itemCount; i++){
            EventRule::ItemID itemID;
            ar & itemID;
            action.itemIDs.insert(itemID);
        }
    }else{
        for(auto itemID : action.itemIDs){
            ar & itemID;
        }
    }
    ar.EndArray();

    return ar.EndObject();
}

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::AlarmOutputAction& action)
{
    ar.StartObject();
    ar.Member("useAction") & action.useAction;
    ar.Member("intervalSeconds") & action.intervalSeconds;

    ar.Member("items");
    size_t itemCount = action.itemIDs.size();
    ar.StartArray(&itemCount);

    if(ar.IsReader){
        for(size_t i = 0; i < itemCount; i++){
            EventRule::ItemID itemID;
            ar & itemID;
            action.itemIDs.insert(itemID);
        }
    }else{
        for(auto itemID : action.itemIDs){
            ar & itemID;
        }
    }
    ar.EndArray();

    return ar.EndObject();
}

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::EmailAction& action)
{
    ar.StartObject();
    ar.Member("useAction") & action.useAction;
    ar.Member("intervalSeconds") & action.intervalSeconds;

    ar.Member("items");
    size_t itemCount = action.itemIDs.size();
    ar.StartArray(&itemCount);

    if(ar.IsReader){
        for(size_t i = 0; i < itemCount; i++){
            EventRule::ItemID itemID;
            ar & itemID;
            action.itemIDs.insert(itemID);
        }
    }else{
        for(auto itemID : action.itemIDs){
            ar & itemID;
        }
    }
    ar.EndArray();

    return ar.EndObject();
}

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::LayoutChangeAction& action)
{
    ar.StartObject();
    ar.Member("useAction") & action.useAction;
    ar.Member("intervalSeconds") & action.intervalSeconds;
    ar.Member("layoutID") & action.layoutID;

    ar.Member("items");
    size_t itemCount = action.itemIDs.size();
    ar.StartArray(&itemCount);

    if(ar.IsReader){
        for(size_t i = 0; i < itemCount; i++){
            EventRule::ItemID itemID;
            ar & itemID;
            action.itemIDs.insert(itemID);
        }
    }else{
        for(auto itemID : action.itemIDs){
            ar & itemID;
        }
    }
    ar.EndArray();

    return ar.EndObject();
}

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventRule::Actions& actions)
{
    ar.StartObject();

    ar.Member("alertAlarmAction") & actions.alertAlarmAction;
    ar.Member("alarmOutputAction") & actions.alarmOutputAction;
    ar.Member("layoutChangeAction") & actions.layoutChangeAction;
    ar.Member("emailAction") & actions.emailAction;

    return ar.EndObject();
}

struct EventLog
{
    std::string eventLogID = "";

    long long serviceUtcTimeMsec = 0;
    long long deviceUtcTimeMsec = 0;

    std::string type = "";

    bool isService = false;
    uuid_string serviceID = "";

    bool isDevice = false;
    uuid_string deviceID = "";

    bool isChannel = false;
    uuid_string channelID = "";

    bool isStart = false;

    struct Parameters{
        std::map<std::string, std::string> source;
        std::map<std::string, std::string> data;
    };

    Parameters parameters;

};
typedef std::shared_ptr<EventLog> EventLogSharedPtr;

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventLog::Parameters& parameters)
{
    ar.StartObject();

    {
        ar.Member("sourceParameters");
        size_t itemCount = parameters.source.size();
        ar.StartArray(&itemCount);

        if(ar.IsReader){
            for(size_t i = 0; i < itemCount; i++){
                std::string name = "";
                std::string value = "";

                ar.StartObject();
                ar.Member("name") & name;
                ar.Member("value") & value;
                ar.EndObject();

                parameters.source[name] = value;
            }
        }else{
            for(auto& param : parameters.source){
                std::string name = param.first;
                std::string value = param.second;
                ar.StartObject();
                ar.Member("name") & name;
                ar.Member("value") & value;
                ar.EndObject();
            }
        }
        ar.EndArray();
    }
    {
        ar.Member("dataParameters");
        size_t itemCount = parameters.data.size();
        ar.StartArray(&itemCount);

        if(ar.IsReader){
            for(size_t i = 0; i < itemCount; i++){
                std::string name = "";
                std::string value = "";

                ar.StartObject();
                ar.Member("name") & name;
                ar.Member("value") & value;
                ar.EndObject();

                parameters.data[name] = value;
            }
        }else{
            for(auto& param : parameters.data){
                std::string name = param.first;
                std::string value = param.second;
                ar.StartObject();
                ar.Member("name") & name;
                ar.Member("value") & value;
                ar.EndObject();
            }
        }
        ar.EndArray();
    }

    return ar.EndObject();
}

struct DeletedDevice
{
    struct Channel{
        uuid_string deviceID;
        std::string channelID;
        std::string name; // DB name
    };

    uuid_string deviceID;
    long long serviceUtcTimeMsec = 0;
    std::string host;
    std::string modelName;
    std::string name; // DB name
    std::string macAddress;

    std::map<std::string, Channel> channels;
};
typedef std::shared_ptr<DeletedDevice> DeletedDeviceSharedPtr;

struct EventSchedule
{
    std::string scheduleID = "";
    std::string scheduleName = "";
    //std::array<bool, 8*24> schedule = {false};
    std::string schedule = "";

};
typedef std::shared_ptr<EventSchedule> EventScheduleSharedPtr;

template <typename Archiver>
Archiver& operator&(Archiver& ar, EventSchedule& eventSchedule)
{
    ar.StartObject();

    ar.Member("scheduleID") & eventSchedule.scheduleID;
    ar.Member("scheduleName") & eventSchedule.scheduleName;
    ar.Member("schedule") & eventSchedule.schedule;

    /*std::stringstream scheduleList;

    for(auto& param : eventSchedule.schedule){
        scheduleList << std::noboolalpha  << param;
    }

    ar.Member("schedule") & scheduleList.str();*/

    return ar.EndObject();
}

enum class DiscoverAddressType
{
    StaticIP = 0,
    DHCP,
    PPPoE
};

enum class InternetProtocolVersion
{
    IPv4,
    IPv6
};

struct DiscoveryDeviceInfo
{
    Wisenet::Core::InternetProtocolVersion ipVersion = Wisenet::Core::InternetProtocolVersion::IPv4;
    Wisenet::Device::DeviceType  deviceType = Device::DeviceType::Unknown;
    DiscoverAddressType     addressType = DiscoverAddressType::StaticIP;
    std::string             manufacturer;
    std::string             model;
    std::string             ipAddress;
    std::string             subnetMaskAddress;
    std::string             gatewayAddress;
    std::string             macAddress;
    unsigned int            devicePort = 0;
    unsigned int            httpPort = 0;
    unsigned int            httpsPort = 0;
    bool                    isHttpsEnable = false;
    bool                    supportIpConfig = false;
    bool                    supportDhcpConfig = false;
    bool                    supportSunapiOnly = false;
    bool                    supportPasswordConfig = false;
    bool                    needToPasswordConfig = false;
    bool                    supportPasswordEncryption = false;
    std::string             passwordNonce;
};

struct IpSettingInfo
{
    DiscoverAddressType     addressType = DiscoverAddressType::StaticIP;
    bool                    supportPasswordEncryption = false;
    unsigned int            devicePort = 0;
    unsigned int            httpPort = 0;
    std::string             ipAddress;
    std::string             subnetMaskAddress;
    std::string             gatewayAddress;
    std::string             macAddress;    
    std::string             id;
    std::string             password;
    std::string             passwordNonce;
};

struct CameraSettingGeneral
{
    struct ImageControl
    {
        std::string ratio;
        std::string rotation;
    };
    
    ImageControl image;
    bool enableAudio = false;
    
    std::string mediaStreaming;
    
    struct Ptz
    {
        bool enablePanTilt = false;
        bool enableZoom = false;
    };
    Ptz ptz;
};

struct CameraSettingFisheye
{
    bool enableDewarping = false;
    bool showGrid = false;
    int size = 0;
    int offsetX = 0;
    int offsetY = 0;
    int ellide = 0;
    std::string angle;
    std::string location;
    std::string lensDistance;
};

struct DashboardData
{
    int saveTime = 0;

    int powerErrorCount = 0;
    int diskErrorCount = 0;
    int overloadErrorCount = 0;
    int connectionErrorCount = 0;
    int fanErrorCount = 0;
    int disconnectedCameraCount = 0;
};

struct LdapSetting
{
    std::string serverUrl= "";
    std::string adminDN = "";
    std::string adminPW = "";
    std::string searchBase = "";
    std::string searchFilter = "";
    bool isSsl = false;
    bool isServerChanged = false;
};

struct LdapConnectionResult
{
    bool connSuccess = false;
    bool searchSuccess = false;
    std::string errorMsg = "";
};

struct LdapUserInfo {
    std::string uid = "";
    std::string name = "";
    std::string email = "";
    std::string dn = "";
    std::string objectclass = "";
};

enum class FirmwareUpdateStatus
{
    Unknown = -1,
    Latest = 0,
    NeedToUpdate,
};

struct Statistics{
    std::string machineId = "";
    int installedDatetime = 0;
    int executionCount = 0;
    int executionMinutes = 0;

    int layoutOpenCount = 0;
    int channelOpenCount = 0;

    int mediaRequestCount = 0;
    int averageMediaResponseTime = 0;

    int currentProcessMediaRequestCount = 0;
    int currentProcessTotalMediaResponseTime = 0;
};

struct LicenseInfo{
    std::string licenseKey = "";
    bool activated = false;
};

}
}

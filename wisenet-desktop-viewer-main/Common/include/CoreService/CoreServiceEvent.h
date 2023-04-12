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

#include "CoreService/CoreServiceStructure.h"

namespace Wisenet
{
namespace Core
{

enum CoreEventType
{
    CoreEventTypeStart = 10000,

    FullInfoEventType, // device event id end + 1
    SaveUserGroupEventType,
    RemoveUserGroupsEventType,
    SaveUserEventType,
    RemoveUsersEventType,
    SaveLayoutEventType,
    RemoveLayoutsEventType,
    SaveSequenceLayoutEventType,
    RemoveSequenceLayoutsEventType,
    SaveWebpageEventType,
    RemoveWebpagesEventType,
    AddDeviceEventType,
    RemoveDevicesEventType,
    SaveBookmarkEventType,
    RemoveBookmarksEventType,
    SaveEventRuleEventType,
    RemoveEventRulesEventType,
    SaveEventScheduleEventType,
    RemoveEventSchedulesEventType,    
    EmailEventType,
    SaveEventEmailEventType,
    RemoveEventEmailEventType,
    SaveChannelsEventType,
    SaveDevicesCredentialEventType,
    RestoreServiceEventType,
    ResetServiceEventType,
    ShowSystemAlarmPanelEventType,
    ShowEventPanelEventType,
    OpenLayoutEventType,
    SaveGroupEventType,
    RemoveGroupsEventType,
    AddGroupMappingEventType,
    RemoveGroupMappingEventType,
    UpdateGroupMappingEventType,
    UpdateDevicesEventType,
    AddLocalFileEventType,
    RemoveLocalFileEventType,
    SaveLayoutsEventType,

    PriorityAlarmEventType,

    LoadLatestSettingsEventType,
    ShowSystemUpdatePanelEventType,

    UpdateVerifyResultEventType,
    ResourceDataEventType,

    DashboardDataUpdatedEventType,

    UpdateLdapSettingEventType,    
    ActivateLicenseEventType,

    CoreEventTypeEnd // end of coreservice event type
};


struct FullInfoEvent : EventBase
{
    std::string EventType() override
    {
        return "FullInfoEvent";
    }

    int EventTypeId() override
    {
        return FullInfoEventType;
    }

    std::map<uuid_string, UserGroup>             userRoles;
    std::map<std::string, User>                 users; //key : userName
    std::map<uuid_string, Group>                groups;
    std::map<std::string, uuid_string>          channelGroupMappings;
    std::map<uuid_string, Layout>               layouts;
    std::map<uuid_string, SequenceLayout>       sequenceLayouts;
    std::map<uuid_string, Webpage>              webpages;
    std::map<uuid_string, Device::Device>       devices;
    std::map<uuid_string, Device::DeviceStatus> devicesStatus;
    std::map<uuid_string, Bookmark>             bookmarks;
    std::map<uuid_string, EventRule>            eventRules;
    std::map<uuid_string, EventSchedule>        eventSchedules;
    std::shared_ptr<EventEmail>                 eventEmail;
    LdapSetting                                 ldapSetting;
    Statistics                                  statistics;
    LicenseInfo                                 licenseInfo;

    std::set<std::string>                       localfiles;

    std::map<uuid_string, std::map<std::string, bool>> deviceLatestEventStatus;
};
typedef std::shared_ptr<FullInfoEvent> FullInfoEventSharedPtr;

struct SaveUserGroupEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveUserGroupEvent";
    }

    int EventTypeId() override
    {
        return SaveUserGroupEventType;
    }

    UserGroup userGroup;
};
typedef std::shared_ptr<SaveUserGroupEvent> SaveUserGroupEventSharedPtr;

struct RemoveUserGroupsEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveUserGroupsEvent";
    }

    int EventTypeId() override
    {
        return RemoveUserGroupsEventType;
    }

     std::vector<std::string> userGroupIDs;
};
typedef std::shared_ptr<RemoveUserGroupsEvent> RemoveUserGroupsEventSharedPtr;

struct SaveUserEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveUserEvent";
    }

    int EventTypeId() override
    {
        return SaveUserEventType;
    }

    User    user;
};
typedef std::shared_ptr<SaveUserEvent> SaveUserEventSharedPtr;

struct RemoveUsersEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveUsersEvent";
    }

    int EventTypeId() override
    {
        return RemoveUsersEventType;
    }

    std::vector<std::string> userIDs;
};
typedef std::shared_ptr<RemoveUsersEvent> RemoveUsersEventSharedPtr;

struct SaveGroupEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveGroupEvent";
    }

    int EventTypeId() override
    {
        return SaveGroupEventType;
    }

    Group   group;
};
typedef std::shared_ptr<SaveGroupEvent> SaveGroupEventSharedPtr;

struct RemoveGroupsEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveGroupsEvent";
    }

    int EventTypeId() override
    {
        return RemoveGroupsEventType;
    }

    std::vector<uuid_string> groupIDs;
};
typedef std::shared_ptr<RemoveGroupsEvent> RemoveGroupsEventSharedPtr;

struct AddGroupMappingEvent : EventBase
{
    std::string EventType() override
    {
        return "AddGroupMappingEvent";
    }

    int EventTypeId() override
    {
        return AddGroupMappingEventType;
    }

    uuid_string groupID;
    std::vector<Group::MappingItem> channels;
};
typedef std::shared_ptr<AddGroupMappingEvent> AddGroupMappingEventSharedPtr;

struct RemoveGroupMappingEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveGroupMappingEvent";
    }

    int EventTypeId() override
    {
        return RemoveGroupMappingEventType;
    }

    uuid_string groupID;
    std::vector<std::string> channelIDs;
};
typedef std::shared_ptr<RemoveGroupMappingEvent> RemoveGroupMappingEventSharedPtr;

struct UpdateGroupMappingEvent : EventBase
{
    std::string EventType() override
    {
        return "UpdateGroupMappingEvent";
    }

    int EventTypeId() override
    {
        return UpdateGroupMappingEventType;
    }

    uuid_string from;
    uuid_string to;
    std::vector<std::string> channelIDs;
};
typedef std::shared_ptr<UpdateGroupMappingEvent> UpdateGroupMappingEventSharedPtr;

struct SaveLayoutEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveLayoutEvent";
    }

    int EventTypeId() override
    {
        return SaveLayoutEventType;
    }

    Layout    layout;
};
typedef std::shared_ptr<SaveLayoutEvent> SaveLayoutEventSharedPtr;

struct SaveLayoutsEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveLayoutsEvent";
    }

    int EventTypeId() override
    {
        return SaveLayoutsEventType;
    }

    std::map<uuid_string, Layout>    layouts;
};
typedef std::shared_ptr<SaveLayoutsEvent> SaveLayoutsEventSharedPtr;

struct RemoveLayoutsEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveLayoutsEvent";
    }

    int EventTypeId() override
    {
        return RemoveLayoutsEventType;
    }

    std::vector<uuid_string> layoutIDs;
};
typedef std::shared_ptr<RemoveLayoutsEvent> RemoveLayoutsEventSharedPtr;

struct SaveSequenceLayoutEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveSequenceLayoutEvent";
    }

    int EventTypeId() override
    {
        return SaveSequenceLayoutEventType;
    }

    SequenceLayout    sequenceLayout;
};
typedef std::shared_ptr<SaveSequenceLayoutEvent> SaveSequenceLayoutEventSharedPtr;

struct RemoveSequenceLayoutsEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveSequenceLayoutsEvent";
    }

    int EventTypeId() override
    {
        return RemoveSequenceLayoutsEventType;
    }

    std::vector<uuid_string> sequenceLayoutIDs;
};
typedef std::shared_ptr<RemoveSequenceLayoutsEvent> RemoveSequenceLayoutsEventSharedPtr;

struct SaveWebpageEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveWebpageEvent";
    }

    int EventTypeId() override
    {
        return SaveWebpageEventType;
    }

    Webpage    webpage;
};
typedef std::shared_ptr<SaveWebpageEvent> SaveWebpageEventSharedPtr;

struct RemoveWebpagesEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveWebpagesEvent";
    }

    int EventTypeId() override
    {
        return RemoveWebpagesEventType;
    }

    std::vector<uuid_string> webpageIDs;
};
typedef std::shared_ptr<RemoveWebpagesEvent> RemoveWebpagesEventSharedPtr;

struct SaveBookmarkEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveBookmarkEvent";
    }

    int EventTypeId() override
    {
        return SaveBookmarkEventType;
    }

    Bookmark bookmark;
};
typedef std::shared_ptr<SaveBookmarkEvent> SaveBookmarkEventSharedPtr;

struct RemoveBookmarksEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveBookmarksEvent";
    }

    int EventTypeId() override
    {
        return RemoveBookmarksEventType;
    }

    std::vector<uuid_string> bookmarkIDs;
};
typedef std::shared_ptr<RemoveBookmarksEvent> RemoveBookmarksEventSharedPtr;

struct SaveEventRuleEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveEventRuleEvent";
    }

    int EventTypeId() override
    {
        return SaveEventRuleEventType;
    }

    EventRule eventRule;
};
typedef std::shared_ptr<SaveEventRuleEvent> SaveEventRuleEventSharedPtr;

struct RemoveEventRulesEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveEventRulesEvent";
    }

    int EventTypeId() override
    {
        return RemoveEventRulesEventType;
    }

    std::vector<uuid_string> eventRuleIDs;
    std::vector<uuid_string> eventScheduleIDs;
};
typedef std::shared_ptr<RemoveEventRulesEvent> RemoveEventRulesEventSharedPtr;



struct SaveEventScheduleEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveEventScheduleEvent";
    }

    int EventTypeId() override
    {
        return SaveEventScheduleEventType;
    }

    EventSchedule eventSchedule;
};
typedef std::shared_ptr<SaveEventScheduleEvent> SaveEventScheduleEventSharedPtr;

struct RemoveEventSchedulesEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveEventSchedulesEvent";
    }

    int EventTypeId() override
    {
        return RemoveEventSchedulesEventType;
    }

    std::vector<uuid_string> eventScheduleIDs;
};
typedef std::shared_ptr<RemoveEventSchedulesEvent> RemoveEventSchedulesEventSharedPtr;

struct SaveEventEmailEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveEventEmailEvent";
    }

    int EventTypeId() override
    {
        return SaveEventEmailEventType;
    }

    std::shared_ptr<EventEmail> eventEmail;
};
typedef std::shared_ptr<SaveEventEmailEvent> SaveEventEmailEventSharedPtr;

struct RemoveEventEmailEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveEventEmailEvent";
    }

    int EventTypeId() override
    {
        return RemoveEventEmailEventType;
    }
};
typedef std::shared_ptr<RemoveEventEmailEvent> RemoveEventEmailEventSharedPtr;

struct AddDeviceEvent : EventBase
{
    std::string EventType() override
    {
        return "AddDeviceEvent";
    }
    int EventTypeId() override
    {
        return AddDeviceEventType;
    }
    Device::Device  device;
    Device::DeviceStatus deviceStatus;
};
typedef std::shared_ptr<AddDeviceEvent> AddDeviceEventSharedPtr;

struct RemoveDevicesEvent  : EventBase
{
    std::string EventType() override
    {
        return "RemoveDevicesEvent";
    }

    int EventTypeId() override
    {
        return RemoveDevicesEventType;
    }

    std::vector<uuid_string> deviceIDs;
};
typedef std::shared_ptr<RemoveDevicesEvent> RemoveDevicesEventSharedPtr;

struct UpdateDevicesEvent  : EventBase
{
    std::string EventType() override
    {
        return "UpdateDevicesEvent";
    }

    int EventTypeId() override
    {
        return UpdateDevicesEventType;
    }

    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<UpdateDevicesEvent> UpdateDevicesEventSharedPtr;

struct SaveChannelsEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveChannelsEvent";
    }

    int EventTypeId() override
    {
        return SaveChannelsEventType;
    }

    std::vector<Device::Device::Channel> channels;
};
typedef std::shared_ptr<SaveChannelsEvent> SaveChannelsEventSharedPtr;

struct SaveDevicesCredentialEvent : EventBase
{
    std::string EventType() override
    {
        return "SaveDevicesCredentialEvent";
    }

    int EventTypeId() override
    {
        return SaveDevicesCredentialEventType;
    }

    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<SaveDevicesCredentialEvent> SaveDevicesCredentialEventSharedPtr;

struct RestoreServiceEvent : EventBase
{
    std::string EventType() override
    {
        return "RestoreServiceEvent";
    }
    int EventTypeId() override
    {
        return RestoreServiceEventType;
    }
};
typedef std::shared_ptr<RestoreServiceEvent> RestoreServiceEventSharedPtr;

struct ResetServiceEvent : EventBase
{
    std::string EventType() override
    {
        return "ResetServiceEvent";
    }
    int EventTypeId() override
    {
        return ResetServiceEventType;
    }
};
typedef std::shared_ptr<ResetServiceEvent> ResetServiceEventSharedPtr;

struct ShowSystemAlarmPanelEvent : EventBase
{
    int intervalSeconds = 0;

    std::string EventType() override
    {
        return "ShowSystemAlarmPanelEventType";
    }

    int EventTypeId() override
    {
        return ShowSystemAlarmPanelEventType;
    }

    EventLog eventLog;
};

struct ShowEventPanelEvent : EventBase
{
    int intervalSeconds = 0;

    std::string EventType() override
    {
        return "ShowEventPanelEventType";
    }

    int EventTypeId() override
    {
        return ShowEventPanelEventType;
    }

    EventLog eventLog;
};

struct PriorityAlarmEvent : EventBase
{
    int intervalSeconds = 0;

    std::string eventRuleName;

    long long serviceUtcTimeMsec = 0;
    long long deviceUtcTimeMsec = 0;

    std::string deviceID;
    std::string channelID;

    std::string EventType() override
    {
        return "PriorityAlarmEventType";
    }

    int EventTypeId() override
    {
        return PriorityAlarmEventType;
    }

    std::string ToString()
    {
        std::stringstream ss;
        ss << "PriorityAlarmEvent: eventRuleName(" << eventRuleName << "),";
        ss << "time(" << serviceUtcTimeMsec << "),";
        ss << "deviceID(" << deviceID << "),";
        ss << "channelID("<< channelID << ")";

        return ss.str();
    }
};

struct EmailEvent : EventBase
{
    std::string EventType() override
    {
        return "EmailEventType";
    }

    int EventTypeId() override
    {
        return EmailEventType;
    }

    std::set<EventRule::ItemID> itemIDs;
};

struct OpenLayoutEvent : EventBase
{
    std::string EventType() override
    {
        return "OpenLayoutEventType";
    }

    int EventTypeId() override
    {
        return OpenLayoutEventType;
    }

    uuid_string layoutID;
};

struct AddLocalFileEvent : EventBase
{
    std::string EventType() override
    {
        return "AddLocalFileEventType";
    }

    int EventTypeId() override
    {
        return AddLocalFileEventType;
    }

    uuid_string localfilePath;
};

struct RemoveLocalFileEvent : EventBase
{
    std::string EventType() override
    {
        return "RemoveLocalFileEventType";
    }

    int EventTypeId() override
    {
        return RemoveLocalFileEventType;
    }

    uuid_string localfilePath;
};

struct LoadLatestSettingsEvent : EventBase
{
    std::string EventType() override
    {
        return "LoadLatestSettingsEventType";
    }

    int EventTypeId() override
    {
        return LoadLatestSettingsEventType;
    }
};

struct ShowSystemUpdatePanelEvent : EventBase
{
    int intervalSeconds = 0;

    std::string EventType() override
    {
        return "ShowSystemUpdatePanelEvent";
    }

    int EventTypeId() override
    {
        return ShowSystemUpdatePanelEventType;
    }

    AuditLog auditLog;
};

struct UpdateVerifyResultEvent : EventBase
{
    std::string EventType() override
    {
        return "UpdateVerifyResultEvent";
    }

    int EventTypeId() override
    {
        return UpdateVerifyResultEventType;
    }
    std::string fileUuid;
    int verifyResult = 0;
};

struct ResourceDataEvent : EventBase
{
    std::string EventType() override
    {
        return "ResourceDataEvent";
    }

    int EventTypeId() override
    {
        return ResourceDataEventType;
    }

    double cpuUsage = 0.0;
    long long usedMemory = 0;
    long long totalMemory = 0;
};

struct DashboardDataUpdatedEvent : EventBase
{
    std::string EventType() override
    {
        return "DashboardDataUpdatedEvent";
    }

    int EventTypeId() override
    {
        return DashboardDataUpdatedEventType;
    }
};

struct UpdateLdapSettingEvent : EventBase
{
    std::string EventType() override
    {
        return "UpdateLdapSettingEvent";
    }
    int EventTypeId() override
    {
        return UpdateLdapSettingEventType;
    }
    LdapSetting ldapSetting;
};
typedef std::shared_ptr<UpdateLdapSettingEvent> UpdateLdapSettingEventSharedPtr;

struct ActivateLicenseEvent : EventBase
{
    std::string EventType() override
    {
        return "ActivateLicenseEvent";
    }
    int EventTypeId() override
    {
        return ActivateLicenseEventType;
    }
    LicenseInfo licenseInfo;
};
typedef std::shared_ptr<ActivateLicenseEvent> ActivateLicenseEventSharedPtr;

}
}

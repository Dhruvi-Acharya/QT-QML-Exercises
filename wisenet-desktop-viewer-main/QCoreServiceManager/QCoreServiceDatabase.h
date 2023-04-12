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

#include <QObject>
#include <QString>
#include <QDirIterator>
#include <mutex>
#include <QDebug>
#include <QUrl>

#include "CoreService/CoreServiceEvent.h"
#include "DeviceClient/DeviceEvent.h"
#include "CoreService/CoreServiceRequestResponse.h"
#include "OpenLayoutIItem.h"

class QCoreServiceDatabase : public QObject
{
    Q_OBJECT
public:
    explicit QCoreServiceDatabase(QObject *parent = nullptr);
    ~QCoreServiceDatabase();

    void SetLoginUserID(std::string& userId);

    /* update local data structure */
    void Update(const Wisenet::EventBaseSharedPtr &event);

    /* get all configuraitons */
    std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> GetUserGroups();
    std::map<std::string, Wisenet::Core::User> GetUsers();
    std::map<Wisenet::uuid_string, Wisenet::Core::Group> GetGroups();
    std::map<std::string, Wisenet::uuid_string> GetChannelGroupMappings();
    std::map<Wisenet::uuid_string, Wisenet::Core::Layout> GetLayouts();
    std::map<Wisenet::uuid_string, Wisenet::Core::SequenceLayout> GetSequenceLayouts();
    std::map<Wisenet::uuid_string, Wisenet::Core::Webpage> GetWebpages();
    std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> GetBookmarks();
    std::map<Wisenet::uuid_string, Wisenet::Core::EventRule> GetEventRules();
    std::map<Wisenet::uuid_string, Wisenet::Core::EventSchedule> GetEventSchedules();
    std::shared_ptr<Wisenet::Core::EventEmail> GetEventEmail();
    Wisenet::Core::LdapSetting GetLdapSetting();
    Wisenet::Core::Statistics GetStatistics();
    Wisenet::Core::LicenseInfo GetLicenseInfo();

    std::map<Wisenet::uuid_string, Wisenet::Device::Device> GetDevices();
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceStatus> GetDevicesStatus();
    std::map<Wisenet::uuid_string, std::map<std::string, bool>>  GetDeviceLatestEventStatus();


    QStringList GetTotalEventList();
    QSet<QString> GetDynamicEventList();

    void GetLoginUser(Wisenet::Core::User& user);
    void GetLoginUserGroup(Wisenet::Core::UserGroup& userGroup);
    Wisenet::Core::UserGroup::Permission GetLoginUserPermission();

    /* get a configuration by uuid */
    bool FindUserRole(const std::string& uuid, Wisenet::Core::UserGroup& retUserRole);
    bool FindUser(const std::string& uuid, Wisenet::Core::User& retUser);
    bool FindGroup(const std::string& uuid, Wisenet::Core::Group& retGroup);
    bool FindLayout(const std::string& uuid, Wisenet::Core::Layout& retLayout);
    bool FindSequenceLayout(const std::string& uuid, Wisenet::Core::SequenceLayout& retSequenceLayout);
    bool FindWebpage(const std::string& uuid, Wisenet::Core::Webpage& retWebpage);
    bool FindBookmark(const std::string& uuid, Wisenet::Core::Bookmark& retBookmark);
    bool FindEventRule(const std::string& uuid, Wisenet::Core::EventRule& retEventRule);
    bool FindEventSchedule(const std::string& uuid, Wisenet::Core::EventSchedule& retEventSchedule);
    bool FindDevice(const std::string& uuid, Wisenet::Device::Device& retDevice);
    bool FindDeviceStatus(const std::string& uuid, Wisenet::Device::DeviceStatus& retDeviceStatus);
    bool FindChannelStatus(const std::string& deviceId, const std::string& channelId,
                           Wisenet::Device::ChannelStatus& retChannelStatus);
    bool FindChannel(const std::string& deviceId, const std::string& channelId,
                     Wisenet::Device::Device::Channel& retChannel);
    bool FIndDeviceLatestEventStatus(const std::string& uuid,
                                     std::map<std::string, bool>& retDeviceStatus);


    /* use QString parameter */
    bool FindUserRole(const QString& uuid, Wisenet::Core::UserGroup& retUserRole)
    { return FindUserRole(uuid.toStdString(), retUserRole);}

    bool FindUser(const QString& userName, Wisenet::Core::User& retUser)
    { return FindUser(userName.toStdString(), retUser);}

    bool FindGroup(const QString& groupId, Wisenet::Core::Group& retGroup)
    { return FindGroup(groupId.toStdString(), retGroup);}

    bool FindLayout(const QString& uuid, Wisenet::Core::Layout& retLayout)
    { return FindLayout(uuid.toStdString(), retLayout);}

    bool FindSequenceLayout(const QString& uuid, Wisenet::Core::SequenceLayout& retSequenceLayout)
    { return FindSequenceLayout(uuid.toStdString(), retSequenceLayout);}

    bool FindWebpage(const QString& uuid, Wisenet::Core::Webpage& retWebpage)
    { return FindWebpage(uuid.toStdString(), retWebpage);}

    bool FindBookmark(const QString& uuid, Wisenet::Core::Bookmark& retBookmark)
    { return FindBookmark(uuid.toStdString(), retBookmark);}

    bool FindEventRule(const QString& uuid, Wisenet::Core::EventRule& retEventRule)
    { return FindEventRule(uuid.toStdString(), retEventRule);}

    bool FindEventSchedule(const QString& uuid, Wisenet::Core::EventSchedule& retEventSchedule)
    { return FindEventSchedule(uuid.toStdString(), retEventSchedule);}

    bool FindDevice(const QString& uuid, Wisenet::Device::Device& retDevice)
    { return FindDevice(uuid.toStdString(), retDevice);}

    bool FindDeviceStatus(const QString& uuid, Wisenet::Device::DeviceStatus& retDeviceStatus)
    { return FindDeviceStatus(uuid.toStdString(), retDeviceStatus);}

    bool FindChannelStatus(const QString& deviceId, const QString& channelId, Wisenet::Device::ChannelStatus& retChannelStatus)
    { return FindChannelStatus(deviceId.toStdString(), channelId.toStdString(), retChannelStatus);}

    bool FindChannel(const QString& deviceId, const QString& channelId, Wisenet::Device::Device::Channel& retChannel)
    { return FindChannel(deviceId.toStdString(), channelId.toStdString(), retChannel);}

    bool ContainsDeviceByMacAddress(const std::string& macAddress);
    bool ContainsDeviceByIp(const std::string& ipAddress, const int port);

    struct GroupResource {
        std::set<std::string> channels;
        std::set<std::string> webPages;
    };
    struct GroupRelation {
        Wisenet::uuid_string groupID;
        Wisenet::uuid_string parentGroupID;
        std::set<Wisenet::uuid_string> subGroups;
    };

    void AllResourcesInGroup(const QString& groupId, GroupResource& result);
    void FillResourceNameFromOpenLayoutItems(QList<OpenLayoutItem>& results);

    bool IsSharedLayout(QString& layoutId);
    bool IsSharedLayout(std::string layoutId);

    // Statistics
    void UpdateStatistics(const Wisenet::Core::Statistics& statistics);
    void countUpLayoutOpen(const QString& layoutId);
    void countUpMediaResponseTime(int msec);
private:
    /* update */
    void SetFullInfo(const Wisenet::Core::FullInfoEventSharedPtr& fullInfo);
    void SaveUserGroups(const Wisenet::Core::SaveUserGroupEventSharedPtr& userGroups);
    void RemoveUserGroup(const Wisenet::Core::RemoveUserGroupsEventSharedPtr& userGroup);
    void SaveUser(const Wisenet::Core::SaveUserEventSharedPtr& user);
    void RemoveUsers(const Wisenet::Core::RemoveUsersEventSharedPtr& users);
    void SaveGroup(const Wisenet::Core::SaveGroupEventSharedPtr &group);
    void RemoveGroups(const Wisenet::Core::RemoveGroupsEventSharedPtr &groups);
    void AddGroupMapping(const Wisenet::Core::AddGroupMappingEventSharedPtr &coreEvent);
    void RemoveGroupMapping(const Wisenet::Core::RemoveGroupMappingEventSharedPtr &coreEvent);
    void UpdateGroupMapping(const Wisenet::Core::UpdateGroupMappingEventSharedPtr &coreEvent);
    void SaveLayout(const Wisenet::Core::SaveLayoutEventSharedPtr& layout);
    void SaveLayouts(const Wisenet::Core::SaveLayoutsEventSharedPtr& layouts);
    void RemoveLayouts(const Wisenet::Core::RemoveLayoutsEventSharedPtr& layouts);
    void SaveSequenceLayout(const Wisenet::Core::SaveSequenceLayoutEventSharedPtr& seqLayout);
    void RemoveSequenceLayouts(const Wisenet::Core::RemoveSequenceLayoutsEventSharedPtr& seqLayouts);
    void SaveWebpage(const Wisenet::Core::SaveWebpageEventSharedPtr& webpage);
    void RemoveWebpages(const Wisenet::Core::RemoveWebpagesEventSharedPtr& webpages);
    void SaveBookmark(const Wisenet::Core::SaveBookmarkEventSharedPtr& bookMark);
    void RemoveBookmarks(const Wisenet::Core::RemoveBookmarksEventSharedPtr& bookMarks);
    void SaveEventRule(const Wisenet::Core::SaveEventRuleEventSharedPtr& rule);
    void RemoveEventRules(const Wisenet::Core::RemoveEventRulesEventSharedPtr& rules);
    void SaveEventSchedule(const Wisenet::Core::SaveEventScheduleEventSharedPtr& schedule);
    void RemoveEventSchedules(const Wisenet::Core::RemoveEventSchedulesEventSharedPtr& schedules);
    void SaveEventEmail(const Wisenet::Core::SaveEventEmailEventSharedPtr& email);
    void RemoveEventEmail();
    void AddDevice(const Wisenet::Core::AddDeviceEventSharedPtr& device);
    void RemoveDevices(const Wisenet::Core::RemoveDevicesEventSharedPtr& devices);
    void UpdateDevices(const Wisenet::Core::UpdateDevicesEventSharedPtr& devices);
    void SaveChannels(const Wisenet::Core::SaveChannelsEventSharedPtr& channels);
    void UpdateDeviceStatus(const Wisenet::Device::DeviceStatusEventSharedPtr& deviceStatus);
    void UpdateChannelsStatus(const Wisenet::Device::ChannelStatusEventSharedPtr& channelStatus);
    void UpdateDeviceLatestStatus(const Wisenet::Device::DeviceLatestStatusEventSharedPtr& deviceLatestStatus);
    void UpdateLdapSetting(const Wisenet::Core::UpdateLdapSettingEventSharedPtr& ldapSetting);
    void UpdateLicense(const Wisenet::Core::ActivateLicenseEventSharedPtr& licenseInfo);

    void cleanupSubGroupRelation(std::set<Wisenet::uuid_string>& subGroups);
private:
    std::mutex  m_mutex;
    std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> m_userGroups;
    std::map<std::string, Wisenet::Core::User> m_users;
    std::map<Wisenet::uuid_string, Wisenet::Core::Group>    m_groups;
    std::map<std::string, Wisenet::uuid_string>             m_channelGroupMappings;
    std::map<Wisenet::uuid_string, Wisenet::Core::Layout> m_layouts;
    std::map<Wisenet::uuid_string, Wisenet::Core::SequenceLayout> m_sequenceLayouts;
    std::map<Wisenet::uuid_string, Wisenet::Core::Webpage> m_webPages;
    std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> m_bookMarks;
    std::map<Wisenet::uuid_string, Wisenet::Core::EventRule> m_eventRules;
    std::map<Wisenet::uuid_string, Wisenet::Core::EventSchedule> m_eventSchedules;
    std::shared_ptr<Wisenet::Core::EventEmail> m_eventEmail;
    Wisenet::Core::LdapSetting m_ldapSetting;
    Wisenet::Core::Statistics m_statistics;
    Wisenet::Core::LicenseInfo m_licenseInfo;

    std::map<Wisenet::uuid_string, Wisenet::Device::Device> m_devices;
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceStatus> m_devicesStatus;
    std::map<Wisenet::uuid_string, std::map<std::string, bool>> m_deviceLatestEventStatus;

    std::map<Wisenet::uuid_string, GroupRelation>   m_groupRelations;

    Wisenet::Core::User m_loginUser;
    Wisenet::Core::UserGroup m_loginUserGroup;
    std::string m_loginUserId;
};


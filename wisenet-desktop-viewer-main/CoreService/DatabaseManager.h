#pragma once

#include <memory>

#include <JsonArchiver.h>
#include "database/ManagementDatabase.h"

namespace Wisenet
{
namespace Core
{

class DatabaseManager : public std::enable_shared_from_this<DatabaseManager>
{
public:
    DatabaseManager();
    ~DatabaseManager();

    bool Open();
    void Close();

    static bool Delete();

    // LogIn
    Wisenet::ErrorCode LogIn(std::string& userId, const std::string& userName, const std::string& password, const bool isHashed);

    // UserGroup
    const std::map<uuid_string, UserGroup>& GetUserGroups(){ return m_userGroups;}
    Wisenet::ErrorCode SaveUserGroup( const UserGroup& userGroup);
    Wisenet::ErrorCode RemoveUserGroups(const std::vector<std::string>&userGroupIDs,
                                      std::vector<std::string>& removedUserNames,
                                      std::vector<uuid_string> &removedLayoutIDs,
                                      std::vector<uuid_string> &removedSequenceIDs);

    //User
    const std::map<uuid_string, User>& GetUsers(){ return m_users; }
    bool IsExistUser(std::string& userName);
    Wisenet::ErrorCode SaveUser(const User& user);
    Wisenet::ErrorCode RemoveUsers(const std::vector<std::string>& userNames,
                                   std::vector<uuid_string> &removedLayoutIDs,
                                   std::vector<uuid_string> &removedSequenceIDs);

    //Group
    const std::map<uuid_string, Group>& GetGroups(){ return m_groups; }
    bool IsExistUserGroup(std::string& userGroupID);
    Wisenet::ErrorCode SaveGroup(const Group& group);
    Wisenet::ErrorCode RemoveGroups(const std::vector<uuid_string>& groupIDs, std::vector<uuid_string>& removedGroups, std::map<uuid_string, std::vector<std::string>>& removedMapping);

    //Group mapping
    const std::map<std::string, uuid_string>& GetChannelGroupMappings(){ return m_channelGroupMappings; }
    Wisenet::ErrorCode AddGroupMapping(const uuid_string& groupID, const std::vector<Group::MappingItem>& resourceList);
    Wisenet::ErrorCode RemoveGroupMapping(const uuid_string& groupID, const std::vector<std::string>& resourceList);
    Wisenet::ErrorCode UpdateGroupMapping(const uuid_string& from, const uuid_string& to, const std::vector<std::string> &resourceList);

    //Layout
    const std::map<uuid_string, Layout>& GetLayouts(){ return m_layouts; }
    Wisenet::ErrorCode SaveLayout(const Layout& layout);
    Wisenet::ErrorCode RemoveLayouts(const std::vector<uuid_string>& layoutIDs,
                                     std::map<uuid_string, SequenceLayout>& changedSequenceLayouts,
                                     std::map<uuid_string,UserGroup>& changedUserRoles);

    //Sequence
    const std::map<uuid_string, SequenceLayout>& GetSequenceLayouts(){ return m_sequenceLayouts; }
    Wisenet::ErrorCode SaveSequenceLayout(const SequenceLayout& sequenceLayout);
    Wisenet::ErrorCode RemoveSequenceLayouts(const std::vector<uuid_string>& sequenceLayoutIDs);

    //Webpage
    const std::map<uuid_string, Webpage>& GetWebPages(){ return m_webpages; }
    Wisenet::ErrorCode SaveWebpage(const Webpage& webpage);
    Wisenet::ErrorCode RemoveWebpages(const std::vector<uuid_string>& webpageIDs,
                                      std::map<uuid_string,UserGroup>& changedUserRoles,
                                      std::map<uuid_string, Layout>& changedLayouts,
                                      std::map<uuid_string, std::vector<std::string>>& removedGroupChannelMappings);

    //Bookmark
    const std::map<uuid_string, Bookmark>& GetBookmarks(){ return m_bookmarks; }
    Wisenet::ErrorCode SaveBookmark(const Bookmark& bookmark);
    Wisenet::ErrorCode RemoveBookmarks(const std::vector<uuid_string>& bookmarkIDs);

    //EventRule
    const std::map<uuid_string, EventRule>& GetEventRules(){ return m_eventRules; }
    bool IsExistEventRule(std::string& eventRuleID);
    Wisenet::ErrorCode SaveEventRule(EventRule& eventRule);
    Wisenet::ErrorCode RemoveEventRules(const std::vector<uuid_string>& eventRuleIDs);

    //Schedule
    const std::map<uuid_string, EventSchedule>& GetEventSchedules(){ return m_eventSchedules; }
    Wisenet::ErrorCode SaveEventSchedule(EventSchedule& eventSchedule);
    Wisenet::ErrorCode RemoveEventSchedules(const std::vector<uuid_string>& eventScheduleIDs);

    //Email
    std::shared_ptr<EventEmail> GetEventEmail(){ return m_eventEmail; }
    Wisenet::ErrorCode SaveEventEmail(std::shared_ptr<EventEmail> eventEmail);
    Wisenet::ErrorCode RemoveEventEmail();

    //Device
    const std::map<uuid_string, Device::Device>& GetDevices(){ return m_devices; }
    Device::Device& GetDevice(const std::string& deviceID);
    Device::Device::Channel& GetChannel(const std::string& deviceID, const std::string& channelID);
    Wisenet::ErrorCode SaveDevice(const Device::Device& device);
    Wisenet::ErrorCode SaveChannel(const Device::Device::Channel& channel);
    Wisenet::ErrorCode SaveDeviceProfileInfo(const Device::DeviceProfileInfo& deviceProfileInfo);
    Wisenet::ErrorCode SaveDeviceStatus(const Device::DeviceStatus& deviceStatus);

    Wisenet::ErrorCode RemoveDevices(const std::vector<uuid_string>& deviceIDs,
                                     std::map<uuid_string,UserGroup>& changedUserRoles,
                                     std::map<uuid_string, Layout>& changedLayouts,
                                     std::vector<uuid_string>& removedBookmarks);
    Wisenet::ErrorCode SaveDevices(const std::vector<Device::Device>& devices);
    Wisenet::ErrorCode SaveChannels(const std::vector<Device::Device::Channel>& channels,
                                    std::map<uuid_string,UserGroup>& changedUserRoles,
                                    std::map<uuid_string, Layout>& changedLayouts,
                                    std::vector<uuid_string> removedBookmarks);
    void FillDeviceInfoFromDB(Device::Device& retDevice, Device::DeviceProfileInfo& deviceProfileInfo);
    ErrorCode FillAndSaveChannelStatusFromDB(
            const uuid_string& deviceID,
            std::map<std::string, Device::ChannelStatus>& channelsStatus);

    const std::map<uuid_string, Device::DeviceStatus>& GetDevicesStatus(){ return m_devicesStatus; }

    std::map<uuid_string, Device::DeviceProfileInfo>& GetDeviceProfile();
    std::string GetRealProfileNumber(std::string deviceID, std::string channelID, std::string profileID);

    const std::map<uuid_string, std::map<std::string, bool>>& GetDeviceLatestEventStatus(){ return m_deviceLatestEventStatus; }
    void UpdateDeviceLatestEventStatus(const uuid_string& deviceId, const std::string& key, const bool& value);

    //Service
    const GetServiceInformationResponse& GetServiceInformation(){ return m_service; }

    //Cloud
    const GetCloudInformationResponse& GetCloudInformation(){ return m_cloud; }
    Wisenet::ErrorCode SaveCloudInformation(const GetCloudInformationResponse& cloud);

    //Dashboard
    //void GetDashboardData(DashboardData& dashboardData);

    //Settings
    const GetServiceSettingsResponse& GetServiceSettings(){ return m_serviceSettings; }
    int64_t GetLogRetensionTimeMsec(){ return m_logRetensionTimeMsec; }

    Wisenet::ErrorCode SetupService(const std::string &userId, const std::string& newPassword);

    Wisenet::ErrorCode BackupService(const std::string& path, std::string& fileName /*out*/);
    Wisenet::ErrorCode RestoreService(const std::string& path);
    Wisenet::ErrorCode ResetService();

    std::string GetDbPath(){ return m_path.toStdString();}

    // LDAP
    const LdapSetting GetLdapSetting(){ return m_ldapSetting; }
    Wisenet::ErrorCode UpdateLdapSetting(const LdapSetting& ldapSetting);

    // Statistics
    const Statistics GetStatistics(){ return m_statistics; }
    Wisenet::ErrorCode UpdateStatistics(const Statistics& statistics);

    // License
    LicenseInfo& GetLicenseInfo(){ return m_licenseInfo; }
    Wisenet::ErrorCode ActivateLicense(const std::string &licenseKey);

private:
    bool Load();
    void ClearAll();

    void SetLogRetentionTimeMsec();

    std::shared_ptr<ManagementDatabase> m_managementDatabase;

    std::map<uuid_string, UserGroup> m_userGroups;
    std::map<uuid_string, User> m_users;
    std::map<uuid_string, Group> m_groups;
    std::map<std::string, uuid_string> m_channelGroupMappings;
    std::map<uuid_string, Layout> m_layouts;
    std::map<uuid_string, SequenceLayout> m_sequenceLayouts;
    std::map<uuid_string, Webpage> m_webpages;
    std::map<uuid_string, Bookmark> m_bookmarks;
    std::map<uuid_string, EventRule> m_eventRules;
    std::map<uuid_string, EventSchedule> m_eventSchedules;
    std::shared_ptr<EventEmail> m_eventEmail;
    LdapSetting m_ldapSetting;
    Statistics m_statistics;
    LicenseInfo m_licenseInfo;

    std::map<uuid_string, Device::Device> m_devices;
    std::map<uuid_string, Device::DeviceProfileInfo> m_deviceProfileInfos;
    std::map<uuid_string, Device::DeviceStatus> m_devicesStatus;

    std::map<uuid_string, std::map<std::string, bool>> m_deviceLatestEventStatus;

    struct GetServiceInformationResponse m_service;
    struct GetCloudInformationResponse m_cloud;

    struct GetServiceSettingsResponse m_serviceSettings;
    int64_t m_logRetensionTimeMsec;

    CoreServicetype m_serviceType;
    std::string   m_version;
    QString     m_path;
    bool        m_bReset = false;
};

}
}

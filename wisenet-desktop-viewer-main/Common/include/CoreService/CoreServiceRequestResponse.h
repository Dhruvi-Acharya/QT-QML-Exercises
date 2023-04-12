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

#include "CoreService/CoreServiceStructure.h"
#include "DeviceClient/DeviceRequestResponse.h"


namespace Wisenet
{
namespace Core
{

//==========> LogSettings()
struct GetServiceSettingsRequest  : RequestBase
{

};
typedef std::shared_ptr<GetServiceSettingsRequest> GetGetServiceSettingsRequestSharedPtr;

struct GetServiceSettingsResponse : ResponseBase
{
    std::map<std::string, std::string> settings;
};
typedef std::shared_ptr<GetServiceSettingsResponse> GetGetServiceSettingsResponseSharedPtr;

struct SaveServiceSettingsRequest : RequestBase
{
    std::map<std::string, std::string> settings;
};
typedef std::shared_ptr<SaveServiceSettingsRequest> SaveServiceSettingsRequestSharedPtr;

//==========> CloudInformationResponse()
struct GetCloudInformationResponse : ResponseBase
{
    uuid_string cloudID;
    std::string userName;
    std::string password;
};
typedef std::shared_ptr<GetCloudInformationResponse> GetCloudInformationResponseSharedPtr;

//==========> GetServiceInformation()
struct GetServiceInformationRequest  : RequestBase
{

};
typedef std::shared_ptr<GetServiceInformationRequest> GetServiceInformationRequestSharedPtr;

struct GetServiceInformationResponse  : ResponseBase
{
    uuid_string     serviceID;
    std::string     version;
    CoreServicetype serviceType = CoreServicetype::LocalService;
    OsInfo          osInfo;
};
typedef std::shared_ptr<GetServiceInformationResponse > GetServiceInformationResponseSharedPtr;

//==========> SetupService()
struct SetupServiceRequest  : RequestBase
{
    uuid_string userID;
    std::string newPassword;
};
typedef std::shared_ptr<SetupServiceRequest> SetupServiceRequestRequestSharedPtr;

//==========> Login()
struct LoginRequest : RequestBase
{
    bool isHashed = false;
    std::string password;
};
typedef std::shared_ptr<LoginRequest> LoginRequestSharedPtr;

struct LoginResponse : ResponseBase
{
    bool isLdapUser = false;
    int lockedSeconds = 0;
    std::string userId;
};
typedef std::shared_ptr<LoginResponse> LoginResponseSharedPtr;

//==========> Logout()
struct LogoutRequest : RequestBase
{
};
typedef std::shared_ptr<LogoutRequest> LogoutRequestSharedPtr;

//==========> StartEventService()
struct StartEventServiceRequest : RequestBase
{
    EventBaseHandler  eventHandler;
};
typedef std::shared_ptr<StartEventServiceRequest> StartEventServiceRequestSharedPtr;


//==========> SaveUserGroups()
struct SaveUserGroupRequest : RequestBase
{
    UserGroup userGroup;
};
typedef std::shared_ptr<SaveUserGroupRequest> SaveUserGroupRequestSharedPtr;

//==========> RemoveUserGroup()
struct RemoveUserGroupsRequest : RequestBase
{
    std::vector<std::string> userGroupIDs;
};
typedef std::shared_ptr<RemoveUserGroupsRequest> RemoveUserGroupsRequestSharedPtr;

//==========> SaveUser()
struct SaveUserRequest : RequestBase
{
    User user;
};
typedef std::shared_ptr<SaveUserRequest> SaveUserRequestSharedPtr;

//==========> RemoveUsers()
struct RemoveUsersRequest : RequestBase
{
    std::vector<std::string> userIDs;
};
typedef std::shared_ptr<RemoveUsersRequest> RemoveUsersRequestSharedPtr;

//==========> SaveGroup()
struct SaveGroupRequest : RequestBase
{
    Group group;
};
typedef std::shared_ptr<SaveGroupRequest> SaveGroupRequestSharedPtr;

//==========> RemoveGroups()
struct RemoveGroupsRequest : RequestBase
{
    std::vector<uuid_string> groupIDs;
};
typedef std::shared_ptr<RemoveGroupsRequest> RemoveGroupsRequestSharedPtr;

//==========> AddGroupMapping()
struct AddGroupMappingRequest : RequestBase
{
    uuid_string groupID;
    std::vector<Group::MappingItem> channels;
};
typedef std::shared_ptr<AddGroupMappingRequest> AddGroupMappingRequestSharedPtr;

//==========> RemoveGroups()
struct RemoveGroupMappingRequest : RequestBase
{
    uuid_string groupID;
    std::vector<std::string> channelIDs;
};
typedef std::shared_ptr<RemoveGroupMappingRequest> RemoveGroupMappingRequestSharedPtr;

//==========> UpdateGroupMapping()
struct UpdateGroupMappingRequest : RequestBase
{
    uuid_string from;
    uuid_string to;
    std::vector<std::string> channelIDs;
};
typedef std::shared_ptr<UpdateGroupMappingRequest> UpdateGroupMappingRequestSharedPtr;

//==========> AddGroupWithUpdateMapping()
struct AddGroupWithUpdateMappingRequest : RequestBase
{
    Wisenet::Core::Group group;
    std::map<std::string, std::vector<Group::MappingItem>> groupChannelMapping;
};
typedef std::shared_ptr<AddGroupWithUpdateMappingRequest> AddGroupWithUpdateMappingRequestSharedPtr;

//==========> GetEventRule()
struct GetLayoutsRequest : RequestBase
{
};
typedef std::shared_ptr<GetLayoutsRequest> GetLayoutsRequestSharedPtr;

struct GetLayoutsResponse : ResponseBase
{
    std::map<uuid_string, Layout> layouts;
};
typedef std::shared_ptr<GetLayoutsResponse> GetLayoutsResponseSharedPtr;

//==========> SaveLayout()
struct SaveLayoutRequest : RequestBase
{
    Layout layout;
};
typedef std::shared_ptr<SaveLayoutRequest> SaveLayoutRequestSharedPtr;

//==========> RemoveLayouts()
struct RemoveLayoutsRequest : RequestBase
{
    std::vector<uuid_string> layoutIDs;
};
typedef std::shared_ptr<RemoveLayoutsRequest> RemoveLayoutsRequestSharedPtr;

//==========> SaveSequenceLayout()
struct SaveSequenceLayoutRequest : RequestBase
{
    SequenceLayout sequenceLayout;
};
typedef std::shared_ptr<SaveSequenceLayoutRequest> SaveSequenceLayoutRequestSharedPtr;

//==========> RemoveSequenceLayouts()
struct RemoveSequenceLayoutsRequest : RequestBase
{
    std::vector<uuid_string> sequenceLayoutIDs;
};
typedef std::shared_ptr<RemoveSequenceLayoutsRequest> RemoveSequenceLayoutsRequestSharedPtr;

//==========> SaveWebpage()
struct SaveWebpageRequest : RequestBase
{
    Webpage webpage;
};
typedef std::shared_ptr<SaveWebpageRequest> SaveWebpageRequestSharedPtr;

//==========> RemoveWebpages()
struct RemoveWebpagesRequest : RequestBase
{
    std::vector<uuid_string> webpageIDs;
};
typedef std::shared_ptr<RemoveWebpagesRequest> RemoveWebpagesRequestSharedPtr;

//==========> AddDevice()
struct AddDeviceRequest : RequestBase
{
    uuid_string deviceID;
    Device::DeviceConnectInfo connectionInfo;
    Device::DeviceConnectForceInfo connectionForceInfo;
};
typedef std::shared_ptr<AddDeviceRequest> AddDeviceRequestSharedPtr;

//==========> RemoveDevices()
struct RemoveDevicesRequest : RequestBase
{
    std::vector<uuid_string> deviceIDs;
};
typedef std::shared_ptr<RemoveDevicesRequest> RemoveDevicesRequestSharedPtr;

//==========> UpdateDevices()
struct UpdateDevicesRequest : RequestBase
{
    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<UpdateDevicesRequest> UpdateDevicesRequestSharedPtr;

struct UpdateDevicesResponse : ResponseBase
{
    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<UpdateDevicesResponse> UpdateDevicesResponseSharedPtr;

//==========> GetAuditLog()
struct GetAuditLogRequest : RequestBase
{
    int64_t fromTime = 0;
    int64_t toTime = 0;
};
typedef std::shared_ptr<GetAuditLogRequest> GetAuditLogRequestSharedPtr;

struct GetAuditLogResponse : ResponseBase
{
    std::shared_ptr<std::map<uuid_string,DeletedDevice>> deletedDevices;
    std::shared_ptr<std::vector<AuditLog>> auditLogs;
};
typedef std::shared_ptr<GetAuditLogResponse> GetAuditLogResponseSharedPtr;

//==========> SaveBookmark()
struct SaveBookmarkRequest : RequestBase
{
    Bookmark bookmark;
};
typedef std::shared_ptr<SaveBookmarkRequest> SaveBookmarkRequestSharedPtr;

//==========> RemoveBookmarks()
struct RemoveBookmarksRequest : RequestBase
{
    std::vector<uuid_string> bookmarkIDs;
};
typedef std::shared_ptr<RemoveBookmarksRequest> RemoveBookmarksRequestSharedPtr;

//==========> GetEventRule()
struct GetEventRuleRequest : RequestBase
{
};
typedef std::shared_ptr<GetEventRuleRequest> GetEventRuleRequestSharedPtr;

struct GetEventRuleResponse : ResponseBase
{
    std::map<uuid_string, EventRule> eventRules;
};
typedef std::shared_ptr<GetEventRuleResponse> GetEventRuleResponseSharedPtr;

//==========> SaveEventRule()
struct SaveEventRuleRequest : RequestBase
{
    EventRule eventRule;
};
typedef std::shared_ptr<SaveEventRuleRequest> SaveEventRuleRequestSharedPtr;

//==========> RemoveEventRules()
struct RemoveEventRulesRequest : RequestBase
{
    std::vector<uuid_string> eventRuleIDs;
    std::vector<uuid_string> eventScheduleIDs;
};
typedef std::shared_ptr<RemoveEventRulesRequest> RemoveEventRuleRequestSharedPtr;

//==========> GetEventSchedule()
struct GetEventScheduleRequest : RequestBase
{
};
typedef std::shared_ptr<GetEventScheduleRequest> GetEventScheduleRequestSharedPtr;

struct GetEventScheduleResponse : ResponseBase
{
    std::map<uuid_string, EventSchedule> eventSchedules;
};
typedef std::shared_ptr<GetEventScheduleResponse> GetEventScheduleResponseSharedPtr;

//==========> SaveEventSchedule()
struct SaveEventScheduleRequest : RequestBase
{
    EventSchedule eventSchedule;
};
typedef std::shared_ptr<SaveEventScheduleRequest> SaveEventScheduleRequestSharedPtr;

//==========> RemoveEventSchedules()
struct RemoveEventSchedulesRequest : RequestBase
{
    std::vector<uuid_string> eventScheduleIDs;
};
typedef std::shared_ptr<RemoveEventSchedulesRequest> RemoveEventScheduleRequestSharedPtr;

//==========> SaveEventEmail()
struct SaveEventEmailRequest: RequestBase
{
    std::shared_ptr<EventEmail> eventEmail;
};
typedef std::shared_ptr<SaveEventEmailRequest> SaveEventEmailRequestSharedPtr;

//==========> RemoveEventEmail()
struct RemoveEventEmailRequest : RequestBase
{
    EventEmail eventEmail;
};
typedef std::shared_ptr<RemoveEventEmailRequest> RemoveEventEmailRequestSharedPtr;

//==========> GetEventLog()
struct GetEventLogRequest : RequestBase
{
    int64_t fromTime = 0;
    int64_t toTime = 0;

    bool isAllChannel = false;
    struct ChannelID{
        std::string deviceId;
        std::string channelId;
    };

    std::vector<ChannelID> channelIds;

    bool isAllType = false;
    std::vector<std::string> types;
};
typedef std::shared_ptr<GetEventLogRequest> GetEventLogRequestSharedPtr;

struct GetEventLogResponse : ResponseBase
{
    std::shared_ptr<std::map<uuid_string,DeletedDevice>> deletedDevices;
    std::shared_ptr<std::vector<EventLog>> eventLogs;
};
typedef std::shared_ptr<GetEventLogResponse> GetEventLogResponseSharedPtr;

//==========> GetSystemLog()
struct GetSystemLogRequest : RequestBase
{
    int64_t fromTime = 0;
    int64_t toTime = 0;

    bool containService = false;

    bool isAllDevice = false;
    std::vector<uuid_string> deviceIds;

    bool isAllType = false;
    std::vector<std::string> types;
};
typedef std::shared_ptr<GetSystemLogRequest> GetSystemLogRequestSharedPtr;

struct GetSystemLogResponse : ResponseBase
{
    std::shared_ptr<std::map<uuid_string,DeletedDevice>> deletedDevices;
    std::shared_ptr<std::vector<EventLog>> eventLogs;
};
typedef std::shared_ptr<GetSystemLogResponse> GetSystemLogResponseSharedPtr;

//==========> AddLog()
struct AddLogRequest : RequestBase
{
    std::vector<EventLog> eventLogs;
};
typedef std::shared_ptr<AddLogRequest> AddLogRequestSharedPtr;
//

//==========> SaveChannel()
struct SaveChannelsRequest : RequestBase
{
     std::vector<Device::Device::Channel> channels;
};
typedef std::shared_ptr<SaveChannelsRequest> SaveChannelsRequestSharedPtr;

//==========> SaveDevicesCredential()
struct SaveDevicesCredentialRequest : RequestBase
{
    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<SaveDevicesCredentialRequest> SaveDevicesCredentialRequestSharedPtr;

struct SaveDevicesCredentialResponse : ResponseBase
{
    std::vector<Device::Device> devices;
};
typedef std::shared_ptr<SaveDevicesCredentialResponse> SaveDevicesCredentialResponseSharedPtr;

//==========> DiscoverDevices()
struct DiscoverDevicesRequest : RequestBase
{
    int64_t fromTime = 0;
    int64_t toTime = 0;
};
typedef std::shared_ptr<DiscoverDevicesRequest> DiscoverDevicesRequestSharedPtr;

struct DiscoverDevicesResponse : ResponseBase
{
    std::vector<DiscoveryDeviceInfo> discoveredDevices;
};
typedef std::shared_ptr<DiscoverDevicesResponse> DiscoverDevicesResponseSharedPtr;

//==========> IpSettingDevice()
struct IpSettingDeviceRequest : RequestBase
{
    IpSettingInfo ipSetting;
};
typedef std::shared_ptr<IpSettingDeviceRequest> IpSettingDeviceRequestSharedPtr;

struct IpSettingDeviceResponse : ResponseBase
{
    IpSettingInfo ipInstall;
};
typedef std::shared_ptr<IpSettingDeviceResponse> IpSettingDeviceResponseSharedPtr;


//==========> DeviceInitPassword()
struct InitDevicePasswordRequest : RequestBase
{
    std::string macAddress;
    std::string password;
};
typedef std::shared_ptr<InitDevicePasswordRequest> InitDevicePasswordRequestSharedPtr;

//==========> GetDeviceProfile()
struct GetDeviceProfileRequest : RequestBase
{
    uuid_string deviceID;
};
typedef std::shared_ptr<GetDeviceProfileRequest> GetDeviceProfileRequestSharedPtr;

struct GetDeviceProfileResponse : ResponseBase
{
    Device::DeviceProfileInfo profileInfo;
};
typedef std::shared_ptr<GetDeviceProfileResponse> GetDeviceProfileResponseSharedPtr;

//==========> BackupService()
struct BackupServiceRequest : RequestBase
{
    std::string backupFilePath;
};
typedef std::shared_ptr<BackupServiceRequest> BackupServiceRequestSharedPtr;

struct BackupServiceResponse : ResponseBase
{
    std::string fileName;
};
typedef std::shared_ptr<BackupServiceResponse> BackupServiceResponseSharedPtr;

//==========> RestoreService()
struct RestoreServiceRequest : RequestBase
{
    std::string restoreFilePath;
};
typedef std::shared_ptr<RestoreServiceRequest> RestoreServiceRequestSharedPtr;

//==========> ResetService()
struct ResetServiceRequest : RequestBase
{
};
typedef std::shared_ptr<ResetServiceRequest> ResetServiceRequestSharedPtr;

//==========> UpdateSoftwareService()
struct UpdateSoftwareServiceRequest : RequestBase
{
};
typedef std::shared_ptr<UpdateSoftwareServiceRequest> UpdateSoftwareServiceRequestSharedPtr;


//==========> GetDashboardData()
struct GetDashboardDataRequest : RequestBase
{
};
typedef std::shared_ptr<GetDashboardDataRequest> GetDashboardDataRequestSharedPtr;

struct GetDashboardDataResponse : ResponseBase
{
    std::map<int, DashboardData> dashboardData;
};
typedef std::shared_ptr<GetDashboardDataResponse> GetDashboardDataResponseSharedPtr;

//==========> UpdateLdapSetting()
struct UpdateLdapSettingRequest : RequestBase
{
    LdapSetting ldapSetting;
};
typedef std::shared_ptr<UpdateLdapSettingRequest> UpdateLdapSettingRequestSharedPtr;


struct LdapConnectRequest : RequestBase
{
    LdapSetting ldapSetting;
};
typedef std::shared_ptr<LdapConnectRequest> LdapConnectRequestSharedPtr;

struct LdapConnectResponse : ResponseBase
{
    LdapConnectionResult connResult;
    std::vector<LdapUserInfo> fetchedUser;
};
typedef std::shared_ptr<LdapConnectResponse> LdapConnectResponseSharedPtr;

struct UpdateStatisticsRequest : RequestBase
{
    Statistics statistics;
};
typedef std::shared_ptr<UpdateStatisticsRequest> UpdateStatisticsRequestSharedPtr;

struct ActivateLicenseRequest : RequestBase
{
    std::string licenseKey;
};
typedef std::shared_ptr<ActivateLicenseRequest> ActivateLicenseRequestSharedPtr;

}
}

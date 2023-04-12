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

#include <string>
#include <memory>
#include <unordered_map>
#include <boost/asio.hpp>
#include "CoreService/ICoreService.h"
#include "DeviceClient/IDeviceClient.h"
#include "MediaSession.h"

namespace Wisenet
{
namespace Core
{

class DatabaseManager;
class LogManager;

class UserSession : public std::enable_shared_from_this<UserSession>
{
public:
    UserSession(DatabaseManager& dataBase,
                LogManager& logManager,
                std::string userName="");
    ~UserSession();

    void LogOut();

    /*
     * Check Administrator Authority
     * 1. Settings of UserRole & User
     * 2. Add/Remove Device
     * 3. Settings of Webpages
     */
    bool CheckAdminAuthority();

    /* Dispatch and filter event for user */
    void SetUserEventHandler(EventBaseHandler userEvtHandler);
    void ProcessEvent(const EventBaseSharedPtr& event);
    void ProcessFullInfoEvent();

    /* Execute command for user */
    ResponseBaseSharedPtr SaveUserGroup(SaveUserGroupRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveUserGroups(RemoveUserGroupsRequestSharedPtr const& request,
                                         std::vector<uuid_string>& removedUserIDs,
                                         std::vector<uuid_string>& removedLayoutIDs,
                                         std::vector<uuid_string>& removedSequenceIDs);
    ResponseBaseSharedPtr SaveUser(SaveUserRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveUsers(RemoveUsersRequestSharedPtr const& request,
                                      std::vector<uuid_string> &removedLayoutIDs,
                                      std::vector<uuid_string> &removedSequenceIDs);

    ResponseBaseSharedPtr SaveGroup(SaveGroupRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveGroups(RemoveGroupsRequestSharedPtr const& request, std::vector<uuid_string>& removedGroups, std::map<uuid_string, std::vector<std::string>>& removedMapping);

    ResponseBaseSharedPtr AddGroupMapping(AddGroupMappingRequestSharedPtr const&request);
    ResponseBaseSharedPtr RemoveGroupMapping(RemoveGroupMappingRequestSharedPtr const&request);
    ResponseBaseSharedPtr UpdateGroupMapping(UpdateGroupMappingRequestSharedPtr const&request);
    ResponseBaseSharedPtr AddGroupWithUpdateMapping(AddGroupWithUpdateMappingRequestSharedPtr const&request);

    ResponseBaseSharedPtr GetLayouts();
    ResponseBaseSharedPtr SaveLayout(SaveLayoutRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveLayouts(RemoveLayoutsRequestSharedPtr const& request,
                                        std::map<uuid_string, SequenceLayout>& changedSequenceLayouts,
                                        std::map<uuid_string,UserGroup>& changedUserRoles);
    ResponseBaseSharedPtr SaveSequenceLayout(SaveSequenceLayoutRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveSequenceLayouts(RemoveSequenceLayoutsRequestSharedPtr const& request);
    ResponseBaseSharedPtr SaveWebpage(SaveWebpageRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveWebpages(RemoveWebpagesRequestSharedPtr const& request,
                                         std::map<uuid_string,UserGroup>& changedUserRoles,
                                         std::map<uuid_string, Layout>& changedLayouts,
                                         std::map<uuid_string, std::vector<std::string>>& removedGroupChannelMappings);
    ResponseBaseSharedPtr SaveBookmark(SaveBookmarkRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveBookmarks(RemoveBookmarksRequestSharedPtr const& request);
    ResponseBaseSharedPtr SaveChannels(const std::vector<Device::Device::Channel>& channels,
                                       std::map<uuid_string,UserGroup>& changedUserRoles,
                                       std::map<uuid_string, Layout>& changedLayouts,
                                       std::vector<uuid_string> removedBookmarks);
    ResponseBaseSharedPtr SaveDevicesCredential(SaveDevicesCredentialRequestSharedPtr const& request);
    ResponseBaseSharedPtr SaveDevices(UpdateDevicesRequestSharedPtr const&request);

    ResponseBaseSharedPtr BackupService(BackupServiceRequestSharedPtr const& request);
    ResponseBaseSharedPtr RestoreService(RestoreServiceRequestSharedPtr const& request);
    ResponseBaseSharedPtr ResetService(ResetServiceRequestSharedPtr const& request);


    void GetAuditLog(GetAuditLogRequestSharedPtr const& request,
                     ResponseBaseHandler const&responseHandler);

    void GetEventLog(GetEventLogRequestSharedPtr const& request,
                     ResponseBaseHandler const&responseHandler);

    void GetSystemLog(GetSystemLogRequestSharedPtr const& request,
                      ResponseBaseHandler const& responseHandler);

    ResponseBaseSharedPtr GetEventRule();
    ResponseBaseSharedPtr SaveEventRule(SaveEventRuleRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveEventRules(RemoveEventRuleRequestSharedPtr const& request);

    ResponseBaseSharedPtr GetEventSchedule();
    ResponseBaseSharedPtr SaveEventSchedule(SaveEventScheduleRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveEventSchedules(RemoveEventScheduleRequestSharedPtr const& request);

    ResponseBaseSharedPtr SaveEventEmail(SaveEventEmailRequestSharedPtr const& request);
    ResponseBaseSharedPtr RemoveEventEmail(RemoveEventEmailRequestSharedPtr const& request);

    ResponseBaseSharedPtr UpdateSoftwareService(UpdateSoftwareServiceRequestSharedPtr const&request);

    ResponseBaseSharedPtr UpdateLdapSetting(UpdateLdapSettingRequestSharedPtr const& request);
    ResponseBaseSharedPtr UpdateStatistics(UpdateStatisticsRequestSharedPtr const& request);

    LicenseInfo& GetLicenseInfo();
    ResponseBaseSharedPtr ActivateLicense(ActivateLicenseRequestSharedPtr const& request);

    std::map<uuid_string, Device::DeviceProfileInfo>&  GetDeviceProfile();

    /* Try to connect to device and then insert DB */
    ErrorCode AddDevice(const Device::DeviceConnectResponseSharedPtr& connectResponse);
    ErrorCode RemoveDevices(const std::vector<uuid_string>& deviceIDs,
                            std::map<uuid_string,UserGroup>& changedUserRoles,
                            std::map<uuid_string, Layout>& changedLayouts,
                            std::vector<uuid_string>& removedBookmarks);

    Device::Device& GetDevice(const std::string& deviceID);
    ErrorCode ConnectDevice(const Device::DeviceConnectResponseSharedPtr& connectResponse);

    /* MediaSession */
    MediaSessionSharedPtr CreateMediaSession(const std::string& mediaID);
    MediaSessionSharedPtr GetMediaSession(const std::string& mediaID);
    void DeleteMediaSession(const std::string& mediaID);

    const AuditLog& GetAuditLog(){ return m_auditLog; };
private:
    boost::optional<User> GetUserInfo() const;

private:
    // CoreService instance
    DatabaseManager&        m_db;
    LogManager&             m_logManager;

    std::string             m_loginID;
    boost::optional<User>   m_userInfo;
    EventBaseHandler        m_userEventHandler = nullptr; // service --> user event

    AuditLog                m_auditLog;
    AuditLogLogInActionDetail m_detail;
    std::map<std::string, MediaSessionSharedPtr> m_mediaSessions;

    bool                    m_bLogOut;
};

}
}

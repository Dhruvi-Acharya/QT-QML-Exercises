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
#include "UserSession.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <sstream>
#include "CoreServiceLogSettings.h"
#include "CoreService.h"
#include "CoreServiceUtil.h"
#include "WeakCallback.h"
#include "DatabaseManager.h"
#include "LogManager.h"
#include "TimeUtil.h"

namespace Wisenet
{
namespace Core
{

UserSession::UserSession(DatabaseManager& dataBase,
                         LogManager& logManager,
                         std::string userName)
    : m_db(dataBase)
    , m_logManager(logManager)
    , m_loginID(userName)
    , m_auditLog()
    , m_bLogOut(false)
{
    SPDLOG_DEBUG("New UserSession, userName={}", userName);
    m_userInfo = GetUserInfo();

    if(!m_loginID.empty()) {
        //정상적으로 로그인 된 경우이므로 Audit Log 를 생성한다.
        m_auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        m_auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        m_auditLog.userName = userName;
        m_auditLog.host = "127.0.0.1";
        m_auditLog.actionType = "User Login";
        m_auditLog.itemType = AuditLog::ItemType::Service;

        m_auditLog.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

        m_detail.isSuccess = true;
        m_detail.startUtcTimeMsec = m_auditLog.serviceUtcTimeMsec;

        JsonWriter writer;
        writer & m_detail;

        m_auditLog.actionDetail = writer.GetString();

        m_logManager.SaveAuditLogInMemory(m_auditLog);
    }
}

UserSession::~UserSession()
{
    m_userEventHandler = nullptr;

    if(!m_bLogOut && !m_auditLog.auditLogID.empty()) {
        //정상적으로 로그인 되었다가 로그아웃되는 경우이므로 Audit Log를 생성한다.
        m_detail.endUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

        JsonWriter writer;
        writer & m_detail;

        m_auditLog.actionDetail = writer.GetString();

        m_logManager.SaveAuditLogInMemory(m_auditLog);
    }
}

void UserSession::LogOut()
{
    m_bLogOut = true;

    if (!m_auditLog.auditLogID.empty()) {
        //정상적으로 로그인 되었다가 로그아웃되는 경우이므로 Audit Log를 생성한다.

        m_detail.endUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

        JsonWriter writer;
        writer & m_detail;

        m_auditLog.actionDetail = writer.GetString();

        m_logManager.SaveAuditLogInMemory(m_auditLog);
    }
}

bool UserSession::CheckAdminAuthority()
{
    /* TODO */
    return true;
}

void UserSession::SetUserEventHandler(EventBaseHandler userEvtHandler)
{
    m_userEventHandler = userEvtHandler;
}

boost::optional<User> UserSession::GetUserInfo() const
{
    boost::optional<User> user;

    auto &users = m_db.GetUsers();

    for(auto &item : users){
        if(item.second.loginID == m_loginID)
            user = item.second;
    }

    if (!user) {
        SPDLOG_ERROR("Can not find user info in DB, user={}", m_loginID);
    }
    return user;
}

void UserSession::ProcessEvent(const EventBaseSharedPtr &event)
{
    // Audit Log
    if (event->EventTypeId() == Device::DeviceEventType::DeviceStatusEventType) {
        SPDLOG_DEBUG("UserSession::ProcessEvent - DeviceStatusEvnet : Start");
        Device::DeviceStatusEventSharedPtr deviceStatusEvent =
                std::static_pointer_cast<Device::DeviceStatusEvent>(event);

        if(Device::DeviceStatusType::Connected == deviceStatusEvent->deviceStatus.status){
            m_logManager.AddDeviceStatusAuditLog(m_auditLog.sessionID,"Service",m_auditLog.host,AuditLogOperationType::Connect,deviceStatusEvent->deviceStatus.deviceID);
        }else if(Device::DeviceStatusType::ConnectedAndInfoChanged != deviceStatusEvent->deviceStatus.status){
            m_logManager.AddDeviceStatusAuditLog(m_auditLog.sessionID,"Service",m_auditLog.host,AuditLogOperationType::Disconnect,deviceStatusEvent->deviceStatus.deviceID);
        }
    }
    else if(event->EventTypeId() == Device::DeviceEventType::DeviceLatestStatusEventType)
    {
        Device::DeviceLatestStatusEventSharedPtr deviceLatestStatusEvent =
                std::static_pointer_cast<Device::DeviceLatestStatusEvent>(event);

        //qDebug() << "DeviceLatestStatusEventType" << QString::fromStdString(deviceLatestStatusEvent->deviceID) << QString::fromStdString(deviceLatestStatusEvent->eventKey) << deviceLatestStatusEvent->status;
        m_db.UpdateDeviceLatestEventStatus(deviceLatestStatusEvent->deviceID, deviceLatestStatusEvent->eventKey, deviceLatestStatusEvent->status);
    }

    if (!m_userEventHandler)
        return;

    if (!m_userInfo) {
        return;
    }

    const auto& info = m_userInfo.value();

    if (info.userType == User::UserType::Custom) {
        /* do filtering */
    }

    m_userEventHandler(event);
}


void UserSession::ProcessFullInfoEvent()
{
    if (!m_userEventHandler)
        return;

    SPDLOG_DEBUG("Start FullInfoEvent for user={}", m_db.GetUsers().size());

    auto &userRoles = m_db.GetUserGroups();
    auto &users = m_db.GetUsers();
    auto &groups = m_db.GetGroups();
    auto &channelGroupMappings = m_db.GetChannelGroupMappings();
    auto &layouts = m_db.GetLayouts();
    auto &sequenceLayouts = m_db.GetSequenceLayouts();
    auto &webpages = m_db.GetWebPages();
    auto &devices = m_db.GetDevices();
    auto &devicesStatus = m_db.GetDevicesStatus();
    auto &bookmarks = m_db.GetBookmarks();
    auto &eventRules = m_db.GetEventRules();
    auto &eventSchedules = m_db.GetEventSchedules();
    auto eventEmail = m_db.GetEventEmail();
    auto &deviceLatestEventStatus = m_db.GetDeviceLatestEventStatus();
    auto ldapSetting = m_db.GetLdapSetting();
    auto statistics = m_db.GetStatistics();
    auto licenseInfo = m_db.GetLicenseInfo();

    //const auto& info = GetUserInfo();
    if (!m_userInfo) {
        return;
    }
    const auto& info = m_userInfo.value();

    FullInfoEventSharedPtr event = std::make_shared<FullInfoEvent>();
    // userRoles
    // user role based resource :: layouts, webpages, devices, devicesStatus, bookmarks
    if (info.userType == User::UserType::Owner || info.userType == User::UserType::Admin) {
        event->userRoles = userRoles; // all user roles
        event->users = users;
        event->groups = groups; // all groups
        event->channelGroupMappings = channelGroupMappings; // channel-group mappings
        event->webpages = webpages; // all webpages
        event->devices = devices; // all devices
        event->devicesStatus = devicesStatus;
        event->bookmarks = bookmarks;
        event->eventRules = eventRules;
        event->eventSchedules = eventSchedules;
        event->eventEmail = eventEmail;
        event->ldapSetting = ldapSetting;
        //event->sequenceLayouts = sequenceLayouts;

        event->deviceLatestEventStatus = deviceLatestEventStatus;

        // my layout
        for (auto& layout : layouts) {
            if (layout.second.userName == info.loginID) {
                event->layouts.emplace(layout);
            }
        }

        // shared layout
        auto usergroupItr = userRoles.find(info.userGroupID);
        if (usergroupItr != userRoles.end()) {
            auto& adminGroup = usergroupItr->second;

            if(adminGroup.assignedResource)
            {
                auto& resource = adminGroup.assignedResource.value();

                for (auto& layoutID : resource.layouts) {
                    auto itr = layouts.find(layoutID);
                    if (itr != layouts.end()) {
                        event->layouts.emplace(*itr);
                    }
                }
            }
        }
    }
    else {
        auto itr = userRoles.find(info.userGroupID);
        if (itr != userRoles.end()) {
            event->userRoles.emplace(*itr);
            auto& userRole = itr->second; // my user role

            event->groups = groups;

            SPDLOG_DEBUG("Start FullInfoEvent for userRole.accessAllResources={}", userRole.accessAllResources);

            if(userRole.assignedResource)
            {
                auto& resource = userRole.assignedResource.value();

                // accessible all resources
                if (userRole.accessAllResources) {
                    event->groups = groups; // all groups
                    event->channelGroupMappings = channelGroupMappings; // channel-group mappings
                    event->webpages = webpages;
                    event->devices = devices;
                    event->devicesStatus = devicesStatus;
                    event->bookmarks = bookmarks;

                    // my layouts
                    for (auto& layout : layouts) {
                        if (info.loginID == layout.second.userName)
                            event->layouts.emplace(layout);
                    }

                    // allowed layouts
                    for (auto& layoutID : resource.layouts) {
                        auto itr = layouts.find(layoutID);
                        if (itr != layouts.end()) {
                            event->layouts.emplace(*itr);
                        }
                    }
                }
                // specific resources
                else {
                    // webpages
                    for(auto& webpageID : resource.webpages) {
                        auto itr = webpages.find(webpageID);
                        if (itr != webpages.end()) {
                            event->webpages.emplace(*itr);
                        }
                    }
                    // my layouts
                    for (auto& layout : layouts) {
                        if (info.loginID == layout.second.userName)
                            event->layouts.emplace(layout);
                    }
                    // allowed layouts
                    for (auto& layoutID : resource.layouts) {
                        auto itr = layouts.find(layoutID);
                        if (itr != layouts.end()) {
                            event->layouts.emplace(*itr);
                        }
                    }

                    // devices
                    for(auto& device : resource.devices) {
                        auto ditr = devices.find(device.first);
                        auto dsitr = devicesStatus.find(device.first);
                        if (ditr == devices.end() || dsitr == devicesStatus.end())
                            continue;

                        const Device::Device& sourceDevice = ditr->second;
                        const Device::DeviceStatus& sourceDeviceStatus = dsitr->second;

                        // copy to target
                        Device::Device targetDevice = sourceDevice;
                        Device::DeviceStatus targetDeviceStatus = sourceDeviceStatus;

                        // channel clear and assign
                        targetDevice.channels.clear();
                        targetDeviceStatus.channelsStatus.clear();

                        // find channels to assign
                        for(auto& channel : device.second.channels) {
                            auto itr = sourceDevice.channels.find(channel);
                            if (itr != sourceDevice.channels.end()) {
                                targetDevice.channels.emplace(*itr);
                            }
                            auto sitr = sourceDeviceStatus.channelsStatus.find(channel);
                            if (sitr != sourceDeviceStatus.channelsStatus.end()) {
                                targetDeviceStatus.channelsStatus.emplace(*sitr);
                            }
                        }

                        // final target assign
                        event->devices.emplace(targetDevice.deviceID, targetDevice);
                        event->devicesStatus.emplace(targetDeviceStatus.deviceID, targetDeviceStatus);
                    }
                }
            }
            // check bookmark view role
            if(userRole.userPermission.playback)
            {
                for(auto& bookmark : bookmarks)
                {
                    uuid_string bookmarkDeviceId = bookmark.second.deviceID;
                    std::string bookmarkChannelId = bookmark.second.channelID;

                    //SPDLOG_DEBUG("UserSession::ProcessFullInfoEvent() bookmark device={} channel={}", bookmarkDeviceId, bookmarkChannelId);

                    if(event->devices.find(bookmarkDeviceId) != event->devices.end())
                    {
                        auto bookmarkDevice = event->devices[bookmarkDeviceId];

                        if(bookmarkDevice.channels.find(bookmarkChannelId) != bookmarkDevice.channels.end())
                        {
                            //SPDLOG_DEBUG("UserSession::ProcessFullInfoEvent() bookmark device={} channel={} added", bookmarkDeviceId, bookmarkChannelId);
                            event->bookmarks.emplace(bookmark);
                        }
                    }
                }
            }

            // mapping
            for(auto& channelGroupMapping : channelGroupMappings)
            {
                auto itemId = channelGroupMapping.first;

                std::vector<std::string> tokens;
                boost::split(tokens, itemId, boost::is_any_of("_"));

                if(tokens.size() == 2)
                {
                    uuid_string deviceId = tokens[0];
                    std::string channelId = tokens[1];
                    //SPDLOG_DEBUG("UserSession::ProcessFullInfoEvent() mapping device={} channel={}", deviceId, channelId);

                    if(event->devices.find(deviceId) != event->devices.end())
                    {
                        auto mappingDevice = event->devices[deviceId];

                        if(mappingDevice.channels.find(channelId) != mappingDevice.channels.end())
                        {
                            SPDLOG_DEBUG("UserSession::ProcessFullInfoEvent() mapping device={} channel={} added", deviceId, channelId);
                            event->channelGroupMappings.emplace(channelGroupMapping);
                        }
                    }
                }
            }

        }

        auto userItr = users.find(info.userID);
        if (userItr != users.end())
        {
            event->users.emplace(*userItr);
        }

        for(auto& user : event->users){
            auto roleItr = userRoles.find(user.second.userGroupID);
            if(roleItr != userRoles.end())
            {
                event->userRoles.emplace(*roleItr);
            }
        }
    }

    // sequence layouts
    for (auto& sequenceLayout : sequenceLayouts) {
        if (info.loginID == sequenceLayout.second.userName)
            event->sequenceLayouts.emplace(sequenceLayout);
    }

    event->statistics = statistics;
    event->licenseInfo = licenseInfo;

    m_userEventHandler(event);
}


ResponseBaseSharedPtr UserSession::SaveUserGroup(
        const SaveUserGroupRequestSharedPtr &request)
{
    bool isExist = m_db.IsExistUserGroup(request->userGroup.userGroupID);

    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveUserGroup(request->userGroup);

    if(res->isSuccess()) {
        // User가 추가되거나 변경되었을 때 AuditLog 생성.
        auto operationType = AuditLogOperationType::Added;
        if(isExist) {
            operationType = AuditLogOperationType::Updated;
        }

        m_logManager.AddUserGroupAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,operationType,request->userGroup.name);
    }

    return res;
}

ResponseBaseSharedPtr UserSession::RemoveUserGroups(
        const RemoveUserGroupsRequestSharedPtr &request,
        std::vector<uuid_string>& removedUserIDs,
        std::vector<uuid_string>& removedLayoutIDs,
        std::vector<uuid_string>& removedSequenceIDs)
{
    std::vector<std::string> removedUserGroupNames;

    auto& userGroups = m_db.GetUserGroups();

    for(auto& userGroupID : request->userGroupIDs){
        auto itor = userGroups.find(userGroupID);
        if(itor != userGroups.end()){
            removedUserGroupNames.emplace_back(itor->second.name);
        }
    }

    std::map<uuid_string, User> users = m_db.GetUsers();

    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveUserGroups(request->userGroupIDs,
                                           removedUserIDs,
                                           removedLayoutIDs,
                                           removedSequenceIDs);

    if(res->isSuccess()) {
        // UserGroup 삭제시 AuditLog 생성.
        for(auto& removedUserGroupName : removedUserGroupNames){
            auto operationType = AuditLogOperationType::Removed;
            m_logManager.AddUserGroupAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,operationType,removedUserGroupName);
        }

        // User 삭제시 AuditLog 생성.
        for(auto& removedUserID : removedUserIDs){
            auto itor = users.find(removedUserID);
            if(itor != users.end()){
                m_logManager.AddUserAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Removed, itor->second.loginID);
            }
        }

    }

    return res;
}

ResponseBaseSharedPtr UserSession::SaveUser(const SaveUserRequestSharedPtr &request)
{
    bool isExist = m_db.IsExistUser(request->user.loginID);

    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveUser(request->user);

    if(res->isSuccess()) {
        // User가 추가되거나 변경되었을 때 AuditLog 생성.
        auto operationType = AuditLogOperationType::Added;
        if(isExist) {
            operationType = AuditLogOperationType::Updated;
        }
        m_logManager.AddUserAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,operationType,request->user.loginID);
    }

    return res;
}

ResponseBaseSharedPtr UserSession::RemoveUsers(const RemoveUsersRequestSharedPtr &request,
                                               std::vector<uuid_string>& removedLayoutIDs,
                                               std::vector<uuid_string>& removedSequenceIDs)
{
    auto res = std::make_shared<ResponseBase>();

    std::map<uuid_string, User> users = m_db.GetUsers();

    for(auto item : request->userIDs){
        SPDLOG_DEBUG("UserSession::RemoveUsers {}", item);
    }

    res->errorCode = m_db.RemoveUsers(request->userIDs,
                                      removedLayoutIDs,
                                      removedSequenceIDs);


    if(res->isSuccess()) {
        // User가 삭제되는 경우 AuditLog 생성.
        SPDLOG_DEBUG("UserSession::RemoveUsers : Success user num:{}",users.size());

        for(auto& userID : request->userIDs)
        {
            auto itor = users.find(userID);
            if(itor != users.end()){
                m_logManager.AddUserAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Removed, itor->second.loginID);
            }else{
                SPDLOG_DEBUG("UserSession::RemoveUsers : Not found userID:{}",userID);
            }
        }
    }
    return res;
}

ResponseBaseSharedPtr UserSession::SaveGroup(const SaveGroupRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveGroup(request->group);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveGroups(const RemoveGroupsRequestSharedPtr &request, std::vector<uuid_string>& removedGroups, std::map<uuid_string, std::vector<std::string>>& removedMapping)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveGroups(request->groupIDs, removedGroups, removedMapping);
    return res;
}

ResponseBaseSharedPtr UserSession::AddGroupMapping(const AddGroupMappingRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.AddGroupMapping(request->groupID, request->channels);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveGroupMapping(const RemoveGroupMappingRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveGroupMapping(request->groupID, request->channelIDs);
    return res;
}

ResponseBaseSharedPtr UserSession::UpdateGroupMapping(const UpdateGroupMappingRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.UpdateGroupMapping(request->from, request->to, request->channelIDs);
    return res;
}

ResponseBaseSharedPtr UserSession::AddGroupWithUpdateMapping(const AddGroupWithUpdateMappingRequestSharedPtr &request)
{
    QUuid emptyUuid;
    std::string emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces).toUtf8().constData();

    auto res = std::make_shared<ResponseBase>();

    res->errorCode = m_db.SaveGroup(request->group);

    if(res->errorCode != Wisenet::ErrorCode::NoError)
        return res;

    for (auto& kvp : request->groupChannelMapping) {

        if(emptyUuidString == kvp.first)
            m_db.AddGroupMapping(request->group.groupID, kvp.second);
        else
        {
            std::vector<std::string> idList;
            for(auto& item : kvp.second)
            {
                idList.push_back(item.id);
            }

            m_db.UpdateGroupMapping(kvp.first, request->group.groupID, idList);
        }
    }

    return res;
}

ResponseBaseSharedPtr UserSession::GetLayouts()
{
    auto res = std::make_shared<GetLayoutsResponse>();
    res->layouts = m_db.GetLayouts();
    return res;
}

ResponseBaseSharedPtr UserSession::SaveLayout(const SaveLayoutRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveLayout(request->layout);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveLayouts(const RemoveLayoutsRequestSharedPtr &request,
                                                 std::map<uuid_string, SequenceLayout>& changedSequenceLayouts,
                                                 std::map<uuid_string,UserGroup>& changedUserRoles)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveLayouts(request->layoutIDs,
                                        changedSequenceLayouts,
                                        changedUserRoles);
    return res;
}

ResponseBaseSharedPtr UserSession::SaveSequenceLayout(const SaveSequenceLayoutRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveSequenceLayout(request->sequenceLayout);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveSequenceLayouts(const RemoveSequenceLayoutsRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveSequenceLayouts(request->sequenceLayoutIDs);
    return res;
}

ResponseBaseSharedPtr UserSession::SaveWebpage(const SaveWebpageRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveWebpage(request->webpage);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveWebpages(const RemoveWebpagesRequestSharedPtr &request,
                                                  std::map<uuid_string,UserGroup>& changedUserRoles,
                                                  std::map<uuid_string, Layout>& changedLayouts,
                                                  std::map<uuid_string, std::vector<std::string>>& removedGroupChannelMappings)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveWebpages(request->webpageIDs,
                                         changedUserRoles,
                                         changedLayouts,
                                         removedGroupChannelMappings);
    return res;
}

ResponseBaseSharedPtr UserSession::SaveBookmark(SaveBookmarkRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveBookmark(request->bookmark);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveBookmarks(RemoveBookmarksRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveBookmarks(request->bookmarkIDs);
    return res;
}

ResponseBaseSharedPtr UserSession::SaveDevicesCredential(const SaveDevicesCredentialRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveDevices(request->devices);

    if(res->isSuccess()){

        for(auto& device : request->devices){            
            m_logManager.AddDeviceAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Updated,device.deviceID);
        }

    }

    return res;
}

ResponseBaseSharedPtr UserSession::SaveDevices(const UpdateDevicesRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveDevices(request->devices);

    if(res->isSuccess()){

        for(auto& device : request->devices){            
            m_logManager.AddDeviceAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Updated,device.deviceID);
        }

    }
    return res;
}

ResponseBaseSharedPtr UserSession::SaveChannels(const std::vector<Device::Device::Channel>& channels,
                                                std::map<uuid_string,UserGroup>& changedUserRoles,
                                                std::map<uuid_string, Layout>& changedLayouts,
                                                std::vector<uuid_string> removedBookmarks)
{
    SPDLOG_INFO("UserSession::SaveChannels");
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveChannels(channels,
                                       changedUserRoles,
                                       changedLayouts,
                                       removedBookmarks);

    if(res->isSuccess()){

        for(auto& channel : channels) {
            SPDLOG_INFO("UserSession::SaveChannels - AddChannelAuditLog");
            m_logManager.AddChannelAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Updated,channel.deviceID,channel.channelID);
        }

    }
    return res;
}

void UserSession::GetAuditLog(GetAuditLogRequestSharedPtr const&request, ResponseBaseHandler const&responseHandler)
{
    m_logManager.GetAuditLog(request, responseHandler);
}

ResponseBaseSharedPtr UserSession::BackupService(const BackupServiceRequestSharedPtr &request)
{
    auto res = std::make_shared<BackupServiceResponse>();
    SPDLOG_DEBUG("UserSession::BackupService:{} ",request->backupFilePath);
    res->errorCode = m_db.BackupService(request->backupFilePath, res->fileName);

    if(ErrorCode::NoError == res->errorCode){
        m_logManager.AddBackupSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,true);
    }else{
        m_logManager.AddBackupSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,false);
    }

    return res;
}

ResponseBaseSharedPtr UserSession::RestoreService(const RestoreServiceRequestSharedPtr &request)
{
    auto res = std::make_shared<ResponseBase>();
    SPDLOG_DEBUG("UserSession::RestoreService {}", request->restoreFilePath);
    res->errorCode = m_db.RestoreService(request->restoreFilePath);

    if(ErrorCode::NoError == res->errorCode){
        m_logManager.AddRestoreSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,true);
    }else{
        m_logManager.AddRestoreSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,false);
    }

    return res;
}

ResponseBaseSharedPtr UserSession::ResetService(const ResetServiceRequestSharedPtr &request)
{
    Q_UNUSED(request);
    auto res = std::make_shared<ResponseBase>();
    SPDLOG_DEBUG("UserSession::ResetService Start");
    res->errorCode = m_db.ResetService();

    if(ErrorCode::NoError == res->errorCode){
        m_logManager.AddInitializeSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,true);
    }else{
        m_logManager.AddInitializeSettingsAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,false);
    }

    return res;
}

void UserSession::GetEventLog(GetEventLogRequestSharedPtr const&request, ResponseBaseHandler const&responseHandler)
{
    m_logManager.GetEventLog(request, responseHandler);
}

void UserSession::GetSystemLog(const GetSystemLogRequestSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    m_logManager.GetSystemLog(request, responseHandler);
}

ResponseBaseSharedPtr UserSession::GetEventRule()
{
    auto res = std::make_shared<GetEventRuleResponse>();
    res->eventRules = m_db.GetEventRules();
    return res;
}

ResponseBaseSharedPtr UserSession::SaveEventRule(SaveEventRuleRequestSharedPtr const&request)
{
    bool isExist = m_db.IsExistEventRule(request->eventRule.eventRuleID);

    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveEventRule(request->eventRule);

    if(res->isSuccess()){
        auto operationType = AuditLogOperationType::Added;
        if(isExist) {
            operationType = AuditLogOperationType::Updated;
        }

        m_logManager.AddEventRulesAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,operationType,request->eventRule.name);

    }
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveEventRules(RemoveEventRuleRequestSharedPtr const&request)
{
    std::vector<std::string> removedEventRuleNames;

    auto& eventRules = m_db.GetEventRules();

    for(auto& eventRuleID : request->eventRuleIDs){
        auto itor = eventRules.find(eventRuleID);
        if(itor != eventRules.end()){
            removedEventRuleNames.emplace_back(itor->second.name);
        }
    }

    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveEventRules(request->eventRuleIDs);

    if(res->isSuccess()){

        for(auto& removedEventRuleName : removedEventRuleNames){
            auto operationType = AuditLogOperationType::Removed;
            m_logManager.AddEventRulesAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,operationType,removedEventRuleName);
        }
    }
    return res;
}

ResponseBaseSharedPtr UserSession::GetEventSchedule()
{
    auto res = std::make_shared<GetEventScheduleResponse>();
    res->eventSchedules = m_db.GetEventSchedules();
    return res;
}

ResponseBaseSharedPtr UserSession::SaveEventSchedule(SaveEventScheduleRequestSharedPtr const&request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveEventSchedule(request->eventSchedule);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveEventSchedules(RemoveEventScheduleRequestSharedPtr const&request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveEventSchedules(request->eventScheduleIDs);
    return res;
}


ResponseBaseSharedPtr UserSession::SaveEventEmail(SaveEventEmailRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.SaveEventEmail(request->eventEmail);
    return res;
}

ResponseBaseSharedPtr UserSession::RemoveEventEmail(RemoveEventEmailRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.RemoveEventEmail();
    return res;
}

ResponseBaseSharedPtr UserSession::UpdateLdapSetting(UpdateLdapSettingRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.UpdateLdapSetting(request->ldapSetting);
    return res;
}

ResponseBaseSharedPtr UserSession::UpdateStatistics(UpdateStatisticsRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.UpdateStatistics(request->statistics);
    return res;
}

LicenseInfo& UserSession::GetLicenseInfo()
{
    return m_db.GetLicenseInfo();
}

ResponseBaseSharedPtr UserSession::ActivateLicense(ActivateLicenseRequestSharedPtr const& request)
{
    auto res = std::make_shared<ResponseBase>();
    res->errorCode = m_db.ActivateLicense(request->licenseKey);
    return res;
}

std::map<uuid_string, Device::DeviceProfileInfo>&  UserSession::GetDeviceProfile()
{
    return m_db.GetDeviceProfile();
}

ErrorCode UserSession::AddDevice(const Device::DeviceConnectResponseSharedPtr& connectResponse)
{
    if (!CheckAdminAuthority()) {
        return Wisenet::ErrorCode::PermissionDenied;
    }


    if(connectResponse->device.deviceName.length()==0)
        connectResponse->device.name = connectResponse->device.modelName;
    else
        connectResponse->device.name = connectResponse->device.deviceName;

    // set name by default
    if (connectResponse->device.channels.size()==1) {
        connectResponse->device.channels.begin()->second.name =
                connectResponse->device.name;
    }
    else {
        //auto channelNum = connectResponse->device.channels.size();
        //int digits = Wisenet::Core::numDigits(channelNum);

        for(auto& kv : connectResponse->device.channels) {
            kv.second.name = kv.second.channelName;
            //connectResponse->device.modelName + "-CH" + kv.first;
        }
    }

    auto devices = m_db.GetDevices();
    for(auto &device : devices){
        if(device.second.macAddress == connectResponse->device.macAddress){
            return Wisenet::ErrorCode::AlreadyRegistered;
        }
    }

    ErrorCode res = m_db.SaveDevice(connectResponse->device);
    if (res == ErrorCode::NoError) {
        m_db.SaveDeviceStatus(connectResponse->deviceStatus);
        m_db.SaveDeviceProfileInfo(connectResponse->deviceProfileInfo);

        // Add Device & Channel
        m_logManager.AddDeviceAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Added,connectResponse->device.deviceID);
        m_logManager.AddDeviceStatusAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Connect,connectResponse->device.deviceID);

        for(auto& channel : connectResponse->device.channels) {            
            m_logManager.AddChannelAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Added,channel.second.deviceID,channel.second.channelID);
        }

    }
    return res;
}

ErrorCode UserSession::RemoveDevices(const std::vector<uuid_string>& deviceIDs,
                                     std::map<uuid_string,UserGroup>& changedUserRoles,
                                     std::map<uuid_string, Layout>& changedLayouts,
                                     std::vector<uuid_string>& removedBookmarks)
{
    if (!CheckAdminAuthority()) {
        return Wisenet::ErrorCode::PermissionDenied;
    }

    std::vector<Device::Device> devices;
    for(auto& deviceID : deviceIDs){
        devices.emplace_back(m_db.GetDevice(deviceID));
    }

    ErrorCode res = m_db.RemoveDevices(deviceIDs,
                                       changedUserRoles,
                                       changedLayouts,
                                       removedBookmarks);

    if(ErrorCode::NoError == res) {

        // Add Device & Channel
        for(auto& device : devices) {
            m_logManager.AddDeviceAuditLog(m_auditLog.sessionID,m_auditLog.userName, m_auditLog.host, AuditLogOperationType::Removed,device.deviceID);

            for(auto& channel : device.channels) {               
                m_logManager.AddChannelAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,AuditLogOperationType::Removed,channel.second.deviceID,channel.second.channelID);
            }
        }
        //Add Deleted Device & Channel
        for(auto& device : devices){
            auto deletedDevice = std::make_shared<DeletedDevice>();

            deletedDevice->deviceID = device.deviceID;
            deletedDevice->serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
            deletedDevice->modelName = device.modelName;
            deletedDevice->macAddress = device.macAddress;
            deletedDevice->name = device.name;

            for(auto& channel : device.channels){

                DeletedDevice::Channel deletedChannel;
                deletedChannel.deviceID = channel.second.deviceID;
                deletedChannel.channelID = channel.second.channelID;
                deletedChannel.name = channel.second.name;

                deletedDevice->channels[deletedChannel.channelID] = deletedChannel;
            }

            m_logManager.AddDeletedDevice(deletedDevice);
        }
    }

    return res;
}

Device::Device& UserSession::GetDevice(const std::string& deviceID)
{
    return m_db.GetDevice(deviceID);
}

ErrorCode UserSession::ConnectDevice(const Device::DeviceConnectResponseSharedPtr& connectResponse)
{
    SPDLOG_DEBUG("UserSession::ConnectDevice: Start");

    // fill device name from db
    m_db.FillDeviceInfoFromDB(connectResponse->device, connectResponse->deviceProfileInfo);
    ErrorCode res = m_db.SaveDevice(connectResponse->device);

    if (res == ErrorCode::NoError) {
        m_db.SaveDeviceStatus(connectResponse->deviceStatus);
        m_db.SaveDeviceProfileInfo(connectResponse->deviceProfileInfo);
    }

    return res;
}

MediaSessionSharedPtr UserSession::CreateMediaSession(const std::string &mediaID)
{
    auto mediaSession = std::make_shared<MediaSession>(m_logManager, m_auditLog.sessionID, m_auditLog.userName, m_auditLog.host);
#ifdef USE_INSERT_OR_ASSIGN
    m_mediaSessions.insert_or_assign(mediaID, mediaSession);
#else
    m_mediaSessions[mediaID] = mediaSession;
#endif

    return mediaSession;
}

MediaSessionSharedPtr UserSession::GetMediaSession(const std::string &mediaID)
{
    auto itor = m_mediaSessions.find(mediaID);

    if(itor == m_mediaSessions.end()) {
        return nullptr;
    }

    return itor->second;
}

void UserSession::DeleteMediaSession(const std::string &mediaID)
{
    m_mediaSessions.erase(mediaID);
}

ResponseBaseSharedPtr UserSession::UpdateSoftwareService(const UpdateSoftwareServiceRequestSharedPtr &request)
{
    Q_UNUSED(request);
    auto res = std::make_shared<ResponseBase>();
    m_logManager.AddSoftwareUpgradeAuditLog(m_auditLog.sessionID,m_auditLog.userName,m_auditLog.host,true);

    return res;
}


}
}

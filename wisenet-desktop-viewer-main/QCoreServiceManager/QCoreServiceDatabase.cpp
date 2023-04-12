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
#include "QCoreServiceDatabase.h"
#include "LogSettings.h"
#include "BaseStructure.h"
#include <QSet>
#include <boost/algorithm/string.hpp>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[QSVCMAN-DB] "}, level, __VA_ARGS__)


QCoreServiceDatabase::QCoreServiceDatabase(QObject *parent)
    : QObject(parent)
{
    SPDLOG_DEBUG("Create a new QCoreServiceDatabase instance");

    m_eventEmail = std::make_shared<Wisenet::Core::EventEmail>();
}

QCoreServiceDatabase::~QCoreServiceDatabase()
{
    SPDLOG_DEBUG("Destroy a QCoreServiceDatabase instance");
}

void QCoreServiceDatabase::SetLoginUserID(std::string& userId)
{
    SPDLOG_DEBUG("QCoreServiceDatabase::SetLoginUserID={}", userId);
    m_loginUserId = userId;
}

void QCoreServiceDatabase::Update(const Wisenet::EventBaseSharedPtr &event)
{
    // full info
    if (event->EventTypeId() == Wisenet::Core::FullInfoEventType) {
        auto fullEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event);
        SetFullInfo(fullEvent);
    }
    // user roles
    else if (event->EventTypeId() == Wisenet::Core::SaveUserGroupEventType) {
        auto roleEvent = std::static_pointer_cast<Wisenet::Core::SaveUserGroupEvent>(event);
        SaveUserGroups(roleEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveUserGroupsEventType) {
        auto roleEvent = std::static_pointer_cast<Wisenet::Core::RemoveUserGroupsEvent>(event);
        RemoveUserGroup(roleEvent);
    }
    // users
    else if (event->EventTypeId() == Wisenet::Core::SaveUserEventType){
        auto userEvent = std::static_pointer_cast<Wisenet::Core::SaveUserEvent>(event);
        SaveUser(userEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveUsersEventType){
        auto userEvent = std::static_pointer_cast<Wisenet::Core::RemoveUsersEvent>(event);
        RemoveUsers(userEvent);
    }
    // Group
    else if(event->EventTypeId() == Wisenet::Core::SaveGroupEventType) {
        auto groupEvent = std::static_pointer_cast<Wisenet::Core::SaveGroupEvent>(event);
        SaveGroup(groupEvent);
    }
    else if(event->EventTypeId() == Wisenet::Core::RemoveGroupsEventType) {
        auto groupEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupsEvent>(event);
        RemoveGroups(groupEvent);
    }
    // Group mapping
    else if(event->EventTypeId() == Wisenet::Core::AddGroupMappingEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddGroupMappingEvent>(event);
        AddGroupMapping(coreEvent);
    }
    else if(event->EventTypeId() == Wisenet::Core::RemoveGroupMappingEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupMappingEvent>(event);
        RemoveGroupMapping(coreEvent);
    }
    else if(event->EventTypeId() == Wisenet::Core::UpdateGroupMappingEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::UpdateGroupMappingEvent>(event);
        UpdateGroupMapping(coreEvent);
    }
    // layouts
    else if (event->EventTypeId() == Wisenet::Core::SaveLayoutEventType){
        auto layoutEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutEvent>(event);
        SaveLayout(layoutEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::SaveLayoutsEventType){
        auto layoutsEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutsEvent>(event);
        SaveLayouts(layoutsEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveLayoutsEventType){
        auto layoutEvent = std::static_pointer_cast<Wisenet::Core::RemoveLayoutsEvent>(event);
        RemoveLayouts(layoutEvent);
    }
    // sequence layouts
    else if (event->EventTypeId() == Wisenet::Core::SaveSequenceLayoutEventType){
        auto seqEvent = std::static_pointer_cast<Wisenet::Core::SaveSequenceLayoutEvent>(event);
        SaveSequenceLayout(seqEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveSequenceLayoutsEventType){
        auto seqEvent = std::static_pointer_cast<Wisenet::Core::RemoveSequenceLayoutsEvent>(event);
        RemoveSequenceLayouts(seqEvent);
    }
    // web pages
    else if (event->EventTypeId() == Wisenet::Core::SaveWebpageEventType){
        auto webEvent = std::static_pointer_cast<Wisenet::Core::SaveWebpageEvent>(event);
        SaveWebpage(webEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveWebpagesEventType){
        auto webEvent = std::static_pointer_cast<Wisenet::Core::RemoveWebpagesEvent>(event);
        RemoveWebpages(webEvent);
    }
    // bookmarks
    else if (event->EventTypeId() == Wisenet::Core::SaveBookmarkEventType){
        auto bmEvent = std::static_pointer_cast<Wisenet::Core::SaveBookmarkEvent>(event);
        SaveBookmark(bmEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveBookmarksEventType){
        auto bmEvent = std::static_pointer_cast<Wisenet::Core::RemoveBookmarksEvent>(event);
        RemoveBookmarks(bmEvent);
    }
    // eventRules
    else if (event->EventTypeId() == Wisenet::Core::SaveEventRuleEventType){
        auto eventRuleEvent = std::static_pointer_cast<Wisenet::Core::SaveEventRuleEvent>(event);
        SaveEventRule(eventRuleEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveEventRulesEventType){
        auto eventRuleEvent = std::static_pointer_cast<Wisenet::Core::RemoveEventRulesEvent>(event);
        RemoveEventRules(eventRuleEvent);
    }
    // eventSchedules
    else if (event->EventTypeId() == Wisenet::Core::SaveEventScheduleEventType){
        auto eventScheduleEvent = std::static_pointer_cast<Wisenet::Core::SaveEventScheduleEvent>(event);
        SaveEventSchedule(eventScheduleEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveEventSchedulesEventType){
        auto eventScheduleEvent = std::static_pointer_cast<Wisenet::Core::RemoveEventSchedulesEvent>(event);
        RemoveEventSchedules(eventScheduleEvent);
    }
    // email
    else if (event->EventTypeId() == Wisenet::Core::SaveEventEmailEventType){
        auto eventEmailEvent = std::static_pointer_cast<Wisenet::Core::SaveEventEmailEvent>(event);
        SaveEventEmail(eventEmailEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveEventEmailEventType){
        RemoveEventEmail();
    }
    // devices
    else if (event->EventTypeId() == Wisenet::Core::AddDeviceEventType){
        auto devEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event);
        AddDevice(devEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::RemoveDevicesEventType){
        auto devEvent = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event);
        RemoveDevices(devEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::UpdateDevicesEventType || event->EventTypeId() == Wisenet::Core::SaveDevicesCredentialEventType){
        auto devEvent = std::static_pointer_cast<Wisenet::Core::UpdateDevicesEvent>(event);
        UpdateDevices(devEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::SaveChannelsEventType){
        auto chEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event);
        SaveChannels(chEvent);
    }
    // device status
    else if (event->EventTypeId() == Wisenet::Device::DeviceStatusEventType){
        auto statusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event);
        UpdateDeviceStatus(statusEvent);
    }
    // channel status
    else if (event->EventTypeId() == Wisenet::Device::ChannelStatusEventType){
        auto statusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event);
        UpdateChannelsStatus(statusEvent);
    }
    else if (event->EventTypeId() == Wisenet::Device::DeviceLatestStatusEventType){
        auto deviceLatestStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceLatestStatusEvent>(event);
        UpdateDeviceLatestStatus(deviceLatestStatusEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::UpdateLdapSettingEventType){
        auto ldapSettingEvent = std::static_pointer_cast<Wisenet::Core::UpdateLdapSettingEvent>(event);
        UpdateLdapSetting(ldapSettingEvent);
    }
    else if (event->EventTypeId() == Wisenet::Core::ActivateLicenseEventType){
        auto activateLicenseEvent = std::static_pointer_cast<Wisenet::Core::ActivateLicenseEvent>(event);
        UpdateLicense(activateLicenseEvent);
    }
}


void QCoreServiceDatabase::SetFullInfo(const Wisenet::Core::FullInfoEventSharedPtr &fullInfo)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_userGroups = fullInfo->userRoles;
    m_users = fullInfo->users;
    m_groups = fullInfo->groups;
    m_channelGroupMappings = fullInfo->channelGroupMappings;
    m_layouts = fullInfo->layouts;
    m_sequenceLayouts = fullInfo->sequenceLayouts;
    m_webPages = fullInfo->webpages;
    m_bookMarks = fullInfo->bookmarks;
    m_eventRules = fullInfo->eventRules;
    m_eventEmail = fullInfo->eventEmail;
    m_eventSchedules = fullInfo->eventSchedules;
    m_devices = fullInfo->devices;
    m_devicesStatus = fullInfo->devicesStatus;
    m_deviceLatestEventStatus = fullInfo->deviceLatestEventStatus;
    m_ldapSetting = fullInfo->ldapSetting;
    m_statistics = fullInfo->statistics;
    m_licenseInfo = fullInfo->licenseInfo;

    // group relation
    for (auto& group : m_groups) {
        auto parentID = group.second.parentGroupID;
        auto groupID = group.first;
        m_groupRelations[groupID].groupID = groupID;
        m_groupRelations[groupID].parentGroupID = parentID;

        // 부모를 찾는다. 없으면 부모를 만든다.
        auto parent = m_groupRelations.find(parentID);
        if (parent != m_groupRelations.end()) {
            parent->second.subGroups.insert(groupID);
        }
        else {
            GroupRelation relation;
            relation.groupID = parentID;
            relation.subGroups.insert(groupID);
            m_groupRelations[parentID] = relation;
        }
    }

    for(auto& user : m_users)
    {
        auto userId = user.second.loginID;
        if(userId == m_loginUserId)
        {
            m_loginUser = user.second;
            break;
        }
    }

    for(auto& userGroup : m_userGroups)
    {
        auto userGroupId = userGroup.second.userGroupID;
        if(userGroupId == m_loginUser.userGroupID)
        {
            m_loginUserGroup = userGroup.second;
            break;
        }
    }

    // 사양 변경으로 삭제
    /*
    for (auto& layout : m_layouts)
    {
        for(auto& item : layout.second.layoutItems)
        {
            if(item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::LocalResource ||
                    item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Image)
            {
                if(m_localfiles.find(item.resourcePath) == m_localfiles.end())
                    m_localfiles.insert(item.resourcePath);
            }
        }
    }*/


    SPDLOG_DEBUG("SetFullInfo usergroups:{} users:{}, layouts:{}, bookmarks:{}", m_userGroups.size(), m_users.size(), m_layouts.size(), m_bookMarks.size());

    if (spdlog::should_log(spdlog::level::debug)) {
        for (auto &gr : m_groups) {
            SPDLOG_DEBUG("SetFullInfo GroupINFO, groupID={}, parentGroupID={}, channels={}",
                         gr.second.groupID, gr.second.parentGroupID, gr.second.mapping.size());
        }
        SPDLOG_DEBUG("SetFullInfo GroupRelation START====");
        for (auto &gr : m_groupRelations) {
            SPDLOG_DEBUG("SetFullInfo GroupRelation, grouID={}, parentGroupID={}, subGroups={}",
                         gr.second.groupID, gr.second.parentGroupID, gr.second.subGroups.size());
            for (auto &childGroup : gr.second.subGroups) {
                SPDLOG_DEBUG("  > GroupRelation, childGroupID={}", childGroup);
            }
        }
        SPDLOG_DEBUG("SetFullInfo GroupRelation END====");
    }
}

void QCoreServiceDatabase::SaveUserGroups(const Wisenet::Core::SaveUserGroupEventSharedPtr &userGroup)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_userGroups[userGroup->userGroup.userGroupID] = userGroup->userGroup;

}

void QCoreServiceDatabase::RemoveUserGroup(const Wisenet::Core::RemoveUserGroupsEventSharedPtr &userGroups)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : userGroups->userGroupIDs) {
        auto itr = m_userGroups.find(id);
        if (itr != m_userGroups.end()) {
            m_userGroups.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveUser(const Wisenet::Core::SaveUserEventSharedPtr &user)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_users[user->user.userID] = user->user;
}

void QCoreServiceDatabase::RemoveUsers(const Wisenet::Core::RemoveUsersEventSharedPtr &users)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : users->userIDs) {
        auto itr = m_users.find(id);
        if (itr != m_users.end()) {
            m_users.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveGroup(const Wisenet::Core::SaveGroupEventSharedPtr &group)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    SPDLOG_DEBUG("SaveGroup() Event received");
    auto groupID = group->group.groupID;
    auto parentID = group->group.parentGroupID;

    m_groups[groupID] = group->group;

    // group relation
    auto gr = m_groupRelations.find(groupID);
    // 기존 그룹을 변경하는 경우 기존의 관계를 정리를 해야 한다.
    if (gr != m_groupRelations.end()) {
        auto oldParentID = gr->second.parentGroupID;
        if (oldParentID != parentID) {
            // 이전부모를 찾아서 자식관계를 끊는다.
            auto oldParent = m_groupRelations.find(oldParentID);
            if (oldParent != m_groupRelations.end()) {
                oldParent->second.subGroups.erase(groupID);
            }

            // 새로운부모를 찾는다. 없으면 만든다.
            auto newParent = m_groupRelations.find(parentID);
            if (newParent != m_groupRelations.end()) {
                newParent->second.subGroups.insert(groupID);
            }
            else {
                GroupRelation relation;
                relation.groupID = parentID;
                relation.subGroups.insert(groupID);
                m_groupRelations[parentID] = relation;
            }
        }
    }
    // 신규 그룹인 경우 부모를 찾아서 추가한다.
    else {
        m_groupRelations[groupID].groupID = groupID;
        m_groupRelations[groupID].parentGroupID = parentID;
        auto parent = m_groupRelations.find(parentID);
        if (parent != m_groupRelations.end()) {
            parent->second.subGroups.insert(groupID);
        }
        else {
            GroupRelation relation;
            relation.groupID = parentID;
            relation.subGroups.insert(groupID);
            m_groupRelations[parentID] = relation;
        }
    }
    if (spdlog::should_log(spdlog::level::debug)) {
        SPDLOG_DEBUG("SaveGroup GroupRelation START====");
        for (auto &gr : m_groupRelations) {
            SPDLOG_DEBUG("SaveGroup GroupRelation, grouID={}, parentGroupID={}, subGroups={}",
                         gr.second.groupID, gr.second.parentGroupID, gr.second.subGroups.size());
            for (auto &childGroup : gr.second.subGroups) {
                SPDLOG_DEBUG("  > GroupRelation, childGroupID={}", childGroup);
            }
        }
        SPDLOG_DEBUG("SaveGroup GroupRelation END====");
    }
}

void QCoreServiceDatabase::RemoveGroups(const Wisenet::Core::RemoveGroupsEventSharedPtr &groups)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    SPDLOG_DEBUG("RemoveGroups() Event received, groups count={}", groups->groupIDs.size());
    for (auto const& groupID : groups->groupIDs) {
        SPDLOG_DEBUG("RemoveGroups() remove Group::{}", groupID);

        auto itr = m_groups.find(groupID);
        if (itr != m_groups.end()) {
            m_groups.erase(groupID);
        }

        // group relation
        // 부모 자식관계를 찾아서 연결을 끊는다.
        auto gr = m_groupRelations.find(groupID);
        if (gr != m_groupRelations.end()) {
            auto parentID = gr->second.parentGroupID;
            auto parent = m_groupRelations.find(parentID);
            if (parent != m_groupRelations.end()) {
                SPDLOG_DEBUG("RemoveGroups() remove Group from Parent::{}", groupID, parentID);
                parent->second.subGroups.erase(groupID);
            }

            // 만약 자식관계의 그룹이 있으면 삭제 순서의 영향을 받지 않기 위해서 자식관계까지 미리 정리한다.
            auto subGroups = gr->second.subGroups;
            m_groupRelations.erase(gr);

            if (subGroups.size() > 0) {
                cleanupSubGroupRelation(subGroups);
            }
        }
    }
    if (spdlog::should_log(spdlog::level::debug)) {
        SPDLOG_DEBUG("RemoveGroups GroupRelation START====");
        for (auto &gr : m_groupRelations) {
            SPDLOG_DEBUG("RemoveGroups GroupRelation, grouID={}, parentGroupID={}, subGroups={}",
                         gr.second.groupID, gr.second.parentGroupID, gr.second.subGroups.size());
            for (auto &childGroup : gr.second.subGroups) {
                SPDLOG_DEBUG("  > GroupRelation, childGroupID={}", childGroup);
            }
        }
        SPDLOG_DEBUG("RemoveGroups GroupRelation END====");
    }
}

// 모든 하위 서브그룹리스트를 groupRelation에서 삭제
void QCoreServiceDatabase::cleanupSubGroupRelation(std::set<Wisenet::uuid_string>& subGroups)
{
    std::set<Wisenet::uuid_string> groups = subGroups;
    std::set<Wisenet::uuid_string> nextSubGroups;

    // Group Tree의 횡으로 loop를 돌면서 sub group list를 다시 뽑아낸다.
    // resursive to iterator
    size_t loopCount = 0;
    while (groups.size() > 0) {
        SPDLOG_DEBUG("cleanupSubGroupRelation() subGroups loopIndex={} count={}", loopCount, groups.size());
        for (auto &group : groups) {
            auto itr = m_groupRelations.find(group);
            if (itr != m_groupRelations.end()) {
                if (itr->second.subGroups.size() > 0) {
                    nextSubGroups.insert(itr->second.subGroups.begin(), itr->second.subGroups.end());
                }
                // group relation에서 삭제
                m_groupRelations.erase(itr);
            }
        }
        groups.clear(); // 그룹리스트 초기화

        if (nextSubGroups.size() > 0) {
            groups = nextSubGroups; // 서브그룹리스트가 있는 경우 서브그룹리스트를 그룹리스트로 전이
            nextSubGroups.clear(); // 서브그룹리스트 초기화
        }
        loopCount++;
    }
}

// 그룹내 모든 채널정보 획득
// 검색알고리즘은 cleanupSubGroupRelation() 과 동일
// result에 결과를 Append한다.
void QCoreServiceDatabase::AllResourcesInGroup(const QString &groupId, GroupResource &result)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    GroupResource tempResult;

    auto groupID = groupId.toStdString();
    auto itr = m_groups.find(groupID);
    if (itr == m_groups.end()) {
        SPDLOG_WARN("AllResourcesInGroup(), can not find groupID in DB, groupID={}", groupID);
        return;
    }

    //tempResult.channels.insert(itr->second.mapping.begin(), itr->second.mapping.end());
    for(auto& item : m_groups[groupID].mapping)
    {
        if(item.second.mappingType == Wisenet::Core::Group::MappingType::Camera)
            tempResult.channels.insert(item.first);
        else if(item.second.mappingType == Wisenet::Core::Group::MappingType::WebPage)
            tempResult.webPages.insert(item.first);
    }

    // iterate sub groups
    auto itr2 = m_groupRelations.find(groupID);
    if (itr2 == m_groupRelations.end()) {
        SPDLOG_ERROR("AllResourcesInGroup(), can not find groupID in Relation, groupID={}", groupID);
        result.channels.insert(tempResult.channels.begin(), tempResult.channels.end());
        result.webPages.insert(tempResult.webPages.begin(), tempResult.webPages.end());
        return;
    }

    std::set<Wisenet::uuid_string> groups = itr2->second.subGroups;
    std::set<Wisenet::uuid_string> nextSubGroups;

    size_t loopCount = 0;
    while (groups.size() > 0) {
        SPDLOG_DEBUG("AllResourcesInGroup() subGroups loopIndex={} count={}", loopCount, groups.size());
        for (auto &group : groups) {
            // m_groupRelations에서 sub groups 정보 획득
            auto itr = m_groupRelations.find(group);
            if (itr != m_groupRelations.end()) {
                if (itr->second.subGroups.size() > 0) {
                    nextSubGroups.insert(itr->second.subGroups.begin(), itr->second.subGroups.end());
                }
                // 서브그룹의 채널 정보를 m_groups에서 획득
                auto itr2 = m_groups.find(group);
                if (itr2 == m_groups.end()) {
                    SPDLOG_ERROR("AllResourcesInGroup() group and relation information mismatch. groupID={}", group);
                    continue;
                }
                // insert to tempResult

                //tempResult.channels.insert(itr2->second.mapping.begin(), itr2->second.mapping.end());
                for(auto& item : m_groups[group].mapping)
                {
                    if(item.second.mappingType == Wisenet::Core::Group::MappingType::Camera)
                        tempResult.channels.insert(item.first);
                    else if(item.second.mappingType == Wisenet::Core::Group::MappingType::WebPage)
                        tempResult.webPages.insert(item.first);
                }
            }
        }
        groups.clear();

        if (nextSubGroups.size() > 0) {
            groups = nextSubGroups;
            nextSubGroups.clear();
        }
        loopCount++;
    }

    if (spdlog::should_log(spdlog::level::debug)) {
        SPDLOG_DEBUG("AllResourcesInGroup(), all channels of group={}, find channels={}", groupID, tempResult.channels.size());
        for (auto &channel : tempResult.channels) {
            SPDLOG_DEBUG("  >> channel={}", channel);
        }
    }
    // append to result
    if(m_loginUser.userType == Wisenet::Core::User::UserType::Custom && !m_loginUserGroup.accessAllResources)
    {
        if(m_loginUserGroup.assignedResource)
        {
            foreach(auto& channelId, tempResult.channels)
            {
                std::vector<std::string> tokens;
                boost::split(tokens, channelId, boost::is_any_of("_"), boost::token_compress_on);

                if(tokens.size() == 2)
                {
                    if(m_loginUserGroup.assignedResource->devices.find(tokens.at(0)) != m_loginUserGroup.assignedResource->devices.end())
                    {
                        if(m_loginUserGroup.assignedResource->devices[tokens[0]].channels.find(tokens.at(1)) != m_loginUserGroup.assignedResource->devices[tokens[0]].channels.end())
                        {
                            result.channels.insert(channelId);
                        }
                    }
                }
            }

            foreach(auto& webPageId, tempResult.webPages)
            {
                foreach(auto& userGroupWebpage, m_loginUserGroup.assignedResource->webpages)
                {
                    if(webPageId == userGroupWebpage)
                        result.webPages.insert(webPageId);
                }
            }
        }
    }
    else
    {
        result.channels.insert(tempResult.channels.begin(), tempResult.channels.end());
        result.webPages.insert(tempResult.webPages.begin(), tempResult.webPages.end());
    }
}

void QCoreServiceDatabase::FillResourceNameFromOpenLayoutItems(QList<OpenLayoutItem> &results)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto &v : results) {
        if (v.itemTypeId() == 0) { // channel
            std::string deviceId = v.itemId().toStdString();
            std::string channelId = v.subId().toStdString();
            auto itr = m_devices.find(deviceId);
            if (itr != m_devices.end()) {
                auto itr2 = itr->second.channels.find(channelId);
                if (itr2 != itr->second.channels.end()) {
                    v.setName(QString::fromStdString(itr2->second.name));
                }
            }
        }
        else if (v.itemTypeId() == 1) { // webpage
            std::string webpageId = v.itemId().toStdString();
            auto itr = m_webPages.find(webpageId);
            if (itr != m_webPages.end()) {
                v.setName(QString::fromStdString(itr->second.name));
            }
        }
        else if (v.itemTypeId() == 3) { // layout
            std::string layoutId = v.itemId().toStdString();
            auto itr= m_layouts.find(layoutId);
            if (itr != m_layouts.end()) {
                v.setName(QString::fromStdString(itr->second.name));
            }
        }
    }
}

bool QCoreServiceDatabase::IsSharedLayout(QString& layoutId)
{
    return IsSharedLayout(layoutId.toStdString());
}

bool QCoreServiceDatabase::IsSharedLayout(std::string layoutId)
{
    Wisenet::Core::Layout layout;

    if(this->FindLayout(layoutId, layout))
    {
        if(layout.userName != this->m_loginUser.loginID)
            return true;
    }

    return false;
}

void QCoreServiceDatabase::AddGroupMapping(const Wisenet::Core::AddGroupMappingEventSharedPtr &coreEvent)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for(auto& item : coreEvent->channels)
    {
        m_groups[coreEvent->groupID].mapping.emplace(item.id, item);
        m_channelGroupMappings.emplace(item.id, coreEvent->groupID);
    }
}

void QCoreServiceDatabase::RemoveGroupMapping(const Wisenet::Core::RemoveGroupMappingEventSharedPtr &coreEvent)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for(auto& channelId : coreEvent->channelIDs)
    {
        m_groups[coreEvent->groupID].mapping.erase(channelId);
        m_channelGroupMappings.erase(channelId);
    }
}

void QCoreServiceDatabase::UpdateGroupMapping(const Wisenet::Core::UpdateGroupMappingEventSharedPtr &coreEvent)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for(auto& channelId : coreEvent->channelIDs)
    {
        Wisenet::Core::Group::MappingItem item = m_groups[coreEvent->from].mapping[channelId];

        m_groups[coreEvent->from].mapping.erase(channelId);
        m_groups[coreEvent->to].mapping.emplace(channelId, item);

        if (m_channelGroupMappings.find(item.id) != m_channelGroupMappings.end())
            m_channelGroupMappings.erase(item.id);
        
        m_channelGroupMappings.emplace(item.id, coreEvent->to);
    }
}

void QCoreServiceDatabase::SaveLayout(const Wisenet::Core::SaveLayoutEventSharedPtr &layout)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_layouts[layout->layout.layoutID] = layout->layout;
}

void QCoreServiceDatabase::SaveLayouts(const Wisenet::Core::SaveLayoutsEventSharedPtr &layouts)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    SPDLOG_DEBUG("QCoreServiceDatabase::SaveLayouts ");
    for(auto& layout : layouts->layouts){
        m_layouts[layout.first] = layout.second;
        SPDLOG_DEBUG("SaveLayouts name:{}", layout.second.name);
        for(auto item : layout.second.layoutItems)
            SPDLOG_DEBUG("SaveLayouts deviceId:{} channelId:{} channelID:{}", item.deviceID, item.channelID, item.itemID);
    }
}

void QCoreServiceDatabase::RemoveLayouts(const Wisenet::Core::RemoveLayoutsEventSharedPtr &layouts)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const& layoutId : layouts->layoutIDs) {
        auto itr = m_layouts.find(layoutId);
        if (itr != m_layouts.end()) {
            m_layouts.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveSequenceLayout(const Wisenet::Core::SaveSequenceLayoutEventSharedPtr &seqLayout)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_sequenceLayouts[seqLayout->sequenceLayout.sequenceLayoutID] = seqLayout->sequenceLayout;
}

void QCoreServiceDatabase::RemoveSequenceLayouts(const Wisenet::Core::RemoveSequenceLayoutsEventSharedPtr &seqLayouts)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const& id : seqLayouts->sequenceLayoutIDs) {
        auto itr = m_sequenceLayouts.find(id);
        if (itr != m_sequenceLayouts.end()) {
            m_sequenceLayouts.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveWebpage(const Wisenet::Core::SaveWebpageEventSharedPtr &webpage)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_webPages[webpage->webpage.webpageID] = webpage->webpage;
}

void QCoreServiceDatabase::RemoveWebpages(const Wisenet::Core::RemoveWebpagesEventSharedPtr &webpages)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : webpages->webpageIDs) {
        auto itr = m_webPages.find(id);
        if (itr != m_webPages.end()) {
            m_webPages.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveBookmark(const Wisenet::Core::SaveBookmarkEventSharedPtr &bookMark)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_bookMarks[bookMark->bookmark.bookmarkID] = bookMark->bookmark;
}

void QCoreServiceDatabase::RemoveBookmarks(const Wisenet::Core::RemoveBookmarksEventSharedPtr &bookMarks)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : bookMarks->bookmarkIDs) {
        auto itr = m_bookMarks.find(id);
        if (itr != m_bookMarks.end()) {
            m_bookMarks.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveEventRule(const Wisenet::Core::SaveEventRuleEventSharedPtr &eventRule)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_eventRules[eventRule->eventRule.eventRuleID] = eventRule->eventRule;
}

void QCoreServiceDatabase::RemoveEventRules(const Wisenet::Core::RemoveEventRulesEventSharedPtr &eventRules)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : eventRules->eventRuleIDs) {
        auto itr = m_eventRules.find(id);
        if (itr != m_eventRules.end()) {
            m_eventRules.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveEventSchedule(const Wisenet::Core::SaveEventScheduleEventSharedPtr &schedule)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_eventSchedules[schedule->eventSchedule.scheduleID] = schedule->eventSchedule;
}

void QCoreServiceDatabase::RemoveEventSchedules(const Wisenet::Core::RemoveEventSchedulesEventSharedPtr &schedules)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : schedules->eventScheduleIDs) {
        auto itr = m_eventSchedules.find(id);
        if (itr != m_eventSchedules.end()) {
            m_eventSchedules.erase(itr);
        }
    }
}

void QCoreServiceDatabase::SaveEventEmail(const Wisenet::Core::SaveEventEmailEventSharedPtr& email)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_eventEmail = email->eventEmail;
}

void QCoreServiceDatabase::RemoveEventEmail()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_eventEmail.reset();
}

void QCoreServiceDatabase::UpdateLdapSetting(const Wisenet::Core::UpdateLdapSettingEventSharedPtr& ldap)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_ldapSetting = ldap->ldapSetting;
}

void QCoreServiceDatabase::UpdateLicense(const Wisenet::Core::ActivateLicenseEventSharedPtr &licenseInfo)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_licenseInfo = licenseInfo->licenseInfo;
}

void QCoreServiceDatabase::UpdateStatistics(const Wisenet::Core::Statistics &statistics)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_statistics = statistics;
}

void QCoreServiceDatabase::countUpLayoutOpen(const QString &layoutId)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_layouts.find(layoutId.toStdString());
    if (itr != m_layouts.end()) {
        Wisenet::Core::Layout layout = itr->second;

        m_statistics.layoutOpenCount++;
        m_statistics.channelOpenCount += layout.layoutItems.size();
    }
}

void QCoreServiceDatabase::countUpMediaResponseTime(int msec)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    m_statistics.currentProcessMediaRequestCount++;
    m_statistics.currentProcessTotalMediaResponseTime += msec;
}

void QCoreServiceDatabase::AddDevice(const Wisenet::Core::AddDeviceEventSharedPtr &device)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    m_devices[device->device.deviceID] = device->device;
    m_devicesStatus[device->device.deviceID] = device->deviceStatus;
}

void QCoreServiceDatabase::RemoveDevices(const Wisenet::Core::RemoveDevicesEventSharedPtr &devices)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&id : devices->deviceIDs) {
        auto itr = m_devices.find(id);
        if (itr != m_devices.end()) {
            m_devices.erase(itr);
        }
        auto itr2 = m_devicesStatus.find(id);
        if (itr2 != m_devicesStatus.end()) {
            m_devicesStatus.erase(itr2);
        }
    }
}

void QCoreServiceDatabase::UpdateDevices(const Wisenet::Core::UpdateDevicesEventSharedPtr &devices)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&device : devices->devices) {
        m_devices[device.deviceID] = device;
    }
}

void QCoreServiceDatabase::SaveChannels(const Wisenet::Core::SaveChannelsEventSharedPtr &channelEvt)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    for (auto const&channel : channelEvt->channels) {
        m_devices[channel.deviceID].channels[channel.channelID] = channel;

        if (channel.use == false)
        {
            std::string channelID = channel.deviceID + "_" + channel.channelID;
            if (m_channelGroupMappings.find(channelID) != m_channelGroupMappings.end())
                m_channelGroupMappings.erase(channelID);

            std::map<Wisenet::uuid_string, Wisenet::Core::Group>::iterator it;
            for(it = m_groups.begin(); it != m_groups.end(); it++)
            {
                auto& group = it->second;
                group.mapping.erase(channelID);
            }
        }
    }
}

void QCoreServiceDatabase::UpdateDeviceStatus(const Wisenet::Device::DeviceStatusEventSharedPtr &deviceStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_devicesStatus.find(deviceStatus->deviceID);
    if (itr != m_devicesStatus.end()) {
        itr->second = deviceStatus->deviceStatus;
        if (deviceStatus->device.has_value()) {
            m_devices[deviceStatus->deviceID] = deviceStatus->device.value();
        }
    }
}

void QCoreServiceDatabase::UpdateChannelsStatus(const Wisenet::Device::ChannelStatusEventSharedPtr &channelStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    auto ditr = m_devices.find(channelStatus->deviceID);
    if (ditr == m_devices.end()) {
        SPDLOG_WARN("UpdateChannelsStatus, can not find deviceID in db, id={}", channelStatus->deviceID);
        return;
    }
    auto& device = ditr->second;

    auto itr = m_devicesStatus.find(channelStatus->deviceID);
    if (itr != m_devicesStatus.end()) {
        auto& deviceStatus = itr->second;
        for (auto const& channel : channelStatus->channelsStatus) {
            Wisenet::Device::ChannelStatus newStatus = channel.second;

            /* 채널 정보가 있는 경우, 디바이스에 업데이트하고, channel status에서는 삭제 */
            if (newStatus.channel.has_value()) {
                Wisenet::Device::Device::Channel channel = newStatus.channel.get();
                device.channels[channel.channelID] = channel;
                newStatus.channel = boost::none;
            }
            deviceStatus.channelsStatus[channel.first] = newStatus;
        }
    }
}

void QCoreServiceDatabase::UpdateDeviceLatestStatus(const Wisenet::Device::DeviceLatestStatusEventSharedPtr& deviceLatestStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    auto itr = m_deviceLatestEventStatus.find(deviceLatestStatus->deviceID);

    if(itr != m_deviceLatestEventStatus.end())
    {
        itr->second.emplace(deviceLatestStatus->eventKey, deviceLatestStatus->status);
    }
    else
    {
        std::map<std::string, bool> statusMap;
        statusMap.emplace(deviceLatestStatus->eventKey, deviceLatestStatus->status);
        m_deviceLatestEventStatus.emplace(deviceLatestStatus->deviceID, statusMap);
    }
}

std::map<Wisenet::uuid_string, Wisenet::Core::UserGroup> QCoreServiceDatabase::GetUserGroups()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_userGroups;
}

std::map<std::string, Wisenet::Core::User> QCoreServiceDatabase::GetUsers()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_users;
}

std::map<Wisenet::uuid_string, Wisenet::Core::Group> QCoreServiceDatabase::GetGroups()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_groups;
}

std::map<std::string, Wisenet::uuid_string> QCoreServiceDatabase::GetChannelGroupMappings()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_channelGroupMappings;
}

std::map<Wisenet::uuid_string, Wisenet::Core::Layout> QCoreServiceDatabase::GetLayouts()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_layouts;
}

std::map<Wisenet::uuid_string, Wisenet::Core::SequenceLayout> QCoreServiceDatabase::GetSequenceLayouts()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_sequenceLayouts;
}

std::map<Wisenet::uuid_string, Wisenet::Core::Webpage> QCoreServiceDatabase::GetWebpages()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_webPages;
}

std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> QCoreServiceDatabase::GetBookmarks()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_bookMarks;
}

std::map<Wisenet::uuid_string, Wisenet::Core::EventRule> QCoreServiceDatabase::GetEventRules()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_eventRules;
}

std::map<Wisenet::uuid_string, Wisenet::Core::EventSchedule> QCoreServiceDatabase::GetEventSchedules()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_eventSchedules;
}

std::shared_ptr<Wisenet::Core::EventEmail> QCoreServiceDatabase::GetEventEmail()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_eventEmail;
}

std::map<Wisenet::uuid_string, Wisenet::Device::Device> QCoreServiceDatabase::GetDevices()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    //SPDLOG_DEBUG("GetDevices() called");
    return m_devices;
}

std::map<Wisenet::uuid_string, Wisenet::Device::DeviceStatus> QCoreServiceDatabase::GetDevicesStatus()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_devicesStatus;
}

std::map<Wisenet::uuid_string, std::map<std::string, bool>>  QCoreServiceDatabase::GetDeviceLatestEventStatus()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    return m_deviceLatestEventStatus;
}

QStringList QCoreServiceDatabase::GetTotalEventList()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    QStringList totalTriggerList;

    for(auto& event : Wisenet::TitleAlarmMap){
        totalTriggerList  << QString::fromStdString(event.first);
        if(event.first == Wisenet::TitleAlarmType::DeviceAlarmInput)
            totalTriggerList  << QString::fromStdString(Wisenet::FixedAlarmType::NetworkAlarmInput);
    }

    QSet<QString> dynamicEvent = GetDynamicEventList();

    foreach(auto & event,dynamicEvent){
        totalTriggerList.push_back(event);
    }

    return totalTriggerList;
}

QSet<QString> QCoreServiceDatabase::GetDynamicEventList()
{
    QSet<QString> dynamicEvent;
    for(auto &device : m_devices){
        for(auto& event : device.second.deviceCapabilities.eventNames){
            if( event == "ObjectDetection")
            {
                dynamicEvent.insert(QString::fromUtf8("ObjectDetection.Person"));
                dynamicEvent.insert(QString::fromUtf8("ObjectDetection.Face"));
                dynamicEvent.insert(QString::fromUtf8("ObjectDetection.Vehicle"));
                dynamicEvent.insert(QString::fromUtf8("ObjectDetection.LicensePlate"));
            }
            else
            {
                dynamicEvent.insert(QString::fromUtf8(event.c_str()));
            }
        }
    }
    return dynamicEvent;
}

void QCoreServiceDatabase::GetLoginUser(Wisenet::Core::User& user)
{
    user = m_loginUser;
}

void QCoreServiceDatabase::GetLoginUserGroup(Wisenet::Core::UserGroup& userGroup)
{
    userGroup = m_loginUserGroup;
}

Wisenet::Core::UserGroup::Permission QCoreServiceDatabase::GetLoginUserPermission()
{
    return m_loginUserGroup.userPermission;
}

Wisenet::Core::LdapSetting QCoreServiceDatabase::GetLdapSetting()
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    qDebug() <<"QCoreServiceDatabase::GetLdapSetting m_ldapSetting" << QString::fromStdString(m_ldapSetting.serverUrl);
    return m_ldapSetting;
}

Wisenet::Core::Statistics QCoreServiceDatabase::GetStatistics()
{
    return m_statistics;
}

Wisenet::Core::LicenseInfo QCoreServiceDatabase::GetLicenseInfo()
{
    return m_licenseInfo;
}

bool QCoreServiceDatabase::FindUserRole(const std::string &uuid, Wisenet::Core::UserGroup &retUserRole)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_userGroups.find(uuid);
    if (itr != m_userGroups.end()) {
        retUserRole = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindUser(const std::string &userName, Wisenet::Core::User &retUser)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_users.find(userName);
    if (itr != m_users.end()) {
        retUser = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindGroup(const std::string& uuid, Wisenet::Core::Group& retGroup)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_groups.find(uuid);
    if (itr != m_groups.end()) {
        retGroup = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindLayout(const std::string &uuid, Wisenet::Core::Layout &retLayout)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_layouts.find(uuid);
    if (itr != m_layouts.end()) {
        retLayout = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindSequenceLayout(const std::string &uuid, Wisenet::Core::SequenceLayout &retSequenceLayout)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_sequenceLayouts.find(uuid);
    if (itr != m_sequenceLayouts.end()) {
        retSequenceLayout = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindWebpage(const std::string &uuid, Wisenet::Core::Webpage &retWebpage)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_webPages.find(uuid);
    if (itr != m_webPages.end()) {
        retWebpage = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindBookmark(const std::string &uuid, Wisenet::Core::Bookmark &retBookmark)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_bookMarks.find(uuid);
    if (itr != m_bookMarks.end()) {
        retBookmark = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindEventRule(const std::string &uuid, Wisenet::Core::EventRule &retEventRule)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_eventRules.find(uuid);
    if (itr != m_eventRules.end()) {
        retEventRule = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindEventSchedule(const std::string &uuid, Wisenet::Core::EventSchedule &retEventSchedule)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_eventSchedules.find(uuid);
    if (itr != m_eventSchedules.end()) {
        retEventSchedule = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindDevice(const std::string &uuid, Wisenet::Device::Device &retDevice)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_devices.find(uuid);
    if (itr != m_devices.end()) {
        retDevice = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindDeviceStatus(const std::string &uuid, Wisenet::Device::DeviceStatus &retDeviceStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_devicesStatus.find(uuid);
    if (itr != m_devicesStatus.end()) {
        retDeviceStatus = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::FindChannelStatus(const std::string &deviceId, const std::string &channelId, Wisenet::Device::ChannelStatus &retChannelStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_devicesStatus.find(deviceId);
    if (itr != m_devicesStatus.end()) {
        auto itr2 = itr->second.channelsStatus.find(channelId);
        if (itr2 != itr->second.channelsStatus.end()) {
            retChannelStatus = itr2->second;
            return true;
        }
    }
    return false;
}

bool QCoreServiceDatabase::FindChannel(const std::string &deviceId, const std::string &channelId, Wisenet::Device::Device::Channel &retChannel)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_devices.find(deviceId);
    if (itr != m_devices.end()) {
        auto itr2 = itr->second.channels.find(channelId);
        if (itr2 != itr->second.channels.end()) {
            retChannel = itr2->second;
            return true;
        }
    }
    return false;
}

bool QCoreServiceDatabase::FIndDeviceLatestEventStatus(const std::string& uuid, std::map<std::string, bool>& retDeviceStatus)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto itr = m_deviceLatestEventStatus.find(uuid);
    if (itr != m_deviceLatestEventStatus.end()) {
        retDeviceStatus = itr->second;
        return true;
    }
    return false;
}

bool QCoreServiceDatabase::ContainsDeviceByMacAddress(const std::string &macAddress)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    /* todo */
    for(auto & item : m_devices){
        if(item.second.macAddress == macAddress)
            return true;
    }
    return false;
}

bool QCoreServiceDatabase::ContainsDeviceByIp(const std::string &ipAddress, const int port)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    /* todo */
    for(auto & item : m_devices){
        if(item.second.connectionInfo.host == ipAddress && item.second.connectionInfo.port == port)
            return true;
    }
    return false;
}

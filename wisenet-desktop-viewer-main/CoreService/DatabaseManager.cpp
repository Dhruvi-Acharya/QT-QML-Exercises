#include "DatabaseManager.h"

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>
#include <QFile>
#include <QProcess>
#include <QDate>
#include <QTime>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

#include "DeviceClient/DeviceEvent.h"
#include "LogSettings.h"
#include "AppLocalDataLocation.h"
#include "DigestAuthenticator.h"


// LOGGER MACRO REDEFINE
#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[SERVICE_DB] "}, level, __VA_ARGS__)

#ifdef WIN32
#define USE_INSERT_OR_ASSIGN
#endif
namespace Wisenet
{
namespace Core
{

DatabaseManager::DatabaseManager()
    :m_managementDatabase(std::make_shared<ManagementDatabase>())
    ,m_serviceType(CoreServicetype::LocalService)
    ,m_version("v0.9.0_20210501")
    ,m_logRetensionTimeMsec(0)
{
}



DatabaseManager::~DatabaseManager()
{

}


void RemoveDirectory(QString Path)
{
    QDir pathDir(Path);
    if (pathDir.exists())
    {
        pathDir.removeRecursively();
    }
}

bool CopyFile(const QString& sourceFileName, const QString& destFileName, bool remove =false)
{
    if (QFile::exists(destFileName))
    {
        QFile::remove(destFileName);
    }
    if(!QFile::copy(sourceFileName, destFileName))
        return false;
    if (remove && QFile::exists(sourceFileName))
    {
        QFile::remove(sourceFileName);
    }
    return true;
}


bool CopyDBFile(const QString &origin, const QString &targetPath, bool remove = false)
{
    SPDLOG_DEBUG("CopyDBFile Start {} {}", targetPath.toStdString(), origin.toStdString());
    QString source = origin + "/"  + ManagementDatabase::FILENAME;
    QString dest = targetPath + "/" + ManagementDatabase::FILENAME;

    if(!CopyFile(source, dest, remove))
        return false;

    if(QFile::exists(source+ "-shm") && !CopyFile(source + "-shm", dest + "-shm", remove))
        return false;

    if(QFile::exists(source+ "-wal") &&!CopyFile(source + "-wal", dest + "-wal", remove))
        return false;
    SPDLOG_DEBUG("CopyDBFile End {} {}", source.toStdString(), dest.toStdString());
    return true;
}

bool RemoveDbFile(QString path)
{
    bool ret = true;
    SPDLOG_DEBUG("RemoveDbFile", path.toStdString());
    QString origin = path + "/"  + ManagementDatabase::FILENAME;
    if (QFile::exists(origin) && !QFile::remove(origin))
        ret = false;
    else if (QFile::exists(origin + "-shm") && !QFile::remove(origin + "-shm"))
        ret = false;
    else if (QFile::exists(origin + "-wal") && !QFile::remove(origin + "-wal"))
        ret = false;
    return ret;
}

bool DatabaseManager::Open()
{
    SPDLOG_DEBUG("Open a database");

    QStringList list = QSqlDatabase::drivers();

    auto isSupported = false;
    for(QString& item : list) {

        SPDLOG_INFO("{}", item.toStdString());

        if(0 == item.compare(QString("QSQLITE"))) {
            isSupported = true;
        }
    }

    if(!isSupported)
    {
        SPDLOG_ERROR("Not supported a SQLite Driver.");
        return false;
    }
    QString dbPath = GetAppDataLocation();
    m_path = dbPath;

    SPDLOG_DEBUG("database path = {}", dbPath.toLocal8Bit().toStdString());

    if(QDir(dbPath + "/Restore").exists())
    {
        SPDLOG_DEBUG("Restore dir exist");
        if(!CopyDBFile(dbPath + "/Restore", dbPath, true))
        {
            SPDLOG_DEBUG("Restore Fail");
            RemoveDbFile(dbPath);
        }
        RemoveDirectory(dbPath + "/Restore");
    }

    if(!QDir(dbPath).exists())
    {
        if(!QDir().mkpath(dbPath))
        {
            SPDLOG_ERROR("Failed to create a database directory. path={}", dbPath.toStdString());
            return false;
        }
    }

    if(!m_managementDatabase->Open(dbPath))
    {
        SPDLOG_ERROR("Failed to open the management database.");
        return false;
    }

    return Load();
}

void DatabaseManager::Close()
{
    m_managementDatabase->Close();
    if(m_bReset){
        if(!RemoveDbFile(m_path))
            SPDLOG_ERROR("reset fail");
        else
            SPDLOG_ERROR("reset success");
    }
    ClearAll();
}

bool DatabaseManager::Delete()
{
    QString management = GetAppDataLocation() + "/" + ManagementDatabase::FILENAME;
    if(QFile::exists(management)) {
        if(!QFile::remove(management)) {
            SPDLOG_ERROR("Failed to delete a management database. path={}", management.toStdString());
            return false;
        }
    }
    return true;
}

ErrorCode DatabaseManager::LogIn(std::string& userId, const std::string &loginID, const std::string &password, const bool isHashed)
{
    for(auto& user : m_users){
        if(user.second.loginID == loginID){
            userId = user.second.userID;
            if(user.second.ldapUser)
                return ErrorCode::LdapUser;
            else if(isHashed && user.second.password != password)
                return ErrorCode::UnAuthorized;
            else if(!isHashed && user.second.password != DigestAuthenticator::hashSpecialSha256(userId, password))
                return ErrorCode::UnAuthorized;
            else
                return ErrorCode::NoError;
        }
    }

    return ErrorCode::UnAuthorized;
}

bool DatabaseManager::IsExistUserGroup(std::string &userGroupID)
{
    try{

        auto itor = m_userGroups.find(userGroupID);

        if(itor != m_userGroups.end()){
            return true;
        }

    } catch (const std::exception& e) {
        SPDLOG_ERROR("IsExistUser is failed. Error={}", e.what());
        return false;
    }

    return false;
}

Wisenet::ErrorCode DatabaseManager::SaveUserGroup(const  UserGroup& userGroup)
{
    auto userGroupsBackup = m_userGroups;
    try {

        m_managementDatabase->GetUserGroupTable().Save(userGroup);
        m_managementDatabase->GetUserGroupResourceTable().Remove(userGroup.userGroupID);
        m_managementDatabase->GetUserGroupResourceTable().Add(userGroup.userGroupID, userGroup.assignedResource.value());
#ifdef USE_INSERT_OR_ASSIGN
        m_userGroups.insert_or_assign(userGroup.userGroupID, userGroup);
#else
        m_userGroups[userGroup.userGroupID] = userGroup;
#endif

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_userGroups = userGroupsBackup;
        SPDLOG_ERROR("SaveUserGroup is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::RemoveUserGroups(const std::vector<std::string> &userGroups,
                                                     std::vector<std::string> &removedUserIDs,
                                                     std::vector<uuid_string> &removedLayoutIDs,
                                                     std::vector<uuid_string> &removedSequenceIDs)
{
    auto userGroupsBackup = m_userGroups;
    auto usersBackup = m_users;
    auto layoutsBackup = m_layouts;
    auto sequenceLayoutsBackup = m_sequenceLayouts;

    try {
        //DB 변경.
        m_managementDatabase->Transaction();

        //1. Remove UserRole
        for(auto& groupID : userGroups){
            m_managementDatabase->GetUserGroupTable().Remove(groupID);
            m_managementDatabase->GetUserGroupResourceTable().Remove(groupID);
            m_userGroups.erase(groupID);

            //2. Remove User by userRoleID
            m_managementDatabase->GetUserTable().RemoveByUserGroupID(groupID,removedUserIDs);
            for(auto& removedUserName : removedUserIDs) {
                m_managementDatabase->GetUserGroupResourceTable().Remove(removedUserName);
            }

            //3. Remove Layout & LayoutItem by userID
            for(auto& removedUserName : removedUserIDs) {
                m_managementDatabase->GetLayoutTable().RemoveByUserName(removedUserName, removedLayoutIDs);
            }
            for(auto& removedLayoutID : removedLayoutIDs){
                m_managementDatabase->GetLayoutItemTable().Remove(removedLayoutID);
                m_layouts.erase(removedLayoutID);
            }

            //4. Sequece Layout & SequenceItem by userID
            for(auto& removedUserName : removedUserIDs){
                m_managementDatabase->GetSequenceTable().RemoveByUserName(removedUserName, removedSequenceIDs);
            }
            for(auto& removedSequenceID : removedSequenceIDs){
                m_managementDatabase->GetSequenceItemTable().Remove(removedSequenceID);
                m_sequenceLayouts.erase(removedSequenceID);
            }
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        removedLayoutIDs.clear();
        removedSequenceIDs.clear();
        m_users = usersBackup;
        m_layouts = layoutsBackup;
        m_sequenceLayouts = sequenceLayoutsBackup;
        SPDLOG_ERROR("RemoveUsers is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

bool DatabaseManager::IsExistUser(std::string &loginID)
{
    try{

        for(auto& user : m_users){
            if(user.second.loginID == loginID)
                return true;
        }
        return false;

    } catch (const std::exception& e) {
        SPDLOG_ERROR("IsExistUser is failed. Error={}", e.what());
        return false;
    }

    return false;
}

ErrorCode DatabaseManager::SaveUser(const User &user)
{
    auto usersBackup = m_users;
    try {
        SPDLOG_DEBUG("DatabaseManager::SaveUser={} pass={}", user.name, user.password);
        m_managementDatabase->GetUserTable().Save(user);
#ifdef USE_INSERT_OR_ASSIGN
        m_users.insert_or_assign(user.userID, user);
#else
        m_users[user.userID] = user;
#endif

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_users = usersBackup;
        SPDLOG_ERROR("SaveUser is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveUsers(const std::vector<std::string> &userIDs,
                                       std::vector<uuid_string> &removedLayoutIDs,
                                       std::vector<uuid_string> &removedSequenceIDs)
{
    auto usersBackup = m_users;
    auto layoutsBackup = m_layouts;
    auto sequenceLayoutsBackup = m_sequenceLayouts;

    try {
        //DB 변경.
        m_managementDatabase->Transaction();

        //1. Remove User
        for(auto& userID : userIDs) {
            m_managementDatabase->GetUserTable().Remove(userID);
            m_users.erase(userID);
        }

        //2. Remove Layout & LayoutItem by userID
        for(auto& userID : userIDs){
            m_managementDatabase->GetLayoutTable().RemoveByUserName(userID, removedLayoutIDs);
        }
        for(auto& removedLayoutID : removedLayoutIDs){
            m_managementDatabase->GetLayoutItemTable().Remove(removedLayoutID);
            m_layouts.erase(removedLayoutID);
        }

        //3. Remove Sequece Layout & SequenceLayoutItem by userID
        for(auto& userID : userIDs){
            m_managementDatabase->GetSequenceTable().RemoveByUserName(userID, removedSequenceIDs);
        }
        for(auto& removedSequenceID : removedSequenceIDs){
            m_managementDatabase->GetSequenceItemTable().Remove(removedSequenceID);
            m_sequenceLayouts.erase(removedSequenceID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        removedLayoutIDs.clear();
        removedSequenceIDs.clear();
        m_users = usersBackup;
        m_layouts = layoutsBackup;
        m_sequenceLayouts = sequenceLayoutsBackup;
        SPDLOG_ERROR("RemoveUsers is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::SaveGroup(const Group& group)
{
    auto groupBackup = m_groups;

    try {
        SPDLOG_DEBUG("SaveGroup() start, id={}, name={}",
                     group.groupID, group.name);
        m_managementDatabase->Transaction();

        m_managementDatabase->GetGroupTable().Save(group);


        // group 데이터를 업데이트 한다.
#ifdef USE_INSERT_OR_ASSIGN
        m_groups.insert_or_assign(group.groupID, group);
#else
        m_groups[group.groupID] = group;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_groups = groupBackup;
        SPDLOG_ERROR("SaveGroup is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::RemoveGroups(const std::vector<uuid_string>& groupIDs, std::vector<uuid_string>& removedGroups, std::map<uuid_string, std::vector<std::string>>& removedMapping)
{
    auto groupBackup = m_groups;

    try {
        SPDLOG_DEBUG("RemoveGroups() start");
        m_managementDatabase->Transaction();

        for(auto& groupID : groupIDs)
        {
            // child를 가져옴
            m_managementDatabase->GetGroupTable().GetChildrenByBottomUpOrder(groupID, removedGroups);

            removedGroups.push_back(groupID);
        }

        for(auto& groupID: removedGroups)
        {
            // Group table에서 지운다.
            m_managementDatabase->GetGroupTable().Remove(groupID);
            m_groups.erase(groupID);

            // Group mapping에서 해제될 채널을 채움

            std::vector<Group::MappingItem> removedMap;
            m_managementDatabase->GetGroupMappingTable().Get(groupID, removedMap);

            std::vector<std::string> v;

            for(auto& item : removedMap){
                v.push_back(item.id);
            }

            removedMapping[groupID] = v;

            // Group mapping table에서 지운다.
            m_managementDatabase->GetGroupMappingTable().RemoveAll(groupID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_groups = groupBackup;
        SPDLOG_ERROR("RemoveGroups is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }



    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::AddGroupMapping(const uuid_string& groupID, const std::vector<Group::MappingItem>& resourceList)
{
    if(m_groups.find(groupID) == m_groups.end())
    {
        return Wisenet::ErrorCode::InvalidRequest;
    }

    try {
        SPDLOG_DEBUG("AddGroupMapping() start, id={}", groupID);
        m_managementDatabase->Transaction();

        for(auto& item : resourceList){
            m_managementDatabase->GetGroupMappingTable().Add(groupID, item.id, item.mappingType);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        SPDLOG_ERROR("AddGroupMapping is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    for(auto& item : resourceList){
        m_groups[groupID].mapping.emplace(item.id, item);
        m_channelGroupMappings.emplace(item.id, groupID);
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::RemoveGroupMapping(const uuid_string& groupID, const std::vector<std::string>& resourceList)
{
    if(m_groups.find(groupID) == m_groups.end())
    {
        return Wisenet::ErrorCode::InvalidRequest;
    }

    try {
        SPDLOG_DEBUG("DeleteGroupMapping() start, id={}", groupID);
        m_managementDatabase->Transaction();

        for(auto& channelID : resourceList){
            m_managementDatabase->GetGroupMappingTable().Remove(groupID, channelID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        SPDLOG_ERROR("DeleteGroupMapping is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    for(auto& channelID : resourceList){
        m_groups[groupID].mapping.erase(channelID);
        m_channelGroupMappings.erase(channelID);
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::UpdateGroupMapping(const uuid_string& from, const uuid_string& to, const std::vector<std::string>& resourceList)
{
    if(m_groups.find(from) == m_groups.end() || m_groups.find(to) == m_groups.end())
    {
        return Wisenet::ErrorCode::InvalidRequest;
    }

    try {
        SPDLOG_DEBUG("UpdateGroupMapping() start, from={}, to={}", from, to);
        m_managementDatabase->Transaction();

        for(auto& channelId : resourceList){
            m_managementDatabase->GetGroupMappingTable().Update(from, to, channelId);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        SPDLOG_ERROR("UpdateGroupMapping is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    for(auto& channelId : resourceList)
    {
        Group::MappingItem item = m_groups[from].mapping[channelId];

        m_groups[from].mapping.erase(channelId);
        m_groups[to].mapping.emplace(channelId, item);
        m_channelGroupMappings.emplace(channelId, to);
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::SaveLayout(const Layout &layout)
{
    auto layoutsBackup = m_layouts;

    try {
        SPDLOG_DEBUG("SaveLayout() start, id={}, itemsCount={}",
                     layout.layoutID, layout.layoutItems.size());
        m_managementDatabase->Transaction();

        // layout이 기존에 있으면 Item을 모두 삭제한다.
        if(0 < m_layouts.count(layout.layoutID)) {
            m_managementDatabase->GetLayoutItemTable().Remove(layout.layoutID);
        }

        m_managementDatabase->GetLayoutTable().Save(layout);

        // 새로운 Item을 추가한다.
        for(auto& layoutItem : layout.layoutItems) {
            m_managementDatabase->GetLayoutItemTable().Add(layout.layoutID, layoutItem);
        }
        // layout 데이터를 업데이트 한다.
#ifdef USE_INSERT_OR_ASSIGN
        m_layouts.insert_or_assign(layout.layoutID, layout);
#else
        m_layouts[layout.layoutID] = layout;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_layouts = layoutsBackup;
        SPDLOG_ERROR("SaveLayout is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveLayouts(const std::vector<uuid_string> &layoutIDs,
                                         std::map<uuid_string, SequenceLayout>& changedSequenceLayouts,
                                         std::map<uuid_string,UserGroup>& changedUserRoles)
{
    auto layoutsBackup = m_layouts;
    auto sequenceLayoutsBackup = m_sequenceLayouts;
    auto userRolesBackup = m_userGroups;
    try {
        //DB 변경.
        m_managementDatabase->Transaction();

        //1. Remove Layout
        //layoutID를 삭제한다.
        for(auto& layoutID : layoutIDs) {
            m_managementDatabase->GetLayoutItemTable().Remove(layoutID);
            m_managementDatabase->GetLayoutTable().Remove(layoutID);

            m_layouts.erase(layoutID);
        }

        //2. Remove Layout in UserRole Table
        //사용하는 User Role 리스트를 얻고 삭제한다.
        std::set<uuid_string> changedUserRoleIDs;
        auto layoutTypeInUserRole = UserGroupResourceTable::UserGroupResourceType::Layout;
        for(auto& layoutID : layoutIDs) {
            m_managementDatabase->GetUserGroupResourceTable().RemoveByItemID(layoutTypeInUserRole, layoutID, changedUserRoleIDs);
        }
        //변경된 User Role리스트를 업데이트 한다.
        for(auto changedUserRoleID : changedUserRoleIDs) {
            auto& userRole = m_userGroups[changedUserRoleID];
            if (userRole.assignedResource) {
                auto& assignedResource = userRole.assignedResource.value();
                assignedResource.layouts.clear();
                m_managementDatabase->GetUserGroupResourceTable().GetLayouts(changedUserRoleID, assignedResource.layouts);
                changedUserRoles.emplace(changedUserRoleID, userRole);
            }
        }

        //3. Remove Layout in SequenceItem Table
        // 사용하는 시퀀스 리스트를 얻고 삭제한다.
        std::set<uuid_string> changedSequenceIDs;
        for(auto& layoutID : layoutIDs) {
            m_managementDatabase->GetSequenceItemTable().RemoveByItemID(layoutID,changedSequenceIDs);
        }
        // 변경된 시퀀스 리스트를 업데이트 한다.
        for(auto changedSequenceID : changedSequenceIDs) {
            auto& sequenceLayout = m_sequenceLayouts[changedSequenceID];
            sequenceLayout.sequenceLayoutItems.clear();
            m_managementDatabase->GetSequenceItemTable().Get(changedSequenceID, sequenceLayout.sequenceLayoutItems);
            changedSequenceLayouts.emplace(changedSequenceID, sequenceLayout);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        changedSequenceLayouts.clear();
        changedUserRoles.clear();
        m_layouts = layoutsBackup;
        m_sequenceLayouts = sequenceLayoutsBackup;
        m_userGroups = userRolesBackup;
        SPDLOG_ERROR("RemoveLayouts is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveSequenceLayout(const SequenceLayout &sequenceLayout)
{
    auto sequenceLayoutsBackup = m_sequenceLayouts;

    try {

        m_managementDatabase->Transaction();

        // 기존에 존재하는 sequncelayout이 있다면, 업데이트하는 것이므로 Item을 모두 삭제한다.
        if(0 < m_sequenceLayouts.count(sequenceLayout.sequenceLayoutID)){
            m_managementDatabase->GetSequenceItemTable().Remove(sequenceLayout.sequenceLayoutID);
        }

        m_managementDatabase->GetSequenceTable().Save(sequenceLayout);
        // 새로운 Item을 추가한다.
        for(auto& sequenceLayoutItem : sequenceLayout.sequenceLayoutItems) {
            m_managementDatabase->GetSequenceItemTable().Add(sequenceLayout.sequenceLayoutID, sequenceLayoutItem);
        }
#ifdef USE_INSERT_OR_ASSIGN
        m_sequenceLayouts.insert_or_assign(sequenceLayout.sequenceLayoutID, sequenceLayout);
#else
        m_sequenceLayouts[sequenceLayout.sequenceLayoutID] = sequenceLayout;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_sequenceLayouts = sequenceLayoutsBackup;
        SPDLOG_ERROR("SaveSequenceLayout is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveSequenceLayouts(const std::vector<uuid_string> &sequenceLayoutIDs)
{
    auto sequenceLayoutsBackup = m_sequenceLayouts;

    try {

        m_managementDatabase->Transaction();

        // SquenceLayout Item과 SequenceLayout을 삭제한다.
        for(auto& sequenceLayoutID : sequenceLayoutIDs) {
            m_managementDatabase->GetSequenceItemTable().Remove(sequenceLayoutID);
            m_managementDatabase->GetSequenceTable().Remove(sequenceLayoutID);

            m_sequenceLayouts.erase(sequenceLayoutID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_sequenceLayouts = sequenceLayoutsBackup;
        SPDLOG_ERROR("RemoveSequenceLayouts is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveWebpage(const Webpage &webpage)
{
    auto webpagesBackup = m_webpages;
    try {
        //webpage를 업데이트 한다.
        m_managementDatabase->GetWebPageTable().Save(webpage);

        //webpage를 업데이트 한다.
#ifdef USE_INSERT_OR_ASSIGN
        m_webpages.insert_or_assign(webpage.webpageID, webpage);
#else
        m_webpages[webpage.webpageID] = webpage;
#endif

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_webpages = webpagesBackup;
        SPDLOG_ERROR("SaveWebpage is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveWebpages(const std::vector<uuid_string> &webpageIDs,
                                          std::map<uuid_string,UserGroup>& changedUserRoles,
                                          std::map<uuid_string, Layout>& changedLayouts,
                                          std::map<uuid_string, std::vector<std::string>>& removedGroupChannelMappings)
{
    auto webpagesBackup = m_webpages;
    auto userRolesBakcup = m_userGroups;
    auto layoutsBackup = m_layouts;

    std::set<uuid_string> changedLayoutIDs;

    try {

        m_managementDatabase->Transaction();

        //1.Remove Webpage
        for(auto& webpageID : webpageIDs) {
            m_managementDatabase->GetWebPageTable().Remove(webpageID);
            m_managementDatabase->GetLayoutItemTable().RemoveByItemID(webpageID, changedLayoutIDs);
            m_webpages.erase(webpageID);

            // mapping에서 삭제
            m_managementDatabase->GetGroupMappingTable().RemoveChannel(webpageID);
            uuid_string groupId = m_channelGroupMappings[webpageID];

            // vector가 없으면 추가
            if(removedGroupChannelMappings.find(groupId) == removedGroupChannelMappings.end())
            {
                std::vector<std::string> channelList;
                removedGroupChannelMappings.emplace(groupId, channelList);
            }

            // event를 보낼 자료구조에 담음
            removedGroupChannelMappings[groupId].push_back(webpageID);

            // 멤버변수 내에 mapping 정리
            m_groups[groupId].mapping.erase(webpageID);
            m_channelGroupMappings.erase(webpageID);
        }

        //2.Remove Webpage in UserRole Resource.
        //사용하는 User Role 리스트를 얻고 삭제한다.
        std::set<uuid_string> changedUserRoleIDs;
        auto webpageTypeInUserRole = UserGroupResourceTable::UserGroupResourceType::Webpage;
        for(auto& webpageID : webpageIDs) {
            m_managementDatabase->GetUserGroupResourceTable().RemoveByItemID(webpageTypeInUserRole, webpageID, changedUserRoleIDs);
        }
        //변경된 User Role리스트를 업데이트 한다.
        for(auto changedUserRoleID : changedUserRoleIDs) {
            auto& userRole = m_userGroups[changedUserRoleID];
            if (userRole.assignedResource) {
                auto& assignedResource = userRole.assignedResource.value();
                assignedResource.webpages.clear();
                m_managementDatabase->GetUserGroupResourceTable().GetWebpages(changedUserRoleID, assignedResource.webpages);
                changedUserRoles.emplace(changedUserRoleID, userRole);
            }
        }

        //변경된 Layout Item 리스트를 업데이트 한다.
        for(auto changedLayoutID : changedLayoutIDs) {
            auto& layout =  m_layouts[changedLayoutID];
            layout.layoutItems.clear();
            m_managementDatabase->GetLayoutItemTable().Get(changedLayoutID, layout.layoutItems);
            changedLayouts.emplace(changedLayoutID, layout);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        changedUserRoles.clear();
        changedLayouts.clear();
        m_webpages = webpagesBackup;
        m_layouts = layoutsBackup;
        m_userGroups = userRolesBakcup;
        SPDLOG_ERROR("RemoveWebpages is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveBookmark(const Bookmark& bookmark)
{
    auto bookmarksBackup = m_bookmarks;

    try {
        m_managementDatabase->Transaction();

        m_managementDatabase->GetBookmarkTable().Save(bookmark);

#ifdef USE_INSERT_OR_ASSIGN
        m_bookmarks.insert_or_assign(bookmark.bookmarkID, bookmark);
#else
        m_bookmarks[bookmark.bookmarkID] = bookmark;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_bookmarks = bookmarksBackup;
        SPDLOG_ERROR("SaveBookmark is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::RemoveBookmarks(const std::vector<uuid_string>& bookmarkIDs)
{
    auto bookmarksBackup = m_bookmarks;

    try {
        m_managementDatabase->Transaction();

        for(auto& bookmarkID : bookmarkIDs) {
            m_managementDatabase->GetBookmarkTable().Remove(bookmarkID);
            m_bookmarks.erase(bookmarkID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_bookmarks = bookmarksBackup;
        SPDLOG_ERROR("RemoveBookmarks is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

bool DatabaseManager::IsExistEventRule(std::string& eventRuleID)
{
    try{

        auto itor = m_eventRules.find(eventRuleID);

        if(itor == m_eventRules.end()){
            return false;
        }

        return true;

    } catch (const std::exception& e) {
        SPDLOG_ERROR("IsExistEventRule is failed. Error={}", e.what());
        return false;
    }
}

ErrorCode DatabaseManager::SaveEventRule(EventRule &eventRule)
{
    auto eventRulesBackup = m_eventRules;

    try {
        m_managementDatabase->Transaction();

        m_managementDatabase->GetEventRuleTable().Save(eventRule);

#ifdef USE_INSERT_OR_ASSIGN
        m_eventRules.insert_or_assign(eventRule.eventRuleID, eventRule);
#else
        m_eventRules[eventRule.eventRuleID] = eventRule;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_eventRules = eventRulesBackup;
        SPDLOG_ERROR("SaveEventRule is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveEventRules(const std::vector<uuid_string> &eventRuleIDs)
{
    auto eventRulesBackup = m_eventRules;
    try {

        m_managementDatabase->Transaction();

        // SquenceLayout Item과 SequenceLayout을 삭제한다.
        for(auto& eventRuleID : eventRuleIDs) {
            m_managementDatabase->GetEventRuleTable().Remove(eventRuleID);
            m_eventRules.erase(eventRuleID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_eventRules = eventRulesBackup;
        SPDLOG_ERROR("RemoveEventRules is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveEventSchedule(EventSchedule &eventSchedule)
{
    auto eventSchedulesBackup = m_eventSchedules;

    try {
        m_managementDatabase->Transaction();

        m_managementDatabase->GetEventScheduleTable().Save(eventSchedule);

#ifdef USE_INSERT_OR_ASSIGN
        m_eventSchedules.insert_or_assign(eventSchedule.scheduleID, eventSchedule);
#else
        m_eventSchedules[eventSchedule.scheduleID] = eventSchedule;
#endif

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_eventSchedules = eventSchedulesBackup;
        SPDLOG_ERROR("SaveEventSchedule is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::RemoveEventSchedules(const std::vector<uuid_string> &eventScheduleIDs)
{
    auto eventSchedulesBackup = m_eventSchedules;
    try {

        m_managementDatabase->Transaction();

        for(auto& eventScheduleID : eventScheduleIDs) {
            m_managementDatabase->GetEventScheduleTable().Remove(eventScheduleID);
            m_eventSchedules.erase(eventScheduleID);
        }

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_eventSchedules = eventSchedulesBackup;
        SPDLOG_ERROR("RemoveEventSchedules is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::SaveEventEmail(std::shared_ptr<EventEmail> eventEmail)
{
    auto eventEmailBackup = m_eventEmail;

    try {
        m_managementDatabase->Transaction();

        // 2021.06.03.
        // ROW가 없으면 INSERT 해야하고, ROW가 있으면 DELETE 후 INSERT 하도록 한다. (Sender의 정보는 유일하다.)
        m_managementDatabase->GetEventEmailTable().Remove();

        m_managementDatabase->GetEventEmailTable().Save(eventEmail);

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_eventEmail = eventEmailBackup;
        SPDLOG_ERROR("SaveEventEmail is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::RemoveEventEmail()
{
    //auto eventEmailBackup = m_eventEmail;

    try {

        m_managementDatabase->Transaction();

        m_managementDatabase->GetEventEmailTable().Remove();

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        //m_eventEmail = eventEmailBackup;
        SPDLOG_ERROR("RemoveEventEmail is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::UpdateLdapSetting(const LdapSetting& ldapSetting)
{
    auto ldapBackup = m_ldapSetting;
    try {
        SPDLOG_DEBUG("DatabaseMaanager::UpdateLdapSetting={}", ldapSetting.serverUrl);
        m_managementDatabase->Transaction();

        m_managementDatabase->GetLdapTable().Remove();
        m_managementDatabase->GetLdapTable().Save(ldapSetting);

        m_ldapSetting = ldapSetting;

        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_ldapSetting = ldapBackup;
        SPDLOG_ERROR("SaveLdapSetting is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::UpdateStatistics(const Statistics &statistics)
{
    auto statisticsBackup = m_statistics;
    try {
        SPDLOG_DEBUG("DatabaseMaanager::UpdateStatistics=count:{}, time:{}", statistics.executionCount, statistics.executionMinutes);
        m_managementDatabase->Transaction();

        m_managementDatabase->GetStatisticsTable().Save(statistics);
        m_statistics = statistics;
        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_statistics = statisticsBackup;
        SPDLOG_ERROR("UpdateStatistics is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::ActivateLicense(const std::string &licenseKey)
{
    auto licenseInfoBackup = m_licenseInfo;
    LicenseInfo licenseInfo;
    licenseInfo.licenseKey = licenseKey;
    try {
        SPDLOG_DEBUG("DatabaseMaanager::ActivateLicense=key:{}", licenseKey);
        m_managementDatabase->Transaction();

        m_managementDatabase->GetLicenseTable().Save(licenseInfo);
        m_managementDatabase->Commit();

        m_managementDatabase->GetLicenseTable().Get(licenseInfo);
        m_licenseInfo = licenseInfo;

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_licenseInfo = licenseInfoBackup;
        SPDLOG_ERROR("ActivateLicense is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

Device::Device& DatabaseManager::GetDevice(const std::string &deviceID)
{
    static Device::Device emptyDevice;

    try {

        auto itor = m_devices.find(deviceID);

        if(itor != m_devices.end()) {
            return itor->second;
        }

    }  catch (const std::exception& e) {
        SPDLOG_ERROR("GetDevice is failed. DeviceID={}, Error={}", deviceID, e.what());
    }

    return emptyDevice;
}

Device::Device::Channel& DatabaseManager::GetChannel(const std::string &deviceID, const std::string &channelID)
{
    static Device::Device::Channel emptyChannel;

    try {

        auto& device = GetDevice(deviceID);

        auto itor = device.channels.find(channelID);

        if(itor != device.channels.end()) {
            return itor->second;
        }

    } catch (const std::exception& e) {
        SPDLOG_ERROR("GetChannel is failed. DeviceID={}, ChannelID={}, Error={}", deviceID, channelID, e.what());
    }

    return emptyChannel;
}


ErrorCode DatabaseManager::SaveDevice(const Device::Device &device)
{
    /* TODO :: check need to update?? */
    //bool needToDbUpdate = true;

    SPDLOG_DEBUG("DatabaseManager::SaveDevice ip:{} use:{}",
                 device.connectionInfo.host, device.connectionInfo.use);

    auto devicesBackup = m_devices;
    try {
        m_managementDatabase->Transaction();

        m_managementDatabase->GetDeviceTable().Save(device);

        for(auto& channel : device.channels) {
            m_managementDatabase->GetChannelTable().Save(channel.second);
        }

#ifdef USE_INSERT_OR_ASSIGN
        m_devices.insert_or_assign(device.deviceID, device);
#else
        m_devices[device.deviceID] = device;
#endif
        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_devices = devicesBackup;
        SPDLOG_ERROR("SaveDevice is failed (1). Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveDevices(const std::vector<Device::Device> &devices)
{
    auto devicesBackup = m_devices;
    try {

        m_managementDatabase->Transaction();

        for(auto& device : devices){
            SPDLOG_DEBUG("DatabaseManager::SaveDevices ip:{} use:{} dtls:{}",
                         device.connectionInfo.host, device.connectionInfo.use,
                         device.connectionInfo.usageDtlsMode);
            m_managementDatabase->GetDeviceTable().Save(device);

#ifdef USE_INSERT_OR_ASSIGN
            m_devices.insert_or_assign(device.deviceID, device);
#else
            m_devices[device.deviceID] = device;

            SPDLOG_DEBUG("DatabaseManager::SaveDevices2 ip:{} use:{}",
                         m_devices[device.deviceID].connectionInfo.host, m_devices[device.deviceID].connectionInfo.use);
#endif
        }
        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_devices = devicesBackup;
        SPDLOG_ERROR("SaveDevices is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveChannel(const Device::Device::Channel& channel)
{
    auto devicesBackup = m_devices;

    SPDLOG_INFO("SaveChannel device{} channel", channel.deviceID, channel.channelID);
    auto device = m_devices.find(channel.deviceID);
    if(device != m_devices.end())
    {
        auto ch = device->second.channels.find(channel.channelID);
        if(ch != device->second.channels.end())
        {
            try {
                m_managementDatabase->Transaction();

                m_managementDatabase->GetChannelTable().Save(channel);

#ifdef USE_INSERT_OR_ASSIGN
                m_devices[channel.deviceID].channels.insert_or_assign(channel.channelID, channel);
#else
                m_devices[channel.deviceID].channels[channel.channelID] = channel;
#endif
                m_managementDatabase->Commit();
            }  catch (const ManagementDatabaseException& e) {
                m_managementDatabase->Rollback();
                m_devices = devicesBackup;
                SPDLOG_ERROR("SaveDevice is failed (2). Error={}", e.what());
                return Wisenet::ErrorCode::ManagementDatabaseError;
            }
        }
    }
    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveChannels(const std::vector<Device::Device::Channel>& channels,
                                        std::map<uuid_string,UserGroup>& changedUserRoles,
                                        std::map<uuid_string, Layout>& changedLayouts,
                                        std::vector<uuid_string> removedBookmarks)
{
    auto devicesBackup = m_devices;
    auto userRolesBackup = m_userGroups;
    auto layoutsBackup = m_layouts;
    auto bookmarksBackup = m_bookmarks;

    std::set<uuid_string> changedUserRoleIDs;
    std::set<uuid_string> changedLayoutIDs;
    std::vector<uuid_string> removedBookmarkIDs;

    try {
        m_managementDatabase->Transaction();

        // channel table update
        for(auto& channel : channels){
            auto device = m_devices.find(channel.deviceID);
            if(device != m_devices.end())
            {
                auto ch = device->second.channels.find(channel.channelID);
                if(ch != device->second.channels.end())
                {
                    m_managementDatabase->GetChannelTable().Save(channel);

#ifdef USE_INSERT_OR_ASSIGN
                    m_devices[channel.deviceID].channels.insert_or_assign(channel.channelID, channel);
#else
                    m_devices[channel.deviceID].channels[channel.channelID] = channel;
#endif
                }
            }

            if(channel.use == false)
            {
                m_managementDatabase->GetUserGroupResourceTable().RemoveByChannelID(channel.deviceID, channel.channelID, changedUserRoleIDs);
                m_managementDatabase->GetLayoutItemTable().RemoveByChannelID(channel.deviceID, channel.channelID, changedLayoutIDs);
                m_managementDatabase->GetBookmarkTable().RemoveByChannelID(channel.deviceID, channel.channelID, removedBookmarkIDs);
                m_managementDatabase->GetGroupMappingTable().RemoveChannel(channel.deviceID + "_" + channel.channelID);

                // 지워진 bookmark
                for(auto& bookmarkID : removedBookmarkIDs) {
                    m_bookmarks.erase(bookmarkID);
                    removedBookmarks.push_back(bookmarkID);
                }
            }
        }

        //Update event를 발생시킬 usergroup, layout
        //변경된 User Role리스트를 업데이트 한다.
        for(auto changedUserRoleID : changedUserRoleIDs) {
            auto& userRole = m_userGroups[changedUserRoleID];
            if (userRole.assignedResource) {
                auto& assignedResource = userRole.assignedResource.value();
                assignedResource.devices.clear();
                m_managementDatabase->GetUserGroupResourceTable().GetDevices(changedUserRoleID, assignedResource.devices);
                changedUserRoles.emplace(changedUserRoleID, userRole);
            }
        }

        //변경된 Layout Item 리스트를 업데이트 한다.
        for(auto changedLayoutID : changedLayoutIDs) {
            auto& layout =  m_layouts[changedLayoutID];
            layout.layoutItems.clear();
            m_managementDatabase->GetLayoutItemTable().Get(changedLayoutID, layout.layoutItems);
            changedLayouts.emplace(changedLayoutID, layout);
        }

        m_managementDatabase->Commit();
    }
    catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_devices = devicesBackup;
        m_userGroups = userRolesBackup;
        m_layouts = layoutsBackup;
        m_bookmarks = bookmarksBackup;
        SPDLOG_ERROR("SaveDevice is failed (3). Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }
    return Wisenet::ErrorCode::NoError;
}

std::string DatabaseManager::GetRealProfileNumber(std::string deviceID, std::string channelID, std::string profileID)
{
    std::string ret= "";
    auto device = m_deviceProfileInfos.find(deviceID);
    if(device != m_deviceProfileInfos.end()){
        auto channel = device->second.videoProfiles.find(channelID);
        if(channel != device->second.videoProfiles.end()){
            auto profile = channel->second.find(profileID);
            if(profile != channel->second.end()){
                return profile->second.profileID;
            }
        }
    }
    return ret;
}

void DatabaseManager::UpdateDeviceLatestEventStatus(const uuid_string& deviceId, const std::string& key, const bool& value)
{
    if(m_deviceLatestEventStatus.find(deviceId) == m_deviceLatestEventStatus.end())
    {
        std::map<std::string, bool> pair;
        pair.emplace(key, value);
        m_deviceLatestEventStatus.emplace(deviceId, pair);
    }
    else
    {
        m_deviceLatestEventStatus[deviceId].emplace(key, value);
    }
}

std::map<uuid_string, Device::DeviceProfileInfo>& DatabaseManager::GetDeviceProfile()
{
    return m_deviceProfileInfos;
}

ErrorCode DatabaseManager::SaveCloudInformation(const GetCloudInformationResponse &cloud)
{
    auto cloudBackup = m_cloud;
    SPDLOG_INFO("SaveCloudInformation id={}, userName={} ", cloud.cloudID, cloud.userName);
    try {

        m_managementDatabase->Transaction();

        m_managementDatabase->GetCloudTable().Save(cloud);

        m_managementDatabase->Commit();

        m_cloud = cloud;

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        m_cloud = cloudBackup;
        SPDLOG_ERROR("SaveCloudInformation is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

/*
void DatabaseManager::GetDashboardData(DashboardData& dashboardData)
{
    int powerErrorCount = 0;
    int diskErrorCount = 0;
    int overloadErrorCount = 0;
    int connectionErrorCount = 0;
    int fanErrorCount = 0;
    int disconnectedCameraCount = 0;

    for(auto& kv : m_devicesStatus) {
        if(!kv.second.isGood())
            connectionErrorCount++;

        for(auto& ckv : kv.second.channelsStatus){
            if(!ckv.second.isGood())
                disconnectedCameraCount++;
        }
    }

    for(auto& kv : m_deviceLatestEventStatus){

        for(auto& eventStatus : kv.second)
        {
            if(eventStatus.second)
            {
                if(0 == eventStatus.first.compare("SystemEvent.DualSMPSFail"))
                    powerErrorCount++;

                if(0 == eventStatus.first.compare("SystemEvent.CpuOverload") || 0 == eventStatus.first.compare("SystemEvent.MemoryError") ||
                        0 == eventStatus.first.compare("SystemEvent.NetCamTrafficOverFlow") || 0 == eventStatus.first.compare("SystemEvent.NetTxTrafficOverflow") ||
                        0 == eventStatus.first.compare("SystemEvent.VPUError"))
                    overloadErrorCount++;

                if(0 == eventStatus.first.compare("SystemEvent.HDDError") || 0 == eventStatus.first.compare("SystemEvent.HDDFail") || 0 == eventStatus.first.compare("SystemEvent.HDDFull") ||
                        0 == eventStatus.first.compare("SystemEvent.HDDNone") || 0 == eventStatus.first.compare("SystemEvent.RAIDDegrade") || 0 == eventStatus.first.compare("SystemEvent.RAIDFail") ||
                        0 == eventStatus.first.compare("SystemEvent.RAIDRebuildStart") || 0 == eventStatus.first.compare("SystemEvent.RecordFiltering") || 0 == eventStatus.first.compare("SystemEvent.RecordingError") ||
                        0 == eventStatus.first.compare("SystemEvent.iSCSIDisconnect") || 0 == eventStatus.first.compare("SystemEvent.SDFail") || 0 == eventStatus.first.compare("SystemEvent.SDFull"))
                    diskErrorCount++;

                if(0 == eventStatus.first.compare("SystemEvent.LeftFanError") || 0 == eventStatus.first.compare("SystemEvent.RightFanError") || 0 == eventStatus.first.compare("SystemEvent.CPUFanError"))
                    fanErrorCount++;
            }
        }
    }

    dashboardData.powerErrorCount = powerErrorCount;
    dashboardData.diskErrorCount = diskErrorCount;
    dashboardData.overloadErrorCount = overloadErrorCount;
    dashboardData.connectionErrorCount = connectionErrorCount;
    dashboardData.fanErrorCount = fanErrorCount;
    dashboardData.disconnectedCameraCount = disconnectedCameraCount;
}
*/

ErrorCode DatabaseManager::SaveDeviceProfileInfo(const Device::DeviceProfileInfo &deviceProfileInfo)
{
#ifdef USE_INSERT_OR_ASSIGN
    m_deviceProfileInfos.insert_or_assign(deviceProfileInfo.deviceID, deviceProfileInfo);
#else
    m_deviceProfileInfos[deviceProfileInfo.deviceID] = deviceProfileInfo;
#endif

    return Wisenet::ErrorCode::NoError;
}

ErrorCode DatabaseManager::SaveDeviceStatus(const Device::DeviceStatus &deviceStatus)
{
#ifdef USE_INSERT_OR_ASSIGN
    m_devicesStatus.insert_or_assign(deviceStatus.deviceID, deviceStatus);
#else
    m_devicesStatus[deviceStatus.deviceID] = deviceStatus;
#endif
    return Wisenet::ErrorCode::NoError;
}


ErrorCode DatabaseManager::RemoveDevices(const std::vector<uuid_string>& deviceIDs,
                                         std::map<uuid_string,UserGroup>& changedUserRoles,
                                         std::map<uuid_string, Layout>& changedLayouts,
                                         std::vector<uuid_string>& removedBookmarks)
{
    auto devicesBackup = m_devices;
    auto deviceProfileInfosBackup = m_deviceProfileInfos;
    auto userRolesBackup = m_userGroups;
    auto layoutsBackup = m_layouts;
    auto bookmarksBackup = m_bookmarks;

    try {
        m_managementDatabase->Transaction();

        //1. Device & Channel 삭제, mapping 정리
        for(auto& deviceID : deviceIDs) {

            // mapping 삭제
            for(auto& kvp : m_devices[deviceID].channels)
            {
                auto& channel = kvp.second;
                m_managementDatabase->GetGroupMappingTable().RemoveChannel(channel.deviceID + "_" + channel.channelID);
            }

            m_managementDatabase->GetDeviceTable().Remove(deviceID);
            m_managementDatabase->GetChannelTable().Remove(deviceID);
            m_devices.erase(deviceID);
            SPDLOG_WARN("DatabaseManager::RemoveDevices, deviceID={}", deviceID);
        }

        //2. User Role Resource에서 삭제.
        std::set<uuid_string> changedUserRoleIDs;
        for(auto& deviceID : deviceIDs) {
            m_managementDatabase->GetUserGroupResourceTable().RemoveByParentID(deviceID, changedUserRoleIDs);
            m_deviceLatestEventStatus.erase(deviceID);
        }

        //변경된 User Role리스트를 업데이트 한다.
        for(auto changedUserRoleID : changedUserRoleIDs) {
            auto& userRole = m_userGroups[changedUserRoleID];
            if (userRole.assignedResource) {
                auto& assignedResource = userRole.assignedResource.value();
                assignedResource.devices.clear();
                m_managementDatabase->GetUserGroupResourceTable().GetDevices(changedUserRoleID, assignedResource.devices);
                changedUserRoles.emplace(changedUserRoleID, userRole);
            }
        }

        //3. Layout Item 에서 삭제.
        std::set<uuid_string> changedLayoutIDs;
        for(auto& deviceID : deviceIDs) {
            m_managementDatabase->GetLayoutItemTable().RemoveByParentID(deviceID,changedLayoutIDs);
        }

        //변경된 Layout Item 리스트를 업데이트 한다.
        for(auto changedLayoutID : changedLayoutIDs) {
            auto& layout =  m_layouts[changedLayoutID];
            layout.layoutItems.clear();
            m_managementDatabase->GetLayoutItemTable().Get(changedLayoutID, layout.layoutItems);
            changedLayouts.emplace(changedLayoutID, layout);
        }

        for(auto& deviceID : deviceIDs) {
            m_deviceProfileInfos.erase(deviceID);
        }

        SPDLOG_ERROR("DatabaseManager::RemoveDevices -- prev bookmark");

        //5. Bookmark 삭제
        std::vector<uuid_string> removedBookmarkIDs;
        for(auto& deviceID : deviceIDs) {
            m_managementDatabase->GetBookmarkTable().RemoveByDeviceID(deviceID, removedBookmarkIDs);
            for(auto& bookmarkID : removedBookmarkIDs) {
                m_bookmarks.erase(bookmarkID);
                removedBookmarks.push_back(bookmarkID);
            }
        }
        SPDLOG_ERROR("DatabaseManager::RemoveDevices -- post bookmark");


        m_managementDatabase->Commit();

    } catch (const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        changedUserRoles.clear();
        changedLayouts.clear();
        m_devices = devicesBackup;
        m_deviceProfileInfos = deviceProfileInfosBackup;
        m_userGroups = userRolesBackup;
        m_layouts = layoutsBackup;
        SPDLOG_ERROR("RemoveDevice is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}


inline void _fillChannelInfoFromDB(const Device::Device::Channel &dbSourceChannel,
                                   const Device::DeviceProfileInfo &deviceProfileInfo,
                                   Device::Device::Channel &destChannel)
{
    destChannel.name = dbSourceChannel.name;
    destChannel.use = dbSourceChannel.use;
    destChannel.highProfile = dbSourceChannel.highProfile;
    destChannel.lowProfile = dbSourceChannel.lowProfile;
    destChannel.recordingProfile = dbSourceChannel.recordingProfile;
    destChannel.ptzEnable = dbSourceChannel.ptzEnable;
    destChannel.fisheyeSettings.fisheyeEnable = dbSourceChannel.fisheyeSettings.fisheyeEnable;
    destChannel.fisheyeSettings.fisheyeLensLocation = dbSourceChannel.fisheyeSettings.fisheyeLensLocation;
    destChannel.fisheyeSettings.fisheyeLensType = dbSourceChannel.fisheyeSettings.fisheyeLensType;
    destChannel.hasDBInfo = true;

    auto itr = deviceProfileInfo.videoProfiles.find(dbSourceChannel.channelID);
    if(itr !=  deviceProfileInfo.videoProfiles.end()){
        auto hitr = itr->second.find(dbSourceChannel.highProfile);
        if(hitr != itr->second.end()){
            auto profile = hitr->second;
            destChannel.highResolution = profile.resolution;
            destChannel.highFramerate = profile.framerate;
            destChannel.highCodecType = profile.codecType;
        }
        auto litr = itr->second.find(dbSourceChannel.lowProfile);
        if(litr != itr->second.end()){
            auto profile = litr->second;
            destChannel.lowResolution = profile.resolution;
            destChannel.lowFramerate = profile.framerate;
            destChannel.lowCodecType = profile.codecType;
        }
        auto ritr = itr->second.find(dbSourceChannel.recordingProfile);
        if(ritr != itr->second.end()){
            auto profile = ritr->second;
            destChannel.recordingResolution = profile.resolution;
            destChannel.recordingFramerate = profile.framerate;
            destChannel.recordingCodecType = profile.codecType;
        }
        SPDLOG_DEBUG("_fillChannelInfoFromDB channel={} high={}-{}*{} low={}-{}*{}",
                     dbSourceChannel.channelID,
                     dbSourceChannel.highProfile, destChannel.highResolution.width, destChannel.highResolution.height,
                     dbSourceChannel.lowProfile, destChannel.lowResolution.width, destChannel.lowResolution.height);
    }

}


void DatabaseManager::FillDeviceInfoFromDB(Device::Device &retDevice, Device::DeviceProfileInfo& deviceProfileInfo)
{
    auto itr = m_devices.find(retDevice.deviceID);
    if (itr == m_devices.end()) {
        SPDLOG_WARN("Can not find device id to update device name, deviceID={}", retDevice.deviceID);
        return;
    }
    auto& sourceDevice = itr->second;
    retDevice.name = sourceDevice.name;
    retDevice.hasDBInfo = true;

    for(auto& kv : retDevice.channels) {
        auto& retChannel = kv.second;
        auto ctr = sourceDevice.channels.find(kv.first);
        if (ctr != sourceDevice.channels.end()) {
            auto& sourceChannel = ctr->second;
            _fillChannelInfoFromDB(sourceChannel, deviceProfileInfo, retChannel);
        }
    }
}


ErrorCode DatabaseManager::FillAndSaveChannelStatusFromDB(
        const uuid_string& deviceID,
        std::map<std::string, Device::ChannelStatus> &channelsStatus)
{
    // db내 device를 찾는다.
    auto ditr = m_devices.find(deviceID);
    if (ditr == m_devices.end()) {
        SPDLOG_WARN("Can not find device to update channel status, deviceID={}", deviceID);
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }
    auto& device = ditr->second;

    // db내 deviceStatus를 찾는다.
    auto itr = m_devicesStatus.find(deviceID);
    if (itr == m_devicesStatus.end()) {
        SPDLOG_WARN("Can not find deviceStatus to update channel status, deviceID={}", deviceID);
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }
    auto& deviceStatus = itr->second;

    auto pitr = m_deviceProfileInfos.find(deviceID);
    if(pitr == m_deviceProfileInfos.end()){
        SPDLOG_WARN("Can not find profileInfo to update channel status, deviceID={}", deviceID);
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }
    auto& deviceProfile = pitr->second;


    // 이벤트로 들어온 channelStatus를 확인한다.
    for(auto& kv : channelsStatus) {
        auto& chStatus = kv.second;
        // 채널상태 정보에 채널 구조체가 포함되어 있는 경우
        if (chStatus.channel.has_value()) {
            auto& destChannel = chStatus.channel.get();
            auto citr = device.channels.find(destChannel.channelID);
            if (citr != device.channels.end()) {
                auto& dbSrcChannel = citr->second;
                // 채널상태 정보에 채널정보가 있는 경우에는, DB에서 관리하는 정보를 source에 채운다.
                _fillChannelInfoFromDB(dbSrcChannel, deviceProfile, destChannel);

                // 다시 DB를 업데이트한다.
                dbSrcChannel = destChannel;
            }
        }
        // channelStatus 이벤트에 채널정보가 있으면 지우고 deviceStatus에 업데이트한다.
        auto sitr = deviceStatus.channelsStatus.find(kv.first);
        if (sitr != deviceStatus.channelsStatus.end()) {
            sitr->second = chStatus;
            sitr->second.channel = boost::none;
        }
        else {
            SPDLOG_WARN("Can not find channel status in DB, deviceID={}, channelID={}", deviceID, kv.first);
        }
    }

    return Wisenet::ErrorCode::NoError;
}

Wisenet::ErrorCode DatabaseManager::SetupService(const std::string &userId, const std::string &newPassword)
{
    // 1. Service가 설정된 것이 있는지 확인.
    if(0 != m_service.serviceID.compare("None")){
        SPDLOG_ERROR("Already has a service.");
        return Wisenet::ErrorCode::InvalidRequest;
    }

    try {

        // 2. Service 테이블 추가.
        auto service = m_service;
        service.serviceID = userId;

        m_managementDatabase->Transaction();
        m_managementDatabase->GetServiceTable().Save(service);

        // 3. User Group테이블 추가. (admin)
        UserGroup userGroup;
        userGroup.userGroupID = boost::uuids::to_string(boost::uuids::random_generator()());
        userGroup.name = "administrator";
        userGroup.userPermission.playback = true;
        userGroup.userPermission.exportVideo = true;
        userGroup.userPermission.ptzControl = true;
        userGroup.userPermission.deviceControl = true;
        userGroup.userPermission.localRecording = true;
        userGroup.userPermission.audio = true;
        userGroup.userPermission.mic = true;

        userGroup.isAdminGroup = true;
        userGroup.accessAllResources = true;

        m_managementDatabase->GetUserGroupTable().Save(userGroup);
        m_userGroups.emplace(userGroup.userGroupID, userGroup);

        User user;
        user.userID = userId;
        user.loginID = "admin";
        user.userGroupID = userGroup.userGroupID;
        user.userType = User::UserType::Owner;
        user.password = newPassword;
        user.name = "";
        user.description = "";
        m_managementDatabase->GetUserTable().Save(user);

        m_managementDatabase->Commit();

        m_service = service;
        m_users.emplace(user.userID, user);

    } catch(const ManagementDatabaseException& e) {
        m_managementDatabase->Rollback();
        SPDLOG_ERROR("SetupService() is failed. Error={}", e.what());
        return Wisenet::ErrorCode::ManagementDatabaseError;
    }

    return Wisenet::ErrorCode::NoError;
}

bool DatabaseManager::Load()
{
    try {

        //0. Cloud
        m_managementDatabase->GetCloudTable().Get(m_cloud);

        //1. Service
        m_managementDatabase->GetServiceTable().Get(m_service);

        // serviceID가 없는 경우 default value로 업데이트.
        if(m_service.serviceID.empty()) {
            m_service.serviceID = "None";
            m_service.serviceType = m_serviceType;
            m_service.version = m_version;
            m_service.osInfo.platform = QSysInfo::prettyProductName().toStdString() + "_" + QSysInfo::currentCpuArchitecture().toStdString();
            m_service.osInfo.osVersion = QSysInfo::kernelVersion().toStdString();
        }

        //2. Device
        m_managementDatabase->GetDeviceTable().GetAll(m_devices);


        //3. Channel
        for(auto& device : m_devices) {
            m_managementDatabase->GetChannelTable().Get(device.second.deviceID, device.second.channels);

            // DeviceStatus는 disconnect 상태로 기본값을 생성한다.
            Device::DeviceStatus deviceStatus;
            deviceStatus.deviceID = device.second.deviceID;
            for(auto& channel : device.second.channels) {
                Device::ChannelStatus channelStatus;
                channelStatus.deviceID = channel.second.deviceID;
                channelStatus.channelID = channel.second.channelID;
                deviceStatus.channelsStatus.emplace(channelStatus.channelID, channelStatus);
            }
            m_devicesStatus.emplace(deviceStatus.deviceID, deviceStatus);
        }

        //4. User
        m_managementDatabase->GetUserTable().GetAll(m_users);

        SPDLOG_DEBUG("DatabaseManager::Load() user size:{}", m_users.size());


        //5. UserGroup
        m_managementDatabase->GetUserGroupTable().GetAll(m_userGroups);

        //6. UserGroupResource
        for(auto& userGroup : m_userGroups) {
            UserGroup::Resource resource;
            m_managementDatabase->GetUserGroupResourceTable().Get(userGroup.second.userGroupID, resource);
            userGroup.second.assignedResource.emplace(resource);
        }

        //7. Group & mapping
        m_managementDatabase->GetGroupTable().GetAll(m_groups);

        std::map<uuid_string, std::map<std::string, Group::MappingItem>> mapping;
        m_managementDatabase->GetGroupMappingTable().GetAll(mapping, m_channelGroupMappings);
        for(auto& group : m_groups) {
            if(mapping.find(group.first) != mapping.end()) {
                group.second.mapping = mapping[group.first];
            }
        }

        //8. Layout
        m_managementDatabase->GetLayoutTable().GetAll(m_layouts);

        //9. LayoutItem
        for(auto& layout : m_layouts) {
            m_managementDatabase->GetLayoutItemTable().Get(layout.second.layoutID, layout.second.layoutItems);
        }

        //10. Sequence
        m_managementDatabase->GetSequenceTable().GetAll(m_sequenceLayouts);

        //11. SequenceItem
        for(auto& sequence : m_sequenceLayouts) {
            m_managementDatabase->GetSequenceItemTable().Get(sequence.second.sequenceLayoutID,sequence.second.sequenceLayoutItems);
        }

        //12. WebPage
        m_managementDatabase->GetWebPageTable().GetAll(m_webpages);

        //13. Bookmark
        m_managementDatabase->GetBookmarkTable().GetAll(m_bookmarks);

        //13. EventRule
        m_managementDatabase->GetEventRuleTable().GetAll(m_eventRules);

        //14. EventSchedule
        m_managementDatabase->GetEventScheduleTable().GetAll(m_eventSchedules);

        //15. EventEmail
        m_eventEmail = std::make_shared<EventEmail>();
        m_managementDatabase->GetEventEmailTable().GetAll(m_eventEmail);

        //16. ServiceSettings
        m_managementDatabase->GetServiceSettingsTable().Get(m_serviceSettings.settings);
        SetLogRetentionTimeMsec(); // Msec값으로 변환해 놓는다.

        //17. LDAP
        m_managementDatabase->GetLdapTable().GetLdap(m_ldapSetting);

        //18. Statistics
        m_managementDatabase->GetStatisticsTable().Get(m_statistics);

        //19. License
        m_managementDatabase->GetLicenseTable().Get(m_licenseInfo);

    }catch(const ManagementDatabaseException& e) {
        SPDLOG_ERROR("Failed to load database. Error = {}", e.what());
        ClearAll();
        return false;
    }
    return true;
}

void DatabaseManager::ClearAll()
{
    m_userGroups.clear();
    m_users.clear();
    m_groups.clear();
    m_layouts.clear();
    m_sequenceLayouts.clear();
    m_webpages.clear();
    m_bookmarks.clear();
    m_devices.clear();
    m_eventRules.clear();
    m_eventSchedules.clear();
    //m_eventEmail.clear();
    m_deviceProfileInfos.clear();
    m_service = {};
}

void DatabaseManager::SetLogRetentionTimeMsec()
{
    auto itor = m_serviceSettings.settings.find(SettingsKey::LogRetensionTimeDay);

    if(itor == m_serviceSettings.settings.end()){
        //설정이 없으므로 디폴트로 30일 설정한다.
        SPDLOG_ERROR("No exists LogRetensionTimeDay setting. LogRetensionTimeDay setting is 2592000000.");
        m_logRetensionTimeMsec = 2592000000;
        return;
    }

    try
    {
        int64_t timeDay = boost::lexical_cast<int64_t>(itor->second);
        m_logRetensionTimeMsec = timeDay * 86400000;

        SPDLOG_INFO("LogRetensionTimeDay setting is {}.", m_logRetensionTimeMsec);

    }catch(std::exception& e){
        SPDLOG_ERROR("Failed to cast LogRetentionTimeDay. LogRetensionTimeDay setting is 2592000000. Error = {}", e.what());
        m_logRetensionTimeMsec = 2592000000;
    }

}

bool CompressDatabase(const QString & path, const QString& fileName)
{
    SPDLOG_DEBUG("[DatabaseManager] CompressDatabase{} {}",path.toStdString(), fileName.toStdString());
    QFile fi(path + "/WisenetViewer/" + ManagementDatabase::FILENAME); // this is your input file
    QFile fo(path + "/" + fileName); // this is your compressed output file

    if (fi.open(QFile::ReadOnly) && fo.open(QFile::WriteOnly))
    {
        int compress_level = 1; // compression level
        auto resultSize = fo.write(qCompress(fi.readAll(), compress_level)); // read input, compress and write to output is a single line of code
        fi.close();
        fo.close();
        SPDLOG_INFO("[DatabaseManager] CompressDatabase= resultSize:{}",resultSize);
    }
    RemoveDirectory(path + "/WisenetViewer");
    return true;
}

bool UnCompressDatabase(const QString & source, const QString & dest)
{
    SPDLOG_DEBUG("[DatabaseManager] UnCompressDatabase start{}",source.toStdString());
    QFile fi(source);
    QFile fo(dest +"/" + ManagementDatabase::FILENAME);
    if (fi.open(QFile::ReadOnly) && fo.open(QFile::WriteOnly)){
        QByteArray compressed = fi.readAll();
        QByteArray uncompressed = qUncompress(compressed); // HERE I GET: qUncompress: Input data is corrupted
        SPDLOG_INFO("[DatabaseManager] UnCompresed= {}",uncompressed.size());
        auto resultSize = fo.write(qUncompress(compressed));
        fi.close();
        fo.close();

        SPDLOG_INFO("[DatabaseManager] UnCompressDatabase resultSize:{} / {}",resultSize, uncompressed.size());

    }
    SPDLOG_DEBUG("[DatabaseManager] UnCompressDatabase end{}",source.toStdString());

    return true;
}

Wisenet::ErrorCode DatabaseManager::BackupService(const std::string &path_, std::string &destPath)
{
    QString path = QString::fromUtf8(path_.c_str());
    SPDLOG_DEBUG("DatabaseManager::BackupService:{} ",path.toStdString());
    Wisenet::ErrorCode res = Wisenet::ErrorCode::NoError;
    QString fileName = "WisenetViewer_" + QDate::currentDate().toString(QString("yyyyMMdd")) + "_" + QTime::currentTime().toString(QString("hhmmss"))+".wdb";
    QString tempDirectory = path + "/WisenetViewer";

    QString dbPath = m_path;
    SPDLOG_INFO("DatabaseManager::BackupService:{} ",fileName.toStdString());
    destPath = (path + "/" + fileName).toStdString();
    RemoveDirectory(tempDirectory);
    QDir().mkdir(tempDirectory);
    if(!CopyDBFile(dbPath, tempDirectory)
            || !m_managementDatabase->OpenClose(tempDirectory)
            || !CompressDatabase(path, fileName)){
        res =  Wisenet::ErrorCode::BackupServiceError;
        destPath = "";
    }
    SPDLOG_INFO("DatabaseManager::BackupService result:{} ",res);
    return res;
}

Wisenet::ErrorCode DatabaseManager::RestoreService(const std::string &path)
{
    Wisenet::ErrorCode res = Wisenet::ErrorCode::NoError;
    QString restoreDirectory = m_path + "/Restore";
    QDir().mkdir(restoreDirectory);
    if(!UnCompressDatabase(QString::fromUtf8(path.c_str()), restoreDirectory))
        res =  Wisenet::ErrorCode::RestoreServiceError;
    else
        m_bReset = true;
    SPDLOG_INFO("DatabaseManager::RestoreService:{} {} res={}",path, m_path.toStdString(), (int)res);
    return res;
}

Wisenet::ErrorCode DatabaseManager::ResetService()
{
    Wisenet::ErrorCode res = Wisenet::ErrorCode::NoError;
    m_bReset = true;
    SPDLOG_DEBUG("DatabaseManager::ResetService End");
    return res;
}

}
}

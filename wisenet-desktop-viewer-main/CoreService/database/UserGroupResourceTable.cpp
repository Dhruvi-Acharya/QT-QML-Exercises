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
#include "UserGroupResourceTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"
#include <JsonArchiver.h>

namespace Wisenet
{
namespace Core
{

UserGroupResourceTable::UserGroupResourceTable(QSqlDatabase& database)
    :m_database(database)
{

}

void UserGroupResourceTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_USER_GROUP_RESOURCE"))) {
        SPDLOG_INFO("[T_USER_GROUP_RESOURCE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);
    QString queryString = "";
    queryString += "CREATE TABLE [T_USER_GROUP_RESOURCE](";
    queryString += "[USER_GROUP_RESOURCE_ID] INTEGER PRIMARY KEY AUTOINCREMENT";
    queryString += ",[USER_GROUP_ID] VARCHAR(40) NOT NULL";
    queryString += ",[ITEM_TYPE] INTEGER NOT NULL DEFAULT 0";
    queryString += ",[ITEM_ID] VARCHAR(40)";
    queryString += ",[PARENT_ID] VARCHAR(40)";
    queryString += ")";

    if(!query.exec(queryString)) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void UserGroupResourceTable::Get(const uuid_string &userGroupID, UserGroup::Resource &resource)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE USER_GROUP_ID=?");
    query.addBindValue(QString(userGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        UserGroupResourceType type = static_cast<UserGroupResourceType>(query.value(2).toInt());
        if(type == UserGroupResourceType::Camera) {
            std::string channelID = query.value(3).toString().toStdString();
            uuid_string deviceID = query.value(4).toString().toStdString();
            resource.devices[deviceID].channels.emplace(channelID);
        } else if(type == UserGroupResourceType::Webpage) {
            resource.webpages.emplace_back(query.value(3).toString().toStdString());
        } else if(type == UserGroupResourceType::Layout) {
            resource.layouts.emplace_back(query.value(3).toString().toStdString());
        }
    }
}

void UserGroupResourceTable::GetWebpages(const uuid_string &userGroupID, std::vector<uuid_string> &webpages)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE USER_GROUP_ID=? AND ITEM_TYPE=?");
    query.addBindValue(QString(userGroupID.c_str()));
    query.addBindValue(static_cast<int>(UserGroupResourceType::Webpage));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        webpages.emplace_back(query.value(3).toString().toStdString());
    }
}

void UserGroupResourceTable::GetDevices(const uuid_string &userGroupID,
                                        std::map<uuid_string, UserGroup::Resource::Device>& devices)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE USER_GROUP_ID=? AND ITEM_TYPE=?");
    query.addBindValue(QString(userGroupID.c_str()));
    query.addBindValue(static_cast<int>(UserGroupResourceType::Camera));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        std::string channelID = query.value(3).toString().toStdString();
        uuid_string deviceID = query.value(4).toString().toStdString();
        devices[deviceID].channels.emplace(channelID);
    }
}

void UserGroupResourceTable::GetLayouts(const uuid_string &userGroupID, std::vector<uuid_string> &layouts)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE USER_GROUP_ID=? AND ITEM_TYPE=?");
    query.addBindValue(QString(userGroupID.c_str()));
    query.addBindValue(static_cast<int>(UserGroupResourceType::Layout));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        layouts.emplace_back(query.value(3).toString().toStdString());
    }
}

void UserGroupResourceTable::Add(const uuid_string &userGroupID, const UserGroup::Resource &resource)
{
    QSqlQuery query(m_database);

    for (auto& device : resource.devices) {
        uuid_string deviceID = device.first;
        for (auto& channelID : device.second.channels) {
            QString queryString = "";
            queryString += "INSERT INTO [T_USER_GROUP_RESOURCE] ";
            queryString += "(USER_GROUP_ID,ITEM_TYPE,ITEM_ID,PARENT_ID) ";
            queryString += "VALUES (?, ?, ?, ?)";
            query.prepare(queryString);
            query.addBindValue(QString(userGroupID.c_str()));
            query.addBindValue(static_cast<int>(UserGroupResourceType::Camera));
            query.addBindValue(QString(channelID.c_str()));
            query.addBindValue(QString(deviceID.c_str()));

            if(!query.exec()) {
                throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
            }
        }
    }

    for(auto& webpage : resource.webpages){
        QString queryString = "";
        queryString += "INSERT INTO [T_USER_GROUP_RESOURCE] ";
        queryString += "(USER_GROUP_ID,ITEM_TYPE,ITEM_ID) ";
        queryString += "VALUES (?, ?, ?)";

        query.prepare(queryString);
        query.addBindValue(QString(userGroupID.c_str()));
        query.addBindValue(static_cast<int>(UserGroupResourceType::Webpage));
        query.addBindValue(QString(webpage.c_str()));

        if(!query.exec()) {
            throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
        }
    }

    for(auto& layout : resource.layouts) {
        SPDLOG_DEBUG("UserGroupResourceTable::Add layout :{}", layout.c_str());
        QString queryString = "";
        queryString += "INSERT INTO [T_USER_GROUP_RESOURCE] ";
        queryString += "(USER_GROUP_ID,ITEM_TYPE,ITEM_ID) ";
        queryString += "VALUES (?, ?, ?)";
        query.prepare(queryString);
        query.addBindValue(QString(userGroupID.c_str()));
        query.addBindValue(static_cast<int>(UserGroupResourceType::Layout));
        query.addBindValue(QString(layout.c_str()));

        if(!query.exec()) {
            throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
        }
    }
}

void UserGroupResourceTable::Remove(const uuid_string &userGroupID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_USER_GROUP_RESOURCE] WHERE USER_GROUP_ID=?");
    query.addBindValue(QString(userGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserGroupResourceTable::RemoveByItemID(const UserGroupResourceType& itemType, const std::string &itemID, std::set<uuid_string> &changedUserRoleIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE ITEM_TYPE=? AND ITEM_ID=?");
    query.addBindValue(static_cast<int>(itemType));
    query.addBindValue(QString(itemID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedUserRoleIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedUserRoleIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_USER_GROUP_RESOURCE] WHERE ITEM_TYPE=? AND ITEM_ID=?");
    query.addBindValue(static_cast<int>(itemType));
    query.addBindValue(QString(itemID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserGroupResourceTable::RemoveByParentID(const std::string &parentID, std::set<uuid_string> &changedUserRoleIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedUserRoleIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedUserRoleIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_USER_GROUP_RESOURCE] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserGroupResourceTable::RemoveByChannelID(const std::string &deviceId, const std::string channelId, std::set<uuid_string> &changedUserRoleIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_USER_GROUP_RESOURCE] WHERE PARENT_ID=? AND ITEM_ID=?");
    query.addBindValue(QString(deviceId.c_str()));
    query.addBindValue(QString(channelId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedUserRoleIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedUserRoleIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_USER_GROUP_RESOURCE] WHERE PARENT_ID=? AND ITEM_ID=?");
    query.addBindValue(QString(deviceId.c_str()));
    query.addBindValue(QString(channelId.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

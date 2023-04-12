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
#include "GroupMappingTable.h"

#include <QString>
#include <QVariant>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

GroupMappingTable::GroupMappingTable(QSqlDatabase& database)
    :m_database(database)
{

}

void GroupMappingTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_GROUP_MAPPING"))) {
        SPDLOG_INFO("[T_GROUP_MAPPING] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "CREATE TABLE [T_GROUP_MAPPING] ([GROUP_ID] VARCHAR(40), [CHANNEL_ID] TEXT, [ITEM_TYPE] INTEGER NOT NULL DEFAULT 0)";

    if(!query.exec(queryString))
    {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupMappingTable::GetAll(std::map<uuid_string, std::map<std::string, Group::MappingItem>>& groupChannelMapping, std::map<std::string, uuid_string>& channelGroupMapping)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_GROUP_MAPPING]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        uuid_string groupID = query.value(0).toString().toStdString();
        std::string channelID = query.value(1).toString().toStdString();
        int itemType = query.value(2).toInt();

        channelGroupMapping.emplace(channelID, groupID);

        Group::MappingItem item;
        item.id = channelID;
        item.mappingType = (Group::MappingType)itemType;

        if(groupChannelMapping.find(groupID) != groupChannelMapping.end())
        {
            groupChannelMapping[groupID].emplace(item.id, item);
        }
        else
        {
            std::map<std::string, Group::MappingItem> assigned;
            assigned.emplace(item.id, item);

            groupChannelMapping.emplace(groupID, assigned);
        }
    }
}

void GroupMappingTable::Get(const uuid_string& groupID, std::vector<Group::MappingItem>& mapping)
{
    QSqlQuery query(m_database);

    QString queryString = "SELECT * FROM [T_GROUP_MAPPING] WHERE GROUP_ID=?";

    query.prepare(queryString);
    query.addBindValue(QString(groupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {

        std::string channelID = query.value(1).toString().toStdString();
        int itemType = query.value(2).toInt();

        Group::MappingItem item;
        item.id = channelID;
        item.mappingType = (Group::MappingType)itemType;

        mapping.push_back(item);
    }
}

void GroupMappingTable::Add(const uuid_string& groupID, const std::string& channelID, const Group::MappingType type)
{
    QSqlQuery query(m_database);

    QString queryString = "INSERT OR REPLACE INTO [T_GROUP_MAPPING] \
                        (GROUP_ID,CHANNEL_ID,ITEM_TYPE) \
            VALUES (?, ?, ?)";

    query.prepare(queryString);
    query.addBindValue(QString(groupID.c_str()));
    query.addBindValue(QString(channelID.c_str()));
    query.addBindValue((int)type);

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupMappingTable::Update(const uuid_string& from, const uuid_string& to, const std::string& channelID)
{
    QSqlQuery query(m_database);

    QString queryString = "UPDATE [T_GROUP_MAPPING] \
                            SET GROUP_ID=? \
                WHERE GROUP_ID = ? AND CHANNEL_ID = ?";

    query.prepare(queryString);
    query.addBindValue(QString(to.c_str()));
    query.addBindValue(QString(from.c_str()));
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupMappingTable::Remove(const uuid_string& groupID, const std::string& channelID)
{
    QSqlQuery query(m_database);

    QString queryString = "DELETE FROM [T_GROUP_MAPPING] WHERE GROUP_ID = ? AND CHANNEL_ID = ?";

    query.prepare(queryString);
    query.addBindValue(QString(groupID.c_str()));
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupMappingTable::RemoveChannel(const std::string& channelID)
{
    QSqlQuery query(m_database);

    QString queryString = "DELETE FROM [T_GROUP_MAPPING] WHERE CHANNEL_ID = ?";

    query.prepare(queryString);
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}


void GroupMappingTable::RemoveAll(const uuid_string& groupID)
{
    QSqlQuery query(m_database);

    QString queryString = "DELETE FROM [T_GROUP_MAPPING] WHERE GROUP_ID = ?";

    query.prepare(queryString);
    query.addBindValue(QString(groupID.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

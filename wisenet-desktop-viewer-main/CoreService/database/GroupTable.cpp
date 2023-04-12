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
#include "GroupTable.h"

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

GroupTable::GroupTable(QSqlDatabase& database)
    :m_database(database)
{

}


void GroupTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_GROUP"))) {
        SPDLOG_INFO("[T_GROUP] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "CREATE TABLE [T_GROUP] (\
            [GROUP_ID] VARCHAR(40) PRIMARY KEY\
            ,[NAME] TEXT \
            ,[PARENT_GROUP_ID] VARCHAR(40)\
            )";

    if(!query.exec(queryString))
    {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupTable::GetAll(std::map<uuid_string, Group> &groups)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_GROUP]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Group group;
        group.groupID = query.value(0).toString().toStdString();
        group.name = query.value(1).toString().toStdString();
        group.parentGroupID = query.value(2).toString().toStdString();

        groups.emplace(group.groupID, group);
    }
}

void GroupTable::GetChildrenByBottomUpOrder(const uuid_string &groupID, std::vector<uuid_string>& children)
{
    QSqlQuery query(m_database);

    QString queryString = "SELECT * FROM [T_GROUP]\
                    WHERE PARENT_GROUP_ID=?";

    query.prepare(queryString);
    query.addBindValue(QString(groupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        uuid_string childId = query.value(0).toString().toStdString();

        GetChildrenByBottomUpOrder(childId, children);

        children.push_back(childId);
    }
}

void GroupTable::Save(const Group &group)
{
    QSqlQuery query(m_database);

    QString queryString = "INSERT OR REPLACE INTO [T_GROUP]\
            (GROUP_ID,NAME,PARENT_GROUP_ID)\
            VALUES (?, ?, ?)";

    query.prepare(queryString);

    query.addBindValue(QString(group.groupID.c_str()));
    query.addBindValue(QString(group.name.c_str()));
    query.addBindValue(QString(group.parentGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void GroupTable::Remove(const uuid_string &groupID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_GROUP] WHERE GROUP_ID=?");
    query.addBindValue(QString(groupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

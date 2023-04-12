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
#include "UserGroupTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

UserGroupTable::UserGroupTable(QSqlDatabase& database)
    :m_database(database)
{

}

void UserGroupTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_USER_GROUP"))) {
        SPDLOG_INFO("[T_USER_GROUP] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_USER_GROUP]( \
                [USER_GROUP_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY\
                ,[NAME] TEXT\
                ,[IS_ADMIN_GROUP]BOOLEAN NOT NULL DEFAULT 0 \
                ,[PLAYBACK]BOOLEAN NOT NULL DEFAULT 0 \
                ,[EXPORT_VIDEO]BOOLEAN NOT NULL DEFAULT 0 \
                ,[PTZ_CONTROL]BOOLEAN NOT NULL DEFAULT 0 \
                ,[DEVICE_CONTROL]BOOLEAN NOT NULL DEFAULT 0 \
                ,[CAPTURE_LOCAL_RECORDING]BOOLEAN NOT NULL DEFAULT 0 \
                ,[AUDIO]BOOLEAN NOT NULL DEFAULT 0 \
                ,[MIC]BOOLEAN NOT NULL DEFAULT 0 \
                ,[ACCESS_ALL_RESOURCE]BOOLEAN NOT NULL DEFAULT 0 \
                )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserGroupTable::GetAll(std::map<uuid_string, UserGroup>& userGroups)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_USER_GROUP]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        UserGroup userGroup;
        userGroup.userGroupID = query.value(0).toString().toStdString();
        userGroup.name = query.value(1).toString().toStdString();
        userGroup.isAdminGroup = query.value(2).toBool();
        userGroup.userPermission.playback = query.value(3).toBool();
        userGroup.userPermission.exportVideo = query.value(4).toBool();
        userGroup.userPermission.ptzControl = query.value(5).toBool();
        userGroup.userPermission.deviceControl = query.value(6).toBool();
        userGroup.userPermission.localRecording = query.value(7).toBool();
        userGroup.userPermission.audio = query.value(8).toBool();
        userGroup.userPermission.mic = query.value(9).toBool();
        userGroup.accessAllResources = query.value(10).toBool();

        userGroups.emplace(userGroup.userGroupID, userGroup);
    }
}

void UserGroupTable::Save(const UserGroup& userGroup)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_USER_GROUP] \
            (USER_GROUP_ID,NAME,IS_ADMIN_GROUP,PLAYBACK,EXPORT_VIDEO,PTZ_CONTROL,DEVICE_CONTROL,CAPTURE_LOCAL_RECORDING,AUDIO,MIC,ACCESS_ALL_RESOURCE) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(userGroup.userGroupID.c_str()));
    query.addBindValue(QString(userGroup.name.c_str()));
    query.addBindValue(userGroup.isAdminGroup);
    query.addBindValue(userGroup.userPermission.playback);
    query.addBindValue(userGroup.userPermission.exportVideo);
    query.addBindValue(userGroup.userPermission.ptzControl);
    query.addBindValue(userGroup.userPermission.deviceControl);
    query.addBindValue(userGroup.userPermission.localRecording);
    query.addBindValue(userGroup.userPermission.audio);
    query.addBindValue(userGroup.userPermission.mic);
    query.addBindValue(userGroup.accessAllResources);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserGroupTable::Remove(const uuid_string& userGroupID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_USER_GROUP] WHERE USER_GROUP_ID=?");
    query.addBindValue(QString(userGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

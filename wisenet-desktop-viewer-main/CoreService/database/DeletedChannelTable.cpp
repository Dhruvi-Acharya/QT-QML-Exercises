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
#include "DeletedChannelTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{

DeletedChannelTable::DeletedChannelTable(QSqlDatabase& database)
    :m_database(database)
{

}

void DeletedChannelTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_DELETED_CHANNEL"))) {
        SPDLOG_INFO("[T_DELETED_CHANNEL] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_DELETED_CHANNEL]([DEVICE_ID] VARCHAR(40) NOT NULL,[CHANNEL_ID] VARCHAR(40) NOT NULL,[CREATE_TIME] INTEGER ,[NAME] VARCHAR(128),PRIMARY KEY(DEVICE_ID,CHANNEL_ID))")) {
       throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeletedChannelTable::Get(const uuid_string &deviceID, std::map<std::string, DeletedDevice::Channel> &deletedChannels)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_DELETED_CHANNEL] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        DeletedDevice::Channel deletedChannel;
        deletedChannel.deviceID = query.value(0).toString().toStdString();
        deletedChannel.channelID = query.value(1).toString().toStdString();
        deletedChannel.name = query.value(3).toString().toStdString();

        deletedChannels.emplace(deletedChannel.channelID, deletedChannel);
    }
}

void DeletedChannelTable::Save(int64_t serviceUtcTimeMsec, const DeletedDevice::Channel &deletedChannel)
{
    QSqlQuery query(m_database);

    SPDLOG_INFO("[T_DELETED_CHANNEL] ChannelTable::Save device:{} -- channel:{}", deletedChannel.deviceID, deletedChannel.channelID);

    query.prepare("INSERT OR REPLACE INTO [T_DELETED_CHANNEL] (DEVICE_ID,CHANNEL_ID,CREATE_TIME,NAME) VALUES (?, ?, ?, ?)");
    query.addBindValue(QString(deletedChannel.deviceID.c_str()));
    query.addBindValue(QString(deletedChannel.channelID.c_str()));
    query.addBindValue(QVariant::fromValue(serviceUtcTimeMsec));
    query.addBindValue(QString(deletedChannel.name.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeletedChannelTable::RemoveTo(int64_t timeMsec)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_DELETED_CHANNEL] WHERE CREATE_TIME<=?");
    query.addBindValue(QVariant::fromValue(timeMsec));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}


}
}

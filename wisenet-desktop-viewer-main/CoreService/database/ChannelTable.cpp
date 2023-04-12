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
#include "ChannelTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

ChannelTable::ChannelTable(QSqlDatabase& database)
    :m_database(database)
{

}

void ChannelTable::CreateTable()
{

    if(m_database.tables().contains(QLatin1String("T_CHANNEL"))) {
        SPDLOG_INFO("[T_CHANNEL] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_CHANNEL]( \
                [DEVICE_ID] VARCHAR(40) NOT NULL\
                ,[CHANNEL_ID] VARCHAR(40) NOT NULL\
                ,[USE] BOOLEAN NULL DEFAULT 0\
                ,[NAME] TEXT\
                ,[HIGH_PROFILE_ID] TEXT\
                ,[LOW_PROFILE_ID] TEXT\
                ,[FISHEYE_DEWARP_ENABLE] BOOLEAN NULL DEFAULT 0\
                ,[PTZ_ENABLE] BOOLEAN NULL DEFAULT 0\
                ,[DB_FISHEYE_MOUNT] INTEGER\
                ,[FISHEYE_LENS_TYPE] VARCHAR(40)\
                ,PRIMARY KEY(DEVICE_ID,CHANNEL_ID)\
                   )")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void ChannelTable::Get(const uuid_string &deviceID, std::map<std::string , Device::Device::Channel> &channels)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_CHANNEL] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Device::Device::Channel channel;
        channel.deviceID = query.value(0).toString().toStdString();
        channel.channelID = query.value(1).toString().toStdString();
        channel.use = query.value(2).toBool();
        channel.name = query.value(3).toString().toStdString();
        channel.highProfile = query.value(4).toString().toStdString();
        channel.lowProfile = query.value(5).toString().toStdString();
        channel.fisheyeSettings.fisheyeEnable = query.value(6).toBool();
        channel.ptzEnable = query.value(7).toBool();
        channel.fisheyeSettings.fisheyeLensLocation = static_cast<FisheyeLensLocation>(query.value(8).toInt());
        channel.fisheyeSettings.fisheyeLensType = query.value(9).toString().toStdString();

        channels.emplace(channel.channelID, channel);
    }
}

void ChannelTable::Save(const Device::Device::Channel &channel)
{
    QSqlQuery query(m_database);

    SPDLOG_INFO("[T_CHANNEL] ChannelTable::Save deviceid[{}] -- name[{}] channel id[{}] use[{}]", channel.deviceID, channel.name, channel.channelID, channel.use);

    query.prepare("INSERT OR REPLACE INTO [T_CHANNEL] \
            (DEVICE_ID,CHANNEL_ID,USE,NAME,HIGH_PROFILE_ID,LOW_PROFILE_ID,FISHEYE_DEWARP_ENABLE,PTZ_ENABLE,DB_FISHEYE_MOUNT,FISHEYE_LENS_TYPE) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(channel.deviceID.c_str()));
    query.addBindValue(QString(channel.channelID.c_str()));
    query.addBindValue(channel.use);
    query.addBindValue(QString(channel.name.c_str()));
    query.addBindValue(QString(channel.highProfile.c_str()));
    query.addBindValue(QString(channel.lowProfile.c_str()));
    query.addBindValue(channel.fisheyeSettings.fisheyeEnable);
    query.addBindValue(channel.ptzEnable);
    query.addBindValue(static_cast<int>(channel.fisheyeSettings.fisheyeLensLocation));
    query.addBindValue(QString(channel.fisheyeSettings.fisheyeLensType.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void ChannelTable::Remove(const uuid_string &deviceID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_CHANNEL] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void ChannelTable::Remove(const uuid_string &deviceID, const std::string &channelID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_CHANNEL] WHERE DEVICE_ID=? AND CHANNEL_ID=?");
    query.addBindValue(QString(deviceID.c_str()));
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

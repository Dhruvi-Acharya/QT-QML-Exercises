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
#include "DeletedDeviceTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{

DeletedDeviceTable::DeletedDeviceTable(QSqlDatabase& database)
    :m_database(database)
{

}

void DeletedDeviceTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_DELETED_DEVICE"))) {
        SPDLOG_INFO("[T_DELETED_DEVICE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_DELETED_DEVICE]([DEVICE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY ,[CREATE_TIME] INTEGER ,[MODEL] VARCHAR(128),[MAC] VARCHAR(128),[NAME] VARCHAR(128))")) {
       throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeletedDeviceTable::GetAll(std::shared_ptr<std::map<uuid_string, DeletedDevice>>& deletedDevices)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_DELETED_DEVICE]")) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        DeletedDevice deletedDevice;
        deletedDevice.deviceID = query.value(0).toString().toStdString();
        deletedDevice.serviceUtcTimeMsec = query.value(1).toLongLong();
        deletedDevice.modelName = query.value(2).toString().toStdString();
        deletedDevice.macAddress = query.value(3).toString().toStdString();
        deletedDevice.name = query.value(4).toString().toStdString();

        deletedDevices->emplace(deletedDevice.deviceID,deletedDevice);
    }
}

void DeletedDeviceTable::Save(const DeletedDevice& deletedDevice)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_DELETED_DEVICE] (DEVICE_ID,CREATE_TIME,MODEL,MAC,NAME) VALUES (?, ?, ?, ?, ?)");

    query.addBindValue(QString(deletedDevice.deviceID.c_str()));
    query.addBindValue(QVariant::fromValue(deletedDevice.serviceUtcTimeMsec));
    query.addBindValue(QString(deletedDevice.modelName.c_str()));
    query.addBindValue(QString(deletedDevice.macAddress.c_str()));
    query.addBindValue(QString(deletedDevice.name.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DeletedDeviceTable::RemoveTo(int64_t timeMsec)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_DELETED_DEVICE] WHERE CREATE_TIME<=?");
    query.addBindValue(QVariant::fromValue(timeMsec));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

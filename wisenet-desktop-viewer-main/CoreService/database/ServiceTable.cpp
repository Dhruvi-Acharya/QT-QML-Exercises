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
#include "ServiceTable.h"

#include <QSysInfo>
#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

ServiceTable::ServiceTable(QSqlDatabase& database)
    :m_database(database)
{

}

void ServiceTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_SERVICE"))) {
        SPDLOG_INFO("[T_SERVICE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_SERVICE]( \
                [SERVICE_ID] VARCHAR(40) NOT NULL PRIMARY KEY\
                ,[TYPE] INTEGER NOT NULL DEFAULT 0\
                ,[VERSION] VARCHAR(40)\
                ,[OS_PLATFORM] VARCHAR(128)\
                ,[OS_VERSION] VARCHAR(128)\
                   )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void ServiceTable::Get(GetServiceInformationResponse& service)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_SERVICE]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        service.serviceID = query.value(0).toString().toStdString();
        service.serviceType = static_cast<CoreServicetype>(query.value(1).toInt());
        service.version = query.value(2).toString().toStdString();
        service.osInfo.platform = query.value(3).toString().toStdString();
        service.osInfo.osVersion = query.value(4).toString().toStdString();
    }
}

void ServiceTable::Save(const GetServiceInformationResponse &service)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_SERVICE] \
            (SERVICE_ID,TYPE,VERSION,OS_PLATFORM,OS_VERSION) \
            VALUES (?, ?, ?, ?, ?)");
    query.addBindValue(QString(service.serviceID.c_str()));
    query.addBindValue(static_cast<int>(service.serviceType));
    query.addBindValue(QString(service.version.c_str()));
    query.addBindValue(QString(service.osInfo.platform.c_str()));
    query.addBindValue(QString(service.osInfo.osVersion.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void ServiceTable::Remove(uuid_string &serviceID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_SERVICE] WHERE SERVICE_ID=?");
    query.addBindValue(QString(serviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

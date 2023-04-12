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
#include "CloudTable.h"

#include <QSysInfo>
#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

CloudTable::CloudTable(QSqlDatabase& database, QString& key)
    :m_database(database)
{
    m_crypto.Init(key);
}

void CloudTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_CLOUD"))) {
        SPDLOG_INFO("[T_CLOUD] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_CLOUD]( \
                [CLOUD_ID] VARCHAR(40) NOT NULL PRIMARY KEY\
                ,[USERNAME] VARCHAR(128)\
                ,[PASSWORD] VARCHAR(128)\
                   )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void CloudTable::Get(GetCloudInformationResponse &cloud)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_CLOUD]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        cloud.cloudID = query.value(0).toString().toStdString();
        cloud.userName = query.value(1).toString().toStdString();

        cloud.password = m_crypto.Decrypt(query.value(2).toByteArray()).toStdString();
    }
}

void CloudTable::Save(const GetCloudInformationResponse &cloud)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_CLOUD] (CLOUD_ID,USERNAME,PASSWORD) VALUES (?, ?, ?)");
    query.addBindValue(QString(cloud.cloudID.c_str()));
    query.addBindValue(QString(cloud.userName.c_str()));

    query.addBindValue(m_crypto.Encrypt(QString(cloud.password.c_str())));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void CloudTable::Remove(uuid_string &cloudID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_CLOUD] WHERE CLOUD_ID=?");
    query.addBindValue(QString(cloudID.c_str()));

    if(!query.exec()) {
      throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}



}
}

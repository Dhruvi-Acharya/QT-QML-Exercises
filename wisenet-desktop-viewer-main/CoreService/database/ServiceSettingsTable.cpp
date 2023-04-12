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
#include "ServiceSettingsTable.h"

#include <QSysInfo>
#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

ServiceSettingsTable::ServiceSettingsTable(QSqlDatabase& database)
    :m_database(database)
{

}

void ServiceSettingsTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_SERVICE_SETTINGS"))) {
        SPDLOG_INFO("[T_SERVICE_SETTINGS] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_SERVICE_SETTINGS]( [KEY] VARCHAR(40) NOT NULL PRIMARY KEY,[VALUE] TEXT)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // Save Default Value
    std::map<std::string, std::string> defaultSettings;
    defaultSettings.emplace(SettingsKey::LogRetensionTimeDay,"30");

    Save(defaultSettings);
}

void ServiceSettingsTable::Get(std::map<std::string,std::string>& settings)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_SERVICE_SETTINGS]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        settings.emplace(query.value(0).toString().toStdString(), query.value(1).toString().toStdString());
    }
}

void ServiceSettingsTable::Save(const std::map<std::string,std::string>& settings)
{
    for(auto& setting : settings ){

        QSqlQuery query(m_database);

        query.prepare("INSERT OR REPLACE INTO [T_SERVICE_SETTINGS] (KEY,VALUE) VALUES (?, ?)");
        query.addBindValue(QString(setting.first.c_str()));
        query.addBindValue(QString(setting.second.c_str()));

        if(!query.exec()) {
            throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
        }

    }
}

void ServiceSettingsTable::Remove(const std::string& settingsKey)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_SERVICE_SETTINGS] WHERE KEY=?");
    query.addBindValue(QString(settingsKey.c_str()));

    if(!query.exec()) {
      throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

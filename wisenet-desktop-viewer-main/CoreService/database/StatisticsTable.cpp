#include "StatisticsTable.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QString>
#include <QSysInfo>
#include <QVariant>

#include "CoreServiceLogSettings.h"

Wisenet::Core::StatisticsTable::StatisticsTable(QSqlDatabase &database)
    : m_database(database)
{
}

void Wisenet::Core::StatisticsTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_STATISTICS"))) {
        SPDLOG_INFO("[T_STATISTICS] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "";
    queryString += "CREATE TABLE [T_STATISTICS] (";
    queryString += "[MACHINE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY";
    queryString += ",[INSTALLED_UTC] INTEGER NOT NULL ";
    queryString += ",[EXECUTION_COUNT] INTEGER NOT NULL ";
    queryString += ",[EXECUTION_MINUTES] INTEGER NOT NULL ";
    queryString += ",[LAYOUT_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[CHANNEL_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[MEDIA_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[AVERAGE_RESPONSE_TIME] INTEGER NOT NULL)";

    if(!query.exec(queryString))
    {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QSqlQuery insertQuery(m_database);
    QString firstInsertQueryString = "";
    firstInsertQueryString += "INSERT OR REPLACE INTO [T_STATISTICS] ";
    firstInsertQueryString += "(MACHINE_ID,INSTALLED_UTC,EXECUTION_COUNT,EXECUTION_MINUTES,LAYOUT_OPEN_COUNT,CHANNEL_OPEN_COUNT,MEDIA_OPEN_COUNT,AVERAGE_RESPONSE_TIME) ";
    firstInsertQueryString += "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    insertQuery.prepare(firstInsertQueryString);

    insertQuery.addBindValue(getMacAddress());
    insertQuery.addBindValue(QDateTime::currentDateTime().toSecsSinceEpoch());
    insertQuery.addBindValue(1);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);

    if(!insertQuery.exec())
    {
        throw ManagementDatabaseException(insertQuery.lastQuery().toStdString(), insertQuery.lastError());
    }
}

void Wisenet::Core::StatisticsTable::Get(Statistics &statistics)
{
    QSqlQuery query(m_database);
    
    query.prepare("SELECT * FROM [T_STATISTICS]");

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        statistics.machineId = query.value(0).toString().toStdString();
        statistics.installedDatetime = query.value(1).toInt();
        statistics.executionCount = query.value(2).toInt();
        statistics.executionMinutes = query.value(3).toInt();
        statistics.layoutOpenCount = query.value(4).toInt();
        statistics.channelOpenCount = query.value(5).toInt();
        statistics.mediaRequestCount = query.value(6).toInt();
        statistics.averageMediaResponseTime = query.value(7).toInt();
    }
}

void Wisenet::Core::StatisticsTable::Save(const Statistics &statistics)
{
    Statistics dbData;

    this->Get(dbData);

    QSqlQuery updateQuery(m_database);
    QString updateQueryString = "";
    updateQueryString += "UPDATE [T_STATISTICS] ";
    updateQueryString += "SET EXECUTION_COUNT=?,EXECUTION_MINUTES=?,LAYOUT_OPEN_COUNT=?,CHANNEL_OPEN_COUNT=?,MEDIA_OPEN_COUNT=?,AVERAGE_RESPONSE_TIME=? ";
    updateQueryString += "WHERE MACHINE_ID=?";

    updateQuery.prepare(updateQueryString);

    updateQuery.addBindValue(statistics.executionCount);
    updateQuery.addBindValue(statistics.executionMinutes);
    updateQuery.addBindValue(statistics.layoutOpenCount);
    updateQuery.addBindValue(statistics.channelOpenCount);
    updateQuery.addBindValue(statistics.mediaRequestCount);
    updateQuery.addBindValue(statistics.averageMediaResponseTime);
    updateQuery.addBindValue(QString::fromStdString(statistics.machineId));

    if(!updateQuery.exec())
    {
        throw ManagementDatabaseException(updateQuery.lastQuery().toStdString(), updateQuery.lastError());
    }
}

QString Wisenet::Core::StatisticsTable::getMacAddress()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (QNetworkInterface interface, interfaces)
    {
        if(interface.hardwareAddress() != "")
        {
            return interface.hardwareAddress();
        }
    }
    return "";
}

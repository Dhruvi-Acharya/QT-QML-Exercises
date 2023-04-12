#include "DashboardTable.h"

#include <QString>
#include <QVariant>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

DashboardTable::DashboardTable(QSqlDatabase& database) : m_database(database)
{

}

void DashboardTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_DASHBOARD"))) {
        SPDLOG_INFO("[T_DASHBOARD] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_DASHBOARD]( [SAVE_TIME] INTEGER NOT NULL UNIQUE PRIMARY KEY, [POWER_ERROR_COUNT] INTEGER NOT NULL, [DISK_ERROR_COUNT] INTEGER NOT NULL, [OVERLOAD_ERROR_COUNT] INTEGER NOT NULL, [CONNECTION_ERROR_COUNT] INTEGER NOT NULL, [FAN_ERROR_COUNT] INTEGER NOT NULL, [DISCONNECTED_COUNT] INTEGER NOT NULL )"))
    {
        throw DashboardDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DashboardTable::Save(const DashboardData& dashboardData, const int deleteTime)
{
    QSqlQuery insertQuery(m_database);

    QString queryString = "INSERT OR REPLACE INTO [T_DASHBOARD]";
    queryString += " (SAVE_TIME, POWER_ERROR_COUNT, DISK_ERROR_COUNT, OVERLOAD_ERROR_COUNT, CONNECTION_ERROR_COUNT, FAN_ERROR_COUNT, DISCONNECTED_COUNT )";
    queryString += " VALUES (?, ?, ?, ?, ?, ?, ?)";

    insertQuery.prepare(queryString);

    insertQuery.addBindValue(QVariant::fromValue(dashboardData.saveTime));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.powerErrorCount));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.diskErrorCount));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.overloadErrorCount));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.connectionErrorCount));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.fanErrorCount));
    insertQuery.addBindValue(QVariant::fromValue(dashboardData.disconnectedCameraCount));

    if(!insertQuery.exec()) {
        throw DashboardDatabaseException(insertQuery.lastQuery().toStdString(), insertQuery.lastError());
    }

    QSqlQuery deleteQuery(m_database);
    //QString deleteQueryString = "DELETE FROM [T_DASHBOARD] WHERE SAVE_TIME<?";
    deleteQuery.prepare("DELETE FROM [T_DASHBOARD] WHERE SAVE_TIME<?");
    deleteQuery.addBindValue(QVariant::fromValue(deleteTime));

    if(!deleteQuery.exec()) {
        throw DashboardDatabaseException(deleteQuery.lastQuery().toStdString(), deleteQuery.lastError());
    }
}

void DashboardTable::GetAll(std::map<int, DashboardData>& dashboardDataMap)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_DASHBOARD] ORDER BY SAVE_TIME ASC");

    if(!query.exec()) {
        throw DashboardDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        DashboardData dashboardData;

        dashboardData.saveTime = query.value(0).toInt();
        dashboardData.powerErrorCount = query.value(1).toInt();
        dashboardData.diskErrorCount = query.value(2).toInt();
        dashboardData.overloadErrorCount = query.value(3).toInt();
        dashboardData.connectionErrorCount = query.value(4).toInt();
        dashboardData.fanErrorCount = query.value(5).toInt();
        dashboardData.disconnectedCameraCount = query.value(6).toInt();

        dashboardDataMap.emplace(dashboardData.saveTime, dashboardData);
    }
}

int DashboardTable::GetLatestTime()
{
    int timeStamp = 0;
    QSqlQuery query(m_database);

    query.prepare("SELECT MAX(SAVE_TIME) FROM [T_DASHBOARD]");

    if(!query.exec()) {
        throw DashboardDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        timeStamp = query.value(0).toInt();
    }

    return timeStamp;
}


}
}

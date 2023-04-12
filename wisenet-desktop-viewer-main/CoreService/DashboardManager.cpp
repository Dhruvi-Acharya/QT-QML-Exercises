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
#include "DashboardManager.h"

#include <chrono>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QDir>

#include "WeakCallback.h"
#include "LogSettings.h"
#include "TimeUtil.h"
#include "AppLocalDataLocation.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[DashboardManager] "}, level, __VA_ARGS__)

namespace Wisenet
{
namespace Core
{


DashboardManager::DashboardManager() :
    m_dashboardDatabase(std::make_shared<DashboardDatabase>())
{

}

bool DashboardManager::Open()
{
    SPDLOG_DEBUG("Open a database");

    QStringList list = QSqlDatabase::drivers();

    auto isSupported = false;
    for(QString& item : list) {

        SPDLOG_INFO("{}", item.toStdString());

        if(0 == item.compare(QString("QSQLITE"))) {
            isSupported = true;
        }
    }

    if(!isSupported)
    {
        SPDLOG_ERROR("Not supported a SQLite Driver.");
        return false;
    }
    QString dbPath = GetAppDataLocation();

    SPDLOG_DEBUG("database path = {}", dbPath.toStdString());

    if(!QDir(dbPath).exists())
    {
        if(!QDir().mkpath(dbPath))
        {
            SPDLOG_ERROR("Failed to create a database directory. path={}", dbPath.toStdString());
            return false;
        }
    }

    if(!m_dashboardDatabase->Open(dbPath))
    {
        SPDLOG_ERROR("Failed to open the dashboard database.");
        return false;
    }

    return true;
}

void DashboardManager::Close()
{
    m_dashboardDatabase->Close();
}

bool DashboardManager::Delete()
{
    QString log = GetAppDataLocation() + "/" + DashboardDatabase::FILENAME;

    if(QFile::exists(log)) {
        if(!QFile::remove(log)) {
            SPDLOG_ERROR("Failed to delete a dashboard database. path={}", log.toStdString());
            return false;
        }
    }

    return true;
}


void DashboardManager::SaveDashboardData(const DashboardData& dashboardData, const int deleteTime)
{
    try
    {
        m_dashboardDatabase->GetDashboardTable().Save(dashboardData, deleteTime);
    }
    catch(const DashboardDatabaseException& e)
    {
        SPDLOG_ERROR("SaveDashboardData() is failed. Error={}", e.what());
    }

}

void DashboardManager::GetDashboardData(std::map<int, DashboardData>& dashboardDataMap)
{
    try
    {
        m_dashboardDatabase->GetDashboardTable().GetAll(dashboardDataMap);
    }
    catch(const DashboardDatabaseException& e)
    {
        SPDLOG_ERROR("GetDashboardLatestTime() is failed. Error={}", e.what());
    }

}

int DashboardManager::GetDashboardLatestTime()
{
    int ret = 0;

    try
    {
        ret = m_dashboardDatabase->GetDashboardTable().GetLatestTime();
    }
    catch(const DashboardDatabaseException& e)
    {
        SPDLOG_ERROR("GetDashboardLatestTime() is failed. Error={}", e.what());
    }

    return ret;
}


}
}

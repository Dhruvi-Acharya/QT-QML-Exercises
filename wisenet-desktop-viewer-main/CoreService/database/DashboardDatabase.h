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
#pragma once

#include <QString>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <CoreService/CoreServiceRequestResponse.h>

#include "DatabaseException.h"
#include "DashboardTable.h"

namespace Wisenet
{
namespace Core
{

class DashboardDatabase
{
public:
    DashboardDatabase();

    bool Open(QString &path);
    void Close();

    void Transaction();
    void Commit();
    void Rollback();

    DashboardTable& GetDashboardTable() { return m_dashboardTable; }

    static QString FILENAME;
private:
    bool CreateTable();
    bool MigrateTable();
    void CreateVersionTable();

    // 0: v1.2.0
    static const int DB_LATEST_VERSION = 0;

    QString m_path;
    QSqlDatabase m_database;

    DashboardTable m_dashboardTable;
};

}
}

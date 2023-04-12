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
#include "AuditLogTable.h"
#include "EventLogTable.h"
#include "SystemLogTable.h"
#include "DeletedDeviceTable.h"
#include "DeletedChannelTable.h"
#include "DashboardTable.h"

namespace Wisenet
{
namespace Core
{

class LogDatabase
{
public:
    LogDatabase();

    bool Open(QString &path);
    void Close();

    void Transaction();
    void Commit();
    void Rollback();
    void Vacuum();

    AuditLogTable& GetAuditLogTable(){ return m_auditLogTable; }
    EventLogTable& GetEventLogTable(){ return m_eventLogTable; }
    SystemLogTable& GetSystemLogTable(){ return m_systemLogTable; }
    DeletedDeviceTable& GetDeletedDeviceTable(){ return m_deletedDeviceTable; }
    DeletedChannelTable& GetDeletedChannelTable(){ return m_deletedChannelTable; }

    static QString FILENAME;
private:
    bool CreateTable();
    bool MigrateTable();
    void CreateVersionTable();

    static const int DB_LATEST_VERSION = 2;

    QString m_path;
    QSqlDatabase m_database;

    AuditLogTable m_auditLogTable;
    EventLogTable m_eventLogTable;
    SystemLogTable m_systemLogTable;

    DeletedDeviceTable m_deletedDeviceTable;
    DeletedChannelTable m_deletedChannelTable;
};

}
}

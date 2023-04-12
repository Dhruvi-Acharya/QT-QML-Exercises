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
#include "AuditLogTable.h"

#include <QString>
#include <QVariant>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

AuditLogTable::AuditLogTable(QSqlDatabase& database)
    :m_database(database)
{

}

void AuditLogTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_AUDIT_LOG"))) {
        SPDLOG_INFO("[T_AUDIT_LOG] Already exists.");
        return;
    }


    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_AUDIT_LOG]( \
                   [AUDIT_LOG_ID] VARCHAR(40) PRIMARY KEY\
                   ,[SESSION_ID] VARCHAR(40) NOT NULL \
                   ,[USERNAME] VARCHAR(40) \
                   ,[HOST] VARCHAR(128) \
                   ,[ACTION_TYPE] VARCHAR(128) \
                   ,[CREATE_TIME] INTEGER \
                   ,[ITEM_TYPE] INTEGER NOT NULL DEFAULT 0 \
                   ,[ITEM_ID] VARCHAR(40) \
                   ,[PARENT_ID] VARCHAR(40) \
                   ,[ACTION] TEXT \
                   )")) {
            throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void AuditLogTable::Get(int64_t fromTime, int64_t toTime, std::shared_ptr<std::vector<AuditLog>>& responses)
{

    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_AUDIT_LOG] WHERE CREATE_TIME>=? AND CREATE_TIME<=? ORDER BY CREATE_TIME DESC, ROWID DESC");
    query.addBindValue(QVariant::fromValue(fromTime));
    query.addBindValue(QVariant::fromValue(toTime));

    if(!query.exec()) {
    throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        AuditLog auditLog;
        auditLog.sessionID = query.value(1).toString().toStdString();
        auditLog.userName = query.value(2).toString().toStdString();
        auditLog.host = query.value(3).toString().toStdString();
        auditLog.actionType = query.value(4).toString().toStdString();
        auditLog.serviceUtcTimeMsec = query.value(5).toLongLong();
        auditLog.itemType = (AuditLog::ItemType)query.value(6).toInt();
        auditLog.itemID = query.value(7).toString().toStdString();
        auditLog.parentID = query.value(8).toString().toStdString();
        auditLog.actionDetail = query.value(9).toString().toStdString();

        responses->emplace_back(auditLog);
    }
}

void AuditLogTable::Save(AuditLog &auditLog)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_AUDIT_LOG] \
                  (AUDIT_LOG_ID, SESSION_ID, USERNAME, HOST, ACTION_TYPE, CREATE_TIME, ITEM_TYPE, ITEM_ID, PARENT_ID, ACTION) \
                  VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(QString(auditLog.auditLogID.c_str()));
    query.addBindValue(QString(auditLog.sessionID.c_str()));
    query.addBindValue(QString(auditLog.userName.c_str()));
    query.addBindValue(QString(auditLog.host.c_str()));
    query.addBindValue(QString(auditLog.actionType.c_str()));
    query.addBindValue(QVariant::fromValue(auditLog.serviceUtcTimeMsec));
    query.addBindValue(QVariant::fromValue((unsigned int)auditLog.itemType));
    query.addBindValue(QString(auditLog.itemID.c_str()));
    query.addBindValue(QString(auditLog.parentID.c_str()));
    query.addBindValue(QString(auditLog.actionDetail.c_str()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void AuditLogTable::RemoveTo(int64_t timeMsec)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_AUDIT_LOG] WHERE CREATE_TIME<=?");
    query.addBindValue(QVariant::fromValue(timeMsec));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

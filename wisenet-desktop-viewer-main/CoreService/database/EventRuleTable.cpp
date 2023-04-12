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
#include "EventRuleTable.h"

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

EventRuleTable::EventRuleTable(QSqlDatabase& database)
    :m_database(database)
{

}

void EventRuleTable::CreateTable(uuid_string scheduleID)
{
    if(m_database.tables().contains(QLatin1String("T_EVENT_RULE"))) {
        SPDLOG_INFO("[T_EVENT_RULE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_EVENT_RULE]( [EVENT_RULE_ID] VARCHAR(40) PRIMARY KEY,[NAME] TEXT,[IS_ENABLED] BOOLEAN NOT NULL DEFAULT 1,[SCHEDULE_ID] VARCHAR(40),[WAITING_TIME] INTEGER,[EVENT] TEXT,[ACTION] TEXT)")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // Test Rule

     Q_UNUSED(scheduleID);
}

void EventRuleTable::GetAll(std::map<uuid_string, EventRule> &eventRules)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_EVENT_RULE]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        EventRule eventRule;
        eventRule.eventRuleID = query.value(0).toString().toStdString();
        eventRule.name = query.value(1).toString().toStdString();
        eventRule.isEnabled = query.value(2).toBool();
        eventRule.scheduleID = query.value(3).toString().toStdString();
        eventRule.waitingSecondTime = query.value(4).toLongLong();

        JsonReader eventReader(query.value(5).toString().toStdString().c_str());
        eventReader & eventRule.events;

        JsonReader actionReader(query.value(6).toString().toStdString().c_str());
        actionReader & eventRule.actions;


        eventRules.emplace(eventRule.eventRuleID, eventRule);
    }
}

void EventRuleTable::Save(EventRule &eventRule)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_EVENT_RULE] (EVENT_RULE_ID,NAME,IS_ENABLED,SCHEDULE_ID,WAITING_TIME,EVENT,ACTION) VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(eventRule.eventRuleID.c_str()));
    query.addBindValue(QString(eventRule.name.c_str()));
    query.addBindValue(eventRule.isEnabled);
    query.addBindValue(QString(eventRule.scheduleID.c_str()));
    query.addBindValue(eventRule.waitingSecondTime);

    JsonWriter eventWriter;
    eventWriter & eventRule.events;
    query.addBindValue(QString(eventWriter.GetString()));

    JsonWriter actionWriter;
    actionWriter & eventRule.actions;
    query.addBindValue(QString(actionWriter.GetString()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void EventRuleTable::Remove(const uuid_string &eventRuleID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_EVENT_RULE] WHERE EVENT_RULE_ID=?");
    query.addBindValue(QString(eventRuleID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

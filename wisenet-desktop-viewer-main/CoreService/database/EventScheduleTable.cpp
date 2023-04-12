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
#include "EventScheduleTable.h"

#include <QString>
#include <QVariant>
#include <sstream>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

EventScheduleTable::EventScheduleTable(QSqlDatabase& database)
    :m_database(database)
{

}

void EventScheduleTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_EVENT_SCHEDULE"))) {
        SPDLOG_INFO("[T_EVENT_SCHEDULE] Already exists.");
        return;
    }


    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_EVENT_SCHEDULE]( [EVENT_SCHEDULE_ID] VARCHAR(40) PRIMARY KEY,[NAME] TEXT, [SCHEDULE] TEXT)")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    {
        SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::CreateTable start");
        // Test Schedule
        EventSchedule eventSchedule;

        eventSchedule.scheduleID = boost::uuids::to_string(boost::uuids::random_generator()());
        eventSchedule.scheduleName = "Always";
        eventSchedule.schedule = std::string(8*24, '1');

        Save(eventSchedule);

        SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::CreateTable end");
    }
}

void EventScheduleTable::GetAll(std::map<uuid_string, EventSchedule> &eventSchedules)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_EVENT_SCHEDULE]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::GetAll start");
    while(query.next()) {
        EventSchedule eventSchedule;
        eventSchedule.scheduleID = query.value(0).toString().toStdString();
        eventSchedule.scheduleName = query.value(1).toString().toStdString();
        eventSchedule.schedule = query.value(2).toString().toStdString();

        /*std::string schedule(query.value(2).toString().toStdString());

        for(size_t i =0; i< schedule.length(); i++){
            if(schedule.at(i) == '1')
                eventSchedule.schedule[i] = true;
            else
                eventSchedule.schedule[i] = false;
        }*/
        eventSchedules.emplace(eventSchedule.scheduleID, eventSchedule);
    }
    SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::GetAll end");
}

uuid_string EventScheduleTable::GetAlwaysSchedule()
{
    std::map<uuid_string, EventSchedule> eventSchedules;
    GetAll(eventSchedules);

    for(auto& item : eventSchedules){
        if(item.second.scheduleName == "Always")
            return item.first;
    }
    return "";
}

void EventScheduleTable::Save(EventSchedule &eventSchedule)
{
    QSqlQuery query(m_database);
    SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::Save start");
    query.prepare("INSERT OR REPLACE INTO [T_EVENT_SCHEDULE] (EVENT_SCHEDULE_ID,NAME,SCHEDULE) VALUES (?, ?, ?)");
    query.addBindValue(QString(eventSchedule.scheduleID.c_str()));
    query.addBindValue(QString(eventSchedule.scheduleName.c_str()));
    query.addBindValue(QString(eventSchedule.schedule.c_str()));

    /*std::ostringstream schedule;
    for(size_t i = 0; i < 8*24; i++){
        schedule << std::noboolalpha << eventSchedule.schedule[i];
    }

    query.addBindValue(schedule.str().c_str());*/


    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    SPDLOG_INFO("[T_EVENT_SCHEDULE] EventScheduleTable::Save end");
}

void EventScheduleTable::Remove(const uuid_string &eventScheduleID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_EVENT_SCHEDULE] WHERE EVENT_SCHEDULE_ID=?");
    query.addBindValue(QString(eventScheduleID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

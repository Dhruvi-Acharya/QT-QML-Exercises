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
#include "EventLogTable.h"

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

EventLogTable::EventLogTable(QSqlDatabase& database)
    :m_database(database)
{

}

void EventLogTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_EVENT_LOG"))) {
        SPDLOG_INFO("[T_EVENT_LOG] Already exists.");
        return;
    }


    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_EVENT_LOG]( [EVENT_LOG_ID] VARCHAR(40) PRIMARY KEY,[CREATE_TIME] INTEGER,[DEVICE_TIME] INTEGER,[TYPE] VARCHAR(128),[DEVICE_ID] VARCHAR(40),[CHANNEL_ID] VARCHAR(40),[IS_START] BOOLEAN NOT NULL DEFAULT 1,[PARAMETERS] TEXT)")) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    //for(unsigned int i = 1 ; i < 11 ; i++){
    //    EventLog log;
    //    std::stringstream ss;
    //    ss << i;
    //    log.eventLogID = ss.str();
    //    log.serviceUtcTimeMsec = i;
    //    log.deviceUtcTimeMsec = i;
    //    log.type = ss.str();
    //    log.deviceID = ss.str();
    //    log.channelID = ss.str();
    //    log.channelName = ss.str();
    //    log.isStart = true;
    //    log.parameters.source["channel"] = ss.str();
    //    log.parameters.data["ROIID"] = ss.str();
    //
    //    Save(log);
    //}
    //
    //std::vector<EventLog> response;
    //
    //auto request = std::make_shared<GetEventLogRequest>();
    //request->fromTime = 2;
    //request->toTime = 7;
    //request->types.emplace_back("2");
    //request->types.emplace_back("6");
    //request->types.emplace_back("4");
    //request->types.emplace_back("7");
    //GetEventLogRequest::ChannelID id;
    //id.deviceId = "6";
    //id.channelId = "6";
    //request->channelIds.emplace_back(id);
    //id.deviceId = "4";
    //id.channelId = "4";
    //request->channelIds.emplace_back(id);
    //
    //Get(request, response);
}

void EventLogTable::Get(const GetEventLogRequestSharedPtr& request, std::shared_ptr<std::vector<EventLog>> response)
{
    std::stringstream ss;

    ss << "SELECT * FROM [T_EVENT_LOG] WHERE (CREATE_TIME>=" << request->fromTime << " AND CREATE_TIME<=" << request->toTime << ") ";

    if(!request->isAllType && (request->types.size() > 0)){
        ss << "AND (TYPE IN (";
        size_t itemCount = 0;
        for(auto& type : request->types){
            itemCount++;
            ss << "'" << type << "'";
            if(itemCount < (request->types.size())){
                ss << " , ";
            }
        }
        ss << ")) ";
    }

    if(!request->isAllChannel && (request->channelIds.size() > 0)){
        ss << "AND (";
        size_t itemCount = 0;
        for(auto& channelId : request->channelIds){
            itemCount++;
            ss << "(";
            ss << "(DEVICE_ID=" << "'" << channelId.deviceId << "'" << ")";
            ss << " AND ";
            ss << "(CHANNEL_ID=" << "'" << channelId.channelId << "'" << ")";
            ss << ")";

            if(itemCount < (request->channelIds.size())){
                ss << " OR ";
            }
        }
        ss << ") ";
    }

    ss << " ORDER BY CREATE_TIME DESC, ROWID DESC";

    SPDLOG_DEBUG("EventLogTable::Get Query : {}", ss.str());

    QSqlQuery query(m_database);

    if(!query.exec(QString(ss.str().c_str()))) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        EventLog log;
        log.eventLogID = query.value(0).toString().toStdString();
        log.serviceUtcTimeMsec = query.value(1).toLongLong();
        log.deviceUtcTimeMsec = query.value(2).toLongLong();
        log.type = query.value(3).toString().toStdString();
        log.isChannel = true;
        log.deviceID = query.value(4).toString().toStdString();
        log.channelID = query.value(5).toString().toStdString();
        log.isStart = query.value(6).toBool();
        std::string parameters = query.value(7).toString().toStdString();
        JsonReader reader(parameters.c_str());
        reader & log.parameters;

        response->emplace_back(log);
    }
}

void EventLogTable::Save(EventLog &eventLog)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT INTO [T_EVENT_LOG] \
            (EVENT_LOG_ID, CREATE_TIME, DEVICE_TIME, TYPE, DEVICE_ID, CHANNEL_ID, IS_START, PARAMETERS) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(QString(eventLog.eventLogID.c_str()));
    query.addBindValue(QVariant::fromValue(eventLog.serviceUtcTimeMsec));
    query.addBindValue(QVariant::fromValue(eventLog.deviceUtcTimeMsec));
    query.addBindValue(QString(eventLog.type.c_str()));
    query.addBindValue(QString(eventLog.deviceID.c_str()));
    query.addBindValue(QString(eventLog.channelID.c_str()));
    query.addBindValue(eventLog.isStart);


    JsonWriter writer;
    writer & eventLog.parameters;
    query.addBindValue(QString(writer.GetString()));


    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void EventLogTable::RemoveTo(int64_t timeMsec)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_EVENT_LOG] WHERE CREATE_TIME<=?");
    query.addBindValue(QVariant::fromValue(timeMsec));

    if(!query.exec()) {
      throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

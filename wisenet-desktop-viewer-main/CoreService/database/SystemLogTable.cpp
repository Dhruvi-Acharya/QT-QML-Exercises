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
#include "SystemLogTable.h"

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


SystemLogTable::SystemLogTable(QSqlDatabase& database)
    :m_database(database)
{

}

void SystemLogTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_SYSTEM_LOG"))) {
        SPDLOG_INFO("[T_SYSTEM_LOG] Already exists.");
        return;
    }


    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_SYSTEM_LOG]( [SYSTEM_LOG_ID] VARCHAR(40) PRIMARY KEY,[CREATE_TIME] INTEGER,[DEVICE_TIME] INTEGER,[TYPE] VARCHAR(128),[IS_SERVICE] BOOLEAN NOT NULL DEFAULT 0, [DEVICE_ID] VARCHAR(40), [IS_START] BOOLEAN NOT NULL DEFAULT 1,[PARAMETERS] TEXT)")) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void Wisenet::Core::SystemLogTable::Get(const Wisenet::Core::GetSystemLogRequestSharedPtr &request, std::shared_ptr<std::vector<Wisenet::Core::EventLog>> &response)
{
    std::stringstream ss;

    ss << "SELECT * FROM [T_SYSTEM_LOG] WHERE ((CREATE_TIME>=" << request->fromTime << " AND CREATE_TIME<=" << request->toTime << ") ";

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

    if(!request->isAllDevice && (request->deviceIds.size() > 0)){
        ss << "AND ((DEVICE_ID IN (";
        size_t itemCount = 0;
        for(auto& deviceId : request->deviceIds){
            itemCount++;
            ss << "'" << deviceId << "'";
            if(itemCount < (request->deviceIds.size())){
                ss << " , ";
            }
        }
        ss << ")) ";
    }

    if(request->containService){
        ss << "OR (IS_SERVICE = 1)) ";
    }else{
        ss << ")";
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
        if(query.value(4).toBool()){
            log.isService = true;
        }else{
            log.isDevice = true;
            log.deviceID = query.value(5).toString().toStdString();
        }

        log.isStart = query.value(6).toBool();

        std::string parameters = query.value(7).toString().toStdString();
        JsonReader reader(parameters.c_str());
        reader & log.parameters;

        response->emplace_back(log);
    }
}

void SystemLogTable::Save(EventLog &eventLog)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT INTO [T_SYSTEM_LOG] \
            (SYSTEM_LOG_ID, CREATE_TIME, DEVICE_TIME, TYPE, IS_SERVICE, DEVICE_ID, IS_START, PARAMETERS) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?)");

    query.addBindValue(QString(eventLog.eventLogID.c_str()));
    query.addBindValue(QVariant::fromValue(eventLog.serviceUtcTimeMsec));
    query.addBindValue(QVariant::fromValue(eventLog.deviceUtcTimeMsec));
    query.addBindValue(QString(eventLog.type.c_str()));
    query.addBindValue(eventLog.isService);
    query.addBindValue(QString(eventLog.deviceID.c_str()));
    query.addBindValue(eventLog.isStart);

    JsonWriter writer;
    writer & eventLog.parameters;
    query.addBindValue(QString(writer.GetString()));

    if(!query.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SystemLogTable::RemoveTo(int64_t timeMsec)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_SYSTEM_LOG] WHERE CREATE_TIME<=?");
    query.addBindValue(QVariant::fromValue(timeMsec));


    if(!query.exec()) {
      throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

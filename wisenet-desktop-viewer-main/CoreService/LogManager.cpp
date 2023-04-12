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
#include "LogManager.h"

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
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[LogManager] "}, level, __VA_ARGS__)



namespace Wisenet
{
namespace Core
{

LogManager::LogManager(boost::asio::io_context& ioc)
    :m_strandForSave(boost::asio::make_strand(ioc.get_executor()))
    ,m_strandForDatabase(boost::asio::make_strand(ioc.get_executor()))
    ,m_logDatabase(std::make_shared<LogDatabase>())
    ,m_auditLogs(std::make_shared < std::vector<AuditLog >>())
    ,m_eventLogs(std::make_shared< std::vector<EventLogSharedPtr>>())
    ,m_systemLogs(std::make_shared<std::vector<EventLogSharedPtr>>())
    ,m_timer(ioc)
    ,m_isConditionTimeout(false)
    ,m_isRunning(false)
    ,m_vacuumTimeMsec(0)
{

}

bool LogManager::Open()
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

    if(!m_logDatabase->Open(dbPath))
    {
        SPDLOG_ERROR("Failed to open the log database.");
        return false;
    }

    restartTimer();

    m_isRunning = true;

    return true;
}

void LogManager::Close()
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);
    m_isRunning = false;
    m_timer.cancel();

    {
        std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
        m_isConditionTimeout = false;
    }

    commitAuditLog(true);

    {
        std::unique_lock<std::mutex> conditionLock(m_conditionMutex);
        if(!m_condition.wait_for(conditionLock,std::chrono::seconds(10),[this](){return m_isConditionTimeout;})) {
            SPDLOG_ERROR("LogManager has timed out in the Close method.");
        }
    }

    {
        std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
        m_isConditionTimeout = false;
    }
    commitEventLog(true);
    {
        std::unique_lock<std::mutex> conditionLock(m_conditionMutex);
        if(!m_condition.wait_for(conditionLock,std::chrono::seconds(10),[this](){return m_isConditionTimeout;})) {
            SPDLOG_ERROR("LogManager has timed out in the Close method.");
        }
    }

    {
        std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
        m_isConditionTimeout = false;
    }
    commitSystemLog(true);
    {
        std::unique_lock<std::mutex> conditionLock(m_conditionMutex);
        if(!m_condition.wait_for(conditionLock,std::chrono::seconds(10),[this](){return m_isConditionTimeout;})) {
            SPDLOG_ERROR("LogManager has timed out in the Close method.");
        }
    }

    m_logDatabase->Close();
}

bool LogManager::Delete()
{
    QString log = GetAppDataLocation() + "/" + LogDatabase::FILENAME;

    if(QFile::exists(log)) {
        if(!QFile::remove(log)) {
            SPDLOG_ERROR("Failed to delete a manament database. path={}", log.toStdString());
            return false;
        }
    }

    return true;
}

void LogManager::AddBackupSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Backup Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogBackupSettingsActionDetail detail;
    detail.isSuccess = isSuccess;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddRestoreSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Restore Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogRestoreSettingsActionDetail detail;
    detail.isSuccess = isSuccess;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddInitializeSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Initialize Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogInitializeSettingsActionDetail detail;
    detail.isSuccess = isSuccess;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddUserAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                 AuditLogOperationType operationType, const std::string &operationUserName)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "User Settings";
    log.itemType = AuditLog::ItemType::Service;

    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

    AuditLogUserActionDetail detail;
    detail.userName = operationUserName;
    detail.operationType = operationType;

    JsonWriter writer;
    writer & detail;
    log.actionDetail = writer.GetString();


    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddUserGroupAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                 AuditLogOperationType operationType, const std::string &operationUserGroupName)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "User Group Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogUserGroupActionDetail detail;
    detail.userGroupName = operationUserGroupName;
    detail.operationType = operationType;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddEventRulesAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                 AuditLogOperationType operationType, const std::string &eventRuleName)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Event Rule Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogEventRuleActionDetail detail;
    detail.operationType = operationType;
    detail.eventRuleName = eventRuleName;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddScheduleAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                 AuditLogOperationType operationType, const std::string &scheduleName)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Schedule Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogScheduleActionDetail detail;
    detail.operationType = operationType;
    detail.scheduleName = scheduleName;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddDeviceAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                   AuditLogOperationType operationType, const std::string& deviceID)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;

    log.actionType = "Device Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

    log.itemType = AuditLog::ItemType::Device;
    log.itemID = deviceID;

    AuditLogDeviceActionDetail detail;
    detail.operationType = operationType;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));

}

void LogManager::AddDeviceStatusAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                         AuditLogOperationType operationType, const std::string& deviceID)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;

    log.actionType = "Device Status";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

    log.itemType = AuditLog::ItemType::Device;
    log.itemID = deviceID;

    AuditLogDeviceStatusActionDetail detail;
    detail.operationType = operationType;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddSoftwareUpgradeAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;
    log.actionType = "Software upgrade";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    log.itemType = AuditLog::ItemType::Service;

    AuditLogUpdateSoftwareActionDetail detail;
    detail.isSuccess = isSuccess;

    JsonWriter writer;
    writer & detail;

    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddChannelAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                    AuditLogOperationType operationType, const std::string& deviceID, const std::string& channelID)
{

    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;

    log.actionType = "Channel Settings";
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

    log.itemType = AuditLog::ItemType::Camera;
    log.itemID = channelID;
    log.parentID = deviceID;

    AuditLogChannelActionDetail detail;
    detail.operationType = operationType;

    JsonWriter writer;
    writer & detail;
    log.actionDetail = writer.GetString();

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::AddMediaAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                                  Device::StreamType streamType, int64_t startUtcTimeMsec, int64_t endUtcTimeMsec,
                                  unsigned int trackID, const std::string& deviceID, const std::string& channelID)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("LogManager is stopped.");
        return;
    }

    AuditLog log;
    log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

    log.sessionID = sessionID;
    log.userName = userName;
    log.host = host;

    log.itemType = AuditLog::ItemType::Camera;
    log.itemID = channelID;
    log.parentID = deviceID;

    switch(streamType)
    {
    case Device::StreamType::liveHigh:
    case Device::StreamType::liveLow:
    {
        log.actionType = "Watching Live";
        AuditLogWatchingLiveDetail detail;
        detail.startUtcTimeMsec = startUtcTimeMsec;
        detail.endUtcTimeMsec = endUtcTimeMsec;

        JsonWriter writer;
        writer & detail;
        log.actionDetail = writer.GetString();
        break;
    }
    case Device::StreamType::playbackHigh:
    case Device::StreamType::playbackLow:
    {
        log.actionType = "Watching Playback";
        AuditLogWatchingPlayBackDetail detail;
        detail.startUtcTimeMsec = startUtcTimeMsec;
        detail.endUtcTimeMsec = endUtcTimeMsec;
        detail.trackID = trackID;

        JsonWriter writer;
        writer & detail;

        log.actionDetail = writer.GetString();
        break;
    }
    case Device::StreamType::backupHigh:
    case Device::StreamType::backupLow:
    {
        log.actionType = "Exporting Video";

        AuditLogExportingVideoDetail detail;
        detail.startUtcTimeMsec = startUtcTimeMsec;
        detail.endUtcTimeMsec = endUtcTimeMsec;
        detail.trackID = trackID;

        JsonWriter writer;
        writer & detail;

        log.actionDetail = writer.GetString();
        break;
    }
    default:
        SPDLOG_ERROR("Unkown action type of audit.");
        return;
    };


    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, log]()
    {
        m_auditLogs->emplace_back(log);
    }));
}

void LogManager::SaveAuditLogInMemory(const AuditLog& auditLog)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("SaveAuditLogInMemory is canceled");
        return;
    }

    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, auditLog]()
    {
        m_auditLogs->emplace_back(auditLog);
    }));
}

void LogManager::commitAuditLog(bool isLast)
{
    boost::asio::post(m_strandForSave,
                      WeakCallback(shared_from_this(),
                                   [this, isLast]()
    {
        if(m_auditLogs->empty()){
            if(isLast) {
                {
                    std::lock_guard<std::mutex> lock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }
            return;
        }

        auto auditLogs = m_auditLogs;
        m_auditLogs.reset(new std::vector<AuditLog>());

        boost::asio::post(m_strandForDatabase,
                          WeakCallback(shared_from_this(),
                                       [this, auditLogs, isLast]()
        {
            try {
                m_logDatabase->Transaction();

                for(auto auditLog : *auditLogs) {
                    m_logDatabase->GetAuditLogTable().Save(auditLog);
                }
                m_logDatabase->Commit();

            } catch(const LogDatabaseException& e) {
                m_logDatabase->Rollback();
                SPDLOG_ERROR("CommitAuditLog() is failed. Error={}", e.what());
            }

            if(isLast) {
                {
                    std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }

        }));

    }));
}

void LogManager::commitEventLog(bool isLast)
{
    boost::asio::post(m_strandForSave,
               WeakCallback(shared_from_this(),
                            [this, isLast]()
    {
        if(m_eventLogs->empty()){
            if(isLast) {
                {
                    std::lock_guard<std::mutex> lock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }
            return;
        }

        auto eventLogs = m_eventLogs;
        m_eventLogs.reset(new std::vector<EventLogSharedPtr>());

        boost::asio::post(m_strandForDatabase,
                   WeakCallback(shared_from_this(),
                                [this, eventLogs, isLast]()
        {
            try {
                m_logDatabase->Transaction();

                for(auto& eventLog : *eventLogs) {
                    m_logDatabase->GetEventLogTable().Save(*eventLog);
                }
                m_logDatabase->Commit();

            } catch(const LogDatabaseException& e) {
                m_logDatabase->Rollback();
                SPDLOG_ERROR("commitEventLog() is failed. Error={}", e.what());
            }

            if(isLast) {
                {
                    std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }

        }));

    }));
}

void LogManager::commitSystemLog(bool isLast)
{
    boost::asio::post(m_strandForSave,
               WeakCallback(shared_from_this(),
                            [this, isLast]()
    {
        if(m_systemLogs->empty()){
            if(isLast) {
                {
                    std::lock_guard<std::mutex> lock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }
            return;
        }

        auto systemLogs = m_systemLogs;
        m_systemLogs.reset(new std::vector<EventLogSharedPtr>());

        boost::asio::post(m_strandForDatabase,
                   WeakCallback(shared_from_this(),
                                [this, systemLogs, isLast]()
        {
            try {
                m_logDatabase->Transaction();

                for(auto& systemLog : *systemLogs) {
                    m_logDatabase->GetSystemLogTable().Save(*systemLog);
                }
                m_logDatabase->Commit();

            } catch(const LogDatabaseException& e) {
                m_logDatabase->Rollback();
                SPDLOG_ERROR("commitSystemLog() is failed. Error={}", e.what());
            }

            if(isLast) {
                {
                    std::lock_guard<std::mutex> conditionLock(m_conditionMutex);
                    m_isConditionTimeout = true;
                }
                m_condition.notify_one();
            }

        }));

    }));
}

void LogManager::GetAuditLog( GetAuditLogRequestSharedPtr const&request, const ResponseBaseHandler &responseHandler)
{
    boost::asio::post(m_strandForDatabase,
                      WeakCallback(shared_from_this(),
                                   [this, request, responseHandler]()
    {
        auto response = std::make_shared<GetAuditLogResponse>();
        response->deletedDevices = std::make_shared<std::map<uuid_string,DeletedDevice>>();
        response->auditLogs = std::make_shared<std::vector<AuditLog>>();
        try {

            m_logDatabase->GetDeletedDeviceTable().GetAll(response->deletedDevices);

            for(auto& deletedDevice : *response->deletedDevices){
                m_logDatabase->GetDeletedChannelTable().Get(deletedDevice.second.deviceID,deletedDevice.second.channels);
            }

            m_logDatabase->GetAuditLogTable().Get(request->fromTime, request->toTime, response->auditLogs);
            response->errorCode = Wisenet::ErrorCode::NoError;

        } catch(const LogDatabaseException& e) {
            m_logDatabase->Rollback();
            SPDLOG_ERROR("GetAuditLog() is failed. Error={}", e.what());
            response->auditLogs->clear();
            response->errorCode = Wisenet::ErrorCode::LogDatabaseError;
        }

        if(responseHandler) {
            responseHandler(response);
        }

    }));
}

void LogManager::AddEventLog(const EventLogSharedPtr &eventLog)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("AddEventLog is canceled");
        return;
    }

    boost::asio::post(m_strandForSave,
               WeakCallback(shared_from_this(),
                            [this, eventLog]()
    {
        m_eventLogs->emplace_back(eventLog);
    }));
}

void LogManager::GetEventLog(const GetEventLogRequestSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    boost::asio::post(m_strandForDatabase,
               WeakCallback(shared_from_this(),
                            [this, request, responseHandler]()
    {
        auto response = std::make_shared<GetEventLogResponse>();
        response->deletedDevices = std::make_shared<std::map<uuid_string,DeletedDevice>>();
        response->eventLogs = std::make_shared<std::vector<EventLog>>();
        try {

            m_logDatabase->GetDeletedDeviceTable().GetAll(response->deletedDevices);

            for(auto& deletedDevice : *response->deletedDevices){
                m_logDatabase->GetDeletedChannelTable().Get(deletedDevice.second.deviceID,deletedDevice.second.channels);
            }

            m_logDatabase->GetEventLogTable().Get(request, response->eventLogs);
            response->errorCode = Wisenet::ErrorCode::NoError;

        } catch(const LogDatabaseException& e) {
            m_logDatabase->Rollback();
            SPDLOG_ERROR("GetEventLog() is failed. Error={}", e.what());
            response->eventLogs->clear();
            response->errorCode = Wisenet::ErrorCode::LogDatabaseError;
        }

        if(responseHandler) {
            responseHandler(response);
        }

    }));
}

void LogManager::AddSystemLog(const EventLogSharedPtr &systemLog)
{
    std::lock_guard<std::recursive_mutex> logLock(m_logMutex);

    if(!m_isRunning) {
        SPDLOG_WARN("AddSystemLog is canceled");
        return;
    }

    boost::asio::post(m_strandForSave,
               WeakCallback(shared_from_this(),
                            [this, systemLog]()
    {
        m_systemLogs->emplace_back(systemLog);
    }));
}

void LogManager::GetSystemLog(const GetSystemLogRequestSharedPtr &request, const ResponseBaseHandler &responseHandler)
{
    boost::asio::post(m_strandForDatabase,
               WeakCallback(shared_from_this(),
                            [this, request, responseHandler]()
    {
        auto response = std::make_shared<GetSystemLogResponse>();
        response->deletedDevices = std::make_shared<std::map<uuid_string,DeletedDevice>>();
        response->eventLogs = std::make_shared<std::vector<EventLog>>();
        try {

            m_logDatabase->GetDeletedDeviceTable().GetAll(response->deletedDevices);

            for(auto& deletedDevice : *response->deletedDevices){
                m_logDatabase->GetDeletedChannelTable().Get(deletedDevice.second.deviceID,deletedDevice.second.channels);
            }

            m_logDatabase->GetSystemLogTable().Get(request, response->eventLogs);
            response->errorCode = Wisenet::ErrorCode::NoError;

        } catch(const LogDatabaseException& e) {
            m_logDatabase->Rollback();
            SPDLOG_ERROR("GetEventLog() is failed. Error={}", e.what());
            response->eventLogs->clear();
            response->errorCode = Wisenet::ErrorCode::LogDatabaseError;
        }

        if(responseHandler) {
            responseHandler(response);
        }

    }));
}

void LogManager::AddDeletedDevice(const DeletedDeviceSharedPtr &deletedDevice)
{
    boost::asio::post(m_strandForDatabase,
               WeakCallback(shared_from_this(),
                            [this, deletedDevice]()
    {
        try {
            m_logDatabase->Transaction();

            m_logDatabase->GetDeletedDeviceTable().Save(*deletedDevice);

            for(auto& channel : deletedDevice->channels){
                m_logDatabase->GetDeletedChannelTable().Save(deletedDevice->serviceUtcTimeMsec,channel.second);
            }

            m_logDatabase->Commit();

        } catch(const LogDatabaseException& e) {
            m_logDatabase->Rollback();
            SPDLOG_ERROR("AddDeletedDevice() is failed. Error={}", e.what());
        }

    }));
}

void LogManager::RemoveTo(long long timeMsec)
{
    boost::asio::post(m_strandForDatabase,
               WeakCallback(shared_from_this(),
                            [this, timeMsec]()
    {
        try {
            m_logDatabase->Transaction();

            // Device와 Channel은 다른 로그보다 나중에 지워지도록 2분을 뺀다.
            m_logDatabase->GetDeletedDeviceTable().RemoveTo(timeMsec - 120000);
            m_logDatabase->GetDeletedChannelTable().RemoveTo(timeMsec - 120000);

            m_logDatabase->GetAuditLogTable().RemoveTo(timeMsec);
            m_logDatabase->GetEventLogTable().RemoveTo(timeMsec);
            m_logDatabase->GetSystemLogTable().RemoveTo(timeMsec);

            m_logDatabase->Commit();

            //설정된 시간이 경과되면 vacuum을 해서 db 크기를 정리한다.
            auto currentMsec = Wisenet::Common::currentUtcMsecs();
            if(VACUUM_ELAPSED_TIME_MSEC < (currentMsec - m_vacuumTimeMsec)){
                m_vacuumTimeMsec = currentMsec;
                m_logDatabase->Vacuum();
            }

        } catch(const LogDatabaseException& e) {
            m_logDatabase->Rollback();
            SPDLOG_ERROR("RemoveTo() is failed. Error={}", e.what());
        }

    }));
}

void LogManager::restartTimer()
{
    m_timer.expires_from_now(std::chrono::seconds(10));
    m_timer.async_wait(WeakCallback(shared_from_this(),
                                    [this](const boost::system::error_code& ec)
    {
        if (ec == boost::asio::error::operation_aborted)
        {
            SPDLOG_INFO("Timer for AuditLog is aborted.");
            return;
        }

        commitAuditLog();
        commitEventLog();
        commitSystemLog();
        restartTimer();
    }));
}



}
}

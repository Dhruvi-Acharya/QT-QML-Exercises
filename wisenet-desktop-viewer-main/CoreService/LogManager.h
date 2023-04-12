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

#include <memory>
#include <boost/asio.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

#include "database/LogDatabase.h"

namespace Wisenet
{
namespace Core
{

class LogManager : public std::enable_shared_from_this<LogManager>
{
public:
    LogManager(boost::asio::io_context& ioc);

    bool Open();
    void Close();

    static bool Delete();

    // AuditLog
    void AddUserAuditLog(const std::string& sessionID, const std::string& userName, const std::string& host,
                         AuditLogOperationType operationType, const std::string& operationUserName);

    void AddDeviceAuditLog(const std::string& sessionID, const std::string& userName, const std::string& host,
                           AuditLogOperationType operationType, const std::string& deviceID);

    void AddChannelAuditLog(const std::string& sessionID, const std::string& userName, const std::string& host,
                            AuditLogOperationType operationType, const std::string& deviceID, const std::string& channelID);

    void AddMediaAuditLog(const std::string& sessionID, const std::string& userName, const std::string& host,
                          Wisenet::Device::StreamType streamType, int64_t startUtcTimeMsec, int64_t endUtcTimeMsec,
                          unsigned int trackID, const std::string& deviceID, const std::string& channelID);

    void SaveAuditLogInMemory(const AuditLog& auditLog);
    void GetAuditLog(GetAuditLogRequestSharedPtr const&request, ResponseBaseHandler const&responseHandler);

    void AddBackupSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess);
    void AddRestoreSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess);
    void AddInitializeSettingsAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess);
    void AddUserGroupAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, AuditLogOperationType operationType, const std::string &operationUserGroupName);
    void AddEventRulesAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, AuditLogOperationType operationType, const std::string &eventRuleName);
    void AddScheduleAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, AuditLogOperationType operationType, const std::string &scheduleName);
    void AddDeviceStatusAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, AuditLogOperationType operationType, const std::string& deviceID);
    void AddSoftwareUpgradeAuditLog(const std::string &sessionID, const std::string &userName, const std::string &host, const bool isSuccess);

    // EventLog
    void AddEventLog(const EventLogSharedPtr& eventLog);
    void GetEventLog(GetEventLogRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler);

    // SystemLog
    void AddSystemLog(const EventLogSharedPtr& systemLog);
    void GetSystemLog(GetSystemLogRequestSharedPtr const& request, ResponseBaseHandler const& responseHandler);

    // Deleted Device
    void AddDeletedDevice(const DeletedDeviceSharedPtr& deletedDevice);

    // Remove Log
    void RemoveTo(long long timeMsec);

private:
    void commitAuditLog(bool isLast = false);
    void commitEventLog(bool isLast = false);
    void commitSystemLog(bool isLast = false);

    void restartTimer();

    static const int64_t VACUUM_ELAPSED_TIME_MSEC = 604800000; // 일주일 경과 시간
    // 메모리에 임시 가지고 있는 m_auditLogs의 데이터 처리 시간 최소화를 위하여 메모리 저장용 strand와 DB 처리용 strand를 분리함.
    boost::asio::strand<boost::asio::io_context::executor_type> m_strandForSave;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strandForDatabase;
    std::shared_ptr<LogDatabase> m_logDatabase;

    std::shared_ptr<std::vector<AuditLog>> m_auditLogs;
    std::shared_ptr<std::vector<EventLogSharedPtr>> m_eventLogs;
    std::shared_ptr<std::vector<EventLogSharedPtr>> m_systemLogs;

    boost::asio::steady_timer m_timer;

    std::recursive_mutex m_logMutex;

    std::condition_variable m_condition;
    std::mutex m_conditionMutex;
    bool m_isConditionTimeout;

    bool m_isRunning;

    int64_t m_vacuumTimeMsec;

};

}
}

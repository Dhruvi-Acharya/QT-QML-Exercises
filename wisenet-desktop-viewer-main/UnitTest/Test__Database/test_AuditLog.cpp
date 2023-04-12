/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "tst_test__database.h"

#include <chrono>
#include <sstream>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

#include "JsonArchiver.h"

void Test__Database::test_auditLog()
{
    /*
    int maxCount = 1000;

    // LogIn
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::LogIn;
        auditLog.createTime = 1;

        auditLog.logInActionDetail.isSuccess = true;
        auditLog.logInActionDetail.startTime = 100;
        auditLog.logInActionDetail.endTime = 150;

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // SystemAction

    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::SystemAction;
        auditLog.createTime = 1;

        auditLog.systemActionDetail.actionDetailType =  Wisenet::Core::AuditLog::ActionDetailType::Updated;
        auditLog.systemActionDetail.systemActionDetailType = Wisenet::Core::AuditLog::SystemActionDetail::SystemActionDetailType::General;

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // UserAction

    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::UserAction;
        auditLog.createTime = 1;

        auditLog.userActionDetail.actionDetailType = Wisenet::Core::AuditLog::ActionDetailType::Updated;
        auditLog.userActionDetail.userName = "user";

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // EventRuleAction
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::EventRuleAction;
        auditLog.createTime = 1;

        auditLog.eventRuleActionDetail.actionDetailType = Wisenet::Core::AuditLog::ActionDetailType::Updated;
        auditLog.eventRuleActionDetail.sourceType = 10;
        auditLog.eventRuleActionDetail.actionType = 11;

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // DeviceAction
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::DeviceAction;
        auditLog.createTime = 1;

        auditLog.deviceActionDetail.actionDetailType = Wisenet::Core::AuditLog::ActionDetailType::Updated;
        Wisenet::Core::AuditLog::DeviceActionDetail::Device device;
        device.deviceID = boost::uuids::to_string(boost::uuids::random_generator()());
        device.name = "XNP-6000";
        device.host = "192.168.1.100";

        auditLog.deviceActionDetail.devices.emplace_back(device);

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // ChannelAction
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::ChannelAction;
        auditLog.createTime = 1;

        auditLog.channelActionDetail.actionDetailType = Wisenet::Core::AuditLog::ActionDetailType::Updated;
        Wisenet::Core::AuditLog::ChannelActionDetail::Channel channel;
        channel.deviceID = boost::uuids::to_string(boost::uuids::random_generator()());
        channel.channelID = "1";
        channel.name = "XNP-6000-CH01";
        channel.host = "192.168.1.100";

        auditLog.channelActionDetail.channels.emplace_back(channel);

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // WatchingLive
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::WatchingLive;
        auditLog.createTime = 1;

        auditLog.watchingLiveDetail.startTime = 1600;
        auditLog.watchingLiveDetail.endTime = 2000;

        Wisenet::Core::AuditLog::ChannelActionDetail::Channel channel;
        channel.deviceID = boost::uuids::to_string(boost::uuids::random_generator()());
        channel.channelID = "1";
        channel.name = "XNP-6000-CH01";
        channel.host = "192.168.1.100";
        auditLog.watchingLiveDetail.channels.emplace_back(channel);

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // WatchingArchive
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::WatchingArchive;
        auditLog.createTime = 1;

        auditLog.watchingArchiveDetail.startTime = 1600;
        auditLog.watchingArchiveDetail.endTime = 2000;

        Wisenet::Core::AuditLog::ChannelActionDetail::Channel channel;
        channel.deviceID = boost::uuids::to_string(boost::uuids::random_generator()());
        channel.channelID = "1";
        channel.name = "XNP-6000-CH01";
        channel.host = "192.168.1.100";
        auditLog.watchingArchiveDetail.channels.emplace_back(channel);

        m_database->SaveAuditLogInMemory(auditLog);
    }

    // ExportingVideo
    for(int i = 0 ; i < maxCount; i++) {

        Wisenet::Core::AuditLog auditLog;
        auditLog.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.sessionID = boost::uuids::to_string(boost::uuids::random_generator()());
        auditLog.userName = "admin";
        auditLog.host = "localhost";
        auditLog.logType = Wisenet::Core::AuditLog::AuditLogType::ExportingVideo;
        auditLog.createTime = 1;

        auditLog.exportingVideoDetail.startTime = 1600;
        auditLog.exportingVideoDetail.endTime = 2000;

        Wisenet::Core::AuditLog::ChannelActionDetail::Channel channel;
        channel.deviceID = boost::uuids::to_string(boost::uuids::random_generator()());
        channel.channelID = "1";
        channel.name = "XNP-6000-CH01";
        channel.host = "192.168.1.100";
        auditLog.exportingVideoDetail.channels.emplace_back(channel);


        m_database->SaveAuditLogInMemory(auditLog);
    }

    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        m_database->CommitAuditLog();
        std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
        SPDLOG_INFO("Commit execution time(sec) : {} seconds", sec.count());
        std::stringstream ss;
        ss << "Commit execution time(sec) : " << sec.count() << " seconds";
        QWARN(ss.str().c_str());
    }

    {
        std::chrono::system_clock::time_point start = std::chrono::system_clock::now();
        std::vector<Wisenet::Core::AuditLog> auditLogs;
        m_database->GetAuditLog(0,10000,auditLogs);
        std::chrono::duration<double> sec = std::chrono::system_clock::now() - start;
        SPDLOG_INFO("Get execution time(sec) : {} seconds, Count : {}", sec.count(), auditLogs.size());
        std::stringstream ss;
        ss << "Get execution time(sec) : " << sec.count() << " seconds, Count : {}" << auditLogs.size();
        QWARN(ss.str().c_str());
    }
    */
}

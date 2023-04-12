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
#include "PTZLogManager.h"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "LogSettings.h"
#include "WeakCallback.h"
#include "DatabaseManager.h"
#include "LogManager.h"
#include "TimeUtil.h"

namespace Wisenet
{
namespace Core
{

PTZLogManager::PTZLogManager(DatabaseManager& dataBase,
                             LogManager& logManager,
                             boost::asio::io_context& ioc)
    : m_db(dataBase)
    , m_logManager(logManager)
    , m_strand(ioc.get_executor())
    , m_timer(ioc)
{

}

void PTZLogManager::Start()
{
    restartTimer();
}

void PTZLogManager::Stop()
{
    m_timer.cancel();
}

void PTZLogManager::AddLog(const std::string &sessionID, const std::string &userName, const std::string &host, const std::string& deviceID, const std::string& channelID)
{
    boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this, sessionID, userName, host, deviceID, channelID]()
    {
        std::string key = sessionID + "_" + deviceID + "_" + channelID;

        auto itor = m_logs.find(key);

        if(itor == m_logs.end()){
            // 새로 추가됨.
            SPDLOG_DEBUG("PTZLogManager::AddLog : Add, sessionID({}),userName({}),host({}),channel({},{})",
                         sessionID, userName, host, deviceID, channelID);

            AuditLog log;
            log.auditLogID = boost::uuids::to_string(boost::uuids::random_generator()());
            log.sessionID = sessionID;
            log.userName = userName;
            log.host = host;

            log.actionType = "PTZ Control";
            log.serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

            log.itemType = AuditLog::ItemType::Camera;
            log.itemID = channelID;
            log.parentID = deviceID;

            AuditLogPTZControlActionDetail detail;
            detail.startUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
            detail.endUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

            JsonWriter writer;
            writer & detail;

            log.actionDetail = writer.GetString();

            m_logs.emplace(key, log);

        }else{
            SPDLOG_DEBUG("PTZLogManager::AddLog : Update, sessionID({}),userName({}),host({}),channel({},{})",
                         sessionID, userName, host, deviceID, channelID);

            JsonReader reader(itor->second.actionDetail.c_str());

            AuditLogPTZControlActionDetail detail;

            reader & detail;

            detail.endUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

            JsonWriter writer;
            writer & detail;

            itor->second.actionDetail = writer.GetString();
        }

    }));
}

void PTZLogManager::restartTimer()
{
    m_timer.expires_after(std::chrono::seconds(1));
    m_timer.async_wait( WeakCallback(shared_from_this(),
                        [this](const boost::system::error_code& ec)
    {
        if (ec){
            SPDLOG_INFO("PTZLogManager::restartTimer is stoppted.");
            return;
        }

        boost::asio::post(m_strand, WeakCallback(shared_from_this(), [this]()
        {
            auto currentUtcTimeMsec = Wisenet::Common::currentUtcMsecs();

            for (auto it = m_logs.cbegin(); it != m_logs.cend() /* not hoisted */; /* no increment */)
            {
              //5초이상 업데이트가 안되어 있으면 종료.
              JsonReader reader(it->second.actionDetail.c_str());

              AuditLogPTZControlActionDetail detail;
              reader & detail;

              if (((currentUtcTimeMsec - detail.endUtcTimeMsec) >= 5000)
                      ||((currentUtcTimeMsec - detail.endUtcTimeMsec) < 0)){

                  SPDLOG_DEBUG("PTZLogManager::AddLog : Update, sessionID({}),userName({}),host({}),channel({},{})",
                               it->second.sessionID, it->second.userName, it->second.host,
                               it->second.parentID, it->second.itemID);

                  m_logManager.SaveAuditLogInMemory(it->second);

                  it = m_logs.erase(it);

              } else {

                  ++it;
              }
            }

            restartTimer();
        }));
    }));
}

}
}

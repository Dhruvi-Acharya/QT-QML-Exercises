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

#include <boost/asio.hpp>
#include "CoreService/CoreServiceStructure.h"

namespace Wisenet
{
namespace Core
{

class DatabaseManager;
class LogManager;

class PTZLogManager : public std::enable_shared_from_this<PTZLogManager>
{
public:
    PTZLogManager(DatabaseManager& dataBase,
                  LogManager& logManager,
                  boost::asio::io_context& ioc);

    void Start();
    void Stop();
    void AddLog(const std::string &sessionID, const std::string &userName, const std::string &host,
                const std::string& deviceID, const std::string& channelID);
private:
    void restartTimer();

    DatabaseManager&    m_db;
    LogManager&         m_logManager;

    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    boost::asio::steady_timer m_timer;

    std::map<std::string,AuditLog> m_logs; // key = (sessionID_deviceId_channelId)

};

}
}

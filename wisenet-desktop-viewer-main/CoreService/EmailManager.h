/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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

#include <QPointer>
#include <QBasicMutex>
#include <map>
#include <mutex>
#include <thread>
#include <chrono>
#include "DatabaseManager.h"


namespace Wisenet
{
namespace Core
{
class EmailManager : public QObject
{
public:
    static EmailManager* Instance();
    static void Initialize();

public:
    EmailManager();
    ~EmailManager();

    void SetEventDeviceInfo(EventLogSharedPtr info);
    void SetHostInfo(QString host);
    void SetEmailList(std::set<EventRule::ItemID> itemIDs);

    // thread
    void StartTask();
    void StopTask();

    void SendEventEmails(std::map<std::string /* user id */, std::string /* user email */> emails);

private:
    void Task();

    void SetReceiveEmailList();

    QString GetHostInfo();

private:
    QString m_host;
    QString m_machineName;
    QString m_userName;
    QString m_senderName;
    QString m_deviceName;
    QString m_channelName;
    QString m_ipAddress;
    QString m_eventTime;

    bool m_isRunning;

    std::shared_ptr<std::thread> m_tasker;

    std::recursive_mutex m_lock;

    std::map<std::string /* user id */, std::string /* user email */> m_emaiMap;
};

}
}


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
#include <QPointer>
#include <QBasicMutex>
#include <QCoreApplication>
#include <boost/asio/ip/host_name.hpp>

#include "QCoreServiceManager.h"
#include "EmailManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "smtpclient.h"
#include "mimemessage.h"
#include "mimetext.h"

namespace Wisenet
{
namespace Core
{

EmailManager *EmailManager::Instance()
{
    static QPointer<EmailManager> gInstance;
    static QBasicMutex mutex;

    const QMutexLocker lock(&mutex);
    if (gInstance.isNull() && !QCoreApplication::closingDown()) {
        gInstance = new EmailManager();
    }
    return gInstance;
}

void EmailManager::Initialize()
{
    EmailManager::Instance();
}

EmailManager::EmailManager()
    : m_host("")
    , m_machineName("")
    , m_userName("")
    , m_senderName("")
    , m_deviceName("")
    , m_channelName("")
    , m_ipAddress("")
    , m_eventTime("")
    , m_isRunning(false)
    , m_tasker(nullptr)
{
    m_userName = qgetenv("USER");
    if (m_userName.isEmpty()) {
        m_userName = qgetenv("USERNAME");
    }

    m_machineName = QString::fromStdString(boost::asio::ip::host_name());

    m_senderName = "Wisenet Viewer[" + m_machineName + "/" + m_userName + "]";
}

EmailManager::~EmailManager()
{
    StopTask();
}

QString EmailManager::GetHostInfo()
{
    return m_host;
}

void EmailManager::SetHostInfo(QString host)
{
    m_host = host;
}

void EmailManager::SetEventDeviceInfo(EventLogSharedPtr info)
{
    Wisenet::Device::Device deviceInfo;
    if(!QCoreServiceManager::Instance().DB()->FindDevice(info->deviceID, deviceInfo))
    {
        return;
    }

    long long serviceUtcTimeMsec = info->serviceUtcTimeMsec;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(serviceUtcTimeMsec);

    for (auto& ch : deviceInfo.channels)
    {
        if (deviceInfo.deviceID == ch.second.deviceID) {
            m_channelName = QString::fromStdString(ch.second.channelName);
            m_ipAddress = QString::fromStdString(ch.second.ip);
            break;
        }
    }

    // 장비(IP주소)
    m_deviceName = QString::fromStdString(deviceInfo.deviceName) + "(" + m_ipAddress + ")";

    // 카메라 이벤트일 경우, 채널명 표기
    if (!m_channelName.isEmpty()) {
        m_deviceName += " - " + m_channelName;
    }

    // 이벤트 발생 시간
    m_eventTime = QLocaleManager::Instance()->getDateTime(dateTime);
}

void EmailManager::Task()
{
    while (m_isRunning)
    {
        m_lock.lock();
        if (m_emaiMap.size() > 0) {

            auto emailMap = std::move(m_emaiMap);

            SendEventEmails(emailMap);
        }
        m_lock.unlock();

         std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
}

void EmailManager::StartTask()
{
    if (!m_tasker && !m_isRunning)
    {
        m_isRunning = true;

        m_tasker = std::make_shared<std::thread>(&EmailManager::Task, this);
    }
}

void EmailManager::StopTask()
{
    if (m_tasker && m_isRunning)
    {
        m_isRunning = false;

        m_tasker->detach();

        if (m_tasker->joinable())
        {
            m_tasker->join();

            m_tasker.reset();
        }
    }
}

void EmailManager::SetEmailList(std::set<EventRule::ItemID> itemIDs)
{
    std::lock_guard<std::recursive_mutex> lock(m_lock);

    m_emaiMap.clear();

    auto users = QCoreServiceManager::Instance().DB()->GetUsers();

    for (auto& emailId : itemIDs) {
        if (users.find(emailId.deviceID) == users.end())
            continue;

        for (auto& user : users) {
            auto userId = user.second.userID;
            auto userEmail = user.second.email;

            if (userEmail.empty() || userEmail == "")
                continue;

            if (emailId.deviceID != userId)
                continue;

            auto item = m_emaiMap.find(userId);

            if (item != m_emaiMap.end())
                continue;

            m_emaiMap.insert(std::pair<std::string, std::string>(userId, userEmail));
        }
    }
}

void EmailManager::SendEventEmails(std::map<std::string /* user id */, std::string /* user email */> emails)
{
    auto sender = QCoreServiceManager::Instance().DB()->GetEventEmail();

    auto serverAddr = sender->serverAddress;
    auto id = sender->id;
    auto pwd = sender->password;
    auto port = sender->port;
    auto from = sender->sender;
    auto auth = sender->isAuthentication;

    // 호스트 정보 저장 (이벤트 액션 전송 시 사용하기 위해...)
    Wisenet::Core::EmailManager::Instance()->SetHostInfo(QString::fromStdString(serverAddr));

    //SPDLOG_DEBUG("=====> emailMap count: {}", m_emaiMap.size());

    for (auto& it : emails)
    {
        // 2022.07.06. 호스트 정보가 비어있는 상태에서 SMTP Client를 초기화하면 크래시 발생하기에 예외처리
        if (GetHostInfo().isEmpty())
            continue;

        SmtpClient smtpClient(GetHostInfo(), std::stoi(port), SmtpClient::SslConnection);

        // id
        smtpClient.setUser(QString::fromStdString(id));

        // pwd
        smtpClient.setPassword(QString::fromStdString(pwd));

        MimeMessage mimeMessage;
        MimeText mimeText;

        // from
        auto fromObj = std::make_shared<EmailAddress>(QString::fromStdString(from), m_senderName);
        mimeMessage.setSender(fromObj.get());

        // to
        auto toObj = std::make_shared<EmailAddress>(it.second.c_str(), it.second.c_str());
        mimeMessage.addRecipient(toObj.get());

        auto rules = QCoreServiceManager::Instance().DB()->GetEventRules();

        QString eventRuleName = "";
        QString eventName = "";

        bool found = false;
        for (auto& rule : rules) {
            auto emailUse = rule.second.actions.emailAction.useAction;
            auto emailItemSize = rule.second.actions.emailAction.itemIDs.size();

            if (!emailUse && !emailItemSize) {
                continue;
            }

            for (auto& itemID : rule.second.actions.emailAction.itemIDs) {
                if (itemID.deviceID == it.first /* 이메일의 GUID */) {
                    for (auto& trigger : rule.second.events) {
                        eventRuleName = QString::fromStdString(rule.second.name);
                        eventName = QString::fromStdString(trigger.type);
                        found = true;
                        break;
                    }
                }

                if (found)
                    break;
            }

            if (found)
                break;
        }

        if (!found)
            continue;

        // subject
        mimeMessage.setSubject(eventName);

        // content
        QString ruleName = "Rule Name: " + eventRuleName;
        QString eventTrigger = "Event trigger: " + eventName + " / " + m_deviceName + " / " + m_eventTime;
        QString content = ruleName + "\r\n" + eventTrigger;

        mimeText.setText(content);
        mimeMessage.addPart(&mimeText);

//        SPDLOG_DEBUG("=====> from: {}, sendername: {}, to: {}, eventRuleName: {}, eventName: {}, content: {}"
//                     , from
//                     , m_senderName.toStdString()
//                     , it.second.c_str()
//                     , eventRuleName.toStdString()
//                     , eventName.toStdString()
//                     , content.toStdString());

        // connect
        if (!smtpClient.connectToHost()) {
            SPDLOG_INFO("[EmailManager] SendEventEmails - fail (connect)");
            continue; // fail - connect
        }

        // login
        if (auth) {
            if (!smtpClient.login()) {
                SPDLOG_INFO("[EmailManager] SendEventEmails - fail (login)");
                continue; // fail - login
            }
        }

        // send
        if (!smtpClient.sendMail(mimeMessage)) {
            SPDLOG_INFO("[EmailManager] SendEventEmails - fail (send)");
            continue; // fail - send
        }

        SPDLOG_INFO("[EmailManager] SendEventEmails - success !!!");
        smtpClient.quit();
    }

    m_emaiMap.clear();
}

}
}

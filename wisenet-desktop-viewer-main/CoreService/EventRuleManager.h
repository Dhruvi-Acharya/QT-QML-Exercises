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

#include <map>
#include <CoreService/CoreServiceStructure.h>
#include "DeviceClient/DeviceEvent.h"

namespace Wisenet
{
namespace Core
{

typedef std::function <void(const long long&, const long long&,const std::string&, const EventRule::AlertAlarmAction&)>  AlertAlarmActionHandler;
typedef std::function <void(const uuid_string&, const EventRule::AlarmOutputAction&)>  AlarmOutputActionHandler;
typedef std::function <void(const EventRule::LayoutChangeAction&)>  LayoutChangeActionHandler;
typedef std::function <void(const EventLogSharedPtr&, const EventRule::EmailAction&)> EmailActionHandler;
typedef std::function <void(const EventLogSharedPtr&)>  WriteToLogActionHandler;

class DatabaseManager;
class EventRuleRunner
{
public:
    explicit EventRuleRunner(const EventRule& eventRule, DatabaseManager& dataBase);

    void SetAlertAlarmActionHandler(AlertAlarmActionHandler const& handler);
    void SetAlarmOutputActionHandler(AlarmOutputActionHandler const& handler);
    void SetLayoutChangeActionHandler(LayoutChangeActionHandler const& handler);
    void SetEmailActionHandler(EmailActionHandler const& handler);

    bool ProcessEvent(const EventLogSharedPtr& eventLog);

private:
    bool containEvent(const EventLogSharedPtr& eventLog);
    bool containItem(const EventRule::Event& event, const EventLogSharedPtr& eventLog);
    bool containInterval(const EventLogSharedPtr& eventLog);
    bool containSchedule(const EventLogSharedPtr& eventLog);

    EventRule m_eventRule;
    DatabaseManager& m_db;

    std::map<std::string, long long> m_receivedTimes;

    AlertAlarmActionHandler m_alertAlarmActionHandler;
    AlarmOutputActionHandler m_alarmOutputActionHandler;
    LayoutChangeActionHandler m_layoutChangeActionHandler;
    EmailActionHandler m_emailActionHandler;
};
typedef std::shared_ptr<EventRuleRunner> EventRuleRunnerSharedPtr;

class EventRuleManager
{
public:
    EventRuleManager(DatabaseManager& dataBase);

    // 첫번째로 Action Functor 설정 해야함.
    void SetAlertAlarmActionHandler(AlertAlarmActionHandler const& handler);
    void SetAlarmOutputActionHandler(AlarmOutputActionHandler const& handler);
    void SetLayoutChangeActionHandler(LayoutChangeActionHandler const& handler);
    void SetEmailActionHandler(EmailActionHandler const& handler);
    void SetWriteToLogActionHandler(WriteToLogActionHandler const& handler);

    // Event Rule 설정.
    void SetEventRules(const std::map<uuid_string, EventRule>& eventRules);
    void SaveEventRule(const EventRule& eventRules);
    void RemoveEventRule(const std::vector<uuid_string>& eventRuleIDs);    

    // Event 입력.
    void ProcessEvent(const EventBaseSharedPtr& event);
    void SendDeviceStatusEvent(const std::string& deviceID, const Wisenet::Device::DeviceStatusType& type);

private:
    bool isIgnoredEvent(const Device::DeviceAlarmEventSharedPtr& deviceAlarmEvent);
    std::string deviceStatusToSystemEventType(const Device::DeviceStatusType &type);

    std::map<uuid_string, EventRuleRunnerSharedPtr> m_eventRuleRunners;

    DatabaseManager& m_db;
    AlertAlarmActionHandler m_alertAlarmActionHandler;
    AlarmOutputActionHandler m_alarmOutputActionHandler;
    LayoutChangeActionHandler m_layoutChangeActionHandler;
    EmailActionHandler m_emailActionHandler;
    WriteToLogActionHandler m_writeToLogActionHandler;
};

}
}

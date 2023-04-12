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
#include "QCoreServiceManager.h"
#include "EventRuleManager.h"
#include "CoreServiceLogSettings.h"
#include "DatabaseManager.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "TimeUtil.h"

namespace Wisenet
{
namespace Core
{

EventRuleRunner::EventRuleRunner(const EventRule& eventRule, DatabaseManager& dataBase)
    :m_eventRule(eventRule),
     m_db(dataBase),
     m_alertAlarmActionHandler(nullptr),
     m_alarmOutputActionHandler(nullptr),
     m_layoutChangeActionHandler(nullptr),
     m_emailActionHandler(nullptr)
{
}

void EventRuleRunner::SetAlertAlarmActionHandler(AlertAlarmActionHandler const& handler)
{
    m_alertAlarmActionHandler = handler;
}

void EventRuleRunner::SetAlarmOutputActionHandler(AlarmOutputActionHandler const& handler)
{
    m_alarmOutputActionHandler = handler;
}

void EventRuleRunner::SetLayoutChangeActionHandler(LayoutChangeActionHandler const& handler)
{
    m_layoutChangeActionHandler = handler;
}

void EventRuleRunner::SetEmailActionHandler(EmailActionHandler const& handler)
{
    m_emailActionHandler = handler;
}

bool EventRuleRunner::ProcessEvent(const EventLogSharedPtr& eventLog)
{
    //사용중 상태가 아니면 처리하지 않음.
    if(!m_eventRule.isEnabled){
        return false;
    }

    if(!containSchedule(eventLog)){
        return false;
    }

    // 설정된 장비/채널과 이벤트가 일치하는지 확인한다.
    if(!containEvent(eventLog)){
        return false;
    }

    // 4개까지 설정되어 있고 시간 범위내에 발생된 것이지 확인한다.
    if(!containInterval(eventLog)){
        return false;
    }

    if(m_eventRule.actions.alertAlarmAction.useAction
            && m_alertAlarmActionHandler
            && (m_eventRule.actions.alertAlarmAction.itemIDs.size() > 0)){
        m_alertAlarmActionHandler(Wisenet::Common::currentUtcMsecs(), eventLog->deviceUtcTimeMsec, m_eventRule.name, m_eventRule.actions.alertAlarmAction);
    }

    if(m_eventRule.actions.alarmOutputAction.useAction){
        m_alarmOutputActionHandler(m_eventRule.eventRuleID,m_eventRule.actions.alarmOutputAction);
    }

    if(m_eventRule.actions.layoutChangeAction.useAction){
        m_layoutChangeActionHandler(m_eventRule.actions.layoutChangeAction);
    }

    if(m_eventRule.actions.emailAction.useAction){
        m_emailActionHandler(eventLog, m_eventRule.actions.emailAction);
    }

    return true;
}

bool EventRuleRunner::containEvent(const EventLogSharedPtr& eventLog)
{
    for(auto& event : m_eventRule.events){

        auto subTypes = Wisenet::GetSubAlarmType(event.type);

        for(auto& subType : subTypes){
            if(0 == subType.compare(eventLog->type)){
                SPDLOG_DEBUG("EventRuleRunner::containEvent - is true type({})", eventLog->type);

                if(containItem(event, eventLog)){
                    SPDLOG_DEBUG("EventRuleRunner::containItem - is true type({})", eventLog->type);
                    return true;
                }else{
                    SPDLOG_DEBUG("EventRuleRunner::containItem - is false type({})", eventLog->type);
                    return false;
                }
            }
        }
    }
    return false;
}

bool EventRuleRunner::containItem(const EventRule::Event& event, const EventLogSharedPtr& eventLog)
{
    if(event.isAllItem){
        return true;
    }

    EventRule::ItemID itemID;
    itemID.isDevice = eventLog->isDevice;
    itemID.isChannel = eventLog->isChannel;
    itemID.deviceID = eventLog->deviceID;
    itemID.channelID = eventLog->channelID;

    if(0 == eventLog->type.compare(Wisenet::FixedAlarmType::NetworkAlarmInput)){
        itemID.isDevice = false;
        itemID.isChannel = false;
        itemID.isAlarm = true;
        itemID.alarmID = "1";
    }else if(0 == eventLog->type.compare(Wisenet::FixedAlarmType::DeviceAlarmInput)){
        itemID.isDevice = false;
        itemID.isChannel = false;
        itemID.isAlarm = true;
        auto itor = eventLog->parameters.source.find("SystemID");
        if(itor != eventLog->parameters.source.end()){
            itemID.alarmID = itor->second;
        }
    }

    auto itor = event.itemIDs.find(itemID);

    if(itor != event.itemIDs.end()){
        return true;
    }

    return false;
}

bool EventRuleRunner::containSchedule(const EventLogSharedPtr& eventLog)
{
    // Get schedule data from DB
    auto &schedules = m_db.GetEventSchedules();
    auto schedule = schedules.find(m_eventRule.scheduleID);
    QString scheduleData = QString::fromStdString(schedule->second.schedule);

    // Get the time when the event occured
    QDateTime receivedTime = QDateTime::fromMSecsSinceEpoch(eventLog->serviceUtcTimeMsec);

    // Get the hour and day of the week
    int hour = receivedTime.time().hour(); // 0 to 23 of the time

    QDate date = receivedTime.date();
    int day = date.dayOfWeek(); // 1 = Monday to 7 = Sunday
    //SPDLOG_DEBUG("EventRuleRunner::containSchedule event time : {}h {}day", hour, day);

    if(scheduleData[24*(day-1)+hour] == '1'){
        //SPDLOG_DEBUG("EventRuleRunner::containSchedule schedule ON");
        return true;
    }
    else{
        //SPDLOG_DEBUG("EventRuleRunner::containSchedule schedule OFF");
        return false;
    }
}

bool EventRuleRunner::containInterval(const EventLogSharedPtr& eventLog)
{
    // 1개가 설정되어 있으면 Interval 체크를 하지 않는다.
    if(1 == m_eventRule.events.size()){
        return true;
    }

    //새로 받은 이벤트 시간을 업데이트 한다.
    m_receivedTimes[Wisenet::GetTitleAlarmType(eventLog->type)] = eventLog->serviceUtcTimeMsec;

    if(m_eventRule.events.size() > m_receivedTimes.size()){
        return false;
    }



    //가장 오래된 시간을 찾는다. 현재 받은 시간이 최신 시간이다.
    long long oldestTime = eventLog->serviceUtcTimeMsec;

    for(auto& itor : m_receivedTimes){
        if(oldestTime > itor.second){
            oldestTime = itor.second;
        }
    }

    long long elapsedTime = eventLog->serviceUtcTimeMsec - oldestTime;

    SPDLOG_DEBUG("EventRuleRunner::containInterval - mostrecenttime({}),oldesttime({}),elapsedTime({})",eventLog->serviceUtcTimeMsec, oldestTime, elapsedTime);

    if( m_eventRule.waitingSecondTime * 1000 > elapsedTime){

        return true;
    }

    return false;
}


EventRuleManager::EventRuleManager(DatabaseManager& dataBase)
    :m_db(dataBase)
{

}

void EventRuleManager::SetAlertAlarmActionHandler(AlertAlarmActionHandler const& handler)
{
    m_alertAlarmActionHandler = handler;

    for(auto& eventRunner : m_eventRuleRunners){
        eventRunner.second->SetAlertAlarmActionHandler(m_alertAlarmActionHandler);
    }
}

void EventRuleManager::SetAlarmOutputActionHandler(AlarmOutputActionHandler const& handler)
{
    m_alarmOutputActionHandler = handler;

    for(auto& eventRunner : m_eventRuleRunners){
        eventRunner.second->SetAlarmOutputActionHandler(m_alarmOutputActionHandler);
    }
}

void EventRuleManager::SetLayoutChangeActionHandler(LayoutChangeActionHandler const& handler)
{
    m_layoutChangeActionHandler = handler;

    for(auto& eventRunner : m_eventRuleRunners){
        eventRunner.second->SetLayoutChangeActionHandler(m_layoutChangeActionHandler);
    }
}


void EventRuleManager::SetEmailActionHandler(EmailActionHandler const& handler)
{
    m_emailActionHandler = handler;

    for(auto& eventRunner : m_eventRuleRunners){
        eventRunner.second->SetEmailActionHandler(m_emailActionHandler);
    }
}

void EventRuleManager::SetWriteToLogActionHandler(WriteToLogActionHandler const& handler)
{
    m_writeToLogActionHandler = handler;
}

void EventRuleManager::SetEventRules(const std::map<uuid_string, EventRule> &eventRules)
{
    SPDLOG_DEBUG("EventRuleManager::SetEventRules - size = {}", eventRules.size());

    for(auto& itor : eventRules) {
        SaveEventRule(itor.second);
    }
}

void EventRuleManager::SaveEventRule(const EventRule &eventRule)
{
    SPDLOG_DEBUG("EventRuleManager::SaveEventRule - eventRuleID({}),Name({})", eventRule.eventRuleID,eventRule.name);
    m_eventRuleRunners[eventRule.eventRuleID] = std::make_shared<EventRuleRunner>(eventRule, m_db);

    m_eventRuleRunners[eventRule.eventRuleID]->SetAlertAlarmActionHandler(m_alertAlarmActionHandler);
    m_eventRuleRunners[eventRule.eventRuleID]->SetAlarmOutputActionHandler(m_alarmOutputActionHandler);
    m_eventRuleRunners[eventRule.eventRuleID]->SetLayoutChangeActionHandler(m_layoutChangeActionHandler);
    m_eventRuleRunners[eventRule.eventRuleID]->SetEmailActionHandler(m_emailActionHandler);
}

void EventRuleManager::RemoveEventRule(const std::vector<uuid_string> &eventRuleIDs)
{
    SPDLOG_DEBUG("EventRuleManager::RemoveEventRule - size = {}", eventRuleIDs.size());

    for(auto& eventRuleID : eventRuleIDs) {
        auto itor = m_eventRuleRunners.find(eventRuleID);
        if(itor != m_eventRuleRunners.end()){
            m_eventRuleRunners.erase(eventRuleID);
        }
    }
}

void EventRuleManager::ProcessEvent(const EventBaseSharedPtr &event)
{
    if(Device::DeviceEventType::DeviceAlarmEventType == event->EventTypeId()) {

        Device::DeviceAlarmEventSharedPtr deviceAlarmEvent = std::static_pointer_cast<Device::DeviceAlarmEvent>(event);

        if(isIgnoredEvent(deviceAlarmEvent)){
            return;
        }
        auto eventLog = std::make_shared<EventLog>();
        eventLog->eventLogID = boost::uuids::to_string(boost::uuids::random_generator()());
        eventLog->serviceUtcTimeMsec = deviceAlarmEvent->serviceUtcTimeMsec;
        eventLog->deviceUtcTimeMsec = deviceAlarmEvent->deviceUtcTimeMsec;
        eventLog->type = deviceAlarmEvent->type;
        eventLog->isStart = deviceAlarmEvent->data;
        eventLog->isDevice = deviceAlarmEvent->isDevice;
        eventLog->isChannel = deviceAlarmEvent->isChannel;
        eventLog->deviceID = deviceAlarmEvent->deviceID;
        eventLog->channelID = deviceAlarmEvent->channelID;
        eventLog->parameters.source = deviceAlarmEvent->sourceParameters;
        eventLog->parameters.data = deviceAlarmEvent->dataParameters;

        if(m_writeToLogActionHandler){
            m_writeToLogActionHandler(eventLog);
        }

        for(auto& itor : m_eventRuleRunners){
            itor.second->ProcessEvent(eventLog);
        }

    }else if(Device::DeviceEventType::DeviceStatusEventType == event->EventTypeId()) {

        auto deviceStatusEvent = std::static_pointer_cast<Device::DeviceStatusEvent>(event);
        if(deviceStatusEvent->deviceStatus.status != Wisenet::Device::DeviceStatusType::ConnectedAndInfoChanged)
            SendDeviceStatusEvent(deviceStatusEvent->deviceID, deviceStatusEvent->deviceStatus.status);
    }

}

void EventRuleManager::SendDeviceStatusEvent(const std::string &deviceID, const Device::DeviceStatusType &type)
{
    auto eventLog = std::make_shared<EventLog>();

    eventLog->eventLogID = boost::uuids::to_string(boost::uuids::random_generator()());
    eventLog->serviceUtcTimeMsec = Wisenet::Common::currentUtcMsecs();
    eventLog->deviceUtcTimeMsec = 0;
    eventLog->type = deviceStatusToSystemEventType(type);
    eventLog->isStart = true;
    eventLog->isDevice = true;
    eventLog->isChannel = false;
    eventLog->deviceID = deviceID;

    SPDLOG_DEBUG("EventRuleManager::ProcessEvent - DeviceStatusEventType : {}", eventLog->type);

    if(!eventLog->type.empty() && m_writeToLogActionHandler){
        m_writeToLogActionHandler(eventLog);
    }
}

bool EventRuleManager::isIgnoredEvent(const Device::DeviceAlarmEventSharedPtr &deviceAlarmEvent)
{
    if(!deviceAlarmEvent->data){
        if(0 == deviceAlarmEvent->type.compare(Wisenet::FixedAlarmType::VideolossEnd)){
            //Videoloss.End 이벤트는 이벤트 처리함.
            return false;
        }else if(0 == deviceAlarmEvent->type.compare(Wisenet::FixedAlarmType::TrackingEnd)){
            //Tracking.End 이벤트는 이벤트 처리함.
            return false;
        }

        return true;
    }

    return false;
}

std::string EventRuleManager::deviceStatusToSystemEventType(const Device::DeviceStatusType &type)
{
    std::string systemEventType = "";

    switch(type){
    case Device::DeviceStatusType::Connected:
        systemEventType = Wisenet::AdditionalLogType::DeviceConnected;
        break;
    case Device::DeviceStatusType::DisconnectedByService:
        systemEventType = Wisenet::AdditionalLogType::DeviceDisconnectedByService;
        break;
    case Device::DeviceStatusType::DisconnectedWithError:
        systemEventType = Wisenet::AdditionalLogType::DeviceDisconnectedWithError;
        break;
    case Device::DeviceStatusType::DisconnectedWithRestriction:
        systemEventType = Wisenet::AdditionalLogType::DeviceDisconnectedWithRestriction;
        break;
    case Device::DeviceStatusType::Unauthorized:
        systemEventType = Wisenet::AdditionalLogType::DeviceDisconnectedWithUnauthorized;
        break;
    case Device::DeviceStatusType::ConnectedAndInfoChanged:
        break;
    }

    return systemEventType;
}

}
}

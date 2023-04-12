#include "AddEventRuleViewModel.h"

#include "LogSettings.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <set>
#include <utility>
#include "../../../Utility/TranslationSort.h"

AddEventRuleViewModel::AddEventRuleViewModel(QObject* parent) : m_ruleId("")
{
    Q_UNUSED(parent);
}

AddEventRuleViewModel::~AddEventRuleViewModel()
{
    qDebug() << "~AddEventRuleViewModel()";
}

void AddEventRuleViewModel::setRuleId(const QString &ruleId)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setRuleId ID:{}", ruleId.toStdString());
    if(ruleId != m_ruleId){
        m_ruleId = ruleId;
        emit this->ruleIdChanged();
    }
}

void AddEventRuleViewModel::setRuleName(const QString &ruleName)
{
    if(ruleName != m_ruleName){
        m_ruleName = ruleName;
        emit this->ruleNameChanged();
    }
}

void AddEventRuleViewModel::setDuration(const int waitingTime)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setDuration:{}", waitingTime);
    if(waitingTime != m_duration){
        m_duration = waitingTime;
        emit this->durationChanged(waitingTime);
    }
}

void AddEventRuleViewModel::setSchedule(const QString &schedule)
{
    if(schedule != m_schedule){
        m_schedule = schedule;
        emit this->scheduleChanged();
    }
    qDebug() << "AddEventRuleViewModel::setSchedule m_schedule" << m_schedule;
}

void AddEventRuleViewModel::setScheduleNameList()
{
    // 2022.12.28. converity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    //m_scheduleMap
    SPDLOG_DEBUG("AddEventRuleViewModel::setScheduleNameList()");
    auto schedules = db->GetEventSchedules();

    m_scheduleMap.clear();
    m_scheduleNameList.clear();
    for(auto& schedule: schedules)
    {
        m_scheduleMap[QString::fromStdString(schedule.first)] = QString::fromStdString(schedule.second.scheduleName);
        m_scheduleNameList.append(QString::fromStdString(schedule.second.scheduleName));
    }

    m_scheduleNameList.sort(); // QStringList 정렬한 후
    m_scheduleNameList.move(m_scheduleNameList.indexOf("Always"),0); // Always 데이터 맨 앞으로 순서 이동
}


QVariantMap AddEventRuleViewModel::getEventGuidList()
{
    SPDLOG_DEBUG("AddEventRuleViewModel::getEventGuidList ");
    QVariantMap vmap;

    QMapIterator<QString, QStringList> i(m_eventGuidList);
    while (i.hasNext()) {
        i.next();
        SPDLOG_DEBUG("AddEventRuleViewModel::getEventGuidList {} {}", i.key().toStdString());
        vmap.insert(i.key(), i.value());
    }
    return vmap;
}

QVariantMap AddEventRuleViewModel::getEventTriggerAll()
{
    SPDLOG_DEBUG("AddEventRuleViewModel::getEventTriggerAll ");
    QVariantMap vmap;

    QMapIterator<QString, bool> i(m_eventTriggerAll);
    while (i.hasNext()) {
        i.next();
        SPDLOG_DEBUG("AddEventRuleViewModel::getEventTriggerAll {}", i.key().toStdString());
        vmap.insert(i.key(), i.value());
    }
    return vmap;
}


int AddEventRuleViewModel::getAlarmoutDuration()
{
    SPDLOG_DEBUG("AddEventRuleViewModel::getAlarmoutDuration ");

    return m_alarmoutDuration;
}

QString AddEventRuleViewModel::getSchedule()
{
    SPDLOG_DEBUG("AddEventRuleViewModel::getAlarmoutDuration ");

    return m_schedule;
}


void AddEventRuleViewModel::readRule(QString ruleId)
{
    // 2022.12.28. converity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("AddEventRuleViewModel::readRuleInfo ID:{}", ruleId.toStdString());

    clearEventTree();
    auto eventRules = QCoreServiceManager::Instance().DB()->GetEventRules();
    auto rule = eventRules.find(ruleId.toStdString());
    setTotalList();
    setScheduleNameList();

    m_eventGuidList.clear();
    m_eventTriggerAll.clear();
    if(rule == eventRules.end() || ruleId.length() < 1) {
        SPDLOG_DEBUG("AddEventRuleViewModel::readRuleInfo ID:{} new", ruleId.toStdString());

        m_ruleId = QString::fromStdString(boost::uuids::to_string(boost::uuids::random_generator()()));
        auto schedules = db->GetEventSchedules();
        m_scheduleId = QString::fromStdString(schedules.begin()->first);
        m_isEnabled = true;

        setIsFullTrigger(false);
        setIsAvailableDuration(false);
        setDuration(defaultDuration);
        setRuleName("");
        setAddedEventList(QStringList());
        setAddedActionList(QStringList());
        setRemainingEventList();
        setExceptActionList();
        setSchedule("Always");

        m_alarmoutDuration = 0;
        return;
    }

    SPDLOG_DEBUG("AddEventRuleViewModel::readRuleInfo Name:{}", rule->second.name);

    std::set<std::string> eventTypes;

    for(auto& event : rule->second.events){


        eventTypes.insert(event.type);

        auto eventType = QString::fromStdString(event.type);

        auto iter = m_eventTriggerAll.find(eventType);
        if(iter == m_eventTriggerAll.end()){
            m_eventTriggerAll.insert(eventType, event.isAllItem);
        }

        if(event.isAllItem == false){
            auto iter2 = m_eventGuidList.find(eventType);
            if(iter2 == m_eventGuidList.end()){
                m_eventGuidList.insert(eventType, makeEventGuidList(eventType, event.itemIDs));
            }
        }

    }
    QStringList eventTrigger;
    for(auto& event : eventTypes){
        eventTrigger.append(QString::fromStdString(event));
    }

    QStringList eventAction;
    if(rule->second.actions.alarmOutputAction.useAction){
        eventAction.append(fixedAlarmOut);
        m_eventGuidList.insert(fixedAlarmOut, makeEventGuidList(fixedAlarmOut, rule->second.actions.alarmOutputAction.itemIDs));
        m_alarmoutDuration = rule->second.actions.alarmOutputAction.intervalSeconds;
        SPDLOG_DEBUG("AddEventRuleViewModel::readRuleInfo m_alarmoutDuration:{}", m_alarmoutDuration);

    }
    else{
        m_alarmoutDuration =0;
    }

    if(rule->second.actions.alertAlarmAction.useAction){
        eventAction.append(fixedAlertAlarm);
        m_eventGuidList.insert(fixedAlertAlarm, makeEventGuidList(fixedAlertAlarm, rule->second.actions.alertAlarmAction.itemIDs));
    }
    if(rule->second.actions.layoutChangeAction.useAction){
        eventAction.append(fixedLayoutChange);
        m_eventGuidList.insert(fixedLayoutChange, QStringList(rule->second.actions.layoutChangeAction.layoutID.c_str()));
    }
    if (rule->second.actions.emailAction.useAction) {
        eventAction.append(fixedEmail);
        m_eventGuidList.insert(fixedEmail, makeEventGuidList(fixedEmail, rule->second.actions.emailAction.itemIDs));
    }

    m_ruleId = QString::fromStdString(rule->first);
    setRuleName(QString::fromStdString(rule->second.name));
    m_scheduleId = QString::fromStdString(rule->second.scheduleID);
    m_schedule = m_scheduleMap[m_scheduleId];
    m_isEnabled = rule->second.isEnabled;
    setDuration(rule->second.waitingSecondTime);
    setSchedule(m_schedule);
    setAddedEventList(eventTrigger);
    setRemainingEventList();
    setAddedActionList(eventAction);
    setExceptActionList();

}

QStringList AddEventRuleViewModel::makeEventGuidList(QString eventName, std::set<Wisenet::Core::EventRule::ItemID> itemIds)
{
    QStringList eventGuidList;
    for(auto &item : itemIds){
        QString guid = makeEventGuid(eventName, item);
        SPDLOG_DEBUG("AddEventRuleViewModel::makeEventGuidList eventName:{} -- guid:{}", eventName.toStdString(), guid.toStdString());
        eventGuidList.push_back(guid);
    }
    return eventGuidList;
}

QString AddEventRuleViewModel::makeEventGuid(QString eventName, Wisenet::Core::EventRule::ItemID item)
{
    QString guid =  QString::fromStdString(item.deviceID);

    if(eventName == fixedAlarmInput || eventName == fixedNetworkAlarmInput || eventName == fixedAlarmOut){
        if(eventName == fixedNetworkAlarmInput)
            guid += ("_" + QString::fromStdString(item.channelID));
        guid += ("_" + QString::fromStdString(item.alarmID)) ;
    }
    else if(fixedSystemEvent.contains(eventName)){
        ;
    }
    else if(eventName == fixedEmail) {
        ;
    }
    else{
        guid += ("_" + QString::fromStdString(item.channelID));
    }
    return guid;
}

void AddEventRuleViewModel::makeDefaultEventItem(QString setId, QString item, Wisenet::Core::EventRule::ItemID &itemId)
{
    auto tokens = setId.split('_');

    if(tokens.size() > 0)
        itemId.deviceID = tokens.at(0).toStdString();


    if(item == fixedAlarmInput || item == fixedNetworkAlarmInput || item == fixedAlarmOut) {
        if(tokens.size() == 2){
            itemId.alarmID = tokens.at(1).toStdString();
        }
        else if(tokens.size() == 3){
            itemId.channelID = tokens.at(1).toStdString();
            itemId.alarmID = tokens.at(2).toStdString();
        }

        itemId.isAlarm = true;
    }
    else if(fixedSystemEvent.contains(item)){
        itemId.isDevice = true;
    }
    else if (item == fixedEmail) {
        itemId.isAlarm = true;
    }
    else{
        itemId.isChannel = true;
        if(tokens.size() > 1)
            itemId.channelID = tokens.at(1).toStdString();
    }
}

void AddEventRuleViewModel::saveRule(int alarmDuration)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::saveRule Name:{} {} m_eventTriggerMap size:{} m_eventActionMap size:{} -- alarmDuration:{} ",
                 m_ruleName.toStdString(), m_duration,
                 m_eventTriggerMap.size(), m_eventActionMap.size(),
                 alarmDuration
                 );

    auto request = std::make_shared<Wisenet::Core::SaveEventRuleRequest>();
    auto eventRules = QCoreServiceManager::Instance().DB()->GetEventRules();

    Wisenet::Core::EventRule eventRule;
    eventRule.eventRuleID = m_ruleId.toStdString();
    eventRule.name = m_ruleName.toStdString();
    eventRule.isEnabled = m_isEnabled;
    m_scheduleId = m_scheduleMap.key(m_schedule);
    eventRule.scheduleID = m_scheduleId.toStdString();
    eventRule.waitingSecondTime = m_duration;

    for(auto& item: m_addedEventList){
        SPDLOG_DEBUG("AddEventRuleViewModel::saveRule m_addedEventList");

        Wisenet::Core::EventRule::Event event;
        event.type = item.toStdString();

        auto eventTrigger = m_eventTriggerMap.find(event.type);
        if(eventTrigger != m_eventTriggerMap.end()){
            SPDLOG_DEBUG("AddEventRuleViewModel::saveRule Trigger mapFind");
            event.isAllItem = eventTrigger->second.isAll;

            if(event.isAllItem == false){
                for(auto& guid : eventTrigger->second.ids){
                    /* guid
                    Name:AlarmInput 75410404-d6e6-4e50-bcee-5cf549dbbb6e_2
                    Name:AlarmInput 75410404-d6e6-4e50-bcee-5cf549dbbb6e_9_1
                    Name:MotionDetection 75410404-d6e6-4e50-bcee-5cf549dbbb6e_2
                    Name:SystemEvent 75410404-d6e6-4e50-bcee-5cf549dbbb6e
                    */
                    Wisenet::Core::EventRule::ItemID itemId;

                    makeDefaultEventItem(guid, item, itemId);
                    event.itemIDs.emplace(itemId);
                }
            }
        }
        eventRule.events.push_back(event);
    }

    for(auto& item: m_addedActionList){
        SPDLOG_DEBUG("AddEventRuleViewModel::saveRule action (1) : {}", item.toStdString());

        std::string mainEventName = item.toStdString();

        auto eventAction = m_eventActionMap.find(mainEventName);

        if(eventAction != m_eventActionMap.end()){
            if(item == fixedAlarmOut){
                eventRule.actions.alarmOutputAction.useAction = true;
                eventRule.actions.alarmOutputAction.intervalSeconds = alarmDuration;
            }
            else if(item == fixedAlertAlarm){
                eventRule.actions.alertAlarmAction.useAction = true;
            }
            else if( item == fixedLayoutChange){
                eventRule.actions.layoutChangeAction.useAction = true;
            }
            else { // fixedEmail
                eventRule.actions.emailAction.useAction = true;
            }

            for(auto& guid : eventAction->second.ids){
                Wisenet::Core::EventRule::ItemID itemId;
                SPDLOG_DEBUG("AddEventRuleViewModel::saveRule action (2) : {}", guid.toStdString());
                makeDefaultEventItem(guid, item, itemId);
                if(item == fixedAlarmOut){
                    eventRule.actions.alarmOutputAction.itemIDs.insert(itemId);
                }
                else if(item == fixedAlertAlarm){
                    eventRule.actions.alertAlarmAction.useAction = true;
                    eventRule.actions.alertAlarmAction.itemIDs.insert(itemId);
                }
                else if( item == fixedLayoutChange){
                    eventRule.actions.layoutChangeAction.useAction = true;
                    eventRule.actions.layoutChangeAction.layoutID = guid.toStdString();
                }
                else { // fixedEmail
                    eventRule.actions.emailAction.useAction = true;
                    eventRule.actions.emailAction.itemIDs.insert(itemId);
                }
            }
        }
    }

    request->eventRule = eventRule;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveEventRule,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "AddEventRuleViewModel::saveRule() response:" << QString::fromStdString(response->errorString());
        if(response->isSuccess()){
            emit resultMessage(true, "Success");
        }
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));

    });
}

void AddEventRuleViewModel::setTotalList()
{
    // 2022.12.28. converity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    auto totalTriggerList = db->GetTotalEventList();
    m_totalTriggerList = TranslationSort::getInstance()->sort(totalTriggerList);


    m_totalActionList.clear();
    m_totalActionList << fixedAlarmOut << fixedAlertAlarm << fixedLayoutChange << fixedEmail;
}

void AddEventRuleViewModel::setAddedEventList(const QStringList& eventList)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setAddedEventList count:{}",eventList.count());
    if(eventList != m_addedEventList){
        m_addedEventList = eventList;
        emit this->addedEventListChanged();

        setIsFullTrigger(eventList.count() >= maxTrigger);
        setIsAvailableDuration(eventList.count() >= availableDuration);
        setRemainingEventList();
    }
}

void AddEventRuleViewModel::setIsFullTrigger(bool bFull)
{
    if(bFull != m_isFullTrigger){
        m_isFullTrigger = bFull;
        emit this->isFullTriggerChanged();
    }
}

void AddEventRuleViewModel::setIsAvailableDuration(bool bUse)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setIsAvailableDuration ");
    if(bUse != m_isAvailableDuration){
        SPDLOG_DEBUG("AddEventRuleViewModel::setIsAvailableDuration 1");
        m_isAvailableDuration = bUse;
        emit this->isAvailableDurationChanged();
        SPDLOG_DEBUG("AddEventRuleViewModel::setIsAvailableDuration 2");
    }
}

void AddEventRuleViewModel::appendEventList(QString event)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::appendEventList Name:{}",event.toStdString());

    if(!m_addedEventList.contains(event)){
        m_addedEventList.append(event);
        emit this->addedEventListChanged();

        setIsFullTrigger(m_addedEventList.count() >= maxTrigger);
        setIsAvailableDuration(m_addedEventList.count() >= availableDuration);
        setRemainingEventList();
    }
}

void AddEventRuleViewModel::removeEventList(QString event)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::removedEventList Name:{}",event.toStdString());

    if(m_addedEventList.contains(event)){
        m_addedEventList.removeAll(event);
        emit this->addedEventListChanged();

        setIsFullTrigger(m_addedEventList.count() >= maxTrigger);
        setIsAvailableDuration(m_addedEventList.count() >= availableDuration);
        setRemainingEventList();

        auto itr = m_eventTriggerMap.find(event.toStdString());
        if(itr != m_eventTriggerMap.end())
            m_eventTriggerMap.erase(event.toStdString());

        auto itr2 = m_eventTriggerAll.find(event);
        if(itr2 != m_eventTriggerAll.end())
            m_eventTriggerAll.erase(itr2);
    }
}

void AddEventRuleViewModel::setRemainingEventList()
{   
    QStringList eventList = m_totalTriggerList;

    for(auto& item : m_addedEventList){
        eventList.removeAll(item);
    }


    if(m_remainingEventList != eventList){
        m_remainingEventList = eventList;
        emit remainingEventListChanged();
    }
}

void AddEventRuleViewModel::setAddedActionList(const QStringList& eventList)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setAddedActionList count:{}",eventList.count());

    if(eventList != m_addedActionList){
        SPDLOG_DEBUG("AddEventRuleViewModel::setAddedActionList count:{} count2:{}",eventList.count(), m_addedActionList.count());
        m_addedActionList = eventList;
        emit this->addedActionListChanged();

        setIsFullAction(eventList.count() >= maxAction);
        setExceptActionList();
    }
}

void AddEventRuleViewModel::setIsFullAction(bool bFull)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::setIsFullAction full:{}",bFull);

    if(bFull != m_isFullAction){
        m_isFullAction = bFull;
        emit this->isFullActionChanged();
    }
}

void AddEventRuleViewModel::appendActionList(QString event)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::appendActionList Name:{}",event.toStdString());

    if(!m_addedActionList.contains(event)){
        m_addedActionList.append(event);
        emit this->addedActionListChanged();

        setIsFullAction(m_addedActionList.count() >= maxAction);
        setExceptActionList();
    }
}

void AddEventRuleViewModel::removeActionList(QString event)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::removeActionList Name:{} m_addedActionList.count:{}",event.toStdString(), m_addedActionList.count());

    if(m_addedActionList.contains(event)){
        m_addedActionList.removeAll(event);
        emit this->addedActionListChanged();
        setIsFullAction(m_addedActionList.count() >= maxAction);
        setExceptActionList();

        auto itr = m_eventActionMap.find(event.toStdString());
        if(itr != m_eventActionMap.end())
            m_eventActionMap.erase(event.toStdString());

        if(event == fixedAlarmOut)
            m_alarmoutDuration = 0;
    }
}

void AddEventRuleViewModel::setExceptActionList()
{
    QStringList eventList = m_totalActionList;

    for(auto& item : m_addedActionList){
        eventList.removeAll(item);
    }

    if(m_exceptActionList != eventList){
        m_exceptActionList = eventList;
        emit exceptActionListChanged();
    }
}

void AddEventRuleViewModel::clearEventTree()
{
    m_eventTriggerMap.clear();
    m_eventActionMap.clear();

    SPDLOG_DEBUG("AddEventRuleViewModel::AddEventRuleViewModel::clearEventTree() {} {}", m_eventTriggerMap.size(), m_eventActionMap.size());
}

void AddEventRuleViewModel::appendEventTree(bool isTrigger, QString event, bool isAll, std::vector<QString> items)
{
    SPDLOG_DEBUG("AddEventRuleViewModel::appenEventTree isTrigger:{} event:{} isAll:{}", isTrigger,event.toStdString(), isAll);
    std::string strEvent = event.toStdString();

    eventRuleTree tree;
    tree.isAll = isAll;

    for(auto& item : items){
        tree.ids.insert(item);
        SPDLOG_DEBUG("AddEventRuleViewModel::appenEventTree items:{}", item.toStdString());
    }

    if(isTrigger)
        m_eventTriggerMap[strEvent] = tree;
    else
        m_eventActionMap[strEvent] = tree;

    SPDLOG_DEBUG("AddEventRuleViewModel::appenEventTree {} {}", m_eventTriggerMap.size(), m_eventActionMap.size());
}

QString AddEventRuleViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

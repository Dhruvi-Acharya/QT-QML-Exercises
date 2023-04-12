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
#include <QAbstractListModel>
#include "QCoreServiceManager.h"

class AddEventRuleViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString ruleId READ ruleId WRITE setRuleId NOTIFY ruleIdChanged)
    Q_PROPERTY(QString ruleName READ ruleName WRITE setRuleName NOTIFY ruleNameChanged)
    Q_PROPERTY(int duration READ duration WRITE setDuration NOTIFY durationChanged)
    Q_PROPERTY(QString schedule READ schedule WRITE setSchedule NOTIFY scheduleChanged)

    Q_PROPERTY(QStringList addedEventList READ addedEventList  NOTIFY addedEventListChanged)
    Q_PROPERTY(bool isFullTrigger READ isFullTrigger NOTIFY isFullTriggerChanged)
    Q_PROPERTY(bool isAvailableDuration READ isAvailableDuration NOTIFY isAvailableDurationChanged)
    Q_PROPERTY(QStringList remainingEventList READ remainingEventList  NOTIFY remainingEventListChanged)

    Q_PROPERTY(QStringList addedActionList READ addedActionList  NOTIFY addedActionListChanged)
    Q_PROPERTY(bool isFullAction READ isFullAction NOTIFY isFullActionChanged)
    Q_PROPERTY(QStringList exceptActionList READ exceptActionList  NOTIFY exceptActionListChanged)

  public:

    const int maxTrigger = 4;
    const int availableDuration = 2;
    const int maxAction = 4;
    const int defaultDuration = 4;

    const QString fixedAlarmOut = "Alarm output";
    const QString fixedAlertAlarm = "AlertAlarm";
    const QString fixedLayoutChange = "Open layout";
    const QString fixedEmail = "E-mail";
    const QVector<QString> fixedSystemEvent = {"DevicePowerStatus", "DeviceFanStatus",
                                              "DeviceStorageStatus", "DeviceRecordingStatus",
                                              "DeviceSystemStatus"};
    const QString fixedAlarmInput = "AlarmInput";
    const QString fixedNetworkAlarmInput = "NetworkAlarmInput";


    AddEventRuleViewModel(QObject* parent = nullptr);
    ~AddEventRuleViewModel();

    Q_INVOKABLE void readRule(QString ruleId);
    Q_INVOKABLE void saveRule(int alarmDuration);

    Q_INVOKABLE void clearEventTree();
    Q_INVOKABLE void appendEventTree(bool isTrigger, QString event, bool isAll, std::vector<QString> items);

    Q_INVOKABLE void appendEventList(QString event);
    Q_INVOKABLE void removeEventList(QString event);

    Q_INVOKABLE void appendActionList(QString event);
    Q_INVOKABLE void removeActionList(QString event);

    Q_INVOKABLE QVariantMap getEventGuidList();
     Q_INVOKABLE QVariantMap getEventTriggerAll();

    Q_INVOKABLE int getAlarmoutDuration();
    Q_INVOKABLE QString translate(QVariant display);
    Q_INVOKABLE QString getSchedule();
    Q_INVOKABLE QStringList scheduleNameList() {return m_scheduleNameList;};


    //getter
    QString ruleId() { return m_ruleId;}
    QString ruleName() { return m_ruleName;}
    int duration() { return m_duration;}
    QString schedule() {return m_schedule;}

    QStringList remainingEventList() { return m_remainingEventList;}
    QStringList addedEventList() { return m_addedEventList;}
    bool isFullTrigger() { return m_isFullTrigger;}
    bool isAvailableDuration() { return m_isAvailableDuration;}

    QStringList exceptActionList() { return m_exceptActionList;}
    QStringList addedActionList() { return m_addedActionList;}
    bool isFullAction() { return m_isFullAction;}

    //setter
    void setRuleId(const QString& ruleId);
    void setRuleName(const QString& ruleName);
    void setDuration(const int duration);
    void setSchedule(const QString& schedule);

    void setTotalList();

    void setAddedEventList(const QStringList& eventList);
    void setRemainingEventList();
    void setIsFullTrigger(bool bFull);
    void setIsAvailableDuration(bool bUse);

    void setAddedActionList(const QStringList& eventList);
    void setExceptActionList();
    void setIsFullAction(bool bFull);

    void setScheduleNameList();

signals:
    void ruleIdChanged();
    void ruleNameChanged();
    void durationChanged(int duration);
    void scheduleChanged();
    void scheduleNameListChanged();

    void remainingEventListChanged();
    void addedEventListChanged();
    void isFullTriggerChanged();
    void isAvailableDurationChanged();

    void exceptActionListChanged();
    void addedActionListChanged();
    void isFullActionChanged();
    void resultMessage(bool success, QString msg);
private:
    void makeDefaultEventItem(QString setId, QString item, Wisenet::Core::EventRule::ItemID &itemId);
    QString makeEventGuid(QString eventName, Wisenet::Core::EventRule::ItemID itemId);
    QStringList makeEventGuidList(QString eventName, std::set<Wisenet::Core::EventRule::ItemID> itemIds);


    QString m_ruleId;
    QString m_scheduleId;
    bool    m_isEnabled = false;
    QString m_ruleName;
    int m_duration = 0;
    QString m_schedule = "";

    QStringList m_totalTriggerList;
    QStringList m_remainingEventList;
    QStringList m_addedEventList;
    bool m_isFullTrigger = false;
    bool m_isAvailableDuration = false;

    QStringList m_totalActionList;
    QStringList m_exceptActionList;
    QStringList m_addedActionList;
    bool m_isFullAction = false;

    QMap<QString, QString> m_scheduleMap;
    QStringList m_scheduleNameList;

    struct eventRuleTree{
        bool isAll = false;
        std::set<QString> ids;
    };

    std::map<std::string, eventRuleTree> m_eventTriggerMap;
    std::map<std::string, eventRuleTree> m_eventActionMap;

    QMap<QString, QStringList>  m_eventGuidList;
    QMap<QString, bool> m_eventTriggerAll;
    int m_alarmoutDuration = 0;

};


#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "../../Model/EventListModel.h"
#include "../../Model/EventModel.h"

class NotificationViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY (EventListModel* eventListModel READ eventListModel NOTIFY eventListModelChangedEvent)

public:
    static NotificationViewModel* getInstance()
    {
        static NotificationViewModel instance;
        return &instance;
    }

    explicit NotificationViewModel(QObject *parent = nullptr);
    ~NotificationViewModel();

    EventListModel* eventListModel()
    {
        return &m_eventListModel;
    }

    void clear();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void NewEvent(QCoreServiceEventPtr event);

public slots:
    void test();
    void test2();

signals:
    void eventListModelChangedEvent(EventListModel* arg);
    void newSystemAlarmReceived(QString sourceName, QString alarmType);

private:
    EventListModel m_eventListModel;
};

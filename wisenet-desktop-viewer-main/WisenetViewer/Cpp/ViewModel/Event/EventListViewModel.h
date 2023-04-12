#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "../../Model/EventListModel.h"
#include "../../Model/EventModel.h"
#include "../../Model/EventListFilterProxyModel.h"

class EventListViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(EventListModel* eventListModel READ eventListModel NOTIFY eventListModelChangedEvent)
    Q_PROPERTY(EventListFilterProxyModel* eventListFilterProxyModel READ eventListFilterProxyModel NOTIFY eventListFilterProxyModelChangedEvent)
    Q_PROPERTY(bool eventTestVisible READ eventTestVisible WRITE setEventTestVisible NOTIFY eventTestVisibleChanged)

public:
    static EventListViewModel* getInstance()
    {
        static EventListViewModel instance;
        return &instance;
    }

    explicit EventListViewModel(QObject *parent = nullptr);
    ~EventListViewModel();

    EventListModel* eventListModel()
    {
        return &m_eventListModel;
    }

    EventListFilterProxyModel* eventListFilterProxyModel()
    {
        return &m_eventListFilterProxyModel;
    }

    bool eventTestVisible(){ return m_eventTestVisible; }
    void setEventTestVisible(bool eventTestVisible)
    {
        m_eventTestVisible = eventTestVisible;
        emit eventTestVisibleChanged(eventTestVisible);
    }

    void clear();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

private:
    void Event_ShowEvent(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);
    void Event_PriorityAlarmEvent(QCoreServiceEventPtr event);

public slots:
    void testPriorityAlarm();

signals:
    void eventListModelChangedEvent(EventListModel* arg);
    void eventListFilterProxyModelChangedEvent(EventListFilterProxyModel* arg);
    void eventTestVisibleChanged(bool eventTestVisible);

private:
    EventListModel m_eventListModel;
    EventListFilterProxyModel m_eventListFilterProxyModel;
    bool m_eventTestVisible = false;
};

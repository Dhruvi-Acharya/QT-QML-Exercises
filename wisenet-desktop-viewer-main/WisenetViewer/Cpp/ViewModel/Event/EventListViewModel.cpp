#include "EventListViewModel.h"

EventListViewModel::EventListViewModel(QObject* parent) :
    QObject(parent)
{
    setEventTestVisible(QCoreServiceManager::Instance().Settings()->eventTestVisible());
    m_eventListModel.setMaxEventCount(QCoreServiceManager::Instance().Settings()->maxEventCount());
    m_eventListFilterProxyModel.setSourceModel(&m_eventListModel);

    qDebug() << "EventListViewModel::~EventListViewModel()";
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &EventListViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

EventListViewModel::~EventListViewModel()
{
    qDebug() << "EventListViewModel::~EventListViewModel()";
    //m_eventListModel.removeAll();
}

void EventListViewModel::clear()
{
    qDebug() << "EventListViewModel::clear()";
    m_eventListModel.removeAll();
}

void EventListViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    if(serviceEvent->EventTypeId() == Wisenet::Core::ShowEventPanelEventType)
    {
        Event_ShowEvent(event);
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::SaveChannelsEventType)
    {
        Event_SaveChannels(event);
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::PriorityAlarmEventType)
    {
        Event_PriorityAlarmEvent(event);
    }
}

void EventListViewModel::Event_ShowEvent(QCoreServiceEventPtr event)
{
    auto alarmEvent = std::static_pointer_cast<Wisenet::Core::ShowEventPanelEvent>(event->eventDataPtr);

    Wisenet::Core::EventLog log = alarmEvent->eventLog;
    Wisenet::Device::Device::Channel channel;

    if(!QCoreServiceManager::Instance().DB()->FindChannel(log.deviceID, log.channelID, channel))
        return;

    long long serviceUtcTimeMsec = log.serviceUtcTimeMsec;
    long long deviceUtcTimeMsec = log.deviceUtcTimeMsec;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(serviceUtcTimeMsec);
    EventModel* model = new EventModel();

    QString deviceId = QString::fromStdString(log.deviceID);
    QString channelId = QString::fromStdString(log.channelID);
    QString channelName = QString::fromUtf8(channel.name.c_str());
    QString alarmType = QString::fromStdString(log.type);
    QString time = QLocaleManager::Instance()->getDateTime(dateTime);
    model->setSourceId(deviceId + "_" + channelId);
    model->setSourcelName(channelName);
    model->setAlarmType(alarmType);
    model->setAlarmName(QCoreApplication::translate("WisenetLinguist", alarmType.toStdString().c_str()));
    model->setOccurrenceTime(time);
    model->setUtcTime(serviceUtcTimeMsec);
    model->setDeviceTime(deviceUtcTimeMsec);

    m_eventListModel.insertFirst(model);
}

void EventListViewModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        QString id = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);
        QString name = QString::fromUtf8( channel.name.c_str());

        m_eventListModel.applySaveChannels(id, name);

        qDebug() << "EventListViewModel::Event_SaveChannels() Channel:"  << id << name;
    }
}

void EventListViewModel::Event_PriorityAlarmEvent(QCoreServiceEventPtr event)
{
    auto priorityAlarmEvent = std::static_pointer_cast<Wisenet::Core::PriorityAlarmEvent>(event->eventDataPtr);

    Wisenet::Device::Device::Channel channel;

    if(!QCoreServiceManager::Instance().DB()->FindChannel(priorityAlarmEvent->deviceID, priorityAlarmEvent->channelID, channel))
        return;

    long long serviceUtcTimeMsec = priorityAlarmEvent->serviceUtcTimeMsec;
    long long deviceUtcTimeMsec = priorityAlarmEvent->deviceUtcTimeMsec;

    QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(serviceUtcTimeMsec);
    EventModel* model = new EventModel();

    QString deviceId = QString::fromStdString(priorityAlarmEvent->deviceID);
    QString channelId = QString::fromStdString(priorityAlarmEvent->channelID);
    QString channelName = QString::fromUtf8(channel.name.c_str());
    QString alarmType = QString::fromUtf8(priorityAlarmEvent->eventRuleName.c_str());
    QString time = QLocaleManager::Instance()->getDateTime(dateTime);
    model->setSourceId(deviceId + "_" + channelId);
    model->setSourcelName(channelName);
    model->setAlarmType(alarmType);
    model->setAlarmName(alarmType);
    model->setOccurrenceTime(time);
    model->setUtcTime(serviceUtcTimeMsec);
    model->setDeviceTime(deviceUtcTimeMsec);
    model->setIsPriorityAlarm(true);

    m_eventListModel.insertFirst(model);
}

void EventListViewModel::testPriorityAlarm()
{
    EventModel* model = new EventModel();

    long long msec = QDateTime::currentMSecsSinceEpoch();
    QDateTime dateTime =  QDateTime::fromMSecsSinceEpoch(msec);

    QString deviceId = QCoreServiceManager::Instance().Settings()->eventTestDeviceId();
    QString channelId = QCoreServiceManager::Instance().Settings()->eventTestChannelId();

    Wisenet::Device::Device::Channel channel;
    if(!QCoreServiceManager::Instance().DB()->FindChannel(deviceId, channelId, channel))
    {
        qDebug() << "EventListViewModel::testPriorityAlarm() return - cannot find channel " << deviceId << channelId;
        return;
    }

    QString channelName = QString::fromUtf8(channel.name.c_str());
    QString alarmType = "AlertAlarm";
    QString time = QLocaleManager::Instance()->getDateTime(dateTime);
    QString sourceId = deviceId + "_" + channelId;
    model->setSourceId(sourceId);
    model->setSourcelName(channelName);
    model->setAlarmType(alarmType);
    model->setOccurrenceTime(time);
    model->setUtcTime(msec);
    model->setDeviceTime(msec);
    model->setIsPriorityAlarm(true);

    qDebug() << "EventListViewModel::testPriorityAlarm()" << sourceId << deviceId << channelId << channelName << time;

    m_eventListModel.insertFirst(model);
}

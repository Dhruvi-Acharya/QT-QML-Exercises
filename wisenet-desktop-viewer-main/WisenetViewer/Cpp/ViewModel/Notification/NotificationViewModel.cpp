#include "NotificationViewModel.h"

NotificationViewModel::NotificationViewModel(QObject* parent) :
    QObject(parent)
{
    qDebug() << "NotificationViewModel::~NotificationViewModel()";
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &NotificationViewModel::coreServiceEventTriggered, Qt::QueuedConnection);

//    test();
}

NotificationViewModel::~NotificationViewModel()
{
    qDebug() << "NotificationViewModel::~NotificationViewModel()";
}

void NotificationViewModel::clear()
{
    qDebug() << "NotificationViewModel::clear()";
    m_eventListModel.removeAll();
}

void NotificationViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    if(serviceEvent->EventTypeId() == Wisenet::Core::ShowSystemAlarmPanelEventType)
    {
        SPDLOG_INFO("NotificationViewModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());
        NewEvent(event);
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::ShowSystemUpdatePanelEventType)
    {
        EventModel* model = new EventModel();

        QString deviceName = "";
        QString alarmType = "SystemEvent.UpdateSoftware";

        model->setSourcelName(deviceName);
        model->setAlarmType(alarmType);

        m_eventListModel.append(model);

        emit newSystemAlarmReceived(deviceName, alarmType);
    }
}

void NotificationViewModel::NewEvent(QCoreServiceEventPtr event)
{
    auto alarmEvent = std::static_pointer_cast<Wisenet::Core::ShowSystemAlarmPanelEvent>(event->eventDataPtr);

    Wisenet::Core::EventLog log = alarmEvent->eventLog;

    qDebug() <<"NotificationViewModel::NewEvent";

    if(log.isService)
    {
        qDebug() << "isService:" << log.isService << ", type:" << QString::fromStdString(log.type) << ", id:" << QString::fromStdString(log.serviceID);

        if(log.type == "SystemEvent.IFrameMode" || log.type == "SystemEvent.FullFrameMode")
        {
            EventModel* model = new EventModel();

            QString alarmType = QString::fromStdString(log.type);
            QString time = QLocaleManager::Instance()->getDateTimeFromMsec(log.serviceUtcTimeMsec);

            model->setSourcelName(QCoreApplication::translate("WisenetLinguist","Service"));
            model->setAlarmType(alarmType);
            model->setOccurrenceTime(time);
            model->setUtcTime(log.serviceUtcTimeMsec);

            m_eventListModel.append(model);

            emit newSystemAlarmReceived(QCoreApplication::translate("WisenetLinguist","Service"), alarmType);
        }
    }
    else if(log.isDevice)
    {
        qDebug() << "isDevice:" << log.isDevice << ", type:" << QString::fromStdString(log.type) << ", id:" << QString::fromStdString(log.deviceID);

        Wisenet::Device::Device device;
        if(!QCoreServiceManager::Instance().DB()->FindDevice(QString::fromStdString(log.deviceID), device))
            return;

        long long serviceUtcTimeMsec = log.serviceUtcTimeMsec;
        //long long deviceUtcTimeMsec = log.deviceUtcTimeMsec;

        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(serviceUtcTimeMsec);
        EventModel* model = new EventModel();

        QString deviceId = QString::fromStdString(log.deviceID);
        QString deviceName = QString::fromUtf8(device.name.c_str());
        QString alarmType = QString::fromStdString(log.type);
        QString time = QLocaleManager::Instance()->getDateTime(dateTime);
        model->setSourceId(deviceId);
        model->setSourcelName(deviceName);
        model->setAlarmType(alarmType);
        model->setOccurrenceTime(time);
        model->setUtcTime(serviceUtcTimeMsec);

        m_eventListModel.append(model);

//        qDebug() << "deviceName:" << deviceName << "alarmType:" << alarmType;
        emit newSystemAlarmReceived(deviceName, alarmType);
    }
    else if(log.isStart)
    {
        qDebug() << "isStart:" << log.isStart << ", type:" << QString::fromStdString(log.type) << ", id:" << QString::fromStdString(log.serviceID);
    }
    else if(log.isChannel)
    {
        Wisenet::Device::Device::Channel channel;

        if(!QCoreServiceManager::Instance().DB()->FindChannel(log.deviceID, log.channelID, channel))
            return;

        long long serviceUtcTimeMsec = log.serviceUtcTimeMsec;
        //long long deviceUtcTimeMsec = log.deviceUtcTimeMsec;

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
        model->setOccurrenceTime(time);
        model->setUtcTime(serviceUtcTimeMsec);

        m_eventListModel.append(model);

        emit newSystemAlarmReceived(channelName, alarmType);
    }
}

void NotificationViewModel::test()
{
    EventModel* model = new EventModel();

    model->setSourcelName("ARN-1611");
    model->setAlarmType("SystemEvent.RecordFiltering");
    model->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model);

    EventModel* model2 = new EventModel();

    model2->setSourcelName("ARN-221");
    model2->setAlarmType("SystemEvent.RAIDFail");
    model2->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model2->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model2);

    EventModel* model3 = new EventModel();

    model3->setSourcelName("ARN-161331");
    model3->setAlarmType("SystemEvent.NetTxTrafficOverflow");
    model3->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model3->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model3);

    EventModel* model4 = new EventModel();

    model4->setSourcelName("ARN-1611");
    model4->setAlarmType("SystemEvent.iSCSIDisconnect");
    model4->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model4->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model4);

    EventModel* model5 = new EventModel();

    model5->setSourcelName("ARN-161111111111111111111111111111111111111111111111111");
    model5->setAlarmType("SystemEvent.NASDisconnect");
    model5->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model4->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model5);


    EventModel* model6 = new EventModel();

    model6->setSourcelName("ARN-161111111111111111111111111111111111111111111111111");
    model6->setAlarmType("SystemEvent.CpuOverload");
    model6->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model4->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model6);


    EventModel* model7 = new EventModel();

    model7->setSourcelName("ARN-1610");
    model7->setAlarmType("SystemEvent.CoreService.FirmwareUpgrade");
    model7->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model4->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model7);


    EventModel* model8 = new EventModel();

    model8->setSourcelName("ARN-1610");
    model8->setAlarmType("SystemEvent.UpdateSoftware");
    model8->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model4->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model8);
}

void NotificationViewModel::test2()
{
    EventModel* model = new EventModel();

    QString alarmType = "SystemEvent.CoreService.DeviceDisconnectedWithRestriction";

    model->setSourcelName("ARN-1611");
    model->setAlarmType(alarmType);
    model->setOccurrenceTime("yyyy-MM-dd hh:mm:ss");
    //model->setUtcTime(serviceUtcTimeMsec);

    m_eventListModel.append(model);

    emit newSystemAlarmReceived("ARN-1611", alarmType);
}

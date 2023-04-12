#include "DashboardViewModel.h"

DashboardViewModel::DashboardViewModel(QObject* parent) :
    QObject(parent)
{
    qDebug() << "DashboardViewModel::~DashboardViewModel()";
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DashboardViewModel::coreServiceEventTriggered, Qt::QueuedConnection);

    initialize();

    m_disconnectedCameraListProxy.setSourceModel(&m_disconnectedCameraList);
    m_connectedCameraListProxy.setSourceModel(&m_connectedCameraList);

    connect(&m_groupTreeModel, &DashboardGroupTreeModel::invalidate, &m_groupTreeProxyModel, &TreeProxyModel::invalidate);
    connect(&m_deviceTreeModel, &DashboardDeviceTreeModel::invalidate, &m_deviceTreeProxyModel, &TreeProxyModel::invalidate);

    m_groupTreeProxyModel.setSourceModel(&m_groupTreeModel);
    m_deviceTreeProxyModel.setSourceModel(&m_deviceTreeModel);

    m_groupTreeProxyModel.doSort(Qt::SortOrder::AscendingOrder);
    m_deviceTreeProxyModel.doSort(Qt::SortOrder::AscendingOrder);
}

DashboardViewModel::~DashboardViewModel()
{
    qDebug() << "DashboardViewModel::~DashboardViewModel()";
}

void DashboardViewModel::initialize()
{
    m_powerErrorListModel.appendEventType("SystemEvent.DualSMPSFail");

    m_overloadErrorListModel.appendEventType("SystemEvent.CpuOverload");
    m_overloadErrorListModel.appendEventType("SystemEvent.MemoryError");
    m_overloadErrorListModel.appendEventType("SystemEvent.NetCamTrafficOverFlow");
    m_overloadErrorListModel.appendEventType("SystemEvent.NetTxTrafficOverflow");
    m_overloadErrorListModel.appendEventType("SystemEvent.VPUError");

    m_diskErrorListModel.appendEventType("SystemEvent.HDDError");
    m_diskErrorListModel.appendEventType("SystemEvent.HDDFail");
    m_diskErrorListModel.appendEventType("SystemEvent.HDDFull");
    m_diskErrorListModel.appendEventType("SystemEvent.HDDNone");
    m_diskErrorListModel.appendEventType("SystemEvent.RAIDDegrade");
    m_diskErrorListModel.appendEventType("SystemEvent.RAIDFail");
    m_diskErrorListModel.appendEventType("SystemEvent.RAIDRebuildStart");
    m_diskErrorListModel.appendEventType("SystemEvent.RecordFiltering");
    m_diskErrorListModel.appendEventType("SystemEvent.RecordingError");
    m_diskErrorListModel.appendEventType("SystemEvent.iSCSIDisconnect");
    m_diskErrorListModel.appendEventType("SystemEvent.SDFail");
    m_diskErrorListModel.appendEventType("SystemEvent.SDFull");

    m_fanErrorListModel.appendEventType("SystemEvent.FanError");
    m_fanErrorListModel.appendEventType("SystemEvent.LeftFanError");
    m_fanErrorListModel.appendEventType("SystemEvent.RightFanError");
    m_fanErrorListModel.appendEventType("SystemEvent.FrameFanError");
    m_fanErrorListModel.appendEventType("SystemEvent.CPUFanError");

    m_connectionErrorListModel.appendEventType("SystemEvent.CoreService.DeviceDisconnectedByService");
}

void DashboardViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("MainTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    {
        Event_FullInfo(event);
        emit recorderCountChanged();
        break;
    }
    case Wisenet::Core::AddGroupMappingEventType:
    case Wisenet::Core::RemoveGroupMappingEventType:
    case Wisenet::Core::UpdateGroupMappingEventType:
    {
        m_disconnectedCameraListProxy.invalidate();
        m_connectedCameraListProxy.invalidate();

        emit disconnectedCameraCountChanged();
        emit connectedCameraCountChanged();
        break;
    }
    case Wisenet::Core::SaveChannelsEventType:
        Event_SaveChannels(event);
        break;
    case Wisenet::Core::AddDeviceEventType:
        Event_AddDevice(event);
        break;
    case Wisenet::Core::RemoveDevicesEventType:
        Event_RemoveDevices(event);
        break;
    case Wisenet::Device::DeviceStatusEventType:
        Event_DeviceStatus(event);
        break;
    case Wisenet::Device::ChannelStatusEventType:
        Event_ChannelStatus(event);
        break;
    case Wisenet::Device::DeviceLatestStatusEventType:
        Event_DeviceLatestStatus(event);
        break;
    case Wisenet::Core::DashboardDataUpdatedEventType:
    {
        qDebug() << "DashboardViewModel::coreServiceEventTriggered() DashboardDataUpdatedEvent";
        refreshData();
        break;
    }
    default:
        break;
    }
}

void DashboardViewModel::printStatus()
{
    qDebug() << "DashboardViewModel::printStatus";

    auto deviceLatestEventStatus = QCoreServiceManager::Instance().DB()->GetDeviceLatestEventStatus();

    for(auto& kv : deviceLatestEventStatus)
    {
        for(auto& kv2 : kv.second)
        {
            qDebug() << QString::fromStdString(kv.first) << QString::fromStdString(kv2.first) << kv2.second;
        }
    }
}

void DashboardViewModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    qDebug() << "DashboardViewModel::Event_FullInfo() groups:" << coreEvent->groups.size() << ", devices:" << coreEvent->devices.size();

    for(auto& kv : coreEvent->devices)
    {
        auto& device = kv.second;
        auto& deviceStatus = coreEvent->devicesStatus[kv.first];
        UpdateConnectionStatus(device, deviceStatus);
    }

    for(auto& kv : coreEvent->deviceLatestEventStatus)
    {
        std::string stdDeviceId = kv.first;
        Wisenet::Device::Device device;

        if(!QCoreServiceManager::Instance().DB()->FindDevice(stdDeviceId, device))
            continue;

        UpdateDeviceError(device, kv.second);
    }
}

void DashboardViewModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    qDebug() << "DashboardViewModel::Event_SaveChannels()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        QString channelID = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);

        if(channel.use == false)
        {
            m_disconnectedCameraList.remove(channelID);
            m_connectedCameraList.remove(channelID);
        }
        else
        {
            if(!m_disconnectedCameraList.contains(channelID) && !m_connectedCameraList.contains(channelID))
            {
                Wisenet::Device::ChannelStatus channelStatus;

                if(QCoreServiceManager::Instance().DB()->FindChannelStatus(channel.deviceID, channel.channelID, channelStatus))
                {
                    ErrorItem* item = newErrorItem(channelID, QString::fromUtf8(channel.name.c_str()), 1);

                    if(channelStatus.isGood())
                        m_connectedCameraList.append(item);
                    else
                        m_disconnectedCameraList.append(item);
                }
            }
        }
    }

    m_disconnectedCameraListProxy.invalidate();
    m_connectedCameraListProxy.invalidate();

    emit disconnectedCameraCountChanged();
    emit connectedCameraCountChanged();
}

void DashboardViewModel::Event_AddDevice(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);

    QString deviceId = QString::fromStdString(coreEvent->device.deviceID);

    qDebug() << "DashboardViewModel::Event_AddDevice" << deviceId;

    emit recorderCountChanged();
}

void DashboardViewModel::UpdateDeviceError(const Wisenet::Device::Device& device, const std::map<std::string, bool>& eventStatus)
{
    QString deviceUuid = QString::fromStdString(device.deviceID);

    m_powerErrorListModel.removeDevice(deviceUuid);
    m_diskErrorListModel.removeDevice(deviceUuid);
    m_overloadErrorListModel.removeDevice(deviceUuid);
    m_fanErrorListModel.removeDevice(deviceUuid);

    for(auto& eventKeyValuePair : eventStatus)
    {
        //qDebug() << "DashboardViewModel::Event_FullInfo UpdateDeviceError" << deviceUuid << QString::fromStdString(eventKeyValuePair.first) << eventKeyValuePair.second;
        if(eventKeyValuePair.second == true)
        {
            QString eventType = QString::fromStdString(eventKeyValuePair.first);
            QString deviceName = QString::fromUtf8(device.name.c_str());
            appendSystemEvent(eventType, deviceUuid, deviceName);
        }
    }

    emit powerErrorCountChanged();
    emit diskErrorCountChanged();
    emit overloadErrorCountChanged();
    emit fanErrorCountChanged();

    emit recorderErrorCountChanged();
}

ErrorItem* DashboardViewModel::newErrorItem(const QString& objectId, const QString& objectName, const int objectType)
{
    ErrorItem* errorItem = new ErrorItem();

    errorItem->objectId = objectId;
    errorItem->objectName = objectName;
    errorItem->objectType = objectType;

    return errorItem;
}

void DashboardViewModel::appendSystemEvent(const QString& eventType, const QString& objectId, const QString& objectName)
{
    if(eventType == "SystemEvent.DualSMPSFail")
        m_powerErrorListModel.appendDevice(eventType, newErrorItem(objectId, objectName, 0));

    if(eventType == "SystemEvent.CpuOverload" || eventType == "SystemEvent.MemoryError" || eventType == "SystemEvent.NetCamTrafficOverFlow"
            ||eventType == "SystemEvent.NetTxTrafficOverflow" || eventType == "SystemEvent.VPUError")
        m_overloadErrorListModel.appendDevice(eventType, newErrorItem(objectId, objectName, 0));

    if(eventType == "SystemEvent.HDDError" || eventType == "SystemEvent.HDDFail" || eventType == "SystemEvent.HDDFull" ||
            eventType == "SystemEvent.HDDNone" || eventType == "SystemEvent.RAIDDegrade" || eventType == "SystemEvent.RAIDFail" ||
            eventType == "SystemEvent.RAIDRebuildStart" || eventType == "SystemEvent.RecordFiltering" || eventType == "SystemEvent.RecordingError" ||
            eventType == "SystemEvent.iSCSIDisconnect" || eventType == "SystemEvent.SDFail" || eventType == "SystemEvent.SDFull")
        m_diskErrorListModel.appendDevice(eventType, newErrorItem(objectId, objectName, 0));

    if(eventType == "SystemEvent.FanError" || eventType == "SystemEvent.FrameFanError" || eventType == "SystemEvent.LeftFanError" ||
            eventType == "SystemEvent.RightFanError" || eventType == "SystemEvent.CPUFanError")
        m_fanErrorListModel.appendDevice(eventType, newErrorItem(objectId, objectName, 0));
}

void DashboardViewModel::UpdateConnectionStatus(const Wisenet::Device::Device& device,
                                                const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceUuid = QString::fromStdString(device.deviceID);

    // device
    if(deviceStatus.isGood())
    {
        m_connectionErrorListModel.removeDevice(deviceUuid);
    }
    else
    {
        ErrorItem* errorItem = new ErrorItem();

        errorItem->objectId = deviceUuid;
        errorItem->objectName = QString::fromUtf8(device.name.c_str());
        errorItem->objectType = 0;

        m_connectionErrorListModel.appendDevice("SystemEvent.CoreService.DeviceDisconnectedByService", errorItem);
    }

    // Channel
    for (auto& item : device.channels) {

        if(item.second.use == false)
            continue;

        bool isNormal = false;

        std::string stdChannelId = device.deviceID + "_" + item.first;
        QString channelUuid = QString::fromStdString(stdChannelId);

        auto citr = deviceStatus.channelsStatus.find(item.first);
        if (citr != deviceStatus.channelsStatus.end()) {
            auto& channelStatus = citr->second;
            if (channelStatus.isGood())
                isNormal = true;
        }

        if(isNormal)
        {
            m_disconnectedCameraList.remove(channelUuid);
            if(!m_connectedCameraList.contains(channelUuid))
            {
                ErrorItem* errorItem = new ErrorItem();

                errorItem->objectId = channelUuid;
                errorItem->objectName = QString::fromUtf8(item.second.name.c_str());
                errorItem->objectType = 1;
                m_connectedCameraList.append(errorItem);
            }
        }
        else
        {
            m_connectedCameraList.remove(channelUuid);
            if(!m_disconnectedCameraList.contains(channelUuid))
            {
                ErrorItem* errorItem = new ErrorItem();

                errorItem->objectId = channelUuid;
                errorItem->objectName = QString::fromUtf8(item.second.name.c_str());
                errorItem->objectType = 1;

                m_disconnectedCameraList.append(errorItem);
            }
        }
    }

    m_disconnectedCameraListProxy.invalidate();
    m_connectedCameraListProxy.invalidate();

    emit connectionErrorCountChanged();
    emit disconnectedCameraCountChanged();
    emit connectedCameraCountChanged();
    emit recorderErrorCountChanged();
}

void DashboardViewModel::Event_RemoveDevices(QCoreServiceEventPtr event)
{
    auto removeDevices = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event->eventDataPtr);

    for(auto & stdDeviceId : removeDevices->deviceIDs)
    {
        QString deviceId = QString::fromStdString(stdDeviceId);

        m_powerErrorListModel.removeDevice(deviceId);
        m_diskErrorListModel.removeDevice(deviceId);
        m_overloadErrorListModel.removeDevice(deviceId);
        m_connectionErrorListModel.removeDevice(deviceId);
        m_fanErrorListModel.removeDevice(deviceId);

        m_disconnectedCameraList.removeWithDeviceId(deviceId);
        m_connectedCameraList.removeWithDeviceId(deviceId);
    }

    m_disconnectedCameraListProxy.invalidate();
    m_connectedCameraListProxy.invalidate();

    emit recorderCountChanged();

    emit powerErrorCountChanged();
    emit diskErrorCountChanged();
    emit overloadErrorCountChanged();
    emit connectionErrorCountChanged();
    emit fanErrorCountChanged();

    emit recorderErrorCountChanged();

    emit disconnectedCameraCountChanged();
    emit connectedCameraCountChanged();
}

void DashboardViewModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);
    qDebug() << "DashboardViewModel::Event_DeviceStatus deviceUuid " << deviceUuid;

    Wisenet::Device::Device device;

    if(QCoreServiceManager::Instance().DB()->FindDevice(deviceStatusEvent->deviceID, device))
    {
        UpdateConnectionStatus(device, deviceStatusEvent->deviceStatus);
    }

    refreshData();
}

void DashboardViewModel::Event_ChannelStatus(QCoreServiceEventPtr event)
{
    qDebug() << "DashboardViewModel::Event_ChannelStatus()";

    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    for (auto& channelStatus : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus status = channelStatus.second;

        Wisenet::Device::Device::Channel channel;

        if(!QCoreServiceManager::Instance().DB()->FindChannel(status.deviceID, status.channelID, channel))
            continue;

        QString channelUuid = QString::fromStdString(status.deviceID) + "_" + QString::fromStdString(status.channelID);

        if(status.isGood())
        {
            m_disconnectedCameraList.remove(channelUuid);

            if(!m_connectedCameraList.contains(channelUuid))
            {
                ErrorItem* errorItem = new ErrorItem();

                errorItem->objectId = channelUuid;
                errorItem->objectName = QString::fromUtf8(channel.name.c_str());
                errorItem->objectType = 1;
                m_connectedCameraList.append(errorItem);
            }
        }
        else
        {
            m_connectedCameraList.remove(channelUuid);
            if(!m_disconnectedCameraList.contains(channelUuid))
            {
                ErrorItem* errorItem = new ErrorItem();

                errorItem->objectId = channelUuid;
                errorItem->objectName = QString::fromUtf8(channel.name.c_str());
                errorItem->objectType = 1;

                m_disconnectedCameraList.append(errorItem);
            }
        }
    }

    m_disconnectedCameraListProxy.invalidate();
    m_connectedCameraListProxy.invalidate();

    refreshData();
}

void DashboardViewModel::Event_DeviceLatestStatus(QCoreServiceEventPtr event)
{
    auto deviceEvent = std::static_pointer_cast<Wisenet::Device::DeviceLatestStatusEvent>(event->eventDataPtr);

    //qDebug() << "DashboardViewModel::Event_DeviceLatestStatus" << QString::fromStdString(deviceEvent->deviceID) << QString::fromStdString(deviceEvent->eventKey) << deviceEvent->status;

    Wisenet::Device::Device device;
    std::map<std::string, bool> deviceStatus;

    if(!QCoreServiceManager::Instance().DB()->FindDevice(deviceEvent->deviceID, device))
        return;

    if(!QCoreServiceManager::Instance().DB()->FIndDeviceLatestEventStatus(deviceEvent->deviceID, deviceStatus))
        return;

    UpdateDeviceError(device, deviceStatus);

    refreshData();
}

void DashboardViewModel::refreshData()
{
    auto getDashboardDataRequest = std::make_shared<Wisenet::Core::GetDashboardDataRequest>();

    /*
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetDashboardData,
                this, getDashboardDataRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {

        auto response = std::static_pointer_cast<Wisenet::Core::GetDashboardDataResponse>(reply->responseDataPtr);
        if(response->isSuccess()){
            //qDebug() << "DashboardViewModel GetDashboardData success";
            QVariantMap vMap;
            QVariantMap vMap2;
            QVariantMap vMap3;
            QVariantMap vMap4;

            setDashboardData(response->dashboardData, vMap, 8);
            setDashboardData(response->dashboardData, vMap2, 4);
            setDashboardData(response->dashboardData, vMap3, 2);
            setDashboardData(response->dashboardData, vMap4, 1);

            chartDataUpdated(vMap, vMap2, vMap3, vMap4);
        }
        else
        {
            //qDebug() << "DashboardViewModel GetDashboardData fail";
        }
    });
    */
}

void DashboardViewModel::setDashboardData(std::map<int, Wisenet::Core::DashboardData>& dashboardData, QVariantMap& vMap, int interval)
{    
    QVariantList labelList;

    QVariantList datasets;

    QVariantList powerData;
    QVariantList diskData;
    QVariantList overloadData;
    QVariantList connectionData;
    QVariantList fanData;
    QVariantList disconnectedData;

    QVariantMap power;
    power.insert("label", QCoreApplication::translate("WisenetLinguist", "Power error"));
    power.insert("fill", false);
    power.insert("cubicInterpolationMode", "monotone");
    power.insert("lineTension", 0.0);
    power.insert("pointBorderColor", "#C4C4C4");
    power.insert("backgroundColor", "#FFA722");
    power.insert("borderColor", "#FFA722");

    QVariantMap disk;
    disk.insert("label", QCoreApplication::translate("WisenetLinguist", "SystemEvent.RecordingError"));
    disk.insert("fill", false);
    disk.insert("cubicInterpolationMode", "monotone");
    disk.insert("lineTension", 0.0);
    disk.insert("pointBorderColor", "#C4C4C4");
    disk.insert("backgroundColor", "#7C88EB");
    disk.insert("borderColor", "#7C88EB");

    QVariantMap overload;
    overload.insert("label", QCoreApplication::translate("WisenetLinguist", "Overload"));
    overload.insert("fill", false);
    overload.insert("cubicInterpolationMode", "monotone");
    overload.insert("lineTension", 0.0);
    overload.insert("pointBorderColor", "#C4C4C4");
    overload.insert("backgroundColor", "#D13022");
    overload.insert("borderColor", "#D13022");

    QVariantMap connection;
    connection.insert("label", QCoreApplication::translate("WisenetLinguist", "Connection error"));
    connection.insert("fill", false);
    connection.insert("cubicInterpolationMode", "monotone");
    connection.insert("lineTension", 0.0);
    connection.insert("pointBorderColor", "#C4C4C4");
    connection.insert("backgroundColor", "#4356FF");
    connection.insert("borderColor", "#4356FF");

    QVariantMap fan;
    fan.insert("label", QCoreApplication::translate("WisenetLinguist", "SystemEvent.FanError"));
    fan.insert("fill", false);
    fan.insert("cubicInterpolationMode", "monotone");
    fan.insert("lineTension", 0.0);
    fan.insert("pointBorderColor", "#C4C4C4");
    fan.insert("backgroundColor", "#CC601A");
    fan.insert("borderColor", "#CC601A");

    QVariantMap disconnected;
    disconnected.insert("label", QCoreApplication::translate("WisenetLinguist", "Disconnected"));
    disconnected.insert("fill", false);
    disconnected.insert("cubicInterpolationMode", "monotone");
    disconnected.insert("lineTension", 0.0);
    disconnected.insert("pointBorderColor", "#C4C4C4");
    disconnected.insert("backgroundColor", "#2738D0");
    disconnected.insert("borderColor", "#2738D0");

    //foreach(auto& data, dashboardData)
    QDateTime now = QDateTime::currentDateTime();
    QDateTime lastWeek = now.addDays(-6);
    QTime midnight = QTime(0,0,0,0);
    lastWeek.setTime(midnight);

    int nowSinceEpoch = (int)now.toSecsSinceEpoch();
    int lastWeekMidnightSinceEpoch = lastWeek.toSecsSinceEpoch();
    int dataCount = 0;

    while(lastWeekMidnightSinceEpoch < nowSinceEpoch)
    {
        QDateTime convertBack = QDateTime::fromSecsSinceEpoch(lastWeekMidnightSinceEpoch, Qt::LocalTime, now.offsetFromUtc());

        if(dataCount % interval == 0)
        {
            if(convertBack.time().hour() == 0)
                labelList.append(QLocaleManager::Instance()->getDate(convertBack));
            else
            {
                labelList.append(convertBack.time().toString("HH:mm"));
            }

            if(dashboardData.find(lastWeekMidnightSinceEpoch) != dashboardData.end())
            {
                Wisenet::Core::DashboardData data = dashboardData[lastWeekMidnightSinceEpoch];

                powerData.append(data.powerErrorCount);
                diskData.append(data.diskErrorCount);
                overloadData.append(data.overloadErrorCount);
                connectionData.append(data.connectionErrorCount);
                fanData.append(data.fanErrorCount);
                disconnectedData.append(data.disconnectedCameraCount);
                //qDebug() << "setDashboardData()" << convertBack << data.saveTime << data.powerErrorCount << data.diskErrorCount << data.overloadErrorCount << data.connectionErrorCount << data.fanErrorCount << data.disconnectedCameraCount;
            }
            else
            {
                //qDebug() << "setDashboardData()" << convertBack << "NO DATA";
                powerData.append("null");
                diskData.append("null");
                overloadData.append("null");
                connectionData.append("null");
                fanData.append("null");
                disconnectedData.append("null");
            }
        }

        lastWeekMidnightSinceEpoch += (3 * 60 * 60);
        dataCount++;
    }

    labelList.append("Now");

    powerData.append(powerErrorCount());
    diskData.append(diskErrorCount());
    overloadData.append(overloadErrorCount());
    connectionData.append(connectionErrorCount());
    fanData.append(fanErrorCount());
    disconnectedData.append(disconnectedCameraCount());


    power.insert("data", powerData);
    datasets.append(power);

    disk.insert("data", diskData);
    datasets.append(disk);

    overload.insert("data", overloadData);
    datasets.append(overload);

    connection.insert("data", connectionData);
    datasets.append(connection);

    fan.insert("data", fanData);
    datasets.append(fan);

    disconnected.insert("data", disconnectedData);
    datasets.append(disconnected);

    vMap.insert("labels", labelList);
    vMap.insert("datasets", datasets);
}

// for test
QVariantMap DashboardViewModel::getChartData()
{
    QVariantMap vMap;

    QVariantList labelList = {"6/1","6/2","6/3","6/4","6/5","6/6","6/7"};
    vMap.insert("labels", labelList);

    QVariantList datasets;

    QVariantMap power;
    power.insert("label", QCoreApplication::translate("WisenetLinguist", "Power error"));
    power.insert("fill", false);
    power.insert("cubicInterpolationMode", "monotone");
    power.insert("lineTension", 0.0);
    power.insert("pointBorderColor", "#C4C4C4");
    power.insert("pointHoverBorderColor", "#FFFFFF");
    power.insert("pointHoverBackgroundColor", "#FFA722");
    power.insert("backgroundColor", "#FFA722");
    power.insert("borderColor", "#FFA722");
    QVariantList powerData = {0,0,0,0,0,0,0};
    power.insert("data", powerData);
    datasets.append(power);

    QVariantMap disk;
    disk.insert("label", QCoreApplication::translate("WisenetLinguist", "SystemEvent.RecordingError"));
    disk.insert("fill", false);
    disk.insert("cubicInterpolationMode", "monotone");
    disk.insert("lineTension", 0.0);
    disk.insert("pointBorderColor", "#C4C4C4");
    disk.insert("pointHoverBorderColor", "#FFFFFF");
    disk.insert("pointHoverBackgroundColor", "#7C88EB");
    disk.insert("backgroundColor", "#7C88EB");
    disk.insert("borderColor", "#7C88EB");
    QVariantList diskData = {0,0,0,0,0,0,0};
    disk.insert("data", diskData);
    datasets.append(disk);

    QVariantMap overload;
    overload.insert("label", QCoreApplication::translate("WisenetLinguist", "Overload"));
    overload.insert("fill", false);
    overload.insert("cubicInterpolationMode", "monotone");
    overload.insert("lineTension", 0.0);
    overload.insert("pointBorderColor", "#C4C4C4");
    overload.insert("pointHoverBorderColor", "#FFFFFF");
    overload.insert("pointHoverBackgroundColor", "#D13022");
    overload.insert("backgroundColor", "#D13022");
    overload.insert("borderColor", "#D13022");
    QVariantList overloadData = {1,1,1,1,1,1,1};
    overload.insert("data", overloadData);
    datasets.append(overload);

    QVariantMap connection;
    connection.insert("label", QCoreApplication::translate("WisenetLinguist", "Connection error"));
    connection.insert("fill", false);
    connection.insert("cubicInterpolationMode", "monotone");
    connection.insert("lineTension", 0.0);
    connection.insert("pointBorderColor", "#C4C4C4");
    connection.insert("pointHoverBorderColor", "#FFFFFF");
    connection.insert("pointHoverBackgroundColor", "#4356FF");
    connection.insert("backgroundColor", "#4356FF");
    connection.insert("borderColor", "#4356FF");
    QVariantList connectionData = {0,0,0,0,0,0,0};
    connection.insert("data", connectionData);
    datasets.append(connection);

    QVariantMap fan;
    fan.insert("label", QCoreApplication::translate("WisenetLinguist", "SystemEvent.FanError"));
    fan.insert("fill", false);
    fan.insert("cubicInterpolationMode", "monotone");
    fan.insert("lineTension", 0.0);
    fan.insert("pointBorderColor", "#C4C4C4");
    fan.insert("pointHoverBorderColor", "#FFFFFF");
    fan.insert("pointHoverBackgroundColor", "#CC601A");
    fan.insert("backgroundColor", "#CC601A");
    fan.insert("borderColor", "#CC601A");
    QVariantList fanData = {0,0,0,0,0,0,0};
    fan.insert("data", fanData);
    datasets.append(fan);

    QVariantMap disconnected;
    disconnected.insert("label", QCoreApplication::translate("WisenetLinguist", "Disconnected"));
    disconnected.insert("fill", false);
    disconnected.insert("cubicInterpolationMode", "monotone");
    disconnected.insert("lineTension", 0.0);
    disconnected.insert("pointBorderColor", "#C4C4C4");
    disconnected.insert("pointHoverBorderColor", "#FFFFFF");
    disconnected.insert("pointHoverBackgroundColor", "#2738D0");
    disconnected.insert("backgroundColor", "#2738D0");
    disconnected.insert("borderColor", "#2738D0");
    QVariantList disconnectedData = {57,50,58,60,45,60,58};
    disconnected.insert("data", disconnectedData);
    datasets.append(disconnected);

    vMap.insert("datasets", datasets);

    return vMap;
}

#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "HealthMonitoringViewModel.h"
#include "../../Model/HealthMonitoring/HealthMonitoringDeviceModel.h"
#include "../../Model/HealthMonitoring/HealthMonitoringCameraModel.h"

HealthMonitoringViewModel::HealthMonitoringViewModel(QObject* parent) :
    QObject(parent)
{
    qDebug() << "HealthMonitoringViewModel::HealthMonitoringViewModel()";

    m_deviceListProxy.setSourceModel(&m_deviceListModel);
    m_alertLogListSortFilterProxyModel.setSourceModel(&m_alertLogListModel);

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &HealthMonitoringViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

HealthMonitoringViewModel::~HealthMonitoringViewModel()
{
    qDebug() << "HealthMonitoringViewModel::~HealthMonitoringViewModel()";
}

void HealthMonitoringViewModel::initialize()
{
    m_deviceListModel.clear();
    m_deviceListModel.beginUpdate();

    auto deviceMap = QCoreServiceManager::Instance().DB()->GetDevices();

    foreach(auto& deviceIter, deviceMap)
    {
        Wisenet::Device::Device device = deviceMap[deviceIter.first];
        Wisenet::Device::DeviceStatus deviceStatus;

        QString deviceId = QString::fromStdString(device.deviceID);
        QCoreServiceManager::Instance().DB()->FindDeviceStatus(deviceId, deviceStatus);

        UpdateDevice(device, deviceStatus);
    }

    m_deviceListModel.endUpdate();
    m_deviceListProxy.invalidate();

    foreach(auto& deviceIter, deviceMap)
    {
        Request_DeviceGetRecordingPeriodRequest(deviceIter.first);
        Request_DeviceStorageInfoRequest(deviceIter.first);
    }

    emit cameraNetworkErrorCountChanged();
    emit deviceNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
    emit deviceFirmwareUpdateCountChanged();
}

QString HealthMonitoringViewModel::getProfileString(const Wisenet::Media::VideoCodecType& codecType, const Wisenet::Device::Resolution& resolution, const int framerate)
{
    QString codecStr = "";

    switch(codecType){
    case Wisenet::Media::VideoCodecType::UNKNOWN:
        codecStr = "Unknown";
        break;
    case Wisenet::Media::VideoCodecType::H265:
        codecStr = "H.265";
        break;
    case Wisenet::Media::VideoCodecType::H264:
        codecStr = "H.264";
        break;
    case Wisenet::Media::VideoCodecType::MJPEG:
        codecStr = "MJPEG";
        break;
    case Wisenet::Media::VideoCodecType::MPEG4:
        codecStr = "MPEG4";
        break;
    case Wisenet::Media::VideoCodecType::VP8:
        codecStr = "VP8";
        break;
    case Wisenet::Media::VideoCodecType::VP9:
        codecStr = "VP9";
        break;
    case Wisenet::Media::VideoCodecType::AV1:
        codecStr = "AV1";
        break;
    default:
        break;
    }

    QString ret = QString("%1, %2x%3, %4FPS").arg(codecStr).arg(resolution.width).arg(resolution.height).arg(framerate);

    return ret;
}

void HealthMonitoringViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    {
        Event_FullInfo(event);
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
    default:
        break;
    }
}

int HealthMonitoringViewModel::deviceErrorCount()
{
    int errorCount = 0;

    return errorCount;
}

int HealthMonitoringViewModel::diskErrorCount()
{
    int errorCount = 0;

    return errorCount;
}

int HealthMonitoringViewModel::cameraNetworkErrorCount()
{
    return m_deviceListModel.cameraNetworkErrorCount();
}

int HealthMonitoringViewModel::deviceNetworkErrorCount()
{
    return m_deviceListModel.deviceNetworkErrorCount();
}

int HealthMonitoringViewModel::cameraFirmwareUpdateCount()
{
    return m_deviceListModel.cameraFirmwareUpdateCount();
}

int HealthMonitoringViewModel::deviceFirmwareUpdateCount()
{
    return m_deviceListModel.deviceFirmwareUpdateCount();
}

void HealthMonitoringViewModel::Request_DeviceGetRecordingPeriodRequest(std::string deviceId)
{
    auto request = std::make_shared<Wisenet::Device::DeviceGetRecordingPeriodRequest>();
    request->deviceID = deviceId;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetRecordingPeriod, this, request,
                [this, request, deviceId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetRecordingPeriodResponse>(reply->responseDataPtr);

        if(response->isSuccess())
        {
            QString startTime = QLocaleManager::Instance()->getDateTime24h(response->result.startTime);
            QString endTime =  QLocaleManager::Instance()->getDateTime24h(response->result.endTime);

            QString qDeviceId = QString::fromStdString(deviceId);
            QString period = startTime + " ~ " + endTime;

            m_deviceListModel.updateRecordingPeriod(qDeviceId, period);
        }
    });
}

void HealthMonitoringViewModel::Request_DeviceStorageInfoRequest(std::string deviceId)
{
    auto request = std::make_shared<Wisenet::Device::DeviceStorageInfoRequest>();
    request->deviceID = deviceId;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceStorageInfo, this, request,
                [this, request, deviceId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceStorageInfoResponse>(reply->responseDataPtr);

        if(response->isSuccess())
        {
            qDebug() << "Request_DeviceStorageInfoRequest" << response->usedSpace << response->totalSpace << response->isNormal;

            QString qDeviceId = QString::fromStdString(deviceId);
            int usage = (int)(((float)response->usedSpace / (float)response->totalSpace) * 100) ;

            qDebug() << "Request_DeviceStorageInfoRequest" << usage;

            m_deviceListModel.updateDiskUsagePercent(qDeviceId, usage);
        }
    });
}

void HealthMonitoringViewModel::UpdateDevice(const Wisenet::Device::Device& device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    QString deviceModelName = QString::fromUtf8(device.modelName.c_str());
    QString deviceFirmwareVersion = QString::fromUtf8(device.firmwareVersion.c_str());

    HealthMonitoringDeviceModel* deviceModel = nullptr;

    if(m_deviceListModel.getData()->contains(deviceId))
    {
        deviceModel = m_deviceListModel.getDeviceModel(deviceId);
    }
    else
    {
        deviceModel = new HealthMonitoringDeviceModel();
        deviceModel->setUuid(deviceId);
        m_deviceListModel.append(deviceModel);
    }

    deviceModel->setDeviceName(QString::fromUtf8(device.name.c_str()));
    deviceModel->setDeviceModelName(deviceModelName);

    //deviceModel->setDiskUsagePercent(diskUsage);
    //deviceModel->setDiskStatus(int diskStatus);
    deviceModel->setConnected(deviceStatus.isGood());
    deviceModel->setConnectionType((int)device.connectionInfo.connectedType);

    Wisenet::Core::FirmwareUpdateStatus isLatest = QCoreServiceManager::Instance().checkLatestVersion(deviceModelName, deviceFirmwareVersion);
    deviceModel->setFirmwareVersion((int)isLatest);

    deviceModel->setIpAddress(QString::fromUtf8(device.connectionInfo.ip.c_str()));
    deviceModel->setPortNumber(device.connectionInfo.port);
    deviceModel->setDualRecordingEnabled(device.deviceCapabilities.recordingDualTrack);

    QString timezone = QString("GMT%1%2:%3").arg(device.deviceTimeZone.utcTimeBias > 0 ? "-" : "+")
            .arg(std::abs(device.deviceTimeZone.utcTimeBias) / 60, 2, 10, QLatin1Char('0'))
            .arg(std::abs(device.deviceTimeZone.utcTimeBias) % 60, 2, 10, QLatin1Char('0'));

    deviceModel->setTimeZone(timezone);
    deviceModel->setMacAddress(QString::fromUtf8(device.macAddress.c_str()));

    int normalChannelCount = 0;
    int networkErrorChannelCount = 0;
    int firmwareUpdateChannelCount = 0;

    foreach(auto& channel, device.channels)
    {
        QString channelId = QString::fromStdString(channel.first);
        QString channelModelName = QString::fromUtf8(channel.second.channelModelName.c_str());
        QString channelFirmwareVersion = QString::fromUtf8(channel.second.channelCurVersion.c_str());
        Wisenet::Device::ChannelStatus channelStatus;
        QCoreServiceManager::Instance().DB()->FindChannelStatus(deviceId, channelId, channelStatus);

        HealthMonitoringCameraModel* cameraModel = nullptr;

        if(channel.second.use == false)
        {
            deviceModel->cameraListModel()->removeChannel(channelId.toInt());
            continue;
        }

        if(deviceModel->cameraListModel()->getData()->contains(channelId.toInt()))
        {
            cameraModel = deviceModel->cameraListModel()->getCameraModel(channelId.toInt());
        }
        else
        {
            cameraModel = new HealthMonitoringCameraModel();
            cameraModel->setChannelId(channelId.toInt());
            deviceModel->cameraListModel()->append(cameraModel);
        }

        cameraModel->setModelName(channelModelName);
        cameraModel->setName(QString::fromUtf8(channel.second.name.c_str()));
        cameraModel->setStatus(channelStatus.isGood());

        Wisenet::Core::FirmwareUpdateStatus isLatestVersion = QCoreServiceManager::Instance().checkLatestVersion(channelModelName, channelFirmwareVersion);
        cameraModel->setFirmwareVersion((int)isLatestVersion);

        if(!channelStatus.isGood())
            networkErrorChannelCount++;
        else if(isLatestVersion == Wisenet::Core::FirmwareUpdateStatus::NeedToUpdate)
            firmwareUpdateChannelCount++;
        else
            normalChannelCount++;

        cameraModel->setPrimaryProfile(getProfileString(channel.second.highCodecType, channel.second.highResolution, channel.second.highFramerate));
        cameraModel->setSecondaryProfile(getProfileString(channel.second.lowCodecType, channel.second.lowResolution, channel.second.lowFramerate));
        cameraModel->setRecordingProfile(getProfileString(channel.second.recordingCodecType, channel.second.recordingResolution, channel.second.recordingFramerate));

        cameraModel->setIpAddress(QString::fromStdString(channel.second.ip));
    }

    deviceModel->setNormalChannelCount(normalChannelCount);
    deviceModel->setNetworkErrorChannelCount(networkErrorChannelCount);
    deviceModel->setFirmwareUpdateChannelCount(firmwareUpdateChannelCount);

    deviceModel->cameraListProxyModel()->invalidate();
}

void HealthMonitoringViewModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    //initialize();
}

void HealthMonitoringViewModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    qDebug() << "HealthMonitoringViewModel::Event_SaveChannels()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    QSet<QString> deviceIdSet;

    for(auto& channel : coreEvent->channels)
    {
        QString deviceId = QString::fromStdString(channel.deviceID);

        if(!deviceIdSet.contains(deviceId))
        {
            deviceIdSet.insert(deviceId);
        }
    }

    foreach(QString id, deviceIdSet.values())
    {
        Wisenet::Device::Device device;
        Wisenet::Device::DeviceStatus deviceStatus;

        qDebug() << "HealthMonitoringViewModel::Event_SaveChannels() update: " << id;

        if(!QCoreServiceManager::Instance().DB()->FindDevice(id, device) ||
                !QCoreServiceManager::Instance().DB()->FindDeviceStatus(id, deviceStatus))
        {
            continue;
        }

        UpdateDevice(device, deviceStatus);
        m_deviceListModel.updateSummary(id);
    }

    emit cameraNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
}

void HealthMonitoringViewModel::Event_AddDevice(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);

    m_deviceListModel.beginUpdate();

    UpdateDevice(coreEvent->device, coreEvent->deviceStatus);

    m_deviceListModel.endUpdate();
    m_deviceListProxy.invalidate();

    Request_DeviceGetRecordingPeriodRequest(coreEvent->device.deviceID);
    Request_DeviceStorageInfoRequest(coreEvent->device.deviceID);

    emit cameraNetworkErrorCountChanged();
    emit deviceNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
    emit deviceFirmwareUpdateCountChanged();
}

void HealthMonitoringViewModel::Event_RemoveDevices(QCoreServiceEventPtr event)
{
    auto removeDevices = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event->eventDataPtr);

    for(auto & stdDeviceId : removeDevices->deviceIDs)
    {
        m_deviceListModel.removeDevice(QString::fromStdString(stdDeviceId));
    }

    emit cameraNetworkErrorCountChanged();
    emit deviceNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
    emit deviceFirmwareUpdateCountChanged();
}

void HealthMonitoringViewModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);
    qDebug() << "HealthMonitoringViewModel::Event_DeviceStatus deviceUuid " << deviceUuid;

    Wisenet::Device::Device device;

    if(QCoreServiceManager::Instance().DB()->FindDevice(deviceStatusEvent->deviceID, device))
    {
        UpdateDevice(device, deviceStatusEvent->deviceStatus);
    }

    emit cameraNetworkErrorCountChanged();
    emit deviceNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
    emit deviceFirmwareUpdateCountChanged();
}

void HealthMonitoringViewModel::Event_ChannelStatus(QCoreServiceEventPtr event)
{
    qDebug() << "DashboardViewModel::Event_ChannelStatus()";

    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    QSet<QString> deviceIdSet;
    for (auto& channelStatus : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus status = channelStatus.second;

        QString deviceId = QString::fromStdString(status.deviceID);

        if(!deviceIdSet.contains(deviceId))
        {
            deviceIdSet.insert(deviceId);
        }
    }

    foreach(QString id, deviceIdSet.values())
    {
        Wisenet::Device::Device device;
        Wisenet::Device::DeviceStatus deviceStatus;

        qDebug() << "HealthMonitoringViewModel::Event_ChannelStatus() update: " << id;

        if(!QCoreServiceManager::Instance().DB()->FindDevice(id, device) ||
                !QCoreServiceManager::Instance().DB()->FindDeviceStatus(id, deviceStatus))
        {
            continue;
        }

        UpdateDevice(device, deviceStatus);

        m_deviceListModel.updateSummary(id);
    }

    emit cameraNetworkErrorCountChanged();
    emit cameraFirmwareUpdateCountChanged();
}

void HealthMonitoringViewModel::Event_DeviceLatestStatus(QCoreServiceEventPtr event)
{
    auto deviceEvent = std::static_pointer_cast<Wisenet::Device::DeviceLatestStatusEvent>(event->eventDataPtr);

    Wisenet::Device::Device device;
    std::map<std::string, bool> deviceStatus;

    if(!QCoreServiceManager::Instance().DB()->FindDevice(deviceEvent->deviceID, device))
        return;

    if(!QCoreServiceManager::Instance().DB()->FIndDeviceLatestEventStatus(deviceEvent->deviceID, deviceStatus))
        return;

    // TODO
}

#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "../../Model/HealthMonitoring/HealthMonitoringDeviceListModel.h"
#include "../../Model/HealthMonitoring/HealthMonitoringDeviceListSortFilterProxyModel.h"
#include "../../Model/HealthMonitoring/AlertLogListModel.h"
#include "../../Model/HealthMonitoring/AlertLogListSortFilterProxyModel.h"

class HealthMonitoringViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(HealthMonitoringDeviceListSortFilterProxyModel* deviceListProxy READ deviceListProxy NOTIFY deviceListProxyChanged)

    Q_PROPERTY(int deviceErrorCount READ deviceErrorCount  NOTIFY deviceErrorCountChanged)
    Q_PROPERTY(int diskErrorCount READ diskErrorCount NOTIFY diskErrorCountChanged)
    Q_PROPERTY(int cameraNetworkErrorCount READ cameraNetworkErrorCount NOTIFY cameraNetworkErrorCountChanged)
    Q_PROPERTY(int deviceNetworkErrorCount READ deviceNetworkErrorCount NOTIFY deviceNetworkErrorCountChanged)
    Q_PROPERTY(int cameraFirmwareUpdateCount READ cameraFirmwareUpdateCount NOTIFY cameraFirmwareUpdateCountChanged)
    Q_PROPERTY(int deviceFirmwareUpdateCount READ deviceFirmwareUpdateCount NOTIFY deviceFirmwareUpdateCountChanged)

public:
    static HealthMonitoringViewModel* getInstance()
    {
        static HealthMonitoringViewModel instance;
        return &instance;
    }

    explicit HealthMonitoringViewModel(QObject *parent = nullptr);
    ~HealthMonitoringViewModel();

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

    HealthMonitoringDeviceListSortFilterProxyModel* deviceListProxy(){ return &m_deviceListProxy; }

    int deviceErrorCount();
    int diskErrorCount();
    int cameraNetworkErrorCount();
    int deviceNetworkErrorCount();
    int cameraFirmwareUpdateCount();
    int deviceFirmwareUpdateCount();
private:
    QString getProfileString(const Wisenet::Media::VideoCodecType& codecType, const Wisenet::Device::Resolution& resolution, const int framerate);

    void Request_DeviceGetRecordingPeriodRequest(std::string deviceId);
    void Request_DeviceStorageInfoRequest(std::string deviceId);

    void UpdateDevice(const Wisenet::Device::Device& device, const Wisenet::Device::DeviceStatus& deviceStatus);
    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);
    void Event_AddDevice(QCoreServiceEventPtr event);
    void Event_RemoveDevices(QCoreServiceEventPtr event);
    void Event_DeviceStatus(QCoreServiceEventPtr event);
    void Event_ChannelStatus(QCoreServiceEventPtr event);
    void Event_DeviceLatestStatus(QCoreServiceEventPtr event);

public slots:
    void initialize();

signals:
    void deviceListProxyChanged();
    void deviceErrorCountChanged();
    void diskErrorCountChanged();
    void cameraNetworkErrorCountChanged();
    void deviceNetworkErrorCountChanged();
    void cameraFirmwareUpdateCountChanged();
    void deviceFirmwareUpdateCountChanged();

private:
    AlertLogListModel m_alertLogListModel;
    AlertLogListSortFilterProxyModel m_alertLogListSortFilterProxyModel;

    HealthMonitoringDeviceListModel m_deviceListModel;
    HealthMonitoringDeviceListSortFilterProxyModel m_deviceListProxy;
};

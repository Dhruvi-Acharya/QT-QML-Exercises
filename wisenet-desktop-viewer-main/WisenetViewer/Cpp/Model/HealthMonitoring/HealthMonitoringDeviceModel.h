#pragma once
#include <QObject>
#include "HealthMonitoringCameraListSortFilterProxyModel.h"
#include "HealthMonitoringCameraListModel.h"

class HealthMonitoringDeviceModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString uuid READ uuid WRITE setUuid NOTIFY uuidChanged)
    Q_PROPERTY(QString deviceModelName READ deviceModelName WRITE setDeviceModelName NOTIFY deviceModelNameChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(int diskUsagePercent READ diskUsagePercent WRITE setDiskUsagePercent NOTIFY diskUsagePercentChanged)
    Q_PROPERTY(int diskStatus READ diskStatus WRITE setDiskStatus NOTIFY diskStatusChanged)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(int connectionType READ connectionType WRITE setConnectionType NOTIFY connectionTypeChanged)
    Q_PROPERTY(int firmwareVersion READ firmwareVersion WRITE setFirmwareVersion NOTIFY firmwareVersionChanged)
    Q_PROPERTY(QString currentTime READ currentTime WRITE setCurrentTime NOTIFY currentTimeChanged)
    Q_PROPERTY(QString ipAddress READ ipAddress WRITE setIpAddress NOTIFY ipAddressChanged)
    Q_PROPERTY(int portNumber READ portNumber WRITE setPortNumber NOTIFY portNumberChanged)
    Q_PROPERTY(bool dualRecordingEnabled READ dualRecordingEnabled WRITE setDualRecordingEnabled NOTIFY dualRecordingEnabledChanged)
    Q_PROPERTY(QString timeZone READ timeZone WRITE setTimeZone NOTIFY timeZoneChanged)
    Q_PROPERTY(QString recordingPeriod READ recordingPeriod WRITE setRecordingPeriod NOTIFY recordingPeriodChanged)
    Q_PROPERTY(QString macAddress READ macAddress WRITE setMacAddress NOTIFY macAddressChanged)
    Q_PROPERTY(HealthMonitoringCameraListSortFilterProxyModel* cameraListProxyModel READ cameraListProxyModel NOTIFY cameraListProxyModelChanged)

    Q_PROPERTY(int normalChannelCount READ normalChannelCount WRITE setNormalChannelCount NOTIFY normalChannelCountChanged)
    Q_PROPERTY(int networkErrorChannelCount READ networkErrorChannelCount WRITE setNetworkErrorChannelCount NOTIFY networkErrorChannelCountChanged)
    Q_PROPERTY(int firmwareUpdateChannelCount READ firmwareUpdateChannelCount WRITE setFirmwareUpdateChannelCount NOTIFY firmwareUpdateChannelCountChanged)

public:
    explicit HealthMonitoringDeviceModel(QObject *parent = nullptr);
    virtual ~HealthMonitoringDeviceModel();

    QString uuid(){ return m_uuid; }
    QString deviceModelName(){ return m_deviceModelName; }
    QString deviceName(){ return m_deviceName; }
    int diskUsagePercent(){ return m_diskUsagePercent; }
    int diskStatus(){ return m_diskStatus; }
    bool connected(){ return m_connected; }
    int connectionType(){ return m_connectionType; }
    int firmwareVersion(){ return m_firmwareVersion; }
    QString currentTime(){ return m_currentTime; }
    QString ipAddress(){ return m_ipAddress; }
    int portNumber(){ return m_portNumber; }
    bool dualRecordingEnabled(){ return m_dualRecordingEnabled; }
    QString timeZone(){ return m_timeZone; }
    QString recordingPeriod(){ return m_recordingPeriod; }
    QString macAddress(){ return m_macAddress; }
    HealthMonitoringCameraListModel* cameraListModel(){ return &m_cameraListModel; }
    HealthMonitoringCameraListSortFilterProxyModel* cameraListProxyModel(){ return &m_cameraListProxyModel; }

    int normalChannelCount(){ return m_normalChannelCount; }
    int networkErrorChannelCount(){ return m_networkErrorChannelCount; }
    int firmwareUpdateChannelCount(){ return m_firmwareUpdateChannelCount; }

    void setUuid(QString uuid){ m_uuid = uuid; }
    void setDeviceModelName(QString deviceModelName){ m_deviceModelName = deviceModelName; }
    void setDeviceName(QString deviceName){ m_deviceName = deviceName; }
    void setDiskUsagePercent(int diskUsagePercent){ m_diskUsagePercent = diskUsagePercent; }
    void setDiskStatus(int diskStatus){ m_diskStatus = diskStatus; }
    void setConnected(bool connected){ m_connected = connected; }
    void setConnectionType(int connectionType){ m_connectionType = connectionType; }
    void setFirmwareVersion(int firmwareVersion){ m_firmwareVersion = firmwareVersion; }
    void setCurrentTime(QString currentTime){ m_currentTime = currentTime; }
    void setIpAddress(QString ipAddress){ m_ipAddress = ipAddress; }
    void setPortNumber(int portNumber){ m_portNumber = portNumber; }
    void setDualRecordingEnabled(bool dualRecordingEnabled){ m_dualRecordingEnabled = dualRecordingEnabled; }
    void setTimeZone(QString timeZone){ m_timeZone = timeZone; }
    void setRecordingPeriod(QString recordingPeriod){ m_recordingPeriod = recordingPeriod; }
    void setMacAddress(QString macAddress){ m_macAddress = macAddress; }

    void setNormalChannelCount(int normalChannelCount){ m_normalChannelCount = normalChannelCount; }
    void setNetworkErrorChannelCount(int networkErrorChannelCount){ m_networkErrorChannelCount = networkErrorChannelCount; }
    void setFirmwareUpdateChannelCount(int firmwareUpdateChannelCount){ m_firmwareUpdateChannelCount = firmwareUpdateChannelCount; }

signals:
    void uuidChanged();
    void deviceModelNameChanged();
    void deviceNameChanged();
    void diskUsagePercentChanged();
    void diskStatusChanged();
    void connectedChanged();
    void connectionTypeChanged();
    void firmwareVersionChanged();
    void currentTimeChanged();
    void ipAddressChanged();
    void portNumberChanged();
    void dualRecordingEnabledChanged();
    void timeZoneChanged();
    void recordingPeriodChanged();
    void macAddressChanged();
    void cameraListProxyModelChanged();

    void normalChannelCountChanged();
    void networkErrorChannelCountChanged();
    void firmwareUpdateChannelCountChanged();

private:
    QString m_uuid = "";
    QString m_deviceModelName = "";
    QString m_deviceName = "";
    int m_diskUsagePercent = 0;
    int m_diskStatus = 0;
    bool m_connected = false;
    int m_connectionType = 0;
    int m_firmwareVersion = 0;
    QString m_currentTime = "";
    QString m_ipAddress = "";
    int m_portNumber = 0;
    bool m_dualRecordingEnabled = false;
    QString m_timeZone = "";
    QString m_recordingPeriod = "";
    QString m_macAddress = "";

    HealthMonitoringCameraListModel m_cameraListModel;
    HealthMonitoringCameraListSortFilterProxyModel m_cameraListProxyModel;

    int m_normalChannelCount = 0;
    int m_networkErrorChannelCount = 0;
    int m_firmwareUpdateChannelCount = 0;
};

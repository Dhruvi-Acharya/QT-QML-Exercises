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
#include "HealthMonitoringDeviceListModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"

HealthMonitoringDeviceListModel::HealthMonitoringDeviceListModel(QObject* parent) : QAbstractListModel(parent)
{
}

HealthMonitoringDeviceListModel::~HealthMonitoringDeviceListModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}

void HealthMonitoringDeviceListModel::clear()
{
    beginResetModel();
    qDeleteAll(m_data);
    m_data.clear();
    endResetModel();
}

int HealthMonitoringDeviceListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QHash<int, QByteArray> HealthMonitoringDeviceListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    roles[DeviceModelNameRole] = "deviceModelName";
    roles[DiskUsagePercentRole] = "diskUsagePercent";
    roles[DeviceNameRole] = "deviceName";
    roles[DiskStatusRole] = "diskStatus";
    roles[ConnectionTypeRole] = "connectionType";
    roles[FirmwareVersionRole] = "firmwareVersion";
    roles[CurrentTimeRole] = "currentTime";
    roles[IpAddressRole] = "ipAddress";
    roles[PortNumberRole] = "portNumber";
    roles[DualRecordingEnabledRole] = "dualRecordingEnabled";
    roles[TimezoneRole] = "timezone";
    roles[RecordingPeriodRole] = "recordingPeriod";
    roles[MacAddressRole] = "macAddress";
    roles[CameraListDataRole] = "cameraListData";

    roles[NormalChannelCountRole] = "normalChannelCount";
    roles[NetworkErrorChannelCountRole] = "networkErrorChannelCount";
    roles[FirmwareUpdateChannelCountRole] = "firmwareUpdateChannelCount";

    return roles;
}

QVariant HealthMonitoringDeviceListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case DeviceModelNameRole:
        value = m_data.values()[index.row()]->deviceModelName();
        break;
    case DeviceNameRole:
        value = m_data.values()[index.row()]->deviceName();
        break;
    case DiskUsagePercentRole:
        value = m_data.values()[index.row()]->diskUsagePercent();
        break;
    case DiskStatusRole:
        value = m_data.values()[index.row()]->diskStatus();
        break;
    case ConnectionTypeRole:
        value = m_data.values()[index.row()]->connectionType();
        break;
    case FirmwareVersionRole:
        value = m_data.values()[index.row()]->firmwareVersion();
        break;
    case CurrentTimeRole:
        value = m_data.values()[index.row()]->currentTime();
        break;
    case IpAddressRole:
        value = m_data.values()[index.row()]->ipAddress();
        break;
    case PortNumberRole:
        value = m_data.values()[index.row()]->portNumber();
        break;
    case DualRecordingEnabledRole:
        value = m_data.values()[index.row()]->dualRecordingEnabled();
        break;
    case TimezoneRole:
        value = m_data.values()[index.row()]->timeZone();
        break;
    case RecordingPeriodRole:
        value = m_data.values()[index.row()]->recordingPeriod();
        break;
    case MacAddressRole:
        value = m_data.values()[index.row()]->macAddress();
        break;
    case CameraListDataRole:
    {
        HealthMonitoringCameraListSortFilterProxyModel* model = m_data.values()[index.row()]->cameraListProxyModel();
        value.setValue(model);
        break;
    }
    case NormalChannelCountRole:
        value = m_data.values()[index.row()]->normalChannelCount();
        break;
    case NetworkErrorChannelCountRole:
        value = m_data.values()[index.row()]->networkErrorChannelCount();
        break;
    case FirmwareUpdateChannelCountRole:
        value = m_data.values()[index.row()]->firmwareUpdateChannelCount();
        break;
    default:
        break;
    }

    return value;
}

void HealthMonitoringDeviceListModel::beginUpdate()
{
    beginResetModel();
}

void HealthMonitoringDeviceListModel::endUpdate()
{
    endResetModel();
}

void HealthMonitoringDeviceListModel::append(HealthMonitoringDeviceModel* model)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.insert(model->uuid(), model);
}

void HealthMonitoringDeviceListModel::removeDevice(const QString& uuid)
{
    if(m_data.contains(uuid))
    {
        HealthMonitoringDeviceModel* deviceModel = m_data[uuid];

        int rowNumber = m_data.values().indexOf(deviceModel, 0);
        beginRemoveRows(QModelIndex(), rowNumber, rowNumber);

        m_data.remove(uuid);
        delete deviceModel;
        deviceModel = nullptr;
        endRemoveRows();
    }
}

void HealthMonitoringDeviceListModel::updateDiskUsagePercent(const QString& deviceId, const int usage)
{
    if(m_data.contains(deviceId))
    {
        HealthMonitoringDeviceModel* deviceModel = m_data[deviceId];
        deviceModel->setDiskUsagePercent(usage);

        int row = m_data.values().indexOf(deviceModel);
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {DiskUsagePercentRole});
    }
}

void HealthMonitoringDeviceListModel::updateRecordingPeriod(const QString& deviceId, const QString& period)
{
    if(m_data.contains(deviceId))
    {
        HealthMonitoringDeviceModel* deviceModel = m_data[deviceId];
        deviceModel->setRecordingPeriod(period);

        int row = m_data.values().indexOf(deviceModel);
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {RecordingPeriodRole});
    }
}


void HealthMonitoringDeviceListModel::updateSummary(const QString& deviceId)
{
    if(m_data.contains(deviceId))
    {
        HealthMonitoringDeviceModel* deviceModel = m_data[deviceId];

        int row = m_data.values().indexOf(deviceModel);
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {NormalChannelCountRole, NetworkErrorChannelCountRole, FirmwareUpdateChannelCountRole});
    }
}


int HealthMonitoringDeviceListModel::cameraNetworkErrorCount()
{
    int ret = 0;
    foreach(HealthMonitoringDeviceModel* deviceModel, m_data.values())
    {
        ret += deviceModel->networkErrorChannelCount();
    }

    qDebug() << "HealthMonitoringDeviceListModel::cameraNetworkErrorCount" << ret;
    return ret;
}

int HealthMonitoringDeviceListModel::deviceNetworkErrorCount()
{
    int ret = 0;
    foreach(HealthMonitoringDeviceModel* deviceModel, m_data.values())
    {
        if(!deviceModel->connected())
            ret++;
    }

    qDebug() << "HealthMonitoringDeviceListModel::deviceNetworkErrorCount" << ret;
    return ret;
}

int HealthMonitoringDeviceListModel::cameraFirmwareUpdateCount()
{
    int ret = 0;
    foreach(HealthMonitoringDeviceModel* deviceModel, m_data.values())
    {
        ret += deviceModel->firmwareUpdateChannelCount();
    }

    qDebug() << "HealthMonitoringDeviceListModel::cameraFirmwareUpdateCount" << ret;
    return ret;
}

int HealthMonitoringDeviceListModel::deviceFirmwareUpdateCount()
{
    int ret = 0;
    foreach(HealthMonitoringDeviceModel* deviceModel, m_data.values())
    {
        if(deviceModel->firmwareVersion() == 2)
            ret++;
    }

    qDebug() << "HealthMonitoringDeviceListModel::deviceFirmwareUpdateCount" << ret;
    return ret;
}

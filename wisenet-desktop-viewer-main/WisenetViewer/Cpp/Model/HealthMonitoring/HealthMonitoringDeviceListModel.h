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

#include <QObject>
#include <set>
#include "QCoreServiceManager.h"
#include "HealthMonitoringDeviceModel.h"

class HealthMonitoringDeviceListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        DeviceModelNameRole = Qt::UserRole + 1,
        DeviceNameRole,
        DiskUsagePercentRole,
        DiskStatusRole,
        ConnectionTypeRole,
        FirmwareVersionRole,
        CurrentTimeRole,
        IpAddressRole,
        PortNumberRole,
        DualRecordingEnabledRole,
        TimezoneRole,
        RecordingPeriodRole,
        MacAddressRole,

        CameraListDataRole,

        NormalChannelCountRole,
        NetworkErrorChannelCountRole,
        FirmwareUpdateChannelCountRole,
    };

    explicit HealthMonitoringDeviceListModel(QObject *parent = nullptr);
    virtual ~HealthMonitoringDeviceListModel();

    void clear();
    void beginUpdate();
    void endUpdate();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void append(HealthMonitoringDeviceModel* model);
    void removeDevice(const QString& uuid);
    void updateDiskUsagePercent(const QString& deviceId, const int usage);
    void updateRecordingPeriod(const QString& deviceId, const QString& period);
    void updateSummary(const QString& deviceId);

    QMap<QString, HealthMonitoringDeviceModel*>* getData(){ return &m_data; }
    HealthMonitoringDeviceModel* getDeviceModel(const QString& deviceId){ return m_data[deviceId]; }

    int cameraNetworkErrorCount();
    int deviceNetworkErrorCount();
    int cameraFirmwareUpdateCount();
    int deviceFirmwareUpdateCount();

private:
    QMap<QString, HealthMonitoringDeviceModel*> m_data;

};


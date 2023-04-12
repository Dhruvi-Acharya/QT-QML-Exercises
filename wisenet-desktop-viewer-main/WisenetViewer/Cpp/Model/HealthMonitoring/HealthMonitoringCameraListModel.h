#pragma once
#include <QObject>
#include <QAbstractListModel>
#include "HealthMonitoringCameraModel.h"

class HealthMonitoringCameraListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        ChannelIdRole = Qt::UserRole + 1,
        ModelNameRole,
        NameRole,
        StatusRole,
        FirmwareVersionRole,
        PrimaryProfileRole,
        SecondaryProfileRole,
        RecordingProfileRole,
        IpRole,
    };

    explicit HealthMonitoringCameraListModel(QObject *parent = nullptr);
    virtual ~HealthMonitoringCameraListModel();
    void clear();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void append(HealthMonitoringCameraModel* model);
    void removeChannel(int channelId);
    void beginUpdate(){ beginResetModel(); }
    void endUpdate(){ endResetModel(); }

    QMap<int, HealthMonitoringCameraModel*>* getData(){ return &m_data; }
    HealthMonitoringCameraModel* getCameraModel(int channelId){ return m_data[channelId]; }

private:
    QMap<int, HealthMonitoringCameraModel*> m_data;
};


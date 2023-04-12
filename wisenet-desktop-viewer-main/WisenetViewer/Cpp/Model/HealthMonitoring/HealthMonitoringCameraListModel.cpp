#include "HealthMonitoringCameraListModel.h"

HealthMonitoringCameraListModel::HealthMonitoringCameraListModel(QObject *parent) : QAbstractListModel(parent)
{

}

HealthMonitoringCameraListModel::~HealthMonitoringCameraListModel()
{
    qDeleteAll(m_data);
    m_data.clear();
}

void HealthMonitoringCameraListModel::clear()
{
    beginResetModel();
    qDeleteAll(m_data);
    m_data.clear();
    endResetModel();
}

int HealthMonitoringCameraListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QHash<int, QByteArray> HealthMonitoringCameraListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    roles[ChannelIdRole] = "channelId";
    roles[ModelNameRole] = "cameraModelName";
    roles[NameRole] = "cameraName";
    roles[StatusRole] = "cameraStatus";
    roles[FirmwareVersionRole] = "cameraFirmwareVersion";
    roles[PrimaryProfileRole] = "cameraPrimaryProfile";
    roles[SecondaryProfileRole] = "cameraSecondaryProfile";
    roles[RecordingProfileRole] = "cameraRecordingProfile";
    roles[IpRole] = "cameraIp";

    return roles;
}

QVariant HealthMonitoringCameraListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case ChannelIdRole:
        value = m_data.values()[index.row()]->channelId();
        break;
    case ModelNameRole:
        value = m_data.values()[index.row()]->modelName();
        break;
    case NameRole:
        value = m_data.values()[index.row()]->name();
        break;
    case StatusRole:
        value = m_data.values()[index.row()]->status();
        break;
    case FirmwareVersionRole:
        value = m_data.values()[index.row()]->firmwareVersion();
        break;
    case PrimaryProfileRole:
        value = m_data.values()[index.row()]->primaryProfile();
        break;
    case SecondaryProfileRole:
        value = m_data.values()[index.row()]->secondaryProfile();
        break;
    case RecordingProfileRole:
        value = m_data.values()[index.row()]->recordingProfile();
        break;
    case IpRole:
        value = m_data.values()[index.row()]->ipAddress();
        break;
    default:
        break;
    }

    return value;
}

void HealthMonitoringCameraListModel::append(HealthMonitoringCameraModel* model)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.insert(model->channelId(), model);
    endInsertRows();
}

void HealthMonitoringCameraListModel::removeChannel(int channelId)
{
    if(m_data.contains(channelId))
    {
        HealthMonitoringCameraModel* cameraModel = m_data[channelId];

        int rowNumber = m_data.values().indexOf(cameraModel);

        beginRemoveRows(QModelIndex(), rowNumber, rowNumber);
        m_data.remove(channelId);
        delete cameraModel;
        cameraModel = nullptr;
        endRemoveRows();
    }
}

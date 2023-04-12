#include "DashboardErrorListModel.h"
#include <QDebug>

DashboardErrorListModel::DashboardErrorListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

DashboardErrorListModel::~DashboardErrorListModel()
{
    //qDebug() << "~DashboardErrorListModel()";
    qDeleteAll(m_data);
    m_data.clear();
}

int DashboardErrorListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QHash<int, QByteArray> DashboardErrorListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[ObjectIdRole] = "objectId";
    roles[ObjectTypeRole] = "objectType";
    roles[ObjectNameRole] = "objectName";
    return roles;
}

QVariant DashboardErrorListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case ObjectIdRole:
        value = m_data.values()[index.row()]->objectId;
        break;
    case ObjectTypeRole:
        value = m_data.values()[index.row()]->objectType;
        break;
    case ObjectNameRole:
        value = m_data.values()[index.row()]->objectName;
        break;
    default:
        break;
    }

    return value;
}

bool DashboardErrorListModel::contains(const QString& objectId)
{
    return this->m_data.contains(objectId);
}

void DashboardErrorListModel::append(ErrorItem* model)
{
    if(this->m_data.contains(model->objectId))
        return;

    beginResetModel();
    this->m_data.insert(model->objectId, model);
    endResetModel();
}

void DashboardErrorListModel::remove(const QString& objectId)
{
    if(!this->m_data.contains(objectId))
        return;

    beginResetModel();
    ErrorItem* item = this->m_data.take(objectId);
    delete item;
    endResetModel();
}

void DashboardErrorListModel::removeWithDeviceId(const QString& deviceId)
{
    beginResetModel();

    foreach(const auto& key, this->m_data.keys())
    {
        QStringList keyList = key.split('_');
        if(keyList.size() == 2 && keyList[0] == deviceId)
        {
            ErrorItem* item = this->m_data.take(key);
            delete item;
        }
    }

    endResetModel();
}


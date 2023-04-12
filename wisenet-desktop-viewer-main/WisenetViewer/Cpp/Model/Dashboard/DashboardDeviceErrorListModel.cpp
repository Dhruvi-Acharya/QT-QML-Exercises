#include "DashboardDeviceErrorListModel.h"
#include <QDebug>

DashboardDeviceErrorListModel::DashboardDeviceErrorListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

DashboardDeviceErrorListModel::~DashboardDeviceErrorListModel()
{
    //qDebug() << "~DashboardDeviceErrorListModel()";
    qDeleteAll(m_data);
    m_data.clear();
}

int DashboardDeviceErrorListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QHash<int, QByteArray> DashboardDeviceErrorListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[TitleRole] = "subTitle";
    roles[ListDataRole] = "listData";
    return roles;
}

QVariant DashboardDeviceErrorListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case TitleRole:
    {
        QString title = m_data.keys()[index.row()];
        value = title;
        //qDebug() << "DashboardDeviceErrorListModel::data()" << title;
        break;
    }
    case ListDataRole:
    {
        DashboardErrorListSortProxyModel* model = m_data.values()[index.row()];
        value.setValue(model);
        break;
    }
    default:
        break;
    }

    return value;
}

bool DashboardDeviceErrorListModel::contains(const QString& title)
{
    return this->m_data.contains(title);
}

void DashboardDeviceErrorListModel::appendEventType(const QString& title)
{
    DashboardErrorListSortProxyModel* proxyModel = new DashboardErrorListSortProxyModel();
    DashboardErrorListModel* listModel = new DashboardErrorListModel();
    proxyModel->setSourceModel(listModel);
    beginResetModel();
    this->m_data.insert(title, proxyModel);
    endResetModel();
}

DashboardErrorListModel* DashboardDeviceErrorListModel::getSource(const QString& title)
{
    DashboardErrorListModel* item = nullptr;
    if(this->m_data.find(title) != this->m_data.end())
        item = (DashboardErrorListModel*)m_data[title]->sourceModel();

    return item;
}

DashboardErrorListSortProxyModel* DashboardDeviceErrorListModel::get(const QString& title)
{
    DashboardErrorListSortProxyModel* item = nullptr;
    if(this->m_data.find(title) != this->m_data.end())
        item = (DashboardErrorListSortProxyModel*)m_data[title];

    return item;
}

void DashboardDeviceErrorListModel::remove(const QString& title)
{
    if(!this->m_data.contains(title))
        return;

    beginResetModel();
    DashboardErrorListSortProxyModel* item = this->m_data.take(title);

    delete item;
    item = nullptr;

    endResetModel();
}

void DashboardDeviceErrorListModel::appendDevice(const QString& eventKey, ErrorItem* item)
{
    if(m_data.contains(eventKey))
    {
        ((DashboardErrorListModel*)m_data[eventKey]->sourceModel())->append(item);
        m_data[eventKey]->invalidate();
    }
}

void DashboardDeviceErrorListModel::removeDevice(const QString& deviceId)
{
    foreach(auto& model, m_data)
    {
        ((DashboardErrorListModel*)model->sourceModel())->remove(deviceId);
        model->invalidate();
    }
}

int DashboardDeviceErrorListModel::deviceCount()
{
    //QSet<QString> deviceSet;
    int count = 0;

    foreach(auto& model, m_data)
    {
        auto modelData = ((DashboardErrorListModel*)model->sourceModel())->getData();

        count += modelData.count();
        /*
        foreach (auto& item, modelData) {
            if(!deviceSet.contains(item->objectId))
                deviceSet.insert(item->objectId);
        }*/
    }

    //return deviceSet.count();
    return count;
}

#include "DashboardCameraSortFilterProxyModel.h"
#include "DashboardErrorListModel.h"
#include <QUuid>
#include "QCoreServiceManager.h"

DashboardCameraSortFilterProxyModel::DashboardCameraSortFilterProxyModel(QObject *parent) : DashboardErrorListSortProxyModel(parent)
{
    //qDebug() << "DashboardCameraSortFilterProxyModel()";
    sort(0, Qt::SortOrder::AscendingOrder);
}

DashboardCameraSortFilterProxyModel::~DashboardCameraSortFilterProxyModel()
{
    //qDebug() << "~DashboardCameraSortFilterProxyModel()";
}

bool DashboardCameraSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //qDebug() << "DashboardCameraSortFilterProxyModel::filterAcceptsRow" << m_useGroupFilter << m_useDeviceFilter;
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    auto idData = sourceModel()->data(sourceIndex, DashboardErrorListModel::ModelRoles::ObjectIdRole);
    QString objectId = idData.value<QString>();
    QUuid emptyUuid;

    if(m_useGroupFilter)
    {
        if(emptyUuid.toString(QUuid::WithoutBraces) == m_groupId)
        {
            return true;
        }
        else
        {
            std::string channelId = objectId.toStdString();

            auto channelGroupMappings = QCoreServiceManager::Instance().DB()->GetChannelGroupMappings();

            Wisenet::uuid_string stdGroupId = channelGroupMappings[channelId];
            QString id = QString::fromStdString(stdGroupId);

            return isParentGroup(id);
        }
    }
    else if(m_useDeviceFilter)
    {
        if(emptyUuid.toString(QUuid::WithoutBraces) == m_deviceId)
        {
            return true;
        }
        else
        {
            QStringList list = objectId.split("_");

            if(list.size() == 2 && list[0] == m_deviceId)
            {
                return true;
            }
            else
            {
                return false;
            }

        }
    }

    return true;
}

bool DashboardCameraSortFilterProxyModel::isParentGroup(const QString& id) const
{
    QUuid emptyUuid;
    if(id == emptyUuid.toString(QUuid::WithoutBraces))
        return false;

    if(id == m_groupId)
        return true;

    Wisenet::Core::Group group;

    if(!QCoreServiceManager::Instance().DB()->FindGroup(id, group))
        return false;

    return isParentGroup(QString::fromStdString(group.parentGroupID));
}

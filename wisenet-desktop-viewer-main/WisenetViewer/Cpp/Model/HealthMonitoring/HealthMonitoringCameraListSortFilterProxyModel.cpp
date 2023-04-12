#include "HealthMonitoringCameraListSortFilterProxyModel.h"
#include "HealthMonitoringCameraListModel.h"

HealthMonitoringCameraListSortFilterProxyModel::HealthMonitoringCameraListSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{

}

bool HealthMonitoringCameraListSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, (int)HealthMonitoringCameraListModel::ModelRoles::ChannelIdRole);
    QVariant rightData = sourceModel()->data(right, (int)HealthMonitoringCameraListModel::ModelRoles::ChannelIdRole);

    int leftId = leftData.toInt();
    int rightId = rightData.toInt();

    return leftId < rightId;
}

#include "HealthMonitoringDeviceListSortFilterProxyModel.h"
#include "HealthMonitoringDeviceListModel.h"

HealthMonitoringDeviceListSortFilterProxyModel::HealthMonitoringDeviceListSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{

}

bool HealthMonitoringDeviceListSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, (int)HealthMonitoringDeviceListModel::ModelRoles::DeviceNameRole);
    QVariant rightData = sourceModel()->data(right, (int)HealthMonitoringDeviceListModel::ModelRoles::DeviceNameRole);

    QString leftName = leftData.toString();
    QString rightName = rightData.toString();

    int compare = QString::localeAwareCompare(leftName, rightName);
    bool ret = (compare < 0);

    return ret;
}

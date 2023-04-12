#pragma once
#include <QSortFilterProxyModel>

class HealthMonitoringDeviceListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit HealthMonitoringDeviceListSortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};

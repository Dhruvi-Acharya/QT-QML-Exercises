#pragma once
#include <QSortFilterProxyModel>

class HealthMonitoringCameraListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit HealthMonitoringCameraListSortFilterProxyModel(QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
};
Q_DECLARE_METATYPE(HealthMonitoringCameraListSortFilterProxyModel*)

#include "AlertLogListSortFilterProxyModel.h"
#include "AlertLogListModel.h"

AlertLogListSortFilterProxyModel::AlertLogListSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
}

AlertLogListSortFilterProxyModel::~AlertLogListSortFilterProxyModel()
{
}

bool AlertLogListSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    return true;
}

bool AlertLogListSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, (int)AlertLogListModel::ModelRoles::OccurrenceTimeRole);
    QVariant rightData = sourceModel()->data(right, (int)AlertLogListModel::ModelRoles::OccurrenceTimeRole);

    long long leftTime = leftData.toLongLong();
    long long rightTime = rightData.toLongLong();

    bool ret = leftTime < rightTime;

    return ret;
}

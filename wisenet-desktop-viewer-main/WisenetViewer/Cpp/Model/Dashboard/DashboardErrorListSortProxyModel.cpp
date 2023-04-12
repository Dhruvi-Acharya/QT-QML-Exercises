#include "DashboardErrorListSortProxyModel.h"
#include "DashboardErrorListModel.h"
#include <QDebug>

DashboardErrorListSortProxyModel::DashboardErrorListSortProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    //qDebug() << "DashboardErrorListSortProxyModel()";
    sort(0, Qt::SortOrder::AscendingOrder);
}

DashboardErrorListSortProxyModel::~DashboardErrorListSortProxyModel()
{
    //qDebug() << "~DashboardErrorListSortProxyModel()";
    DashboardErrorListModel* model = static_cast<DashboardErrorListModel*>(this->sourceModel());

    if(!model)
    {
        delete model;
        model = nullptr;
    }
}

bool DashboardErrorListSortProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, (int)DashboardErrorListModel::ModelRoles::ObjectNameRole);
    QVariant rightData = sourceModel()->data(right, (int)DashboardErrorListModel::ModelRoles::ObjectNameRole);

    QString leftName = leftData.toString();
    QString rightName = rightData.toString();

    int compare = QString::localeAwareCompare(leftName, rightName);
    bool ret = (compare < 0);

    return ret;
}

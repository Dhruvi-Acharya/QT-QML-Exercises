#pragma once
#include <QSortFilterProxyModel>
#include <QSet>

class DashboardErrorListSortProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit DashboardErrorListSortProxyModel(QObject *parent = nullptr);
    virtual ~DashboardErrorListSortProxyModel();

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

};
Q_DECLARE_METATYPE(DashboardErrorListSortProxyModel*)

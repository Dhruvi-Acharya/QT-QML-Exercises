#pragma once
#include <QObject>
#include <QSortFilterProxyModel>

class AlertLogListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit AlertLogListSortFilterProxyModel(QObject *parent = nullptr);
    virtual ~AlertLogListSortFilterProxyModel();

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

signals:

};

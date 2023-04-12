#pragma once

#include <QSortFilterProxyModel>
#include "BaseTreeItemInfo.h"

class TreeProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(QString filterText READ filterText WRITE setFilterText NOTIFY filterTextChanged)

public:
    explicit TreeProxyModel(QObject *parent = nullptr);
    ~TreeProxyModel();

    QString filterText()
    {
        return m_filterText;
    }

signals:
    void filterTextChanged();
    void filterParentExpand(const QModelIndex& index)const;
    void filterParentExpand2(const int index)const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

public slots:
    void setFilterText(const QString &value);
    void doSort(Qt::SortOrder order);

private:
    QString m_filterText;
    QString m_tempText;
};

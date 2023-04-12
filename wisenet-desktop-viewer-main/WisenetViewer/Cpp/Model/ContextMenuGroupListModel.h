#pragma once
#include <QAbstractListModel>
#include <QList>
#include <QDebug>
#include <QSortFilterProxyModel>
#include "QCoreServiceManager.h"
#include "LogSettings.h"

struct GroupItem
{
    QString groupName = "";
    QString groupId = "";
};

class ContextMenuGroupListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count CONSTANT)

public:
    enum RoleNames
    {
        GroupIdRole = Qt::UserRole,
        GroupNameRole
    };

    explicit ContextMenuGroupListModel(QObject *parent = nullptr);
    ~ContextMenuGroupListModel();

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void initialize();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int idx, GroupItem* item);
    void append(GroupItem* item);
    void remove(int idx);
    void removeAll();
    QList<GroupItem*> getData();

    int count() const { return m_data.count(); }

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_SaveGroup(QCoreServiceEventPtr event);
    void Event_RemoveGroup(QCoreServiceEventPtr event);

    QHash<int, QByteArray> m_roleNames;
    QList<GroupItem*> m_data;

};

class ContextMenuGroupListSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit ContextMenuGroupListSortModel(QObject *parent = nullptr){ Q_UNUSED(parent); }

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const
    {
        QString leftData = sourceModel()->data(left, ContextMenuGroupListModel::RoleNames::GroupNameRole).toString();
        QString rightData = sourceModel()->data(right, ContextMenuGroupListModel::RoleNames::GroupNameRole).toString();

        qDebug() << "ContextMenuGroupListSortModel sort : leftData" << leftData << "rightData" << rightData;

        int compare = QString::localeAwareCompare(leftData, rightData);
        bool ret = (compare < 0);

        return ret;
    }

public slots:
    void doSort(Qt::SortOrder order)
    {
        qDebug() << "doSort " << order;
        sort(0, order);
    }
};


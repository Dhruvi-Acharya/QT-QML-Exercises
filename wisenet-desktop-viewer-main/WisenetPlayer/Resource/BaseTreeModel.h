#pragma once
#include <QModelIndex>
#include <QVariant>
#include <QAbstractItemModel>
#include "TreeItemModel.h"
#include "WisenetViewerDefine.h"
#include "LogSettings.h"

class BaseTreeModel : public QAbstractItemModel
{
public:
    enum TreeModelRoles
    {
        TreeModelRoleName = Qt::UserRole + 1,
    };

    explicit BaseTreeModel(QObject *parent = nullptr);
    virtual ~BaseTreeModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation,
                        int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QHash<int, QByteArray> roleNames() const override;

    QModelIndex getIndex(TreeItemModel* treeItem);
    QModelIndex getParentIndex(TreeItemModel* treeItem);

protected:
    TreeItemModel *rootItem;
    QHash<int, QByteArray> m_roleNameMapping;
};

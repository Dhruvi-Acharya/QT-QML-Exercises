#include "BaseTreeModel.h"

BaseTreeModel::BaseTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    qDebug() << "BaseTreeModel()";
    rootItem = nullptr;
}

BaseTreeModel::~BaseTreeModel()
{
    qDebug() << "~BaseTreeModel()";
    if(rootItem)
        delete rootItem;
}

int BaseTreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItemModel*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant BaseTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != TreeModelRoleName)
        return QVariant();

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());

    return item->data(role - Qt::UserRole - 1);
}

Qt::ItemFlags BaseTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QVariant BaseTreeModel::headerData(int section, Qt::Orientation orientation,
                                   int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex BaseTreeModel::index(int row, int column, const QModelIndex &parent)
const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItemModel *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItemModel*>(parent.internalPointer());

    TreeItemModel *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex BaseTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItemModel *childItem = static_cast<TreeItemModel*>(index.internalPointer());
    TreeItemModel *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

QModelIndex BaseTreeModel::getIndex(TreeItemModel* treeItem)
{
    if(treeItem == rootItem){
        return QModelIndex();
    }

    TreeItemModel *parentItem = treeItem->parentItem();

    int row = parentItem->indexOfChild(treeItem);

    return createIndex(row, 0, treeItem);
}

QModelIndex BaseTreeModel::getParentIndex(TreeItemModel* treeItem)
{
    TreeItemModel *parentItem = treeItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int BaseTreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItemModel *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItemModel*>(parent.internalPointer());

    return parentItem->childCount();
}

QHash<int, QByteArray> BaseTreeModel::roleNames() const
{
    return m_roleNameMapping;
}

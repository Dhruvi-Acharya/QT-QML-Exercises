#include "SetupMainTreeViewModel.h"

SetupMainTreeViewModel::SetupMainTreeViewModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    rootItem = new SetupTreeNode(nullptr);
}

SetupMainTreeViewModel::~SetupMainTreeViewModel()
{
    if(rootItem)
        delete rootItem;
}

QVariant SetupMainTreeViewModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (!(role == Qt::DisplayRole || role == Qt::DecorationRole))
        return QVariant();

    SetupTreeNode *item = static_cast<SetupTreeNode*>(index.internalPointer());

    return item->getTreenode();
}

Qt::ItemFlags SetupMainTreeViewModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex SetupMainTreeViewModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    SetupTreeNode *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SetupTreeNode*>(parent.internalPointer());

    SetupTreeNode *childItem = parentItem->child(row);

    if (childItem) return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex SetupMainTreeViewModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    SetupTreeNode *childItem = static_cast<SetupTreeNode*>(index.internalPointer());
    SetupTreeNode *parentItem = childItem->ParentNode();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int SetupMainTreeViewModel::rowCount(const QModelIndex &parent) const
{
    SetupTreeNode *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<SetupTreeNode*>(parent.internalPointer());

    return parentItem->count();
}

int SetupMainTreeViewModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QHash<int, QByteArray> SetupMainTreeViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "displayRole"}, {Qt::DecorationRole, "iconRole"}};
}

#include "devicetreemodel.h"

#include <QFile>

#include "devicetreeitemtype.h"
#include "devicetreeitem.h"

DeviceTreeModel::DeviceTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    QVector<QVariant> rootData;
    rootData << "deviceTree";

    rootItem = new DeviceTreeItem(rootData);

    DefaultSetup();
}

DeviceTreeModel::~DeviceTreeModel()
{
    delete rootItem;
}

QVariant DeviceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex DeviceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if(!hasIndex(row,column,parent))
        return QModelIndex();

    DeviceTreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DeviceTreeItem*>(parent.internalPointer());

    DeviceTreeItem *childItem = parentItem->child(row);

    if (childItem)
        return createIndex(row, column, childItem);

    return QModelIndex();
}

QModelIndex DeviceTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    DeviceTreeItem *childItem = static_cast<DeviceTreeItem*>(index.internalPointer());
    DeviceTreeItem *parentItem = childItem->parentItem();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int DeviceTreeModel::rowCount(const QModelIndex &parent) const
{
    DeviceTreeItem *parentItem;

    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<DeviceTreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}

int DeviceTreeModel::columnCount(const QModelIndex &parent) const
{
    if(parent.isValid())
        return static_cast<DeviceTreeItem*>(parent.internalPointer())->columnCount();

    return rootItem->columnCount();
}

QVariant DeviceTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != DeviceTreeRole)
        return QVariant();

    DeviceTreeItem *item = static_cast<DeviceTreeItem*>(index.internalPointer());

    return item->data(role - Qt::UserRole - 1);
}

QHash<int,QByteArray> DeviceTreeModel::roleNames() const
{
     QHash<int, QByteArray> result;
     result.insert(DeviceTreeRole, QByteArrayLiteral("deviceTree"));
     return result;
}

Qt::ItemFlags DeviceTreeModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
}

QModelIndex DeviceTreeModel::rootIndex()
{
    return createIndex(0, 0, rootItem);
}

QVariant DeviceTreeModel::newItemType(const QString &name, const QString &ip, int type)
{
    DeviceTreeItemType *item = new DeviceTreeItemType(this);
    item->setName(name);
    item->setIp(ip);
    item->setType(type);

    QVariant v;
    v.setValue(item);

    return v;
}

QVector<QVariant> DeviceTreeModel::makeColumnData(QString baseName, QString baseIp, int index)
{
    auto number = QString::number(index);
    auto name = baseName + number;
    auto ip = baseIp + number;

    QVector<QVariant> columnData;
    columnData.reserve(2);
    columnData << newItemType(name, ip, 0);
    return columnData;
}


void DeviceTreeModel::DefaultSetup()
{
    const int serverCount = 5;
    const int nvrCount = 10;
    const int channelCount = 64;
    const int camCount = 128;

    QVector<DeviceTreeItem*> parents;
    parents << rootItem;
    for (int s = 0 ; s < serverCount ; s++) {
        auto columnData = makeColumnData("SSM Server-", "192.168.0.", s);
        parents.last()->appendChild(new DeviceTreeItem(columnData, parents.last()));

        parents << parents.last()->child(parents.last()->childCount()-1);
        for (int n = 0 ; n < nvrCount ; n++) {
            auto columnData = makeColumnData("PRP-8000-", "192.168.10.", n);
            parents.last()->appendChild(new DeviceTreeItem(columnData, parents.last()));

            parents << parents.last()->child(parents.last()->childCount()-1);
            for (int ch = 0 ; ch < channelCount ; ch++) {
                auto columnData = makeColumnData("NVR Channel-", "192.168.100.", ch);
                parents.last()->appendChild(new DeviceTreeItem(columnData, parents.last()));
            }
            parents.pop_back();
        }

        for (int c = 0 ; c < camCount ; c++) {
            auto columnData = makeColumnData("XNB-9000-", "192.168.200.", c);
            parents.last()->appendChild(new DeviceTreeItem(columnData, parents.last()));
        }
        parents.pop_back();
    }
}

DeviceTreeItem *DeviceTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        DeviceTreeItem *item = static_cast<DeviceTreeItem*>(index.internalPointer());
        if (item)
            return item;
    }
    return rootItem;
}

/*
DeviceTreeModel::DeviceTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

QVariant DeviceTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    // FIXME: Implement me!
}

bool DeviceTreeModel::setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role)
{
    if (value != headerData(section, orientation, role)) {
        // FIXME: Implement me!
        emit headerDataChanged(orientation, section, section);
        return true;
    }
    return false;
}

QModelIndex DeviceTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

QModelIndex DeviceTreeModel::parent(const QModelIndex &index) const
{
    // FIXME: Implement me!
}

int DeviceTreeModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

int DeviceTreeModel::columnCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
        return 0;

    // FIXME: Implement me!
}

bool DeviceTreeModel::hasChildren(const QModelIndex &parent) const
{
    // FIXME: Implement me!
}

bool DeviceTreeModel::canFetchMore(const QModelIndex &parent) const
{
    // FIXME: Implement me!
    return false;
}

void DeviceTreeModel::fetchMore(const QModelIndex &parent)
{
    // FIXME: Implement me!
}

QVariant DeviceTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    // FIXME: Implement me!
    return QVariant();
}

bool DeviceTreeModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (data(index, role) != value) {
        // FIXME: Implement me!
        emit dataChanged(index, index, QVector<int>() << role);
        return true;
    }
    return false;
}

Qt::ItemFlags DeviceTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return Qt::ItemIsEditable; // FIXME: Implement me!
}

bool DeviceTreeModel::insertRows(int row, int count, const QModelIndex &parent)
{
    beginInsertRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endInsertRows();
}

bool DeviceTreeModel::insertColumns(int column, int count, const QModelIndex &parent)
{
    beginInsertColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endInsertColumns();
}

bool DeviceTreeModel::removeRows(int row, int count, const QModelIndex &parent)
{
    beginRemoveRows(parent, row, row + count - 1);
    // FIXME: Implement me!
    endRemoveRows();
}

bool DeviceTreeModel::removeColumns(int column, int count, const QModelIndex &parent)
{
    beginRemoveColumns(parent, column, column + count - 1);
    // FIXME: Implement me!
    endRemoveColumns();
}
*/

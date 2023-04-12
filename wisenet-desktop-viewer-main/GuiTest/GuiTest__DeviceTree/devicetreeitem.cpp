#include "devicetreeitem.h"

DeviceTreeItem::DeviceTreeItem(const QVector<QVariant> &data, DeviceTreeItem *parent)
    : m_itemData(data), m_parentItem(parent)
{}

DeviceTreeItem::~DeviceTreeItem()
{
    qDeleteAll(m_childItems);
}

void DeviceTreeItem::appendChild(DeviceTreeItem *item)
{
    m_childItems.append(item);
}

DeviceTreeItem *DeviceTreeItem::child(int row)
{
    if (row < 0 || row >= m_childItems.size())
        return nullptr;
    return m_childItems.at(row);
}

int DeviceTreeItem::childCount() const
{
    return m_childItems.count();
}

int DeviceTreeItem::columnCount() const
{
    return m_itemData.count();
}

QVariant DeviceTreeItem::data(int column) const
{
    if (column < 0 || column >= m_itemData.size())
        return QVariant();
    return m_itemData.at(column);
}

DeviceTreeItem *DeviceTreeItem::parentItem()
{
    return m_parentItem;
}

int DeviceTreeItem::row() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<DeviceTreeItem*>(this));

    return 0;
}

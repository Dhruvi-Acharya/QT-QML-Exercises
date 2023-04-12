#ifndef DEVICETREEITEM_H
#define DEVICETREEITEM_H

#include <QVariant>
#include <QVector>

class DeviceTreeItem
{
public:
    explicit DeviceTreeItem(const QVector<QVariant> &data, DeviceTreeItem *parentItem = nullptr);
    ~DeviceTreeItem();

    void appendChild(DeviceTreeItem *child);

    DeviceTreeItem *child(int row);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    DeviceTreeItem *parentItem();

private:
    QVector<DeviceTreeItem*> m_childItems;
    QVector<QVariant> m_itemData;
    DeviceTreeItem *m_parentItem;
};

#endif // DEVICETREEITEM_H

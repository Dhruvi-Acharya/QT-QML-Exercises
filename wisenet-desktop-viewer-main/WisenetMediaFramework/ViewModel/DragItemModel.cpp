#include "DragItemModel.h"

DragItemModel::DragItemModel(QObject *parent) : QObject(parent)
{

}

QString DragItemModel::uuid() const
{
    return m_uuid;
}

void DragItemModel::setUuid(const QString &uuid)
{
    m_uuid = uuid;
}

QString DragItemModel::channelId() const
{
    return m_channelId;
}

void DragItemModel::setChannelId(const QString &channelId)
{
    m_channelId = channelId;
}

int DragItemModel::itemType() const
{
    return m_itemType;
}

void DragItemModel::setItemType(int itemType)
{
    m_itemType = itemType;
}

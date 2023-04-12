#include "BaseTreeItemInfo.h"

BaseTreeItemInfo::BaseTreeItemInfo(QObject *parent) : QObject(parent)
{
    m_itemType = ItemType::MainRoot;
    m_itemStatus = ItemStatus::Normal;
}

BaseTreeItemInfo::BaseTreeItemInfo(const BaseTreeItemInfo &other)
{
    m_uuid = other.uuid();
    m_itemType = (ItemType)other.itemType();
    m_itemStatus = (ItemStatus)other.itemStatus();
    m_displayName = other.displayName();
}

BaseTreeItemInfo::~BaseTreeItemInfo()
{
    qDebug() << "TreeItemInfo::~BaseTreeItemInfo()";
}

void BaseTreeItemInfo::setType(const ItemType itemType)
{
    m_itemType = itemType;
}

void BaseTreeItemInfo::setUuid(const QString uuid)
{
    m_uuid = uuid;
}

void BaseTreeItemInfo::setItemStatus(const int status)
{
    m_itemStatus = (ItemStatus)status;
    emit itemStatusChanged(status);
}

void BaseTreeItemInfo::setDisplayName(QString displayName)
{
    m_displayName = displayName;
    emit displayNameChanged(displayName);
}

void BaseTreeItemInfo::setItemId(QString itemId)
{
    m_itemId = itemId;
}

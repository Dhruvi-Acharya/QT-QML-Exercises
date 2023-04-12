/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#include "TreeItemModel.h"

TreeItemModel::TreeItemModel(BaseTreeItemInfo* treeItemInfo, TreeItemModel *parent)
{
    m_parentItem = parent;
    m_treeItemInfo = treeItemInfo;
    m_treeValue.setValue(m_treeItemInfo);
}

TreeItemModel::TreeItemModel(const QVariant &data, TreeItemModel *parent)
{
    m_treeItemInfo = nullptr;
    m_parentItem = parent;
    m_treeValue = data;
}

TreeItemModel::~TreeItemModel()
{
    //qDebug() << "TreeItemModel::~TreeItemModel()";

    m_treeItemInfo->deleteLater();
    qDeleteAll(m_childItems);
}

QList<TreeItemModel*> TreeItemModel::children()
{
    return m_childItems;
}

TreeItemModel* TreeItemModel::child(int row)
{
    return m_childItems.at(row);
}

int TreeItemModel::childCount() const
{
    return m_childItems.count();
}

int TreeItemModel::columnCount() const
{
    return 1;
}

bool TreeItemModel::appendChild(TreeItemModel* child)
{
    m_childItems.append(child);
    return true;
}

bool TreeItemModel::insertChild(int position, TreeItemModel* child)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    m_childItems.insert(position, child);

    return true;
}

bool TreeItemModel::unlinkChild(int position)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    m_childItems.removeAt(position);

    return true;
}

bool TreeItemModel::removeChild(int position)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    delete m_childItems.takeAt(position);

    return true;
}

void TreeItemModel::removeAllChild()
{
    qDeleteAll(m_childItems);
    m_childItems.clear();
}

QVariant TreeItemModel::data(int column) const
{
    Q_UNUSED(column)
    return m_treeValue;
}

int TreeItemModel::row() const
{
    if (m_parentItem)
    {
        int retIndex = m_parentItem->m_childItems.indexOf(const_cast<TreeItemModel*>(this));

        return retIndex;
    }

    return 0;
}

TreeItemModel* TreeItemModel::parentItem()
{
    return m_parentItem;
}

void TreeItemModel::setParentItem(TreeItemModel* parent)
{
    m_parentItem = parent;
}

int TreeItemModel::indexOfChild(TreeItemModel * child)
{
    return m_childItems.indexOf(const_cast<TreeItemModel*>(child));
}

void TreeItemModel::getChildrenUuid(QSet<QString>& childrenSet)
{
    for(int i=0; i<m_childItems.size(); i++)
    {
        childrenSet.insert(m_childItems[i]->getUuid());

        m_childItems[i]->getChildrenUuid(childrenSet);
    }
}

QString TreeItemModel::getName() const
{
    QString name = m_treeItemInfo->displayName();

    return name;
}

void TreeItemModel::setName(QString name)
{
    m_treeItemInfo->setDisplayName(name);
}



QString TreeItemModel::getUuid() const
{
    //TreeItemInfo treeItemInfo = qvariant_cast<TreeItemInfo>(data(0));
    //TreeItemInfo ct = qvariant_cast<TreeItemInfo>(myVariant);

    QString uuid = m_treeItemInfo->uuid();

    //qDebug() << "Getuuid() name [" << treeItemInfo->userDefineName() << "] uuid [" << uuid << "]";

    return uuid;
}

ItemType TreeItemModel::getItemType()
{
    ItemType itemType = (ItemType)m_treeItemInfo->itemType();

    return itemType;
}


BaseTreeItemInfo* TreeItemModel::getTreeItemInfo() const
{
    return m_treeItemInfo;
}

void TreeItemModel::addCamera(const QString &deviceId, const QString &channelId)
{
    LayoutTreeItemInfo* layoutItem = dynamic_cast<LayoutTreeItemInfo*>(m_treeItemInfo);
    if(layoutItem)
        layoutItem->addCamera(deviceId, channelId);
}

void TreeItemModel::setStatus(int status)
{
    m_treeItemInfo->setItemStatus(status);
}

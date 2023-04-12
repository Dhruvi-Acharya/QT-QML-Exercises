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
#include "UserFilterTreeSourceModel.h"
#include <QUuid>
#include <QDebug>

#include "QCoreServiceManager.h"

UserFilterTreeSourceModel::UserFilterTreeSourceModel(QObject *parent)
    :BaseTreeModel(parent)
{
    m_roleNameMapping[TreeModelRoleName] = "display";

    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);
}

UserFilterTreeSourceModel::~UserFilterTreeSourceModel()
{
    removeAll();
}

Qt::ItemFlags UserFilterTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());

    if(item->childCount() > 0){

        flags ^= Qt::ItemIsSelectable;
        return flags;
    }

    return flags;
}

QModelIndex UserFilterTreeSourceModel::getIndexByValue(QString event)
{
    if(!m_treeItemModelMap.contains(event)){
        return QModelIndex();
    }

    TreeItemModel * item = m_treeItemModelMap.take(event);

    QModelIndex index = getIndex(item);

    return index;
}

void UserFilterTreeSourceModel::setUserData(QSet<QString> events)
{
    removeAll();
    addEventData(events);
}

BaseTreeItemInfo *UserFilterTreeSourceModel::newTreeItemInfo(const QString& uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    BaseTreeItemInfo *t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

TreeItemModel * UserFilterTreeSourceModel::AddChildend(TreeItemModel *parent, const ItemType& type, const QString &name)
{
    beginInsertRows(getIndex(parent),parent->childCount(),parent->childCount());
    BaseTreeItemInfo* baseItem = newTreeItemInfo(name, type, ItemStatus::Normal,name);
    TreeItemModel * model = new TreeItemModel(baseItem, parent);
    parent->appendChild(model);
    m_treeItemModelMap.insert(name,model);
    endInsertRows();

    return model;
}

void UserFilterTreeSourceModel::removeEventData(QSet<QString> &events)
{
    foreach(QString event, events){

        if(!m_treeItemModelMap.contains(event)){
            continue;
        }

        //아이템 삭제
        TreeItemModel* item = m_treeItemModelMap.take(event);
        TreeItemModel* parent = item->parentItem();

        beginRemoveRows(getIndex(parent),item->row(),item->row());
        m_treeItemModelMap.remove(item->getUuid());
        parent->removeChild(item->row());
        endRemoveRows();

        //parent가 MainRoot가 아닌 경우 child가 없는 상태가 되면 삭제.
        if((ItemType::MainRoot != parent->getItemType())
                &&(0 == parent->childCount())){

            item = parent;
            parent = item->parentItem();

            beginRemoveRows(getIndex(parent),item->row(),item->row());
            m_treeItemModelMap.remove(item->getUuid());
            parent->removeChild(item->row());
            endRemoveRows();
        }
    }
}

void UserFilterTreeSourceModel::addEventData(QSet<QString> &events)
{
    //순서
    //Service
    //UserName

    if(events.end() != events.find(QString::fromUtf8("Service"))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8("Service"));
        events.remove(QString::fromUtf8("Service"));
    }

    foreach(QString event , events){
        AddChildend(rootItem, ItemType::Event, event);
        events.remove(event);
    }
}

void UserFilterTreeSourceModel::removeAll()
{
    beginResetModel();

    auto childrenList = rootItem->children();

    foreach(TreeItemModel* itemModel, childrenList){
        itemModel->removeAllChild();
    }
    rootItem->removeAllChild();
    m_treeItemModelMap.clear();
    endResetModel();
}

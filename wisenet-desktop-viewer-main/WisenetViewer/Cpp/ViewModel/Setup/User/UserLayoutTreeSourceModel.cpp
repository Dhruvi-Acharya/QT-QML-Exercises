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
#include "UserLayoutTreeSourceModel.h"
#include <QUuid>

UserLayoutTreeSourceModel::UserLayoutTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &UserLayoutTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);

    readLayouts();
}

UserLayoutTreeSourceModel::~UserLayoutTreeSourceModel()
{

}

Qt::ItemFlags UserLayoutTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if(ItemType::Layout == type){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void UserLayoutTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::SaveLayoutEventType:
    case Wisenet::Core::RemoveLayoutsEventType:
        readLayouts();
        break;
    }
}

BaseTreeItemInfo *UserLayoutTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *UserLayoutTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
{
    DeviceTreeItemInfo *t = new DeviceTreeItemInfo(this);
    t->setType(type);
    t->setItemStatus(status);
    t->setUuid(uuid);
    t->setDisplayName(name);

    t->setModelName(modelName);
    t->setDeviceName(deviceName);
    t->setAddress(ipAddress);
    t->setChannelNumber(channelNumber);

    return t;
}

void UserLayoutTreeSourceModel::setupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::LayoutRoot, ItemStatus::Normal, tr("Layout"));
    rootItem = new TreeItemModel(rootInfo);
    QUuid empty;
    auto* layoutInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::LayoutRoot, ItemStatus::Normal, tr("Root"));
    m_resourceRootModel = new TreeItemModel(layoutInfo, rootItem);
    rootItem->appendChild(m_resourceRootModel);
    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);
    endResetModel();

}

void UserLayoutTreeSourceModel::readLayouts()
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    //auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    // 2023.01.02. coverity
    std::map<Wisenet::uuid_string, Wisenet::Core::Layout> layouts;
    if (db != nullptr) {
        layouts = db->GetLayouts();
    }

    for (auto& kv : layouts) {
        auto& layout = kv.second;
        addTreeLayoutItem(layout);
    }
    endResetModel();
}

void UserLayoutTreeSourceModel::addTreeLayoutItem(const Wisenet::Core::Layout &layout)
{
    QString layoutId = QString::fromStdString(layout.layoutID);
    SPDLOG_DEBUG("UserLayoutTreeSourceModel::addTreeLayoutItem layout:{} {}", layout.name, layout.layoutID);

    BaseTreeItemInfo* layoutInfo = newTreeItemInfo(layoutId, ItemType::Layout, ItemStatus::Normal, QString::fromStdString(layout.name));
    TreeItemModel* layoutModel = new TreeItemModel(layoutInfo, m_resourceRootModel);
    m_resourceRootModel->appendChild(layoutModel);
    m_treeItemModelMap.insert(layoutId, layoutModel);
}

QModelIndex UserLayoutTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}


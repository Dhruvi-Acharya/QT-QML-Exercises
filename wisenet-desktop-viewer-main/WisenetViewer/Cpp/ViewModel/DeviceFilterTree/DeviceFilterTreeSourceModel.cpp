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
#include "DeviceFilterTreeSourceModel.h"

DeviceFilterTreeSourceModel::DeviceFilterTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
    ,m_isSelectableDeviceItem(false)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
}

DeviceFilterTreeSourceModel::~DeviceFilterTreeSourceModel()
{

}

Qt::ItemFlags DeviceFilterTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if((ItemType::Camera == type) || (ItemType::Channel == type)){
        return flags;
    }

    if(m_isSelectableDeviceItem && (ItemType::Device == type)){
            return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceFilterTreeSourceModel::setSelectableDeviceItem(bool isSelectableDeviceItem)
{
    m_isSelectableDeviceItem = isSelectableDeviceItem;
}

void DeviceFilterTreeSourceModel::setDevices(QVector<QStringList> cameraList)
{
    qDebug() << "DeviceFilterTreeSourceModel::setDevices" << cameraList;

    removeAll();

    beginResetModel();
    foreach(const QStringList &camera , cameraList){

        if(camera.size() < 4)
            continue;
        TreeItemModel * parent = addTreeDeviceItem(camera[0],camera[2]);

        if(parent && (0 < camera[1].length())){
            addChannelItem(parent,camera[1],camera[3]);
        }
    }
    endResetModel();
}


BaseTreeItemInfo *DeviceFilterTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceFilterTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceFilterTreeSourceModel::setupCategoryTree()
{
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    beginResetModel();
    QUuid empty;
    auto* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    m_resourceRootModel = new TreeItemModel(deviceInfo, rootItem);
    rootItem->appendChild(m_resourceRootModel);
    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);
    endResetModel();
}

TreeItemModel * DeviceFilterTreeSourceModel::addTreeDeviceItem(const QString& deviceId, const QString& deviceName)
{
    if(m_treeItemModelMap.contains(deviceId)){
        return m_treeItemModelMap[deviceId];
    }

    QString displayName;
    Wisenet::Device::Device device;
    if(QCoreServiceManager::Instance().DB()->FindDevice(deviceId.toStdString(),device)){
        displayName = QString::fromUtf8(device.name.c_str());
    }else{
        displayName = deviceName;
    }

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, ItemStatus::Normal, displayName);
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);

    return deviceModel;
}

void DeviceFilterTreeSourceModel::addChannelItem(TreeItemModel *parent, const QString& channelId, const QString& channelName)
{
    QString channelUuid = parent->getUuid();
    channelUuid += "_";
    channelUuid += channelId;

    if(m_treeItemModelMap.contains(channelUuid)){
        return;
    }

    QString displayName;
    Wisenet::Device::Device::Channel channel;
    if(QCoreServiceManager::Instance().DB()->FindChannel(parent->getUuid().toStdString(),channelId.toStdString(),channel)){
        displayName = QString::fromUtf8(channel.name.c_str());
    }else{
        displayName = channelName;
    }

    BaseTreeItemInfo* channelInfo = newTreeItemInfo(channelUuid,
                                                    ItemType::Camera,
                                                    ItemStatus::Normal,
                                                    displayName,
                                                    "",
                                                    parent->getName(),
                                                    "",
                                                    1);

    TreeItemModel* channelModel = new TreeItemModel(channelInfo, parent);
    parent->appendChild(channelModel);
    m_treeItemModelMap.insert(channelUuid, channelModel);
}

void DeviceFilterTreeSourceModel::removeAll()
{
    beginResetModel();

    auto childrenList = m_resourceRootModel->children();

    foreach(TreeItemModel* itemModel, childrenList){
        itemModel->removeAllChild();
    }
    m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();
    endResetModel();
}

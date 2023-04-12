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
#include "DeviceEventTreeSourceModel.h"
#include <QUuid>

DeviceEventTreeSourceModel::DeviceEventTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceEventTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceEventTreeSourceModel::~DeviceEventTreeSourceModel()
{

}

Qt::ItemFlags DeviceEventTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if(ItemType::Device == type){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceEventTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
    case Wisenet::Core::UpdateDevicesEventType:
        readDevice();
        break;
    }
}

BaseTreeItemInfo *DeviceEventTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceEventTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceEventTreeSourceModel::setupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);
    QUuid empty;
    auto* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    m_resourceRootModel = new TreeItemModel(deviceInfo, rootItem);
    rootItem->appendChild(m_resourceRootModel);
    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);
    endResetModel();
}


void DeviceEventTreeSourceModel::readDevice()
{
    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    SPDLOG_DEBUG("DeviceEventTreeSourceModel::readDevice()");
    auto devices = QCoreServiceManager::Instance().DB()->GetDevices();
    auto devicesStatus = QCoreServiceManager::Instance().DB()->GetDevicesStatus();
    for (auto& kv : devices) {
        auto& device = kv.second;
        auto& deviceStatus = devicesStatus[kv.first];
        addTreeDeviceItem(device, deviceStatus);
    }
    endResetModel();
}

void DeviceEventTreeSourceModel::addTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    SPDLOG_DEBUG("DeviceEventTreeSourceModel::addTreeDeviceItem device:{} {}", device.name, device.deviceID);

    ItemStatus status = deviceStatus.isGood()?ItemStatus::Normal:ItemStatus::Abnormal;

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.modelName + "(" + device.connectionInfo.host +")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);
}

QModelIndex DeviceEventTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

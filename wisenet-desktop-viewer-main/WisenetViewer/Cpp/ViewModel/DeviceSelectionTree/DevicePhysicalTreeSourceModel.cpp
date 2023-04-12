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
#include "DevicePhysicalTreeSourceModel.h"
#include <QUuid>

DevicePhysicalTreeSourceModel::DevicePhysicalTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DevicePhysicalTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);

    readDevices();
}

DevicePhysicalTreeSourceModel::~DevicePhysicalTreeSourceModel()
{

}

Qt::ItemFlags DevicePhysicalTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if((ItemType::Camera == type) || (ItemType::Channel == type)){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DevicePhysicalTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
    case Wisenet::Core::UpdateDevicesEventType:
        readDevices();
        break;
    }
}

BaseTreeItemInfo *DevicePhysicalTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DevicePhysicalTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DevicePhysicalTreeSourceModel::setupCategoryTree()
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

void DevicePhysicalTreeSourceModel::readDevices(bool all)
{
    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    auto devices = QCoreServiceManager::Instance().DB()->GetDevices();
    auto devicesStatus = QCoreServiceManager::Instance().DB()->GetDevicesStatus();
    for (auto& kv : devices) {
        auto& device = kv.second;
        auto& deviceStatus = devicesStatus[kv.first];
        addTreeDeviceItem(device, deviceStatus);
        if(all || deviceStatus.isGood())
            addTreeDeviceChannelItem(device, deviceStatus);
    }
    endResetModel();
}

void DevicePhysicalTreeSourceModel::addTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    SPDLOG_DEBUG("AddTreeDeviceItem device:{} {}", device.name, device.deviceID);

    ItemStatus status = deviceStatus.isGood()?ItemStatus::Normal:ItemStatus::Abnormal;

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.modelName + "(" + device.connectionInfo.host +")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);
}

void DevicePhysicalTreeSourceModel::addTreeDeviceChannelItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus &deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);

    for (auto& item : device.channels) {
        int channelNumber = std::stoi(item.second.channelID);
        ItemStatus status = ItemStatus::Abnormal;
        std::string stdChannelId = device.deviceID + "_" + item.first;
        QString channelUuid = QString::fromStdString(stdChannelId);
        auto citr = deviceStatus.channelsStatus.find(item.first);
        if (citr != deviceStatus.channelsStatus.end()) {
            auto& channelStatus = citr->second;
            if (channelStatus.isGood())
                status = ItemStatus::Normal;

            SPDLOG_DEBUG("addTreeDeviceChannelItem ch:{} status:{}", channelNumber, status);
        }

        BaseTreeItemInfo* channelInfo = newTreeItemInfo(channelUuid,
                                                        ItemType::Camera,
                                                        status,
                                                        QString::fromUtf8(item.second.name.c_str()),
                                                        QString::fromUtf8(item.second.channelName.c_str()),
                                                        QString::fromUtf8(item.second.channelName.c_str()),
                                                        QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                        channelNumber);

        TreeItemModel* parentModel = m_treeItemModelMap[deviceId];
        TreeItemModel* channelModel = new TreeItemModel(channelInfo, parentModel);
        parentModel->appendChild(channelModel);
        SPDLOG_DEBUG("AddTreeDeviceItem channel:{} ++", channelModel->getUuid().toStdString());
        m_treeItemModelMap.insert(channelModel->getUuid(), channelModel);
    }
}

QModelIndex DevicePhysicalTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

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
#include "DeviceListTreeSourceModel.h"
#include <QUuid>
#include "LogSettings.h"

DeviceListTreeSourceModel::DeviceListTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
{
    SPDLOG_DEBUG("DeviceListTreeSourceModel()");
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceListTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceListTreeSourceModel::~DeviceListTreeSourceModel()
{
    SPDLOG_DEBUG("~DeviceListTreeSourceModel()");
}

Qt::ItemFlags DeviceListTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if((ItemType::Device == type) || (ItemType::Camera == type) || (ItemType::Channel == type)){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceListTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:  readDevices(); break;
    case Wisenet::Core::AddDeviceEventType: addDevice(event); break;
    case Wisenet::Core::RemoveDevicesEventType: removeDevices(event); break;
    case Wisenet::Device::DeviceStatusEventType: deviceStatusEvent(event); break;
    case Wisenet::Device::ChannelStatusEventType:  channelStatusEvent(event); break;
    case Wisenet::Core::UpdateDevicesEventType: readDevices(); break;
    case Wisenet::Core::SaveChannelsEventType: readDevices(); break;
    }
}

void DeviceListTreeSourceModel::addDevice(QCoreServiceEventPtr event){

    qDebug() << "DeviceListTreeSourceModel::addDevice()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);

    if(!m_treeItemModelMap.contains(QString::fromStdString(coreEvent->device.deviceID)))
    {
        beginResetModel();
        addTreeDeviceItem(coreEvent->device, coreEvent->deviceStatus);
        addTreeDeviceChannelItem(coreEvent->device, coreEvent->deviceStatus);
        endResetModel();
        emit expand();
    }
}

void DeviceListTreeSourceModel::removeDevices(QCoreServiceEventPtr event){

    qDebug() << "DeviceListTreeSourceModel::removeDevices()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event->eventDataPtr);
    beginResetModel();
    for(auto & item : coreEvent->deviceIDs){
        QString deviceId = QString::fromStdString(item);

        if(m_treeItemModelMap.contains(deviceId)){
            auto model = getModelIndex(deviceId);
            m_treeItemModelMap.remove(deviceId);
            m_resourceRootModel->removeChild(model.row());
        }
    }
    endResetModel();
    emit expand();
}

void DeviceListTreeSourceModel::deviceStatusEvent(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);

    SPDLOG_DEBUG("DeviceListTreeSourceModel::deviceStatusEvent deviceUuid {}", deviceStatusEvent->deviceID);

    if(m_treeItemModelMap.contains(deviceUuid))
    {
        TreeItemModel* deviceItemModel = m_treeItemModelMap[deviceUuid];
        int deviceStatus = deviceStatusEvent->deviceStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;
        deviceItemModel->getTreeItemInfo()->setItemStatus(deviceStatus);

        SPDLOG_DEBUG("DeviceListTreeSourceModel::deviceStatusEvent deviceUuid [{}] name[{}] status[{}]", deviceStatusEvent->deviceID, deviceItemModel->getName().toStdString(), deviceStatus);
    }

    for (auto& channel : deviceStatusEvent->deviceStatus.channelsStatus)
    {
        QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int channelStatus = channel.second.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("DeviceListTreeSourceModel::deviceStatusEvent channelUuid [{}] name[{}] status[{}]", channelUuid.toStdString(), channelItemModel->getName().toStdString(), channelStatus);
            channelItemModel->getTreeItemInfo()->setItemStatus(channelStatus);
        }
    }
}

void DeviceListTreeSourceModel::channelStatusEvent(QCoreServiceEventPtr event)
{
    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    for (auto& channel : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus channelStatus = channel.second;

        QString channelUuid = QString::fromStdString(channelStatus.deviceID) + "_" + QString::fromStdString(channelStatus.channelID);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int status = channelStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("DeviceListTreeSourceModel::channelStatusEvent channelUuid [{}] name[{}] status[{}]", channelUuid.toStdString(), channelItemModel->getName().toStdString(), status);
            channelItemModel->getTreeItemInfo()->setItemStatus(status);
        }
    }
}

BaseTreeItemInfo *DeviceListTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceListTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceListTreeSourceModel::setupCategoryTree()
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

void DeviceListTreeSourceModel::readDevices()
{
    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    auto devices = QCoreServiceManager::Instance().DB()->GetDevices();
    auto devicesStatus = QCoreServiceManager::Instance().DB()->GetDevicesStatus();


    SPDLOG_DEBUG("DeviceListTreeSourceModel::readDevices device start count:",
                 devices.size());
    for (auto& kv : devices) {
        auto& device = kv.second;
        auto& deviceStatus = devicesStatus[kv.first];

        SPDLOG_DEBUG("DeviceListTreeSourceModel::readDevices device:{} {}",
                     device.connectionInfo.host, (int)deviceStatus.status);

        addTreeDeviceItem(device, deviceStatus);
        addTreeDeviceChannelItem(device, deviceStatus);
    }
    endResetModel();
    emit expand();
}

void DeviceListTreeSourceModel::addTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    SPDLOG_DEBUG("AddTreeDeviceItem device:{} {}", device.name, device.deviceID);

    ItemStatus status = deviceStatus.isGood()?ItemStatus::Normal:ItemStatus::Abnormal;

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.name + " (" + device.connectionInfo.host + ")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);
}

void DeviceListTreeSourceModel::addTreeDeviceChannelItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus &deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    for (auto& item : device.channels) {
        int channelNumber = std::stoi(item.second.channelID);
        ItemStatus status = ItemStatus::Unused;
        std::string stdChannelId = device.deviceID + "_" + item.first;
        QString channelUuid = QString::fromStdString(stdChannelId);
        auto citr = deviceStatus.channelsStatus.find(item.first);
        if (citr != deviceStatus.channelsStatus.end()) {
            auto& channelStatus = citr->second;
            if (channelStatus.isGood() && item.second.use == true)
                status = ItemStatus::Normal;
            else if(!channelStatus.isGood())
                 status = ItemStatus::Abnormal;
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

QModelIndex DeviceListTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

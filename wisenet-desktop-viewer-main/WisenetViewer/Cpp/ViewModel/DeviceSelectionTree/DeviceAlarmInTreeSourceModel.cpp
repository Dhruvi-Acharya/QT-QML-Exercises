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
#include "DeviceAlarmInTreeSourceModel.h"
#include <QUuid>
#include "CoreServiceUtil.h"

DeviceAlarmInTreeSourceModel::DeviceAlarmInTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceAlarmInTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceAlarmInTreeSourceModel::~DeviceAlarmInTreeSourceModel()
{

}

Qt::ItemFlags DeviceAlarmInTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if(ItemType::AlarmIn == type){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceAlarmInTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
    case Wisenet::Core::UpdateDevicesEventType:
        readAlarmIn(m_isNetworkAlarmIn);
        break;
    }
}

BaseTreeItemInfo *DeviceAlarmInTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceAlarmInTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceAlarmInTreeSourceModel::setupCategoryTree()
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


void DeviceAlarmInTreeSourceModel::readAlarmIn(bool isNetworkAlarmIn)
{
    m_isNetworkAlarmIn = isNetworkAlarmIn;
    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    SPDLOG_DEBUG("DeviceAlarmInTreeSourceModel::readAlarmIn()");
    auto devices = QCoreServiceManager::Instance().DB()->GetDevices();
    auto devicesStatus = QCoreServiceManager::Instance().DB()->GetDevicesStatus();
    for (auto& kv : devices) {
        auto& device = kv.second;
        auto& deviceStatus = devicesStatus[kv.first];
        addTreeDeviceItem(device, deviceStatus);
        addTreeDeviceAlarmInItem(device, deviceStatus);
    }
    endResetModel();
}

void DeviceAlarmInTreeSourceModel::addTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    SPDLOG_DEBUG("DeviceAlarmInTreeSourceModel::AddTreeDeviceItem device:{} {}", device.name, device.deviceID);

    ItemStatus status = deviceStatus.isGood()?ItemStatus::Normal:ItemStatus::Abnormal;

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.modelName + "(" + device.connectionInfo.host +")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);
}

void DeviceAlarmInTreeSourceModel::addTreeDeviceAlarmInItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus &deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    if(!m_isNetworkAlarmIn){
        for (auto& item : device.deviceInputs) {
            SPDLOG_DEBUG("DeviceAlarmInTreeSourceModel::addTreeDeviceAlarmInItem {} -- {}", item.first, item.second.deviceInputID);

            int inputNumber = std::stoi(item.second.deviceInputID);
            ItemStatus status = ItemStatus::Normal;
            QString inputUuid = QString::fromStdString(device.deviceID + "_" + item.first);

            QString name = QString::fromStdString(device.modelName + "-" +
                                                  Wisenet::Core::fillDigits(device.deviceInputs.size(), item.second.deviceInputID) +
                                                  "(AlarmIn)");

            BaseTreeItemInfo* alarmInfo = newTreeItemInfo(inputUuid, ItemType::AlarmIn, status, name, name, name,
                                                          QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                          inputNumber);

            TreeItemModel* parentModel = m_treeItemModelMap[deviceId];
            TreeItemModel* childModel = new TreeItemModel(alarmInfo, parentModel);
            parentModel->appendChild(childModel);
            SPDLOG_DEBUG("addTreeDeviceAlarmOutItem alarm:{} ++", childModel->getUuid().toStdString());
            m_treeItemModelMap.insert(childModel->getUuid(), childModel);
        }
    }
    else{
        int channelSize = device.channels.size();
        for (auto& item : device.channels) {
            for( auto & item2 : item.second.channelInputs){
                int inputNumber = std::stoi(item2.second.inputID);
                ItemStatus status = ItemStatus::Normal;
                QString inputUuid = QString::fromStdString(device.deviceID + "_" + item.first + "_" + item2.first);
                QString name = QString::fromStdString(device.modelName + "-CH" +
                                                      Wisenet::Core::fillDigits(channelSize, item.first) + "-" + item2.first + " (Alarm Input)");

                BaseTreeItemInfo* alarmInfo = newTreeItemInfo(inputUuid, ItemType::AlarmIn, status, name, name, name,
                                                              QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                              inputNumber);

                TreeItemModel* parentModel = m_treeItemModelMap[deviceId];
                TreeItemModel* childModel = new TreeItemModel(alarmInfo, parentModel);
                parentModel->appendChild(childModel);
                SPDLOG_DEBUG("addTreeDeviceAlarmOutItem alarm:{} ++", childModel->getUuid().toStdString());
                m_treeItemModelMap.insert(childModel->getUuid(), childModel);
            }
        }
    }
}

QModelIndex DeviceAlarmInTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

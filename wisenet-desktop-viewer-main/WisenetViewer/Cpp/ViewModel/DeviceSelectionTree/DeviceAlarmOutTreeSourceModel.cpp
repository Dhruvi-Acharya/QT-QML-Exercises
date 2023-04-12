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
#include "DeviceAlarmOutTreeSourceModel.h"
#include <QUuid>
#include "CoreServiceUtil.h"

DeviceAlarmOutTreeSourceModel::DeviceAlarmOutTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceAlarmOutTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceAlarmOutTreeSourceModel::~DeviceAlarmOutTreeSourceModel()
{

}

Qt::ItemFlags DeviceAlarmOutTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if(ItemType::AlarmOutput == type){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceAlarmOutTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
    case Wisenet::Core::UpdateDevicesEventType:
        readAlarmOut();
        break;
    }
}

BaseTreeItemInfo *DeviceAlarmOutTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceAlarmOutTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceAlarmOutTreeSourceModel::setupCategoryTree()
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


void DeviceAlarmOutTreeSourceModel::readAlarmOut()
{
    beginResetModel();
    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    SPDLOG_DEBUG("DeviceAlarmOutTreeSourceModel::readAlarmOut()");
    auto devices = QCoreServiceManager::Instance().DB()->GetDevices();
    auto devicesStatus = QCoreServiceManager::Instance().DB()->GetDevicesStatus();
    for (auto& kv : devices) {
        auto& device = kv.second;
        auto& deviceStatus = devicesStatus[kv.first];
        addTreeDeviceItem(device, deviceStatus);
        addTreeDeviceAlarmOutItem(device, deviceStatus);
    }
    endResetModel();
}

void DeviceAlarmOutTreeSourceModel::addTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);
    SPDLOG_DEBUG("DeviceAlarmOutTreeSourceModel::AddTreeDeviceItem device:{} {}", device.name, device.deviceID);

    ItemStatus status = deviceStatus.isGood()?ItemStatus::Normal:ItemStatus::Abnormal;

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.modelName + "(" + device.connectionInfo.host +")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(deviceId, deviceModel);
}

static int numDigits(size_t number)
{
    int digits = 0;
    if (number < 0)
        digits = 1;
    while (number) {
        number /= 10;
        digits++;
    }
    return digits;
}

void DeviceAlarmOutTreeSourceModel::addTreeDeviceAlarmOutItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus &deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);


    for (auto& item : device.deviceOutputs) {
        SPDLOG_DEBUG("DeviceAlarmOutTreeSourceModel::addTreeDeviceAlarmOutItem deviceoutput:{} -- {} -- {}", item.first, item.second.deviceOutputID, item.second.outputType);

        int outputNumber = std::stoi(item.second.deviceOutputID);
        ItemStatus status = ItemStatus::Normal;
        QString outputUuid = QString::fromStdString( device.deviceID + "_" + item.first);
        QString alarmoutName = QString::fromStdString(device.modelName + "-" +
                                                      Wisenet::Core::fillDigits(device.deviceOutputs.size(), item.second.deviceOutputID) +
                                                      (item.second.outputType ==Wisenet::Device::Device::DeviceOutput::OutputType::Beep? " (Beep)": " (AlarmOut)"));

        BaseTreeItemInfo* alarmInfo = newTreeItemInfo(outputUuid,
                                                        ItemType::AlarmOutput,
                                                        status,
                                                        alarmoutName,
                                                        alarmoutName,
                                                        alarmoutName,
                                                        alarmoutName,
                                                        outputNumber);

        TreeItemModel* parentModel = m_treeItemModelMap[deviceId];
        TreeItemModel* childModel = new TreeItemModel(alarmInfo, parentModel);
        parentModel->appendChild(childModel);
        SPDLOG_DEBUG("addTreeDeviceAlarmOutItem alarm:{} ++", childModel->getUuid().toStdString());
        m_treeItemModelMap.insert(childModel->getUuid(), childModel);

    }
}

QModelIndex DeviceAlarmOutTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

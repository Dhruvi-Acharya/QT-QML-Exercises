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
#include "DeviceSelectionTreeSourceModel.h"
#include <QUuid>

DeviceSelectionTreeSourceModel::DeviceSelectionTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";

    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    SetupCategoryTree();

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceSelectionTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceSelectionTreeSourceModel::~DeviceSelectionTreeSourceModel()
{

}

Qt::ItemFlags DeviceSelectionTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if((ItemType::Camera == type)
            || (ItemType::Channel == type)){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceSelectionTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("MainTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        Event_FullInfo(event);
        break;
    case Wisenet::Core::SaveChannelsEventType:
        Event_SaveChannels(event);
        break;
    case Wisenet::Core::SaveGroupEventType:
        Event_SaveGroup(event);
        break;
    case Wisenet::Core::RemoveGroupsEventType:
        Event_RemoveGroup(event);
        break;
    case Wisenet::Core::AddGroupMappingEventType:
        Event_AddGroupMapping(event);
        break;
    case Wisenet::Core::RemoveGroupMappingEventType:
        Event_RemoveGroupMapping(event);
        break;
    case Wisenet::Core::UpdateGroupMappingEventType:
        Event_UpdateGroupMapping(event);
        break;
    case Wisenet::Core::RemoveDevicesEventType:
        Event_RemoveDevices(event);
        break;
    case Wisenet::Device::DeviceStatusEventType:
        Event_DeviceStatus(event);
        break;
    case Wisenet::Device::ChannelStatusEventType:
        Event_ChannelStatus(event);
        break;
    case Wisenet::Core::UpdateDevicesEventType:
        Event_UpdateDevices(event);
        break;
    }
}

void DeviceSelectionTreeSourceModel::initializeTree()
{
    clear();

    std::map<Wisenet::uuid_string, Wisenet::Core::Group> groupMap = QCoreServiceManager::Instance().DB()->GetGroups();
    std::map<Wisenet::uuid_string, Wisenet::Device::Device> deviceMap = QCoreServiceManager::Instance().DB()->GetDevices();
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceStatus> deviceStatusMap = QCoreServiceManager::Instance().DB()->GetDevicesStatus();

    for (auto& kv : groupMap) {
        auto& group = kv.second;
        AddTreeGroupItem(group);
    }

    for (auto& kv : deviceMap) {
        auto& device = kv.second;
        auto& deviceStatus = deviceStatusMap[kv.first];
        AddTreeDeviceItem(device, deviceStatus);
    }

    emit treeInitialized();
}

BaseTreeItemInfo *DeviceSelectionTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceSelectionTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name,
                                                                  const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber, const bool smartSearchCap)
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
    t->setSmartSearchCap(smartSearchCap);

    return t;
}

void DeviceSelectionTreeSourceModel::SetupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    QUuid empty;
    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    m_resourceRootModel = new TreeItemModel(deviceInfo, rootItem);

    rootItem->appendChild(m_resourceRootModel);

    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);

    endResetModel();
}

void DeviceSelectionTreeSourceModel::clear()
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::clear()");
    m_treeItemModelMap.clear();

    if (rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }

    SetupCategoryTree();
}

void DeviceSelectionTreeSourceModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    clear();

    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    for (auto& kv : coreEvent->groups) {
        auto& group = kv.second;
        AddTreeGroupItem(group);
    }

    for (auto& kv : coreEvent->devices) {
        auto& device = kv.second;
        auto& deviceStatus = coreEvent->devicesStatus[kv.first];
        AddTreeDeviceItem(device, deviceStatus);
    }

    emit treeInitialized();
}

void DeviceSelectionTreeSourceModel::Event_SaveGroup(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_SaveGroup()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveGroupEvent>(event->eventDataPtr);

    QString qroupId = QString::fromStdString(coreEvent->group.groupID);

    if(this->m_treeItemModelMap.contains(qroupId))
        UpdateTreeGroupItem(coreEvent->group);
    else
        AddTreeGroupItem(coreEvent->group);
}

void DeviceSelectionTreeSourceModel::Event_RemoveGroup(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroup()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupsEvent>(event->eventDataPtr);

    foreach(std::string groupId, coreEvent->groupIDs)
    {
        QString id = QString::fromStdString(groupId);

        if(m_treeItemModelMap.contains(id))
        {
            TreeItemModel* model = m_treeItemModelMap[id];

            QModelIndex parentIndex = getParentIndex(model);

            beginRemoveRows(parentIndex, model->row(), model->row());
            model->parentItem()->removeChild(model->row());
            endRemoveRows();
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_AddGroupMapping(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_AddGroupMapping()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddGroupMappingEvent>(event->eventDataPtr);

    QString groupId = QString::fromStdString(coreEvent->groupID);

    if(m_treeItemModelMap.contains(groupId))
    {
        TreeItemModel* groupItemModel = m_treeItemModelMap[groupId];

        foreach(Wisenet::Core::Group::MappingItem item, coreEvent->channels)
        {
            QString channelId = QString::fromStdString(item.id);

            // 이미 있는 경우 move
            if(m_treeItemModelMap.contains(channelId))
            {
                SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_AddGroupMapping() move {}", channelId.toStdString());
                QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);
                TreeItemModel* itemModel = m_treeItemModelMap[channelId];

                beginRemoveRows(deviceRootIndex, itemModel->row(), itemModel->row());
                m_resourceRootModel->unlinkChild(itemModel->row());
                endRemoveRows();

                QModelIndex groupIndex = getIndex(groupItemModel);
                beginInsertRows(groupIndex, groupItemModel->childCount(), groupItemModel->childCount());
                groupItemModel->appendChild(itemModel);
                itemModel->setParentItem(groupItemModel);
                endInsertRows();
            }
            else // 없는 경우 add
            {
                SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_AddGroupMapping() add {}", channelId.toStdString());

                // Camera로 판단.
                if(channelId.contains('_'))
                {
                    QStringList split = channelId.split('_');

                    if(split.count() != 2)
                    {
                        SPDLOG_DEBUG("Event_AddGroupMapping() continue 1");
                        continue;
                    }

                    QString deviceId = split.at(0);
                    QString channelNumber = split.at(1);
                    std::string stdDeviceId = deviceId.toStdString();
                    std::string stdChannelNumber = channelNumber.toStdString();

                    Wisenet::Device::Device device;
                    Wisenet::Device::Device::Channel channel;
                    Wisenet::Device::ChannelStatus channelStatus;

                    if(!QCoreServiceManager::Instance().DB()->FindDevice(stdDeviceId, device))
                    {
                        SPDLOG_DEBUG("Event_AddGroupMapping() cannot find device : continue 2 {}", stdDeviceId);
                        continue;
                    }

                    if(!QCoreServiceManager::Instance().DB()->FindChannel(stdDeviceId, stdChannelNumber, channel))
                    {
                        SPDLOG_DEBUG("Event_AddGroupMapping() cannot find channel : continue 2 {} {}", stdDeviceId, stdChannelNumber);
                        continue;
                    }

                    if(m_isObjectSearchMode == true)
                    {
                        if(channel.mediaCapabilities.personSearch == false && channel.mediaCapabilities.faceSearch == false
                                && channel.mediaCapabilities.vehicleSearch == false && channel.mediaCapabilities.ocrSearch == false)
                        {
                            continue;
                        }
                    }

                    if(!QCoreServiceManager::Instance().DB()->FindChannelStatus(stdDeviceId, stdChannelNumber, channelStatus))
                    {
                        SPDLOG_DEBUG("Event_AddGroupMapping() cannot find channel status : continue 2 {} {}", stdDeviceId, stdChannelNumber);
                        continue;
                    }

                    ItemStatus status = channelStatus.isGood() ? ItemStatus::Normal : ItemStatus::Abnormal;

                    BaseTreeItemInfo* cameraInfo = newTreeItemInfo(channelId,
                                                                   ItemType::Camera,
                                                                   status,
                                                                   QString::fromUtf8(channel.name.c_str()),
                                                                   QString::fromUtf8(channel.channelName.c_str()),
                                                                   QString::fromUtf8(channel.channelName.c_str()),
                                                                   QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                                   channelNumber.toInt(),
                                                                   channel.mediaCapabilities.smartSearch);

                    TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, groupItemModel);
                    QModelIndex groupIndex = getIndex(groupItemModel);

                    beginInsertRows(groupIndex, groupItemModel->childCount(), groupItemModel->childCount());
                    groupItemModel->appendChild(cameraModel);
                    endInsertRows();

                    m_treeItemModelMap.insert(channelId, cameraModel);

                    SPDLOG_DEBUG("Event_AddGroupMapping() END");
                }

            }
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping()");

    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupMappingEvent>(event->eventDataPtr);

    QString groupId = QString::fromStdString(coreEvent->groupID);

    if(m_treeItemModelMap.contains(groupId))
    {
        TreeItemModel* groupItem = m_treeItemModelMap[groupId];

        //beginResetModel();
        foreach(std::string channelId, coreEvent->channelIDs)
        {
            QString id = QString::fromStdString(channelId);

            if(!m_treeItemModelMap.contains(id))
                continue;

            TreeItemModel* itemModel = m_treeItemModelMap[id];

            QModelIndex parentIndex = getIndex(groupItem);
            int rowNumber = itemModel->row();

            if(rowNumber < 0)
            {
                SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping() beginRemove continue {} {} {}", itemModel->getName().toStdString(), channelId, rowNumber);
                continue;
            }

            SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping() beginRemove {} {} {}", itemModel->getName().toStdString(), channelId, rowNumber);
            beginRemoveRows(parentIndex, rowNumber, rowNumber);
            bool unlinkResult = groupItem->unlinkChild(rowNumber);
            SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping() unlinkResult {} {} {} {}", itemModel->getName().toStdString(), channelId, rowNumber, unlinkResult);
            endRemoveRows();

            SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_RemoveGroupMapping() beginInsert {} {} {}", itemModel->getName().toStdString(), channelId, m_resourceRootModel->childCount());
            QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);
            beginInsertRows(deviceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
            m_resourceRootModel->appendChild(itemModel);
            itemModel->setParentItem(m_resourceRootModel);
            endInsertRows();
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_UpdateGroupMapping(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_UpdateGroupMapping()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::UpdateGroupMappingEvent>(event->eventDataPtr);

    QString from = QString::fromStdString(coreEvent->from);
    QString to = QString::fromStdString(coreEvent->to);

    if(m_treeItemModelMap.contains(from) && m_treeItemModelMap.contains(to))
    {
        TreeItemModel* parentItem = m_treeItemModelMap[from];
        TreeItemModel* newParentItem = m_treeItemModelMap[to];

        foreach(std::string channelId, coreEvent->channelIDs)
        {
            QString id = QString::fromStdString(channelId);

            if(m_treeItemModelMap.contains(id))
            {
                TreeItemModel* itemModel = m_treeItemModelMap[id];

                QModelIndex parentIndex = getIndex(parentItem);
                beginRemoveRows(parentIndex, itemModel->row(), itemModel->row());
                parentItem->unlinkChild(itemModel->row());
                endRemoveRows();

                QModelIndex newParentIndex = getIndex(newParentItem);
                beginInsertRows(newParentIndex, newParentItem->childCount(), newParentItem->childCount());
                newParentItem->appendChild(itemModel);
                itemModel->setParentItem(newParentItem);
                endInsertRows();
            }
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_AddDevice(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);
    AddTreeDeviceItem(coreEvent->device, coreEvent->deviceStatus);
}

void DeviceSelectionTreeSourceModel::Event_RemoveDevices(QCoreServiceEventPtr event)
{
    auto removeDevices = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event->eventDataPtr);

    for(auto & stdDeviceId : removeDevices->deviceIDs)
    {
        QString deviceId = QString::fromStdString(stdDeviceId);

        foreach(auto& key, m_treeItemModelMap.keys())
        {
            QStringList splitKey = key.split('_');

            if(splitKey.length() < 2)
                continue;

            if(splitKey[0] == deviceId)
            {
                // Remove channel;
                RemoveTreeChannelItem(key);
            }
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);

    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_DeviceStatus deviceUuid {}", deviceStatusEvent->deviceID);

    if(m_treeItemModelMap.contains(deviceUuid))
    {
        TreeItemModel* deviceItemModel = m_treeItemModelMap[deviceUuid];
        int deviceStatus = deviceStatusEvent->deviceStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;
        deviceItemModel->getTreeItemInfo()->setItemStatus(deviceStatus);

        SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_DeviceStatus deviceUuid [{}] name[{}] status[{}]", deviceStatusEvent->deviceID, deviceItemModel->getName().toStdString(), deviceStatus);
    }

    for (auto& channel : deviceStatusEvent->deviceStatus.channelsStatus)
    {
        QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int channelStatus = channel.second.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_DeviceStatus channelUuid [{}] name[{}] status[{}]", channelUuid.toStdString(), channelItemModel->getName().toStdString(), channelStatus);
            channelItemModel->getTreeItemInfo()->setItemStatus(channelStatus);
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_UpdateDevices(QCoreServiceEventPtr event)
{
    auto devicesEvent = std::static_pointer_cast<Wisenet::Core::UpdateDevicesEvent>(event->eventDataPtr);

    for(auto & device : devicesEvent->devices){
        QString deviceUuid = QString::fromStdString(device.deviceID);
        for (auto& channel : device.channels)
        {
            QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);
            if(m_treeItemModelMap.contains(channelUuid)){
                m_treeItemModelMap[channelUuid]->setName(QString::fromUtf8(channel.second.name.c_str()));
            }
        }
    }
    emit invalidate();
}

void DeviceSelectionTreeSourceModel::Event_ChannelStatus(QCoreServiceEventPtr event)
{
    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    for (auto& channel : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus channelStatus = channel.second;

        QString channelUuid = QString::fromStdString(channelStatus.deviceID) + "_" + QString::fromStdString(channelStatus.channelID);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int status = channelStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_DeviceStatus channelUuid [{}] name[{}] status[{}]", channelUuid.toStdString(), channelItemModel->getName().toStdString(), status);
            channelItemModel->getTreeItemInfo()->setItemStatus(status);
        }
    }
}

void DeviceSelectionTreeSourceModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_SaveChannels()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        QString id = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);

        SPDLOG_DEBUG("DeviceSelectionTreeSourceModel::Event_SaveChannels() channelUuid[{}] name[{}] use[{}]", id.toStdString(), channel.name, channel.use);

        if(channel.use == false)
        {
            // Remove
            RemoveTreeChannelItem(id);
        }
        else
        {
            if(m_treeItemModelMap.contains(id)) // Update
            {
                UpdateTreeChannelItem(channel);
            }
            else // Add
            {
                if(m_isObjectSearchMode == true)
                {
                    if(channel.mediaCapabilities.personSearch == false && channel.mediaCapabilities.faceSearch == false
                            && channel.mediaCapabilities.vehicleSearch == false && channel.mediaCapabilities.ocrSearch == false)
                    {
                        continue;
                    }
                }

                AddTreeChannelItemToRoot(channel);
            }
        }
    }
}


TreeItemModel *DeviceSelectionTreeSourceModel::AddTreeGroupItem(const Wisenet::Core::Group &group)
{
    QString groupID = QString::fromStdString(group.groupID);
    QString name = QString::fromUtf8(group.name.c_str());
    QString parentGroupID = QString::fromStdString(group.parentGroupID);

    QUuid emptyUuid;
    QString emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces);

    if(m_treeItemModelMap.contains(groupID))
    {
        qDebug() << "AddTreeGroupItem - already added : " << name << ", uuid : " << groupID;

        return m_treeItemModelMap[groupID];
    }

    // parent가 root
    if(parentGroupID == emptyUuidString)
    {
        qDebug() << "AddTreeGroupItem1 parentGroupID == emptyUuidString" << name << ", uuid : " << groupID << ", parent : " << parentGroupID;
        BaseTreeItemInfo* groupInfo = newTreeItemInfo(groupID, ItemType::Group, ItemStatus::Normal, name);

        QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);

        TreeItemModel* groupModel = new TreeItemModel(groupInfo, m_resourceRootModel);

        beginInsertRows(deviceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
        m_resourceRootModel->appendChild(groupModel);
        endInsertRows();

        m_treeItemModelMap.insert(groupID, groupModel);

        return groupModel;
    }
    else // parent가 있음.
    {
        qDebug() << "AddTreeGroupItem - has parent" << name << ", uuid : " << groupID << ", parent : " << parentGroupID;
        std::map<Wisenet::uuid_string, Wisenet::Core::Group> groupMap = QCoreServiceManager::Instance().DB()->GetGroups();
        TreeItemModel* parentModel;

        if(groupMap.find(group.parentGroupID) != groupMap.end())
        {
            Wisenet::Core::Group parentGroup = groupMap[group.parentGroupID];
            parentModel = AddTreeGroupItem(parentGroup);

            BaseTreeItemInfo* groupInfo = newTreeItemInfo(groupID, ItemType::Group, ItemStatus::Normal, name);
            TreeItemModel* groupModel = new TreeItemModel(groupInfo, parentModel);

            QModelIndex parentIndex = index(parentModel->row(), 0, getParentIndex(parentModel));

            beginInsertRows(parentIndex, parentModel->childCount(), parentModel->childCount());
            parentModel->appendChild(groupModel);
            endInsertRows();

            m_treeItemModelMap.insert(groupID, groupModel);

            return groupModel;
        }
        else
        {
            qDebug() << "AddTreeGroupItem - error " << name << ", uuid : " << groupID;
            return nullptr;
        }
    }
}

void DeviceSelectionTreeSourceModel::AddTreeDeviceItem(const Wisenet::Device::Device &device, const Wisenet::Device::DeviceStatus &deviceStatus)
{
    std::map<std::string, Wisenet::uuid_string> channelGroupMappings = QCoreServiceManager::Instance().DB()->GetChannelGroupMappings();

    for (auto& item : device.channels) {

        if(item.second.use == false)
            continue;

        // ObjectSearch일 때는, 관련 search를 지원하는 채널만 Tree에 담음
//        qDebug() << "channelNumber" << std::stoi(item.second.channelID) << "channelName : "<<QString::fromUtf8(item.second.channelName.c_str());
//        qDebug() << "personSearch : " << item.second.mediaCapabilities.personSearch << ", faceSearch : " << item.second.mediaCapabilities.faceSearch << ", vehicleSearch : " << item.second.mediaCapabilities.vehicleSearch << ", ocrSearch : " << item.second.mediaCapabilities.ocrSearch;
        SPDLOG_DEBUG("channelNumber:{} channelName:{} ",std::stoi(item.second.channelID), item.second.channelName.c_str());
        SPDLOG_DEBUG("personSearch:{} faceSearch:{} vehicleSearch:{} ocrSearch:{}", item.second.mediaCapabilities.personSearch, item.second.mediaCapabilities.faceSearch, item.second.mediaCapabilities.vehicleSearch, item.second.mediaCapabilities.ocrSearch);
        if(m_isObjectSearchMode == true)
        {
            if(item.second.mediaCapabilities.personSearch == false && item.second.mediaCapabilities.faceSearch == false
                    && item.second.mediaCapabilities.vehicleSearch == false && item.second.mediaCapabilities.ocrSearch == false)
            {
                continue;
            }
        }

        int channelNumber = std::stoi(item.second.channelID);
        ItemStatus status = ItemStatus::Abnormal;

        std::string stdChannelId = device.deviceID + "_" + item.first;
        QString channelUuid = QString::fromStdString(stdChannelId);

        auto citr = deviceStatus.channelsStatus.find(item.first);
        if (citr != deviceStatus.channelsStatus.end()) {
            auto& channelStatus = citr->second;
            if (channelStatus.isGood())
                status = ItemStatus::Normal;
        }

        BaseTreeItemInfo* channelInfo = newTreeItemInfo(channelUuid,
                                                        ItemType::Camera,
                                                        status,
                                                        QString::fromUtf8(item.second.name.c_str()),
                                                        QString::fromUtf8(item.second.channelName.c_str()),
                                                        QString::fromUtf8(item.second.channelName.c_str()),
                                                        QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                        channelNumber,
                                                        item.second.mediaCapabilities.smartSearch);

        if(channelGroupMappings.find(stdChannelId) != channelGroupMappings.end())
        {
            Wisenet::uuid_string stdGroupId = channelGroupMappings[stdChannelId];
            QString groupId = QString::fromStdString(stdGroupId);

            if(m_treeItemModelMap.contains(groupId))
            {
                TreeItemModel* parentModel = m_treeItemModelMap[groupId];
                QModelIndex parentIndex = getIndex(parentModel);
                TreeItemModel* channelModel = new TreeItemModel(channelInfo, parentModel);

                beginInsertRows(parentIndex, parentModel->childCount(), parentModel->childCount());
                parentModel->appendChild(channelModel);
                endInsertRows();

                m_treeItemModelMap.insert(channelModel->getUuid(), channelModel);
            }
        }
        else
        {
            QModelIndex parentIndex = index(0, 0);
            TreeItemModel* channelModel = new TreeItemModel(channelInfo, m_resourceRootModel);

            beginInsertRows(parentIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
            m_resourceRootModel->appendChild(channelModel);
            endInsertRows();

            m_treeItemModelMap.insert(channelModel->getUuid(), channelModel);
        }

    }
}

void DeviceSelectionTreeSourceModel::UpdateTreeGroupItem(const Wisenet::Core::Group& group)
{
    QString groupId = QString::fromStdString(group.groupID);
    QString parentGroupId = QString::fromStdString(group.parentGroupID);
    QString groupName = QString::fromStdString(group.name);

    qDebug() << "UpdateTreeGroupItem " << groupName << ", GroupId : " << groupId;

    TreeItemModel* treeItemModel = m_treeItemModelMap[groupId];

    if(treeItemModel->parentItem()->getUuid() != parentGroupId)
    {
        TreeItemModel* parentTreeItemModel = treeItemModel->parentItem();
        TreeItemModel* newParentTreeItemModel = m_treeItemModelMap[parentGroupId];

        QModelIndex parentIndex = getParentIndex(treeItemModel);

        qDebug() << "UpdateTreeGroupItem 1";

        beginRemoveRows(parentIndex, treeItemModel->row(), treeItemModel->row());
        parentTreeItemModel->unlinkChild(treeItemModel->row());
        endRemoveRows();

        qDebug() << "UpdateTreeGroupItem 2";
        QModelIndex newParentIndex = getIndex(newParentTreeItemModel);

        beginInsertRows(newParentIndex, newParentTreeItemModel->childCount(), newParentTreeItemModel->childCount());
        treeItemModel->setParentItem(newParentTreeItemModel);
        newParentTreeItemModel->appendChild(treeItemModel);
        endInsertRows();

        qDebug() << "UpdateTreeGroupItem 3";
    }
    else
    {
        treeItemModel->setName(groupName);
        invalidate();
    }
}

void DeviceSelectionTreeSourceModel::AddTreeChannelItemToRoot(const Wisenet::Device::Device::Channel& channel)
{
    QString deviceID = QString::fromStdString(channel.deviceID);
    QString channelNumber = QString::fromStdString(channel.channelID);

    QString channelId = deviceID + "_" + channelNumber;

    Wisenet::Device::Device device;
    Wisenet::Device::DeviceStatus deviceStatus;
    Wisenet::Device::ChannelStatus channelStatus;

    QCoreServiceManager::Instance().DB()->FindDevice(deviceID, device);
    QCoreServiceManager::Instance().DB()->FindDeviceStatus(deviceID, deviceStatus);
    QCoreServiceManager::Instance().DB()->FindChannelStatus(deviceID, channelNumber, channelStatus);

    BaseTreeItemInfo* cameraInfo = newTreeItemInfo(channelId,
                                                   ItemType::Camera,
                                                   channelStatus.isGood()? ItemStatus::Normal : ItemStatus::Abnormal,
                                                   QString::fromUtf8(channel.name.c_str()),
                                                   QString::fromUtf8(device.modelName.c_str()),
                                                   QString::fromUtf8(device.deviceName.c_str()),
                                                   QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                   channelNumber.toInt(),
                                                   channel.mediaCapabilities.smartSearch);

    QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);
    TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, m_resourceRootModel);

    beginInsertRows(deviceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
    m_resourceRootModel->appendChild(cameraModel);
    endInsertRows();

    m_treeItemModelMap.insert(channelId, cameraModel);
}

void DeviceSelectionTreeSourceModel::UpdateTreeChannelItem(const Wisenet::Device::Device::Channel& channel)
{
    QString deviceID = QString::fromStdString(channel.deviceID);
    QString channelNumber = QString::fromStdString(channel.channelID);

    QString channelId = deviceID + "_" + channelNumber;
    TreeItemModel* cameraModel = m_treeItemModelMap[channelId];

    QString name = QString::fromUtf8(channel.name.c_str());

    cameraModel->setName(name);
    invalidate();
}

void DeviceSelectionTreeSourceModel::RemoveTreeChannelItem(const QString& channelId)
{
    if(!m_treeItemModelMap.contains(channelId))
        return;

    TreeItemModel* treeModel = m_treeItemModelMap.take(channelId);
    TreeItemModel* parentModel = treeModel->parentItem();
    int rowNumber = treeModel->row();

    QModelIndex parentIndex = getIndex(parentModel);
    beginRemoveRows(parentIndex, rowNumber, rowNumber);
    parentModel->removeChild(rowNumber);
    endRemoveRows();
}

QModelIndex DeviceSelectionTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

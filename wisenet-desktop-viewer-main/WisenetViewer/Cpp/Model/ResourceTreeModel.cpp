/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "ResourceTreeModel.h"
#include "FileWriteManager.h"
#include "LogSettings.h"
#include "FilePathConveter.h"
#include <QUuid>
#include <QDebug>
#include <QSet>
#include <QDesktopServices>

ResourceTreeModel::ResourceTreeModel(QObject *parent)
    : BaseTreeModel(parent)
{
    SPDLOG_DEBUG("ResourceTreeModel()");
    m_roleNameMapping[TreeModelRoleName] = "display";

    SetupCategoryTree();

    auto path = QCoreServiceManager::Instance().Settings()->localRecordingPath();
    m_watcher.addPath(path);

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &ResourceTreeModel::coreServiceEventTriggered, Qt::QueuedConnection);
    connect(FileWriteManager::getInstance(), &FileWriteManager::endExport,
            this, &ResourceTreeModel::onEndExport, Qt::QueuedConnection);
    connect(QCoreServiceManager::Instance().Settings(), &QLocalSettings::localRecordingPathChanged,
            this, &ResourceTreeModel::onLocalRecordingPathChanged, Qt::QueuedConnection);

    connect(&m_watcher, &QFileSystemWatcher::fileChanged,
            this, &ResourceTreeModel::onLocalRecordingPathChanged, Qt::QueuedConnection);
    connect(&m_watcher, &QFileSystemWatcher::directoryChanged,
            this, &ResourceTreeModel::onLocalRecordingPathChanged, Qt::QueuedConnection);
    connect(QCoreServiceManager::Instance().Signature(), &SignatureVerifier::verifyFinished,
            this, &ResourceTreeModel::verifyFinished, Qt::QueuedConnection);
}

ResourceTreeModel::~ResourceTreeModel()
{
    SPDLOG_DEBUG("~ResourceTreeModel()");
    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }
}

BaseTreeItemInfo* ResourceTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
{
    BaseTreeItemInfo *t;

    if(type == ItemType::Layout)
    {
        t = new LayoutTreeItemInfo(this);
        t->setUuid(uuid);
        t->setType(type);
        t->setItemStatus(status);
        t->setDisplayName(name);
    }
    else
    {
        t = new BaseTreeItemInfo(this);
        t->setUuid(uuid);
        t->setType(type);
        t->setItemStatus(status);
        t->setDisplayName(name);
    }

    return t;
}

BaseTreeItemInfo* ResourceTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber, const QString& deviceUuid)
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
    t->setDeviceUuid(deviceUuid);

    return t;
}

void ResourceTreeModel::SetupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    QUuid empty;
    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    m_resourceRootModel = new TreeItemModel(deviceInfo, rootItem);

    BaseTreeItemInfo* localfilesInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::LocalFileRoot, ItemStatus::Normal, tr("Local Files"));
    m_localFileRootModel = new TreeItemModel(localfilesInfo, rootItem);

    //beginInsertRows(index(0,0), 0, 1);
    rootItem->appendChild(m_resourceRootModel);

    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);

    rootItem->appendChild(m_localFileRootModel);
    //endInsertRows();

    endResetModel();
}

void ResourceTreeModel::clear()
{
    SPDLOG_DEBUG("ResourceTreeModel::clear()");
    m_treeItemModelMap.clear();

    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }

    SetupCategoryTree();
}

void ResourceTreeModel::initializeTree()
{
    SPDLOG_DEBUG("ResourceTreeModel::initializeTree()");
    std::map<Wisenet::uuid_string, Wisenet::Core::Group> groupMap = QCoreServiceManager::Instance().DB()->GetGroups();
    std::map<Wisenet::uuid_string, Wisenet::Device::Device> deviceMap = QCoreServiceManager::Instance().DB()->GetDevices();
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceStatus> deviceStatusMap = QCoreServiceManager::Instance().DB()->GetDevicesStatus();
    std::map<Wisenet::uuid_string, Wisenet::Core::Webpage> webPageMap = QCoreServiceManager::Instance().DB()->GetWebpages();
    // TODO :: std::set<std::string> localfileSet = QCoreServiceManager::Instance().DB()->GetLocalfiles();

    for (auto& kv : groupMap) {
        auto& group = kv.second;
        AddTreeGroupItem(group);
    }

    for (auto& kv : deviceMap) {
        auto& device = kv.second;
        auto& deviceStatus = deviceStatusMap[kv.first];
        AddTreeDeviceItem(device, deviceStatus);
    }

    for (auto& kv : webPageMap) {
        auto& webpage = kv.second;
        AddTreeWebpageItem(webpage);
    }

    // load local file list
    reloadFileItems();

    emit expandRoot();
}

void ResourceTreeModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("ResourceTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

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
    case Wisenet::Core::SaveWebpageEventType:
        Event_SaveWebpage(event);
        break;
    case Wisenet::Core::RemoveWebpagesEventType:
        Event_RemoveWebpages(event);
        break;
        //case Wisenet::Core::AddDeviceEventType:
        //Event_AddDevice(event);
        //break;
    case Wisenet::Core::RemoveDevicesEventType:
        Event_RemoveDevices(event);
        break;
    case Wisenet::Device::DeviceStatusEventType:
        Event_DeviceStatus(event);
        break;
    case Wisenet::Device::ChannelStatusEventType:
        Event_ChannelStatus(event);
        break;
    case Wisenet::Core::AddLocalFileEventType:
        Event_AddLocalFile(event);
        break;
    case Wisenet::Core::RemoveLocalFileEventType:
        Event_RemoveLocalFile(event);
        break;
    case Wisenet::Core::UpdateDevicesEventType:
        Event_UpdateDevices(event);
        break;
    case Wisenet::Core::UpdateVerifyResultEventType:
        Event_UpdateLocalFileIcon(event);
        break;
    }
}

void ResourceTreeModel::ChangeParentGroup(QString newGroupId, QStringList children)
{
    QMap<QString, QString> channelGroupMapping;
    QSet<QString> groupSet;
    QSet<QString> channelSet;

    auto stdGroupMap = QCoreServiceManager::Instance().DB()->GetGroups();

    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 1");

    //1. Group, channel 분류
    foreach(QString uuid, children){
        if(uuid == "")
            continue;

        std::string stdUuid = uuid.toStdString();

        if(stdGroupMap.find(stdUuid) != stdGroupMap.end())
        {
            groupSet.insert(uuid);

            for(auto& kvp : stdGroupMap[stdUuid].mapping)
            {
                // Channel-Key : GroupID mapping
                channelGroupMapping.insert(QString::fromStdString(kvp.first), uuid);
            }
        }
        else
        {
            channelSet.insert(uuid);
        }
    }

    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 2");
    //2. Channel들이 선택된 그룹에 있는지 확인, 그룹에 있으면 뺌
    foreach(QString channelId, channelSet.values())
    {
        if(channelGroupMapping.contains(channelId))
        {
            channelSet.remove(channelId);
            continue;
        }

        // parent가 root인 경우는 건너 뜀
        QString parentGroupId = m_treeItemModelMap[channelId]->parentItem()->getUuid();
        QUuid empty;
        QString rootId = empty.toString(QUuid::WithoutBraces);

        if(parentGroupId == rootId)
            continue;

        bool hasAncestorInSet = checkAncestor(parentGroupId, groupSet);
        if(hasAncestorInSet)
        {
            channelSet.remove(channelId);
        }
    }

    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 3");
    //3. parent를 변경할 그룹 확인
    foreach(QString groupId, groupSet.values())
    {
        // 현재 parent에 다시 drop했을 경우에 제외
        if(stdGroupMap[groupId.toStdString()].parentGroupID == newGroupId.toStdString()){
            groupSet.remove(groupId);
            continue;
        }

        // 선택된 그룹 중에 상하위 모두 선택했을 때 하위 제외
        bool hasAncestorInSet = checkAncestor(groupId, groupSet);

        if(hasAncestorInSet)
            groupSet.remove(groupId);
    }

    //4. 남은 그룹, 채널에 대해서 그룹은 parent변경, 채널은 mapping 변경 request
    // channelSet --> 옮길 채널
    // groupSet --> parent를 바꿀 group
    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 4");
    QMap<QString, QList<QString>> channelMap;

    foreach(QString channel, channelSet)
    {
        TreeItemModel* itemModel = m_treeItemModelMap[channel];
        SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup name: {}, channelid: {}, parent {}", itemModel->getName().toStdString(), itemModel->getUuid().toStdString(), newGroupId.toStdString());

        QString parentId = itemModel->parentItem()->getUuid();

        if(channelMap.contains(parentId))
        {
            channelMap[parentId].append(channel);
        }
        else
        {
            QList<QString> list;
            list.append(channel);

            channelMap.insert(parentId, list);
        }
    }

    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 5");
    QUuid emptyUuid;
    QString emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces);
    QMapIterator<QString, QList<QString>> iterator(channelMap);
    while (iterator.hasNext()) {
        iterator.next();

        if(iterator.key() == emptyUuidString)
            Request_AddGroupMapping(newGroupId, iterator.value());
        else if(newGroupId == emptyUuidString)
            Request_RemoveGroupMapping(iterator.key(), iterator.value());
        else
            Request_UpdateGroupMapping(iterator.key(), newGroupId, iterator.value());
    }

    SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup() step 6");
    foreach(QString group, groupSet)
    {
        TreeItemModel* itemModel = m_treeItemModelMap[group];
        SPDLOG_DEBUG("ResourceTreeModel::ChangeParentGroup group name: {}, id: {}, parent: {}", itemModel->getName().toStdString(), itemModel->getUuid().toStdString(), newGroupId.toStdString());

        Wisenet::Core::Group localGroup = stdGroupMap[group.toStdString()];
        localGroup.parentGroupID = newGroupId.toStdString();

        Request_SaveGroup(localGroup);
    }
}

TreeItemModel* ResourceTreeModel::AddTreeGroupItem(const Wisenet::Core::Group& group)
{
    QString groupID = QString::fromStdString(group.groupID);
    QString name = QString::fromUtf8(group.name.c_str());
    QString parentGroupID = QString::fromStdString(group.parentGroupID);

    QUuid emptyUuid;
    QString emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces);

    if(m_treeItemModelMap.contains(groupID))
    {
        SPDLOG_DEBUG("AddTreeGroupItem - already added : {}, uuid : {}", group.name, group.groupID);

        return m_treeItemModelMap[groupID];
    }

    // parent가 root
    if(parentGroupID == emptyUuidString)
    {
        SPDLOG_DEBUG("AddTreeGroupItem1 parentGroupID == emptyUuidString {}, uuid : {}, parent : {}", group.name, group.groupID, group.parentGroupID);
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
        SPDLOG_DEBUG("AddTreeGroupItem - has parent {}, uuid : {}, parent : {}", group.name, group.groupID, group.parentGroupID);
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
            SPDLOG_DEBUG("AddTreeGroupItem - error {}, uuid : {}", group.name, group.groupID);
            return nullptr;
        }
    }
}

void ResourceTreeModel::UpdateTreeGroupItem(const Wisenet::Core::Group& group)
{
    QString groupId = QString::fromStdString(group.groupID);
    QString parentGroupId = QString::fromStdString(group.parentGroupID);
    QString groupName = QString::fromStdString(group.name);

    SPDLOG_DEBUG("UpdateTreeGroupItem {}, GroupId : {}", group.name, group.groupID);

    TreeItemModel* treeItemModel = m_treeItemModelMap[groupId];

    if(treeItemModel->parentItem()->getUuid() != parentGroupId)
    {
        TreeItemModel* parentTreeItemModel = treeItemModel->parentItem();
        TreeItemModel* newParentTreeItemModel = m_treeItemModelMap[parentGroupId];

        QModelIndex parentIndex = getParentIndex(treeItemModel);

        SPDLOG_DEBUG("UpdateTreeGroupItem 1");

        beginRemoveRows(parentIndex, treeItemModel->row(), treeItemModel->row());
        parentTreeItemModel->unlinkChild(treeItemModel->row());
        endRemoveRows();

        SPDLOG_DEBUG("UpdateTreeGroupItem 2");
        QModelIndex newParentIndex = getIndex(newParentTreeItemModel);

        beginInsertRows(newParentIndex, newParentTreeItemModel->childCount(), newParentTreeItemModel->childCount());
        treeItemModel->setParentItem(newParentTreeItemModel);
        newParentTreeItemModel->appendChild(treeItemModel);
        endInsertRows();

        SPDLOG_DEBUG("UpdateTreeGroupItem 3");
    }
    else
    {
        treeItemModel->setName(groupName);
        invalidate();
    }
}

void ResourceTreeModel::AddTreeChannelItemToRoot(const Wisenet::Device::Device::Channel& channel)
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
                                                   channelNumber.toInt());

    QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);
    TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, m_resourceRootModel);

    beginInsertRows(deviceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
    m_resourceRootModel->appendChild(cameraModel);
    endInsertRows();

    m_treeItemModelMap.insert(channelId, cameraModel);
}

void ResourceTreeModel::AddTreeChannelItem(const Wisenet::Core::Group& group)
{
    QString groupID = QString::fromStdString(group.groupID);

    TreeItemModel* groupModel = m_treeItemModelMap[groupID];

    for(auto& kvp : group.mapping)
    {
        QString groupName = QString::fromUtf8(group.name.c_str());
        QString channelId = QString::fromStdString(kvp.first);

        SPDLOG_DEBUG("AddTreeChannelItem {}, channel : {}", group.name, kvp.first);

        QStringList deviceChannelPair = channelId.split('_');

        if(deviceChannelPair.count() != 2)
            continue;

        QString deviceID = deviceChannelPair.at(0);
        QString channelNumber = deviceChannelPair.at(1);

        Wisenet::Device::Device device;
        Wisenet::Device::DeviceStatus deviceStatus;
        Wisenet::Device::ChannelStatus channelStatus;
        QCoreServiceManager::Instance().DB()->FindDevice(deviceID, device);
        QCoreServiceManager::Instance().DB()->FindDeviceStatus(deviceID, deviceStatus);
        QCoreServiceManager::Instance().DB()->FindChannelStatus(deviceID, channelNumber, channelStatus);

        BaseTreeItemInfo* cameraInfo = newTreeItemInfo(channelId,
                                                       ItemType::Camera,
                                                       channelStatus.isGood()? ItemStatus::Normal : ItemStatus::Abnormal,
                                                       QString::fromUtf8(device.name.c_str()),
                                                       QString::fromUtf8(device.modelName.c_str()),
                                                       QString::fromUtf8(device.deviceName.c_str()),
                                                       QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                       channelNumber.toInt());

        TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, groupModel);

        QModelIndex groupIndex = getIndex(groupModel);
        beginInsertRows(groupIndex, groupModel->childCount(), groupModel->childCount());
        groupModel->appendChild(cameraModel);
        endInsertRows();

        m_treeItemModelMap.insert(channelId, cameraModel);
    }
}

void ResourceTreeModel::UpdateTreeChannelItem(const Wisenet::Device::Device::Channel& channel)
{
    QString deviceID = QString::fromStdString(channel.deviceID);
    QString channelNumber = QString::fromStdString(channel.channelID);

    QString channelId = deviceID + "_" + channelNumber;
    TreeItemModel* cameraModel = m_treeItemModelMap[channelId];

    QString name = QString::fromUtf8(channel.name.c_str());

    cameraModel->setName(name);
    invalidate();
}

void ResourceTreeModel::RemoveTreeChannelItem(const QString& channelId)
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

void ResourceTreeModel::AddTreeDeviceItem(const Wisenet::Device::Device& device,
                                          const Wisenet::Device::DeviceStatus& deviceStatus)
{
    std::map<std::string, Wisenet::uuid_string> channelGroupMappings = QCoreServiceManager::Instance().DB()->GetChannelGroupMappings();

    for (auto& item : device.channels) {

        if(item.second.use == false)
            continue;

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
                                                        channelNumber);

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

void ResourceTreeModel::AddTreeWebpageItem(const Wisenet::Core::Webpage& webpage)
{
    std::map<std::string, Wisenet::uuid_string> channelGroupMappings = QCoreServiceManager::Instance().DB()->GetChannelGroupMappings();

    BaseTreeItemInfo* webpageInfo = newTreeItemInfo(QString::fromStdString(webpage.webpageID), ItemType::WebPage, ItemStatus::Normal, QString::fromUtf8(webpage.name.c_str()));

    if(channelGroupMappings.find(webpage.webpageID) != channelGroupMappings.end())
    {
        Wisenet::uuid_string stdGroupId = channelGroupMappings[webpage.webpageID];
        QString groupId = QString::fromStdString(stdGroupId);

        if(m_treeItemModelMap.contains(groupId))
        {
            TreeItemModel* parentModel = m_treeItemModelMap[groupId];
            QModelIndex parentIndex = getIndex(parentModel);
            TreeItemModel* channelModel = new TreeItemModel(webpageInfo, parentModel);

            beginInsertRows(parentIndex, parentModel->childCount(), parentModel->childCount());
            parentModel->appendChild(channelModel);
            endInsertRows();

            m_treeItemModelMap.insert(channelModel->getUuid(), channelModel);
        }
    }
    else
    {
        QModelIndex resourceRootIndex = index(m_resourceRootModel->row(), 0);
        TreeItemModel * channelModel = new TreeItemModel(webpageInfo, m_resourceRootModel);

        beginInsertRows(resourceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
        m_resourceRootModel->appendChild(channelModel);
        endInsertRows();

        m_treeItemModelMap.insert(channelModel->getUuid(), channelModel);
    }
}

void ResourceTreeModel::UpdateTreeWebpageItem(const Wisenet::Core::Webpage& webpage)
{
    QString uuid = QString::fromStdString(webpage.webpageID);
    QString webPageName = QString::fromUtf8(webpage.name.c_str());

    TreeItemModel* webpageModel = m_treeItemModelMap[uuid];

    webpageModel->setName(webPageName);
    invalidate();
}

void ResourceTreeModel::AddLocalDirectoryItem(const QString& folderUrl)
{
    QStringList urls = folderUrl.split('/');

    QString folderName = urls.last();

    if(!m_treeItemModelMap.contains(folderName))
    {
        BaseTreeItemInfo* localfileInfo = newTreeItemInfo(folderUrl, ItemType::LocalDirectory, ItemStatus::Normal, folderName);

        TreeItemModel* localDirectoryModel = new TreeItemModel(localfileInfo, m_localFileRootModel);

        QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
        beginInsertRows(localfileRootIndex, m_localFileRootModel->childCount(), m_localFileRootModel->childCount());
        m_localFileRootModel->appendChild(localDirectoryModel);
        endInsertRows();

        m_treeItemModelMap.insert(folderUrl, localDirectoryModel);
    }
}

void ResourceTreeModel::AddLocalFileItem(TreeItemModel* parentModel, const QString& fileUrl)
{
    SPDLOG_DEBUG("ResourceTreeModel::AddLocalFileItem : {}", fileUrl.toUtf8().constData());

    ItemStatus status = ItemStatus::Normal;

    // Digital signiture result map에 있는 지 확인
    QMap<QString, int> verifiedInfo = QCoreServiceManager::Instance().Signature()->getVerifyResultMap();
    if(verifiedInfo.contains(fileUrl))
    {
        switch(verifiedInfo[fileUrl])
        {
        case 4:
            status = ItemStatus::Authenticated;
            break;
        case 5:
            status = ItemStatus::Manipulated;
            break;
        case 6:
            status = ItemStatus::NotSigned;
            break;
        case 7:
            status = ItemStatus::Verifying;
            break;
        }
    }

    QUrl url = QUrl(fileUrl);
    QString fileName = url.toLocalFile();
    QStringList token = fileName.split('/');
    fileName = token.last();

    if(!m_treeItemModelMap.contains(fileUrl))
    {
        BaseTreeItemInfo* localfileInfo = newTreeItemInfo(fileUrl, ItemType::LocalFile, status, fileName);

        TreeItemModel* localFileModel = new TreeItemModel(localfileInfo, parentModel);

        QModelIndex localfileRootIndex = getIndex(parentModel);
        beginInsertRows(localfileRootIndex, parentModel->childCount(), parentModel->childCount());
        parentModel->appendChild(localFileModel);
        endInsertRows();

        m_treeItemModelMap.insert(fileUrl, localFileModel);
    }
}

void ResourceTreeModel::RemoveLocalFileItem(const QString& fileUrl)
{
    SPDLOG_DEBUG("ResourceTreeModel::RemoveLocalFileItem : {}", fileUrl.toUtf8().constData());
    if(!m_treeItemModelMap.contains(fileUrl))
        return;

    TreeItemModel* treeModel = m_treeItemModelMap.take(fileUrl);

    // 2023.01.02. coverity
    if (treeModel == NULL || treeModel == nullptr) {
        return;
    }

    QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
    int row = treeModel->row();

    if (treeModel != NULL || treeModel != nullptr) {
        if (treeModel->row() < 1) {
            row = 1;
        }
        beginRemoveRows(localfileRootIndex, row, row);
        m_localFileRootModel->removeChild(row);
        endRemoveRows();
    }
}

void ResourceTreeModel::reloadFileItems()
{
    // map에서 로컬파일리스트 초기화
    SPDLOG_DEBUG("ResourceTreeModel::reloadFileItems(), current count={}", m_localFileRootModel->childCount());

    QMap<QString, QStringList> dirFilePair;
    QSet<QString> urlSet;
    GetLocalDirs(dirFilePair, urlSet);

    if (m_localFileRootModel->childCount() > 0)
    {
        auto rootChildren = m_localFileRootModel->children();

        for (auto child : rootChildren)
        {
            // Folder 삭제 체크
            if(child->getItemType() == ItemType::LocalDirectory)
            {
                // 폴더 아래 전부 다 삭제
                if(!urlSet.contains(child->getUuid()))
                {
                    for(auto subDirFile : child->children())
                    {
                        QCoreServiceManager::Instance().Signature()->removeVerifyResult(subDirFile->getUuid());
                    }

                    SPDLOG_DEBUG("ResourceTreeModel::remove child:: {}", child->getUuid().toStdString());
                    QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
                    TreeItemModel* fileModel = m_treeItemModelMap.take(child->getUuid());

                    beginRemoveRows(localfileRootIndex, fileModel->row(), fileModel->row());
                    m_localFileRootModel->removeChild(fileModel->row());
                    endRemoveRows();
                }
                // 폴더 하위 파일 있는 지 확인하여 삭제
                else
                {
                    for(auto subDirFile : child->children())
                    {
                        if(!urlSet.contains(subDirFile->getUuid()))
                        {
                            QCoreServiceManager::Instance().Signature()->removeVerifyResult(subDirFile->getUuid());
                            QModelIndex childIndex = getIndex(child);

                            TreeItemModel* fileModel = m_treeItemModelMap.take(subDirFile->getUuid());

                            beginRemoveRows(childIndex, fileModel->row(), fileModel->row());
                            child->removeChild(fileModel->row());
                            endRemoveRows();
                        }
                    }
                }
            }
            // Root에 있는 file 삭제 체크
            else if(child->getItemType() == ItemType::LocalFile && !urlSet.contains(child->getUuid()))
            {
                QCoreServiceManager::Instance().Signature()->removeVerifyResult(child->getUuid());

                SPDLOG_DEBUG("ResourceTreeModel::remove child:: {}", child->getUuid().toStdString());
                QModelIndex localfileRootIndex = getIndex(m_localFileRootModel);
                TreeItemModel* fileModel = m_treeItemModelMap.take(child->getUuid());

                beginRemoveRows(localfileRootIndex, fileModel->row(), fileModel->row());
                m_localFileRootModel->removeChild(fileModel->row());
                endRemoveRows();
            }
        }
    }


    // 새로 파일리스트를 불러와서 구성한다.
    for (auto& dirPath : dirFilePair.keys())
    {
        QStringList fileList = dirFilePair[dirPath];

        // Folder가 없으면 추가
        if(!dirPath.endsWith("/.") && !m_treeItemModelMap.contains(dirPath))
        {
            AddLocalDirectoryItem(dirPath);
        }

        // File이 없으면 추가
        for(auto& filePath : fileList)
        {
            if(!m_treeItemModelMap.contains(filePath))
            {
                if(dirPath.endsWith("/."))
                {
                    AddLocalFileItem(m_localFileRootModel, filePath);
                }
                else
                {
                    TreeItemModel* parentModel = m_treeItemModelMap[dirPath];
                    AddLocalFileItem(parentModel, filePath);
                }
            }
        }
    }
}

void ResourceTreeModel::onLocalRecordingPathChanged()
{
    SPDLOG_DEBUG("ResourceTreeModel::onLocalRecordingPathChanged()");
    reloadFileItems();
}

void ResourceTreeModel::GetLocalDirs(QMap<QString, QStringList>& dirFilePair, QSet<QString>& urlSet)
{
    QString path = QCoreServiceManager::Instance().Settings()->localRecordingPath();
    SPDLOG_DEBUG("ResourceTreeModel::GetLocalDirs() localRecordingPath {}", path.toStdString());
    m_watcher.removePaths(m_watcher.directories());
    m_watcher.addPath(path);

    // QDir::Drives|QDir::Dirs|QDir::NoDotDot 옵션으로 하고 싶었으나 QDir::Drives가 동작 안함.
    // 미디어 파일 폴더를 Drive root (ex F:\) 로 했을 때 directoryIt(path, QDir::Dirs)에 포함되지 않아 아래 로직에서는 NotDot 옵션을 쓰고 여기에서 미리 추가해 줌.
    // 마지막에 "/." 을 추가한 이유는 reloadFileItems()에서 /.를 기준으로 current dir을 판단함.
    // Media file folder
    QString url = FilePathConveter::getInstance()->getUrlNameByFileName(path).toString() + "/.";
    urlSet.insert(url);
    GetLocalFiles(url, path, dirFilePair, urlSet);

    // Media file folder의 sub folders
    //QDirIterator directoryIt(path, QDir::Drives|QDir::Dirs|QDir::NoDotDot); // QDir::Drives 안됨.
    QDirIterator directoryIt(path, QDir::Dirs|QDir::NoDotAndDotDot);

    while (directoryIt.hasNext()) {
        directoryIt.next();
        QString dirUrl = QUrl::fromLocalFile(directoryIt.filePath()).toString();

#ifdef Q_OS_MACOS
    if(dirUrl.right(3) == "app"){
        SPDLOG_DEBUG("ResourceTreeModel::GetLocalDirs() exclude app file = {}", dirUrl.toStdString());
        continue;
    }
#endif

        QString dirPath = FilePathConveter::getInstance()->getPathByUrl(dirUrl);
        SPDLOG_DEBUG("ResourceTreeModel::GetLocalDirs() Directory path : {}", dirPath.toUtf8().constData());
        //qDebug() << "ResourceTreeModel::GetLocalDirs() Directory url " << dirUrl;

        m_watcher.addPath(dirPath);
        GetLocalFiles(dirUrl, dirPath, dirFilePair, urlSet);
    }

    qDebug() << "ResourceTreeModel::GetLocalDirs() watcher dirs" << m_watcher.directories();
    qDebug() << "ResourceTreeModel::GetLocalDirs() dirFilePair" << dirFilePair;
    qDebug() << "ResourceTreeModel::GetLocalDirs() ret" << urlSet;
}

void ResourceTreeModel::GetLocalFiles(const QString& dirUrl, const QString& dirPath, QMap<QString, QStringList>& dirFilePair, QSet<QString>& urlSet)
{
    QStringList fileList;
    QDirIterator subDirFileIt(dirPath, {"*.mp4", "*.mkv", "*.avi", "*.wnm", "*.sec", "*.png", "*.jpg", "*.jpeg", "*.bmp"}, QDir::Files);

    while (subDirFileIt.hasNext()) {
        subDirFileIt.next();

        QString subFileUrl = QUrl::fromLocalFile(subDirFileIt.filePath()).toString();
        //qDebug() << "ResourceTreeModel::GetLocalFiles() media file url" << subFileUrl;
        fileList.append(subFileUrl);
        urlSet.insert(subFileUrl);
    }

    urlSet.insert(dirUrl);
    dirFilePair.insert(dirUrl, fileList);
}

bool ResourceTreeModel::checkAcceptable(QString parentUuid, DragItemListModel* model)
{
    bool includeDraggable = false;

    auto groupMap = QCoreServiceManager::Instance().DB()->GetGroups();

    if(groupMap.find(parentUuid.toStdString()) == groupMap.end())
    {
        return false;
    }

    foreach(auto& item, model->itemList()){
        std::string id = item->uuid().toStdString();

        if(item->itemType() == ItemType::Group || item->itemType() == ItemType::Camera || item->itemType() == ItemType::WebPage)
            includeDraggable = true;

        if(groupMap.find(id) == groupMap.end())
            continue;

        // child가 parent가 되는지 확인
        bool ret = checkToCircle(item->uuid(), parentUuid);

        if(ret == true)
        {
            //qDebug() << "checkAcceptable() false";
            return false;
        }
    }

    if(!includeDraggable)
        return false;

    //qDebug() << "checkAcceptable() true";
    return true;
}

bool ResourceTreeModel::checkToCircle(QString uuid, QString moveParent)
{
    auto groupMap = QCoreServiceManager::Instance().DB()->GetGroups();
    QUuid emptyUuid;

    if(groupMap[moveParent.toStdString()].groupID == uuid.toStdString())
    {
        return true;
    }
    else
    {
        QString parent = QString::fromStdString(groupMap[moveParent.toStdString()].parentGroupID);

        if(parent == emptyUuid.toString(QUuid::WithoutBraces))
            return false;

        return checkToCircle(uuid, parent);
    }
}

void ResourceTreeModel::moveGroup(QString groupId, QStringList itemList)
{
    qDebug() << "ResourceTreeModel::moveGroup : " << itemList;

    if(itemList.count() == 0)
    {
        SPDLOG_DEBUG("ResourceTreeModel::moveGroup : itemUuidList is empty");
        return;
    }

    ChangeParentGroup(groupId, itemList);
}

void ResourceTreeModel::dropSource(const QModelIndex& itemIndex, QString parentUuid, DragItemListModel* model)
{
    SPDLOG_DEBUG("ResourceTreeModel::dropSource()");

    TreeItemModel *newParentItem = static_cast<TreeItemModel*>(itemIndex.internalPointer());

    if(newParentItem == nullptr)
    {
        SPDLOG_DEBUG("ResourceTreeModel::dropSource() : cannot find parent : {}", parentUuid.toStdString());
        return;
    }

    QStringList itemUuidList;
    for(auto& item : model->itemList())
    {
        SPDLOG_DEBUG("ResourceTreeModel::dropSource() channelId : {}, type : {}", item->uuid().toStdString(), item->itemType());

        if(item->itemType() == ItemType::LocalFile)
            continue;

        itemUuidList.append(item->uuid());
    }

    ChangeParentGroup(parentUuid, itemUuidList);
}

bool ResourceTreeModel::checkAncestor(QString groupId, QSet<QString> sourceSet)
{
    if(sourceSet.count() == 0)
        return false;

    auto stdGroupMap = QCoreServiceManager::Instance().DB()->GetGroups();

    QString parentId = QString::fromStdString(stdGroupMap[groupId.toStdString()].parentGroupID);

    QUuid emptyUuid;
    if(parentId == emptyUuid.toString(QUuid::WithoutBraces))
        return false;

    if(sourceSet.contains(parentId))
        return true;
    else
        return checkAncestor(parentId, sourceSet);
}

void ResourceTreeModel::deleteItems(QStringList groups, QStringList cameras, QStringList webpages, QStringList localFiles, bool removeChannels)
{
    QStringList deleteGroupsBottomUpOrder;
    QSet<QString> groupSet;
    QSet<QString> channelSet;

    // group을 순환하면서 group은 child 순서대로 역순, channel set에 넣음.
    for(auto& groupId : groups)
    {
        composeChildrenList(groupId, deleteGroupsBottomUpOrder, groupSet, channelSet);

        if(!groupSet.contains(groupId))
            deleteGroupsBottomUpOrder.append(groupId);
    }

    // remove or ungroup
    if(removeChannels)
    {
        // Remove
        std::vector<Wisenet::Device::Device::Channel> camerasToRemove;

        for(auto& cameraId : cameras)
        {
            Wisenet::Device::Device::Channel channel;
            auto items = cameraId.split('_');
            if(items.size() ==2 && QCoreServiceManager::Instance().DB()->FindChannel(items.at(0), items.at(1), channel)){
                channel.use = false;
                camerasToRemove.push_back(channel);
            }
        }

        Request_SaveChannels(camerasToRemove);

        Request_RemoveWebpage(webpages);
    }
    else
    {
        // Ungroup
        // camera, web page
        QMap<QString, QList<QString>> groupCameraMapping;

        for(auto& cameraId : cameras)
        {
            if(channelSet.contains(cameraId))
                continue;

            TreeItemModel* cameraModel = m_treeItemModelMap[cameraId];
            QString groupId = cameraModel->parentItem()->getUuid();

            groupCameraMapping[groupId].append(cameraId);
        }

        for(auto& webpageId : webpages)
        {
            if(channelSet.contains(webpageId))
                continue;

            TreeItemModel* webpageModel = m_treeItemModelMap[webpageId];
            QString groupId = webpageModel->parentItem()->getUuid();

            groupCameraMapping[groupId].append(webpageId);
        }

        // Mapping 삭제
        foreach(auto& key, groupCameraMapping.keys())
        {
            Request_RemoveGroupMapping(key, groupCameraMapping[key]);
        }
    }

    foreach(auto& groupId, deleteGroupsBottomUpOrder)
    {
        Request_RemoveGroup(groupId);
    }

    // Local file 지우기
    //    foreach(auto& localfile, localFiles)
    //    {
    //        QCoreServiceManager::Instance().RemoveLocalFile(localfile.toUtf8().constData());
    //    }
}

void ResourceTreeModel::newGroupWithChannels(QString groupName, QStringList cameras, QStringList webpages)
{
    Wisenet::Core::Group group;

    QUuid emptyUuid;
    QString emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces);
    group.parentGroupID = emptyUuidString.toStdString();
    group.groupID = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    group.name = groupName.toUtf8().constData();

    std::map<std::string, std::vector<Wisenet::Core::Group::MappingItem>> groupChannelMapping;

    foreach(QString channelId, cameras){

        if(!m_treeItemModelMap.contains(channelId))
            continue;

        TreeItemModel* channelModel = m_treeItemModelMap[channelId];
        QString groupId = channelModel->parentItem()->getUuid();

        Wisenet::Core::Group::MappingItem item;
        item.id = channelId.toStdString();
        item.mappingType = Wisenet::Core::Group::MappingType::Camera;

        groupChannelMapping[groupId.toStdString()].push_back(item);
    }

    foreach(QString channelId, webpages){

        if(!m_treeItemModelMap.contains(channelId))
            continue;

        TreeItemModel* channelModel = m_treeItemModelMap[channelId];
        QString groupId = channelModel->parentItem()->getUuid();

        Wisenet::Core::Group::MappingItem item;
        item.id = channelId.toStdString();
        item.mappingType = Wisenet::Core::Group::MappingType::WebPage;

        groupChannelMapping[groupId.toStdString()].push_back(item);
    }

    SPDLOG_DEBUG("ResourceTreeModel::newGroupWithChannels");
    auto addGroupWithUpdateMappingRequest = std::make_shared<Wisenet::Core::AddGroupWithUpdateMappingRequest>();
    addGroupWithUpdateMappingRequest->group = group;
    addGroupWithUpdateMappingRequest->groupChannelMapping = groupChannelMapping;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::AddGroupWithUpdateMapping,
                this, addGroupWithUpdateMappingRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::newGroupWithChannels success");
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::newGroupWithChannels fail");
    });
}

void ResourceTreeModel::composeChildrenList(QString groupId, QStringList& groupList, QSet<QString>& groupSet, QSet<QString>& itemSet)
{
    if(m_treeItemModelMap.contains(groupId))
    {
        TreeItemModel* groupModel = m_treeItemModelMap[groupId];

        QList<TreeItemModel*> itemList = groupModel->children();

        for(auto& item : itemList)
        {
            QString itemId = item->getUuid();

            if(item->getItemType() == ItemType::Group)
            {
                composeChildrenList(itemId, groupList, groupSet, itemSet);

                if(!groupSet.contains(itemId))
                {
                    groupSet.insert(itemId);
                    groupList.append(itemId);
                }
            }
            else
            {
                if(!itemSet.contains(itemId))
                    itemSet.insert(itemId);
            }
        }
    }
}

void ResourceTreeModel::changeDisplayName(QString uuid, int itemType, QString name)
{
    SPDLOG_DEBUG("ResourceTreeModel::changeDisplayName uuid: {}, itemType: {}, name: {}", uuid.toStdString(), itemType, name.toUtf8().constData());
    ItemType enumItemType = (ItemType) itemType;

    switch(enumItemType)
    {
    case ItemType::Group:
    {
        Wisenet::Core::Group group;
        QCoreServiceManager::Instance().DB()->FindGroup(uuid.toStdString(), group);

        Wisenet::Core::Group localGroup = group;
        localGroup.name = name.toStdString();

        Request_SaveGroup(localGroup);
        break;
    }
    case ItemType::Camera:
    {
        Wisenet::Device::Device::Channel channel;
        QStringList ids = uuid.split('_');
        QCoreServiceManager::Instance().DB()->FindChannel(ids[0], ids[1], channel);

        Wisenet::Device::Device::Channel localChannel = channel;

        localChannel.name = name.toStdString();

        std::vector<Wisenet::Device::Device::Channel> channels;
        channels.push_back(localChannel);

        Request_SaveChannels(channels);
        break;
    }
    case ItemType::WebPage:
    {
        Wisenet::Core::Webpage webpage;
        QCoreServiceManager::Instance().DB()->FindWebpage(uuid.toStdString(), webpage);

        Wisenet::Core::Webpage localWebpage = webpage;
        localWebpage.name = name.toUtf8().constData();

        Request_SaveWebage(localWebpage);
        break;
    }
    default:
        break;
    }
}

void ResourceTreeModel::addGroup(const QModelIndex& parentIndex, QString name)
{
    TreeItemModel* parentModel = static_cast<TreeItemModel*>(parentIndex.internalPointer());

    Wisenet::Core::Group group;

    if(parentModel->getItemType() == ItemType::ResourceRoot)
    {
        QUuid emptyUuid;
        group.parentGroupID = emptyUuid.toString(QUuid::WithoutBraces).toStdString();
    }
    else
    {
        group.parentGroupID = parentModel->getUuid().toStdString();
    }

    group.groupID = QUuid::createUuid().toString(QUuid::WithoutBraces).toStdString();
    group.name = name.toStdString();

    Request_SaveGroup(group);
}

void ResourceTreeModel::addLocalFile(const QString fileUrl)
{
    SPDLOG_DEBUG("ResourceTreeModel::addLocalFile {}", fileUrl.toUtf8().constData());

    // TODO :: QCoreServiceManager::Instance().AddLocalFile(fileUrl.toUtf8().constData());
}

void ResourceTreeModel::removeLocalFile(const QString fileUrl)
{
    SPDLOG_DEBUG("ResourceTreeModel::removeLocalFile {}", fileUrl.toUtf8().constData());

    // TODO :: QCoreServiceManager::Instance().RemoveLocalFile(fileUrl.toUtf8().constData());
}

void ResourceTreeModel::openLocalDirPath(const QString &dirPath)
{
    SPDLOG_DEBUG("ResourceTreeModel::openLocalDirPath - dirPath={}", dirPath.toUtf8().constData());
    QString dir = dirPath.left(dirPath.lastIndexOf('/'));

    QString urlString = dir.replace("file://", "");

    if(urlString[0] != '/') {
        urlString = "//" + urlString;
    }

    QUrl url = QUrl::fromLocalFile(urlString);

    SPDLOG_DEBUG("ResourceTreeModel::openLocalDirPath {}, {}", dir.toUtf8().constData(), urlString.toUtf8().constData());
    QDesktopServices::openUrl(url);
}

void ResourceTreeModel::onEndExport(const QString fileDir)
{
    // FileWriteManager에서 올려주는 fileDir이 native seperator 기준이라서 local path를 변환해서 비교
    auto localDir = QCoreServiceManager::Instance().Settings()->localRecordingPath();
    auto nativeLocalDir = QDir::toNativeSeparators(localDir);
    SPDLOG_DEBUG("ResourceTreeModel::onEndExport(), reload file items, dir={}, localPath={}", fileDir.toUtf8().constData(), nativeLocalDir.toUtf8().constData());
    if (fileDir.isEmpty() || fileDir == nativeLocalDir) {
        SPDLOG_DEBUG("ResourceTreeModel::onEndExport(), start to scan directory");
        reloadFileItems();
    }
    else {
        SPDLOG_DEBUG("ResourceTreeModel::onEndExport(), it's different directory, do not scan directory");
    }
}

void ResourceTreeModel::Request_SaveChannels(std::vector<Wisenet::Device::Device::Channel>& channels)
{
    auto request = std::make_shared<Wisenet::Core::SaveChannelsRequest>();

    request->channels = channels;

    if(request->channels.size() > 0){
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::SaveChannels,
                    this, request,
                    [](const QCoreServiceReplyPtr& reply)
        {
            auto response = reply->responseDataPtr;
            if(response->isSuccess())
                SPDLOG_DEBUG("ResourceTreeModel::Request_SaveChannels success");
            else
                SPDLOG_DEBUG("ResourceTreeModel::Request_SaveChannels fail");
        });
    }
}

void ResourceTreeModel::Request_SaveGroup(Wisenet::Core::Group& group)
{
    qDebug() << "ResourceTreeModel::Request_UpdateGroup";
    auto saveGroupRequest = std::make_shared<Wisenet::Core::SaveGroupRequest>();
    saveGroupRequest->group = group;
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveGroup,
                this, saveGroupRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_UpdateGroup success");
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_UpdateGroup fail");
    });
}

void ResourceTreeModel::Request_RemoveGroup(const QString& uuid)
{
    SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroup {}", uuid.toStdString());
    auto removeGroupsRequest = std::make_shared<Wisenet::Core::RemoveGroupsRequest>();
    removeGroupsRequest->groupIDs.push_back(uuid.toStdString());

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveGroups,
                this, removeGroupsRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroup success {}", uuid.toStdString());
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroup fail {}", uuid.toStdString());
    });
}

void ResourceTreeModel::Request_AddGroupMapping(QString groupId, QList<QString> channelList)
{
    SPDLOG_DEBUG("ResourceTreeModel::Request_AddGroupMapping() {}", groupId.toStdString());
    auto addGroupMappingRequest = std::make_shared<Wisenet::Core::AddGroupMappingRequest>();
    addGroupMappingRequest->groupID = groupId.toStdString();

    foreach(QString channelId, channelList){

        Wisenet::Core::Group::MappingItem item;
        item.id = channelId.toStdString();

        if(m_treeItemModelMap[channelId]->getItemType() == ItemType::Camera)
            item.mappingType = Wisenet::Core::Group::MappingType::Camera;
        else if(m_treeItemModelMap[channelId]->getItemType() == ItemType::WebPage)
            item.mappingType = Wisenet::Core::Group::MappingType::WebPage;

        addGroupMappingRequest->channels.push_back(item);
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::AddGroupMapping,
                this, addGroupMappingRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_AddGroupMapping success {}", groupId.toStdString());
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_AddGroupMapping fail {}", groupId.toStdString());
    });
}

void ResourceTreeModel::Request_RemoveGroupMapping(QString groupId, QList<QString> channelList)
{
    SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroupMapping() {}", groupId.toStdString());
    auto removeGroupMappingRequest = std::make_shared<Wisenet::Core::RemoveGroupMappingRequest>();
    removeGroupMappingRequest->groupID = groupId.toStdString();

    foreach(QString channelId, channelList){
        removeGroupMappingRequest->channelIDs.push_back(channelId.toStdString());
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveGroupMapping,
                this, removeGroupMappingRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroupMapping success {}", groupId.toStdString());
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveGroupMapping fail {}", groupId.toStdString());
    });
}

void ResourceTreeModel::Request_UpdateGroupMapping(QString from, QString to, QList<QString> channelList)
{
    SPDLOG_DEBUG("ResourceTreeModel::Request_UpdateGroupMapping() from {} to {}", from.toStdString(), to.toStdString());
    auto updateGroupMappingRequest = std::make_shared<Wisenet::Core::UpdateGroupMappingRequest>();
    updateGroupMappingRequest->from = from.toStdString();
    updateGroupMappingRequest->to = to.toStdString();

    foreach(QString channelId, channelList){
        updateGroupMappingRequest->channelIDs.push_back(channelId.toStdString());
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::UpdateGroupMapping,
                this, updateGroupMappingRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_UpdateGroupMapping success from {} to {}", from.toStdString(), to.toStdString());
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_UpdateGroupMapping fail from {} to {}", from.toStdString(), to.toStdString());
    });
}

void ResourceTreeModel::Request_SaveWebage(Wisenet::Core::Webpage& webpage)
{
    auto saveWebpageRequest = std::make_shared<Wisenet::Core::SaveWebpageRequest>();

    saveWebpageRequest->webpage = webpage;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveWebpage,
                this, saveWebpageRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_SaveWebage success");
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_SaveWebage fail");
    });
}

void ResourceTreeModel::Request_RemoveWebpage(const QStringList& list)
{
    // remove
    SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveWebpage");
    auto removeWebpagesRequest = std::make_shared<Wisenet::Core::RemoveWebpagesRequest>();

    for(auto& id : list)
    {
        removeWebpagesRequest->webpageIDs.push_back(id.toStdString());
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveWebpages,
                this, removeWebpagesRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveWebpage success");
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Request_RemoveWebpage fail");
    });
}

void ResourceTreeModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    SPDLOG_DEBUG("ResourceTreeModel::Event_FullInfo() groups: {}, devices: {}, webpages: {}", coreEvent->groups.size(), coreEvent->devices.size(), coreEvent->webpages.size());

    for (auto& kv : coreEvent->groups) {
        auto& group = kv.second;
        AddTreeGroupItem(group);

        //AddTreeChannelItem(group);
    }

    for (auto& kv : coreEvent->devices) {
        auto& device = kv.second;
        auto& deviceStatus = coreEvent->devicesStatus[kv.first];
        AddTreeDeviceItem(device, deviceStatus);
    }

    for (auto& kv : coreEvent->webpages) {
        auto& webpage = kv.second;
        AddTreeWebpageItem(webpage);
    }

    // load local file list
    reloadFileItems();

    emit expandRoot();
}

void ResourceTreeModel::Event_SaveGroup(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_SaveGroup()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveGroupEvent>(event->eventDataPtr);

    QString qroupId = QString::fromStdString(coreEvent->group.groupID);

    if(this->m_treeItemModelMap.contains(qroupId))
        UpdateTreeGroupItem(coreEvent->group);
    else
    {
        AddTreeGroupItem(coreEvent->group);

        emit expandSelectedIndex();
    }
}

void ResourceTreeModel::Event_RemoveGroup(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroup()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupsEvent>(event->eventDataPtr);

    foreach(std::string groupId, coreEvent->groupIDs)
    {
        QString id = QString::fromStdString(groupId);

        if(m_treeItemModelMap.contains(id))
        {
            TreeItemModel* model = m_treeItemModelMap[id];
            TreeItemModel* parentModel = model->parentItem();

            QModelIndex parentIndex = getIndex(parentModel);

            beginRemoveRows(parentIndex, model->row(), model->row());
            parentModel->removeChild(model->row());
            endRemoveRows();
        }
    }
}

void ResourceTreeModel::Event_AddGroupMapping(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping()");
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
                SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() move {}", item.id);
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
                SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() add {}", item.id);

                // Camera로 판단.
                if(channelId.contains('_'))
                {
                    QStringList split = channelId.split('_');

                    if(split.count() != 2)
                    {
                        SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() continue 1");
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
                        SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() cannot find device : continue 2 {}", stdDeviceId);
                        continue;
                    }

                    if(!QCoreServiceManager::Instance().DB()->FindChannel(stdDeviceId, stdChannelNumber, channel))
                    {
                        SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() cannot find channel : continue 2 {}, {}", stdDeviceId, stdChannelNumber);
                        continue;
                    }

                    if(!QCoreServiceManager::Instance().DB()->FindChannelStatus(stdDeviceId, stdChannelNumber, channelStatus))
                    {
                        SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() cannot find channel status : continue 2 [}, {}", stdDeviceId, stdChannelNumber);
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
                                                                   channelNumber.toInt());

                    TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, groupItemModel);
                    QModelIndex groupIndex = getIndex(groupItemModel);

                    beginInsertRows(groupIndex, groupItemModel->childCount(), groupItemModel->childCount());
                    groupItemModel->appendChild(cameraModel);
                    endInsertRows();

                    m_treeItemModelMap.insert(channelId, cameraModel);

                    SPDLOG_DEBUG("ResourceTreeModel::Event_AddGroupMapping() END");
                }

            }
        }
    }
}

void ResourceTreeModel::Event_RemoveGroupMapping(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroupMapping()");

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

            std::string stdChannelName = itemModel->getName().toUtf8().constData();

            if(rowNumber < 0)
            {
                SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroupMapping() beginRemove continue {} {} {}", stdChannelName, channelId, rowNumber);
                continue;
            }

            SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroupMapping() beginRemove {} {} {}", stdChannelName, channelId, rowNumber);
            beginRemoveRows(parentIndex, rowNumber, rowNumber);
            bool unlinkResult = groupItem->unlinkChild(rowNumber);
            SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroupMapping() unlinkResult {} {} {} {}", stdChannelName, channelId, rowNumber, unlinkResult);
            endRemoveRows();

            SPDLOG_DEBUG("ResourceTreeModel::Event_RemoveGroupMapping() beginInsert {} {} {}", stdChannelName, channelId, m_resourceRootModel->childCount());
            QModelIndex deviceRootIndex = getIndex(m_resourceRootModel);
            beginInsertRows(deviceRootIndex, m_resourceRootModel->childCount(), m_resourceRootModel->childCount());
            m_resourceRootModel->appendChild(itemModel);
            itemModel->setParentItem(m_resourceRootModel);
            endInsertRows();
        }
    }
}

void ResourceTreeModel::Event_UpdateGroupMapping(QCoreServiceEventPtr event)
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

void ResourceTreeModel::Event_AddDevice(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);
    AddTreeDeviceItem(coreEvent->device, coreEvent->deviceStatus);
}

void ResourceTreeModel::Event_RemoveDevices(QCoreServiceEventPtr event)
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

void ResourceTreeModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);

    SPDLOG_DEBUG("ResourceTreeModel::Event_DeviceStatus deviceUuid {}", deviceStatusEvent->deviceID);

    if(m_treeItemModelMap.contains(deviceUuid))
    {
        TreeItemModel* deviceItemModel = m_treeItemModelMap[deviceUuid];
        int deviceStatus = deviceStatusEvent->deviceStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;
        deviceItemModel->getTreeItemInfo()->setItemStatus(deviceStatus);

        SPDLOG_DEBUG("ResourceTreeModel::Event_DeviceStatus deviceUuid [{}] name [{}] status [{}]", deviceStatusEvent->deviceID, deviceItemModel->getName().toUtf8().constData(), deviceStatus);
    }

    for (auto& channel : deviceStatusEvent->deviceStatus.channelsStatus)
    {
        QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int channelStatus = channel.second.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("ResourceTreeModel::Event_DeviceStatus channelUuid [{}] name [{}] status [{}]", channelUuid.toStdString(), channelItemModel->getName().toUtf8().constData(), channelStatus);
            channelItemModel->getTreeItemInfo()->setItemStatus(channelStatus);
        }
    }
}

void ResourceTreeModel::Event_ChannelStatus(QCoreServiceEventPtr event)
{
    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    for (auto& channel : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus channelStatus = channel.second;

        QString channelUuid = QString::fromStdString(channelStatus.deviceID) + "_" + QString::fromStdString(channelStatus.channelID);

        if(m_treeItemModelMap.contains(channelUuid)){
            TreeItemModel* channelItemModel = m_treeItemModelMap[channelUuid];
            int status = channelStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            SPDLOG_DEBUG("Event_DeviceStatus channelUuid [{}] name [{}] status [{}]", channelUuid.toStdString(),  channelItemModel->getName().toUtf8().constData(), status);
            channelItemModel->getTreeItemInfo()->setItemStatus(status);
        }
    }
}

void ResourceTreeModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    SPDLOG_DEBUG("ResourceTreeModel::Event_SaveChannels()");
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        QString id = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);

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
                AddTreeChannelItemToRoot(channel);
            }
        }
    }
}

void ResourceTreeModel::Event_SaveWebpage(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveWebpageEvent>(event->eventDataPtr);

    QString webPageId = QString::fromStdString(coreEvent->webpage.webpageID);

    if(m_treeItemModelMap.contains(webPageId))
        UpdateTreeWebpageItem(coreEvent->webpage);
    else
    {
        AddTreeWebpageItem(coreEvent->webpage);
        emit expandSelectedIndex();
    }
}

void ResourceTreeModel::Event_RemoveWebpages(QCoreServiceEventPtr event)
{
    auto removeWebpagesEvent = std::static_pointer_cast<Wisenet::Core::RemoveWebpagesEvent>(event->eventDataPtr);

    for (auto& uuid : removeWebpagesEvent->webpageIDs) {
        QString uuidToDelete = QString::fromStdString(uuid);

        TreeItemModel* item = m_treeItemModelMap.take(uuidToDelete);

        TreeItemModel* parentItem = item->parentItem();
        QModelIndex parentIndex = getIndex(parentItem);

        beginRemoveRows(parentIndex, item->row(), item->row());
        parentItem->removeChild(item->row());
        endRemoveRows();
    }
}

void ResourceTreeModel::Event_AddLocalFile(QCoreServiceEventPtr event)
{
    //auto addLocalFileEvent = std::static_pointer_cast<Wisenet::Core::AddLocalFileEvent>(event->eventDataPtr);
    //QString path = QString::fromUtf8(addLocalFileEvent->localfilePath.c_str());
    //AddLocalFileItem(path);
    reloadFileItems();
}

void ResourceTreeModel::Event_RemoveLocalFile(QCoreServiceEventPtr event)
{
    auto removeLocalFileEvent = std::static_pointer_cast<Wisenet::Core::RemoveLocalFileEvent>(event->eventDataPtr);

    QString path = QString::fromUtf8(removeLocalFileEvent->localfilePath.c_str());
    RemoveLocalFileItem(path);
}

void ResourceTreeModel::Event_UpdateDevices(QCoreServiceEventPtr event)
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

void ResourceTreeModel::Event_UpdateLocalFileIcon(QCoreServiceEventPtr event)
{
    auto updateVerifyResultEvent = std::static_pointer_cast<Wisenet::Core::UpdateVerifyResultEvent>(event->eventDataPtr);

    QString fileUuid = QString::fromStdString(updateVerifyResultEvent->fileUuid);
    int verifyResult = updateVerifyResultEvent->verifyResult;
    SPDLOG_DEBUG("ResourceTreeModel::Event_UpdateLocalFileIcon fileUuid ->", updateVerifyResultEvent->fileUuid);

    if (verifyResult > 0) {
        if(m_treeItemModelMap.contains(fileUuid)) {
            m_treeItemModelMap[fileUuid]->setStatus(verifyResult);
        }
        else
            SPDLOG_DEBUG("ResourceTreeModel::Event_UpdateLocalFileIcon - m_treeItemModelMap doesn't contains this file.");
    }
    else { // 검증 완수 못했을 경우 아이콘 원복
        SPDLOG_DEBUG("ResourceTreeModel::Event_UpdateLocalFileIcon - Restore the icon.");
        QCoreServiceManager::Instance().Signature()->removeVerifyResult(fileUuid);
        m_treeItemModelMap[fileUuid]->setStatus(1);
    }
}

void ResourceTreeModel::verifySignature(const QStringList wnmList)
{
    // result <- 서명 검증 성공 : 4, 서명 검증 실패 : 5, 서명 없음 : 6, publicKey 오픈 에러 : 0
    for(auto& fileUuid : wnmList) {
        int extensionIndex = fileUuid.lastIndexOf(".");
        QString extension = fileUuid.right(fileUuid.length() - extensionIndex - 1);
        if(extension.toLower()!="wnm")
            continue;
        else {
            if(m_treeItemModelMap.contains(fileUuid)) {
                m_treeItemModelMap[fileUuid]->setStatus(ItemStatus::Verifying);
            }
            QString filePath = FilePathConveter::getInstance()->getPathByUrl(fileUuid);
            QCoreServiceManager::Instance().Signature()->verify(filePath);
        }
    }
}

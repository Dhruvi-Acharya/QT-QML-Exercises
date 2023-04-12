#include "LayoutTreeModel.h"

LayoutTreeModel::LayoutTreeModel(QObject *parent)
    : BaseTreeModel(parent)
{
    qDebug() << "LayoutTreeModel()";
    m_roleNameMapping[TreeModelRoleName] = "display";

    SetupCategoryTree();

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &LayoutTreeModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

LayoutTreeModel::~LayoutTreeModel()
{
    qDebug() << "~LayoutTreeModel()";

    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }
}

void LayoutTreeModel::clear()
{
    qDebug() << "LayoutTreeModel::clear()";
    m_treeItemModelMap.clear();
    m_channelItemIdMapping.clear();
    m_sequenceTreeItemMap.clear();

    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }

    SetupCategoryTree();
}

void LayoutTreeModel::initializeTree()
{
    qDebug() << "LayoutTreeModel::initializeTree()";
    std::map<Wisenet::uuid_string, Wisenet::Core::Layout> layoutMap = QCoreServiceManager::Instance().DB()->GetLayouts();
    std::map<Wisenet::uuid_string, Wisenet::Core::SequenceLayout> sequenceLayoutMap = QCoreServiceManager::Instance().DB()->GetSequenceLayouts();

    for (auto& kv : layoutMap)
    {
        auto& layout = kv.second;
        AddTreeLayoutItem(layout);
    }

    for (auto& kv : sequenceLayoutMap)
    {
        auto& sequence = kv.second;
        AddTreeSequenceItem(sequence);
    }

    emit expandRoot();
}

void LayoutTreeModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("LayoutTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        // Triggered after login
        Event_FullInfo(event);
        break;
    case Wisenet::Core::SaveUserGroupEventType:
        break;
    case Wisenet::Core::RemoveUserGroupsEventType:
        break;
    case Wisenet::Core::SaveUserEventType:
        break;
    case Wisenet::Core::RemoveUsersEventType:
        break;
    case Wisenet::Core::SaveLayoutEventType:
        Event_SaveLayout(event);
        break;
    case Wisenet::Core::SaveLayoutsEventType:
        Event_SaveLayouts(event);
        break;
    case Wisenet::Core::RemoveLayoutsEventType:
        Event_RemoveLayout(event);
        break;
    case Wisenet::Core::SaveSequenceLayoutEventType:
        Event_SaveSequenceLayout(event);
        break;
    case Wisenet::Core::RemoveSequenceLayoutsEventType:
        Event_RemoveSequenceLayout(event);
        break;
    case Wisenet::Device::DeviceStatusEventType:
        Event_DeviceStatus(event);
        break;
    case Wisenet::Device::ChannelStatusEventType:
        Event_ChannelStatus(event);
        break;
    case Wisenet::Core::SaveChannelsEventType:
        Event_SaveChannels(event);
        break;
    case Wisenet::Core::SaveWebpageEventType:
        Event_SaveWebpage(event);
        break;
    case Wisenet::Core::RemoveWebpagesEventType:
        Event_RemoveWebpage(event);
        break;
    case Wisenet::Core::UpdateDevicesEventType:
        Event_UpdateDevices(event);
        break;
    default:
        break;
    }
}

void LayoutTreeModel::changeDisplayName(QString uuid, int itemType, QString name)
{
    qDebug() << "LayoutTreeModel::changeDisplayName uuid:" << uuid << ", itemType:" << itemType << ", name:" << name;
    ItemType enumItemType = (ItemType) itemType;

    switch(enumItemType)
    {
    case ItemType::Device:
    case ItemType::Camera:
        break;
    case ItemType::Layout:
    {
        Wisenet::Core::Layout localLayout;
        if(QCoreServiceManager::Instance().DB()->FindLayout(uuid.toStdString(), localLayout))
        {
            localLayout.name = name.toStdString();

            Request_UpdateLayout(localLayout);
            break;
        }
    }
    case ItemType::Sequence:
    {
        qDebug() << "LayoutTreeModel::changeDisplayName Sequence";
        Wisenet::Core::SequenceLayout sequenceLayout;
        if(QCoreServiceManager::Instance().DB()->FindSequenceLayout(uuid.toStdString(), sequenceLayout))
        {
            sequenceLayout.name = name.toStdString();

            Request_UpdateSequenceLayout(sequenceLayout);
            break;
        }
    }
    case ItemType::WebPage:
        break;
    default:
        break;
    }
}

void LayoutTreeModel::dropSource(const QString& parentUuid, DragItemListModel* source)
{
    qDebug() << "LayoutTreeModel::dropSource()";

    QList<DragItemModel*> itemList = source->itemList();

    if(m_treeItemModelMap.contains(parentUuid))
    {
        LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)m_treeItemModelMap[parentUuid]->getTreeItemInfo();

        QMap<QString, DragItemModel*> itemMap;
        for(auto& item : itemList)
        {
            if(!itemMap.contains(item->uuid()))
                itemMap.insert(item->uuid(), item);
        }

        // add
        foreach(auto& item, itemMap)
        {
            qDebug() << item->uuid() << " " << item->itemType();
            if(item->itemType() == (int)ItemType::Camera)
            {
                qDebug() << "LayoutTreeModel::dropSource Camera";

                QStringList parsed = item->uuid().split('_');

                layoutInfo->addCamera(parsed[0], parsed[1]);
            }
            else if(item->itemType() == (int)ItemType::WebPage)
            {
                qDebug() << "LayoutTreeModel::dropSource WebPage";
                layoutInfo->addWebpage(item->uuid());
            }
            else if(item->itemType() == (int)ItemType::LocalFile)
            {
                qDebug() << "LayoutTreeModel::dropSource LocalFile";
                layoutInfo->addLocalfile(item->uuid());
            }
        }

        layoutInfo->saveLayout();
    }
}

void LayoutTreeModel::deleteItems(QString items)
{
    QStringList list = items.split(":");

    // 검색이 편하게 set에 저장
    QSet<QString> tempSet(list.begin(), list.end());

    QList<QString> layoutList;
    QMap<QString, QList<QString>> layoutChannelMapping;

    QList<QString> sequenceList;

    for(auto& itemId : list)
    {
        if(m_treeItemModelMap.contains(itemId))
        {
            BaseTreeItemInfo* treeItemInfo = m_treeItemModelMap[itemId]->getTreeItemInfo();

            // Layout이면 삭제 대상 list에 넣음
            if(treeItemInfo->itemType() == ItemType::Layout)
            {
                // 공유레이아웃이면 못지움.
                if(QCoreServiceManager::Instance().DB()->IsSharedLayout(itemId))
                    continue;

                layoutList.append(itemId);
            }

            BaseTreeItemInfo* parentTreeItemInfo = m_treeItemModelMap[itemId]->parentItem()->getTreeItemInfo();

            // parent가 layout이면
            if(parentTreeItemInfo->itemType() == ItemType::Layout)
            {
                QString parentLayoutId = parentTreeItemInfo->uuid();

                // 공유레이아웃이면 못지움.
                if(QCoreServiceManager::Instance().DB()->IsSharedLayout(parentLayoutId))
                    continue;

                // parent가 삭제 대상 리스트에 없는 경우에 layout에서 제거 대상으로 함.
                // 해당 채널이 속한 Layout이 삭제되는 경우는 굳이 layout에서 제거할 필요없음.
                if(!tempSet.contains(parentLayoutId))
                {
                    if(!layoutChannelMapping.contains(parentLayoutId))
                    {
                        QList<QString> list;
                        layoutChannelMapping.insert(parentLayoutId, list);
                    }

                    layoutChannelMapping[parentLayoutId].append(itemId);
                }
            }
        }
        else if(m_sequenceTreeItemMap.contains(itemId))
        {
            BaseTreeItemInfo* treeItemInfo = m_sequenceTreeItemMap[itemId]->getTreeItemInfo();

            if(treeItemInfo->itemType() == ItemType::Sequence)
            {
                sequenceList.append(itemId);
            }
        }
    }

    // layout 삭제 request
    for(auto& layoutId : layoutList)
    {
        Request_RemoveLayout(layoutId);
    }

    // layout에서 제거
    for(auto& layoutId : layoutChannelMapping.keys())
    {
        LayoutTreeItemInfo * info = (LayoutTreeItemInfo*)m_treeItemModelMap[layoutId]->getTreeItemInfo();

        for(auto& itemid : layoutChannelMapping[layoutId])
        {
            info->removeItem(itemid);
        }

        info->saveLayout();
    }

    // sequence 삭제 request
    for(auto& sequenceId : sequenceList)
    {
        Request_RemoveSequenceLayout(sequenceId);
    }
}

void LayoutTreeModel::addLayoutItems(QString layoutId, QVariantList items)
{
    //qDebug() << "LayoutTreeModel::addLayoutItem" << layoutId;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::addLayoutItem not contains return " << layoutId;
        return;
    }

    TreeItemModel* layoutTreeItemModel = m_treeItemModelMap[layoutId];

    foreach(QVariant v, items)
    {
        if (v.canConvert<MediaLayoutItemViewModel*>()) {
            MediaLayoutItemViewModel* srcItem = v.value<MediaLayoutItemViewModel*>();

            QString itemId = srcItem->itemId();
            MediaLayoutItemViewModel::ItemType itemType = srcItem->itemType();

            if(!m_treeItemModelMap.contains(itemId))
            {
                if(itemType == MediaLayoutItemViewModel::ItemType::Camera)
                {
                    QString deviceId = srcItem->deviceId();
                    QString cameraId = srcItem->channelId();

                    AddCameraItem(layoutId, itemId, deviceId, cameraId);
                }
                else if(itemType == MediaLayoutItemViewModel::ItemType::Webpage)
                {
                    QString webpageId = srcItem->webPageId();

                    AddWebpageItem(layoutId, itemId, webpageId);
                }
                else if(itemType == MediaLayoutItemViewModel::ItemType::LocalResource || itemType == MediaLayoutItemViewModel::ItemType::Image)
                {
                    QString fileUrl = srcItem->localResource().toString();

                    AddLocalFileItem(layoutId, itemId, fileUrl);
                }
            }
        }
    }

    LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)layoutTreeItemModel->getTreeItemInfo();
    layoutTreeItemInfo->layoutViewModel()->syncAddedItems(items);
}

void LayoutTreeModel::removeLayoutItems(QString layoutId, QList<QString> itemIds)
{
    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::removeLayoutItem not contains return " << layoutId;
        return;
    }

    TreeItemModel* layoutTreeItemModel = m_treeItemModelMap[layoutId];

    foreach(QString itemId, itemIds)
    {
        if(m_treeItemModelMap.contains(itemId))
        {
            TreeItemModel* itemModel = m_treeItemModelMap.take(itemId);

            QModelIndex layoutIndex = getIndex(layoutTreeItemModel);

            if(ItemType::Camera == itemModel->getItemType())
            {
                DeviceTreeItemInfo* cameraInfo = (DeviceTreeItemInfo*) itemModel->getTreeItemInfo();

                m_channelItemIdMapping[cameraInfo->getChannelId()].remove(cameraInfo->itemId());

                // Delete
                beginRemoveRows(layoutIndex, itemModel->row(), itemModel->row());
                layoutTreeItemModel->removeChild(itemModel->row());
                endRemoveRows();
            }
            else
            {
                BaseTreeItemInfo* treeInfo = itemModel->getTreeItemInfo();

                m_channelItemIdMapping[treeInfo->uuid()].remove(treeInfo->itemId());

                // Delete
                beginRemoveRows(layoutIndex, itemModel->row(), itemModel->row());
                layoutTreeItemModel->removeChild(itemModel->row());
                endRemoveRows();
            }
        }
    }

    LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)layoutTreeItemModel->getTreeItemInfo();
    layoutTreeItemInfo->layoutViewModel()->syncRemovedItems(itemIds);
}


void LayoutTreeModel::updateLayoutItems(QString layoutId, QVariantList items)
{
    //qDebug() << "LayoutTreeModel::updateLayoutItems" << layoutId;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        //qDebug() << "LayoutTreeModel::updateLayoutItems not contains return " << layoutId;
        return;
    }

    TreeItemModel* layoutTreeItemModel = m_treeItemModelMap[layoutId];
    LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)layoutTreeItemModel->getTreeItemInfo();
    layoutTreeItemInfo->layoutViewModel()->syncUpdatedItems(items);
}

void LayoutTreeModel::updateLayoutProperty(QString layoutId, QVariant layoutModel)
{
    //qDebug() << "LayoutTreeModel::updateLayoutProperty" << layoutId;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::updateLayoutProperty not contains return " << layoutId;
        return;
    }

    TreeItemModel* layoutTreeItemModel = m_treeItemModelMap[layoutId];
    LayoutTreeItemInfo* layoutTreeItemInfo = (LayoutTreeItemInfo*)layoutTreeItemModel->getTreeItemInfo();

    MediaLayoutViewModel* mediaLayoutViewModel = layoutModel.value<MediaLayoutViewModel*>();
    layoutTreeItemInfo->layoutViewModel()->syncPropertyUpdatdLayout(mediaLayoutViewModel);
}

void LayoutTreeModel::tabClosedWithoutSave(QList<QString> itemIds)
{
    qDebug() << "LayoutTreeModel::tabClosedWithoutSave()";

    foreach(QString itemId, itemIds)
    {
        std::string stdLayoutId = itemId.toStdString();

        Wisenet::Core::Layout layout;
        if(QCoreServiceManager::Instance().DB()->FindLayout(stdLayoutId, layout))
        {
            UpdateTreeLayoutItem(layout);
        }
    }
}


void LayoutTreeModel::makeNewLayout()
{
    QString layoutNameParam = "Layout";
    Wisenet::Core::Layout layout;
    layout.name = layoutNameParam.toStdString();
    layout.layoutID = QUuid::createUuid().toString(QUuid::WithoutBraces).toUtf8().constData();

    Wisenet::Core::User user;
    QCoreServiceManager::Instance().DB()->GetLoginUser(user);

    qDebug() << "LayoutTreeModel::makeNewLayout - User" << QString::fromStdString(user.loginID) << (int)user.userType;
    layout.userName = user.loginID;

    auto request = std::make_shared<Wisenet::Core::SaveLayoutRequest>();
    request->layout = layout;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveLayout,
                this, request,
                [this, layoutNameParam](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "LayoutTreeModel::makeNewLayout - RequestToCoreService : add fail " << layoutNameParam;
        }
        else {
            qDebug() << "LayoutTreeModel::makeNewLayout - RequestToCoreService : add success " << layoutNameParam;
        }
    });
}


void LayoutTreeModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    qDebug() << "LayoutTreeModel::Event_FullInfo() Layout:" << coreEvent->layouts.size() << ", Sequence:" << coreEvent->sequenceLayouts.size();

    for (auto& kv : coreEvent->layouts)
    {
        auto& layout = kv.second;
        AddTreeLayoutItem(layout);
    }

    for (auto& kv : coreEvent->sequenceLayouts)
    {
        auto& sequence = kv.second;
        AddTreeSequenceItem(sequence);
    }

    emit expandRoot();
}


BaseTreeItemInfo* LayoutTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& itemId)
{
    BaseTreeItemInfo *t;

    if(type == ItemType::Layout || type == ItemType::SequenceItem)
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
        t->setItemId(itemId);
    }

    return t;
}

BaseTreeItemInfo* LayoutTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName,
                                                   const QString& deviceName, const QString& ipAddress, const int channelNumber, const QString& deviceId, const QString& itemId)
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
    t->setDeviceUuid(deviceId);
    t->setItemId(itemId);

    return t;
}

BaseTreeItemInfo* LayoutTreeModel::getLayoutItemInfo(const QString& layoutId)
{
    if(m_treeItemModelMap.contains(layoutId))
    {
        return m_treeItemModelMap[layoutId]->getTreeItemInfo();
    }

    return nullptr;
}

void LayoutTreeModel::SetupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    BaseTreeItemInfo* layoutInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::LayoutRoot, ItemStatus::Normal, tr("Layouts"));
    m_layoutRootItem = new TreeItemModel(layoutInfo, rootItem);

    BaseTreeItemInfo* sequencesInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::SequenceRoot, ItemStatus::Normal, tr("Sequences"));
    m_sequenceRootItem = new TreeItemModel(sequencesInfo, rootItem);

    //beginInsertRows(index(0,0), 0, 0);
    rootItem->appendChild(m_layoutRootItem);
    rootItem->appendChild(m_sequenceRootItem);
    //endInsertRows();

    endResetModel();
}

void LayoutTreeModel::AddTreeLayoutItem(Wisenet::Core::Layout& layout)
{
    qDebug() << "LayoutTreeModel::AddTreeLayoutItem()";
    QString layoutId = QString::fromStdString(layout.layoutID);

    if(m_treeItemModelMap.contains(layoutId))
        return;

    LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)newTreeItemInfo(layoutId, ItemType::Layout, ItemStatus::Normal, QString::fromUtf8(layout.name.c_str()));
    layoutInfo->setLayout(layout);
    TreeItemModel * layoutItem = new TreeItemModel(layoutInfo, m_layoutRootItem);

    QModelIndex layoutRootIndex = index(m_layoutRootItem->row(), 0);

    beginInsertRows(layoutRootIndex, m_layoutRootItem->childCount(), m_layoutRootItem->childCount());
    m_layoutRootItem->appendChild(layoutItem);
    endInsertRows();

    m_treeItemModelMap.insert(layoutItem->getUuid(), layoutItem);

    for(auto& layoutItem : layout.layoutItems)
    {
        if(layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Camera)
        {
            AddCameraItem(layout, layoutItem);
        }
        else if(layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Webpage)
        {
            AddWebageItem(layout, layoutItem);
        }
        else if(layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::LocalResource ||
                layoutItem.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Image)
        {
            AddLocalFileItem(layout, layoutItem);
        }
    }
}

void LayoutTreeModel::UpdateTreeLayoutItem(Wisenet::Core::Layout& layout)
{
    qDebug() << "LayoutTreeModel::UpdateTreeLayoutItem()";
    // Name update
    QString layoutId = QString::fromStdString(layout.layoutID);
    QString layoutName = QString::fromStdString(layout.name);

    TreeItemModel* layoutTreeItemModel = m_treeItemModelMap[layoutId];

    LayoutTreeItemInfo* treeItemInfo = (LayoutTreeItemInfo*)layoutTreeItemModel->getTreeItemInfo();

    if(treeItemInfo->displayName() != layoutName)
        treeItemInfo->setDisplayName(layoutName);

    treeItemInfo->setLayout(layout);

    QSet<QString> itemIdSet;

    // Added item
    for(auto& item : layout.layoutItems)
    {
        QString itemId = QString::fromStdString(item.itemID);

        qDebug() << "LayoutTreeModel::UpdateTreeLayoutItem()" << itemId;
        itemIdSet.insert(itemId);

        if(!m_treeItemModelMap.contains(itemId))
        {
            qDebug() << "LayoutTreeModel::UpdateTreeLayoutItem() ADD " << itemId;
            if(item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Camera)
                AddCameraItem(layout, item);
            else if(item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Webpage)
                AddWebageItem(layout, item);
            else if(item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::LocalResource || item.itemType == Wisenet::Core::LayoutItem::LayoutItemType::Image)
                AddLocalFileItem(layout, item);
        }
    }

    // Deleted Item
    QList<TreeItemModel*> children = layoutTreeItemModel->children();

    for(int i=children.count()-1; i >= 0; i--)
    {
        QString itemId;
        QString uuid;

        if(children[i]->getItemType() == ItemType::Camera)
        {
            DeviceTreeItemInfo* cameraInfo = (DeviceTreeItemInfo*) children[i]->getTreeItemInfo();

            itemId = cameraInfo->itemId();
            uuid = cameraInfo->uuid();
        }
        else if(children[i]->getItemType() == ItemType::WebPage || children[i]->getItemType() == ItemType::LocalFile)
        {
            BaseTreeItemInfo* itemInfo = (BaseTreeItemInfo*) children[i]->getTreeItemInfo();

            itemId = itemInfo->itemId();
            uuid = itemInfo->uuid();
        }

        if(!itemIdSet.contains(itemId))
        {
            QModelIndex layoutIndex = getIndex(layoutTreeItemModel);
            qDebug() << "LayoutTreeModel::UpdateTreeLayoutItem() DELETE uuid:" << uuid << ", itemId:" << itemId;

            m_treeItemModelMap.remove(itemId);
            m_channelItemIdMapping[uuid].remove(itemId);

            // Delete
            beginRemoveRows(layoutIndex, i, i);
            layoutTreeItemModel->removeChild(i);
            endRemoveRows();
        }
    }

    invalidate();
}

void LayoutTreeModel::AddCameraItem(QString layoutId, QString itemId, QString deviceId, QString channelNumber)
{
    std::string stdDeviceId = deviceId.toStdString();
    std::string stdChannelNumber = channelNumber.toStdString();
    QString channelId = deviceId + "_" + channelNumber;

    qDebug() << "LayoutTreeModel::AddCameraItem LayoutID" << layoutId << ", ItemID" << itemId << ", DeviceID" << deviceId << ", channelNumber" << channelNumber;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::AddCameraItem LayoutID not contains "  << layoutId;
        return;
    }

    TreeItemModel* layoutModel = m_treeItemModelMap[layoutId];
    QModelIndex layoutIndex = getIndex(layoutModel);

    Wisenet::Device::Device device;
    Wisenet::Device::Device::Channel channel;
    Wisenet::Device::ChannelStatus channelStatus;

    if(!QCoreServiceManager::Instance().DB()->FindDevice(stdDeviceId, device))
    {
        qDebug() << "LayoutTreeModel::AddCameraItem cannot find device return " << deviceId;
        return;
    }

    if(!QCoreServiceManager::Instance().DB()->FindChannel(stdDeviceId, stdChannelNumber, channel))
    {
        qDebug() << "LayoutTreeModel::AddCameraItem cannot find channel " << deviceId << channelNumber;
        return;
    }

    if(!QCoreServiceManager::Instance().DB()->FindChannelStatus(stdDeviceId, stdChannelNumber, channelStatus))
    {
        qDebug() << "LayoutTreeModel::AddCameraItem cannot find devicestatus " << deviceId << channelNumber;
        return;
    }


    BaseTreeItemInfo* cameraInfo = newTreeItemInfo(channelId,
                                                   ItemType::Camera,
                                                   channelStatus.isGood()? ItemStatus::Normal : ItemStatus::Abnormal,
                                                   QString::fromUtf8(channel.name.c_str()),
                                                   QString::fromUtf8(channel.channelName.c_str()),
                                                   QString::fromUtf8(channel.channelName.c_str()),
                                                   QString::fromUtf8(device.connectionInfo.host.c_str()),
                                                   channelNumber.toInt(),
                                                   QString::fromStdString(device.deviceID),
                                                   itemId);

    TreeItemModel* cameraModel = new TreeItemModel(cameraInfo, layoutModel);

    beginInsertRows(layoutIndex, layoutModel->childCount(), layoutModel->childCount());
    layoutModel->appendChild(cameraModel);
    endInsertRows();

    if(!m_channelItemIdMapping.contains(channelId))
    {
        QSet<QString> set;
        m_channelItemIdMapping.insert(channelId, set);
    }

    m_treeItemModelMap.insert(itemId, cameraModel);
    m_channelItemIdMapping[channelId].insert(itemId);
}

void LayoutTreeModel::AddCameraItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem)
{
    QString layoutId = QString::fromStdString(layout.layoutID);
    QString itemId = QString::fromStdString(layoutItem.itemID);
    QString deviceId = QString::fromStdString(layoutItem.deviceID);
    QString channelNumber = QString::fromStdString(layoutItem.channelID);

    AddCameraItem(layoutId, itemId, deviceId, channelNumber);
}

void LayoutTreeModel::AddWebpageItem(QString layoutId, QString itemId, QString webpageId)
{
    qDebug() << "LayoutTreeModel::AddWebpageItem LayoutID" << layoutId << ", ItemID" << itemId << ", webpageId" << webpageId;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::AddWebpageItem LayoutID not contains "  << layoutId;
        return;
    }

    TreeItemModel* layoutModel = m_treeItemModelMap[layoutId];
    QModelIndex layoutIndex = getIndex(layoutModel);

    Wisenet::Core::Webpage webpage;

    if(!QCoreServiceManager::Instance().DB()->FindWebpage(webpageId, webpage))
        return;

    QString webPageName = QString::fromUtf8(webpage.name.c_str());

    //newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
    BaseTreeItemInfo* webpageInfo = newTreeItemInfo(webpageId, ItemType::WebPage, ItemStatus::Normal, webPageName, itemId);

    TreeItemModel* webpageModel = new TreeItemModel(webpageInfo, layoutModel);

    beginInsertRows(layoutIndex, layoutModel->childCount(), layoutModel->childCount());
    layoutModel->appendChild(webpageModel);
    endInsertRows();

    if(!m_channelItemIdMapping.contains(webpageId))
    {
        QSet<QString> set;
        m_channelItemIdMapping.insert(webpageId, set);
    }

    m_treeItemModelMap.insert(itemId, webpageModel);
    m_channelItemIdMapping[webpageId].insert(itemId);
}

void LayoutTreeModel::AddWebageItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem)
{
    QString layoutId = QString::fromStdString(layout.layoutID);
    QString itemId = QString::fromStdString(layoutItem.itemID);
    QString webpageId = QString::fromStdString(layoutItem.webpageID);

    AddWebpageItem(layoutId, itemId, webpageId);
}

void LayoutTreeModel::AddLocalFileItem(QString layoutId, QString itemId, QString fileUrl)
{
    qDebug() << "LayoutTreeModel::AddLocalFileItem LayoutID" << layoutId << ", ItemID" << itemId << ", fileUrl" << fileUrl;

    if(!m_treeItemModelMap.contains(layoutId))
    {
        qDebug() << "LayoutTreeModel::AddLocalFileItem LayoutID not contains "  << layoutId;
        return;
    }

    TreeItemModel* layoutModel = m_treeItemModelMap[layoutId];
    QModelIndex layoutIndex = getIndex(layoutModel);

    QUrl url = QUrl(fileUrl);
    QString localFileName = url.toLocalFile();
    QStringList token = localFileName.split('/');
    localFileName = token.last();

    //newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
    BaseTreeItemInfo* localfileInfo = newTreeItemInfo(fileUrl, ItemType::LocalFile, ItemStatus::Normal, localFileName, itemId);

    TreeItemModel* localfileModel = new TreeItemModel(localfileInfo, layoutModel);

    beginInsertRows(layoutIndex, layoutModel->childCount(), layoutModel->childCount());
    layoutModel->appendChild(localfileModel);
    endInsertRows();

    if(!m_channelItemIdMapping.contains(fileUrl))
    {
        QSet<QString> set;
        m_channelItemIdMapping.insert(fileUrl, set);
    }

    m_treeItemModelMap.insert(itemId, localfileModel);
    m_channelItemIdMapping[fileUrl].insert(itemId);
}

void LayoutTreeModel::AddLocalFileItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem)
{
    QString layoutId = QString::fromStdString(layout.layoutID);
    QString itemId = QString::fromStdString(layoutItem.itemID);
    QString fileUrl = QString::fromStdString(layoutItem.resourcePath);

    AddLocalFileItem(layoutId, itemId, fileUrl);
}

void LayoutTreeModel::AddTreeSequenceItem(Wisenet::Core::SequenceLayout& sequenceLayout)
{
    qDebug() << "LayoutTreeModel::AddTreeSequenceItem()";
    QString sequenceLayoutID = QString::fromStdString(sequenceLayout.sequenceLayoutID);

    if(m_sequenceTreeItemMap.contains(sequenceLayoutID))
    {
        qDebug() << "LayoutTreeModel::AddTreeSequenceItem() already added return";
        return;
    }

    BaseTreeItemInfo* sequenceLayoutInfo = (LayoutTreeItemInfo*)newTreeItemInfo(sequenceLayoutID, ItemType::Sequence, ItemStatus::Normal, QString::fromUtf8(sequenceLayout.name.c_str()));

    TreeItemModel * sequenceLayoutItemModel = new TreeItemModel(sequenceLayoutInfo, m_sequenceRootItem);

    QModelIndex sequenceLayoutRootIndex = index(m_sequenceRootItem->row(), 0);

    beginInsertRows(sequenceLayoutRootIndex, m_sequenceRootItem->childCount(), m_sequenceRootItem->childCount());
    m_sequenceRootItem->appendChild(sequenceLayoutItemModel);
    endInsertRows();

    m_sequenceTreeItemMap.insert(sequenceLayoutID, sequenceLayoutItemModel);

    if(sequenceLayout.sequenceLayoutItems.size() <= 0)
        return;

    QModelIndex sequenceIndex = getIndex(sequenceLayoutItemModel);

    beginInsertRows(sequenceIndex, 0, sequenceLayout.sequenceLayoutItems.size() - 1);
    for(auto& sequenceLayoutItem : sequenceLayout.sequenceLayoutItems)
    {
        Wisenet::Core::Layout layout;

        if(!QCoreServiceManager::Instance().DB()->FindLayout(sequenceLayoutItem.layoutID, layout))
            continue;

        QString layoutId = QString::fromStdString(layout.layoutID);
        QString itemId = QString::fromStdString(sequenceLayoutItem.itemID);

        LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)newTreeItemInfo(layoutId, ItemType::SequenceItem, ItemStatus::Normal, QString::fromUtf8(layout.name.c_str()));
        layoutInfo->setItemId(itemId);
        layoutInfo->setLayout(layout);
        TreeItemModel * layoutItem = new TreeItemModel(layoutInfo, sequenceLayoutItemModel);

        sequenceLayoutItemModel->appendChild(layoutItem);
    }
    endInsertRows();
}

void LayoutTreeModel::UpdateTreeSequenceItem(Wisenet::Core::SequenceLayout& sequenceLayout)
{
    qDebug() << "LayoutTreeModel::UpdateTreeSequenceItem()";

    QString sequenceLayoutID = QString::fromStdString(sequenceLayout.sequenceLayoutID);
    QString sequenceLayoutName = QString::fromUtf8(sequenceLayout.name.c_str());

    if(!m_sequenceTreeItemMap.contains(sequenceLayoutID))
    {
        qDebug() << "LayoutTreeModel::UpdateTreeSequenceItem() not contains - return" << sequenceLayoutID << sequenceLayoutName;
        return;
    }

    TreeItemModel * sequenceLayoutItemModel = m_sequenceTreeItemMap[sequenceLayoutID];

    // Name update
    BaseTreeItemInfo* sequenceLayoutInfo = sequenceLayoutItemModel->getTreeItemInfo();
    sequenceLayoutInfo->setDisplayName(sequenceLayoutName);

    QModelIndex sequenceIndex = getIndex(sequenceLayoutItemModel);

    // Delete all layout items
    beginRemoveRows(sequenceIndex, 0, sequenceLayoutItemModel->childCount()-1);
    sequenceLayoutItemModel->removeAllChild();
    endRemoveRows();

    if(sequenceLayout.sequenceLayoutItems.size() == 0)
        return;

    // Add items
    beginInsertRows(sequenceIndex, 0, sequenceLayout.sequenceLayoutItems.size() - 1);
    for(auto& sequenceLayoutItem : sequenceLayout.sequenceLayoutItems)
    {
        Wisenet::Core::Layout layout;
        if(!QCoreServiceManager::Instance().DB()->FindLayout(sequenceLayoutItem.layoutID, layout))
            continue;

        QString layoutId = QString::fromStdString(layout.layoutID);
        QString itemId = QString::fromStdString(sequenceLayoutItem.itemID);

        LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)newTreeItemInfo(layoutId, ItemType::SequenceItem, ItemStatus::Normal, QString::fromUtf8(layout.name.c_str()));
        layoutInfo->setLayout(layout);
        layoutInfo->setItemId(itemId);
        TreeItemModel * layoutItem = new TreeItemModel(layoutInfo, sequenceLayoutItemModel);

        sequenceLayoutItemModel->appendChild(layoutItem);
    }
    endInsertRows();

    invalidate();
}

void LayoutTreeModel::Event_SaveLayout(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_SaveLayout";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutEvent>(event->eventDataPtr);

    QString layoutId = QString::fromStdString(coreEvent->layout.layoutID);

    if(!m_treeItemModelMap.contains(layoutId))
    {
        if(QCoreServiceManager::Instance().DB()->IsSharedLayout(layoutId))
        {
            qDebug() << "LayoutTreeModel::Event_SaveLayout SharedLayout return" << layoutId;
            return;
        }

        AddTreeLayoutItem(coreEvent->layout);

        emit layoutAdded();
    }
    else
    {
        LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)m_treeItemModelMap[layoutId]->getTreeItemInfo();
        layoutInfo->setLayout(coreEvent->layout);

        UpdateTreeLayoutItem(coreEvent->layout);

//        qDebug() << "LayoutTreeModel::Event_SaveLayout - 1 m_sequenceTreeItemMap.size="<<m_sequenceTreeItemMap.size();
        for(auto& item : m_sequenceTreeItemMap) {
//            qDebug() << "LayoutTreeModel::Event_SaveLayout - 2-1";

            if(!item)
                continue;

            auto children = item->children();

//            qDebug() << "LayoutTreeModel::Event_SaveLayout - 2-2 item->children.size="<<children.size();

            for(auto& layoutItem  : children) {
                if(layoutItem->getItemType() != ItemType::SequenceItem) {
                    continue;
                }
//                qDebug() << "LayoutTreeModel::Event_SaveLayout - 3-1 layoutItem->getName="<<layoutItem->getName();

                if(layoutItem->getUuid() == layoutId && layoutItem->getName() != QString::fromUtf8(coreEvent->layout.name.c_str())) {
//                    qDebug() << "LayoutTreeModel::Event_SaveLayout name to"<<QString::fromUtf8(coreEvent->layout.name.c_str());
                    layoutItem->setName(QString::fromUtf8(coreEvent->layout.name.c_str()));
                }
            }

//            qDebug() << "LayoutTreeModel::Event_SaveLayout - 2-2";
        }
    }

    invalidate();
}

void LayoutTreeModel::Event_SaveLayouts(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_SaveLayouts";
    auto saveLayoutsEvent = std::static_pointer_cast<Wisenet::Core::SaveLayoutsEvent>(event->eventDataPtr);

    for(auto& layout : saveLayoutsEvent->layouts)
    {
        QString layoutId = QString::fromStdString(layout.first);

        if(!m_treeItemModelMap.contains(layoutId))
        {
            if(QCoreServiceManager::Instance().DB()->IsSharedLayout(layoutId))
            {
                qDebug() << "LayoutTreeModel::Event_SaveLayouts SharedLayout continue" << layoutId;
                continue;
            }

            AddTreeLayoutItem(layout.second);
        }
        else
        {
            LayoutTreeItemInfo* layoutInfo = (LayoutTreeItemInfo*)m_treeItemModelMap[layoutId]->getTreeItemInfo();
            layoutInfo->setLayout(layout.second);

            UpdateTreeLayoutItem(layout.second);

            for(auto& item : m_sequenceTreeItemMap) {

                BaseTreeItemInfo* treeItemInfo = item->getTreeItemInfo();

                if(treeItemInfo->itemType() != ItemType::SequenceItem) {
                    continue;
                }

                if(treeItemInfo->uuid() == layoutId && treeItemInfo->displayName() != layoutInfo->displayName()) {
                    qDebug() << "LayoutTreeModel::Event_SaveLayouts name to"<<layoutInfo->displayName();
                    treeItemInfo->setDisplayName(layoutInfo->displayName());
                }
            }
        }
    }

    qDebug() << "LayoutTreeModel::Event_SaveLayouts-------7 fin";

    invalidate();
}

void LayoutTreeModel::Event_RemoveLayout(QCoreServiceEventPtr event)
{
    auto removeLayoutsEvent = std::static_pointer_cast<Wisenet::Core::RemoveLayoutsEvent>(event->eventDataPtr);

    for (auto& uuid : removeLayoutsEvent->layoutIDs) {

        QString deletedLayoutId = QString::fromStdString(uuid);

        if(!m_treeItemModelMap.contains(deletedLayoutId))
            continue;

        TreeItemModel* layoutItemModel = m_treeItemModelMap.take(deletedLayoutId);

        QList<TreeItemModel*> channelList = layoutItemModel->children();

        for(auto& channelModel : channelList)
        {
            BaseTreeItemInfo* channelInfo = channelModel->getTreeItemInfo();
            QString channelUuid = channelInfo->uuid();

            m_treeItemModelMap.remove(channelUuid);
            m_channelItemIdMapping[channelUuid].remove(channelInfo->itemId());
        }

        beginRemoveRows(getIndex(m_layoutRootItem), layoutItemModel->row(), layoutItemModel->row());
        m_layoutRootItem->removeChild(layoutItemModel->row());
        endRemoveRows();
    }
}

void LayoutTreeModel::Event_SaveChannels(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_SaveChannels()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(event->eventDataPtr);

    for(auto& channel : coreEvent->channels)
    {
        if(channel.use == false)
            continue;

        QString channelId = QString::fromStdString(channel.deviceID) + "_" + QString::fromStdString(channel.channelID);
        QString channelName = QString::fromUtf8(channel.name.c_str());

        if(m_channelItemIdMapping.contains(channelId))
        {
            QSet<QString> itemSet = m_channelItemIdMapping[channelId];

            for(auto& itemId : itemSet)
            {
                qDebug() << "LayoutTreeModel::Event_SaveChannels() itemId:" << itemId;
                if(m_treeItemModelMap.contains(itemId))
                {
                    TreeItemModel* itemModel = m_treeItemModelMap[itemId];
                    itemModel->setName(channelName);
                }
            }
        }
    }

    invalidate();
}

void LayoutTreeModel::Event_SaveWebpage(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveWebpageEvent>(event->eventDataPtr);

    Wisenet::Core::Webpage webpage = coreEvent->webpage;

    QString webPageId = QString::fromStdString(webpage.webpageID);
    QString webpageName =QString::fromUtf8(webpage.name.c_str());

    if(m_channelItemIdMapping.contains(webPageId))
    {
        QSet<QString> itemSet = m_channelItemIdMapping[webPageId];

        for(auto& itemId : itemSet)
        {
            TreeItemModel* itemModel = m_treeItemModelMap[itemId];
            itemModel->setName(webpageName);
        }
    }

    invalidate();
}

void LayoutTreeModel::Event_RemoveWebpage(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_RemoveWebpage()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveWebpagesEvent>(event->eventDataPtr);

    for(auto& id : coreEvent->webpageIDs)
    {
        QString webpageId = QString::fromStdString(id);

        qDebug() << "LayoutTreeModel::Event_RemoveWebpage() webpageId:" << webpageId;

        if(m_channelItemIdMapping.contains(webpageId))
        {
            QSet<QString> itemSet = m_channelItemIdMapping[webpageId];

            for(auto& itemId : itemSet)
            {
                qDebug() << "LayoutTreeModel::Event_RemoveWebpage() itemId:" << itemId;
                if(m_treeItemModelMap.contains(itemId))
                {
                    TreeItemModel* itemModel = m_treeItemModelMap.take(itemId);
                    TreeItemModel* layoutModel = itemModel->parentItem();
                    QModelIndex layoutIndex = getIndex(layoutModel);

                    beginRemoveRows(layoutIndex, itemModel->row(), itemModel->row());
                    layoutModel->removeChild(itemModel->row());
                    endRemoveRows();
                }
            }

            m_channelItemIdMapping.remove(webpageId);
        }
    }

    invalidate();
}

void LayoutTreeModel::Event_UpdateDevices(QCoreServiceEventPtr event)
{
    auto devicesEvent = std::static_pointer_cast<Wisenet::Core::UpdateDevicesEvent>(event->eventDataPtr);

    for(auto & device : devicesEvent->devices){
        QString deviceUuid = QString::fromStdString(device.deviceID);
        for (auto& channel : device.channels)
        {
            QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);

            if(m_channelItemIdMapping.contains(channelUuid))
            {
                QSet<QString> itemSet = m_channelItemIdMapping[channelUuid];

                for(auto& itemId : itemSet)
                {
                    qDebug() << "LayoutTreeModel::Event_SaveChannels() itemId:" << itemId;
                    if(m_treeItemModelMap.contains(itemId))
                    {
                        TreeItemModel* itemModel = m_treeItemModelMap[itemId];
                        itemModel->setName(QString::fromUtf8(channel.second.name.c_str()));
                    }
                }
            }
        }
    }
    emit invalidate();
}

void LayoutTreeModel::Event_SaveSequenceLayout(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_SaveSequenceLayout";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveSequenceLayoutEvent>(event->eventDataPtr);

    QString sequenceId = QString::fromStdString(coreEvent->sequenceLayout.sequenceLayoutID);

    if(!m_sequenceTreeItemMap.contains(sequenceId))
    {
        AddTreeSequenceItem(coreEvent->sequenceLayout);
    }
    else
    {
        UpdateTreeSequenceItem(coreEvent->sequenceLayout);
    }

    invalidate();
}

void LayoutTreeModel::Event_RemoveSequenceLayout(QCoreServiceEventPtr event)
{
    qDebug() << "LayoutTreeModel::Event_RemoveSequenceLayout";
    auto removeSequenceLayoutsEvent = std::static_pointer_cast<Wisenet::Core::RemoveSequenceLayoutsEvent>(event->eventDataPtr);

    for (auto& uuid : removeSequenceLayoutsEvent->sequenceLayoutIDs) {

        QString deletedSequenceLayoutId = QString::fromStdString(uuid);

        if(m_sequenceTreeItemMap.find(deletedSequenceLayoutId) == m_sequenceTreeItemMap.end()) {
            continue;
        }

        /*
        QList<TreeItemModel*> layoutList = m_sequenceTreeItemMap[deletedSequenceLayoutId]->children();

        for(auto& layoutModel : layoutList)
        {
            BaseTreeItemInfo* channelInfo = layoutModel->getTreeItemInfo();
            QString layoutUuid = channelInfo->uuid();

            delete m_sequenceTreeItemMap.take(layoutUuid);
        }*/

        TreeItemModel* sequenceModel = m_sequenceTreeItemMap.take(deletedSequenceLayoutId);

        beginRemoveRows(getIndex(m_sequenceRootItem), sequenceModel->row(), sequenceModel->row());
        m_sequenceRootItem->removeChild(sequenceModel->row());
        endRemoveRows();
    }

    invalidate();
}

void LayoutTreeModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);

    qDebug() << "LayoutTreeModel::Event_DeviceStatus deviceUuid " << deviceUuid;

    for (auto& channel : deviceStatusEvent->deviceStatus.channelsStatus)
    {
        QString channelUuid = deviceUuid + "_" + QString::fromStdString(channel.first);

        if(m_channelItemIdMapping.contains(channelUuid)){

            for(auto& itemId : m_channelItemIdMapping[channelUuid].values())
            {
                if(m_treeItemModelMap.contains(itemId))
                {
                    TreeItemModel* channelItemModel = m_treeItemModelMap[itemId];

                    if(channelItemModel == nullptr)
                    {
                        qDebug() << "LayoutTreeModel::Event_DeviceStatus m_treeItemModelMap.contains() true nullptr " << itemId;
                        continue;
                    }

                    int channelStatus = channel.second.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

                    qDebug() << "LayoutTreeModel::Event_DeviceStatus channelUuid [" << channelUuid << "] status [" << channelStatus << "]";
                    channelItemModel->getTreeItemInfo()->setItemStatus(channelStatus);
                }
                else
                {
                    qDebug() << "LayoutTreeModel::Event_DeviceStatus m_treeItemModelMap.contains() false " << itemId;
                }
            }
        }
    }
}

void LayoutTreeModel::Event_ChannelStatus(QCoreServiceEventPtr event)
{
    auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(event->eventDataPtr);

    for (auto& channel : channelStatusEvent->channelsStatus)
    {
        Wisenet::Device::ChannelStatus channelStatus = channel.second;

        QString channelUuid = QString::fromStdString(channelStatus.deviceID) + "_" + QString::fromStdString(channelStatus.channelID);

        if(m_channelItemIdMapping.contains(channelUuid)){

            int status = channelStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;

            for(auto& itemId : m_channelItemIdMapping[channelUuid].values())
            {
                if(m_treeItemModelMap.contains(itemId))
                {
                    TreeItemModel* channelItemModel = m_treeItemModelMap[itemId];

                    if(channelItemModel == nullptr)
                    {
                        qDebug() << "LayoutTreeModel::Event_ChannelStatus m_treeItemModelMap.contains() true nullptr " << itemId;
                        continue;
                    }

                    qDebug() << "LayoutTreeModel::Event_ChannelStatus channelUuid [" << channelUuid << "] status [" << status << "]";
                    channelItemModel->getTreeItemInfo()->setItemStatus(status);
                }
                else
                {
                    qDebug() << "LayoutTreeModel::Event_ChannelStatus m_treeItemModelMap.contains() false " << itemId;
                }
            }
        }
    }
}

void LayoutTreeModel::Request_UpdateLayout(Wisenet::Core::Layout& layout)
{
    qDebug() << "LayoutTreeModel::Request_UpdateLayout";
    auto saveLayoutRequest = std::make_shared<Wisenet::Core::SaveLayoutRequest>();
    saveLayoutRequest->layout = layout;
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveLayout,
                this, saveLayoutRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = reply->responseDataPtr;
        if(response->isSuccess()){
            qDebug() << "LayoutTreeModel::Request_UpdateLayout success";
        }
        else
            qDebug() << "LayoutTreeModel::Request_UpdateLayout fail";
    });
}

void LayoutTreeModel::Request_RemoveLayout(const QString& layoutId)
{
    qDebug() << "LayoutTreeModel::Request_RemoveLayout()";
    std::string layoutToDelete = layoutId.toStdString();

    auto request = std::make_shared<Wisenet::Core::RemoveLayoutsRequest>();
    request->layoutIDs.push_back(layoutToDelete);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveLayouts,
                this, request,
                [this, layoutId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "LayoutTreeModel::Request_RemoveLayout - RequestToCoreService : delete fail " << layoutId;
        }
        else {
            qDebug() << "LayoutTreeModel::Request_RemoveLayout - RequestToCoreService : delete success " << layoutId;
        }
    });
}

void LayoutTreeModel::Request_UpdateSequenceLayout(Wisenet::Core::SequenceLayout& sequenceLayout)
{
    qDebug() << "LayoutTreeModel::Request_UpdateSequenceLayout()"; //SaveSequenceLayout
//    std::string stdId = sequenceLayoutId.toStdString();

    auto request = std::make_shared<Wisenet::Core::SaveSequenceLayoutRequest>();
    request->sequenceLayout = sequenceLayout;

    auto sequenceLayoutId = QString::fromStdString(sequenceLayout.sequenceLayoutID);
    auto sequenceName = QString::fromStdString(sequenceLayout.name);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveSequenceLayout,
                this, request,
                [this, sequenceName, sequenceLayoutId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "LayoutTreeModel::Request_UpdateSequenceLayout - fail " << sequenceName;
        }
        else {
            qDebug() << "LayoutTreeModel::Request_UpdateSequenceLayout - success " << sequenceName;
            emit sequenceInfoChanged(sequenceName, sequenceLayoutId); // 시퀀스 DB Update가 완료되었으므로 LayoutNavigationView에 UI 갱신 요청
        }
    });
}

void LayoutTreeModel::Request_RemoveSequenceLayout(const QString& sequenceLayoutId)
{
    qDebug() << "LayoutTreeModel::Request_RemoveSequenceLayout()";
    std::string stdId = sequenceLayoutId.toStdString();

    auto request = std::make_shared<Wisenet::Core::RemoveSequenceLayoutsRequest>();
    request->sequenceLayoutIDs.push_back(stdId);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveSequenceLayouts,
                this, request,
                [this, sequenceLayoutId](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isFailed()) {
            qDebug() << "LayoutTreeModel::Request_RemoveSequenceLayout - RequestToCoreService : delete fail " << sequenceLayoutId;
        }
        else {
            qDebug() << "LayoutTreeModel::Request_RemoveSequenceLayout - RequestToCoreService : delete success " << sequenceLayoutId;
        }
    });
}

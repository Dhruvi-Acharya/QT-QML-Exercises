#include "DashboardDeviceTreeModel.h"

DashboardDeviceTreeModel::DashboardDeviceTreeModel(QObject *parent)
    : BaseTreeModel(parent)
{
    //qDebug() << "DashboardGroupTreeModel()";
    m_roleNameMapping[TreeModelRoleName] = "display";

    SetupTree();

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DashboardDeviceTreeModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DashboardDeviceTreeModel::~DashboardDeviceTreeModel()
{
    //qDebug() << "~DashboardGroupTreeModel()";
    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }
}

void DashboardDeviceTreeModel::SetupTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo);

    QUuid empty;
    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("Root"));
    m_rootItemModel = new TreeItemModel(deviceInfo, rootItem);

    rootItem->appendChild(m_rootItemModel);
    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_rootItemModel);

    endResetModel();
}

void DashboardDeviceTreeModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("ResourceTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        Event_FullInfo(event);
        break;
    case Wisenet::Core::AddDeviceEventType:
        Event_AddDevice(event);
        break;
    case Wisenet::Core::RemoveDevicesEventType:
        Event_RemoveDevices(event);
        break;
    case Wisenet::Device::DeviceStatusEventType:
        Event_DeviceStatus(event);
        break;
    }
}

BaseTreeItemInfo* DashboardDeviceTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
{
    BaseTreeItemInfo *t;

    t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

void DashboardDeviceTreeModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    qDebug() << "DashboardDeviceTreeModel::Event_FullInfo() groups:" << coreEvent->groups.size() << ", devices:" << coreEvent->devices.size() << ", webpages:" << coreEvent->webpages.size();

    for (auto& kv : coreEvent->devices) {
        auto& device = kv.second;
        auto& deviceStatus = coreEvent->devicesStatus[kv.first];
        AddTreeDeviceItem(device, deviceStatus);
    }

    emit expandRoot();
}

void DashboardDeviceTreeModel::Event_AddDevice(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);
    AddTreeDeviceItem(coreEvent->device, coreEvent->deviceStatus);
}

void DashboardDeviceTreeModel::Event_RemoveDevices(QCoreServiceEventPtr event)
{
    auto removeDevices = std::static_pointer_cast<Wisenet::Core::RemoveDevicesEvent>(event->eventDataPtr);

    for(auto & stdDeviceId : removeDevices->deviceIDs)
    {
        QString deviceId = QString::fromStdString(stdDeviceId);
        RemoveTreeDeviceItem(deviceId);
    }
}

void DashboardDeviceTreeModel::Event_DeviceStatus(QCoreServiceEventPtr event)
{
    auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);

    QString deviceUuid = QString::fromStdString(deviceStatusEvent->deviceID);

    //qDebug() << "DashboardDeviceTreeModel::Event_DeviceStatus deviceUuid " << deviceUuid;

    if(m_treeItemModelMap.contains(deviceUuid))
    {
        TreeItemModel* deviceItemModel = m_treeItemModelMap[deviceUuid];
        int deviceStatus = deviceStatusEvent->deviceStatus.isGood() ? (int)ItemStatus::Normal : (int)ItemStatus::Abnormal;
        deviceItemModel->getTreeItemInfo()->setItemStatus(deviceStatus);

        //qDebug() << "DashboardDeviceTreeModel::Event_DeviceStatus deviceUuid [" << deviceUuid << "] status [" << deviceStatus << "]";
    }
}

void DashboardDeviceTreeModel::AddTreeDeviceItem(const Wisenet::Device::Device& device,
                                                 const Wisenet::Device::DeviceStatus& deviceStatus)
{
    QString deviceId = QString::fromStdString(device.deviceID);

    ItemStatus status = deviceStatus.isGood() ? ItemStatus::Normal : ItemStatus::Abnormal;

    QModelIndex deviceRootIndex = getIndex(m_rootItemModel);

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(deviceId, ItemType::Device, status, QString::fromStdString(device.name + " (" + device.connectionInfo.host + ")"));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_rootItemModel);

    beginInsertRows(deviceRootIndex, m_rootItemModel->childCount(), m_rootItemModel->childCount());
    m_rootItemModel->appendChild(deviceModel);

    endInsertRows();

    m_treeItemModelMap.insert(deviceId, deviceModel);
}

void DashboardDeviceTreeModel::RemoveTreeDeviceItem(const QString& deviceId)
{
    if(!m_treeItemModelMap.contains(deviceId))
        return;

    TreeItemModel* treeModel = m_treeItemModelMap.take(deviceId);
    TreeItemModel* parentModel = treeModel->parentItem();
    int rowNumber = treeModel->row();

    QModelIndex parentIndex = getIndex(parentModel);
    beginRemoveRows(parentIndex, rowNumber, rowNumber);
    parentModel->removeChild(rowNumber);
    endRemoveRows();
}

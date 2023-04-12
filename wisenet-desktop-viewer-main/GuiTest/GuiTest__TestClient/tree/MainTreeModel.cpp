/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#include "MainTreeModel.h"
#include <QUuid>
#include <QDebug>
#include "LogSettings.h"

MainTreeModel::MainTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    TreeItemInfo rootInfo {TreeItemInfo::MainRoot, TreeItemInfo::Normal};
    rootInfo .SetNames("Root");

    m_rootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), rootInfo);
    SetupCategoryTree();

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &MainTreeModel::coreServiceEventTriggered, Qt::QueuedConnection);   
  
}

MainTreeModel::~MainTreeModel()
{
    if(m_rootItem)
        delete m_rootItem;    
}


QModelIndex MainTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    MainTreeItemBase *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    MainTreeItemBase *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex MainTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    MainTreeItemBase *childItem = getItem(index);
    MainTreeItemBase *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == m_rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->IndexOfParent(), 0, parentItem);
}

int MainTreeModel::rowCount(const QModelIndex &parent) const
{
    const MainTreeItemBase *parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

int MainTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MainTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qDebug() << "***********invalid index-------------";
        return QVariant();
    }

    //if (role != Qt::DisplayRole && role != Qt::EditRole) {
    if (role != Qt::DisplayRole) {
        qDebug() << "------------invalid role-------------";
        return QVariant();
    }

    MainTreeItemBase *item = getItem(index);

    QVariant v;
    v.setValue(item->getData());
    return v;
}

MainTreeItemBase *MainTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        MainTreeItemBase *item = static_cast<MainTreeItemBase*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}

Qt::ItemFlags MainTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
    //return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}


void MainTreeModel::SetupCategoryTree()
{

    TreeItemInfo info {TreeItemInfo::DeviceRoot, TreeItemInfo::Normal};
    info.SetNames("Device");
    m_deviceRootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::LayoutRoot); info.SetNames("Layout");
    m_layoutRootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::GroupRoot); info.SetNames("Group");
    m_groupRootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::UserRoot); info.SetNames("User");
    m_userRootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);


    //Test Code
    /*info.SetType(TreeItemInfo::User); info.SetNames("Admin");
    m_userRootItem->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));*/

    /*
    info.SetType(TreeItemInfo::Camera);
    info.SetNames("CamName", "SNB-6004", "SNB-6004", "192.168.100.10");
    info.SetStatus(TreeItemInfo::Normal);
    m_deviceRootItem->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));*/
    //Test Code End
    m_rootItem->pushBackChild(m_deviceRootItem);
    m_rootItem->pushBackChild(m_layoutRootItem);
    m_rootItem->pushBackChild(m_groupRootItem);
    m_rootItem->pushBackChild(m_userRootItem);

    //TreeItemInfo info2{ TreeItemInfo::Camera, TreeItemInfo::Normal };
    //info2.SetNames("CamName", "SNB-6004", "SNB-6004", "192.168.100.10");
    //m_deviceRootItem->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info2));
}

QHash<int,QByteArray> MainTreeModel::roleNames() const
{
    auto a = QAbstractItemModel::roleNames();
    return {
        //{Qt::UserRole + 1, "deviceTree"},
        {Qt::DisplayRole, "display"}//,
//        {roleDataType, "dataType"},
//        {roleView, "view"},
//        {roleCommand, "command"},
//        {roleHighlight, "highlight"},
//        {roleVisible, "visible"},
//        {roleRef1, "ref1"}
    };
}

void MainTreeModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    SPDLOG_INFO("MainTreeModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    if (serviceEvent->EventTypeId() == Wisenet::Core::AddDeviceEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::AddDeviceEvent>(event->eventDataPtr);

        AddTreeDeviceItem(coreEvent->device, coreEvent->deviceStatus);
    }

    else if (serviceEvent->EventTypeId() == Wisenet::Core::FullInfoEventType) {
        auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

        for (auto& kv : coreEvent->devices) {
            auto& device = kv.second;
            auto& deviceStatus = coreEvent->devicesStatus[kv.first];
            AddTreeDeviceItem(device, deviceStatus);
        }
    }
}

void MainTreeModel::AddTreeDeviceItem(const Wisenet::Device::Device& device,
                                      const Wisenet::Device::DeviceStatus& deviceStatus)
{
    TreeItemInfo::ItemType itemType = TreeItemInfo::ItemType::Recorder;

    if(device.deviceType == Wisenet::Device::DeviceType::SunapiCamera)
        itemType = TreeItemInfo::ItemType::Camera;

    TreeItemInfo info{ itemType, TreeItemInfo::ItemStatus::Abnormal };
    qDebug() << "new device(multi channel):: uuid::" << device.deviceID.c_str() << ", model=" << device.modelName.c_str();
    info.SetNames(
                QString::fromUtf8(device.name.c_str()),
                QString::fromUtf8(device.modelName.c_str()),
                QString::fromUtf8(device.deviceName.c_str()),
                QString::fromUtf8(device.connectionInfo.host.c_str()),
                QString::fromUtf8(device.deviceID.c_str()));
    info.SetStatus(deviceStatus.isGood()?TreeItemInfo::ItemStatus::Normal:TreeItemInfo::ItemStatus::Abnormal);

    MainTreeItemBase* m_itemList = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);;

    m_deviceRootItem->pushBackChild(m_itemList);

    for (auto& item : device.channels) {
        TreeItemInfo channelInfo{ TreeItemInfo::ItemType::Channel, TreeItemInfo::ItemStatus::Abnormal };

        int channelNumber = std::stoi(item.second.channelID);
        channelInfo.SetNames(item.second.name.c_str(),
                             item.second.channelName.c_str(),
                             item.second.channelName.c_str(),
                             device.connectionInfo.host.c_str(),
                             device.deviceID.c_str());
        channelInfo.SetChannelNumber(channelNumber);

        auto citr = deviceStatus.channelsStatus.find(item.first);
        if (citr != deviceStatus.channelsStatus.end()) {
            auto& channelStatus = citr->second;
            if (channelStatus.isGood())
                channelInfo.SetStatus(TreeItemInfo::Normal);
            else
                channelInfo.SetStatus(TreeItemInfo::Abnormal);
        }

        m_itemList->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), channelInfo));

    }

    emit layoutChanged();
}

void MainTreeModel::RemoveTreeDeviceItem(QCoreServiceEventPtr event)
{
//    if (event)
//    {
//        auto evt = std::static_pointer_cast<Wisenet::Device::DeviceRemoveEvent>(event->eventDataPtr);
//        if (evt)
//        {
//            emit layoutChanged();
//        }
//    }
}

void MainTreeModel::UpdateTreeDeviceItem(QCoreServiceEventPtr event)
{
//    if (event)
//    {
//        auto evt = std::static_pointer_cast<Wisenet::Device::DeviceRemoveEvent>(event->eventDataPtr);
//        if (evt)
//        {
//            emit layoutChanged();
//        }
//    }
}

void MainTreeModel::showDeviceContextMenu( float x, float y)
{
    qDebug() << "Show Device Context Menu" << x << y ;
}

QString MainTreeModel::getSelectedIndex(const QModelIndex& itemIndex)
{
    QVariant value1 = itemIndex.data(Qt::DisplayRole);
    if (value1.isValid())
        return  value1.toString();
    else
        return QString();
}

void MainTreeModel::AddTreeUserItem(QCoreServiceEventPtr event)
{
//    auto loginEvent = std::static_pointer_cast<Wisenet::Core::LoginEvent>(event->eventDataPtr);

//    TreeItemInfo info{ TreeItemInfo::User, TreeItemInfo::ItemStatus::Normal};
//    info.SetNames(QString::fromStdString(loginEvent->userName));
//    m_userRootItem->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));
//    emit layoutChanged();
}

void MainTreeModel::refreshRootItem()
{
    m_rootItem->removeAllChild();
    SetupCategoryTree();
}




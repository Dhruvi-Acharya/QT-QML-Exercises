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
#include "DeviceEmailTreeSourceModel.h"
#include <QUuid>
#include "CoreServiceUtil.h"

DeviceEmailTreeSourceModel::DeviceEmailTreeSourceModel(QObject *parent)
    : BaseTreeModel(parent)
    ,m_resourceRootModel(nullptr)
{
    m_roleNameMapping[TreeModelRoleName] = "display";
    setupCategoryTree();
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DeviceEmailTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DeviceEmailTreeSourceModel::~DeviceEmailTreeSourceModel()
{

}

Qt::ItemFlags DeviceEmailTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());
    ItemType type = item->getItemType();
    if (ItemType::EventEmail == type){
        return flags;
    }

    flags ^= Qt::ItemIsSelectable;

    return flags;
}

void DeviceEmailTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
    case Wisenet::Core::UpdateDevicesEventType:
        readEmail();
        break;
    }
}

BaseTreeItemInfo *DeviceEmailTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    auto t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

BaseTreeItemInfo *DeviceEmailTreeSourceModel::newTreeItemInfo(const QString &uuid, const ItemType &type, const ItemStatus &status, const QString &name, const QString &modelName, const QString &deviceName, const QString &ipAddress, const int channelNumber)
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

void DeviceEmailTreeSourceModel::setupCategoryTree()
{
    beginResetModel();
    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::ResourceRoot, ItemStatus::Normal, tr("All user"));
    rootItem = new TreeItemModel(rootInfo);
    QUuid empty;
    auto* deviceInfo = newTreeItemInfo(empty.toString(QUuid::WithoutBraces), ItemType::EventEmailAllUser, ItemStatus::Normal, tr("All user"));
    m_resourceRootModel = new TreeItemModel(deviceInfo, rootItem);
    rootItem->appendChild(m_resourceRootModel);
    m_treeItemModelMap.insert(empty.toString(QUuid::WithoutBraces), m_resourceRootModel);
    endResetModel();
}


void DeviceEmailTreeSourceModel::readEmail()
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    beginResetModel();

    if(m_resourceRootModel)
        m_resourceRootModel->removeAllChild();
    m_treeItemModelMap.clear();

    SPDLOG_DEBUG("DeviceEmailTreeSourceModel::readEmail()");
    auto userGroups = db->GetUserGroups();
    auto users = db->GetUsers();

    // 추가된 그룹/이메일 리스트 출력
    for (auto& it : userGroups) {
        QVector<QVariant> groupInfo;

//        if (it.second.isAdminGroup) {
//            continue;
//        }

        // 그룹 추가
        addTreeEmailItem(QString::fromStdString(it.second.name), "", groupInfo);

        // 그룹 하위 이메일 추가
        for (auto& it2 : users) {
            // 그룹 아이디 같은지 확인
            if (QString::fromStdString(it.second.userGroupID) == QString::fromStdString(it2.second.userGroupID)) {
                SPDLOG_DEBUG("DeviceEmailTreeSourceModel::readEmail() userID: {}, groupName: {}, userID: {}, userEmail: {}"
                             , it2.second.userID
                             , it.second.name
                             , it2.second.name
                             , it2.second.email);
                addTreeEmailItem(QString::fromStdString(it2.second.userID), QString::fromStdString(it.second.name), QString::fromStdString(it2.second.name), QString::fromStdString(it2.second.email));
            }
        }
    }

    endResetModel();
}

void DeviceEmailTreeSourceModel::addTreeEmailItem(QString groupName, QString groupEmail, QVector<QVariant> info)
{
    SPDLOG_DEBUG("DeviceEmailTreeSourceModel::addTreeEmailItem group:{} {}", groupName.toStdString(), groupEmail.toStdString());

    BaseTreeItemInfo* deviceInfo = newTreeItemInfo(groupName, ItemType::EventEmailGroup, ItemStatus::Normal, QString(groupName));
    TreeItemModel* deviceModel = new TreeItemModel(deviceInfo, m_resourceRootModel);

    m_resourceRootModel->appendChild(deviceModel);
    m_treeItemModelMap.insert(groupName, deviceModel);
}

void DeviceEmailTreeSourceModel::addTreeEmailItem(QString uuid, QString groupName, QString userId, QString userEmail)
{
    auto email = userEmail;

    if (email.isEmpty()) {
        email = QCoreApplication::translate("WisenetLinguist", "Please set up your E-mail before using it.");
    }

    BaseTreeItemInfo* emailInfo = newTreeItemInfo(uuid,
                                                    ItemType::EventEmail,
                                                    ItemStatus::Normal,
                                                    QString(userId + " (" + email +")"));

    TreeItemModel* parentModel = m_treeItemModelMap[groupName];
    TreeItemModel* childModel = new TreeItemModel(emailInfo, parentModel);
    parentModel->appendChild(childModel);
    SPDLOG_DEBUG("addTreeEmailItem action:{} ++", childModel->getUuid().toStdString());
    m_treeItemModelMap.insert(childModel->getUuid(), childModel);
}

QModelIndex DeviceEmailTreeSourceModel::getModelIndex(QString guid)
{
    auto itemModel = m_treeItemModelMap.find(guid);
    if(itemModel != m_treeItemModelMap.end()){

        return this->getIndex(itemModel.value());
    }
    return QModelIndex();
}

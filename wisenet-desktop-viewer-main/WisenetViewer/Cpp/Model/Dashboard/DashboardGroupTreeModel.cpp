#include "DashboardGroupTreeModel.h"

DashboardGroupTreeModel::DashboardGroupTreeModel(QObject *parent)
    : BaseTreeModel(parent)
{
    //qDebug() << "DashboardGroupTreeModel()";
    m_roleNameMapping[TreeModelRoleName] = "display";

    SetupTree();

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &DashboardGroupTreeModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

DashboardGroupTreeModel::~DashboardGroupTreeModel()
{
    //qDebug() << "~DashboardGroupTreeModel()";
    if(rootItem)
    {
        delete rootItem;
        rootItem = nullptr;
    }
}

void DashboardGroupTreeModel::SetupTree()
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

void DashboardGroupTreeModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
        Event_FullInfo(event);
        break;
    case Wisenet::Core::SaveGroupEventType:
        Event_SaveGroup(event);
        break;
    case Wisenet::Core::RemoveGroupsEventType:
        Event_RemoveGroup(event);
        break;
    }
}

BaseTreeItemInfo* DashboardGroupTreeModel::newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name)
{
    BaseTreeItemInfo *t;

    t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);
    t->setDisplayName(name);

    return t;
}

void DashboardGroupTreeModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    qDebug() << "DashboardGroupTreeModel::Event_FullInfo() groups:" << coreEvent->groups.size() << ", devices:" << coreEvent->devices.size() << ", webpages:" << coreEvent->webpages.size();

    for (auto& kv : coreEvent->groups) {
        auto& group = kv.second;
        AddTreeGroupItem(group);
    }

    emit expandRoot();
}

void DashboardGroupTreeModel::Event_SaveGroup(QCoreServiceEventPtr event)
{
    qDebug() << "DashboardGroupTreeModel::Event_SaveGroup()";
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveGroupEvent>(event->eventDataPtr);

    QString qroupId = QString::fromStdString(coreEvent->group.groupID);

    if(this->m_treeItemModelMap.contains(qroupId))
        UpdateTreeGroupItem(coreEvent->group);
    else
    {
        AddTreeGroupItem(coreEvent->group);
    }
}

void DashboardGroupTreeModel::Event_RemoveGroup(QCoreServiceEventPtr event)
{
    qDebug() << "DashboardGroupTreeModel::Event_RemoveGroup()";
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

TreeItemModel* DashboardGroupTreeModel::AddTreeGroupItem(const Wisenet::Core::Group& group)
{
    QString groupID = QString::fromStdString(group.groupID);
    QString name = QString::fromUtf8(group.name.c_str());
    QString parentGroupID = QString::fromStdString(group.parentGroupID);

    QUuid emptyUuid;
    QString emptyUuidString = emptyUuid.toString(QUuid::WithoutBraces);

    if(m_treeItemModelMap.contains(groupID))
    {
        //qDebug() << "AddTreeGroupItem - already added : " << name << ", uuid : " << groupID;

        return m_treeItemModelMap[groupID];
    }

    // parent가 root
    if(parentGroupID == emptyUuidString)
    {
        //qDebug() << "AddTreeGroupItem1 parentGroupID == emptyUuidString" << name << ", uuid : " << groupID << ", parent : " << parentGroupID;
        BaseTreeItemInfo* groupInfo = newTreeItemInfo(groupID, ItemType::Group, ItemStatus::Normal, name);

        QModelIndex deviceRootIndex = getIndex(m_rootItemModel);

        TreeItemModel* groupModel = new TreeItemModel(groupInfo, m_rootItemModel);

        beginInsertRows(deviceRootIndex, m_rootItemModel->childCount(), m_rootItemModel->childCount());
        m_rootItemModel->appendChild(groupModel);
        endInsertRows();

        m_treeItemModelMap.insert(groupID, groupModel);

        return groupModel;
    }
    else // parent가 있음.
    {
        //qDebug() << "AddTreeGroupItem - has parent" << name << ", uuid : " << groupID << ", parent : " << parentGroupID;
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
            //qDebug() << "AddTreeGroupItem - error " << name << ", uuid : " << groupID;
            return nullptr;
        }
    }
}

void DashboardGroupTreeModel::UpdateTreeGroupItem(const Wisenet::Core::Group& group)
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


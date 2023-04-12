#include "ContextMenuGroupListModel.h"

ContextMenuGroupListModel::ContextMenuGroupListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[GroupIdRole] = "groupId";
    m_roleNames[GroupNameRole] = "groupName";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &ContextMenuGroupListModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void ContextMenuGroupListModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("ContextMenuGroupListModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

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

ContextMenuGroupListModel::~ContextMenuGroupListModel()
{
    qDebug() << "~ContextMenuGroupListModel()";
    removeAll();
}

void ContextMenuGroupListModel::initialize()
{
    qDebug() << "ContextMenuGroupListModel::initializeTree()";
    std::map<Wisenet::uuid_string, Wisenet::Core::Group> groupMap = QCoreServiceManager::Instance().DB()->GetGroups();

    for (auto& kv : groupMap) {
        auto& group = kv.second;

        GroupItem* groupItem = new GroupItem();

        groupItem->groupId = QString::fromStdString(group.groupID);
        groupItem->groupName =  QString::fromUtf8(group.name.c_str());

        this->append(groupItem);
    }
}

int ContextMenuGroupListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant ContextMenuGroupListModel::data(const QModelIndex &idx, int role) const
{
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    GroupItem* item = m_data.at(row);

    switch(role)
    {
    case GroupIdRole:
        return item->groupId;
    case GroupNameRole:
        return item->groupName;
    }

    return QVariant();
}

void ContextMenuGroupListModel::insert(int idx, GroupItem* item)
{
    if(idx < 0 || idx > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), idx, idx);
    m_data.insert(idx, item);
    endInsertRows();
}

void ContextMenuGroupListModel::append(GroupItem* item)
{
    insert(m_data.count(), item);
}

void ContextMenuGroupListModel::remove(int idx)
{
    if(idx < 0 || idx > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    delete m_data.takeAt(idx);
    endRemoveRows();
}

void ContextMenuGroupListModel::removeAll()
{
    if(m_data.count() == 0)
        return;

    beginResetModel();
    qDeleteAll(m_data);
    m_data.clear();
    endResetModel();
}

QList<GroupItem*> ContextMenuGroupListModel::getData()
{
    return m_data;
}

QHash<int, QByteArray> ContextMenuGroupListModel::roleNames() const
{
    return m_roleNames;
}

void ContextMenuGroupListModel::Event_FullInfo(QCoreServiceEventPtr event)
{
    auto coreEvent = std::static_pointer_cast<Wisenet::Core::FullInfoEvent>(event->eventDataPtr);

    for (auto& kv : coreEvent->groups) {
        auto& group = kv.second;

        GroupItem* groupItem = new GroupItem();

        groupItem->groupId = QString::fromStdString(group.groupID);
        groupItem->groupName =  QString::fromUtf8(group.name.c_str());

        this->append(groupItem);
    }
}

void ContextMenuGroupListModel::Event_SaveGroup(QCoreServiceEventPtr event)
{
    qDebug() << "ResourceTreeModel::Event_SaveGroup()";
    //auto coreEvent = std::static_pointer_cast<Wisenet::Core::SaveGroupEvent>(event->eventDataPtr);
    //QString qroupId = QString::fromStdString(coreEvent->group.groupID);

    removeAll();
    initialize();
}

void ContextMenuGroupListModel::Event_RemoveGroup(QCoreServiceEventPtr event)
{
    qDebug() << "ResourceTreeModel::Event_RemoveGroup()";
    /*auto coreEvent = std::static_pointer_cast<Wisenet::Core::RemoveGroupsEvent>(event->eventDataPtr);

    foreach(std::string groupId, coreEvent->groupIDs)
    {
        QString id = QString::fromStdString(groupId);
    }*/

    removeAll();
    initialize();
}

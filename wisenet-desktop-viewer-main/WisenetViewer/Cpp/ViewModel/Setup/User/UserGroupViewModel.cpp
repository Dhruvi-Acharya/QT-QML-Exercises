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
#include "UserGroupViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"

UserGroupViewModel::UserGroupViewModel(QObject* parent)
    :m_hoveredRow(-1), m_highlightRow(-1), m_lastHighlightRow(-1)
{
    Q_UNUSED(parent);
    m_columnNames << "ID" <<"Check" <<"User Group" << "Is Admin" << "Permission" << "Resources" << "Layout";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &UserGroupViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

int UserGroupViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int UserGroupViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.count();
}

QVariant UserGroupViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.size())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HighlightRole:
        if( m_highlightRows.count( index.row() ) )
            return true;
        return false;
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> UserGroupViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"} };
}

void UserGroupViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

void UserGroupViewModel::setHighlightRow(int rowNum, int modifiers)
{
    bool pressedCtrl = modifiers & Qt::ControlModifier;
    bool pressedShift = modifiers & Qt::ShiftModifier;

    bool fireRowChanged(false);
    int minRow(-1), maxRow(-1);

    if(m_highlightRows.count(rowNum)) {
        if(pressedCtrl || m_highlightRows.size() == 1) {
            //deselect it
            m_highlightRows.erase(rowNum);
            if(m_highlightRows.empty())
                m_highlightRow = -1;
            else
                m_highlightRow = *m_highlightRows.begin();
            fireRowChanged = true;
            minRow = rowNum; maxRow = rowNum;
            m_lastHighlightRow = -1;
        }
        else {
            minRow = *m_highlightRows.begin();
            maxRow = *m_highlightRows.rbegin();
            m_highlightRows.clear();
            m_highlightRows.insert(rowNum);
            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
        }
    }
    else {
        if(rowNum == -1) {
            //select none
            if(!m_highlightRows.empty()) {
                //clear selection
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
                m_highlightRows.clear();
                m_highlightRow = -1;
                m_lastHighlightRow = -1;
                fireRowChanged = true;
            }
        }
        else if(rowNum == -2) {
            //select all
            for(int irow=0; irow<m_index.size(); irow++){
                m_highlightRows.insert(irow);
            }
            fireRowChanged = true;
            m_highlightRow = m_index.size()-1;
            m_lastHighlightRow = m_index.size()-1;
            if(!m_highlightRows.empty()) {
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
            }
        }
        else {
            if((!pressedCtrl && !pressedShift)) {
                if(!m_highlightRows.empty()) {
                    minRow = *m_highlightRows.begin();
                    maxRow = *m_highlightRows.rbegin();
                    m_highlightRows.clear();
                }
            }
            if(!pressedShift || m_lastHighlightRow == -1){
                m_highlightRows.insert(rowNum);
            }
            else {
                int fromRow = m_lastHighlightRow;
                int toRow = rowNum;
                if(fromRow > toRow) std::swap(fromRow, toRow);
                minRow = fromRow;
                maxRow = toRow;
                while(fromRow <= toRow) {
                    m_highlightRows.insert(fromRow);
                    fromRow++;
                }
            }

            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
            if(minRow < 0 || rowNum < minRow) minRow = rowNum;
            if(maxRow < 0 || rowNum > maxRow) maxRow = rowNum;
        }
    }

    if(fireRowChanged) {
        emit highlightRowChanged();
        emit highlightRowCountChanged();
        emit selectedRowCountChanged();

        if(minRow != -1) {
            QVector<int> role;
            role << Role::HighlightRole;
            emit dataChanged(QModelIndex(index(minRow, 0)), QModelIndex(index(maxRow, columnCount() -1)), role);
        }
    }
}

void UserGroupViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, columnCount() -1)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, columnCount() -1)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, columnCount() -1)), role);
        }
    }
}

int UserGroupViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[UserGroupCheck].toBool() == true)
            ret++;
    }
    return ret;
}

int UserGroupViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVariant UserGroupViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull())
        return QVariant();

    return cellValue;
}

QStringList UserGroupViewModel::GetPermissionString(Wisenet::Core::UserGroup &group){
    QStringList permission;

    if(group.isAdminGroup)
        permission.push_back(QString("All permission"));
    else{
        if(group.userPermission.deviceControl)
            permission.push_back(PermissionDeviceControl);
        if(group.userPermission.playback)
            permission.push_back(PermissionPlayback);
        if(group.userPermission.exportVideo)
            permission.push_back(PermissionExportVideo);
        if(group.userPermission.localRecording)
            permission.push_back(PermissionLocalRecording);
        if(group.userPermission.ptzControl)
            permission.push_back(PermissionPtzControl);
        if(group.userPermission.audio)
            permission.push_back(PermissionAudio);
        if(group.userPermission.mic)
            permission.push_back(PermissionMic);
    }

    return permission;
}

QString UserGroupViewModel::GetDeviceString(Wisenet::Core::UserGroup &group){
    QString resource="";

    const int maxItemCount = 1;

    if(group.isAdminGroup || group.accessAllResources)
        resource = translate(QString::fromStdString("All Devices"));
    else if(group.assignedResource){
        // if(group.assignedResource->devices.size() > maxItemCount)
        //    resource = "Select " + std::to_string(group.assignedResource->devices.size()) + " devices";
        //else{
        int totalCount = 0;
        for(auto& device : group.assignedResource->devices){
            totalCount += device.second.channels.size();
        }
        SPDLOG_DEBUG("UserGroupViewModel::GetDeviceString() size:{}/{}", totalCount, maxItemCount);
        if(totalCount > maxItemCount)
            resource =
                     QString::fromStdString(std::to_string(totalCount)) + " " + translate(QString::fromStdString("channels"));
        else{
            for(auto& device : group.assignedResource->devices){
                for(auto& channelId : device.second.channels){
                    Wisenet::Device::Device::Channel channel;
                    if(QCoreServiceManager::Instance().DB()->FindChannel(device.first, channelId, channel)){
                        resource = QString::fromStdString(channel.name);
                        SPDLOG_DEBUG("UserGroupViewModel::GetDeviceString() channel.name:{}/{}", channel.name, channel.channelID);
                    }
                }
            }
        }
    }

    return resource;
}

QString UserGroupViewModel::translate(QVariant display){
    std::string result = display.toString().toStdString();
    return QCoreApplication::translate("WisenetLinguist",result.c_str());

}

QString UserGroupViewModel::GetLayoutString(Wisenet::Core::UserGroup &group)
{
    if(group.assignedResource){
        auto nLayouts = group.assignedResource->layouts.size();
        SPDLOG_DEBUG("UserGroupViewModel::GetLayoutString() size:{}", nLayouts);

        if(nLayouts > 0)
            return QString::number(nLayouts) + " " + translate(QString::fromStdString("Layouts"));
        else
            return "";
    }
    return "";
}

void UserGroupViewModel::reset()
{
    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;

    auto userGroups = QCoreServiceManager::Instance().DB()->GetUserGroups();
    int i = 0;

    for(auto& iter : userGroups){
        QVector<QVariant> display;
        display.push_back(QString::fromStdString(iter.first));
        display.push_back(false);
        display.push_back(QString::fromStdString(iter.second.name));
        display.push_back(iter.second.isAdminGroup);
        display.push_back(GetPermissionString(iter.second));
        display.push_back(GetDeviceString(iter.second));
        display.push_back(GetLayoutString(iter.second));
        m_data.push_back(display);
        m_index.push_back(i++);
    }

    SPDLOG_DEBUG("UserGroupViewModel::reset() end {}", userGroups.size());

    sort(UserGroupName);
}

void UserGroupViewModel::UpdateSortData(int column, Qt::SortOrder order)
{
    m_sorted.clear();

    m_sorted.fill(0, m_data.size());
    std::iota(m_sorted.begin(), m_sorted.end(), 0);

    std::sort(m_sorted.begin(),m_sorted.end(),[this, order,column](int indexA, int indexB){
        const QVariant& valueA = m_data[indexA][column];
        const QVariant& valueB = m_data[indexB][column];

        if(Qt::SortOrder::AscendingOrder == order)
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) < 0;
        else
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;
    });
}

bool UserGroupViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];
    m_data[row][UserGroupCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, UserGroupCheck)), QModelIndex(index(rowNum, UserGroupCheck)));
    emit selectedRowCountChanged();

    return true;
}

bool UserGroupViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index)
        m_data[item][UserGroupCheck] = checked;
    emit dataChanged(QModelIndex(index(0, UserGroupCheck)), QModelIndex(index(m_index.length()-1, UserGroupCheck)));
    emit selectedRowCountChanged();

    return true;
}

QString UserGroupViewModel::getUserGroupId(int rowNum)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];

    QString userId = m_data[whatRow][UserGroupTitle::UserGroupID].toString();

    return userId;
}

void UserGroupViewModel::deleteUserGroup()
{
    auto request = std::make_shared<Wisenet::Core::RemoveUsersRequest>();
    bool bFindAdmin = false;

    SPDLOG_DEBUG("UserGroupViewModel::deleteUserGroup start size:{} ", m_data.size());
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][UserGroupCheck].toBool() == false)
            continue;
        auto userGroupId = m_data[dataIndex][UserGroupID].toString().toStdString();
        auto groupName = m_data[dataIndex][UserGroupName].toString().toStdString();
        auto isAdmin = m_data[dataIndex][UserGroupIsAdmin].toBool();
        if(isAdmin){
            bFindAdmin = true;
            continue;
        }
        SPDLOG_DEBUG("UserGroupViewModel::deleteUserGroup id:{} {}", userGroupId, groupName);
        request->userIDs.push_back(userGroupId);
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveUserGroups,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(bFindAdmin)
            emit resultMessage(false, "administrator group can't remove");
        else if(response->isSuccess()){
            reset();
            emit resultMessage(true, "Success");
        }
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));
        qDebug() << "UserListViewModel::deleteUserGroup() response:" << QString::fromStdString(response->errorString());
    });
}

void UserGroupViewModel::editUserGroup(int row, int col, QString item)
{
    SPDLOG_DEBUG("UserGroupViewModel::editUserGroup row:{} col:{} item:{}", row, col, item.toStdString());

    if(row < 0 || row >= m_data.size())
        return;

    int whatRow = row;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];
    auto userId = m_data[whatRow][UserGroupID] ;

    auto request = std::make_shared<Wisenet::Core::SaveUserGroupRequest>();
    auto userGroups = QCoreServiceManager::Instance().DB()->GetUserGroups();


    auto user = userGroups.find(userId.toString().toStdString());

    if(user == userGroups.end()){
        return;
    }

    request->userGroup = user->second;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveUser,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "UserListViewModel::editUser() response:" << QString::fromStdString(response->errorString());

    });

}


void UserGroupViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveUserGroupEventType:
    case Wisenet::Core::RemoveUserGroupsEventType:
    case Wisenet::Core::SaveLayoutEventType:
    case Wisenet::Core::RemoveLayoutsEventType:
        reset();
    }
}


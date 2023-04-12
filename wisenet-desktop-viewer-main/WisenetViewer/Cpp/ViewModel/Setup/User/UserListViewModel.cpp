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
#include "UserListViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"

UserListViewModel::UserListViewModel(QObject* parent)
    :m_hoveredRow(-1), m_highlightRow(-1), m_lastHighlightRow(-1)
{
    Q_UNUSED(parent);
    m_columnNames << "UserGuid" <<"Check" << "ID" << "Ldap" << "Name" << "Type" << "Group ID" << "Group" << "Email" << "Description";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &UserListViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

int UserListViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int UserListViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.count();
}

QVariant UserListViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.count())
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

QHash<int, QByteArray> UserListViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"} };
}

void UserListViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

void UserListViewModel::setHighlightRow(int rowNum, int modifiers)
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

void UserListViewModel::setHoveredRow(int rowNum, bool hovered)
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

int UserListViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVariant UserListViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull()){
        return QVariant();
    }
    else{
        return cellValue;
    }

}

void UserListViewModel::reset()
{
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;

    auto users = db->GetUsers();
    auto userGroups = db->GetUserGroups();

    int i = 0;

    for(auto& iter : users){
        auto userGroup = userGroups.find(iter.second.userGroupID);
        std::string groupName = "";
        if(userGroup != userGroups.end())
            groupName = userGroup->second.name;

        QVector<QVariant> display;
        display.push_back(QString::fromStdString(iter.first));
        display.push_back(false);
        display.push_back(QString::fromStdString(iter.second.loginID));
        display.push_back(iter.second.ldapUser);
        display.push_back(QString::fromStdString(iter.second.name));
        display.push_back(iter.second.userType == Wisenet::Core::User::UserType::Owner?true:false);
        display.push_back(QString::fromStdString(iter.second.userGroupID));
        display.push_back(QString::fromStdString(groupName));
        display.push_back(QString::fromStdString(iter.second.email));
        display.push_back(QString::fromStdString(iter.second.description));

        m_data.push_back(display);
        m_index.push_back(i++);
    }

    SPDLOG_DEBUG("UserListViewModel::reset() end {}", users.size());

    sort(UserLoginID);
}

void UserListViewModel::UpdateSortData(int column, Qt::SortOrder order)
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

bool UserListViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("[UserListViewModel]setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];
    m_data[row][UserCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, UserCheck)), QModelIndex(index(rowNum, UserCheck)));
    emit selectedRowCountChanged();
    return true;
}

bool UserListViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index)
        m_data[item][UserCheck] = checked;
    emit dataChanged(QModelIndex(index(0, UserCheck)), QModelIndex(index(m_index.length()-1, UserCheck)));
    emit selectedRowCountChanged();

    return true;
}

int UserListViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[UserCheck].toBool() == true)
            ret++;
    }
    SPDLOG_DEBUG("[UserListViewModel] selectedRowCount:{}",ret);

    return ret;
}

QString UserListViewModel::getUserId(int rowNum)
{
   if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];

    QString userId = m_data[whatRow][UserTitle::UserID].toString();

    return userId;
}

bool UserListViewModel::getIsLdapUser(int rowNum)
{
   if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];

    if(m_data[whatRow][UserTitle::LdapUser].toBool())
        return true;
    else
        return false;
}

void UserListViewModel::deleteUser()
{
    auto request = std::make_shared<Wisenet::Core::RemoveUsersRequest>();
    bool bFindOwner = false;

    SPDLOG_DEBUG("UserListViewModel::deleteUser start size:{} ", m_data.size());

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][UserCheck].toBool() == false)
            continue;
        auto userId = m_data[dataIndex][UserID].toString().toStdString();
        auto loginId = m_data[dataIndex][UserLoginID].toString().toStdString();
        auto isOwner = m_data[dataIndex][UserType].toBool();
        if(isOwner){
            bFindOwner = true;
            continue;
        }
        SPDLOG_DEBUG("UserListViewModel::deleteUser id:{} {}", userId, loginId);
        request->userIDs.push_back(userId);
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveUsers,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "UserListViewModel::deleteUser() response:" << QString::fromStdString(response->errorString());
        if(bFindOwner)
            emit resultMessage(false, "admin can't remove");
        else if(response->isSuccess()){
            reset();
            emit resultMessage(true, "Success");
        }
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));
    });
}

void UserListViewModel::editUser(int row, int col, QString item)
{
    SPDLOG_DEBUG("UserListViewModel::editUser row:{} col:{} item:{}", row, col, item.toStdString());

    if(row < 0 || row >= m_data.size())
        return;

    int whatRow = row;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];
    auto userId = m_data[whatRow][UserID] ;

    auto request = std::make_shared<Wisenet::Core::SaveUserRequest>();
    auto users = QCoreServiceManager::Instance().DB()->GetUsers();


    auto user = users.find(userId.toString().toStdString());

    if(user == users.end()){
        return;
    }

    request->user = user->second;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveUser,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "UserListViewModel::editUser() response:" << QString::fromStdString(response->errorString());

    });
}


void UserListViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveUserGroupEventType:
    case Wisenet::Core::RemoveUserGroupsEventType:
    case Wisenet::Core::SaveUserEventType:
    case Wisenet::Core::RemoveUsersEventType:        
        reset();
    }
}

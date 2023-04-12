#include "LdapUserViewModel.h"
#include "LogSettings.h"
#include <QDebug>
#include <QMap>
#include <QVector>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

LdapUserViewModel::LdapUserViewModel(QObject* parent)
    :m_hoveredRow(-1), m_highlightRow(-1), m_lastHighlightRow(-1)
{
    Q_UNUSED(parent);
    m_columnNames << "Check" << "ID" << "Name" << "Email" << "DN";
}

void LdapUserViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

int LdapUserViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int LdapUserViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.count();
}

QVariant LdapUserViewModel::data(const QModelIndex &index, int role) const
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

QHash<int, QByteArray> LdapUserViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"} };
}

void LdapUserViewModel::UpdateSortData(int column, Qt::SortOrder order)
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

void LdapUserViewModel::setHighlightRow(int rowNum, int modifiers)
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

void LdapUserViewModel::setHoveredRow(int rowNum, bool hovered)
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

bool LdapUserViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];

    if(m_data[row][LdapUserID].toString() == "admin")
        return false;

    m_data[row][LdapUserCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, LdapUserCheck)), QModelIndex(index(rowNum, LdapUserCheck)));
    emit selectedRowCountChanged();
    return true;
}

bool LdapUserViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);
    qDebug() << "[LdapUserViewModel] setAllCheckState" << checked;
    for(auto& item : m_index)
        m_data[item][LdapUserCheck] = checked;
    emit dataChanged(QModelIndex(index(0, LdapUserCheck)), QModelIndex(index(m_index.length()-1, LdapUserCheck)));
    emit selectedRowCountChanged();
    return true;
}

int LdapUserViewModel::highlightRow() const
{
    return m_highlightRow;
}

int LdapUserViewModel::selectedRowCount() {
    int ret = 0;
    for (auto& item: m_data) {
        if(item[LdapUserCheck].toBool() == true)
            ret++;
    }
    return ret;
}

QVariant LdapUserViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull())
        return QVariant();

    return cellValue;
}

void LdapUserViewModel::setServerUrl(QUrl serverUrl)
{
    if(serverUrl != m_serverUrl)
    {
        m_serverUrl = serverUrl;
        emit serverUrlChanged();
    }
}

void LdapUserViewModel::setAdminDN(QString adminDN)
{
    if(adminDN != m_adminDN)
    {
        m_adminDN = adminDN;
        emit adminDNChanged();
    }
}

void LdapUserViewModel::setPassword(QString password)
{
    if(password != m_password)
    {
        m_password = password;
        emit passwordChanged();
    }
}

void LdapUserViewModel::setSearchBase(QString searchBase)
{
    if(searchBase != m_searchBase)
    {
        m_searchBase = searchBase;
        emit searchBaseChanged();
    }
}

void LdapUserViewModel::setSearchFilter(QString searchFilter)
{
    if(searchFilter != m_searchFilter)
    {
        m_searchFilter = searchFilter;
        emit searchFilterChanged();
    }
}

QStringList LdapUserViewModel::getUserGroupList()
{
    QStringList userGroupList;

    // 2022.12.27. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return userGroupList;
    }

    SPDLOG_DEBUG("[LdapUserViewModel] getUserGroupList");

    auto userGroups = db->GetUserGroups();

    for (auto& userGroup: userGroups) {
        userGroupList.append(QString::fromStdString(userGroup.second.name));
    }

    userGroupList.sort();
    userGroupList.move(userGroupList.indexOf("administrator"), 0);

    return userGroupList;
}

void LdapUserViewModel::checkLdapServerUrl()
{
    // 2022.12.27. covertity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("[LdapUserViewModel] checkLdapServerUrl()");
    auto prevLdapSetting = db->GetLdapSetting();

    if(prevLdapSetting.serverUrl=="") {
        qDebug() << "LdapUserViewModel::checkLdapServerUrl previous ldap server url is null";
        setLdapConnection();
    }
    else
    {
        qDebug() << "LdapUserViewModel::checkLdapServerUrl previous ldap server url is not null";
        QUrl serverUrl =  m_serverUrl;
        QUrl prevUrl = QString::fromStdString(prevLdapSetting.serverUrl);

        serverUrl.setPort(-1);
        prevUrl.setPort(-1);

        QString urlAuthority = serverUrl.authority();
        QString prevUrlAuthority = prevUrl.authority();
        qDebug() << "LdapUserViewModel::checkLdapServerurl : " << prevUrlAuthority << " : " << urlAuthority;

        if(prevUrlAuthority != urlAuthority) {
            qDebug() << "LdapUserViewModel::checkLdapServerUrl previous ldap server url is different";
            m_isServerChanged = true;
        }
        else {
            qDebug() << "LdapUserViewModel::checkLdapServerUrl previous ldap server url is same";
            m_isServerChanged = false;
        }
        setLdapConnection();
    }
}

void LdapUserViewModel::setLdapConnection()
{
    SPDLOG_DEBUG("[LdapUserViewModel] setLdapConnection()");

    QString urlScheme = m_serverUrl.scheme();

    // clear variables related to LDAP connection and LDAP users
    clear();
    m_searchResults.clear();

    if (urlScheme == "ldap")  // serverUrl의 주소가 ldap://인 플레인 연결일 경우
    {
        makeConnection(false); // LDAP over TLS : a connection that uses port 389
    }
    else if (urlScheme == "ldaps") // serverUrl의 주소가 ldaps://로 시작하는 보안 연결일 경우
    {
        //m_serverUrl.setPort(-1);
        makeConnection(true); // LDAP over SSL : a connection that uses port 636
    }
}

void LdapUserViewModel::makeConnection(bool isSsl)
{
    SPDLOG_DEBUG("[LdapUserViewModel] makeConnection(), isSsl : ", isSsl);

    auto request = std::make_shared<Wisenet::Core::LdapConnectRequest>();
    Wisenet::Core::LdapSetting setting;
    setting.serverUrl = m_serverUrl.toString().toStdString();
    setting.adminDN = m_adminDN.toStdString();
    setting.adminPW = m_password.toStdString();
    setting.searchBase = m_searchBase.toStdString();
    setting.searchFilter = m_searchFilter.toStdString();
    setting.isSsl = isSsl;

    request->ldapSetting = setting;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::LdapConnect,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::LdapConnectResponse>(reply->responseDataPtr);
        SPDLOG_DEBUG("LdapUserViewModel::makeConnection response:{}", response->errorString());

        if(response->isSuccess()){
            if(response->connResult.searchSuccess){
                // Case 1) 서버 연결과 검색 모두 성공했을 때
                emit connCompleted(true, true, m_isServerChanged, response->fetchedUser.size(),
                                   QString::fromStdString(response->connResult.errorMsg));

                m_searchResults.reserve(response->fetchedUser.size());
                std::copy(response->fetchedUser.begin(), response->fetchedUser.end(), std::back_inserter(m_searchResults));
            }
            else{
                if(response->connResult.connSuccess){
                    // Case 2) 서버 연결 성공, 검색 실패
                    emit connCompleted(true, false, m_isServerChanged, 0,
                                       QString::fromStdString(response->connResult.errorMsg));
                }
                else{
                    // Case 3) 서버 연결 실패, 검색 실패
                    emit connCompleted(false, false, m_isServerChanged, 0,
                                       QString::fromStdString(response->connResult.errorMsg));
                }
            }
        }
        else
            SPDLOG_DEBUG("LdapUserViewModel::makeConnection response: failed");
    });
}

void LdapUserViewModel::showLdapUsers()
{
    // 2022.12.27. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("[LdapUserViewModel] showLdapUsers()");

    beginResetModel();
    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;
    int i = 0;

    // 유저 목록에서 user의 id 다 받아오기
    QSet<QString> userIdSet;
    auto users = db->GetUsers();
    for(auto& user : users){
        userIdSet.insert(QString::fromStdString(user.second.loginID));
    }

    for(auto& result : m_searchResults) {
        QVector<QVariant> display;
        display.push_back(false);
        display.push_back(QString::fromStdString(result.uid));
        display.push_back(QString::fromStdString(result.name));
        display.push_back(QString::fromStdString(result.email));
        display.push_back(QString::fromStdString(result.dn));
        display.push_back(userIdSet.contains(QString::fromStdString(result.uid)));

        m_data.push_back(display);
        m_index.push_back(i++);
    }

    endResetModel();
}

void LdapUserViewModel::registerLdapUser(QString userGroupName)
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("[LdapUserViewModel] registerLdapUser()");
    qDebug() << "[LdapUserViewModel] registerLdapUser()" << userGroupName;
    QMap<std::string, Wisenet::uuid_string> duplicateIdMap;

    // 중복된 아이디의 유저 정보 map에 저장 -> 로그인ID : 유저 UUID
    auto users = db->GetUsers();
    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][LdapUserCheck].toBool() == true) {
            for(auto& user : users){
                if(user.second.loginID == m_data[dataIndex][LdapUserID].toString().toStdString())
                    duplicateIdMap.insert(user.second.loginID, user.second.userID);
            }
        }
    }

    // 유저가 선택한 유저 그룹이 admin 그룹인지 확인
    bool isAdminGroup = false;
    std::string userGroupID;
    auto userGroups = db->GetUserGroups();

    for(auto& userGroup : userGroups) {
        if(userGroup.second.name == userGroupName.toStdString()) {
            userGroupID = userGroup.second.userGroupID;
            isAdminGroup = userGroup.second.isAdminGroup;
        }
    }

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][LdapUserCheck].toBool() == false)
            continue;

        auto request = std::make_shared<Wisenet::Core::SaveUserRequest>();
        Wisenet::Core::User user;

        // 동일한 아이디를 가진 WNV 유저가 이미 있을 경우 맵에서 UUID 찾아서 해당 값으로 세이브 요청
        if(duplicateIdMap.find(m_data[dataIndex][LdapUserID].toString().toStdString()) != duplicateIdMap.end())
            user.userID = duplicateIdMap.value(m_data[dataIndex][LdapUserID].toString().toStdString());
        else
            user.userID = boost::uuids::to_string(boost::uuids::random_generator()());

        user.loginID = m_data[dataIndex][LdapUserID].toString().toStdString();
        user.name  =  m_data[dataIndex][LdapUserName].toString().toStdString();
        user.email =  m_data[dataIndex][LdapUserEmail].toString().toStdString();
        user.userGroupID = userGroupID;
        if(isAdminGroup)
            user.userType = Wisenet::Core::User::UserType::Admin;
        else
            user.userType = Wisenet::Core::User::UserType::Custom;
        user.ldapUser = true;
        user.dn = m_data[dataIndex][LdapUserDN].toString().toStdString();

        qDebug() << "[LdapUserViewModel] user.loginID" << m_data[dataIndex][LdapUserID].toString();
        request->user = user;

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::SaveUser,
                    this, request,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
            if(response->isSuccess()) {
                emit resultMessage(true, "Success");
            }
            else {
                emit resultMessage(false, QString::fromStdString(response->errorString()));
            }
            qDebug() << "LdapUserViewModel::registerLdapUser() response:" << QString::fromStdString(response->errorString());
        });
    }
}

void LdapUserViewModel::clear()
{
    SPDLOG_DEBUG("[LdapUserViewModel] clear()");
    // clear variables related to LDAP connection and LDAP users
    beginResetModel();
    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;
    endResetModel();
}

void LdapUserViewModel::updateLdapSetting()
{
    SPDLOG_DEBUG("[LdapUserViewModel] updateLdapSetting Start, isServerChanged", m_isServerChanged);

    auto updateRequest = std::make_shared<Wisenet::Core::UpdateLdapSettingRequest>();

    updateRequest->ldapSetting.serverUrl = m_serverUrl.toString().toStdString();
    updateRequest->ldapSetting.adminDN = m_adminDN.toStdString();
    updateRequest->ldapSetting.searchBase = m_searchBase.toStdString();
    updateRequest->ldapSetting.searchFilter = m_searchFilter.toStdString();
    updateRequest->ldapSetting.isServerChanged = m_isServerChanged;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::UpdateLdapSetting,
                this, updateRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "LdapUserViewModel::UpdateLdapSetting() -> UpdateLdapSetting response:" << QString::fromStdString(response->errorString());

        if(response->isSuccess()) {
            SPDLOG_DEBUG("[LdapUserViewModel] UpdateLdapSettingRequest Success.");
            emit resultMessage(true, "Success");
            showLdapUsers();
        }
        else {
            SPDLOG_DEBUG("[LdapUserViewModel] UpdateLdapSettingRequest Failed.");
            emit resultMessage(false, QString::fromStdString(response->errorString()));
        }
    });

    if(m_isServerChanged) {
        auto request = std::make_shared<Wisenet::Core::RemoveUsersRequest>();
        auto users = QCoreServiceManager::Instance().DB()->GetUsers();

        for(auto& user : users) {
            if(user.second.ldapUser)
                request->userIDs.push_back(user.second.userID);
        }

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::RemoveUsers,
                    this, request,
                    [=](const QCoreServiceReplyPtr& reply)
        {
            auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
            qDebug() << "UserListViewModel::deleteLdapUser() response:" << QString::fromStdString(response->errorString());

            if(response->isSuccess()){
                SPDLOG_DEBUG("[LdapUserViewModel] RemoveUsersRequest Success.");
                emit resultMessage(true, "Success");
                showLdapUsers();
            }
            else{
                SPDLOG_DEBUG("[LdapUserViewModel] RemoveUsersRequest Failed.");
                emit resultMessage(false, QString::fromStdString(response->errorString()));
            }
        });
    }
    else
        showLdapUsers();
}

void LdapUserViewModel::loadLdapSetting()
{
    // 2022.12.27. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    SPDLOG_DEBUG("[LdapUserViewModel] loadLdapSetting()");

    auto ldapSetting = db->GetLdapSetting();
    m_serverUrl = QString::fromStdString(ldapSetting.serverUrl);
    m_adminDN = QString::fromStdString(ldapSetting.adminDN);
    m_searchBase = QString::fromStdString(ldapSetting.searchBase);
    m_searchFilter = QString::fromStdString(ldapSetting.searchFilter);

    qDebug() << "LdapUserViewModel::loadLdapSetting()" << m_serverUrl << m_adminDN << m_searchBase << m_searchFilter;
}

bool LdapUserViewModel::checkDuplicateUser()
{
    SPDLOG_DEBUG("[LdapUserViewModel] checkDuplicateUser()");

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][LdapUserCheck].toBool() && m_data[dataIndex][LdapDuplicateID].toBool())
            return true;
    }
    return false;
}

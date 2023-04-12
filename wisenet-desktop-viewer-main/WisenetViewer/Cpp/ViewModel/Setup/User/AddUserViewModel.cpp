#include "AddUserViewModel.h"
#include "DigestAuthenticator.h"
#include "LogSettings.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>
#include <set>


AddUserViewModel::AddUserViewModel(QObject* parent) : m_userGuid("")
{
    Q_UNUSED(parent);

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &AddUserViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

AddUserViewModel::~AddUserViewModel()
{
    qDebug() << "~AddUserViewModel()";
}

void AddUserViewModel::setUserGuid(const QString &guid)
{
    SPDLOG_DEBUG("AddUserViewModel::setUserGuid:{}", guid.toStdString());
    if(guid != m_userGuid){
        m_userGuid = guid;
        emit this->userGuidChanged();
    }
}

void AddUserViewModel::setLoginId(const QString &loginId)
{
    SPDLOG_DEBUG("AddUserViewModel::setLoginId Name:{}", loginId.toStdString());
    if(loginId != m_loginId){
        m_loginId = loginId;
        emit this->loginIdChanged();
    }
}

void AddUserViewModel::setPassword(const QString &password)
{
    SPDLOG_DEBUG("AddUserViewModel::setPassword");
    if(password != m_password){
        m_password = password;
        emit this->passwordChanged();
    }
}

void AddUserViewModel::setName(const QString &name)
{
    SPDLOG_DEBUG("AddUserViewModel::setName:{}", name.toStdString());
    if(name != m_name){
        m_name = name;
        emit this->nameChanged();
    }
}

void AddUserViewModel::setDescription(const QString &description)
{
    //SPDLOG_DEBUG("AddUserViewModel::setDescription:{}", description.toStdString());
    if(description != m_description){
        m_description = description;
        emit this->descriptionChanged();
    }
}

void AddUserViewModel::setGroupId(const QString &groupId)
{
    //SPDLOG_DEBUG("AddUserViewModel::setGroupName:{}", groupId.toStdString());
    if(groupId != m_groupId){
        m_groupId = groupId;
        emit this->groupIdChanged();
    }
}

void AddUserViewModel::setEmail(const QString& email)
{
    //SPDLOG_DEBUG("AddUserViewModel::setEmail:{}", email.toStdString());
    if(email != m_email){
        m_email = email;
        emit this->emailChanged();
    }
}

void AddUserViewModel::setIsOwner(bool isOwner)
{
    //SPDLOG_DEBUG("AddUserViewModel::setIsOwner:{}", isOwner);
    if(isOwner != m_isOwner){
        m_isOwner = isOwner;
        emit this->isOwnerChanged();
    }
}

void AddUserViewModel::setDN(const QString& dn)
{
    SPDLOG_DEBUG("AddUserViewModel::setDN:{}", dn.toStdString());
    if(dn != m_dn){
        m_dn = dn;
        emit this->dnChanged();
    }
}

void AddUserViewModel::setIsLdapUser(bool isLdapUser)
{
    SPDLOG_DEBUG("AddUserViewModel::setIsLdapUser:{}", isLdapUser);
    if(isLdapUser != m_isLdapUser){
        m_isLdapUser = isLdapUser;
        emit this->isLdapUserChanged();
    }
}

void AddUserViewModel::readUser(QString id)
{
    SPDLOG_DEBUG("AddUserViewModel::readUser ID:{}", id.toStdString());

    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    if (db != nullptr) {
        auto users = db->GetUsers();
        auto user = users.find(id.toStdString());
        if(user == users.end() || id =="") {
            setUserGuid(QString::fromStdString(boost::uuids::to_string(boost::uuids::random_generator()())));
            setLoginId("");
            setPassword("");
            setName("");
            setDescription("");
            setIsOwner(false);
            setGroupId("");
            setEmail("");
            setIsLdapUser(false);
            return;
        }

        SPDLOG_DEBUG("AddUserViewModel::readUser Name:{}", user->second.name);
        setUserGuid(QString::fromStdString(user->first));
        setLoginId(QString::fromStdString(user->second.loginID));
        setPassword(QString::fromStdString(user->second.password));
        setName(QString::fromStdString(user->second.name));
        setDescription(QString::fromStdString(user->second.description));
        setIsOwner(user->second.userType == Wisenet::Core::User::UserType::Owner?true:false);
        setGroupId(QString::fromStdString(user->second.userGroupID));
        setEmail(QString::fromStdString(user->second.email));
        setIsLdapUser(user->second.ldapUser);
        setDN(QString::fromStdString(user->second.dn));
    }
}

void AddUserViewModel::saveUser()
{
    SPDLOG_DEBUG("AddUserViewModel::saveUser Name:{}, isLdap:{}", m_name.toStdString(), m_isLdapUser);

    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    std::map<std::string, Wisenet::Core::User> users;
    if (db != nullptr) {
        users = db->GetUsers();
    }

    for(auto& user : users){
        if(user.second.loginID == m_loginId.toStdString() && user.first != m_userGuid.toStdString()){
            emit resultMessage("Exist user name");
            return;
        }
    }

    auto userGroups = db->GetUserGroups();
    auto userGroup = userGroups.find(m_groupId.toStdString());

    auto request = std::make_shared<Wisenet::Core::SaveUserRequest>();

    Wisenet::Core::User user;
    user.userID = m_userGuid.toStdString();
    user.loginID = m_loginId.toStdString();
    user.password = DigestAuthenticator::hashSpecialSha256(user.userID, m_password.toStdString());
    user.name = m_name.toStdString();
    user.description = m_description.toStdString();
    user.userGroupID = m_groupId.toStdString();
    user.email = m_email.toStdString();
    user.userType = user.loginID=="admin"?Wisenet::Core::User::UserType::Owner:
                    userGroup->second.isAdminGroup?Wisenet::Core::User::UserType::Admin:
                                Wisenet::Core::User::UserType::Custom;
    user.ldapUser = m_isLdapUser;
    user.dn = m_dn.toStdString();
    request->user = user;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveUser,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(response->isSuccess())
            emit resultMessage("Success");
        else
            emit resultMessage(QString::fromStdString(response->errorString()));
        qDebug() << "AddUserViewModel::saveUser() response:" << QString::fromStdString(response->errorString());
    });
}


bool AddUserViewModel::isDuplicateLoginId(QString loginId)
{
    SPDLOG_DEBUG("AddUserViewModel::isDuplicateLoginId Name:{}", loginId.toStdString());

    if (loginId.length() == 0) {
        return false;
    }

    // 2023.01.03. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return false;
    }

    std::map<std::string, Wisenet::Core::User> users;
    if (db != nullptr) {
        users = db->GetUsers();
    }

    for (auto& user : users) {
        if (user.second.loginID == loginId.toStdString() && user.first != m_userGuid.toStdString()) {
            SPDLOG_DEBUG("AddUserViewModel::isDuplicateLoginId id:{} found", loginId.toStdString());
            return true;
        }
    }

    SPDLOG_DEBUG("AddUserViewModel::isDuplicateLoginId id:{} can'tfound", loginId.toStdString());
    return false;
}

void AddUserViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveUserEventType:
    case Wisenet::Core::RemoveUsersEventType:
    case Wisenet::Core::SaveUserGroupEventType:
    case Wisenet::Core::RemoveUserGroupsEventType:
    case Wisenet::Core::SaveLayoutEventType:
    case Wisenet::Core::RemoveLayoutsEventType:
        readUser(m_userGuid);
    }
}







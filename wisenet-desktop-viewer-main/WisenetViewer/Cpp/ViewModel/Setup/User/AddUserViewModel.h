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
#pragma once
#include <QAbstractListModel>
#include "QCoreServiceManager.h"
#include "AddUserGroupViewModel.h"

class AddUserViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString userGuid READ userGuid WRITE setUserGuid NOTIFY userGuidChanged)
    Q_PROPERTY(QString loginId READ loginId WRITE setLoginId NOTIFY loginIdChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString email READ email WRITE setEmail NOTIFY emailChanged)
    Q_PROPERTY(QString description READ description WRITE setDescription NOTIFY descriptionChanged)
    Q_PROPERTY(bool isOwner READ isOwner WRITE setIsOwner NOTIFY isOwnerChanged)
    Q_PROPERTY(QString groupId READ groupId WRITE setGroupId NOTIFY groupIdChanged)
    Q_PROPERTY(bool isLdapUser READ isLdapUser WRITE setIsLdapUser NOTIFY isLdapUserChanged)
    Q_PROPERTY(QString dn READ dn WRITE setDN NOTIFY dnChanged)

public:
    AddUserViewModel(QObject* parent = nullptr);
    ~AddUserViewModel();

    Q_INVOKABLE void readUser(QString userId);
    Q_INVOKABLE void saveUser();
    Q_INVOKABLE bool isDuplicateLoginId(QString loginId);

    //getter
    QString userGuid() { return m_userGuid;}
    QString loginId() { return m_loginId;}
    QString password() { return m_password;}
    QString name() { return m_name;}
    QString email() { return m_email; }
    QString description() { return m_description;}
    bool isOwner() { return m_isOwner;}
    bool isLdapUser() {return m_isLdapUser;}
    QString dn() {return m_dn;}

    QString groupId() { return m_groupId;}

    //setter
    void setUserGuid(const QString& userId);
    void setLoginId(const QString& loginId);
    void setPassword(const QString& password);
    void setName(const QString& name);
    void setEmail(const QString& email);
    void setDescription(const QString& description);
    void setIsOwner(bool isOwner);
    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void setGroupId(const QString& groupId);
    void setIsLdapUser(bool isLdapUser);
    void setDN(const QString& dn);

signals:
    void userGuidChanged();
    void loginIdChanged();
    void passwordChanged();
    void nameChanged();
    void emailChanged();
    void descriptionChanged();
    void isOwnerChanged();
    void resultMessage(QString msg);
    void groupIdChanged();
    void dnChanged();
    void isLdapUserChanged();

private:
    QString m_userGuid;
    QString m_loginId;
    QString m_password;
    QString m_name;
    QString m_description;
    QString m_email;
    bool    m_isOwner = false;
    QString m_groupId;
    bool    m_isLdapUser = false;
    QString m_dn;
};


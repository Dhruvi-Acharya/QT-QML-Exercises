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
#include "UserTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"
#include "DigestAuthenticator.h"

namespace Wisenet
{
namespace Core
{

UserTable::UserTable(QSqlDatabase& database,QString& key)
    :m_database(database)
{
    m_crypto.Init(key);
}


void UserTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_USER"))) {
        SPDLOG_INFO("[T_USER] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_USER]( \
                [USER_ID]VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY\
                ,[LOGIN_ID]VARCHAR(40) NOT NULL UNIQUE\
                ,[PASSWORD]VARCHAR(128) NOT NULL\
                ,[USER_GROUP_ID]VARCHAR(40) \
                ,[NAME]VARCHAR(40) \
                ,[USER_TYPE]INTEGER NOT NULL DEFAULT 0 \
                ,[DESCRIPTION]VARCHAR(256) \
                ,[EMAIL]VARCHAR(256) \
                ,[LDAP]BOOL DEFAULT FALSE\
                ,[DN] TEXT\
                   )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserTable::GetAll(std::map<std::string, User>& users)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_USER]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        User user;
        user.userID = query.value(0).toString().toStdString();
        user.loginID = query.value(1).toString().toStdString();

        //user.password = m_crypto.Decrypt(query.value(2).toByteArray()).toStdString();
        user.password = query.value(2).toString().toStdString();

        user.userGroupID = query.value(3).toString().toStdString();
        user.name = query.value(4).toString().toStdString();
        user.userType = static_cast<User::UserType>(query.value(5).toInt());
        user.description = query.value(6).toString().toStdString();
        user.email = query.value(7).toString().toStdString();
        user.ldapUser = query.value(8).toBool();
        user.dn = query.value(9).toString().toStdString();
        users.emplace(user.userID, user);
        SPDLOG_DEBUG("userID={} {} ", user.userID, user.loginID);
    }

}

void UserTable::Save(const User& user)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_USER] \
            (USER_ID,LOGIN_ID,PASSWORD,USER_GROUP_ID,NAME,USER_TYPE,DESCRIPTION,EMAIL,LDAP,DN) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(user.userID.c_str()));
    query.addBindValue(QString(user.loginID.c_str()));

    //query.addBindValue(m_crypto.Encrypt(QString(user.password.c_str())));
    query.addBindValue(QString(user.password.c_str()));

    query.addBindValue(QString(user.userGroupID.c_str()));
    query.addBindValue(QString(user.name.c_str()));
    query.addBindValue(static_cast<int>(user.userType));
    query.addBindValue(QString(user.description.c_str()));
    query.addBindValue(QString(user.email.c_str()));
    query.addBindValue(user.ldapUser);
    query.addBindValue(QString(user.dn.c_str()));
    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserTable::Remove(const uuid_string& userID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_USER] WHERE USER_ID=?");
    query.addBindValue(QString(userID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void UserTable::RemoveByUserGroupID(const uuid_string &userGroupID, std::vector<std::string> &removedUserIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_USER] WHERE USER_GROUP_ID=?");
    query.addBindValue(QString(userGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        removedUserIDs.emplace_back(query.value(0).toString().toStdString());
    }

    if(removedUserIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_USER] WHERE USER_GROUP_ID=?");
    query.addBindValue(QString(userGroupID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

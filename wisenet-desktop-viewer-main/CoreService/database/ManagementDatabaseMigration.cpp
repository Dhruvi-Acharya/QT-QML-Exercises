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
#include "ManagementDatabaseMigration.h"

#include <QDateTime>
#include <QNetworkInterface>
#include <QString>
#include <QSysInfo>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"
#include "DigestAuthenticator.h"
#include "ManagementDatabase.h"

namespace Wisenet
{
namespace Core
{

ManagementDatabaseMigration::ManagementDatabaseMigration(QSqlDatabase& database)
    :m_database(database)
{
    m_crypto.Init(QString("#!WisenetViewer_v1.0.0!#"));
    // MigrationFunction vector를 생성한다.
    m_migrationHandlers.push_back(std::bind(&ManagementDatabaseMigration::toV1, this));
    m_migrationHandlers.push_back(std::bind(&ManagementDatabaseMigration::toV2, this));
    m_migrationHandlers.push_back(std::bind(&ManagementDatabaseMigration::toV3, this));
    m_migrationHandlers.push_back(std::bind(&ManagementDatabaseMigration::toV4, this));
    m_migrationHandlers.push_back(std::bind(&ManagementDatabaseMigration::toV5, this));
}

void ManagementDatabaseMigration::MigrateFrom(int fromVersion)
{
    SPDLOG_INFO("[LocalService] Start migration from Version({}) for the management database.",fromVersion);
    //vector index는 version - 1 입니다.
    //fromVersion 부터 시작을 하면 바로 다음 버전의 마이그레이션 함수를 호출하는 것임.
    for(size_t i = fromVersion ; i < m_migrationHandlers.size(); i++){
        m_migrationHandlers[i]();
    }
}

void ManagementDatabaseMigration::toV1()
{
    // version 0 에서 version 1으로 Migration 되어야 하는 내용 추가.
    QSqlQuery query(m_database);

    // 1. T_VER 테이블 추가 및 VER 필드에 1값 추가.
    if(!query.exec("CREATE TABLE [T_VER]([VER]INTEGER)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    if(!query.exec("INSERT INTO [T_VER] VALUES(1)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 2. T_CHANNEL 테이블
    // 2.1 FISHEYE_LENS_TYPE 컬럼 추가
    if(!query.exec("ALTER TABLE [T_CHANNEL] ADD COLUMN [FISHEYE_LENS_TYPE] VARCHAR(40)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 3. T_LAYOUT_ITEM 테이블
    // 3.1 FISHEYE_DEWARP_ENABLE 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_DEWARP_ENABLE] BOOLEAN DEFAULT 0")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // 3.2 FISHEYE_VIEW_MODE 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_VIEW_MODE] INTEGER NOT NULL DEFAULT 0")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // 3.3 FISHEYE_VIEW_PARAM 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_VIEW_PARAM] TEXT")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // 3.4 FISHEYE_FILE_ENABLE 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_FILE_ENABLE] BOOLEAN DEFAULT 0")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // 3.5 FISHEYE_FILE_LENSTYPE 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_FILE_LENSTYPE] VARCHAR(40)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    // 3.6 FISHEYE_FILE_LOCATION 컬럼 추가
    if(!query.exec("ALTER TABLE [T_LAYOUT_ITEM] ADD COLUMN [FISHEYE_FILE_LOCATION] INTEGER NOT NULL DEFAULT 0")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 4. T_BOOKMARK 테이블
    // 4.1 EVENT_LOG_ID 컬럼 추가
    if(!query.exec("ALTER TABLE [T_BOOKMARK] ADD COLUMN [EVENT_LOG_ID] VARCHAR(40)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    SPDLOG_INFO("[LocalService] Migrated to version 1 for the management database.");
}

void ManagementDatabaseMigration::toV2()
{
    // version1 에서 version2로 Migration 되어야 하는 내용 추가.
    QSqlQuery query(m_database);

    // 1. T_DEVICE 테이블
    // 1.1  컬럼 추가
    if(!query.exec("ALTER TABLE [T_DEVICE] ADD COLUMN [USAGE_DTLS_MODE] BOOLEAN")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 2. T_VER 테이블 추가 및 VER 필드에 값 추가. (2 == 1.1.1 의미)
    if(!query.exec("UPDATE [T_VER] SET VER = 2 WHERE VER = 1")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    SPDLOG_INFO("[LocalService] Migrated to version 1.1.1 for the management database.");
}

void ManagementDatabaseMigration::toV3()
{
    // version2 에서 version3 으로 Migration 되어야 하는 내용 추가.
    QSqlQuery query(m_database);

    // 1. T_USER 테이블
    // 1.1 EMAIL 컬럼 추가
    if(!query.exec("ALTER TABLE [T_USER] ADD COLUMN [EMAIL] VARCHAR(100)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QString emailQuery = "";

    emailQuery += "CREATE TABLE [T_EMAIL]( ";
    emailQuery += "[SENDER] VARCHAR(128) NOT NULL UNIQUE PRIMARY KEY ";
    emailQuery += ",[RECEIVER] VARCHAR(128)";
    emailQuery += ",[SERVER_ADDRESS] VARCHAR(128)";
    emailQuery += ",[ID] VARCHAR(128)";
    emailQuery += ",[PASSWORD] VARCHAR(128)";
    emailQuery += ",[PORT] VARCHAR(128)";
    emailQuery += ",[AUTHENTICATION]BOOLEAN NOT NULL DEFAULT 1)";

    // 2. T_EMAIL 테이블 추가
    if(!query.exec(emailQuery)) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 3. T_VER 테이블 추가 및 VER 필드에 값 추가. (3 == 1.2.0 의미)
    if(!query.exec("UPDATE [T_VER] SET VER = 3 WHERE VER = 2")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    SPDLOG_INFO("[LocalService] Migrated to version 3 for the management database.");
}

void ManagementDatabaseMigration::toV4()
{
    // version3 에서 version4 으로 Migration 되어야 하는 내용 추가.
    QSqlQuery query(m_database);
    if(!query.exec("SELECT * FROM [T_USER]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    std::map<std::string, User> users;

    while(query.next()) {
        User user;
        user.userID = query.value(0).toString().toStdString();
        user.loginID = query.value(1).toString().toStdString();

        user.password = m_crypto.Decrypt(query.value(2).toByteArray()).toStdString();
        user.password = DigestAuthenticator::hashSpecialSha256(user.userID, user.password);

        user.userGroupID = query.value(3).toString().toStdString();
        user.name = query.value(4).toString().toStdString();
        user.userType = static_cast<User::UserType>(query.value(5).toInt());
        user.description = query.value(6).toString().toStdString();
        user.email = query.value(7).toString().toStdString();
        users.emplace(user.userID, user);
    }

    QString queryString = "";

    queryString += "INSERT OR REPLACE INTO [T_USER] ";
    queryString += "(USER_ID,LOGIN_ID,PASSWORD,USER_GROUP_ID,NAME,USER_TYPE,DESCRIPTION,EMAIL) ";
    queryString += "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    for(auto& user : users){
        query.prepare(queryString);
        query.addBindValue(QString(user.second.userID.c_str()));
        query.addBindValue(QString(user.second.loginID.c_str()));

        //query.addBindValue(m_crypto.Encrypt(QString(user.password.c_str())));
        query.addBindValue(QString(user.second.password.c_str()));

        query.addBindValue(QString(user.second.userGroupID.c_str()));
        query.addBindValue(QString(user.second.name.c_str()));
        query.addBindValue(static_cast<int>(user.second.userType));
        query.addBindValue(QString(user.second.description.c_str()));
        query.addBindValue(QString(user.second.email.c_str()));
        if(!query.exec()) {
            throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
        }
    }
    // 1. T_USER 테이블
    // 1.1 LDAP 컬럼 추가
    if(!query.exec("ALTER TABLE [T_USER] ADD COLUMN [LDAP] BOOL")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 1.2 DN 컬럼 추가
    if(!query.exec("ALTER TABLE [T_USER] ADD COLUMN [DN] TEXT")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 2. T_LDAP 테이블 추가
    if(!query.exec("CREATE TABLE [T_LDAP]([SERVER_URL] TEXT, [ADMIN_DN] TEXT, [SEARCHBASE] TEXT, [SEARCHFILTER] TEXT)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 3. T_VER 테이블 추가 및 VER 필드에 값 추가. (3 == 1.2.0 의미)
    if(!query.exec("UPDATE [T_VER] SET VER = 4 WHERE VER = 3")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void ManagementDatabaseMigration::toV5()
{
    // 1. T_STATISTICS
    // 1.1. T_STATISTICS 테이블 생성
    QSqlQuery query(m_database);

    QString queryString = "";
    queryString += "CREATE TABLE [T_STATISTICS] (";
    queryString += "[MACHINE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY";
    queryString += ",[INSTALLED_UTC] INTEGER NOT NULL ";
    queryString += ",[EXECUTION_COUNT] INTEGER NOT NULL ";
    queryString += ",[EXECUTION_MINUTES] INTEGER NOT NULL ";
    queryString += ",[LAYOUT_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[CHANNEL_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[MEDIA_OPEN_COUNT] INTEGER NOT NULL ";
    queryString += ",[AVERAGE_RESPONSE_TIME] INTEGER NOT NULL)";

    if(!query.exec(queryString))
    {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    QString macAddress = "";
    foreach (QNetworkInterface interface, interfaces)
    {
        if(interface.hardwareAddress() != "")
        {
            macAddress = interface.hardwareAddress();
            break;
        }
    }

    // 1.2. T_STATISTICS 테이블 값 추가
    QSqlQuery insertQuery(m_database);
    QString firstInsertQueryString = "";
    firstInsertQueryString += "INSERT OR REPLACE INTO [T_STATISTICS] ";
    firstInsertQueryString += "(MACHINE_ID,INSTALLED_UTC,EXECUTION_COUNT,EXECUTION_MINUTES,LAYOUT_OPEN_COUNT,CHANNEL_OPEN_COUNT,MEDIA_OPEN_COUNT,AVERAGE_RESPONSE_TIME) ";
    firstInsertQueryString += "VALUES (?, ?, ?, ?, ?, ?, ?, ?)";

    insertQuery.prepare(firstInsertQueryString);

    insertQuery.addBindValue(macAddress);
    insertQuery.addBindValue(QDateTime::currentDateTime().toSecsSinceEpoch());
    insertQuery.addBindValue(1);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);
    insertQuery.addBindValue(0);

    if(!insertQuery.exec())
    {
        throw ManagementDatabaseException(insertQuery.lastQuery().toStdString(), insertQuery.lastError());
    }

    // 2. T_DEVICE
    if(!query.exec("ALTER TABLE [T_DEVICE] ADD COLUMN [REGISTRATION_MSEC] INTEGER")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 3. T_LICENSE
    QString licenseQueryString = "CREATE TABLE [T_LICENSE] ([LICENSE_KEY] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY)";

    if(!query.exec(licenseQueryString))
    {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // 4. T_VER 테이블 추가 및 VER 필드에 값 추가. (5 == 1.4.0 의미)
    if(!query.exec("UPDATE [T_VER] SET VER = 5 WHERE VER = 4")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

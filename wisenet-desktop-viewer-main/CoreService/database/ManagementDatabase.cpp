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
#include "database/ManagementDatabase.h"

#include <QStandardPaths>
#include <QVariant>
#include <QFile>

#include "database/ManagementDatabaseMigration.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{

#ifdef WISENET_DEVELOP_VERSION
QString ManagementDatabase::FILENAME = "management.dev.sqlite";
#else
QString ManagementDatabase::FILENAME = "management.sqlite";
#endif
QString ManagementDatabase::KEY = "#!WisenetViewer_v1.0.0!#";

const int ManagementDatabase::DB_LATEST_VERSION;

ManagementDatabase::ManagementDatabase()
    :m_path("")
    ,m_database()
    ,m_cloudTable(m_database,KEY)
    ,m_userTable(m_database,KEY)
    ,m_userGroupTable(m_database)
    ,m_userGroupResourceTable(m_database)
    ,m_groupTable(m_database)
    ,m_groupMappingTable(m_database)
    ,m_sequenceTable(m_database)
    ,m_sequenceItemTable(m_database)
    ,m_layoutTable(m_database)
    ,m_layoutItemTable(m_database)
    ,m_serviceTable(m_database)
    ,m_fileTable(m_database)
    ,m_deviceTable(m_database,KEY)
    ,m_channelTable(m_database)
    ,m_webPageTable(m_database)
    ,m_bookmarkTable(m_database)
    ,m_eventRuleTable(m_database)
    ,m_eventScheduleTable(m_database)
    ,m_eventEmailTable(m_database,KEY)
    ,m_serviceSettinsTable(m_database)
    ,m_ldapTable(m_database)
    ,m_statisticsTable(m_database)
    ,m_licenseTable(m_database)
{

}

bool ManagementDatabase::OpenClose(QString &path)
{
    auto localPath = path + "/" + ManagementDatabase::FILENAME;
    auto database = QSqlDatabase::addDatabase("QSQLITE",QString("MANAGEMENT_TEMP"));
    database.setDatabaseName(localPath);

    SPDLOG_INFO("[LocalService] A management OpenClose database start:{}", localPath.toStdString());

    if(!database.open()) {
        QSqlError error = database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to open a management database.path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
        QSqlDatabase::removeDatabase(QString("MANAGEMENT_TEMP"));
        return false;
    }

    database.exec(QString("pragma synchronous=NORMAL"));
    database.exec(QString("pragma journal_mode = WAL"));

    database.close();
    database = {};

    QSqlDatabase::removeDatabase(QString("MANAGEMENT_TEMP"));
    SPDLOG_INFO("[LocalService] A management OpenClose database end:{}", localPath.toStdString());

    return true;
}


bool ManagementDatabase::Open(QString &path)
{
    m_path = path + "/" + ManagementDatabase::FILENAME;

    // 1. 파일이 존재하는지 확인한다.
    bool needToCreate = false;
    if(!QFile::exists(m_path)){
        needToCreate = true;
        SPDLOG_INFO("[LocalService] Need to create a manaement database. Path:{}", m_path.toLocal8Bit().toStdString());
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE",QString("MANAGEMENT"));

    m_database.setDatabaseName(m_path);

    SPDLOG_INFO("[LocalService] A management database Path:{}", m_path.toLocal8Bit().toStdString());

    if(!m_database.open()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to open a management database.path={} Massage={}({}).", m_path.toLocal8Bit().toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
        QSqlDatabase::removeDatabase(QString("MANAGEMENT"));
        return false;
    }

    // 저널 모드를 WAL 모드로 설정.
    m_database.exec(QString("pragma synchronous=NORMAL"));
    m_database.exec(QString("pragma journal_mode = WAL"));

    auto result = false;

    if(needToCreate){
        result = CreateTable();
    }else{
        result = MigrateTable();
    }

    return result;
}

void ManagementDatabase::Close()
{
    m_database.close();
    m_database = {};

    QSqlDatabase::removeDatabase(QString("MANAGEMENT"));

    SPDLOG_INFO("[LocalService] Closed a management database. path={}", m_path.toLocal8Bit().toStdString());
}

bool ManagementDatabase::CreateTable()
{
    try
    {
        SPDLOG_INFO("[LocalService] Create all table in a management database.");

        Transaction();

        CreateVersionTable();

        m_cloudTable.CreateTable();
        m_userTable.CreateTable();
        m_userGroupTable.CreateTable();
        m_userGroupResourceTable.CreateTable();
        m_groupTable.CreateTable();
        m_groupMappingTable.CreateTable();
        m_sequenceTable.CreateTable();
        m_sequenceItemTable.CreateTable();
        m_layoutTable.CreateTable();
        m_layoutItemTable.CreateTable();
        m_serviceTable.CreateTable();
        m_fileTable.CreateTable();
        m_deviceTable.CreateTable();
        m_channelTable.CreateTable();
        m_webPageTable.CreateTable();
        m_bookmarkTable.CreateTable();
        m_eventScheduleTable.CreateTable(); // schedule table은 rule table 보다 먼저 생성되어야 한다.
        m_eventEmailTable.CreateTable();
        m_ldapTable.CreateTable();
        m_statisticsTable.CreateTable();
        m_licenseTable.CreateTable();
        auto alwaysScheduleID = m_eventScheduleTable.GetAlwaysSchedule();

        m_eventRuleTable.CreateTable(alwaysScheduleID);

        m_serviceSettinsTable.CreateTable();

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to create all table of management database. path={}", m_path.toLocal8Bit().toStdString());

    }catch(const ManagementDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

bool ManagementDatabase::MigrateTable()
{
    try
    {
        int currentVersion = 0;
        if(!m_database.tables().contains(QLatin1String("T_VER"))) {
            // Wisenet Viewer 1.0.0에서 T_VER 테이블이 추가되지 않음.
            SPDLOG_INFO("[LocalService] database version(0)");
            currentVersion = 0;
        }else{
            //T_VER 테이블에서 버전 확인
            QSqlQuery query(m_database);

            if(!query.exec("SELECT * FROM [T_VER]")){
                QSqlError error = query.lastError();
                SPDLOG_ERROR("[LocalService] Failed to query from a management database.path={} Message={}({})",m_path.toLocal8Bit().toStdString(),error.text().toStdString(),error.nativeErrorCode().toStdString());
                return false;
            }
            if(!query.next()){
                SPDLOG_ERROR("[LocalService] Failed to query a version.");
                return false;
            }
            currentVersion = query.value(0).toInt();
            SPDLOG_INFO("[LocalService] database version({})",currentVersion);
        }

        if(currentVersion == DB_LATEST_VERSION){
            SPDLOG_INFO("[LocalService] a Management database is latest version.");
            return true;
        }

        //Migration 실시.
        Transaction();

        ManagementDatabaseMigration databaseMigration(m_database);
        databaseMigration.MigrateFrom(currentVersion);

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to migrate all table of the management database. path={}", m_path.toLocal8Bit().toStdString());

    }catch(const ManagementDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

void ManagementDatabase::CreateVersionTable()
{
    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_VER]([VER]INTEGER)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QSqlQuery insertQuery(m_database);
    insertQuery.prepare("INSERT INTO [T_VER] VALUES(?)");
    insertQuery.addBindValue(QVariant::fromValue(DB_LATEST_VERSION));

    if(!insertQuery.exec()) {
        throw ManagementDatabaseException(insertQuery.lastQuery().toStdString(), insertQuery.lastError());
    }
}

void ManagementDatabase::Transaction()
{
    if(!m_database.transaction()) {
        throw ManagementDatabaseException(m_database.lastError());
    }
}

void ManagementDatabase::Commit()
{
    if(!m_database.commit()) {
        throw ManagementDatabaseException(m_database.lastError());
    }
}

void ManagementDatabase::Rollback()
{
    if(!m_database.rollback()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to rollback a management database. path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
    }
}

}
}

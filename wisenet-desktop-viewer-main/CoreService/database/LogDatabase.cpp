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
#include "LogDatabase.h"

#include <QStandardPaths>
#include <QVariant>
#include <QFile>

#include "LogDatabaseMigration.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{
#ifdef WISENET_DEVELOP_VERSION
QString LogDatabase::FILENAME = "log.dev.sqlite";
#else
QString LogDatabase::FILENAME = "log.sqlite";
#endif

const int LogDatabase::DB_LATEST_VERSION;

LogDatabase::LogDatabase()
    :m_path("")
    ,m_database()
    ,m_auditLogTable(m_database)
    ,m_eventLogTable(m_database)
    ,m_systemLogTable(m_database)
    ,m_deletedDeviceTable(m_database)
    ,m_deletedChannelTable(m_database)
{

}

bool LogDatabase::Open(QString &path)
{
    m_path = path + "/" + LogDatabase::FILENAME;

    // 1. 파일이 존재하는지 확인한다.
    bool needToCreate = false;
    if(!QFile::exists(m_path)){
        needToCreate = true;
        SPDLOG_INFO("[LocalService] Need to create a log database. Path:{}", m_path.toLocal8Bit().toStdString());
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE", QString("LOG"));

    m_database.setDatabaseName(m_path);

    SPDLOG_INFO("[LocalService] A log database Path:{}", m_path.toStdString());

    if(!m_database.open()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to open a log database.path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
        QSqlDatabase::removeDatabase(QString("LOG"));
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

void LogDatabase::Close()
{
    m_database.close();
    m_database = {};

    QSqlDatabase::removeDatabase(QString("LOG"));

    SPDLOG_INFO("[LocalService] Closed a log database. path={}", m_path.toStdString());
}

bool LogDatabase::CreateTable()
{
    try
    {
        Transaction();

        CreateVersionTable();

        // Create Table
        m_auditLogTable.CreateTable();

        m_eventLogTable.CreateTable();

        m_systemLogTable.CreateTable();

        m_deletedDeviceTable.CreateTable();

        m_deletedChannelTable.CreateTable();

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to create all table of management database. path={}", m_path.toStdString());

    }catch(const LogDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

bool LogDatabase::MigrateTable()
{
    try
    {
        int currentVersion = 0;
        if(!m_database.tables().contains(QLatin1String("T_VER"))) {
            // Wisenet Viewer 1.0.0에서 T_VER 테이블이 추가되지 않음.
            SPDLOG_INFO("[LocalService] database version(0) for the log database.");
            currentVersion = 0;
        }else{
            //T_VER 테이블에서 버전 확인
            QSqlQuery query(m_database);

            if(!query.exec("SELECT * FROM [T_VER]")){
                QSqlError error = query.lastError();
                SPDLOG_ERROR("[LocalService] Failed to query from the log database.path={} Message={}({})",m_path.toLocal8Bit().toStdString(),error.text().toStdString(),error.nativeErrorCode().toStdString());
                return false;
            }
            if(!query.next()){
                SPDLOG_ERROR("[LocalService] Failed to query a version for the log database.");
                return false;
            }
            currentVersion = query.value(0).toInt();
            SPDLOG_INFO("[LocalService] log database version({})",currentVersion);
        }

        if(currentVersion == DB_LATEST_VERSION){
            SPDLOG_INFO("[LocalService] the log database is latest version.");
            return true;
        }

        //Migration 실시.
        Transaction();

        LogDatabaseMigration databaseMigration(m_database);
        databaseMigration.MigrateFrom(currentVersion);

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to migrate all table of the log database. path={}", m_path.toLocal8Bit().toStdString());

    }catch(const LogDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

void LogDatabase::CreateVersionTable()
{
    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_VER]([VER]INTEGER)")) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QSqlQuery insertQuery(m_database);
    insertQuery.prepare("INSERT INTO [T_VER] VALUES(?)");
    insertQuery.addBindValue(QVariant::fromValue(DB_LATEST_VERSION));

    if(!insertQuery.exec()) {
        throw LogDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LogDatabase::Transaction()
{
    if(!m_database.transaction()) {
        throw LogDatabaseException(m_database.lastError());
    }
}

void LogDatabase::Commit()
{
    if(!m_database.commit()) {
        throw LogDatabaseException(m_database.lastError());
    }
}

void LogDatabase::Rollback()
{
    if(!m_database.rollback()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to rollback a log database. path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
    }
}

void LogDatabase::Vacuum()
{
    SPDLOG_INFO("[LocalService] Start vacuum.");

    QSqlQuery query = m_database.exec(QString("vacuum"));

    if(!query.isActive()){
        QSqlError error = query.lastError();
        SPDLOG_ERROR("[LocalService] Failed to rollback a log database. path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
    }else{
        SPDLOG_INFO("[LocalService] Finished vacuum sucessfully.");
    }
}

}
}

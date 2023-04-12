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
#include "DashboardDatabase.h"

#include <QStandardPaths>
#include <QVariant>
#include <QFile>

#include "DashboardDatabaseMigration.h"
#include "LogSettings.h"

namespace Wisenet
{
namespace Core
{
#ifdef WISENET_DEVELOP_VERSION
QString DashboardDatabase::FILENAME = "dashboard.dev.sqlite";
#else
QString DashboardDatabase::FILENAME = "dashboard.sqlite";
#endif

const int DashboardDatabase::DB_LATEST_VERSION;

DashboardDatabase::DashboardDatabase() :
    m_dashboardTable(m_database)
{

}

bool DashboardDatabase::Open(QString &path)
{
    m_path = path + "/" + DashboardDatabase::FILENAME;

    // 1. 파일이 존재하는지 확인한다.
    bool needToCreate = false;
    if(!QFile::exists(m_path)){
        needToCreate = true;
        SPDLOG_INFO("[LocalService] Need to create a dashboard database. Path:{}", m_path.toLocal8Bit().toStdString());
    }

    m_database = QSqlDatabase::addDatabase("QSQLITE", QString("DASHBOARD"));

    m_database.setDatabaseName(m_path);

    SPDLOG_INFO("[LocalService] A dashboard database Path:{}", m_path.toStdString());

    if(!m_database.open()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to open a dashboard database.path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
        QSqlDatabase::removeDatabase(QString("DASHBOARD"));
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

void DashboardDatabase::Close()
{
    m_database.close();
    m_database = {};

    QSqlDatabase::removeDatabase(QString("DASHBOARD"));

    SPDLOG_INFO("[LocalService] Closed a dashboard database. path={}", m_path.toStdString());
}

bool DashboardDatabase::CreateTable()
{
    try
    {
        Transaction();

        CreateVersionTable();

        // Create Table
        m_dashboardTable.CreateTable();

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to create all table of management database. path={}", m_path.toStdString());

    }catch(const DashboardDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

bool DashboardDatabase::MigrateTable()
{
    try
    {
        int currentVersion = 0;
        if(!m_database.tables().contains(QLatin1String("T_VER"))) {
            // Wisenet Viewer 1.0.0에서 T_VER 테이블이 추가되지 않음.
            SPDLOG_INFO("[LocalService] database version(0) for the dashboard database.");
            currentVersion = 0;
        }else{
            //T_VER 테이블에서 버전 확인
            QSqlQuery query(m_database);

            if(!query.exec("SELECT * FROM [T_VER]")){
                QSqlError error = query.lastError();
                SPDLOG_ERROR("[LocalService] Failed to query from the dashboard database.path={} Message={}({})",m_path.toLocal8Bit().toStdString(),error.text().toStdString(),error.nativeErrorCode().toStdString());
                return false;
            }
            if(!query.next()){
                SPDLOG_ERROR("[LocalService] Failed to query a version for the dashboard database.");
                return false;
            }
            currentVersion = query.value(0).toInt();
            SPDLOG_INFO("[LocalService] dashboard database version({})",currentVersion);
        }

        if(currentVersion == DB_LATEST_VERSION){
            SPDLOG_INFO("[LocalService] the dashboard database is latest version.");
            return true;
        }

        //Migration 실시.
        Transaction();

        DashboardDatabaseMigration databaseMigration(m_database);
        databaseMigration.MigrateFrom(currentVersion);

        Commit();

        SPDLOG_INFO("[LocalService] Succeed to migrate all table of the dashboard database. path={}", m_path.toLocal8Bit().toStdString());

    }catch(const DashboardDatabaseException& e) {
        Rollback();
        SPDLOG_ERROR("[LocalService] {}", e.what());
        return false;
    }

    return true;
}

void DashboardDatabase::CreateVersionTable()
{
    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_VER]([VER]INTEGER)")) {
        throw DashboardDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    QSqlQuery insertQuery(m_database);
    insertQuery.prepare("INSERT INTO [T_VER] VALUES(?)");
    insertQuery.addBindValue(QVariant::fromValue(DB_LATEST_VERSION));

    if(!insertQuery.exec()) {
        throw DashboardDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void DashboardDatabase::Transaction()
{
    if(!m_database.transaction()) {
        throw DashboardDatabaseException(m_database.lastError());
    }
}

void DashboardDatabase::Commit()
{
    if(!m_database.commit()) {
        throw DashboardDatabaseException(m_database.lastError());
    }
}

void DashboardDatabase::Rollback()
{
    if(!m_database.rollback()) {
        QSqlError error = m_database.lastError();
        SPDLOG_ERROR("[LocalService] Failed to rollback a dashboard database. path={} Massage={}({}).", m_path.toStdString(), error.text().toStdString(),error.nativeErrorCode().toStdString());
    }
}


}
}

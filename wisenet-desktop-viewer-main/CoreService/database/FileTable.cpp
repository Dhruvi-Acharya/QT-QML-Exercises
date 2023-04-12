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
#include "FileTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

FileTable::FileTable(QSqlDatabase& database)
    :m_database(database)
{

}

void FileTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_FILE"))) {
        SPDLOG_INFO("[T_FILE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_FILE]([FILE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY,[PATH] TEXT,[DATA] BLOB)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void FileTable::Get(const std::string &id, File &file)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_FILE] WHERE FILE_ID=?");
    query.addBindValue(QString(id.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        file.fileID = query.value(0).toString().toStdString();
        file.path = query.value(1).toString().toStdString();
        QByteArray data = query.value( 2 ).toByteArray();
        file.data.reserve(data.size());
        file.data.assign(data.constData(), data.constData() + data.size());
    }
}

void FileTable::GetAll(std::map<std::string, File> &files)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_FILE]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        File file;
        file.fileID = query.value(0).toString().toStdString();
        file.path = query.value(1).toString().toStdString();
        QByteArray data = query.value( 2 ).toByteArray();
        file.data.reserve(data.size());
        file.data.assign(data.constData(), data.constData() + data.size());

        files.emplace(file.fileID, file);
    }

}

void FileTable::Save(const File &file)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_FILE] (FILE_ID,PATH,DATA) VALUES (?, ?, ?)");
    query.addBindValue(QString(file.fileID.c_str()));
    query.addBindValue(QString(file.path.c_str()));

    QByteArray data("", 0);

    if(file.data.size() > 0 && file.data.data() != nullptr)
    {
        data = QByteArray(reinterpret_cast<const char*>(file.data.data()),static_cast<int>(file.data.size()));
    }

    query.addBindValue(data);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void FileTable::Remove(const std::string& fileID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_FILE] WHERE FILE_ID=?");
    query.addBindValue(QString(fileID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

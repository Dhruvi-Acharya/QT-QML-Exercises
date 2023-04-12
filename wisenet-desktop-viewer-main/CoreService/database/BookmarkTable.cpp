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
#include "BookmarkTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{


BookmarkTable::BookmarkTable(QSqlDatabase& database)
    :m_database(database)
{

}

void BookmarkTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_BOOKMARK"))) {
        SPDLOG_INFO("[T_BOOKMARK] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_BOOKMARK]( [BOOKMARK_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY, [DEVICE_ID] VARCHAR(40) NOT NULL, [CHANNEL_ID] VARCHAR(40) NOT NULL, [NAME] TEXT NOT NULL, [DESCRIPTION] TEXT, [TAG] TEXT, [START_TIME] INTEGER NOT NULL, [END_TIME] INTEGER NOT NULL, [OVERPAPPED_ID] INTEGER NOT NULL DEFAULT -1, [IMAGE] BLOB, [EVENT_LOG_ID] VARCHAR(40) )"))
    {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void BookmarkTable::GetAll(std::map<uuid_string, Bookmark>& bookmarks)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_BOOKMARK]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Bookmark bookmark;
        bookmark.bookmarkID = query.value(0).toString().toStdString();
        bookmark.deviceID = query.value(1).toString().toStdString();
        bookmark.channelID = query.value(2).toString().toStdString();
        bookmark.name = query.value(3).toString().toStdString();
        bookmark.description = query.value(4).toString().toStdString();
        bookmark.tag = query.value(5).toString().toStdString();
        bookmark.startTime = query.value(6).toLongLong();
        bookmark.endTime = query.value(7).toLongLong();
        bookmark.overlappedID = query.value(8).toInt();
        QByteArray imageByteArray = query.value(9).toByteArray();
        bookmark.image.assign(imageByteArray.begin(), imageByteArray.end());
        bookmark.eventLogID = query.value(10).toString().toStdString();

        bookmarks.emplace(bookmark.bookmarkID, bookmark);
    }
}

void BookmarkTable::Save(const Bookmark& bookmark)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_BOOKMARK] (BOOKMARK_ID, DEVICE_ID, CHANNEL_ID, NAME, DESCRIPTION, TAG, START_TIME, END_TIME, OVERPAPPED_ID, IMAGE, EVENT_LOG_ID) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(bookmark.bookmarkID.c_str()));
    query.addBindValue(QString(bookmark.deviceID.c_str()));
    query.addBindValue(QString(bookmark.channelID.c_str()));
    query.addBindValue(QString(bookmark.name.c_str()));
    query.addBindValue(QString(bookmark.description.c_str()));
    query.addBindValue(QString(bookmark.tag.c_str()));
    query.addBindValue(QVariant::fromValue(bookmark.startTime));
    query.addBindValue(QVariant::fromValue(bookmark.endTime));
    query.addBindValue(bookmark.overlappedID);

    QByteArray data("", 0);

    if(bookmark.image.size() > 0 && bookmark.image.data() != nullptr)
    {
        data = QByteArray(reinterpret_cast<const char*>(bookmark.image.data()),static_cast<int>(bookmark.image.size()));
    }

    query.addBindValue(data);

    query.addBindValue(QString(bookmark.eventLogID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void BookmarkTable::Remove(const uuid_string& bookmarkID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_BOOKMARK] WHERE BOOKMARK_ID=?");
    query.addBindValue(QString(bookmarkID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void BookmarkTable::RemoveByDeviceID(const uuid_string& deviceID, std::vector<uuid_string>& removedBookmarkIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_BOOKMARK] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        removedBookmarkIDs.emplace_back(query.value(0).toString().toStdString());
    }

    if(removedBookmarkIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_BOOKMARK] WHERE DEVICE_ID=?");
    query.addBindValue(QString(deviceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void BookmarkTable::RemoveByChannelID(const uuid_string& deviceID, const std::string& channelID, std::vector<uuid_string>& removedBookmarkIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_BOOKMARK] WHERE DEVICE_ID=? AND CHANNEL_ID=?");
    query.addBindValue(QString(deviceID.c_str()));
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        removedBookmarkIDs.emplace_back(query.value(0).toString().toStdString());
    }

    if(removedBookmarkIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_BOOKMARK] WHERE DEVICE_ID=? AND CHANNEL_ID=?");
    query.addBindValue(QString(deviceID.c_str()));
    query.addBindValue(QString(channelID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}


}
}

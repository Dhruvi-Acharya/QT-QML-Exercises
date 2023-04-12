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

#include <map>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <CoreService/CoreServiceRequestResponse.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

class BookmarkTable
{
public:
    explicit BookmarkTable(QSqlDatabase& database);

    void CreateTable();

    void GetAll(std::map<uuid_string, Bookmark>& bookmarks);
    void Save(const Bookmark& bookmark);
    void Remove(const uuid_string& bookmarkID);
    void RemoveByDeviceID(const uuid_string& deviceID, std::vector<uuid_string>& removedBookmarkIDs);
    void RemoveByChannelID(const uuid_string& deviceID, const std::string& channelID, std::vector<uuid_string>& removedBookmarkIDs);

private:
    QSqlDatabase& m_database;
};

}
}

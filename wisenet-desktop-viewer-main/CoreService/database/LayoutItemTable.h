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
#include <set>
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

class LayoutItemTable
{
public:
    explicit LayoutItemTable(QSqlDatabase& database);

    void CreateTable();

    void Get(const uuid_string& layoutID, std::vector<LayoutItem>& layoutItems);
    void Add(const uuid_string& layoutID, const LayoutItem& layoutItem);
    void Remove(const uuid_string& layoutID);
    void RemoveByParentID(const uuid_string& parentID, std::set<uuid_string>& changedLayoutIDs);
    void RemoveByChannelID(const std::string &deviceId, const std::string channelId, std::set<uuid_string> &changedLayoutIDs);
    void RemoveByItemID(const std::string itemId, std::set<uuid_string> &changedLayoutIDs);

private:
    QSqlDatabase& m_database;
};

}
}


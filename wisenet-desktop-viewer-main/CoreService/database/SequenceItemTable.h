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

class SequenceItemTable
{
public:
    explicit SequenceItemTable(QSqlDatabase& database);

    void CreateTable();

    void Get(const uuid_string& sequenceID, std::vector<SequenceLayoutItem>& sequenceItems);
    void Add(const uuid_string& sequenceID, const SequenceLayoutItem& sequenceItem);
    void Remove(const uuid_string& sequenceID);
    void RemoveByItemID(const std::string& itemID, std::set<uuid_string>& changedSequenceIDs);
    void RemoveByParentID(const uuid_string& parentID, std::set<uuid_string>& changedSequenceIDs);

private:
    QSqlDatabase& m_database;
};

}
}

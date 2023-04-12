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

#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <CoreService/CoreServiceRequestResponse.h>
#include <CoreService/CoreServiceStructure.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

class GroupTable
{
public:
    explicit GroupTable(QSqlDatabase& database);

    void CreateTable();

    void GetAll(std::map<uuid_string, Group>& groups);
    void GetChildrenByBottomUpOrder(const uuid_string &groupID, std::vector<uuid_string>& children);
    void Save(const Group &group);
    void Remove(const uuid_string &groupID);

private:
    QSqlDatabase& m_database;
};

}
}

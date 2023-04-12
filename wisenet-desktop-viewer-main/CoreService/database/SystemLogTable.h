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
#include <DeviceClient/DeviceStructure.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

class SystemLogTable
{
public:
    SystemLogTable(QSqlDatabase& database);

    void CreateTable();

    void Get(const GetSystemLogRequestSharedPtr& request, std::shared_ptr<std::vector<EventLog>>& response);
    void Save(EventLog& eventLog);
    void RemoveTo(int64_t timeMsec);

private:
     QSqlDatabase& m_database;
};

}
}


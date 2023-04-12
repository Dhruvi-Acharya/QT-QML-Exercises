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
#include "CryptoAdapter.h"

#include <CoreService/CoreServiceRequestResponse.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

typedef std::function <void()> ManagementMigrationHandler;

class ManagementDatabaseMigration
{
public:
    explicit ManagementDatabaseMigration(QSqlDatabase& database);

    void MigrateFrom(int fromVersion);

private:
    //Migration SQL 작성 관련 참고 : https://araikuma.tistory.com/686
    void toV1();
    void toV2();
    void toV3();
    void toV4();
    void toV5();

    std::vector<ManagementMigrationHandler> m_migrationHandlers;
    QSqlDatabase& m_database;
    CryptoAdapter m_crypto;
};

}
}

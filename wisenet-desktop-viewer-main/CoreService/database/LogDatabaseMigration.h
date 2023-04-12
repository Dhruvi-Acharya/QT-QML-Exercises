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
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

typedef std::function <void()> LogMigrationHandler;

class LogDatabaseMigration
{
public:
    explicit LogDatabaseMigration(QSqlDatabase& database);

    void MigrateFrom(int fromVersion);

private:
    //Migration SQL 작성 관련 참고 : https://araikuma.tistory.com/686
    //Log 관련 테이블은 데이터양이 많아 기존 테이블이름을 변경하고 신규 테이블을 만들고 신규 테이블에 기존 테이블 내용을
    //넣는 방식은 사용하면 시간이 많이 걸릴 수 있으므로 사용하지 않도록 해야한다.
    void toV1();

    std::vector<LogMigrationHandler> m_migrationHandlers;
    QSqlDatabase& m_database;
};

}
}



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
#include "DashboardDatabaseMigration.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

DashboardDatabaseMigration::DashboardDatabaseMigration(QSqlDatabase& database)
    :m_database(database)
{
    //MigrationFunction vector를 생성한다.
    //m_migrationHandlers.push_back(std::bind(&DashboardDatabaseMigration::toV1, this));
}

void DashboardDatabaseMigration::MigrateFrom(int fromVersion)
{
    SPDLOG_INFO("[LocalService] Start migration from Version({}) for the dashboard database",fromVersion);
    //vector index는 version - 1 입니다.
    //fromVersion 부터 시작을 하면 바로 다음 버전의 마이그레이션 함수를 호출하는 것임.
    for(size_t i = fromVersion ; i < m_migrationHandlers.size(); i++){
        m_migrationHandlers[i]();
    }
}

}
}

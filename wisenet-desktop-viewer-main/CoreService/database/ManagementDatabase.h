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

#include <QString>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <CoreService/CoreServiceRequestResponse.h>

#include "DatabaseException.h"
#include "CloudTable.h"
#include "UserTable.h"
#include "UserGroupTable.h"
#include "UserGroupResourceTable.h"
#include "ServiceTable.h"
#include "SequenceTable.h"
#include "SequenceItemTable.h"
#include "GroupTable.h"
#include "GroupMappingTable.h"
#include "LayoutTable.h"
#include "LayoutItemTable.h"
#include "FileTable.h"
#include "DeviceTable.h"
#include "ChannelTable.h"
#include "WebPageTable.h"
#include "BookmarkTable.h"
#include "EventRuleTable.h"
#include "EventScheduleTable.h"
#include "EventEmailTable.h"
#include "ServiceSettingsTable.h"
#include "LdapTable.h"
#include "StatisticsTable.h"
#include "LicenseTable.h"

namespace Wisenet
{
namespace Core
{

class ManagementDatabase
{
public:
    ManagementDatabase();

    bool Open(QString &path);
    void Close();
    bool OpenClose(QString &path);

    void Transaction();
    void Commit();
    void Rollback();

    CloudTable& GetCloudTable() { return m_cloudTable; }
    UserTable& GetUserTable(){ return m_userTable; }
    UserGroupTable& GetUserGroupTable() {return m_userGroupTable; }
    UserGroupResourceTable& GetUserGroupResourceTable() { return m_userGroupResourceTable; }

    GroupTable& GetGroupTable() { return m_groupTable; }
    GroupMappingTable& GetGroupMappingTable() { return m_groupMappingTable; }

    SequenceTable& GetSequenceTable() { return m_sequenceTable; }
    SequenceItemTable& GetSequenceItemTable() { return m_sequenceItemTable; }
    LayoutTable& GetLayoutTable() { return m_layoutTable; }
    LayoutItemTable& GetLayoutItemTable() { return m_layoutItemTable; }

    ServiceTable& GetServiceTable() { return m_serviceTable; }
    FileTable& GetFileTable() { return m_fileTable; }
    DeviceTable& GetDeviceTable() { return m_deviceTable; }
    ChannelTable& GetChannelTable() { return m_channelTable; }
    WebPageTable& GetWebPageTable() { return m_webPageTable; }

    BookmarkTable& GetBookmarkTable() { return m_bookmarkTable; }

    EventRuleTable& GetEventRuleTable() { return m_eventRuleTable; }
    EventScheduleTable& GetEventScheduleTable() { return m_eventScheduleTable; }
    EventEmailTable& GetEventEmailTable() { return m_eventEmailTable; }

    ServiceSettingsTable& GetServiceSettingsTable() { return m_serviceSettinsTable; }

    LdapTable& GetLdapTable() {return m_ldapTable;}
    StatisticsTable& GetStatisticsTable() {return m_statisticsTable;}
    LicenseTable& GetLicenseTable() {return m_licenseTable;}

    static QString FILENAME;
    static QString KEY;
    // 1: v1.0.0, 2: v1.1.1, 3: v1.2.0, 4: v1.3.0, 5: v1.4.0
    static const int DB_LATEST_VERSION = 5;
private:
    bool CreateTable();
    bool MigrateTable();
    void CreateVersionTable();

    QString m_path;
    QSqlDatabase m_database;

    CloudTable m_cloudTable;

    UserTable m_userTable;
    UserGroupTable m_userGroupTable;
    UserGroupResourceTable m_userGroupResourceTable;

    GroupTable m_groupTable;
    GroupMappingTable m_groupMappingTable;

    SequenceTable m_sequenceTable;
    SequenceItemTable m_sequenceItemTable;
    LayoutTable m_layoutTable;
    LayoutItemTable m_layoutItemTable;

    ServiceTable m_serviceTable;
    FileTable m_fileTable;
    DeviceTable m_deviceTable;
    ChannelTable m_channelTable;
    WebPageTable m_webPageTable;

    BookmarkTable m_bookmarkTable;

    EventRuleTable m_eventRuleTable;
    EventScheduleTable m_eventScheduleTable;
    EventEmailTable m_eventEmailTable;

    ServiceSettingsTable m_serviceSettinsTable;

    LdapTable m_ldapTable;
    StatisticsTable m_statisticsTable;
    LicenseTable m_licenseTable;
};

}
}

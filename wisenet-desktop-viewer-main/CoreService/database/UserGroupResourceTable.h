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

#include <set>
#include <iostream>
#include <QString>
#include <QVariant>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <boost/core/ignore_unused.hpp>

#include <CoreService/CoreServiceRequestResponse.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

class UserGroupResourceTable
{
public:
    explicit UserGroupResourceTable(QSqlDatabase& database);

    enum class UserGroupResourceType {
        Camera = 0,
        Webpage,
        Layout,
    };

    void CreateTable();
    void Get(const uuid_string& userGroupID, UserGroup::Resource& resource);
    void GetWebpages(const uuid_string& userGroupID, std::vector<uuid_string>& webpages);
    void GetDevices(const uuid_string& userGroupID, std::map<uuid_string, UserGroup::Resource::Device>& devices);
    void GetLayouts(const uuid_string& userGroupID, std::vector<uuid_string>& layouts);

    void Add(const uuid_string& userGroupID, const UserGroup::Resource& resource);
    void Remove(const uuid_string& userGroupID);
    void RemoveByItemID(const UserGroupResourceType& itemType, const std::string& itemID, std::set<uuid_string>& changedUserGroupIDs);
    void RemoveByParentID(const std::string& parentID, std::set<uuid_string>& changedUseGroupIDs);
    void RemoveByChannelID(const std::string &deviceId, const std::string channelId, std::set<uuid_string> &changedUserRoleIDs);

private:
    QSqlDatabase& m_database;
};

}
}


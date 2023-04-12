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
#include "LayoutTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

LayoutTable::LayoutTable(QSqlDatabase& database)
    :m_database(database)
{

}

void LayoutTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_LAYOUT"))) {
        SPDLOG_INFO("[T_LAYOUT] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_LAYOUT]( \
                [LAYOUT_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY\
                ,[USERNAME] VARCHAR(40)\
                ,[NAME] TEXT\
                ,[LOCKED] BOOLEAN NOT NULL DEFAULT 0\
                ,[CELL_WIDTH] REAL NOT NULL DEFAULT 160\
                ,[CELL_HEIGHT] REAL NOT NULL DEFAULT 90\
                ,[CELL_SPACING] REAL NOT NULL DEFAULT 0.0\
                ,[FILL_MODE] INTEGER NOT NULL DEFAULT 1\
                ,[STREAM_PROFILE] INTEGER NOT NULL DEFAULT 1\
                )")) {

        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutTable::GetAll(std::map<uuid_string, Layout> &layouts)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_LAYOUT]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Layout layout;
        layout.layoutID = query.value(0).toString().toStdString();
        layout.userName = query.value(1).toString().toStdString();
        layout.name = query.value(2).toString().toStdString();
        layout.isLocked = query.value(3).toBool();
        layout.cellWidth = query.value(4).toFloat();
        layout.cellHeight = query.value(5).toFloat();
        layout.cellSpacing = query.value(6).toFloat();
        layout.fillMode = static_cast<LayoutItem::FillMode>(query.value(7).toInt());
        layout.profile = static_cast<LayoutItem::StreamProfile>(query.value(8).toInt());

        layouts.emplace(layout.layoutID, layout);
    }
}

void LayoutTable::Save(const Layout &layout)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_LAYOUT] \
            (LAYOUT_ID,USERNAME,NAME,LOCKED,CELL_WIDTH,CELL_HEIGHT,CELL_SPACING,FILL_MODE,STREAM_PROFILE) \
            VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(layout.layoutID.c_str()));
    query.addBindValue(QString(layout.userName.c_str()));
    query.addBindValue(QString(layout.name.c_str()));
    query.addBindValue(layout.isLocked);
    query.addBindValue(layout.cellWidth);
    query.addBindValue(layout.cellHeight);
    query.addBindValue(layout.cellSpacing);
    query.addBindValue(static_cast<int>(layout.fillMode));
    query.addBindValue(static_cast<int>(layout.profile));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutTable::Remove(const std::string& layoutID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_LAYOUT] WHERE LAYOUT_ID=?");
    query.addBindValue(QString(layoutID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LayoutTable::RemoveByUserName(const uuid_string &userName, std::vector<uuid_string> &removedLayoutIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_LAYOUT] WHERE USERNAME=?");
    query.addBindValue(QString(userName.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
    removedLayoutIDs.emplace_back(query.value(0).toString().toStdString());
    }

    if(removedLayoutIDs.empty()){
    return;
    }

    query.prepare("DELETE FROM [T_LAYOUT] WHERE USERNAME=?");
    query.addBindValue(QString(userName.c_str()));

    if(!query.exec()) {
    throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

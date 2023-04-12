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
#include "WebPageTable.h"

#include <QString>
#include <QVariant>

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

WebPageTable::WebPageTable(QSqlDatabase& database)
    :m_database(database)
{

}

void WebPageTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_WEBPAGE"))) {
        SPDLOG_INFO("[T_WEBPAGE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_WEBPAGE]( \
                [WEBPAGE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY\
                ,[NAME] TEXT\
                ,[URL] TEXT\
                ,[USE_AUTH] BOOLEAN NULL DEFAULT 0\
                ,[USERNAME] VARCHAR(128)\
                ,[PASSWORD] VARCHAR(128)\
                )")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    // Default Video tutorial
    Webpage web;
    web.webpageID = boost::uuids::to_string(boost::uuids::random_generator()());
    web.name = "Video tutorial";
    web.url = "https://youtube.com/playlist?list=PLG7UogTGksTonxXaBlOqfsgiU8jIscf4O";
    web.useAuth = false;
    Save(web);
}

void WebPageTable::GetAll(std::map<uuid_string, Webpage>& webPages)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_WEBPAGE]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        Webpage webPage;
        webPage.webpageID = query.value(0).toString().toStdString();
        webPage.name = query.value(1).toString().toStdString();
        webPage.url = query.value(2).toString().toStdString();
        webPage.useAuth = query.value(3).toBool();
        if (webPage.useAuth) {
            webPage.userId = query.value(4).toString().toStdString();
            webPage.userPw = query.value(5).toString().toStdString();
        }

        webPages.emplace(webPage.webpageID, webPage);
    }
}

void WebPageTable::Save(const Webpage &webPage)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_WEBPAGE] \
            (WEBPAGE_ID,NAME,URL,USE_AUTH,USERNAME,PASSWORD) \
            VALUES (?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(webPage.webpageID.c_str()));
    query.addBindValue(QString(webPage.name.c_str()));
    query.addBindValue(QString(webPage.url.c_str()));
    query.addBindValue(webPage.useAuth);
    query.addBindValue(QString(webPage.userId.c_str()));
    query.addBindValue(QString(webPage.userPw.c_str()));


    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void WebPageTable::Remove(const uuid_string& webpageID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_WEBPAGE] WHERE WEBPAGE_ID=?");
    query.addBindValue(QString(webpageID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

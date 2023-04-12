/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include "EventEmailTable.h"

#include <QString>
#include <QVariant>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

EventEmailTable::EventEmailTable(QSqlDatabase& database,QString& key)
    :m_database(database)
{
    m_crypto.Init(key);
}

void EventEmailTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_EMAIL"))) {
        SPDLOG_INFO("[T_EMAIL] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_EMAIL]( \
                [SENDER] VARCHAR(128) NOT NULL UNIQUE PRIMARY KEY\
                ,[RECEIVER] VARCHAR(128)\
                ,[SERVER_ADDRESS] VARCHAR(128)\
                ,[ID] VARCHAR(128)\
                ,[PASSWORD] VARCHAR(128)\
                ,[PORT] VARCHAR(128)\
                ,[AUTHENTICATION]BOOLEAN NOT NULL DEFAULT 1 \
                )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void EventEmailTable::GetAll(std::shared_ptr<EventEmail>& eventEmail)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_EMAIL]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        auto email = std::make_shared<EventEmail>();
        email->sender= query.value(0).toString().toStdString();
        email->receiver = query.value(1).toString().toStdString();
        email->serverAddress = query.value(2).toString().toStdString();
        email->id = query.value(3).toString().toStdString();
        email->password = m_crypto.Decrypt(query.value(4).toByteArray()).toStdString();
        email->port = query.value(5).toString().toStdString();
        email->isAuthentication = query.value(6).toBool();

        eventEmail = std::move(email);
    }
}

void EventEmailTable::Save(std::shared_ptr<EventEmail> eventEmail)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_EMAIL] \
                (SENDER, RECEIVER, SERVER_ADDRESS, ID, PASSWORD, PORT, AUTHENTICATION) \
                VALUES (?, ?, ?, ?, ?, ?, ?)");
    query.addBindValue(QString(eventEmail->sender.c_str()));
    query.addBindValue(QString(eventEmail->receiver.c_str()));
    query.addBindValue(QString(eventEmail->serverAddress.c_str()));
    query.addBindValue(QString(eventEmail->id.c_str()));
    query.addBindValue(m_crypto.Encrypt(QString(eventEmail->password.c_str())));
    query.addBindValue(QString(eventEmail->port.c_str()));
    query.addBindValue(eventEmail->isAuthentication);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void EventEmailTable::Remove()
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_EMAIL]");

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

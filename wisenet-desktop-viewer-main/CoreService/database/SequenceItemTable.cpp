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
#include "SequenceItemTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

SequenceItemTable::SequenceItemTable(QSqlDatabase& database)
    :m_database(database)
{

}

void SequenceItemTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_SEQUENCE_ITEM"))) {
        SPDLOG_INFO("[T_SEQUENCE_ITEM] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_SEQUENCE_ITEM]( \
                [SEQUENCE_ITEM_ID] VARCHAR(40) NOT NULL\
                ,[SEQUENCE_ID] VARCHAR(40) NOT NULL\
                ,[LAYOUT_ID] TEXT \
                ,[DELAY_MS]INTEGER NOT NULL\
                ,[WORK_ORDER] INTEGER NOT NULL\
                )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

}

void SequenceItemTable::Get(const std::string &sequenceID, std::vector<SequenceLayoutItem> &sequenceItems)
{
    QSqlQuery query(m_database);

    query.prepare("SELECT * FROM [T_SEQUENCE_ITEM] WHERE SEQUENCE_ID=?");
    query.addBindValue(QString(sequenceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        SequenceLayoutItem sequenceItem;

        sequenceItem.itemID = query.value(0).toString().toStdString();
        sequenceItem.layoutID = query.value(2).toString().toStdString();

        sequenceItem.delayMs = query.value(3).toInt();
        sequenceItem.order = query.value(4).toInt();

        sequenceItems.emplace_back(sequenceItem);
    }
}

void SequenceItemTable::Add(const std::string& sequenceID, const SequenceLayoutItem &sequenceItem)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT INTO [T_SEQUENCE_ITEM] \
            (SEQUENCE_ITEM_ID,SEQUENCE_ID,LAYOUT_ID,DELAY_MS,WORK_ORDER) \
            VALUES (? ,?, ?, ?, ?)");

    query.addBindValue(QString(sequenceItem.itemID.c_str()));
    query.addBindValue(QString(sequenceID.c_str()));
    query.addBindValue(QString(sequenceItem.layoutID.c_str()));
    query.addBindValue(sequenceItem.delayMs);
    query.addBindValue(sequenceItem.order);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceItemTable::Remove(const std::string &sequenceID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_SEQUENCE_ITEM] WHERE SEQUENCE_ID=?");
    query.addBindValue(QString(sequenceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceItemTable::RemoveByItemID(const std::string &itemID, std::set<uuid_string> &changedSequenceIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_SEQUENCE_ITEM] WHERE LAYOUT_ID=?");
    query.addBindValue(QString(itemID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedSequenceIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedSequenceIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_SEQUENCE_ITEM] WHERE LAYOUT_ID=?");
    query.addBindValue(QString(itemID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceItemTable::RemoveByParentID(const uuid_string &parentID, std::set<uuid_string> &changedSequenceIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_SEQUENCE_ITEM] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        changedSequenceIDs.emplace(query.value(1).toString().toStdString());
    }

    if(changedSequenceIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_SEQUENCE_ITEM] WHERE PARENT_ID=?");
    query.addBindValue(QString(parentID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

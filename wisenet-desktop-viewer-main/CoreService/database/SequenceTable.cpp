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
#include "SequenceTable.h"

#include <QString>
#include <QVariant>

#include <boost/core/ignore_unused.hpp>
#include "CoreServiceLogSettings.h"


namespace Wisenet
{
namespace Core
{

SequenceTable::SequenceTable(QSqlDatabase& database)
    :m_database(database)
{

}

void SequenceTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_SEQUENCE"))) {
        SPDLOG_INFO("[T_SEQUENCE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    if(!query.exec("CREATE TABLE [T_SEQUENCE]( \
                [SEQUENCE_ID] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY\
                ,[NAME] TEXT\
                ,[USERNAME] VARCHAR(40) NOT NULL\
                ,[SETTINGS_MANUAL] BOOL NOT NULL DEFAULT 0\
                )")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceTable::GetAll(std::map<uuid_string, SequenceLayout> &sequences)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_SEQUENCE]")) {
       throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        SequenceLayout sequence;
        sequence.sequenceLayoutID = query.value(0).toString().toStdString();
        sequence.name = query.value(1).toString().toStdString();
        sequence.userName = query.value(2).toString().toStdString();
        sequence.settingsManual = query.value(3).toBool();

        sequences.emplace(sequence.sequenceLayoutID, sequence);
    }
}

void SequenceTable::Save(const SequenceLayout &sequence)
{
    QSqlQuery query(m_database);

    query.prepare("INSERT OR REPLACE INTO [T_SEQUENCE] \
            (SEQUENCE_ID,NAME,USERNAME,SETTINGS_MANUAL) \
            VALUES (?, ?, ?, ?)");
    query.addBindValue(QString(sequence.sequenceLayoutID.c_str()));
    query.addBindValue(QString(sequence.name.c_str()));
    query.addBindValue(QString(sequence.userName.c_str()));
    query.addBindValue(sequence.settingsManual);

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceTable::Remove(const uuid_string &sequenceID)
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_SEQUENCE] WHERE SEQUENCE_ID=?");
    query.addBindValue(QString(sequenceID.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void SequenceTable::RemoveByUserName(const uuid_string &userName, std::vector<uuid_string>& removedSquenceIDs)
{
    QSqlQuery query(m_database);

    //삭제항목 확인
    query.prepare("SELECT * FROM [T_SEQUENCE] WHERE USERNAME=?");
    query.addBindValue(QString(userName.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        removedSquenceIDs.emplace_back(query.value(0).toString().toStdString());
    }

    if(removedSquenceIDs.empty()){
        return;
    }

    query.prepare("DELETE FROM [T_SEQUENCE] WHERE USERNAME=?");
    query.addBindValue(QString(userName.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

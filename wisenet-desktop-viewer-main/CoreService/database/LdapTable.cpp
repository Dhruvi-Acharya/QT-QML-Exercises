#include "LdapTable.h"

#include <QString>
#include <QVariant>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

LdapTable::LdapTable(QSqlDatabase& database) : m_database(database)
{

}

void LdapTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_LDAP"))) {
        SPDLOG_INFO("[T_LDAP] Already exists.");
        return;
    }

    QSqlQuery query(m_database);
    if(!query.exec("CREATE TABLE [T_LDAP]([SERVER_URL] TEXT, [ADMIN_DN] TEXT, [SEARCHBASE] TEXT, [SEARCHFILTER] TEXT)")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LdapTable::GetLdap(LdapSetting& ldapSetting)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_LDAP]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        ldapSetting.serverUrl = query.value(0).toString().toStdString();
        ldapSetting.adminDN = query.value(1).toString().toStdString();
        ldapSetting.searchBase = query.value(2).toString().toStdString();
        ldapSetting.searchFilter = query.value(3).toString().toStdString();
    }
}

void LdapTable::Save(const LdapSetting& ldapSetting)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO [T_LDAP] (SERVER_URL, ADMIN_DN, SEARCHBASE, SEARCHFILTER) VALUES (?, ?, ?, ?)");
    query.addBindValue(QString(ldapSetting.serverUrl.c_str()));
    query.addBindValue(QString(ldapSetting.adminDN.c_str()));
    query.addBindValue(QString(ldapSetting.searchBase.c_str()));
    query.addBindValue(QString(ldapSetting.searchFilter.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    SPDLOG_INFO("[T_LDAP] LdapTable::Save end");
}

void LdapTable::Remove()
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_LDAP]");

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

#include "LicenseTable.h"
#include "CryptoAdapter.h"
#include <QNetworkInterface>
#include <QString>
#include <QVariant>

#include "CoreServiceLogSettings.h"

namespace Wisenet
{
namespace Core
{

LicenseTable::LicenseTable(QSqlDatabase& database) : m_database(database)
{

}

void LicenseTable::CreateTable()
{
    if(m_database.tables().contains(QLatin1String("T_LICENSE"))) {
        SPDLOG_INFO("[T_LICENSE] Already exists.");
        return;
    }

    QSqlQuery query(m_database);

    QString queryString = "CREATE TABLE [T_LICENSE] ([LICENSE_KEY] VARCHAR(40) NOT NULL UNIQUE PRIMARY KEY)";

    if(!query.exec(queryString))
    {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

void LicenseTable::Get(LicenseInfo &licenseInfo)
{
    QSqlQuery query(m_database);

    if(!query.exec("SELECT * FROM [T_LICENSE]")) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }

    while(query.next()) {
        licenseInfo.licenseKey = query.value(0).toString().toStdString();
    }

    if(licenseInfo.licenseKey != "")
    {
        QString licenseString = QString::fromStdString(licenseInfo.licenseKey);
        CryptoAdapter crypto;
        QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
        foreach (QNetworkInterface interface, interfaces)
        {
            if(interface.hardwareAddress() != "" && crypto.LicenseEncrypt(interface.hardwareAddress()) == licenseString)
            {
                licenseInfo.activated = true;
            }
        }
    }
}

void LicenseTable::Save(const LicenseInfo &licenseInfo)
{
    QSqlQuery query(m_database);
    query.prepare("INSERT INTO [T_LICENSE] (LICENSE_KEY) VALUES (?)");
    query.addBindValue(QString(licenseInfo.licenseKey.c_str()));

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
    SPDLOG_INFO("[T_LICENSE] LicenseTable::Save end");
}

void LicenseTable::Remove()
{
    QSqlQuery query(m_database);

    query.prepare("DELETE FROM [T_LICENSE]");

    if(!query.exec()) {
        throw ManagementDatabaseException(query.lastQuery().toStdString(), query.lastError());
    }
}

}
}

#pragma once
#include <QString>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

#include <CoreService/CoreServiceRequestResponse.h>
#include "DatabaseException.h"

namespace Wisenet
{
namespace Core
{

class LicenseTable
{
public:
    explicit LicenseTable(QSqlDatabase& database);

    void CreateTable();

    void Get(LicenseInfo& licenseInfo);
    void Save(const LicenseInfo& licenseInfo);
    void Remove();

private:
    QSqlDatabase& m_database;    
};
\

}
}


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

class LdapTable
{
public:
    explicit LdapTable(QSqlDatabase& database);

    void CreateTable();

    void GetLdap(LdapSetting& ldapSetting);
    void Save(const LdapSetting& ldapSetting);
    void Remove();

private:
    QSqlDatabase& m_database;
};

}
}

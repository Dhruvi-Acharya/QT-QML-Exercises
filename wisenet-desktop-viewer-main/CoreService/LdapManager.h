#include "CoreService/CoreServiceStructure.h"
#include "DatabaseManager.h"

#include <memory>
#include "ldap.h"

namespace Wisenet
{
namespace Core
{

class LdapManager: public std::enable_shared_from_this<LdapManager>
{
public:
    explicit LdapManager(DatabaseManager& databaseManager);
    Wisenet::ErrorCode Login(const std::string& loginID, const std::string& password);

    ResponseBaseSharedPtr LdapConnect(const LdapConnectRequestSharedPtr &request);
    ResponseBaseSharedPtr SearchUser();

private:
    void ldapError(    const char *func,
                       int err,
                       const char *extra,
                       const char *matched,
                       const char *info,
                       char **refs );


    DatabaseManager& m_db;

    LDAP* m_ldapObject;

    std::string m_searchBase;
    std::string m_searchFilter;
    std::set<std::string> m_searchAttrs = {"uid", "displayName", "mail", "objectclass", "sAMAccountName"};
    QList<std::string> m_userObjectClass = {"user", "person", "inetOrgPerson", "posixAccount", "organizationalPerson"};
};

}
}

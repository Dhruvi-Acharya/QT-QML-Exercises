#include "LdapManager.h"
#include "LogSettings.h"
#include <QUrl>

namespace Wisenet
{
namespace Core
{

LdapManager::LdapManager(DatabaseManager& databaseManager)
    :m_db(databaseManager), m_ldapObject(nullptr)
{
}


Wisenet::ErrorCode LdapManager::Login(const std::string& loginID, const std::string& password)
{
    std::string userDN = "";
    std::string userPassword = password;

    std::map<uuid_string, User> users = m_db.GetUsers();
    for(auto& user: users) {
        if(user.second.loginID == loginID)
            userDN = user.second.dn;
    }

    if (userDN == "")
        return ErrorCode::UnAuthorized;

    LdapSetting ldapSetting = m_db.GetLdapSetting();
    QUrl serverUrl =  QString::fromStdString(ldapSetting.serverUrl);

    LDAP* ldapObject;
    int protocolVer = LDAP_VERSION3;
    int tls_req = LDAP_OPT_X_TLS_ALLOW;
    int timeLimit = 60;

    struct berval cred;
    cred.bv_val = const_cast<char*>(userPassword.c_str());
    cred.bv_len = userPassword.size();

    // TLS_REQUIRE_CERT 옵션은 ldap_initialize 이전에 세팅되어야 함.
    ldap_set_option(NULL,LDAP_OPT_X_TLS_REQUIRE_CERT, &tls_req);

    int errorCode = ldap_initialize(&ldapObject, ldapSetting.serverUrl.c_str());
    SPDLOG_DEBUG("LdapManager::LogIn : LDAP initialize {} {}", errorCode, ldapSetting.serverUrl);

    ldap_set_option(ldapObject, LDAP_OPT_PROTOCOL_VERSION, &protocolVer);
    ldap_set_option(ldapObject, LDAP_OPT_TIMELIMIT, &timeLimit);

    if(serverUrl.scheme() == "ldap") {
        errorCode = ldap_start_tls_s(ldapObject, NULL,NULL);
        if(errorCode == LDAP_SUCCESS)
            SPDLOG_DEBUG("LdapManager::LogIn LDAP start_tls SUCCESS.");
        else
            SPDLOG_DEBUG("LdapManager::LogIn LDAP start_tls not supported.");
    }
    errorCode = ldap_sasl_bind_s(ldapObject, userDN.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
    if(errorCode == LDAP_SUCCESS) {
        SPDLOG_DEBUG("LdapManager::LogIn LDAP bind successed.");
        return ErrorCode::LdapSuccess;
    }
    else if(errorCode == LDAP_INVALID_CREDENTIALS) {
        SPDLOG_DEBUG("LdapManager::LogIn LDAP bind failed. - wrong password");
        return ErrorCode::UnAuthorized;
    }
    else{
        SPDLOG_DEBUG("LdapManager::LogIn : LDAP bind failed. {}", errorCode);
        return ErrorCode::LdapConnectionError;
    }
}

void LdapManager::ldapError( const char *func, int err, const char *extra, const char *matched, const char *info, char **refs )
{
    fprintf( stderr, "%s: %s (%d)%s\n",
             func, ldap_err2string( err ), err, extra ? extra : "" );

    if ( matched && *matched ) {
        fprintf( stderr, ("\tmatched DN: %s\n"), matched );
    }

    if ( info && *info ) {
        fprintf( stderr, ("\tadditional info: %s\n"), info );
    }

    if ( refs && *refs ) {
        int i;
        fprintf( stderr, ("\treferrals:\n") );
        for( i=0; refs[i]; i++ ) {
            fprintf( stderr, "\t\t%s\n", refs[i] );
        }
    }
}

ResponseBaseSharedPtr LdapManager::LdapConnect(const LdapConnectRequestSharedPtr &request)
{
    SPDLOG_DEBUG("LdapManager::LdapConnect Start");
    auto res = std::make_shared<LdapConnectResponse>();

    int protocol_version = LDAP_VERSION3;
    int tls_req = LDAP_OPT_X_TLS_ALLOW;
    int timeLimit = 60;
    int initResult;
    int errorCode;

    struct berval cred;
    std::string stringPW= request->ldapSetting.adminPW;
    cred.bv_val = const_cast<char*>(stringPW.c_str());
    cred.bv_len = stringPW.size();

    m_searchBase = request->ldapSetting.searchBase;
    m_searchFilter = request->ldapSetting.searchFilter;

    // TLS_REQUIRE_CERT 옵션은 ldap_initialize 이전에 세팅되어야 함.
    ldap_set_option(NULL, LDAP_OPT_X_TLS_REQUIRE_CERT, &tls_req);

    initResult = ldap_initialize(&m_ldapObject, request->ldapSetting.serverUrl.c_str());
    if(initResult == LDAP_SUCCESS)
        SPDLOG_DEBUG("[LdapManager] LdapConnnect ldap initialize SUCCESS");

    if (m_ldapObject) {
        ldap_set_option(m_ldapObject, LDAP_OPT_PROTOCOL_VERSION, &protocol_version );
        ldap_set_option(m_ldapObject, LDAP_OPT_TIMELIMIT, &timeLimit);
    }

    if(!request->ldapSetting.isSsl) {
        errorCode = ldap_start_tls_s(m_ldapObject, NULL, NULL);
        if(errorCode != LDAP_SUCCESS) {
            SPDLOG_DEBUG("[LdapManager] LdapConnect ldap_start_tls Failed");
            char *msg = NULL;
            ldapError("ldap_start_tls_s", errorCode, NULL, NULL, msg, NULL);
            ldap_memfree(msg);
        }
        else
            SPDLOG_DEBUG("[LdapManager] LdapConnect ldap_start_tls SUCCESS");
    }

    errorCode = ldap_sasl_bind_s(m_ldapObject, request->ldapSetting.adminDN.c_str(), LDAP_SASL_SIMPLE, &cred, NULL, NULL, NULL);
    if (errorCode == LDAP_SUCCESS) {
        SPDLOG_DEBUG("[LdapManager] Connection to LDAP server is successfully done.");
        return SearchUser();
    }
    else {
        SPDLOG_DEBUG("[LdapManager] Connection to LDAP server is failed.");

        char *msg=NULL;
        ldap_get_option(m_ldapObject, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&msg);
        ldapError( "ldap_sasl_bind_s", errorCode, NULL, NULL, msg, NULL );
        ldap_memfree(msg);
        ldap_unbind_ext_s(m_ldapObject, NULL, NULL);

        res->connResult.connSuccess = false;
        res->connResult.searchSuccess = false;
        res->connResult.errorMsg = ldap_err2string(errorCode);
        return res;
    }
}

ResponseBaseSharedPtr LdapManager::SearchUser()
{
    SPDLOG_DEBUG("[LdapManager] SearchUser()");
    // Search
    std::list<std::string> attrLists;
    std::string base = m_searchBase;
    std::string filter = m_searchFilter;
    char** ldap_attrs = attrLists.size() ? new char* [ attrLists.size() + 1 ] : NULL;
    LDAPMessage* result = NULL;

    auto res = std::make_shared<LdapConnectResponse>();

    // search Filter를 걸어서 Search 요청 보내는 경우 아래 코드 사용
    //    if(m_searchFilter == "")
    //        filter = m_searchFilter.toStdString();
    //    else {
    //        if(m_searchFilter.startsWith("(")) // 검색 필터가 괄호로 시작하지 않는 경우 (사용자가 필터 조건을 하나만 설정했을 때)
    //            filter = "(&" + m_searchFilter.toStdString() + ")";
    //        else // 괄호로 시작하는 경우 (사용자가 필터로 조건을 여러 개 설정했을 때)
    //            filter = "(&(" + m_searchFilter.toStdString() + ")";
    //    }

    int errorCode = ldap_search_ext_s(m_ldapObject, base.c_str(), LDAP_SCOPE_SUBTREE,
                                      (filter.empty() ? NULL : filter.c_str()), ldap_attrs, 0, NULL, NULL, NULL, 0, &result);

    if(errorCode == LDAP_SUCCESS)
    {
        SPDLOG_DEBUG("[LdapManager] ldap_search_ext_s successed. User Found : {}", ldap_count_entries(m_ldapObject, result));
        for(LDAPMessage* ldapEntry = ldap_first_entry(m_ldapObject, result);
            ldapEntry; ldapEntry = ldap_next_entry(m_ldapObject, ldapEntry))
        {
            LdapUserInfo user;
            user.dn = std::string(ldap_get_dn(m_ldapObject, ldapEntry));
            SPDLOG_DEBUG("[LdapManager] User DN : {}", user.dn);

            for(auto attr : m_searchAttrs) {
                berval ** value = ldap_get_values_len(m_ldapObject, ldapEntry, attr.c_str());
                int count = ldap_count_values_len(value);
                if(count > 0) {
                    SPDLOG_DEBUG("[LdapManager] User attr : {}", attr);
                    if(attr=="uid" || attr=="sAMAccountName")
                        user.uid = value[0]->bv_val;
                    else if(attr=="displayName")
                        user.name = value[0]->bv_val;
                    else if(attr=="mail")
                        user.email = value[0]->bv_val;
                    else if(attr=="objectclass"){
                        for(int i=0; i<count; i++){
                            if(m_userObjectClass.contains(value[i]->bv_val)) {
                                SPDLOG_DEBUG("[LdapManager] User objectclass : {}", value[i]->bv_val);
                                user.objectclass = value[i]->bv_val;
                                break;
                            }
                        }
                    }
                }
                ldap_value_free_len(value);
            }
            if(m_userObjectClass.contains(user.objectclass))
                res->fetchedUser.push_back(user);
        }

        res->connResult.connSuccess = true;
        res->connResult.searchSuccess = true;
        res->connResult.errorMsg = "";
    }
    else {
        SPDLOG_DEBUG("[LdapManager] SearchUser ldapsearch failed.");
        char *msg=NULL;
        ldap_get_option(m_ldapObject, LDAP_OPT_DIAGNOSTIC_MESSAGE, (void*)&msg);
        ldapError( "ldap_search_ext_s", errorCode, NULL, NULL, msg, NULL );
        ldap_memfree(msg);

        res->connResult.connSuccess = true;
        res->connResult.searchSuccess = false;
        res->connResult.errorMsg = ldap_err2string(errorCode);
    }

    if(ldap_attrs)
        delete [] ldap_attrs;
    ldap_unbind_ext_s(m_ldapObject, NULL, NULL);

    return res;
}


}
}

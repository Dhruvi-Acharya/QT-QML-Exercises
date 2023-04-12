#include <string>
#include <boost/core/ignore_unused.hpp>

#include <openssl/rsa.h>
#include <openssl/pem.h>

#include "cgis/BaseCommand.h"
#include "cgis/SunapiSyntaxBuilder.h"
#include "LogSettings.h"


namespace Wisenet
{
namespace Device
{
class SecurityRsa : public BaseCommand, public JsonParser
{
public:
    // 0. SecurityRsa 는
    // 1. 실패하는 경우 RAS Public Key 미지원되는 것으로 판단하고 평문으로 패스워드 변경
    // 2. 정상적으로 응답하는 경우 암호화하여 패스워드 변경
    // 따라서 실패에 대한 응답 처리를 하지 않고 다음 SUNAPI 진행함.
    explicit SecurityRsa(const std::string& logPrefix, DeviceChangeUserPasswordRequestSharedPtr request)
        :BaseCommand(this,logPrefix, std::make_shared<Wisenet::ResponseBase>(),true,true),
         m_request(request)
    {

    }

    ResponseBaseSharedPtr ProcessPost() override {
        // 성공 실패 여부와 상관없이 응답 처리를 하지 않음.
        return nullptr;
    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("security","rsa", "view");

        return builder.GetUrl();
    };

private:
    bool parseJson(rapidjson::Document& jsonDoc) override
    {
        rapidjson::Value::MemberIterator itr = jsonDoc.FindMember("PublicKey");
        if (itr != jsonDoc.MemberEnd())
            m_publicKey = itr->value.GetString();

        return true;
    }

public:
    DeviceChangeUserPasswordRequestSharedPtr m_request;
    std::string m_publicKey = "";
};

struct User
{
    std::string index;
};

class SecurityUserView : public BaseCommand, public IniParser
{
public:
    explicit SecurityUserView(const std::string& logPrefix, DeviceChangeUserPasswordRequestSharedPtr request, std::string userID)
        :BaseCommand(this,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request),
         m_userID(userID)
    {

    }

    std::string RequestUri() override
    {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("security","users", "view")
                .AddParameter("UserID", m_userID);

        return builder.GetUrl();
    }

    ResponseBaseSharedPtr ProcessPost() override
    {
        auto itor = m_users.find(m_userID);
        if(itor == m_users.end()) {
            m_responseBase->errorCode = Wisenet::ErrorCode::InvalidRequest;
            return m_responseBase;
        }
        //성공인 경우 응답처리를 하지 않아야 하므로 nullptr을 보낸다.
        return nullptr;
    }
private:
    bool parseINI(NameValueText& iniDoc) override
    {
        for(auto itor : iniDoc.m_value)
        {
            //Value Parsing
            //Camera (UserID/Password/Enable/VideoProfileAccess/PTZAccess/AudioInAccess/AudioOutAccess/AlarmOutputAccess)
            //NVR (?)
            std::vector<std::string> valueSplits;
            boost::split(valueSplits, itor.second, boost::is_any_of("/"));

            if(1 > valueSplits.size()) {
                SPDLOG_ERROR("Failed to find a exact value : {}", itor.second);
                continue;
            }

            //Index Parsing
            std::vector<std::string> keySplits;
            boost::split(keySplits, itor.first, boost::is_any_of("."));

            if(2 != keySplits.size()) {
                SPDLOG_ERROR("Failed to find a exact index : {}", itor.first);
                continue;
            }

            User user = {keySplits[1]};
            m_users.emplace(valueSplits[0], user);

            SPDLOG_INFO("Index of {} is {}", valueSplits[0] , user.index);
        }

         return true;
    }

    DeviceChangeUserPasswordRequestSharedPtr m_request;
    std::string m_userID;

public:
    std::unordered_map<std::string, User> m_users; // Key: User Name, Value: User Struct
};



class SecurityUserUpdate : public BaseCommand
{
public:
    explicit SecurityUserUpdate(const std::string& logPrefix, DeviceChangeUserPasswordRequestSharedPtr request, const std::string& index, const std::string& userID, bool isPasswordEncrypted, const std::string& password)
        :BaseCommand(nullptr,logPrefix,std::make_shared<Wisenet::ResponseBase>(),false,false),
         m_request(request),
         m_index(index),
         m_userID(userID),
         m_isPasswordEncrypted(isPasswordEncrypted),
         m_password(password)
    {

    }

    std::string RequestUri() override {
        SunapiSyntaxBuilder builder;
        builder.CreateCgi("security","users", "update")
                .AddParameter("Index", m_index)
                .AddParameter("UserID", m_userID);

        if(m_isPasswordEncrypted) {
            builder.AddParameter("IsPasswordEncrypted", "True");
        } else {
            builder.AddParameter("Password", m_password);
        }

#ifdef WISENET_S1_VERSION
        builder.AddParameter("ApplyAllCameras", "False");
#endif

        return builder.GetUrl();
    };

    std::string GetPostData() override
    {
        return m_password;
    }

    ResponseBaseSharedPtr ProcessPost() override {
       m_responseBase->errorCode = Wisenet::ErrorCode::NoError;
        return m_responseBase;
    }
private:
    DeviceChangeUserPasswordRequestSharedPtr m_request;
    std::string m_index;
    std::string m_userID;
    bool m_isPasswordEncrypted;

public:
    std::string m_password;
};

}
}

#pragma once

#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <random>
#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <openssl/md5.h>
#include <openssl/sha.h>
#include "LogSettings.h"
#include "TextEncodeUtil.h"

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

class DigestAuthenticator
{
public:
    DigestAuthenticator()
        : m_hasAuthorization(false)
        , m_retryCount(0)
        , m_isMD5(false)
        , m_nc(0)
    {}

    ~DigestAuthenticator() = default;

    bool GenerateAuthorization(
            const std::string& wwwAuthenticate,
            const std::string& user, const std::string& password)
    {
        const std::string authType = "Digest";
        std::size_t found = wwwAuthenticate.find(authType);
        if (found == std::string::npos)
            return false;

        // remove digest
        std::string digestAuthLine = wwwAuthenticate.substr(authType.length() + 1, wwwAuthenticate.length());

        boost::char_separator<char> sep(",");
        tokenizer tokens(digestAuthLine, sep);
        m_algorithm = "MD5";
        m_qop = "";
        m_nc = 0;
        m_retryCount = 0;

        for (tokenizer::iterator tok_iter = tokens.begin();
             tok_iter != tokens.end(); ++tok_iter) {
            std::string token = boost::trim_copy(*tok_iter);
            boost::erase_all(token, "\"");

            std::size_t findingLength = token.find("=");
            if (findingLength == std::string::npos)
                continue;

            std::string key = token.substr(0, findingLength);
            std::string value = token.substr(findingLength + 1);

            boost::to_lower(key);

            if (key == "algorithm")
                m_algorithm = value;
            else if (key == "realm")
                m_realm = value;
            else if (key == "qop")
                m_qop = value;
            else if (key == "nonce")
                m_nonce = value;
            else if (key =="opaque")
                m_opaque = value;
        }

        if (boost::iequals(m_algorithm, "MD5"))
            m_isMD5 = true;
        else if (boost::iequals(m_algorithm, "SHA-256"))
            m_isMD5 = false;
        else
            return false;

        boost::to_upper(m_algorithm);

        if (m_qop != "auth" && !m_qop.empty())
            return false;

        m_user = user;
        m_password = password;
        m_cnonce = GenerateNonce();

        m_hasAuthorization = true;
        return true;
    }

    void UpdateAuthorization(const std::string& method, const std::string& uri)
    {
        std::string ha1 = m_user + ":" + m_realm + ":" + m_password;
        std::string ha2 = method + ":" + uri;

        std::string ha1Hash;
        std::string ha2Hash;
        std::string responseHash;
        std::string nc;

        ha1Hash = hash(ha1);
        ha2Hash = hash(ha2);

        if (m_qop.empty()) {
            responseHash = hash(ha1Hash+":"+m_nonce+":"+ha2Hash);
        }
        else {
            m_nc++;
            m_retryCount++;
            std::stringstream ss;
            ss << std::hex << std::setw(8) << std::setfill('0') << m_nc;
            nc = ss.str();

            responseHash = hash(ha1Hash+":"+m_nonce+":"+nc+":"+m_cnonce+":auth:"+ha2Hash);
        }

        std::stringstream authStream;
        authStream << "Digest username=\"" + m_user + "\", ";
        authStream << "realm=\"" + m_realm + "\", ";
        authStream << "nonce=\"" + m_nonce + "\", ";
        authStream << "uri=\"" + uri + "\", ";
        authStream << "algorithm=\"" + m_algorithm + "\", ";
        if(!m_cnonce.empty())
            authStream << "cnonce=\"" + m_cnonce + "\", ";
        if (!m_qop.empty())
            authStream << "nc=" + nc + ", ";
        if(!m_opaque.empty())
            authStream << "opaque=" << m_opaque << ", ";
        authStream << "response=\"" + responseHash + "\"";
        if(!m_qop.empty())
            authStream << ", qop=\"auth\"";

        m_authorization = authStream.str();
    }

    std::string authorization() const noexcept { return m_authorization; }
    bool hasAuthorization() const noexcept { return m_hasAuthorization; }

    void ResetAuthorization()
    {
        m_hasAuthorization = false;
        m_authorization = "";
    }

    static std::string GenerateNonce()
    {
        std::random_device rd;
        std::uniform_int_distribution<unsigned short> length{8, 32};
        std::uniform_int_distribution<unsigned short> distNum{0, 15};

        std::string nonce;
        nonce.resize(length(rd));
        constexpr char hex[16]{'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        for (char& val : nonce) {
            val = hex[distNum(rd)];
        }
        return nonce;
    }

    unsigned int GetRetryCount() const
    {
        return m_retryCount;
    }

    void ResetRetryCount()
    {
        m_retryCount = 0;
    }

    void SetRetryCount(const unsigned int count)
    {
        m_retryCount = count;
    }

    static std::string hashSha256(const std::string& data)
    {
        std::stringstream ss;
        unsigned char hash[SHA256_DIGEST_LENGTH] ="";
        SHA256_CTX sha256Ctx;
        SHA256_Init(&sha256Ctx);
        SHA256_Update(&sha256Ctx, data.c_str(), data.size());
        SHA256_Final(hash, &sha256Ctx);

        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

    static std::string hashSpecialSha256(const std::string &salt, const std::string& data)
    {
        std::string tempData = Wisenet::Common::Base64Encode(salt+data);
        std::string hashFirst = hashSha256(tempData + tempData);
        std::string hashSecond = hashSha256(hashFirst + tempData);
        std::string hashThird = hashSha256(hashSecond);
        return hashThird;
    }

    std::string hashMd5(const std::string& data)
    {
        std::stringstream ss;
        unsigned char hash[MD5_DIGEST_LENGTH] ="";
        MD5_CTX  md5Ctx;
        MD5_Init(&md5Ctx);
        MD5_Update(&md5Ctx, data.c_str(), data.size());
        MD5_Final(hash, &md5Ctx);

        for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }
        return ss.str();
    }

private:
    std::string hash(const std::string& data)
    {
        if (m_isMD5) {
            return hashMd5(data);
        }
        else {
            return hashSha256(data);
        }
    }

private:
    std::string m_user;
    std::string m_password;
    std::string m_qop;
    std::string m_algorithm;
    std::string m_realm;
    std::string m_nonce;
    std::string m_cnonce;
    std::string m_opaque;
    std::string m_authorization;
    bool        m_isMD5 = false;
    bool        m_hasAuthorization = false;
    unsigned int m_nc = 0;
    unsigned int m_retryCount = 0;
};

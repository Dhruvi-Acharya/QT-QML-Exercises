#include "SecurityManager.h"
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <ctype.h>
#include <openssl/md5.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>


#include "LogSettings.h"

namespace Wisenet
{
namespace Library
{

std::string SecurityManager::EncryptPassword(std::string publicKey, std::string password)
{
    SPDLOG_INFO("{}", publicKey);

    BIO* bio = BIO_new_mem_buf((void*)publicKey.c_str(), -1);
    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
    if (!bio)
    {
        SPDLOG_ERROR("BIO is null...");
        return "";
    }

    RSA* rsa = PEM_read_bio_RSAPublicKey(bio, NULL, NULL, NULL);
    if (!rsa)
    {
        SPDLOG_ERROR("RSA is null...");
        BIO_free(bio);
        return "";
    }

    BIO_free(bio);

    int maxSize = RSA_size(rsa);
    unsigned char* output_binary = new unsigned char[maxSize * sizeof(char)];
    int bytes = RSA_public_encrypt((int)(password.length()), (const unsigned char*)password.c_str(), output_binary, rsa, RSA_PKCS1_PADDING);
    std::string output("");

    if (bytes != -1) {
        output.insert(0, reinterpret_cast<const char*>(output_binary), bytes);

    }
    else {
        SPDLOG_ERROR("Failed to encrypt a password.");
    }

    delete[] output_binary;
    RSA_free(rsa);

    return output;
}

}
}

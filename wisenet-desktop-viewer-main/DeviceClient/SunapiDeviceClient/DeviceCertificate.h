#pragma once
#include <boost/asio/ssl.hpp>
#include <openssl/pkcs12.h>
#include <mutex>

struct XPfx
{
    EVP_PKEY *pkey;
    X509 *cert;
};

class DeviceCertificate {
public:
    static DeviceCertificate& getInstance() {
        static DeviceCertificate s;
        return s;
    }

public:

    void SetRootCA(const std::vector<std::string> path);
    void CheckRootCA();
    std::string CheckDeviceCertificate(X509* certificate, std::string& resultMacAddress);
    void ExportKey(const std::string filePath, const std::string passphrase);
    void* GetCertificate() { return (void*)m_certficate; }
private:
    std::string AddRootCAToStore(X509* cert);
    std::string VerifyDeviceCertification(X509* cert);
    DeviceCertificate() ;
    ~DeviceCertificate() ;
private:
    std::vector<std::string>	m_rootCA;
    std::string                 m_rootCAValidation;
    X509_STORE*                 m_store;
    X509_STORE_CTX *            m_storeCtx;
    XPfx*                       m_certficate;
    std::mutex  m_mutex;
};

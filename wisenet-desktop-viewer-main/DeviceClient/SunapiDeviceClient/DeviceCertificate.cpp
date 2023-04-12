#include "DeviceCertificate.h"
#include "LogSettings.h"
#include <iostream>
#include <fstream>
#include <pugixml.hpp>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#ifdef _WIN32
#include <openssl/applink.c>
#endif
//#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>


DeviceCertificate::DeviceCertificate()
{
    SPDLOG_DEBUG("DeviceCertificate::DeviceCertificate()");
    m_store = X509_STORE_new();
    m_storeCtx = X509_STORE_CTX_new();
	m_certficate = nullptr;
    m_rootCAValidation = "";
}

DeviceCertificate::~DeviceCertificate()
{
    X509_STORE_free(m_store);
    X509_STORE_CTX_free(m_storeCtx);
    if(m_certficate)
        delete m_certficate;
    m_certficate = nullptr;
}

static time_t ASN1_GetTimeT(ASN1_TIME* time) {
	struct tm t;
	const char* str = (const char*)time->data;
	size_t i = 0;

	memset(&t, 0, sizeof(t));

	if (time->type == V_ASN1_UTCTIME) {/* two digit year */
		t.tm_year = (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		if (t.tm_year < 70)
			t.tm_year += 100;
	}
	else if (time->type == V_ASN1_GENERALIZEDTIME) {/* four digit year */
		t.tm_year = (str[i++] - '0') * 1000;
		t.tm_year += (str[i++] - '0') * 100;
		t.tm_year += (str[i++] - '0') * 10;
		t.tm_year += (str[i++] - '0');
		t.tm_year -= 1900;
	}
	t.tm_mon = (str[i++] - '0') * 10;
	t.tm_mon += (str[i++] - '0') - 1; // -1 since January is 0 not 1.
	t.tm_mday = (str[i++] - '0') * 10;
	t.tm_mday += (str[i++] - '0');
	t.tm_hour = (str[i++] - '0') * 10;
	t.tm_hour += (str[i++] - '0');
	t.tm_min = (str[i++] - '0') * 10;
	t.tm_min += (str[i++] - '0');
	t.tm_sec = (str[i++] - '0') * 10;
	t.tm_sec += (str[i++] - '0');

	/* Note: we did not adjust the time based on time zone information */
	return mktime(&t);
}

bool IsHTWDeviceCerificate(X509* cert)
{
	if (cert == NULL) return false;

    char issuer_name[256] = "";
	X509_NAME_oneline(X509_get_issuer_name(cert), issuer_name, 256);
	std::string issuerName = std::string(issuer_name);
    SPDLOG_DEBUG("IsHTWDeviceCerificate ={} ",issuerName);
    //if (boost::contains(issuerName, "Hanwha Techwin Private Root CA"))
    if(issuerName.find("Hanwha Techwin Private Root CA") != std::string::npos || issuerName.find("Hanwha Vision Private Root CA 3") != std::string::npos)
		return true;
	else
        return false;
}


std::string GetMacAddressByCertificate(X509* cert)
{
	if (cert == NULL) return "";
    char subject_name[256] = "";
	X509_NAME_oneline(X509_get_subject_name(cert), subject_name, 256);
	std::string subName = std::string(subject_name);

    std::vector<std::string> orgTokens;
    boost::split(orgTokens, subName, boost::is_any_of("/"));

    std::string delimiter = "CN=";
    for(auto item : orgTokens){
        if(item.find(delimiter) != std::string::npos)
        {
            item.erase(0, item.find(delimiter) + delimiter.length());
            std::vector<std::string> tokens;
            boost::split(tokens, item, boost::is_any_of("."));
            if (tokens.size() > 0){
                SPDLOG_DEBUG("GetMacAddressByCertificate new = {}", tokens.at(0));
                return tokens.at(0);
            }
            else
                return "";
        }
    }
    return "";
}

bool IsExpired(X509* cert)
{
    auto before = ASN1_GetTimeT(X509_getm_notBefore(cert));
    auto after = ASN1_GetTimeT(X509_getm_notAfter(cert));

	auto now = std::time(0);
	if (now < before || now > after)
		return true;
	else
		return false;
}

constexpr char hexmap[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f' };

std::string hexStr(unsigned char *data, int len)
{
	std::string s(len * 2, ' ');
	for (int i = 0; i < len; ++i) {
		s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
		s[2 * i + 1] = hexmap[data[i] & 0x0F];
	}
	return s;
}


void DeviceCertificate::CheckRootCA()
{	
    m_rootCAValidation ="";
    for(auto rootCa: m_rootCA)
	{
		auto rootCAPath = rootCa.c_str();
		if (rootCAPath)
		{
			auto fp = fopen(rootCAPath, "r");
			if (fp)
			{
                SPDLOG_DEBUG("DeviceCertificate::CheckRootCA() path: {}", rootCAPath);
				auto rootCA = PEM_read_X509(fp, NULL, NULL, NULL);
				if (rootCA != NULL)
				{
					auto digest = EVP_get_digestbyname("sha1");
                    unsigned char         md[EVP_MAX_MD_SIZE] = "";
                    unsigned int          len = 0;
					int rc = X509_digest((X509*)rootCA, digest, md, &len);

					std::string fingerprint = hexStr(md, len);

                    SPDLOG_DEBUG("DeviceCertificate::CheckRootCA() fingerPrint:{}", fingerprint);
					if (rc == 0 || !X509_check_ca(rootCA) ||
						!(fingerprint == std::string("cd28acd4f05793ef43aa524df30404fd0b7f3a27")  || 
                          fingerprint == std::string("fa47e60b94968ea191bca8ec9514aecdb2832545")  ||
                          fingerprint == std::string("f3fb66fb5eac2f59447b27afe5cd2874383377f4")
                          ) )
                        m_rootCAValidation = "Invalid root CA";
					else if (IsExpired((X509*)rootCA))
                        m_rootCAValidation = "Expired root CA";
					else
                        m_rootCAValidation = AddRootCAToStore(rootCA);
					X509_free(rootCA);
				}
				fclose(fp);
			}
			else
                m_rootCAValidation = "Invalid root CA";
		}
	}
    SPDLOG_DEBUG("DeviceCertificate::CheckRootCA() {}", m_rootCAValidation);
}

std::string DeviceCertificate::CheckDeviceCertificate(X509* certificate, std::string& resultMacAddress)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);
    auto cert = (X509*)certificate;

	if (X509_check_ca(cert))
	{
        SPDLOG_DEBUG("CheckDeviceCertificate SSL_STATUS_SUPPORT_SELF_SIGNED_CERTIFICATE");
        return "Self Signed Certificate";
	}
	
	if (!IsHTWDeviceCerificate(cert))
	{
        SPDLOG_DEBUG("CheckDeviceCertificate SSL_STATUS_UNKNOWN_CERTIFICATE");
        return "Unknown Certificate";
	}
    resultMacAddress = GetMacAddressByCertificate(cert);

    SPDLOG_DEBUG("CDeviceCertificate::CheckDeviceCertificate resultMacAddress={}",resultMacAddress);

	if (IsExpired(cert))
	{
        SPDLOG_DEBUG("CheckDeviceCertificate SSL_STATUS_HTW_DEVICE_CERTIFICATE_EXPIRED");
        return "Device Certificate expired";
	}

    if (m_rootCAValidation.length() >0)
		return m_rootCAValidation;

	return VerifyDeviceCertification(cert);
}


void DeviceCertificate::SetRootCA(const std::vector<std::string> path)
{
	m_rootCA = path;
}

std::string DeviceCertificate::AddRootCAToStore(X509* cert)
{
    if (!X509_STORE_add_cert(m_store, cert))
	{
        return "Htw root CA stored fail";
	}
    return "";
}

std::string DeviceCertificate::VerifyDeviceCertification(X509* cert)
{
	std::string msg;
    X509_STORE_CTX_init(m_storeCtx, m_store, NULL, NULL);
    X509_STORE_CTX_set_cert(m_storeCtx, cert);
    X509_verify_cert(m_storeCtx);

    int sslRet = X509_STORE_CTX_get_error(m_storeCtx);
	if (sslRet != X509_V_OK)
	{
        SPDLOG_DEBUG("CheckDeviceCertificate SSL_STATUS_INVALID_HTW_DEVICE_CERTIFICATE_CHAIN");
		const char* err = X509_verify_cert_error_string(sslRet);
		if (err)
			msg = std::string(err);
        return "Invalid Htw Device Certificate chain";
	}
    SPDLOG_DEBUG("CheckDeviceCertificate Htw device certified");
    return "Htw device certified";
}

void DeviceCertificate::ExportKey(const std::string filePath, const std::string passphrase)
{	
    FILE *fp = nullptr;

    if ((fp = fopen(filePath.c_str(), "rb")) == nullptr)
    {
        SPDLOG_WARN("CheckDeviceCertificate ExportKey : Error opening file: {}",  filePath);
        return;
    }

    PKCS12* p12 = d2i_PKCS12_fp(fp, NULL);
    fclose(fp);

	if (!p12)
	{
		fprintf(stderr, "Error reading PKCS#12 file\n");
		ERR_print_errors_fp(stderr);
        SPDLOG_WARN("CheckDeviceCertificate Error reading PKCS#12 file ");
        return;
	}

    EVP_PKEY *pkey = nullptr;
    X509 *publicCert = nullptr;
	STACK_OF(X509) *ca;
	
	if (!PKCS12_parse(p12, passphrase.c_str(), &pkey, &publicCert, &ca))
	{
		fprintf(stderr, "Error parsing PKCS#12 file\n");
		ERR_print_errors_fp(stderr);
        SPDLOG_WARN("CheckDeviceCertificate Error parsing PKCS#12 file ");
        return;
	}
	
    if (!publicCert && VerifyDeviceCertification(publicCert) == "SSL_STATUS_SUPPORT_HTW_DEVICE_CERTIFICATE")
	{
		if (m_certficate == nullptr)
		{
			m_certficate = new XPfx;

            if(!publicCert)
                m_certficate->cert = publicCert;

            if(!pkey)
                m_certficate->pkey = pkey;

            SPDLOG_DEBUG("CheckDeviceCertificate Success");
		}
	}
	else
        SPDLOG_DEBUG("CheckDeviceCertificate VerifyDeviceCertification Fail : ");
}

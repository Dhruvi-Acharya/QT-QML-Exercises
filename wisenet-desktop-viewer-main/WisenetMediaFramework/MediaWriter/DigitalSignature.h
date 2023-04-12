#ifndef DIGITALSIGNATURE_H
#define DIGITALSIGNATURE_H

#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <QString>
#include <QFile>

class DigitalSignature
{
public:
    DigitalSignature();
    ~DigitalSignature();

    int openPfxFile(QString strPassword, QString strFilePath); // 백업 뷰어에서는 strFilePath를 TCHAR 타입으로 사용함
    int getDerData(unsigned char** derData);
    bool createSignature(QFile* file, QDataStream* dataStream, unsigned char **signature, size_t* signatureLen, qint64 signDataPos);
    bool isOpenPfx() {return pfxOpened;};
    qint32 getKeyLength() {return keyLength;};

private:
    bool pfxOpened;
    qint32 keyLength;

    // used for creating signature
    PKCS12* p12;
    EVP_PKEY* pkey;
    X509* cert;
    RSA* rsaPrivateKey;

};

#endif // DIGITALSIGNATURE_H

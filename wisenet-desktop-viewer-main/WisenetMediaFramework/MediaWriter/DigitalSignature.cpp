#include "DigitalSignature.h"
#include "LogSettings.h"

#include <QDebug>
#include <QDataStream>
#include <QElapsedTimer>

DigitalSignature::DigitalSignature()
    :pfxOpened(false), keyLength(0), p12(NULL), pkey(NULL),cert(NULL),rsaPrivateKey(NULL)
{
    //OpenSSL_add_all_algorithms();
    //ERR_load_crypto_strings();
}

DigitalSignature::~DigitalSignature()
{
    if(p12)
        PKCS12_free(p12);
    if(cert)
        X509_free(cert);
    if(pkey)
        EVP_PKEY_free(pkey);
    if(rsaPrivateKey)
        RSA_free(rsaPrivateKey);

}

/*
아규먼트로 받은 경로에 있는 pfx 파일을 열어서 private key를 뽑아내는 함수
*/
int DigitalSignature::openPfxFile(QString strPassword, QString strFilePath)
{
    FILE * fp;
    STACK_OF(X509) *ca = NULL;
    //qDebug() << "DigitalSignature::OpenPfxFile start pfx FilePath" << strFilePath;

    if(!(fp = fopen(strFilePath.toStdString().c_str(), "rb"))) {
        return -3;
    }

    p12 = d2i_PKCS12_fp(fp, NULL); // pfx 파일을 load
    fclose(fp);

    if(p12 == NULL)
    {
        qDebug() << "DigitalSignature::OpenPfxFile Error reading PKCS#12 file.";
        return -5;
    }

    if(!PKCS12_parse(p12, strPassword.toStdString().c_str(), &pkey, &cert, &ca)) // ca는 null 값으로 넘김, pkey를 얻음
    {
        qDebug() << "DigitalSignature::OpenPfxFile Error parsing PKCS#12 file.";

        unsigned long errcode;
        while((errcode = ERR_get_error()) != 0){
            qDebug() << "DigitalSignature::OpenPfxFile openssl error: " << ERR_error_string(errcode, NULL);
        }

        return -4;
    }

    if(cert==NULL) // getDerData함수에서 cert 필요
    {
        qDebug() << "DigitalSignature::OpenPfxFile Error cert is NULL";
        return -4;
    }

    if(pkey == NULL)
    {
        qDebug() << "DigitalSignature::OpenPfxFile Error EVP_KEY is NULL.";
        return -5;
    }

    rsaPrivateKey = EVP_PKEY_get1_RSA(pkey);

    if(rsaPrivateKey == NULL)
    {
        qDebug() << "DigitalSignature::OpenPfxFile Error RSA is NULL.";
        return -5;
    }

    keyLength = RSA_size(rsaPrivateKey); // RSA modulus size를 바이트 단위로 리턴, RSA로 암호화된 값을 위해 메모리가 얼마나 할당되는지를 결정함.
    pfxOpened = true;

    return 0;
}

int DigitalSignature::getDerData(unsigned char** derData) // 인증서를 der포맷으로 변환후 인증서 길이를 반환하는 함수
{
    if(cert == NULL)
        return 0;

    if(!X509_check_private_key(cert, pkey))
    {
        qDebug() << "DigitalSignature::getDerData X509 check privateKey is false";
        return 0;
    }
    else{
        qDebug() << "DigitalSignature::getDerData X509 check privateKey is true.";

        int derLength = i2d_X509(cert, derData);
        qDebug() << "DigitalSignature::getDerData derLength" << derLength;

        return derLength;
    }
}


bool DigitalSignature::createSignature(QFile* file, QDataStream* dataStream, unsigned char** signature, size_t* signatureLen, qint64 signDataPos)
{
    if(signature == nullptr || *signatureLen < keyLength || pkey == nullptr)
        return false;

    char msg[1024] = {0,};
    size_t msgLen = 1024;
    int dataLen = 0;

    EVP_MD_CTX* RSASignCtx = EVP_MD_CTX_create();

    if(EVP_DigestSignInit(RSASignCtx, NULL, EVP_sha256(), NULL, pkey) <= 0) {
        qDebug() << "DigitalSignature::createSignature -> EVP_DigestInit() Failed";
        return false;
    }

    qDebug() << "DigitalSignature::createSignature curernt file position" << file->pos();

    while ((dataLen = dataStream->readRawData(msg,msgLen)) > 0) {
        //qDebug() << "DigitalSignature::createSignature -> readRawData() dataLength : " << dataLen;
        if (file->pos() > signDataPos) { // 1024 바이트 읽었더니 위치가 signdata 시작 위치 건넜을 때
            qDebug() << "DigitalSignature::createSignature curernt file position" << file->pos() << signDataPos;

            // 파일 포인터 이전으로 되돌리기
            file->seek(file->pos() - msgLen);
            // 앞부분 길이 구하고 readRawData
            int formerDataLen = signDataPos - file->pos();
            dataStream->readRawData(msg, formerDataLen);

            // 뒷부분 길이 구하기 readRawData
            file->seek(signDataPos + 256);

            int latterDataLen = msgLen - formerDataLen;
            char * data = new char[latterDataLen];
            dataStream->readRawData(data, latterDataLen);

            // 두 배열 합치기 memcpy
            std::memcpy(msg + formerDataLen, data, latterDataLen);

            delete[] data;
            if (EVP_DigestSignUpdate(RSASignCtx, msg, dataLen) <= 0)
                return false;

            break;
        }
        else if(file->pos() < signDataPos) { // 1024바이트 읽고 나서 위치가 사인 데이터 위치보다 작을 때
            qDebug() << "DigitalSignature::createSignature curernt file position" << file->pos() << signDataPos;

            if (EVP_DigestSignUpdate(RSASignCtx, msg, dataLen) <= 0)
                return false;
        }
        else { // 1024 바이트 읽고나서 파일 포인터 위치가 사인 데이터 시작점일때
            if (EVP_DigestSignUpdate(RSASignCtx, msg, dataLen) <= 0)
                return false;

            // 파일 포인터를 서명 데이터 뒤로 변경
            file->seek(signDataPos + 256);
            break;
        }
    }

    qDebug() << "DigitalSignature::createSignature curernt file position - while next" << file->pos();

    while ((dataLen = dataStream->readRawData(msg,msgLen)) > 0) {
        if (EVP_DigestSignUpdate(RSASignCtx, msg, dataLen) <= 0)
            return false;
    }

    if(EVP_DigestSignFinal(RSASignCtx, NULL, signatureLen) <= 0){
        return false;
    }

    *signature = new unsigned char[*signatureLen];

    if(EVP_DigestSignFinal(RSASignCtx, *signature, signatureLen) <= 0){
        return false;
    }

    EVP_MD_CTX_free(RSASignCtx);

    return true;
}



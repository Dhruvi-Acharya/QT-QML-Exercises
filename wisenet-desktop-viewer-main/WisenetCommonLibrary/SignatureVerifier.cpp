#include "SignatureVerifier.h"
#include "LogSettings.h"
#include "QCoreServiceManager.h"
#include <memory>
#include <thread>
#include <boost/algorithm/string/predicate.hpp>

#include "ThreadPool.h"
#include "WeakCallback.h"

#include <QDebug>

SignatureVerifier::SignatureVerifier(QObject *parent)
    : QObject(parent)
    , m_cert(NULL), m_pubKey(NULL)
    , m_contextPool(std::make_shared<ThreadPool>(2))
    , m_strand(m_contextPool->ioContext().get_executor())
    , m_isRunning(true)

{
    SPDLOG_INFO("SignatureVerifier is created.");

    m_contextPool->Start();
}

SignatureVerifier::~SignatureVerifier()
{
    if(m_cert) {
        X509_free(m_cert);
        SPDLOG_INFO("~SignatureVerifier freed certificate.");
    }

    if(m_pubKey) {
        EVP_PKEY_free(m_pubKey);
        SPDLOG_INFO("~SignatureVerifier freed public key.");
    }

    m_contextPool->Stop();
}

void SignatureVerifier::Stop(){
    SPDLOG_INFO("Stop SignatureVerifier instance.");
    m_isRunning = false;
}

bool SignatureVerifier::openWnm()
{
    // check extension
    int extensionIndex = m_localResource.lastIndexOf(".");
    if(extensionIndex != -1 && extensionIndex != m_localResource.length() -1)
    {
        QString extension = m_localResource.right(m_localResource.length() - extensionIndex -1);
        if(extension.toLower() != "wnm")
        {
            SPDLOG_ERROR("SignatureVerifier::openWnm() File extension error. fileName={}", m_localResource.toLocal8Bit().toStdString());
            return false;
        }
    }

    // File open
    m_file.setFileName(m_localResource);
    if(m_file.open(QIODevice::ReadOnly) == false)
    {
        SPDLOG_ERROR("SignatureVerifier::openWnm() File open error. fileName={}", m_localResource.toLocal8Bit().toStdString());
        return false;
    }
    m_dataStream.setDevice(&m_file);

    // Load FileOffsetIndex
    qint32 offsetEntryCount = 0;
    m_dataStream >> offsetEntryCount;

    for(int i=0 ; i<offsetEntryCount ; i++)
    {
        WnmElementType elementType = WnmElementType::None;
        m_dataStream >> elementType;
        m_dataStream >> m_fileOffsetIndex[elementType].startOffset;
        m_dataStream >> m_fileOffsetIndex[elementType].nextElementOffset;
    }

    // Load Digital Signing
    loadDigitalSigning();

    return true;
}

void SignatureVerifier::releaseWnm()
{
    if(m_file.isOpen())
        m_file.close();

    if(m_dataStream.device() != nullptr)
        m_dataStream.setDevice(nullptr);

    if(m_signingDataPtr != nullptr)
    {
        delete[] m_signingDataPtr;
        m_signingDataPtr = nullptr;
        m_signingDataSize = 0;
    }

    if(m_certificateDataPtr != nullptr)
    {
        delete[] m_certificateDataPtr;
        m_certificateDataPtr = nullptr;
        m_certificateDataSize = 0;
    }

    if(m_localResource != NULL)
        m_localResource = "";

    m_fileOffsetIndex.clear();
}

void SignatureVerifier::loadDigitalSigning()
{

    if(m_signingDataPtr != nullptr)
    {
        delete[] m_signingDataPtr;
        m_signingDataPtr = nullptr;
        m_signingDataSize = 0;
    }

    if(m_certificateDataPtr != nullptr)
    {
        delete[] m_certificateDataPtr;
        m_certificateDataPtr = nullptr;
        m_certificateDataSize = 0;
    }

    if (!m_file.isOpen() || m_dataStream.device() == nullptr || !m_fileOffsetIndex.contains(WnmElementType::DigitalSigning))
        return; // 파일이 정상 open 되지 않았거나, DigitalSigning offset정보가 없는 경우

    // seek to DigitalSigning offset
    m_file.seek(m_fileOffsetIndex[WnmElementType::DigitalSigning].startOffset);

    // Read signing data
    m_signingDataSize = 0;
    m_dataStream >> m_signingDataSize;
    if(m_signingDataSize > 0)
    {
        m_signingFilePos = m_file.pos();
        m_signingDataPtr = new char[m_signingDataSize];
        m_dataStream.readRawData(m_signingDataPtr, m_signingDataSize);
    }

    // Read certificate data
    m_certificateDataSize = 0;
    m_dataStream >> m_certificateDataSize;
    if(m_certificateDataSize > 0)
    {
        m_certificateDataPtr = new char[m_certificateDataSize];
        m_dataStream.readRawData(m_certificateDataPtr, m_certificateDataSize);
    }
}

bool SignatureVerifier::getPublicKey(const unsigned char* derData, long derDataLength)
{
    // const unsigned char * 형태로 저장된 인코딩된 der 인증서를 X509 구조체로 인증서를 가져와서
    // 인증서에서 evp pub key 구조체 형식의 publicKey를 extract.

    if (m_cert != NULL && m_pubKey != NULL)
        return true;

    m_cert = d2i_X509(NULL, &derData, derDataLength);
    if (m_cert == NULL) {
        SPDLOG_ERROR("SignatureVerifier::getPublicKey - Failed to load der certificate.");
        return false;
    }

    m_pubKey = X509_get_pubkey(m_cert);
    if (m_pubKey == NULL) {
        SPDLOG_ERROR("SignatureVerifier::getPublicKey - Failed to load public Key from X509 cert.");
        return false;
    }

    qDebug() << "SignatureVerifier::getPublicKey Success.";
    return true;
}

int SignatureVerifier::verifySignature(QDataStream* dataStream, unsigned char* signingData, int signingDataSize)
{
    if(signingDataSize == 0) // 서명 정보가 없을 경우 리턴
        return -1;

    int dataLen = 0;
    char msg[1024];
    size_t msgLen = 1024;

    EVP_MD_CTX* RSAVerifyCtx = EVP_MD_CTX_new();
    if(RSAVerifyCtx == nullptr)
        return -1;

    if (EVP_DigestVerifyInit(RSAVerifyCtx, NULL, EVP_sha256(), NULL, m_pubKey) <= 0) {
        EVP_MD_CTX_free(RSAVerifyCtx);
        return -1;
    }

    qDebug() << "SignatureVerifier::verifySignature curernt file position" << m_file.pos();

    while ((dataLen = dataStream->readRawData(msg,msgLen)) > 0) {
        //qDebug() << "SignatureVerifier::verifySignature -> readRawData() dataLength : " << dataLen;
        if (m_file.pos() > m_signingFilePos) { // 1024 바이트 읽었더니 위치가 signdata 시작 위치 건넜을 때
            qDebug() << "SignatureVerifier::verifySignature curernt file position" << m_file.pos() << m_signingFilePos;

            // 파일 포인터 이전으로 되돌리기
            m_file.seek(m_file.pos() - 1024);

            // 앞부분 길이 구하고 readRawData
            int formerDataLen = m_signingFilePos - m_file.pos();
            dataStream->readRawData(msg, formerDataLen);

            // 뒷부분 길이 구하기 readRawData
            m_file.seek(m_signingFilePos + 256);

            int latterDataLen = 1024 - formerDataLen;
            char * data = new char[latterDataLen];
            memset(data, 0, latterDataLen);
            dataStream->readRawData(data, latterDataLen);

            // 두 배열 합치기 memcpy
            std::memcpy(msg + formerDataLen, data, latterDataLen);
            delete[] data;

            if (EVP_DigestSignUpdate(RSAVerifyCtx, msg, dataLen) <= 0){
                EVP_MD_CTX_free(RSAVerifyCtx);
                return  -1;
            }
            break;
        }
        else if(m_file.pos() < m_signingFilePos) { // 1024바이트 읽고 나서 위치가 사인 데이터 위치보다 작을 때
            qDebug() << "SignatureVerifier::verifySignature curernt file position" << m_file.pos() << m_signingFilePos;

            if (!m_isRunning || EVP_DigestSignUpdate(RSAVerifyCtx, msg, dataLen) <= 0){
                EVP_MD_CTX_free(RSAVerifyCtx);
                return  -1;
            }
        }
        else { // 1024 바이트 읽고나서 파일 포인터 위치가 사인 데이터 시작점일때
            if (EVP_DigestSignUpdate(RSAVerifyCtx, msg, dataLen) <= 0){
                EVP_MD_CTX_free(RSAVerifyCtx);
                return  -1;
            }

            // 파일 포인터를 서명 데이터 뒤로 변경
            m_file.seek(m_signingFilePos + 256);
            break;
        }
    }

    qDebug() << "SignatureVerifier::verifySignature curernt file position" << m_file.pos();

    while ((dataLen = dataStream->readRawData(msg,msgLen)) > 0) {
        if (!m_isRunning || EVP_DigestSignUpdate(RSAVerifyCtx, msg, dataLen) <= 0){
            EVP_MD_CTX_free(RSAVerifyCtx);
            return  -1;
        }
    }


    // 최종 해쉬값과 서명 비교
    int authStatus = EVP_DigestVerifyFinal(RSAVerifyCtx, signingData, (size_t)signingDataSize);
    EVP_MD_CTX_free(RSAVerifyCtx);

    if (authStatus == 1){
        qDebug() << "SignatureVerifier::verifySignature -> This signature is authentic.";
        return 4;
    }
    else if (authStatus == 0) {
        char buf = ' ';
        ERR_error_string(ERR_get_error(), &buf);
        qDebug() << "SignatureVerifier::verifySignature -> Verification Failed. - 1" << buf;
        return 5;
    }
    else {
        qDebug() << "SignatureVerifier::verifySignature -> Verification Failed. - 2";
        return 0;
    }
}

void SignatureVerifier::verify(QString filePath)
{
    // 파일 읽어서 m_file, m_dataStream 값 할당
    // loadDigitalSigning으로 디지털 서명 관련 값들 읽기
    // 서명 값 있을 경우에 verifySignature 함수 실행하기
    // return value -> authentic:4, failure:5, serious error:0, not signed file:6, 서명 검증 과정 완수하지 못할 때: -1
    boost::asio::post(m_strand, [this, filePath]()
    {
        int verifyResult = 0;
        char buffer256[256];
        std::memset(buffer256, 0x00, 256);

        if(m_localResource != NULL)
            m_localResource = "";

        m_localResource = filePath;

        if(!openWnm())
            verifyResult = -1;
        else {
            if(m_signingDataPtr != nullptr && m_certificateDataPtr != nullptr) // if this wnm file is signed
            {
                if(!getPublicKey(reinterpret_cast<const unsigned char*>(m_certificateDataPtr), (long)m_certificateDataSize))
                    SPDLOG_ERROR("SignatureVerifier::verify() - Failed to get public key from der Data.");
                else  // have proper cert file
                {
                    SPDLOG_INFO("SignatureVerifier::verify() - Success to get public key from der data.");

                    // 파일 처음 가리키도록 한 후 서명 검증 함수 호출
                    m_file.seek(0);
                    verifyResult = verifySignature(&m_dataStream, reinterpret_cast<unsigned char*>(m_signingDataPtr), m_signingDataSize);
                }
            }
            else // if there is no signing data in wnm file
            {
                qDebug() << "SignatureVerifier::verify() Not signed file fileName=" << m_localResource;
                verifyResult = 6;
            }
        }

        releaseWnm();

        QString fileUuid = QUrl::fromLocalFile(filePath).toString();
        if(verifyResult > 0)
            m_verifyResultList.insert(fileUuid, verifyResult);

        SignatureVerifyEventPtr finished(new SignatureVerifyEvent);
        auto verifyFinishEvent = std::make_shared<Wisenet::Core::UpdateVerifyResultEvent>();
        verifyFinishEvent->fileUuid = fileUuid.toStdString();
        verifyFinishEvent->verifyResult = verifyResult;
        finished->eventDataPtr = verifyFinishEvent;

        emit verifyFinished(finished);
    });
}

void SignatureVerifier::removeVerifyResult(const QString &fileUuid)
{
    std::lock_guard<std::mutex> lock_guard(m_mutex);

    if(m_verifyResultList.contains(fileUuid))
        m_verifyResultList.remove(fileUuid);

    return;
}

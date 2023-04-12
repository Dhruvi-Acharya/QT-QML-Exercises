#ifndef SIGNATUREVERIFIER_H
#define SIGNATUREVERIFIER_H

#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/pkcs12.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include "WnmStruct.h"
#include "QCoreServiceReply.h"

#include <QObject>
#include <QString>
#include <QFile>
#include <QDataStream>
#include <QMap>

#include <boost/asio/strand.hpp>
#include <boost/asio/io_context.hpp>

class ThreadPool;


class SignatureVerifyEvent : public QObject
{
    Q_OBJECT
public:
    explicit SignatureVerifyEvent(QObject *parent = nullptr) : QObject(parent){}
    ~SignatureVerifyEvent(){}

    Wisenet::EventBaseSharedPtr eventDataPtr;
};
typedef QSharedPointer<SignatureVerifyEvent> SignatureVerifyEventPtr;


class SignatureVerifier : public QObject, public std::enable_shared_from_this<SignatureVerifier> // 객체가 자신의 shared_ptr 객체를 알 수 있게 해줌.
{
    Q_OBJECT
public:
    explicit SignatureVerifier(QObject *parent = nullptr); // explicit, 묵시적 형변환 X, 명시적인 형변환
    virtual ~SignatureVerifier();

    void Stop();
    void verify(QString filePath);
    void removeVerifyResult(const QString &fileUuid);

    QMap<QString, int> getVerifyResultMap() {
        std::lock_guard<std::mutex> lock_guard(m_mutex);
        return m_verifyResultList;
    }

signals:
    int verifyFinished(SignatureVerifyEventPtr);

private:
    bool openWnm();
    void releaseWnm();

    void loadDigitalSigning();
    bool getPublicKey(const unsigned char* derData, long derDataLength);
    int verifySignature(QDataStream* dataStream, unsigned char* signingData, int signingDataSize);

    int                     m_signingDataSize = 0;
    char *                  m_signingDataPtr = nullptr;     // 디지털 서명 데이터
    int                     m_certificateDataSize = 0;
    char *                  m_certificateDataPtr = nullptr; // 디지털 인증서 데이터

    // used for verifying signature
    X509* m_cert;
    EVP_PKEY* m_pubKey;

    QString m_localResource;
    QFile m_file;
    QDataStream m_dataStream;
    QMap<WnmElementType, WnmOffsetEntry> m_fileOffsetIndex;
    qint64 m_signingFilePos = 0;

    QMap<QString, int> m_verifyResultList;

    std::shared_ptr<ThreadPool> m_contextPool;
    boost::asio::strand<boost::asio::io_context::executor_type> m_strand;
    std::atomic<bool> m_isRunning {false};

    std::mutex m_mutex;

};

#endif // SIGNATUREVERIFIER_H

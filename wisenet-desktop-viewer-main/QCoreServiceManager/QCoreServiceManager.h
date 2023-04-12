/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd. All rights reserved.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Vision.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Vision. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Vision.
 *
 * Hanwha Vision reserves the right to modify this software without notice.
 *
 * Hanwha Vision Co., Ltd.
 * KOREA
 * https://www.hanwhavision.com/
 *********************************************************************************/
#pragma once
#include <QObject>
#include <map>
#include <memory>
#include <vector>
#include "CoreService.h"
#include "QCoreServiceReply.h"
#include "QCoreServiceDatabase.h"
#include "QLocalSettings.h"
#include "QUpdateManager.h"
#include "SignatureVerifier.h"

typedef std::pair<Wisenet::uuid_string, Wisenet::Device::Device::Channel> deviceChannelPair;

class QCoreServiceManager final : public QObject
{
    Q_OBJECT
public:
    static QCoreServiceManager& Instance();
    static void Release();

    void Start();
    void Stop();

    void Login(const QString& id, const QString& password, bool hashPassword,
               QObject* sender,
               QCoreServiceReplyHandler const& replyHandler);
    void Logout();

    void RequestToCoreService(
            Wisenet::Core::memFuncType func,
            QObject* sender,
            Wisenet::RequestBaseSharedPtr const& request,
            QCoreServiceReplyHandler const& replyHandler);

    void SetS1DaemonPort(unsigned short port);

    static std::string CreateNewUUidString();
    static QString CreateNewUUidQString();

    QPointer<QCoreServiceDatabase> DB();
    QPointer<QLocalSettings> Settings();
    QPointer<SignatureVerifier> Signature();

    QVariant getUpdateStatus(int nOsType, QMap<QString, QString> &mapUpdateFile);
    void getUpdateFwInfoFile();
    QPair<QString,QString> getDeviceRecentVersion(QString deviceModelName);
    Wisenet::Core::FirmwareUpdateStatus checkLatestVersion(const QString& modelName, const QString& firmwareVersion) const;
    QPointer<QUpdateManager> updateManagerInstance() { return m_updateManager; }

    // Statistics
    void updateStatistics();
    QByteArray generateStatistics(QString& key);
    QString getConnectedType(Wisenet::Device::ConnectedType connectedType);

    /* NOTE: do not use from external class */
    explicit QCoreServiceManager(QObject* parent = nullptr);
    ~QCoreServiceManager();
private:
    static QCoreServiceRequestorPtr makeServiceRequestor(QObject* sender, QCoreServiceReplyHandler const& replyHandler);
    static void transferServiceResponse(const QCoreServiceRequestorPtr& requestor,
                                        Wisenet::ResponseBaseSharedPtr response);

    void coreServiceEventHandler(const Wisenet::EventBaseSharedPtr& event);
    bool isNotification(QCoreServiceEventPtr event);
    void signatureVerifyFinished(SignatureVerifyEventPtr);

private:
    Q_DISABLE_COPY_MOVE(QCoreServiceManager)

private:
    QPointer<QCoreServiceDatabase> m_db;
    QPointer<QLocalSettings> m_settings;
    QPointer<QUpdateManager> m_updateManager;
    QPointer<SignatureVerifier> m_signatureVerifier;

    Wisenet::Core::ICoreServiceSharedPtr m_localService;
    std::string m_userName;
    std::string m_password;
    static std::shared_ptr<QCoreServiceManager> m_qsm_instance;
    static std::atomic<bool> m_qsm_init;

    int m_startedUtc = 0;
    
signals:
    void coreServiceEventTriggered(QCoreServiceEventPtr);
};


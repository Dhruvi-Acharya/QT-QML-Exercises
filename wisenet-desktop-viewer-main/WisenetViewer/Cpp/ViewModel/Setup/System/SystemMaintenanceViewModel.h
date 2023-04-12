/*********************************************************************************
 * Copyright(c) 2023 Hanwha Vision Co., Ltd.  All rights reserved.
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
#include <QtNetwork>
#include <QUrl>
#ifdef Q_OS_LINUX
#include <sys/socket.h>
#endif
#ifdef Q_OS_WINDOWS
#include <WinSock2.h>
#endif
#ifdef Q_OS_WINDOWS
#include "libssh2.h"
#include "libssh2_config.h"
#include "libssh2_sftp.h"
#endif
#include <future>
#include "QCoreServiceReply.h"

class QNetworkReply;

class SystemMaintenanceViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)
    Q_PROPERTY(QVariant updateprogress READ getUpdateprogress WRITE setUpdateprogress NOTIFY updateprogressChanged)
    Q_PROPERTY(QVariant needUpdate READ getNeedUpdate WRITE setNeedUpdate NOTIFY needUpdateChanged)
    Q_PROPERTY(QVariant updateresult READ getUpdateresult WRITE setUpdateresult NOTIFY updateresultChanged)
    Q_PROPERTY(QString latestversion READ getLatestversion WRITE setLatestversion NOTIFY latestversionChanged)
    Q_PROPERTY(QString buildDate READ buildDate CONSTANT)

    Q_PROPERTY(QString licenseKey READ licenseKey WRITE setLicenseKey NOTIFY licenseKeyChanged)
    Q_PROPERTY(bool licenseActivated READ licenseActivated NOTIFY licenseActivatedChanged)
    Q_PROPERTY(bool isValidLicense READ isValidLicense NOTIFY isValidLicenseChanged)
public:
    Q_INVOKABLE void startBackup(QString path);
    Q_INVOKABLE void startRestore(QString path);
    Q_INVOKABLE void startReset();
    Q_INVOKABLE void restartApplication();
    Q_INVOKABLE void openSourceLicense();
    Q_INVOKABLE void openLogDir();
    Q_INVOKABLE void startUpdate();
    Q_INVOKABLE void checkUpdate();
    Q_INVOKABLE void applyLicense();

    SystemMaintenanceViewModel(QObject* parent= nullptr);

    static SystemMaintenanceViewModel* getInstance(){
        static SystemMaintenanceViewModel instance;
        return &instance;
    }

    QString version() { return m_version;}
    void setVersion(QString &version);
    QVariant getUpdateprogress() { return m_updateprogress;}
    void setUpdateprogress(QVariant &updateprogress);
    QVariant getNeedUpdate() { return m_needUpdate;}
    void setNeedUpdate(QVariant &needUpdate);
    QVariant getUpdateresult() { return m_updateResult;}
    void setUpdateresult(QVariant &updateresult);
    QString getLatestversion() { return m_latestversion;}
    void setLatestversion(QString &latestversion);
    QString licenseKey() { return m_licenseKey;}
    void setLicenseKey(QString &licenseKey);
    bool licenseActivated() { return m_licenseActivated;}
    bool isValidLicense();
#ifdef Q_OS_WINDOWS
    int waitSocket(int socket_fd, LIBSSH2_SESSION *session);
#endif
    void executeFile(const QString filePath);
    int downloadFileForS1(const std::string serverPath, const std::string localPath);

    QString buildDate() const;

private:
    void Event_ActivateLicense(QCoreServiceEventPtr event);

signals:
    void versionChanged(QString version);
    void showMessageBox(QString tile, QString text, bool bQuit, QString fileName);
    void updateprogressChanged(QVariant updateprogress);
    void needUpdateChanged(QVariant needUpdate);
    void updateresultChanged(QVariant updateresult);
    void latestversionChanged(QString latestversion);
    void exitProgram(bool restart = false);
    void licenseKeyChanged();
    void licenseActivatedChanged();
    void isValidLicenseChanged();

public slots:
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

private slots:
    void httpFinished();
    void httpReadyRead();
    void networkReplyProgress(qint64 bytesRead, qint64 totalBytes);

private:
    QString m_version;
    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply;
    std::unique_ptr<QFile> openFileForWrite(const QString &fileName);
    std::unique_ptr<QFile> file;
    QVariant m_updateprogress;
    QString m_wnvUpdatefilename;
    QVariant m_needUpdate;
    QMap<QString, QString> m_mapUpdateFile;
    QVariant m_updateResult;
    QString m_latestversion;

    QString m_licenseKey;
    bool m_licenseActivated = false;
};


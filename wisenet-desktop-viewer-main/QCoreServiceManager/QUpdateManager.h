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
#include "QCoreServiceReply.h"
#include <QScopedPointer>
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

enum FILE_OS_TYPE
{
    FILE_OS_TYPE_WINDOWS = 1,
    FILE_OS_TYPE_MAC,
    FILE_OS_TYPE_LINUX,
};

struct UpdateFwInfo{
    QString type;
    QString ext;
    QString modelGroup;
    QString fileName;
    QString version;
};

class QUpdateManager : public QObject
{
    Q_OBJECT
public:
    explicit QUpdateManager(QObject *parent = nullptr);
    void Start();
    QVariant getUpdateStatus(FILE_OS_TYPE osType, QMap<QString, QString> &rmapUpdateFile);
    void getUpdateFwInfoFile();
    void readUpdateFwInfo();
    QPair<QString,QString> getDeviceRecentVersion(QString deviceModelName);
    Wisenet::Core::FirmwareUpdateStatus checkLatestVersion(const QString& modelName, const QString& firmwareVersion) const;
    #ifdef Q_OS_WINDOWS
    int waitSocket(int socket_fd, LIBSSH2_SESSION *session);
#endif
    void downloadFileForS1(const std::string serverPath, const std::string localPath);

public slots:
    void onUpdateCheckTimerTimeout();
    void infoFileDownloadFinishedStartAnalyze();
    void httpReadyRead();

signals:
    void coreServiceEventTriggered(QCoreServiceEventPtr);

private:
    QScopedPointer<QTimer> m_updateCheckTimer;
    void checkUpdate();
    bool checkSoftwareVersion(QString &strVersion);
    std::unique_ptr<QFile> openFileForReadWrite(const QString &fileName);

    QUrl url;
    QNetworkAccessManager qnam;
    QNetworkReply *reply = nullptr;
    std::unique_ptr<QFile> m_file;
    std::unique_ptr<QFile> m_deviceUpdateInfofile;
    bool bFirstTimer;
    QMap<QString, QString> m_mapWinUpdateFile; // <1.1.0_20220126, WisenetViewer_v1.1.0_20220126.exe>
    QMap<QString, QString> m_mapMacUpdateFile;
    QMap<QString, QString> m_mapLinuxUpdateFile;
    bool m_bNeedUpdate;
    const qint64 m_cMaxUpdateTime = 60 * 60 * 24 * 30;

    std::map<QString, UpdateFwInfo> m_mapFwInfo; // <key : model name, value : latest fw info>
};

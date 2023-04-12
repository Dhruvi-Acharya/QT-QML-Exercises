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
#include "SystemMaintenanceViewModel.h"
#include <future>
#include <QDesktopServices>
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include <QDesktopServices>
#include <QTextStream>
#include "QLocaleManager.h"
#include "AppLocalDataLocation.h"

SystemMaintenanceViewModel::SystemMaintenanceViewModel(QObject* parent)
{
    Q_UNUSED(parent);
    m_updateprogress = 0;
    m_wnvUpdatefilename = "";
    m_needUpdate = false;
    reply = nullptr;

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &SystemMaintenanceViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void SystemMaintenanceViewModel::setVersion(QString &version)
{
    qDebug() << "SystemMaintenanceViewModel::setVersion" << version;
    if(version != m_version){
        m_version = version;
        emit versionChanged(m_version);
    }
}

void SystemMaintenanceViewModel::startBackup(QString folderPath)
{
    SPDLOG_INFO("SystemMaintenanceViewModel::startBackup() : {}" , folderPath.toUtf8().toStdString());

    auto request = std::make_shared<Wisenet::Core::BackupServiceRequest>();
    std::string path = folderPath.toUtf8().toStdString();
    request->backupFilePath = path;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::BackupService,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::BackupServiceResponse>(reply->responseDataPtr);
        SPDLOG_INFO("SystemMaintenanceViewModel::startBackup() response:{}", response->errorString());
        emit showMessageBox(QCoreApplication::translate("WisenetLinguist","Backup"),
                            QCoreApplication::translate("WisenetLinguist",response->errorString().c_str()), false, QString::fromStdString(response->fileName));

    });
}

void SystemMaintenanceViewModel::startRestore(QString path)
{
    SPDLOG_INFO("SystemMaintenanceViewModel::startRestore() :{} ",path.toUtf8().toStdString());

    auto request = std::make_shared<Wisenet::Core::RestoreServiceRequest>();
    request->restoreFilePath = path.toUtf8().toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RestoreService,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("SystemMaintenanceViewModel::startRestore() response:{}",response->errorString());
        QCoreServiceManager::Instance().Settings()->reset();
        emit showMessageBox(QCoreApplication::translate("WisenetLinguist","Restore"),
                            QCoreApplication::translate("WisenetLinguist",response->errorString().c_str()),
                            response->isSuccess(), "");
    });
}

void SystemMaintenanceViewModel::startReset()
{
    SPDLOG_INFO("SystemMaintenanceViewModel::startReset()");

    auto request = std::make_shared<Wisenet::Core::ResetServiceRequest>();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::ResetService,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("SystemMaintenanceViewModel::startReset() response: {} ",response->errorString());
        QCoreServiceManager::Instance().Settings()->reset();
        emit showMessageBox(QCoreApplication::translate("WisenetLinguist","reset"),
                            QCoreApplication::translate("WisenetLinguist",response->errorString().c_str()),
                            response->isSuccess(),"");
    });
}

void SystemMaintenanceViewModel::restartApplication(){
    SPDLOG_INFO("SystemMaintenanceViewModel::restartApplication()");
    exitProgram(true);
}

void SystemMaintenanceViewModel::openSourceLicense(){
    QString fileName = QCoreApplication::applicationDirPath() + "/OpenSourceLicense.txt";

    QFile file(fileName);
    SPDLOG_DEBUG("SystemMaintenanceViewModel::openSourceLicense(){}", fileName.toLocal8Bit().toStdString());
    if(QFileInfo::exists(fileName)){
        SPDLOG_DEBUG("SystemMaintenanceViewModel::openSourceLicense(){} exist", fileName.toLocal8Bit().toStdString());
        QDesktopServices::openUrl(QUrl::fromLocalFile(fileName));
    }
}

void SystemMaintenanceViewModel::openLogDir()
{
    SPDLOG_DEBUG("SystemMaintenanceViewModel::openLogDir {} ", GetAppDataLocation().toLocal8Bit().toStdString());
    QString path = GetAppDataLocation();
    QDesktopServices::openUrl(QUrl::fromLocalFile(path));
}

void SystemMaintenanceViewModel::checkUpdate()
{
    qDebug() << "SystemMaintenanceViewModel::checkUpdate()";
    QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();
    QMap<QString, QString> mapUpdateFile;
    m_mapUpdateFile.clear();
    if(osType == QOperatingSystemVersion::MacOS)
    {
        m_needUpdate = QCoreServiceManager::Instance().getUpdateStatus(2, mapUpdateFile);
    }
    else if(osType == QOperatingSystemVersion::Windows)
    {
        m_needUpdate = QCoreServiceManager::Instance().getUpdateStatus(1, mapUpdateFile);
    }
    else
    {
        qDebug() << "SystemMaintenanceViewModel Unsupport OS ";
        return;
    }

    m_mapUpdateFile = mapUpdateFile;
    setNeedUpdate(m_needUpdate);

    if(m_mapUpdateFile.size() > 0)
    {
        QString strVersion = m_mapUpdateFile.lastKey();
        auto tokens = strVersion.split("_");
        QVector<QString> numberTokens;
        for(auto& item : tokens){
            numberTokens.push_back(item);
        }

        QString yearmonthdate = numberTokens[1];
        QDate date = QDate::fromString(yearmonthdate,"yyyyMMdd");
        QDateTime datetime;
        datetime.setDate(date);
        QString strDate = QLocaleManager::Instance()->getYearMonthDay(datetime);
        QString latestVersion = numberTokens[0] + ", " + strDate;
        qDebug() << "SystemMaintenanceViewModel::checkUpdate()latestVersion: " << latestVersion;
        setLatestversion(latestVersion);
    }
    else
    {
        QString latestVersion = "-";
        setLatestversion(latestVersion);
    }

    // License
    if(!QCoreServiceManager::Instance().DB().isNull())
    {
        auto licenseInfo = QCoreServiceManager::Instance().DB()->GetLicenseInfo();

        qDebug() << "SystemMaintenanceViewModel::checkUpdate license " << licenseInfo.activated;
        m_licenseActivated = licenseInfo.activated;
        emit licenseActivatedChanged();
    }

}

QString SystemMaintenanceViewModel::buildDate() const
{
    QString strDate = QLocaleManager::Instance()->getYearMonthDay(QLocale("en_US").toDateTime(QString(__DATE__).simplified(), "MMM d yyyy"));
    qDebug() << "SystemMaintenanceViewModel::buildDate" << strDate;

    SPDLOG_DEBUG("SystemMaintenanceViewModel::buildDate={} __DATE__={}", strDate.toStdString(), __DATE__);

    return strDate;
}

void SystemMaintenanceViewModel::Event_ActivateLicense(QCoreServiceEventPtr event)
{
    qDebug() << "SystemMaintenanceViewModel::Event_ActivateLicense()";
    auto activateLicenseEvent = std::static_pointer_cast<Wisenet::Core::ActivateLicenseEvent>(event->eventDataPtr);
    m_licenseActivated = activateLicenseEvent->licenseInfo.activated;
    emit licenseActivatedChanged();
}

void SystemMaintenanceViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::ActivateLicenseEventType:
        Event_ActivateLicense(event);
        break;
    default:
        break;
    }
}

std::unique_ptr<QFile> SystemMaintenanceViewModel::openFileForWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::WriteOnly)) {
        qDebug() << "SystemMaintenanceViewModel::openFileForWrite() failed";
        return nullptr;
    }
    return file;
}


int SystemMaintenanceViewModel::downloadFileForS1(const std::string serverPath, const std::string localPath)
{
#ifdef Q_OS_WINDOWS
    int sock, i, auth_pw = 1;
    struct sockaddr_in sin;
    const char *fingerPrint;
    LIBSSH2_SESSION *session;
    const int sftpPort = 30022;
    const char *sftpIp = "27.102.104.143"; // upload-c3.xtrmcdn.co.kr
    const char *username = "test_htw"; //QCoreServiceManager::Instance().Settings()->s1UpdateServerUsername().toStdString().c_str();
    const char *password = "!THadmin0"; //QCoreServiceManager::Instance().Settings()->s1UpdateServerPassword().toStdString().c_str();
    const char *sftppath = serverPath.c_str(); //"/PcViewer/htw/x64/WisenetViewer_1.02.01_20221018.exe"; //pcviewer-otp-profile-htw2.txt";
    const char *storage = localPath.c_str(); //(QCoreApplication::applicationDirPath().toStdString()+ "/test.exe").c_str(); /* this is the local file name this example uses to store the downloaded file in */
    const int readSize = 1000;
    int rc;
    LIBSSH2_SFTP *sftp_session;
    LIBSSH2_SFTP_HANDLE *sftp_handle;
    LIBSSH2_SFTP_ATTRIBUTES sftpAttrs;
    libssh2_uint64_t fileTotalSize = 0;
    libssh2_uint64_t fileDownloadedSize = 0;
    FILE *tempStorage;
    struct timeval timeOut;
    fd_set fd;
    fd_set fd2;

    libssh2_session_init();

    //    std::string tmp(sftppath);
    //    qDebug() << "sftppath:" << QString::fromStdString(tmp);
    //    tmp = storage;
    //    qDebug() << "storage:" << QString::fromStdString(tmp);


    rc = libssh2_init(0);
    if(rc != 0)
    {
        qDebug() << "libssh2 initialization failed:" << rc;
        return -1;
    }

    /* Ultra basic "connect to port 22 on localhost"
 * The application is responsible for creating the socket establishing
 * the connection
 */
    sock = socket(AF_INET, SOCK_STREAM, 0);

    sin.sin_family = AF_INET;
    sin.sin_port = htons(sftpPort);
    sin.sin_addr.s_addr = inet_addr(sftpIp);
    if(::connect(sock, (struct sockaddr*)(&sin), sizeof(struct sockaddr_in)) != 0)
    {
        qDebug() << "failed to connect!";
        return -1;
    }

    /* Create a session instance
 */
    session = libssh2_session_init();
    if(!session)
    {
        return -1;
    }

    /* ... start it up. This will trade welcome banners, exchange keys,
 * and setup crypto, compression, and MAC layers
 */
    rc = libssh2_session_handshake(session, sock);
    if(rc)
    {
        qDebug() << "Failure establishing SSH session:" << rc;
        return -1;
    }

    libssh2_session_set_blocking(session, 0);

    /* At this point we havn't yet authenticated.  The first thing to do
 * is check the hostkey's fingerPrint against our known hosts Your app
 * may have it hard coded, may go to a file, may present it to the
 * user, that's your call
 */
    fingerPrint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

    tempStorage = fopen(storage, "wb");
    if(tempStorage == false)
    {
        qDebug() << "Can't open temp storage file " << storage;
        return -1;
    }

    if(auth_pw)
    {
        /* We could authenticate via password */
        while((rc = libssh2_userauth_password(session, username, password)) == LIBSSH2_ERROR_EAGAIN);
        if(rc)
        {
            qDebug() << "Authentication by password failed.";
            return -1;
        }
    }

    do {
        sftp_session = libssh2_sftp_init(session);

        if(!sftp_session)
        {
            if(libssh2_session_last_errno(session) == LIBSSH2_ERROR_EAGAIN)
            {
                qDebug() << "non-blocking init";
                waitSocket(sock, session); /* now we wait */
            }
            else
            {
                qDebug() << "Unable to init SFTP session";
                return -1;
            }
        }
    } while(!sftp_session);

    /* Request a file via SFTP */
    do {
        sftp_handle = libssh2_sftp_open(sftp_session, sftppath, LIBSSH2_FXF_READ, 0); //libssh2_sftp_open_ex(sftp_session, sftppath, strlen(sftppath), LIBSSH2_FXF_READ, 0, LIBSSH2_SFTP_OPENFILE);

        if(!sftp_handle)
        {
            if(libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN)
            {
                qDebug() << "Unable to open file with SFTP";
                return -1;
            }
            else
            {
                qDebug() << "non-blocking open";
                waitSocket(sock, session); /* now we wait */
            }
        }
    } while(!sftp_handle);

    // 다운로드 할 인스톨 파일 크기 확인 (s1서버가 올바른 파일 정보를 주지 못하여 업데이트 진행률 표시 불가)
    //    libssh2_trace(session, ~0);
    //    do {
    //        rc = libssh2_sftp_fstat_ex(sftp_handle, &sftpAttrs, 0);
    //        if(rc != 0 && rc != LIBSSH2_ERROR_EAGAIN)
    //        {
    //            qDebug() << "libssh2_sftp_fstat_ex returned" << rc;
    //            return -1;
    //        }
    //    } while(!rc);
    //    fileTotalSize = (libssh2_uint64_t)sftpAttrs.filesize;// readSize;
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, sftpAttrs.filesize:" << (libssh2_uint64_t)sftpAttrs.filesize << "Bytes";
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, fileTotalSize:" << fileTotalSize << "Bytes";
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, permissions:" << (unsigned long)sftpAttrs.permissions;
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, uid:" << (unsigned long)sftpAttrs.uid;
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, gid:" << (unsigned long)sftpAttrs.gid;
    //    qDebug() << "libssh2_sftp_fstat_ex() is done, atime:" << (unsigned long)sftpAttrs.atime;


    //    LIBSSH2_SFTP_ATTRIBUTES sftpAttrs2;
    //    do {
    //        rc = libssh2_sftp_stat_ex(sftp_session, sftppath, strlen(sftppath), LIBSSH2_SFTP_LSTAT, &sftpAttrs2);
    //        if(rc != 0 && rc != LIBSSH2_ERROR_EAGAIN)
    //        {
    //            qDebug() << "libssh2_sftp_stat_ex returned" << rc;
    //            return -1;
    //        }
    //    } while(!rc);
    //    fileTotalSize = (libssh2_uint64_t)sftpAttrs2.filesize;// readSize;
    //    qDebug() << "libssh2_sftp_stat_ex() is done, sftpAttrs2.filesize:" << (libssh2_uint64_t)sftpAttrs2.filesize << "Bytes";
    //    qDebug() << "libssh2_sftp_stat_ex() is done, fileTotalSize:" << fileTotalSize << "Bytes";
    //    qDebug() << "libssh2_sftp_stat_ex() is done, permissions:" << (unsigned long)sftpAttrs2.permissions;
    //    qDebug() << "libssh2_sftp_stat_ex() is done, uid:" << (unsigned long)sftpAttrs2.uid;
    //    qDebug() << "libssh2_sftp_stat_ex() is done, gid:" << (unsigned long)sftpAttrs2.gid;
    //    qDebug() << "libssh2_sftp_stat_ex() is done, atime:" << (unsigned long)sftpAttrs2.atime;

    //    qDebug() << "libssh2_sftp_open() is done, now receive data!";

    //    return -1; // test
    do {
        do {
            char mem[readSize];
            /* read in a loop until we block */
            rc = libssh2_sftp_read(sftp_handle, mem, sizeof(mem));
            if(rc == LIBSSH2_ERROR_EAGAIN)
            {
                continue;
            }

            //            qDebug() << "libssh2_sftp_read returned" << rc;

            if(rc > 0)
            {
                /* write to stderr */
                write(2, mem, rc);
                /* write to temporary storage area */
                fwrite(mem, rc, 1, tempStorage);

                // 다운로드 할 인스톨 파일 크기 확인 (s1서버가 올바른 파일 정보를 주지 못하여 업데이트 진행률 표시 불가)
                //                if(fileTotalSize > 0)
                //                {
                //                    fileDownloadedSize += sizeof(mem);
                //                    quint64 updateProgress = (quint64)(fileDownloadedSize)/(quint64)fileTotalSize >= 100 ? 100 : (quint64)fileDownloadedSize*100/(quint64)fileTotalSize;
                //                    QVariant vupdateProgress(updateProgress);
                //                    qDebug() << "updateProgress:" << updateProgress << "%";
                //                    qDebug() << "fileDownloadedSize:" << fileDownloadedSize << "fileTotalSize:" << fileTotalSize << "Bytes";
                //                    setUpdateprogress(vupdateProgress);
                //                }
            }
        } while(rc > 0);

        if(rc != LIBSSH2_ERROR_EAGAIN)
        {
            /* error or end of file */
            break;
        }

        timeOut.tv_sec = 10;
        timeOut.tv_usec = 0;

        FD_ZERO(&fd);
        FD_ZERO(&fd2);
        FD_SET(sock, &fd);
        FD_SET(sock, &fd2);

        /* wait for readable or writeable */
        rc = select(sock + 1, &fd, &fd2, NULL, &timeOut);
        if(rc <= 0)
        {
            /* negative is error
                   0 is timeOut */
            qDebug() << "SFTP download timed out:" << rc;
            break;
        }

    } while(1);

    //    qDebug() << "libssh2_sftp_close(sftp_handle)";
    libssh2_sftp_close(sftp_handle);
    fclose(tempStorage);

    //    qDebug() << "fopen(storage)";
    tempStorage = fopen(storage, "rb");
    if(!tempStorage)
    {
        /* weird, we can't read the file we just wrote to... */
        qDebug() << "can't open %s for reading" << storage;
        return -1;
    }

    //    qDebug() << "libssh2_sftp_shutdown(sftp_session)";
    libssh2_sftp_shutdown(sftp_session);

    //    qDebug() << "libssh2_session_disconnect()";
    libssh2_session_disconnect(session, "Normal Shutdown");
    //    qDebug() << "libssh2_session_free(session)";
    libssh2_session_free(session);

    //    qDebug() << "closesocket";
#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    if(tempStorage)
    {
        //        qDebug() << "fclose(tempStorage)";
        fclose(tempStorage);
    }

    //    qDebug() << "download done";

    libssh2_exit();
#endif

    return 0; // success !
}

void SystemMaintenanceViewModel::startUpdate()
{
#ifdef WISENET_S1_VERSION
#ifdef Q_OS_WINDOWS
    // wnv update
    std::thread updateUsingSftpThread([this]() {
        std::string serverFileName = m_mapUpdateFile.last().toStdString();

        std::string serverFilePath = /*"/PcViewer/htw/x64/"*/ QCoreServiceManager::Instance().Settings()->s1UpdateServerURL().toStdString() + serverFileName; //WisenetViewer_1.02.01_20221018.exe";
        std::string localFilePath = QCoreApplication::applicationDirPath().toStdString() + "/" + serverFileName;

        QVariant vupdateProgress((quint64)1);
        setUpdateprogress(vupdateProgress);
        int ret = downloadFileForS1(serverFilePath, localFilePath);
        if(ret < 0)
        {
            QVariant vupdateProgress((quint64)0);
            setUpdateprogress(vupdateProgress);
            return;
        }

        // 현재 뷰어 종료
        auto request = std::make_shared<Wisenet::Core::UpdateSoftwareServiceRequest>();

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::UpdateSoftwareService,
                    this, request, nullptr);

        fprintf(stderr, "exit program\n");
        exitProgram();

        // 신규 버전 뷰어 설치파일 실행
        executeFile(QString::fromStdString(localFilePath));
    });

    updateUsingSftpThread.detach();
#endif
#else
    if(reply)
    {
        if(reply->isRunning())
        {
            qDebug() << "SystemMaintenanceViewModel::startUpdate isRunning status";
            return;
        }
    }
    SPDLOG_DEBUG("SystemMaintenanceViewModel::startUpdate()");

    QString strServerURL = QCoreServiceManager::Instance().Settings()->updateSeverURL();
    qDebug() << "SystemMaintenanceViewModel::strServerURL:" << strServerURL;

    QString strVersion = m_mapUpdateFile.lastKey();
    qDebug() << "SystemMaintenanceViewModel::strVersion:" << strVersion;
    QString strFilename = m_mapUpdateFile.last();
    qDebug() << "SystemMaintenanceViewModel::strFilename:" << strFilename;

    //QUrl strUrl("http://55.101.67.139/WisenetViewer_v1.0.0_20220126.exe");
    QUrl strUrl = strServerURL + strFilename;
    qDebug() << "SystemMaintenanceViewModel::strUrl:" << strUrl;
    QString fileName = QCoreApplication::applicationDirPath() + "/" + strFilename;
    file = openFileForWrite(fileName);
    qDebug() << "SystemMaintenanceViewModel::fileName():" << fileName;
    if (!file)
        return;

    m_wnvUpdatefilename = fileName;

    reply = qnam.get(QNetworkRequest(strUrl));
    connect(reply, &QNetworkReply::finished, this, &SystemMaintenanceViewModel::httpFinished);
    connect(reply, &QIODevice::readyRead, this, &SystemMaintenanceViewModel::httpReadyRead);
    connect(reply, &QNetworkReply::downloadProgress, this, &SystemMaintenanceViewModel::networkReplyProgress);
#endif
}

void SystemMaintenanceViewModel::httpFinished()
{
    qDebug() << "SystemMaintenanceViewModel::httpFinished()";
    QFileInfo fi;
    if (file) {
        fi.setFile(file->fileName());
        file->close();
        file.reset();
    }

    if (reply->error()) {
        qDebug() << "SystemMaintenanceViewModel::reply->error()";
        QFile::remove(fi.absoluteFilePath());
        reply->deleteLater();
        reply = nullptr;
        QVariant zero(0);
        setUpdateprogress(zero);
        QVariant result = false;
        setUpdateresult(result);
        return;
    }
    reply->deleteLater();
    reply = nullptr;

    if(getUpdateprogress() == 100)
    {
        auto request = std::make_shared<Wisenet::Core::UpdateSoftwareServiceRequest>();

        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::UpdateSoftwareService,
                    this, request, nullptr);

        exitProgram();
        QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();
        if(osType == QOperatingSystemVersion::MacOS)
        {
            QProcess::execute("open", { m_wnvUpdatefilename });
        }
        else if(osType == QOperatingSystemVersion::Windows)
        {
            QStringList argvs;
            QProcess::startDetached(m_wnvUpdatefilename, argvs);
        }
        else
        {
            qDebug() << "SystemMaintenanceViewModel Unsupport OS ";
        }
    }
    else
    {
        QVariant zero(0);
        setUpdateprogress(zero);
        QVariant result = false;
        setUpdateresult(result);
    }
}

void SystemMaintenanceViewModel::httpReadyRead()
{
    if (file)
    {
        file->write(reply->readAll());
    }
}

void SystemMaintenanceViewModel::networkReplyProgress(qint64 bytesRead, qint64 totalBytes)
{
    if(totalBytes)
    {
        qint64 updateprogress = (bytesRead*100)/totalBytes;
        QVariant vupdateprogress(updateprogress);
        setUpdateprogress(vupdateprogress);
    }
}

void SystemMaintenanceViewModel::applyLicense()
{
    auto request = std::make_shared<Wisenet::Core::ActivateLicenseRequest>();
    request->licenseKey = m_licenseKey.toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::ActivateLicense,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("SystemMaintenanceViewModel::applyLicense() response:{}",response->errorString());
    });
}

void SystemMaintenanceViewModel::setUpdateprogress(QVariant &updateprogress)
{
    if(updateprogress != m_updateprogress){
        m_updateprogress = updateprogress;
        emit updateprogressChanged(m_updateprogress);
    }
}

void SystemMaintenanceViewModel::setNeedUpdate(QVariant &needUpdate)
{
    m_needUpdate = needUpdate;
    emit needUpdateChanged(m_needUpdate);
    qDebug() << "SystemMaintenanceViewModel::needUpdateChanged()";
}

void SystemMaintenanceViewModel::setUpdateresult(QVariant &updateresult)
{
    m_updateResult = updateresult;
    emit updateresultChanged(m_updateResult);
}

void SystemMaintenanceViewModel::setLatestversion(QString &latestversion)
{
    m_latestversion = latestversion;
    emit latestversionChanged(m_latestversion);
}

void SystemMaintenanceViewModel::setLicenseKey(QString &licenseKey)
{
    qDebug() << "SystemMaintenanceViewModel::setLicenseKey" << licenseKey;
    m_licenseKey = licenseKey;
    emit isValidLicenseChanged();
}

bool SystemMaintenanceViewModel::isValidLicense()
{
    if(m_licenseKey.length() == 35 && !QCoreServiceManager::Instance().Settings().isNull())
    {
        bool validation = QCoreServiceManager::Instance().Settings()->checkLicenseValidation(m_licenseKey);
        qDebug() << "SystemMaintenanceViewModel::isValidLicense" << validation;
        return validation;
    }

    return false;
}

#ifdef Q_OS_WINDOWS
int SystemMaintenanceViewModel::waitSocket(int socket_fd, LIBSSH2_SESSION *session)
{
    struct timeval timeOut;
    int rc;
    fd_set fd;
    fd_set *writefd = NULL;
    fd_set *readfd = NULL;
    int dir;

    timeOut.tv_sec = 10;
    timeOut.tv_usec = 0;

    FD_ZERO(&fd);

    FD_SET(socket_fd, &fd);

    /* now make sure we wait in the correct direction */
    dir = libssh2_session_block_directions(session);

    if(dir & LIBSSH2_SESSION_BLOCK_INBOUND)
        readfd = &fd;

    if(dir & LIBSSH2_SESSION_BLOCK_OUTBOUND)
        writefd = &fd;

    rc = select(socket_fd + 1, readfd, writefd, NULL, &timeOut);

    return rc;
}
#endif

void SystemMaintenanceViewModel::executeFile(const QString filePath)
{
    // exit current program & execute update file
    QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();

    qDebug() << "filePath execute program:" << filePath;

    if(osType == QOperatingSystemVersion::MacOS)
    {
        QProcess::execute("open", { filePath });
    }
    else if(osType == QOperatingSystemVersion::Windows)
    {
        QStringList argvs;
        QProcess::startDetached(filePath, argvs);
    }
    else
    {
        qDebug() << "SystemMaintenanceViewModel Unsupport OS ";
    }
}

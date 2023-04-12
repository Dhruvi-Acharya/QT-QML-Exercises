#include "QCoreServiceManager.h"

QUpdateManager::QUpdateManager(QObject *parent) : QObject(parent)
{
    bFirstTimer = true;
    m_bNeedUpdate = false;
}

void QUpdateManager::onUpdateCheckTimerTimeout()
{
     qDebug() << "QUpdateManager::onUpdateCheckTimerTimeout";
     if(bFirstTimer)
     {
        m_updateCheckTimer->stop();
        m_updateCheckTimer.reset(new QTimer());
        if(m_updateCheckTimer.isNull()) {
            return;
        }
        if(QCoreServiceManager::Instance().Settings().isNull()) {
            return;
        }
        qint64 nCheckIntervalSec = QCoreServiceManager::Instance().Settings()->checkUpdateServerInterval();
        if(nCheckIntervalSec <= 0 || nCheckIntervalSec > m_cMaxUpdateTime)
        {
            m_updateCheckTimer->setInterval(86400*1000);
        }
        else
        {
            m_updateCheckTimer->setInterval(nCheckIntervalSec*1000);
        }
        connect(m_updateCheckTimer.data(), &QTimer::timeout, this, &QUpdateManager::onUpdateCheckTimerTimeout, Qt::QueuedConnection);
        m_updateCheckTimer->start();
        bFirstTimer = false;
     }
     checkUpdate();
}

void QUpdateManager::Start()
{
#ifdef Q_OS_LINUX
    return;
#endif

    if(bFirstTimer)
    {
        m_updateCheckTimer.reset(new QTimer());
        m_updateCheckTimer->setInterval(1000);
        connect(m_updateCheckTimer.data(), &QTimer::timeout, this, &QUpdateManager::onUpdateCheckTimerTimeout, Qt::QueuedConnection);
        m_updateCheckTimer->start();
    }
}

QVariant QUpdateManager::getUpdateStatus(FILE_OS_TYPE osType, QMap<QString, QString> &rmapUpdateFile)
{
    switch(osType)
    {
        case FILE_OS_TYPE_WINDOWS:
            rmapUpdateFile = m_mapWinUpdateFile;
        break;
        case FILE_OS_TYPE_MAC:
            rmapUpdateFile = m_mapMacUpdateFile;
        break;
        case FILE_OS_TYPE_LINUX:
            rmapUpdateFile = m_mapLinuxUpdateFile;
        break;
    }
    qDebug() << "QUpdateManager::getUpdateStatus():" << m_bNeedUpdate;
    return m_bNeedUpdate;
}


void QUpdateManager::getUpdateFwInfoFile()
{
    SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile()");

    // Download Update_FW_Info.txt file from upgrade server
    if(QCoreServiceManager::Instance().Settings().isNull())
        return;
    QString strServerURL = QCoreServiceManager::Instance().Settings()->deviceUpdateSeverURL();//"https://update.hanwha-security.com/FW/";
    QString infoFilename = "Update_FW_Info.txt";
    QUrl strUrl = strServerURL + infoFilename;
    QString fileName = QCoreApplication::applicationDirPath() + "/" + infoFilename;

    SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile() - strServerURL={}", strServerURL.toStdString());
//    qDebug() <<"QUpdateManager::getUpdateFwInfoFile() - strUrl =" <<strUrl;
    SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile() - fileName={}", fileName.toStdString());

    // 디바이스fw 파일, 정보 초기화
    QFile::remove(fileName);
    m_mapFwInfo.clear();

    m_deviceUpdateInfofile = openFileForReadWrite(fileName);
    if (!m_deviceUpdateInfofile) {
        SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile() - file open failed");
         return;
    }

    QNetworkAccessManager manager;
    QNetworkReply *response = manager.get(QNetworkRequest(strUrl));

    QEventLoop event;
    connect(response, SIGNAL(finished()), &event, SLOT(quit()));
    event.exec();

    if (response->error()) {
        QFileInfo fi;
        fi.setFile(m_deviceUpdateInfofile->fileName());
        QFile::remove(fi.absoluteFilePath());
        m_deviceUpdateInfofile->remove();
        m_mapFwInfo.clear();
        response->deleteLater();
        response = nullptr;
        return;
    }

    if (m_deviceUpdateInfofile)
    {
        SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile() - write");
        m_deviceUpdateInfofile->write(response->readAll());

        readUpdateFwInfo();
    }
    SPDLOG_DEBUG("QUpdateManager::getUpdateFwInfoFile() - m_deviceUpdateInfofile->size={}", m_deviceUpdateInfofile->size());
    return;
}

void QUpdateManager::readUpdateFwInfo()
{
    if(m_deviceUpdateInfofile == nullptr || m_deviceUpdateInfofile->size() == 0)
        return;

    QTextStream in(m_deviceUpdateInfofile.get());
    in.seek(0);
    m_mapFwInfo.clear();
    while (!in.atEnd())
    {
        QString line = in.readLine(); //read one line at a time
        QStringList lineContents = line.split(",");
        SPDLOG_DEBUG("QUpdateManager::getDeviceRecentVersion() line={}", line.toStdString());
        if(lineContents.size() > 6 ) {

            UpdateFwInfo fwInfo;
            fwInfo.type = lineContents[1];
            fwInfo.ext = lineContents[2];
            fwInfo.modelGroup = lineContents[3];
            fwInfo.fileName = lineContents[4];
            fwInfo.version = lineContents[5];

            m_mapFwInfo.emplace(lineContents[0], fwInfo);

            //SPDLOG_DEBUG("QUpdateManager::readUpdateFwInfo() modelName={} line={}", lineContents[0].toStdString(), fwInfo.version.toStdString());
        }
    }
}

QPair<QString,QString> QUpdateManager::getDeviceRecentVersion(QString deviceModelName)
{
    QString modelCodeName = "-";
    QString recentUpdateVersionName = "-";

    auto iter = m_mapFwInfo.find(deviceModelName);
    if(iter != m_mapFwInfo.end())
    {
        modelCodeName = iter->second.modelGroup;
        recentUpdateVersionName = iter->second.fileName;
    }

    QStringList recentUpdateVersionNameList = recentUpdateVersionName.split(".zip");
    recentUpdateVersionName = recentUpdateVersionNameList[0];

    SPDLOG_DEBUG("QUpdateManager::getDeviceRecentVersion() - deviceModelName={} {} {}",deviceModelName.toStdString(), modelCodeName.toStdString(), recentUpdateVersionName.toStdString());
    return {modelCodeName,recentUpdateVersionName};
}

Wisenet::Core::FirmwareUpdateStatus QUpdateManager::checkLatestVersion(const QString& modelName, const QString& firmwareVersion) const
{
    if(firmwareVersion == "UNKNOWN")
        return Wisenet::Core::FirmwareUpdateStatus::Unknown;

    auto iter = m_mapFwInfo.find(modelName);
    if(iter != m_mapFwInfo.end())
    {
        QString newVersionName = iter->second.version;
        QString currentVersionName = firmwareVersion;
        QStringList newVersionStringList = newVersionName.split("_");
        QStringList currentVersionStringList = currentVersionName.split("_");

        QString newMajor, newMinor;
        QString curMajor, curMinor;

        // 최신 버전의 숫자 이외 문자 제거
        QStringList newVersionCopy = newVersionStringList;
        for(int i=0; i<newVersionCopy.size(); ++i)
        {
            auto nv = newVersionCopy[i];
            for(int j=0; j<nv.size(); ++j)
            {
                if(nv[j] >= '0' && nv[j] <= '9')
                {
                    switch (i) {
                    case 0:
                        newMajor.push_back(nv[j]);
                        break;
                    case 1:
                        newMinor.push_back(nv[j]);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        // 현재 버전의 숫자 이외 문자 제거
        QStringList curVersionCopy = currentVersionStringList;
        for(int i=0; i<curVersionCopy.size(); ++i)
        {
            auto cv = curVersionCopy[i];
            for(int j=0; j<cv.size(); ++j)
            {
                if(cv[j] >= '0' && cv[j] <= '9')
                {
                    switch (i) {
                    case 0:
                        curMajor.push_back(cv[j]);
                        break;
                    case 1:
                        curMinor.push_back(cv[j]);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        qDebug() << "QUpdateManager::checkLatestVersion() newVersion:"<<newMajor<<"."<<newMinor;
        qDebug() << "QUpdateManager::checkLatestVersion() curVersion:"<<curMajor<<"."<<curMinor;

        if(newMajor.toInt() > curMajor.toInt())
        {
            return Wisenet::Core::FirmwareUpdateStatus::NeedToUpdate;
        }
        else if(newMajor.toInt() == curMajor.toInt())
        {
            if(newMinor.toInt() > curMinor.toInt())
            {
                return Wisenet::Core::FirmwareUpdateStatus::NeedToUpdate;
            }
            else
            {
                return Wisenet::Core::FirmwareUpdateStatus::Latest;
            }
        }
        else
        {
            return Wisenet::Core::FirmwareUpdateStatus::Latest;
        }
    }
    else
    {
        return Wisenet::Core::FirmwareUpdateStatus::Unknown;
    }
}

#ifdef Q_OS_WINDOWS
int QUpdateManager::waitSocket(int socket_fd, LIBSSH2_SESSION *session)
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

void QUpdateManager::downloadFileForS1(const std::string serverPath, const std::string localPath)
{
#ifdef Q_OS_WINDOWS
    int sock, auth_pw = 1;
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
//    LIBSSH2_SFTP_ATTRIBUTES sftpAttrs;
//    libssh2_uint64_t fileTotalSize = 0;
//    libssh2_uint64_t fileDownloadedSize = 0;
    FILE *tempStorage;
    struct timeval timeOut;
    fd_set fd;
    fd_set fd2;

//    std::string tmpStr = sftpIp;
//    qDebug() << "QUpdateManager::downloadFileForS1 sftpIp:" << QString::fromStdString(tmpStr);
//    tmpStr = username;
//    qDebug() << "QUpdateManager::downloadFileForS1 username:" << QString::fromStdString(tmpStr);
//    tmpStr = password;
//    qDebug() << "QUpdateManager::downloadFileForS1 password:" << QString::fromStdString(tmpStr);
//    tmpStr = sftppath;
//    qDebug() << "QUpdateManager::downloadFileForS1 sftppath:" << QString::fromStdString(tmpStr);
//    tmpStr = storage;
//    qDebug() << "QUpdateManager::downloadFileForS1 storage:" << QString::fromStdString(tmpStr);

    libssh2_session_init();

    rc = libssh2_init(0);
    if(rc != 0)
    {
        qDebug() << "libssh2 initialization failed" << rc;
        return;
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
        ::closesocket(sock);
        qDebug() << "failed to connect!";
        return;
    }

    /* Create a session instance
 */
    session = libssh2_session_init();
    if(!session)
    {
        ::closesocket(sock);
        return;
    }

    /* ... start it up. This will trade welcome banners, exchange keys,
 * and setup crypto, compression, and MAC layers
 */
    rc = libssh2_session_handshake(session, sock);
    if(rc)
    {
        qDebug() << "Failure establishing SSH session:" << rc;
        ::closesocket(sock);
        return;
    }

    libssh2_session_set_blocking(session, 0);

    /* At this point we havn't yet authenticated.  The first thing to do
 * is check the hostkey's fingerPrint against our known hosts Your app
 * may have it hard coded, may go to a file, may present it to the
 * user, that's your call
 */
    fingerPrint = libssh2_hostkey_hash(session, LIBSSH2_HOSTKEY_HASH_SHA1);

    tempStorage = fopen(storage, "wb");
    if (tempStorage == nullptr || tempStorage == NULL)
    {
        qDebug() << "Can't open temp storage file" << storage;
        ::closesocket(sock);
        return;
    }

    if(auth_pw)
    {
        /* We could authenticate via password */
        while((rc = libssh2_userauth_password(session, username, password)) == LIBSSH2_ERROR_EAGAIN);
        if(rc)
        {
            fclose(tempStorage);
            ::closesocket(sock);
            qDebug() << "Authentication by password failed.";
            return;
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
                fclose(tempStorage);
                ::closesocket(sock);
                qDebug() << "Unable to init SFTP session";
                return;
            }
        }
    } while(!sftp_session);

    /* Request a file via SFTP */
    do {
        sftp_handle = libssh2_sftp_open(sftp_session, sftppath, LIBSSH2_FXF_READ, 0);

        if(!sftp_handle)
        {
            if(libssh2_session_last_errno(session) != LIBSSH2_ERROR_EAGAIN)
            {
                fclose(tempStorage);
                ::closesocket(sock);
                qDebug() << "Unable to open file with SFTP";
                return;
            }
            else
            {
                qDebug() << "non-blocking open";
                waitSocket(sock, session); /* now we wait */
            }
        }
    } while(!sftp_handle);

    qDebug() << "libssh2_sftp_open() is done, now receive data!";
    do {
        do {
            char mem[readSize] = {0x00, };
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

    libssh2_sftp_close(sftp_handle);
    fclose(tempStorage);

    tempStorage = fopen(storage, "rb");
    if(!tempStorage)
    {
        /* weird, we can't read the file we just wrote to... */
        qDebug() << "can't open %s for reading" << storage;
        return;
    }

    libssh2_sftp_shutdown(sftp_session);

    libssh2_session_disconnect(session, "Normal Shutdown");
    libssh2_session_free(session);

#ifdef WIN32
    closesocket(sock);
#else
    close(sock);
#endif

    if(tempStorage)
    {
        fclose(tempStorage);
    }

    libssh2_exit();
#endif
}


void QUpdateManager::checkUpdate()
{
    //1.Download UpgradeHistory.info file from upgrade server
    if(QCoreServiceManager::Instance().Settings().isNull())
        return;

#ifdef WISENET_S1_VERSION
#ifdef Q_OS_WINDOWS
    std::thread checkUpdateThread([this]() {
        std::string serverFilePath = QCoreServiceManager::Instance().Settings()->s1UpdateServerURL().toStdString() + "wisenetviewer-update-info.txt";
        std::string localFilePath = (QCoreApplication::applicationDirPath().toStdString()+ "/wisenetviewer-update-info.txt").c_str();
        downloadFileForS1(serverFilePath, localFilePath);

        // open and parse the info-file
        m_file = openFileForReadWrite(QString::fromStdString(localFilePath));
        if(!m_file)
        {
            return;
        }

        m_mapWinUpdateFile.clear();

        QTextStream textStream(m_file.get());
        textStream.seek(0);

        QString line = textStream.readLine(); //read one line at a time
        QStringList updateInfoList = line.split(",");
//        qDebug() << "QUpdateManager::checkUpdate - version:" << updateInfoList[0] << "fileName:" << updateInfoList[1];
        m_mapWinUpdateFile.insert(updateInfoList[0], updateInfoList[1]);

        bool nNeedtoUpdate = checkSoftwareVersion(updateInfoList[0]);
        if(nNeedtoUpdate)
        {
            qDebug() << "[Windows] Need to Update to Ver:" << m_mapWinUpdateFile.lastKey();
            m_bNeedUpdate = true;
        }
    });

    checkUpdateThread.detach();
#endif
#else
    QString strServerURL = QCoreServiceManager::Instance().Settings()->updateSeverURL();
    QString infoFilename = "UpgradeHistory.info";
    QUrl strUrl = strServerURL + infoFilename;
    QString fileName = QCoreApplication::applicationDirPath() + "/" + infoFilename;
    m_file = openFileForReadWrite(fileName);
    if (!m_file)
         return;

    reply = qnam.get(QNetworkRequest(strUrl));
    connect(reply, &QNetworkReply::finished, this, &QUpdateManager::infoFileDownloadFinishedStartAnalyze);
    connect(reply, &QIODevice::readyRead, this, &QUpdateManager::httpReadyRead);
#endif
}

std::unique_ptr<QFile> QUpdateManager::openFileForReadWrite(const QString &fileName)
{
    std::unique_ptr<QFile> file(new QFile(fileName));
    if (!file->open(QIODevice::ReadWrite)) {
        return nullptr;
    }
    return file;
}

void QUpdateManager::infoFileDownloadFinishedStartAnalyze()
{
    qDebug() << "QUpdateManager::infoFileDownloadFinishedStartAnalyze";
    m_mapWinUpdateFile.clear();
    m_mapMacUpdateFile.clear();
    m_mapLinuxUpdateFile.clear();
    m_bNeedUpdate = false;
    if(m_file)
    {
        if(!m_file->size())
        {
            QFileInfo fi;
            fi.setFile(m_file->fileName());
            m_file->close();
            m_file.reset();
            qDebug() << "QUpdateManager::file size zero";
            if (reply->error()) {
                qDebug() << "QUpdateManager::reply->error()";
                QFile::remove(fi.absoluteFilePath());
                reply->deleteLater();
                reply = nullptr;
            }
            return;
        }
        QTextStream in(m_file.get());
        in.seek(0);
        while (!in.atEnd())
        {
            QString line = in.readLine(); //read one line at a time
            QStringList lineContents = line.split(",");
            SPDLOG_DEBUG("infoFileDownloadFinishedStartAnalyze {}", line.toStdString());
            if(lineContents[0] == "1") {
                m_mapWinUpdateFile.insert(lineContents[1], lineContents[2]);
            } else if(lineContents[0] == "2") {
                m_mapMacUpdateFile.insert(lineContents[1], lineContents[2]);
            } else if(lineContents[0] == "3") {
                m_mapLinuxUpdateFile.insert(lineContents[1], lineContents[2]);
            }
        }
    }

    if (reply->error()) {
        QFileInfo fi;
        if(m_file != nullptr) {
            fi.setFile(m_file->fileName());
            m_file->close();
            m_file.reset();
            QFile::remove(fi.absoluteFilePath());
        }
        qDebug() << "QUpdateManager::reply->error()";
        reply->deleteLater();
        reply = nullptr;
        return;
    }

    QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();
    if(osType == QOperatingSystemVersion::MacOS)
    {
        if(m_mapMacUpdateFile.size() == 0)
            return;
        QString strMacVersion = m_mapMacUpdateFile.lastKey();
        bool nNeedtoUpdate = checkSoftwareVersion(strMacVersion);
        if(nNeedtoUpdate)
        {
            qDebug() << "[MacOS] Need to Update to Ver:" << strMacVersion;
            m_bNeedUpdate = true;
        }
        else
        {
            qDebug() << "[MacOS] Don't Need to Update to Ver:" << strMacVersion;
        }
    }
    else if(osType == QOperatingSystemVersion::Windows)
    {
        if(m_mapWinUpdateFile.size() == 0)
            return;
        QString strWinVersion = m_mapWinUpdateFile.lastKey();
        bool nNeedtoUpdate = checkSoftwareVersion(strWinVersion);
        if(nNeedtoUpdate)
        {
            qDebug() << "[Windows] Need to Update to Ver:" << strWinVersion;
            m_bNeedUpdate = true;
        }
        else
        {
            qDebug() << "[Windows] Don't Need to Update to Ver:" << strWinVersion;
        }
    }
    else
    {
        SPDLOG_DEBUG("Not support OS Type: ", osType);
    }

    QFileInfo fi;
    if (m_file) {
        fi.setFile(m_file->fileName());
        m_file->close();
        m_file.reset();
    }

    if (reply->error()) {
        QFile::remove(fi.absoluteFilePath());
    }

    reply->deleteLater();
    reply = nullptr;

    if(m_bNeedUpdate)
    {
        QCoreServiceEventPtr qUpdateEvent(new QCoreServiceEvent);
        auto showSystemUpdatePanelEvent = std::make_shared<Wisenet::Core::ShowSystemUpdatePanelEvent>();
        qUpdateEvent->eventDataPtr = showSystemUpdatePanelEvent;

        emit coreServiceEventTriggered(qUpdateEvent);
    }
}


void QUpdateManager::httpReadyRead()
{
    if (m_file)
    {
        m_file->write(reply->readAll());
    }
}

bool QUpdateManager::checkSoftwareVersion(QString &strVersion)
{
    QStringList newVersion = strVersion.split(".");
    newVersion[2] = newVersion[2].split("_")[0]; // 언더바(_) 뒤의 문자열 제거
    QStringList curVersion = QCoreApplication::applicationVersion().split(".");

    QString newMajor, newMinor, newMicro;
    QString curMajor, curMinor, curMicro;

    // 최신 버전의 숫자 이외 문자 제거
    QStringList newVersionCopy = newVersion;
    for(int i=0; i<newVersionCopy.size(); ++i)
    {
        auto nv = newVersionCopy[i];
        for(int j=0; j<nv.size(); ++j)
        {
            if(nv[j] >= '0' && nv[j] <= '9')
            {
                switch (i) {
                case 0:
                    newMajor.push_back(nv[j]);
                    break;
                case 1:
                    newMinor.push_back(nv[j]);
                    break;
                case 2:
                    newMicro.push_back(nv[j]);
                    break;
                default:
                    break;
                }
            }
        }
    }

    // 현재 버전의 숫자 이외 문자 제거
    QStringList curVersionCopy = curVersion;
    for(int i=0; i<curVersionCopy.size(); ++i)
    {
        auto cv = curVersionCopy[i];
        for(int j=0; j<cv.size(); ++j)
        {
            if(cv[j] >= '0' && cv[j] <= '9')
            {
                switch (i) {
                case 0:
                    curMajor.push_back(cv[j]);
                    break;
                case 1:
                    curMinor.push_back(cv[j]);
                    break;
                case 2:
                    curMicro.push_back(cv[j]);
                    break;
                default:
                    break;
                }
            }
        }
    }

    qDebug() << "QUpdateManager::newVersion():"<<newMajor<<"."<<newMinor<<"."<<newMicro;
    qDebug() << "QUpdateManager::curVersion():"<<curMajor<<"."<<curMinor<<"."<<curMicro;

    if(newMajor.toInt() > curMajor.toInt())
    {
        return true;
    }
    else if(newMajor.toInt() == curMajor.toInt())
    {
        if(newMinor.toInt() > curMinor.toInt())
        {
            return true;
        }
        else if(newMinor.toInt() == curMinor.toInt())
        {
            if(newMicro.toInt() > curMicro.toInt())
            {
                return true;
            }
        }
    }
    return false;
}

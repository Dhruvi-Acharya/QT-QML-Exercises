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
#include "QLocalSettings.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>
#include <QNetworkInterface>
#include "BaseStructure.h"
#include "AppLocalDataLocation.h"
#include "LogSettings.h"

QLocalSettings::QLocalSettings(QString filePath, QSettings::Format format, QObject *parent)
    : QObject(parent)
    , m_fileName(filePath)
    , m_settingsFormat(format)
{
    m_machineUniqueId = QString::fromStdString(QSysInfo::machineUniqueId().toStdString());
    SPDLOG_DEBUG("m_machineUniqueId :{}", m_machineUniqueId.toStdString());
    m_crypto.Init(m_machineUniqueId);
}

QString QLocalSettings::id()
{
    return m_id;
}

QString QLocalSettings::loginId()
{
    return m_loginId;
}

QString QLocalSettings::password()
{
    return m_password;
}

bool QLocalSettings::savePassword()
{
    return m_savePassword;
}

bool QLocalSettings::hashPassword()
{
    return m_hashPassword;
}

bool QLocalSettings::autoLogin()
{
    return m_autoLogin;
}

bool QLocalSettings::licenseAgreed()
{
    return m_licenseAgreed;
}

bool QLocalSettings::collectingAgreed()
{
    return m_collectingAgreed;
}

bool QLocalSettings::statisticsTest()
{
    return m_statisticsTest;
}

bool QLocalSettings::containCollectingAgreed()
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Login");
    bool contains = settings.contains("CollectingAgreed");
    settings.endGroup();
    qDebug() << "QLocalSettings::containCollectingAgreed()" << contains;

    return contains;
}

QString QLocalSettings::language()
{
    return m_language;
}

QString QLocalSettings::localRecordingPath()
{
    return m_localRecordingPath;
}

int QLocalSettings::liveBufferLength()
{
    return m_liveBufferLength;
}

int QLocalSettings::liveVideoLossTimeout()
{
    return m_liveVideoLossTimeout;
}

int QLocalSettings::playVideoLossTimeout()
{
    return m_playVideoLossTimeout;
}

int QLocalSettings::localRecordingTime()
{
    return m_localRecordingTime;
}

int QLocalSettings::localRecordingFormat()
{
    return m_localRecordingFormat;
}

int QLocalSettings::maxLayoutVideoItems()
{
    return m_maxLayoutVideoItems;
}

int QLocalSettings::maxLayoutWebItems()
{
    return m_maxLayoutWebItems;
}

int QLocalSettings::mediaReconnectPeriod()
{
    return m_mediaReconnectPeriod;
}

bool QLocalSettings::dpiPassThrough()
{
    return m_dpiPassThrough;
}

QString QLocalSettings::autoRegisterId()
{
    return m_autoRegisterId;
}

QString QLocalSettings::autoRegisterPassword()
{
    return m_autoRegisterPassword;
}

bool QLocalSettings::autoRegister()
{
    return m_autoRegister;
}

bool QLocalSettings::autoStart()
{
    return m_autoStart;
}

bool QLocalSettings::useHwDecoding()
{
    return m_useHwDecoding;
}

bool QLocalSettings::useController()
{
    return m_useController;
}

int QLocalSettings::qsvChannelCount()
{
    return m_qsvChannelCount;
}

int QLocalSettings::cudaChannelCount()
{
    return m_cudaChannelCount;
}

int QLocalSettings::toolboxChannelCount()
{
    return m_toolboxChannelCount;
}

bool QLocalSettings::displayOsd()
{
    return m_displayOsd;
}

bool QLocalSettings::displayVideoStatus()
{
    return m_displayVideoStatus;
}

unsigned short QLocalSettings::s1SipDaemonPort()
{
    return m_s1SipDaemonPort;
}

bool QLocalSettings::eventTestVisible()
{
    return m_eventTestVisible;
}

int QLocalSettings::maxEventCount()
{
    return m_maxEventCount;
}

QString QLocalSettings::eventTestDeviceId()
{
    return m_testDeviceId;
}

QString QLocalSettings::eventTestChannelId()
{
    return m_testCameraId;
}

bool QLocalSettings::eventNotificationShowAll()
{
    return m_eventNotificationShowAll;
}
bool QLocalSettings::systemNotificationShowAll()
{
    return m_systemNotificationShowAll;
}
QStringList QLocalSettings::eventNotifications()
{
    return m_eventNotifications;
}

void QLocalSettings::setId(const QString& id)
{
    m_id = id;
}

void QLocalSettings::setLoginId(const QString& id)
{
    m_loginId = id;
}

void QLocalSettings::setPassword(const QString& password)
{
    m_password = password;
}

void QLocalSettings::setSavePassword(const bool savePassord)
{
    m_savePassword = savePassord;
}

void QLocalSettings::setHashPassword(const bool hash)
{
    m_hashPassword = hash;
}

void QLocalSettings::setAutoLogin(const bool autoLogin)
{
    m_autoLogin = autoLogin;
}

void QLocalSettings::setLicenseAgreed()
{
    m_licenseAgreed = true;

    QSettings settings(m_fileName, m_settingsFormat);
    settings.beginGroup("Login");
    settings.setValue("LicenseAgreed", true);
    settings.endGroup();
}

void QLocalSettings::setCollectingAgreed(const bool collectingAgreed)
{
    m_collectingAgreed = collectingAgreed;

    QSettings settings(m_fileName, m_settingsFormat);
    settings.beginGroup("Login");
    settings.setValue("CollectingAgreed", m_collectingAgreed);
    settings.endGroup();
}

void QLocalSettings::setLanguage(const QString& language)
{
    m_language = language;
}

void QLocalSettings::setLocalRecordingPath(const QString& localRecordingPath)
{
    if (m_localRecordingPath != localRecordingPath) {
        m_localRecordingPath = localRecordingPath;
        emit localRecordingPathChanged();
    }
}

void QLocalSettings::setLiveBufferLength(const int liveBuffer)
{
    m_liveBufferLength = liveBuffer;
}

void QLocalSettings::setLocalRecordingTime(const int localRecordingTime)
{
    m_localRecordingTime = localRecordingTime;
}

void QLocalSettings::setLocalRecordingFormat(const int localRecordingFormat)
{
    m_localRecordingFormat = localRecordingFormat;
}

void QLocalSettings::setAutoRegisterId(const QString& autoRegisterId)
{
    m_autoRegisterId = autoRegisterId;
}

void QLocalSettings::setAutoRegisterPassword(const QString& autoRegisterPassword)
{
    m_autoRegisterPassword = autoRegisterPassword;
}

void QLocalSettings::setAutoRegister(const bool autoRegister)
{
    m_autoRegister = autoRegister;
}

void QLocalSettings::setAutoStart(const bool autoStart)
{
    m_autoStart = autoStart;
}

void QLocalSettings::setUseHwDecoding(const bool useHwDecoding)
{
    m_useHwDecoding = useHwDecoding;
}

void QLocalSettings::setUseController(const bool useController)
{
    m_useController = useController;
}

void QLocalSettings::setEventNotificationShowAll( bool isShowAll )
{
    m_eventNotificationShowAll = isShowAll;
}
void QLocalSettings::setSystemNotificationShowAll( bool isShowAll )
{
    m_systemNotificationShowAll = isShowAll;
}
void QLocalSettings::setEventNotifications( QStringList events )
{
    m_eventNotifications = events.size()>0?events:QStringList({""});
}

void QLocalSettings::setDisplayOsd(bool osd)
{
    m_displayOsd = osd;
}

void QLocalSettings::setDisplayVideoStatus(bool videoStatus)
{
    m_displayVideoStatus = videoStatus;
}

void QLocalSettings::setS1SipDaemonPort(unsigned short s1SipDaemonPort)
{
    m_s1SipDaemonPort = s1SipDaemonPort;
}

void QLocalSettings::loadSetting()
{
    QSettings settings(m_fileName, m_settingsFormat);
    qDebug() << "QLocalSettings::loadSetting() file path : " << settings.fileName()  << " Exist:" << QFile::exists(settings.fileName());

    settings.beginGroup("Login");
    m_id = settings.value("ID", "").toString();
    m_hashPassword = settings.value("HashPassword", false).toBool();

    QByteArray passwordArray = settings.value("Password", QByteArray()).toByteArray();
    m_password = m_crypto.Decrypt(passwordArray);
    m_savePassword = settings.value("SavePassword", false).toBool();
    m_autoLogin = settings.value("AutoLogin", false).toBool();
    m_licenseAgreed = settings.value("LicenseAgreed", false).toBool();
    m_collectingAgreed = settings.value("CollectingAgreed", false).toBool();
    m_statisticsTest = settings.value("StatisticsTest", false).toBool();
    settings.endGroup();

    settings.beginGroup("Main");
    m_language = settings.value("Language", "English-USA").toString();
    if(m_language == "English")
        m_language = "English-USA";
    m_localRecordingPath = settings.value("LocalRecordingPath", GetDefaultMediaLocation()).toString().toUtf8();
    m_localRecordingTime = settings.value("LocalRecordingTime", defaultLocalRecrodingTime).toInt();
    m_localRecordingFormat = settings.value("LocalRecordingFormat", defaultLocalRecordingFormat).toInt();
    m_liveBufferLength = settings.value("LiveBufferLength", defaultLiveBufferLength).toUInt();
    m_liveVideoLossTimeout = settings.value("LiveVideoLossTimeout", defaultLiveVideoLossTimeout).toUInt();
    m_playVideoLossTimeout = settings.value("PlayVideoLossTimeout", defaultPlayVideoLossTimeout).toUInt();
    m_maxLayoutVideoItems = settings.value("MaxLayoutVideoItems", m_maxLayoutVideoItems).toInt();
    m_maxLayoutWebItems = settings.value("MaxLayoutWebItems", m_maxLayoutWebItems).toInt();
    m_mediaReconnectPeriod = settings.value("MediaReconnectPeriod", 10).toInt();
    m_dpiPassThrough = settings.value("DpiPassThrough", true).toBool();
    m_autoStart = settings.value("AutoStart", m_autoStart).toBool();
    m_useHwDecoding = settings.value("UseHardwareDecoding", true).toBool();
    m_useController = settings.value("UseController", false).toBool();
    m_qsvChannelCount = settings.value("IntelQuickSync", 8).toInt();
    m_cudaChannelCount = settings.value("NVidiaCuvid", 16).toInt();
    m_toolboxChannelCount = settings.value("VideoToolbox", 0).toInt();
    m_displayOsd = settings.value("DisplayOsd", true).toBool();
    m_displayVideoStatus = settings.value("DisplayVideoStatus", false).toBool();
    m_s1SipDaemonPort = (unsigned short)settings.value("S1SipDaemonPort", 1881).toInt();

    settings.endGroup();

    settings.beginGroup("Device");
    m_autoRegisterId = settings.value("AutoRegisterId", "").toString();
    m_autoRegister = settings.value("AutoRegister", false).toBool();

    if(m_autoRegister)
    {
        QByteArray autoRegisterPasswordArray = settings.value("AutoRegisterPassword", QByteArray()).toByteArray();
        m_autoRegisterPassword = m_crypto.Decrypt(autoRegisterPasswordArray);
    }
    else
    {
        m_autoRegisterPassword = "";
    }

    settings.endGroup();

    settings.beginGroup("Event");
    m_eventTestVisible = settings.value("EventTestVisible", false).toBool();
    m_maxEventCount = settings.value("MaxEventCount", 1000).toInt();
    m_testDeviceId = settings.value("EventTestDeviceId", "00000000-0000-0000-0000-000000000000").toString();
    m_testCameraId = settings.value("EventTestCameraId", "1").toString();
    QStringList listDefaultEventNotifications;
    listDefaultEventNotifications<<"";
    m_eventNotifications = settings.value("EventNotifications", listDefaultEventNotifications).toStringList();
    m_eventNotificationShowAll = settings.value("EventNotificationShowAll", true).toBool();
    m_systemNotificationShowAll = settings.value("SystemNotificationShowAll", true).toBool();
    settings.endGroup();

    settings.beginGroup("Update");
    m_updateServerURL = settings.value("UpdateServerURL", "https://update.hanwha-security.com/Software/WisenetViewer/").toString();
    m_s1UpdateServerURL = settings.value("S1UpdateServerURL", "/PcViewer/htw/x64/").toString();
    m_deviceUpdateServerURL = settings.value("DeviceUpdateServerURL", "https://update.hanwha-security.com/FW/").toString();
    m_checkUpdateServerInterval = settings.value("CheckUpdateServerInterval", 86400).toInt();
    settings.endGroup();

    //qDebug() << "QLocalSettings::LoadSetting() Login:" << m_id << m_password << m_savePassword << m_autoLogin;
    qDebug() << "QLocalSettings::LoadSetting() Main: language=" << m_language << ", localPath=" << m_localRecordingPath << ", passThrough=" << m_dpiPassThrough;
    //qDebug() << "QLocalSettings::LoadSetting() Device:" << m_autoRegisterId << " " << m_autoRegisterPassword << " " << m_autoRegister;
}

void QLocalSettings::saveSetting()
{
    QSettings settings(m_fileName, m_settingsFormat);
    settings.beginGroup("Login");
    settings.setValue("ID", m_id);
    settings.setValue("SavePassword", m_savePassword);
    settings.setValue("AutoLogin", m_autoLogin);
    settings.setValue("StatisticsTest", m_statisticsTest);

    if(m_password != ""){
        settings.setValue("HashPassword", m_hashPassword);
        settings.setValue("Password", m_crypto.Encrypt(m_password));
    }
    else{
        settings.setValue("HashPassword", false);
        settings.setValue("Password", "");
    }
    settings.endGroup();

    settings.beginGroup("Main");
    settings.setValue("Language", m_language);
    settings.setValue("LocalRecordingPath", m_localRecordingPath.toUtf8());
    settings.setValue("LocalRecordingTime", m_localRecordingTime);
    settings.setValue("LocalRecordingFormat", m_localRecordingFormat);
    settings.setValue("LiveBufferLength", m_liveBufferLength);
    settings.setValue("LiveVideoLossTimeout", m_liveVideoLossTimeout);
    settings.setValue("PlayVideoLossTimeout", m_playVideoLossTimeout);
    settings.setValue("MaxLayoutVideoItems", m_maxLayoutVideoItems);
    settings.setValue("MaxLayoutWebItems", m_maxLayoutWebItems);
    settings.setValue("MediaReconnectPeriod", m_mediaReconnectPeriod);
    settings.setValue("DpiPassThrough", m_dpiPassThrough);
    settings.setValue("AutoStart", m_autoStart);
    settings.setValue("UseHardwareDecoding", m_useHwDecoding);
    settings.setValue("UseController", m_useController);
    settings.setValue("IntelQuickSync", m_qsvChannelCount);
    settings.setValue("NVidiaCuvid", m_cudaChannelCount);
    settings.setValue("VideoToolbox", m_toolboxChannelCount);
    settings.setValue("S1SipDaemonPort", m_s1SipDaemonPort);

    settings.endGroup();

    settings.beginGroup("Device");
    settings.setValue("AutoRegisterId", m_autoRegisterId);

    if(m_autoRegisterPassword != "")
        settings.setValue("AutoRegisterPassword", m_crypto.Encrypt(m_autoRegisterPassword));
    else
        settings.setValue("AutoRegisterPassword", "");

    settings.setValue("AutoRegister", m_autoRegister);
    settings.endGroup();

    settings.beginGroup("Event");
    settings.setValue("EventTestVisible", m_eventTestVisible);
    settings.setValue("MaxEventCount", m_maxEventCount);
    settings.setValue("EventTestDeviceId", m_testDeviceId);
    settings.setValue("EventTestCameraId", m_testCameraId);
    settings.setValue("EventNotifications", m_eventNotifications);
    settings.setValue("EventNotificationShowAll", m_eventNotificationShowAll);
    settings.setValue("SystemNotificationShowAll", m_systemNotificationShowAll);
    settings.endGroup();

    settings.beginGroup("Update");
    settings.setValue("UpdateServerURL", m_updateServerURL);
    settings.setValue("S1UpdateServerURL", m_s1UpdateServerURL);
    settings.setValue("DeviceUpdateServerURL", m_deviceUpdateServerURL);
    settings.setValue("CheckUpdateServerInterval", m_checkUpdateServerInterval);
    settings.endGroup();

    qDebug() << "QLocalSettings::saveSetting() :" << m_language << " " << m_localRecordingPath;
}

void QLocalSettings::reset()
{
    qDebug() << "QLocalSettings::resetCredential() start";
    m_id = "";
    m_password = "";
    m_savePassword = false;
    m_autoLogin =false;
    m_useHwDecoding = false;
    m_useController = false;
    m_hashPassword = false;

    m_language ="English-USA";
    m_localRecordingPath= GetDefaultMediaLocation();
    m_autoRegisterId = "";
    m_autoRegisterPassword = "";
    m_autoRegister = false;

    m_localRecordingTime = defaultLocalRecrodingTime;
    m_localRecordingFormat = defaultLocalRecordingFormat; // 0: mkv, 1: wnm
    m_liveBufferLength = defaultLiveBufferLength;
    m_liveVideoLossTimeout = defaultLiveVideoLossTimeout;
    m_playVideoLossTimeout = defaultPlayVideoLossTimeout;

    saveSetting();
    qDebug() << "QLocalSettings::resetCredential() end";

}

void QLocalSettings::saveCudaChannelCount(int count)
{
    m_cudaChannelCount = count;
    saveSetting();
}

void QLocalSettings::saveQsvChannelCount(int count)
{
    m_qsvChannelCount = count;
    saveSetting();
}

void QLocalSettings::saveToolboxChannelCount(int count)
{
    m_toolboxChannelCount = count;
    saveSetting();
}

void QLocalSettings::saveLayouts(const QString& id, const QList<QStringList>& layoutList)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Layout");
    settings.setValue(id, QVariant::fromValue(layoutList));
    settings.endGroup();
}

void QLocalSettings::loadLayouts(const QString& id, QList<QStringList>& layoutList)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Layout");
    layoutList = settings.value(id).value<QList<QStringList>>();
    settings.endGroup();
}


void QLocalSettings::saveScreen(const QString& id, const QStringList& screen)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Screen");
    settings.setValue(id, QVariant::fromValue(screen));
    settings.endGroup();
}

void QLocalSettings::loadScreen(const QString& id, QStringList& screen)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Screen");
    screen = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::saveWindowPosition(const QString& id, const QStringList& windowPositions)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("WindowPosition");
    settings.setValue(id, QVariant::fromValue(windowPositions));
    settings.endGroup();
}

void QLocalSettings::loadWindowPosition(const QString& id, QStringList& windowPosition)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("WindowPosition");
    windowPosition = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::saveDisplayOsd(const QString &id, const QStringList &osd)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("DisplayOsd");
    settings.setValue(id, QVariant::fromValue(osd));
    settings.endGroup();
}

void QLocalSettings::loadDisplayOsd(const QString &id, QStringList &osd)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("DisplayOsd");
    osd = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::saveDisplayVideoStatus(const QString &id, const QStringList &videoStatus)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("DisplayVideoStatus");
    settings.setValue(id, QVariant::fromValue(videoStatus));
    settings.endGroup();
}

void QLocalSettings::loadDisplayVideoStatus(const QString &id, QStringList &videoStatus)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("DisplayVideoStatus");
    videoStatus = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::savePanelFlipList(const QString &id, const QList<QStringList> &panelFlipList)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("PanelFlipList");
    settings.setValue(id, QVariant::fromValue(panelFlipList));
    settings.endGroup();
}

void QLocalSettings::loadPanelFlipList(const QString &id, QList<QStringList> &panelFlipList)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("PanelFlipList");
    panelFlipList = settings.value(id).value<QList<QStringList>>();
    settings.endGroup();
}

void QLocalSettings::saveViewerModes(const QString &id, const QStringList &viewerModes)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("ViewerModes");
    settings.setValue(id, QVariant::fromValue(viewerModes));
    settings.endGroup();
}

void QLocalSettings::loadViewerModes(const QString &id, QStringList &viewerModes)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("ViewerModes");
    viewerModes = settings.value(id).value<QStringList>();
    settings.endGroup();
}

QString QLocalSettings::updateSeverURL()
{
    return m_updateServerURL;
}

QString QLocalSettings::s1UpdateServerURL()
{
    return m_s1UpdateServerURL;
}

QString QLocalSettings::deviceUpdateSeverURL()
{
    return m_deviceUpdateServerURL;
}

qint64 QLocalSettings::checkUpdateServerInterval()
{
    return m_checkUpdateServerInterval;
}

bool QLocalSettings::checkLicenseValidation(const QString &key)
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();

    foreach (QNetworkInterface interface, interfaces)
    {
        if(interface.hardwareAddress() != "" && m_crypto.LicenseEncrypt(interface.hardwareAddress()) == key)
            return true;
    }

    return false;
}

void QLocalSettings::saveShortcutUse(const QList<bool>& shortcutUseList)
{
    qDebug() << "QLocalSettings::saveShortcutUse";
    QSettings settings(m_fileName, m_settingsFormat);
    QVariantList shortcutVarlist;

    for(bool inUse : shortcutUseList)
        shortcutVarlist << inUse;

    settings.beginGroup("Shortcut");
    settings.setValue("ShortcutUse", shortcutVarlist);
    settings.endGroup();
}

void QLocalSettings::loadShortcutUse(QList<bool>& shortcutUseList)
{
    qDebug() << "QLocalSettings::loadShortcutUse";
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("Shortcut");
    for(QVariant data: settings.value("ShortcutUse").toList())
        shortcutUseList << data.toBool();
    settings.endGroup();
}

void QLocalSettings::saveIsLinkedMode(const QString& id, const QStringList& isLinkedMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsLinkedMode");
    settings.setValue(id, QVariant::fromValue(isLinkedMode));
    settings.endGroup();
}

void QLocalSettings::saveIsThumbnailMode(const QString& id, const QStringList& isThumbnailMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsThumbnailMode");
    settings.setValue(id, QVariant::fromValue(isThumbnailMode));
    settings.endGroup();
}

void QLocalSettings::saveIsMultiTimelineMode(const QString& id, const QStringList& isMultiTimelineMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsMultiTimelineMode");
    settings.setValue(id, QVariant::fromValue(isMultiTimelineMode));
    settings.endGroup();
}

void QLocalSettings::saveTimelineHeight(const QString& id, const QStringList& timelineHeight)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("TimelineHeight");
    settings.setValue(id, QVariant::fromValue(timelineHeight));
    settings.endGroup();
}

void QLocalSettings::loadIsLinkedMode(const QString &id, QStringList &isLinkedMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsLinkedMode");
    isLinkedMode = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::loadIsThumbnailMode(const QString& id, QStringList& isThumbnailMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsThumbnailMode");
    isThumbnailMode = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::loadIsMultiTimelineMode(const QString& id, QStringList& isMultiTimelineMode)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("IsMultiTimelineMode");
    isMultiTimelineMode = settings.value(id).value<QStringList>();
    settings.endGroup();
}

void QLocalSettings::loadTimelineHeight(const QString& id, QStringList& timelineHeight)
{
    QSettings settings(m_fileName, m_settingsFormat);

    settings.beginGroup("TimelineHeight");
    timelineHeight = settings.value(id).value<QStringList>();
    settings.endGroup();
}

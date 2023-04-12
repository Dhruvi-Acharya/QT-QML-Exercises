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
#include <QString>
#include <QSettings>
#include <QSysInfo>
#include "CryptoAdapter.h"

class QLocalSettings : public QObject
{
    Q_OBJECT

public:
    explicit QLocalSettings(QString filePath, QSettings::Format format, QObject *parent = nullptr);

    QString id();

    QString loginId();
    QString password();
    bool savePassword();
    bool autoLogin();
    bool licenseAgreed();
    bool collectingAgreed();
    bool statisticsTest();
    bool containCollectingAgreed();
    bool hashPassword();

    QString language();
    QString localRecordingPath();
    int liveBufferLength();
    int liveVideoLossTimeout();
    int playVideoLossTimeout();
    int localRecordingTime();
    int localRecordingFormat();
    int maxLayoutVideoItems();
    int maxLayoutWebItems();
    int mediaReconnectPeriod();
    bool dpiPassThrough();

    QString autoRegisterId();
    QString autoRegisterPassword();
    bool autoRegister();
    bool autoStart();

    bool useHwDecoding();
    bool useController();
    int qsvChannelCount();
    int cudaChannelCount();
    int toolboxChannelCount();

    bool displayOsd();
    bool displayVideoStatus();

    unsigned short s1SipDaemonPort();

    bool eventTestVisible();
    int maxEventCount();
    QString eventTestDeviceId();
    QString eventTestChannelId();

    bool eventNotificationShowAll();
    bool systemNotificationShowAll();
    QStringList eventNotifications();

    void saveLayouts(const QString& id, const QList<QStringList>& layoutList);
    void loadLayouts(const QString& id, QList<QStringList>& layoutList);

    void saveScreen(const QString& id, const QStringList& screen);
    void loadScreen(const QString& id, QStringList& screen);

    void saveWindowPosition(const QString& id, const QStringList& windowPosition);
    void loadWindowPosition(const QString& id, QStringList& windowPosition);

    void saveDisplayOsd(const QString& id, const QStringList& osd);
    void loadDisplayOsd(const QString& id, QStringList& osd);
    void saveDisplayVideoStatus(const QString& id, const QStringList& osd);
    void loadDisplayVideoStatus(const QString& id, QStringList& osd);

    void savePanelFlipList(const QString &id, const QList<QStringList> &panelFlipList);
    void loadPanelFlipList(const QString &id, QList<QStringList> &panelFlipList);
    void saveViewerModes(const QString &id, const QStringList &viewerModes);
    void loadViewerModes(const QString &id, QStringList &viewerModes);

    void saveShortcutUse(const QList<bool>& shortcutUseList);
    void loadShortcutUse(QList<bool>& shortcutUseList);

    void saveIsLinkedMode(const QString& id, const QStringList& isLinkedMode);
    void saveIsThumbnailMode(const QString& id, const QStringList& isThumbnailMode);
    void saveIsMultiTimelineMode(const QString& id, const QStringList& isMultiTimelineMode);
    void saveTimelineHeight(const QString& id, const QStringList& timelineHeight);
    void loadIsLinkedMode(const QString& id, QStringList& isLinkedMode);
    void loadIsThumbnailMode(const QString& id, QStringList& isThumbnailMode);
    void loadIsMultiTimelineMode(const QString& id, QStringList& isMultiTimelineMode);
    void loadTimelineHeight(const QString& id, QStringList& timelineHeight);

    QString updateSeverURL();
    QString s1UpdateServerURL();
    QString deviceUpdateSeverURL();
    qint64 checkUpdateServerInterval();

    bool checkLicenseValidation(const QString& key);

public slots:
    void loadSetting();
    void saveSetting();

    void setId(const QString& id);
    void setLoginId(const QString& id);
    void setPassword(const QString& password);
    void setSavePassword(const bool savePassord);
    void setAutoLogin(const bool autoLogin);
    void setHashPassword(const bool hash);
    void setLicenseAgreed();
    void setCollectingAgreed(const bool collectingAgreed);

    void setLanguage(const QString& language);
    void setLiveBufferLength(const int liveBuffer);
    void setLocalRecordingPath(const QString& localRecordingPath);
    void setLocalRecordingTime(const int localRecordingTime);
    void setLocalRecordingFormat(const int localRecordingFormat);

    void setAutoRegisterId(const QString& autoRegisterId);
    void setAutoRegisterPassword(const QString& autoRegisterPassword);
    void setAutoRegister(const bool autoRegister);
    void setAutoStart(const bool autoStart);
    void setUseHwDecoding(const bool useHwDecoding);
    void setUseController(const bool useController);

    void setEventNotificationShowAll( bool isShowAll );
    void setSystemNotificationShowAll( bool isShowAll );
    void setEventNotifications( QStringList events );
    void setDisplayOsd(const bool osd);
    void setDisplayVideoStatus(const bool videoStatus);

    void setS1SipDaemonPort(unsigned short port);

    void reset();

    void saveCudaChannelCount(int count);
    void saveQsvChannelCount(int count);
    void saveToolboxChannelCount(int count);

signals:
    void localRecordingPathChanged();
private:
    QString m_fileName;
    QSettings::Format m_settingsFormat;

    QString m_id;
    QString m_loginId; // login 직후 저장, Layout 저장 시 사용을 위함.
    QString m_password;
    QString m_language;
    QString m_localRecordingPath;
    QString m_autoRegisterId;
    QString m_autoRegisterPassword;

    int m_liveBufferLength = 100; // milliseconds
    int m_liveVideoLossTimeout = 10000; // milliseconds
    int m_playVideoLossTimeout = 3000; // milliseconds
    int m_localRecordingTime = 5;
    int m_localRecordingFormat = 0; // 0: mkv, 1: wnm
    int m_maxLayoutVideoItems = 64;
    int m_maxLayoutWebItems = 8;
    int m_mediaReconnectPeriod = 10;
    bool m_dpiPassThrough = false;

    bool m_savePassword = false;
    bool m_autoLogin = false;
    bool m_autoRegister = false;
    bool m_licenseAgreed = false;
    bool m_collectingAgreed = false;
    bool m_statisticsTest = false;
    bool m_autoStart = false;
    bool m_hashPassword = false;

    bool m_useHwDecoding = false;
    bool m_useController = false;
    int m_qsvChannelCount = 16;
    int m_cudaChannelCount = 16;
    int m_toolboxChannelCount = 8;

    bool m_eventTestVisible = false;
    bool m_displayOsd = true;
    bool m_displayVideoStatus = false;
    int m_maxEventCount = 1000;
    QString m_testDeviceId = "";
    QString m_testCameraId = "";

    bool m_eventNotificationShowAll = true;
    bool m_systemNotificationShowAll = true;
    QStringList m_eventNotifications = {}; // Events which will not be shown.

    const int defaultLocalRecrodingTime = 5;
    const int defaultLocalRecordingFormat = 0;
    const int defaultLiveBufferLength = 100;
    const int defaultLiveVideoLossTimeout = 10000;
    const int defaultPlayVideoLossTimeout = 3000;

    QString m_machineUniqueId;
    CryptoAdapter m_crypto;

    QString m_updateServerURL = "";
    QString m_s1UpdateServerURL = "";
    QString m_deviceUpdateServerURL = "";
    qint64 m_checkUpdateServerInterval = 86400;

    unsigned short m_s1SipDaemonPort = 1881;
};


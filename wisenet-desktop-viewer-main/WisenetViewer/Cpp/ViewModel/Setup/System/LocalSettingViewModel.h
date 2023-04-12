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

class LocalSettingViewModel : public QObject
{
private:
    Q_OBJECT
    Q_DISABLE_COPY(LocalSettingViewModel)
    Q_PROPERTY(QString language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(int liveBufferLength READ liveBufferLength WRITE setLiveBufferLength NOTIFY liveBufferLengthChanged)
    Q_PROPERTY(QString localRecordingPath READ localRecordingPath WRITE setLocalRecordingPath NOTIFY localRecordingPathChanged)
    Q_PROPERTY(int localRecordingTime READ localRecordingTime WRITE setLocalRecordingTime NOTIFY localRecordingTimeChanged)
    Q_PROPERTY(int localRecordingFormat READ localRecordingFormat WRITE setLocalRecordingFormat NOTIFY localRecordingFormatChanged)
    Q_PROPERTY(bool autoRegister READ autoRegister WRITE setAutoRegister NOTIFY autoRegisterChanged)
    Q_PROPERTY(QString autoRegisterId READ autoRegisterId WRITE setAutoRegisterId NOTIFY autoRegisterIdChanged)
    Q_PROPERTY(QString autoRegisterPassword READ autoRegisterPassword WRITE setAutoRegisterPassword NOTIFY autoRegisterPasswordChanged)
    Q_PROPERTY(bool autoStart READ autoStart WRITE setAutoStart NOTIFY autoStartChanged)
    Q_PROPERTY(bool useHwDecoding READ useHwDecoding WRITE setUseHwDecoding NOTIFY useHwDecodingChanged)
    Q_PROPERTY(bool useController READ useController WRITE setUseController NOTIFY useControllerChanged)

    explicit LocalSettingViewModel(QObject* parent = nullptr);
    ~LocalSettingViewModel();

public:
    static LocalSettingViewModel* getInstance(){
        static LocalSettingViewModel instance;
        return &instance;
    }

    Q_INVOKABLE void saveDeviceCredential(bool autoRegister, QString id, QString password);
    Q_INVOKABLE void saveSetting( bool isNoMsgBox = false );
    Q_INVOKABLE bool isWindows();

    void setLanguage(QString &lang);
    void setLiveBufferLength(int &liveBuffer);
    void setLocalRecordingPath(QString &path);
    void setLocalRecordingTime(int &time);
    void setLocalRecordingFormat(int &format);
    void setAutoRegister(bool autoRegister);
    void setAutoRegisterId(QString &autoRegisterId);
    void setAutoRegisterPassword(QString &autoRegisterPassword);
    void setAutoStart(bool autoStart);
    void setUseHwDecoding(bool useHwDecoding);
    void setUseController(bool useController);

    QString language(){ return m_language;}
    int liveBufferLength(){ return m_liveBufferLength;}
    QString localRecordingPath(){ return m_localRecordingPath;}
    int localRecordingTime(){ return m_localRecordingTime;}
    int localRecordingFormat(){ return m_localRecordingFormat;}
    bool autoRegister(){ return m_autoRegister;}
    QString autoRegisterId(){ return m_autoRegisterId;}
    QString autoRegisterPassword(){ return m_autoRegisterPassword;}
    bool autoStart() {return m_autoStart;}
    bool useHwDecoding() {return m_useHwDecoding;}
    bool useController() {return m_useController;}

    void moveShortcut(bool autoStart);

signals:
    void languageChanged(QString language);
    void liveBufferLengthChanged(int liveBuffer);
    void localRecordingPathChanged(QString path);
    void localRecordingTimeChanged(int time);
    void localRecordingFormatChanged(int format);
    void autoRegisterChanged(bool autoRegister);
    void autoRegisterIdChanged(QString autoRegisterId);
    void autoRegisterPasswordChanged(QString autoRegisterPassword);
    void showMessageBox( QString text, bool bQuit);
    void autoStartChanged(bool autoStart);
    void useHwDecodingChanged(bool useHwDecoding);
    void useControllerChanged(bool useController);

private:
    QString m_language;
    int m_liveBufferLength;
    QString m_localRecordingPath;
    int m_localRecordingTime;
    int m_localRecordingFormat;
    bool m_autoRegister=false;
    QString m_autoRegisterId;
    QString m_autoRegisterPassword;
    bool m_autoStart=false;
    bool m_useHwDecoding = false;
    bool m_useController = false;
};

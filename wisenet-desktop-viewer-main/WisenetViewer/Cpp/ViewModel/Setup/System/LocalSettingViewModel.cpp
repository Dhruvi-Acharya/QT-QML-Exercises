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

#include "LocalSettingViewModel.h"
#include "QCoreServiceManager.h"
#include "DecoderManager.h"
#include "ControllerManager.h"

LocalSettingViewModel::LocalSettingViewModel(QObject* parent) : QObject(parent)
{
    qDebug() << "LocalSettingViewModel()";
    m_language = QCoreServiceManager::Instance().Settings()->language();
    m_localRecordingPath = QCoreServiceManager::Instance().Settings()->localRecordingPath();
    m_localRecordingTime = QCoreServiceManager::Instance().Settings()->localRecordingTime();
    m_localRecordingFormat = QCoreServiceManager::Instance().Settings()->localRecordingFormat();
    m_autoRegister = QCoreServiceManager::Instance().Settings()->autoRegister();
    m_autoRegisterId = QCoreServiceManager::Instance().Settings()->autoRegisterId();
    m_autoRegisterPassword = QCoreServiceManager::Instance().Settings()->autoRegisterPassword();
    m_liveBufferLength = QCoreServiceManager::Instance().Settings()->liveBufferLength();
    m_autoStart = QCoreServiceManager::Instance().Settings()->autoStart();
    m_useHwDecoding = QCoreServiceManager::Instance().Settings()->useHwDecoding();
    m_useController = QCoreServiceManager::Instance().Settings()->useController();

    ControllerManager::Instance()->SetUseController(m_useController);
    qDebug() << "LocalSettingViewModel()" << m_localRecordingPath;
}

LocalSettingViewModel::~LocalSettingViewModel()
{
    qDebug() << "~LocalSettingViewModel()";
}

void LocalSettingViewModel::setLanguage(QString &lang)
{
    qDebug() << "LocalSettingViewModel::setLanguage" << lang;
    if(lang != m_language){
        m_language = lang;
        emit languageChanged(m_language);
        qDebug() << "LocalSettingViewModel::setLanguage2" << m_language;
    }
}

void LocalSettingViewModel::setLiveBufferLength(int &liveBuffer)
{
    qDebug() << "LocalSettingViewModel::setLiveBufferLength" << liveBuffer;
    if(liveBuffer != m_liveBufferLength){
        m_liveBufferLength = liveBuffer;
        emit liveBufferLengthChanged(m_liveBufferLength);
        qDebug() << "LocalSettingViewModel::setLiveBufferLength" << m_liveBufferLength;
    }
}

void LocalSettingViewModel::setLocalRecordingPath(QString &path)
{
    qDebug() << "LocalSettingViewModel::setLocalRecordingPath" << path;
    if(path != m_localRecordingPath){
        m_localRecordingPath = path;
        emit localRecordingPathChanged(m_localRecordingPath);
        qDebug() << "LocalSettingViewModel::setLocalRecordingPath" << m_localRecordingPath;
    }
}

void LocalSettingViewModel::setLocalRecordingTime(int &time)
{
    if(time != m_localRecordingTime){
        m_localRecordingTime = time;
        emit localRecordingTimeChanged(m_localRecordingTime);
    }
}

void LocalSettingViewModel::setLocalRecordingFormat(int &format)
{
    if(format != m_localRecordingFormat){
        m_localRecordingFormat = format;
        emit localRecordingFormatChanged(m_localRecordingFormat);
    }
}

void LocalSettingViewModel::setAutoRegister(bool autoRegister)
{
    qDebug() << "LocalSettingViewModel::setAutoRegister" << autoRegister;
    if(autoRegister != m_autoRegister){
        m_autoRegister = autoRegister;
        emit autoRegisterChanged(m_autoRegister);
        qDebug() << "LocalSettingViewModel::autoRegisterChanged" << m_autoRegister;
    }
}

void LocalSettingViewModel::setAutoRegisterId(QString &autoRegisterId)
{
    qDebug() << "LocalSettingViewModel::setAutoRegisterId" << autoRegisterId;
    if(autoRegisterId != m_autoRegisterId){
        m_autoRegisterId = autoRegisterId;
        emit autoRegisterIdChanged(m_autoRegisterId);
        qDebug() << "LocalSettingViewModel::autoRegisterIdChanged" << m_autoRegisterId;
    }
}

void LocalSettingViewModel::setAutoRegisterPassword(QString &autoRegisterPassword)
{
    qDebug() << "LocalSettingViewModel::setAutoRegisterPassword";
    if(autoRegisterPassword != m_autoRegisterPassword){
        m_autoRegisterPassword = autoRegisterPassword;
        emit autoRegisterPasswordChanged(m_autoRegisterPassword);
        qDebug() << "LocalSettingViewModel::autoRegisterPasswordChanged" ;
    }
}

void LocalSettingViewModel::setAutoStart(bool autoStart)
{
    qDebug() << "LocalSettingViewModel::setAutoStart" << autoStart;
    if(autoStart != m_autoStart){
        m_autoStart = autoStart;
        emit autoStartChanged(m_autoStart);
        qDebug() << "LocalSettingViewModel::autoStartChanged";
    }
}

void LocalSettingViewModel::setUseHwDecoding(bool useHwDecoding)
{
    qDebug() << "LocalSettingViewModel::setUseHwDecoding" << useHwDecoding;
    if(useHwDecoding != m_useHwDecoding){
        m_useHwDecoding = useHwDecoding;
        emit useHwDecodingChanged(m_useHwDecoding);
        qDebug() << "LocalSettingViewModel::useHwDecodingChanged";
    }
}

void LocalSettingViewModel::setUseController(bool useController)
{
    qDebug() << "LocalSettingViewModel::setUseController" << useController;
    if(useController != m_useController){
        m_useController = useController;
        emit useControllerChanged(m_useController);
        qDebug() << "LocalSettingViewModel::useControllerChanged";
    }
}

void LocalSettingViewModel::saveDeviceCredential(bool bAutoRegister, QString id, QString password)
{
    setAutoRegister(bAutoRegister);
    if(bAutoRegister){
        setAutoRegisterId(id);
        setAutoRegisterPassword(password);
    }
    else
    {
        QString temp ="";
        setAutoRegisterId(temp);
        setAutoRegisterPassword(temp);
    }
    qDebug() << "LocalSettingViewModel::saveDeviceCredential " <<
                m_autoRegister << " " <<  m_autoRegisterId;
    QCoreServiceManager::Instance().Settings()->setAutoRegister(m_autoRegister);
    QCoreServiceManager::Instance().Settings()->setAutoRegisterId(m_autoRegisterId);
    QCoreServiceManager::Instance().Settings()->setAutoRegisterPassword(m_autoRegisterPassword);
    QCoreServiceManager::Instance().Settings()->saveSetting();
}

void LocalSettingViewModel::saveSetting( bool isNoMsgBox )
{
    bool isLanguageChange = false;
    bool isAutoStartChange = false;

    if(QCoreServiceManager::Instance().Settings()->language() !=m_language)
        isLanguageChange = true;

    if(QCoreServiceManager::Instance().Settings()->autoStart() != m_autoStart)
        isAutoStartChange = true;

    SPDLOG_DEBUG("LocalSettingViewModel::saveSetting() language : {} language:{} isLanguageChange:{}",
                 QCoreServiceManager::Instance().Settings()->language().toStdString(),m_language.toStdString(), isLanguageChange);

    QCoreServiceManager::Instance().Settings()->setLanguage(m_language);
    QCoreServiceManager::Instance().Settings()->setLocalRecordingPath(m_localRecordingPath);
    QCoreServiceManager::Instance().Settings()->setLocalRecordingTime(m_localRecordingTime);
    QCoreServiceManager::Instance().Settings()->setLocalRecordingFormat(m_localRecordingFormat);
    QCoreServiceManager::Instance().Settings()->setLiveBufferLength(m_liveBufferLength);
    QCoreServiceManager::Instance().Settings()->setAutoStart(m_autoStart);
    QCoreServiceManager::Instance().Settings()->setUseHwDecoding(m_useHwDecoding);
    QCoreServiceManager::Instance().Settings()->setUseController(m_useController);

    QCoreServiceManager::Instance().Settings()->saveSetting();

    DecoderManager::getInstance()->SetUseHwDecoding(m_useHwDecoding);
    ControllerManager::Instance()->SetUseController(m_useController);

    // 저장되어있던 설정과 달라졌을 때 실행하도록
    if (isAutoStartChange)
        moveShortcut(m_autoStart);

    SPDLOG_DEBUG("LocalSettingViewModel::saveSetting()2");
    if( isNoMsgBox == false )
    {
        if(isLanguageChange)
            emit showMessageBox(QCoreApplication::translate("WisenetLinguist","The system restarts"), true);
        else
            emit showMessageBox(QCoreApplication::translate("WisenetLinguist","Local settings changed"), false);
    }
}

void LocalSettingViewModel::moveShortcut(bool autoStart)
{
    qDebug() << "LocalSettingViewModel::qDebug() :: moveShortcut";

    QString copyPath = QDir::currentPath() + "/WisenetViewer.lnk";
    QString shortcutPath = QDir::homePath() +  "/AppData/Roaming/Microsoft/Windows/Start Menu/Programs/Startup/WisenetViewer.lnk";

    // WisenetViewer.exe의 바로가기를 startup 디렉토리 위치로 copy or startup에 있는 바로가기 삭제
    if (autoStart == true)
    {
        qDebug() << "LocalSettingViewModel::moveShortcut : make shortcut";
        if (QFile::exists(copyPath)){
            if(!QFile::copy(copyPath, shortcutPath))
                qDebug() << "LocalSettingViewModel:: Copy Failed";
            else
                qDebug() << "LocalSettingViewModel:: Copy Successed";
        }
    }

    else
    {
        qDebug() << "LocalSettingViewModel::moveShortcut : delete shortcut";

        if(!QFile::remove(shortcutPath))
            qDebug() << "LocalSettingViewModel:: Remove Failed";
        else
            qDebug() << "LocalSettingViewModel:: Remove Successed";
    }
}

bool LocalSettingViewModel::isWindows(){
    qDebug() << "LocalSettingViewModel::isWindows()";

    QOperatingSystemVersion::OSType osType = QOperatingSystemVersion::currentType();
    if(osType == QOperatingSystemVersion::Windows)
        return true;
    else
        return false;
}

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
#include "ExportMediaControl.h"
#include "WisenetMediaPlayer.h"

struct ExportChannelItem
{
    QString deviceId = "";
    QString channeId = "";
    int trackId = -1;
    QString channelName = "";
    WisenetMediaPlayer* player = nullptr;
};

class ExportVideoModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString filePath READ filePath WRITE setFilePath NOTIFY filePathChanged)
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
    Q_PROPERTY(QString folderName READ folderName WRITE setFolderName NOTIFY folderNameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QDateTime fromDateTime READ fromDateTime WRITE setFromDateTime NOTIFY fromDateTimeChanged)
    Q_PROPERTY(QDateTime toDateTime READ toDateTime WRITE setToDateTime NOTIFY toDateTimeChanged)
    Q_PROPERTY(bool enableHighResolution READ enableHighResolution WRITE setEnableHighResolution NOTIFY enableHighResolutionChanged)
    Q_PROPERTY(bool usePassword READ usePassword WRITE setUsePassword NOTIFY usePasswordChanged)
    Q_PROPERTY(bool enableDigitalSignature READ enableDigitalSignature WRITE setEnableDigitalSignature NOTIFY enableDigitalSignatureChanged)

public:
    ExportVideoModel();
    ~ExportVideoModel();

    // Q_PROPERTY
    QString filePath(){return m_filePath;}
    QString fileName(){return m_fileName;}
    QString folderName(){return m_folderName;}
    QString password(){return m_password;}
    QDateTime fromDateTime(){return m_fromDateTime;}
    QDateTime toDateTime(){return m_toDateTime;}
    bool enableHighResolution(){return m_enableHighResolution;}
    bool usePassword(){return m_usePassword;}
    bool enableDigitalSignature(){return m_enableDigitalSignature;}
    void setFilePath(QString path){m_filePath = path; emit filePathChanged();}
    void setFileName(QString name){m_fileName = name; emit fileNameChanged();}
    void setFolderName(QString name){m_folderName = name; emit folderNameChanged();}
    void setPassword(QString password){m_password = password; emit passwordChanged();}
    void setFromDateTime(QDateTime time){m_fromDateTime = time; emit fromDateTimeChanged();}
    void setToDateTime(QDateTime time){m_toDateTime = time; emit toDateTimeChanged();}
    void setEnableHighResolution(bool enable){m_enableHighResolution = enable; emit enableHighResolutionChanged();}
    void setUsePassword(bool use){m_usePassword = use; emit usePasswordChanged();}
    void setEnableDigitalSignature(bool enable){m_enableDigitalSignature = enable; emit enableDigitalSignatureChanged();}

    // Q_INVOKABLE    
    Q_INVOKABLE void setMultiChannelPlayerList(QVariantList playerList);
    Q_INVOKABLE void setMultiChannelList(QVariantList channelList);
    Q_INVOKABLE int getMultiChannelCount(){return m_multiChannelList.size();}
    Q_INVOKABLE QString getChannelName(int index);
    Q_INVOKABLE void startSingleChannelExport(WisenetMediaPlayer* mediaPlayer, QList<int> overlappedIdList);
    Q_INVOKABLE void startMultichannelExport(QVariantList multichannelCheckList);
    Q_INVOKABLE QUrl getExportPathUrl(QString path){return QUrl::fromLocalFile(path);}
    Q_INVOKABLE qint64 getMbytesAvailable();
    Q_INVOKABLE QString getAvailableFileName(QString fileName);

private:
    void startExport(ExportChannelItem channelItem, bool isMultiChannelExport, QList<int> overlappedIdList);

signals:
    void filePathChanged();
    void fileNameChanged();
    void folderNameChanged();
    void passwordChanged();
    void fromDateTimeChanged();
    void toDateTimeChanged();
    void enableHighResolutionChanged();
    void usePasswordChanged();
    void enableDigitalSignatureChanged();

private:
    QString m_filePath = "";
    QString m_fileName = "";
    QString m_folderName = "";
    QString m_password = "";
    QDateTime m_fromDateTime;
    QDateTime m_toDateTime;
    bool m_enableHighResolution = true;
    bool m_usePassword = false;
    bool m_enableDigitalSignature = true;

    QList<ExportChannelItem> m_multiChannelList;
};

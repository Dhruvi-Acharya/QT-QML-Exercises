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
#include <QAbstractListModel>
#include "QCoreServiceManager.h"
#include "UserLayoutTreeSourceModel.h"

class AddUserGroupViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString groupId READ groupId WRITE setGroupId NOTIFY groupIdChanged)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)

    Q_PROPERTY(bool playback READ playback WRITE setPlayback NOTIFY playbackChanged)
    Q_PROPERTY(bool exportVideo READ exportVideo WRITE setExportVideo NOTIFY exportVideoChanged)
    Q_PROPERTY(bool localRecording READ localRecording WRITE setLocalRecording NOTIFY localRecordingChanged)
    Q_PROPERTY(bool ptzControl READ ptzControl WRITE setPtzControl NOTIFY ptzControlChanged)
    Q_PROPERTY(bool audio READ audio WRITE setAudio NOTIFY audioChanged)
    Q_PROPERTY(bool mic READ mic WRITE setMic NOTIFY micChanged)


    Q_PROPERTY(bool isAdminGroup READ isAdminGroup WRITE setIsAdminGroup NOTIFY isAdminGroupChanged)

    Q_PROPERTY(bool isAllResource READ isAllResource WRITE setIsAllResource NOTIFY isAllResourceChanged)

    Q_PROPERTY(QVector<QString> channelGuidList READ channelGuidList WRITE setChannelGuidList NOTIFY channelGuidListChanged)
    Q_PROPERTY(QVector<QString> layoutGuidList READ layoutGuidList WRITE setLayoutGuidList NOTIFY layoutGuidListChanged)

    Q_PROPERTY(QStringList groupNameList READ groupNameList NOTIFY groupNameListChanged)

public:
    AddUserGroupViewModel(QObject* parent = nullptr);
    ~AddUserGroupViewModel();

    Q_INVOKABLE void readUserGroup(QString userGroupId, bool groupView);
    Q_INVOKABLE void saveUserGroup(bool isAllDevice, std::vector<QString> channelList, std::vector<QString> layoutList);
    Q_INVOKABLE QString findUserGroupId(QString name);

    //getter
    QString groupId() { return m_groupId;}
    QString groupName() { return m_groupName;}
    QStringList groupNameList(); //{ return m_groupNameList;}

    bool playback() { return m_playback;}
    bool exportVideo() { return m_exportVideo;}
    bool localRecording() { return m_localRecording;}
    bool ptzControl() { return m_ptzControl;}
    bool audio() { return m_audio;}
    bool mic() { return m_mic;}

    bool isAdminGroup() { return m_isAdminGroup;}
    bool isAllResource() { return m_isAllResource;}
    QVector<QString> channelGuidList() { return m_channelGuidList;}
    QVector<QString> layoutGuidList() { return m_layoutGuidList;}

    //setter
    void setGroupId(const QString& groupId);
    void setGroupName(const QString& groupName);
    void setGroupNameList(const QStringList& groupNameList);
    void setPlayback(bool param);
    void setExportVideo(bool param);
    void setLocalRecording(bool param);
    void setPtzControl(bool param);
    void setAudio(bool param);
    void setMic(bool param);
    void setIsAdminGroup(bool isAdminGroup);
    void setIsAllResource(bool isAllResource);
    void setChannelGuidList(QVector<QString> channelGuidList);
    void setLayoutGuidList(QVector<QString> layoutGuidList);

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void groupIdChanged();
    void groupNameChanged();
    void groupNameListChanged();

    void playbackChanged();
    void exportVideoChanged();
    void localRecordingChanged();
    void ptzControlChanged();
    void audioChanged();
    void micChanged();

    void isAdminGroupChanged();
    void isAllResourceChanged();
    void channelGuidListChanged();
    void layoutGuidListChanged();

    void resultMessage(QString msg, QString groupId="");

private:
    QString m_groupId;
    QString m_groupName;
    QStringList m_groupNameList;

    bool    m_playback = false;
    bool    m_exportVideo = false;
    bool    m_localRecording = false;
    bool    m_ptzControl = false;
    bool    m_audio = false;
    bool    m_mic = false;

    bool    m_isAdminGroup = false;
    bool    m_isAllResource = false;
    QVector<QString>    m_channelGuidList;
    QVector<QString>    m_layoutGuidList;
};

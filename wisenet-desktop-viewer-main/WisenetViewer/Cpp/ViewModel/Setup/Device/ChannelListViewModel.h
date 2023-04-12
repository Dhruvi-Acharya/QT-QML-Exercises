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
#include <set>
#include "QCoreServiceManager.h"

struct DeviceListProfile
{
    int profileNumber;
    QString profileName="";
    QString codec="";
    QString resolution="";
    QString frameRate="";
    QString bitrate="";
};

class ChannelListViewModel  : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int rowCount READ rowCount)
    Q_PROPERTY(bool useChannel READ useChannel WRITE setUseChannel NOTIFY useChannelChanged)
    Q_PROPERTY(bool usePtz READ usePtz WRITE setUsePtz NOTIFY usePtzChanged)
    Q_PROPERTY(bool useDewarping READ useDewarping WRITE setUseDewarping NOTIFY useDewarpingChanged)
    Q_PROPERTY(FisheyeLensLocation fisheyeMount READ fisheyeMount WRITE setFisheyeMount NOTIFY fisheyeMountChanged)
    Q_PROPERTY(QString fisheyeLensType READ fisheyeLensType WRITE setFisheyeLensType NOTIFY fisheyeLensTypeChanged)
    Q_PROPERTY(QString channelName READ channelName NOTIFY channelNameChanged)
    Q_PROPERTY(QString channelId READ channelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString deviceId READ deviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString highProfile READ highProfile WRITE setHighProfile NOTIFY highProfileChanged)
    Q_PROPERTY(QString lowProfile READ lowProfile WRITE setLowProfile NOTIFY lowProfileChanged)
    Q_PROPERTY(int channelCount READ channelCount NOTIFY channelCountChanged)
    Q_PROPERTY(QVector<QString> channelGuidList READ channelGuidList NOTIFY channelGuidListChanged)

public:
    enum DeviceListProfileRoles {
        ProfileNumberRole = Qt::UserRole +1,
        ProfileNameRole,
        CodecRole,
        ResolutionRole,
        FrameRateRole,
        BitrateRole,
    };
    Q_ENUM(DeviceListProfileRoles);

    enum FisheyeLensLocation
    {
        L_Ceiling,
        L_Wall,
        L_Ground
    };
    Q_ENUM(FisheyeLensLocation)

    Q_INVOKABLE void reset();
    Q_INVOKABLE void selectChannels(std::vector<QString> guidList);
    Q_INVOKABLE QVariantMap get(int row);
    Q_INVOKABLE QString getProfileId(QString profileName);
    Q_INVOKABLE void changeChannelInfo(bool use,bool dewarping, FisheyeLensLocation mount, QString lensType,
                                       QString high, QString low, QString newName);

    ChannelListViewModel(QObject* parent = nullptr);

    //getter
    QVector<QString> channelGuidList() { return m_channelGuidList;}
    int channelCount(){ return m_channelGuidList.size();}
    QString channelId(){ return m_channelId; }
    QString deviceId(){ return m_deviceId; }
    QString channelName(){ return m_channelName; }
    QString highProfile() { return m_highProfile;}
    QString lowProfile() { return m_lowProfile;}
    bool useChannel(){ return m_useChannel; }
    bool usePtz(){ return m_usePtz; }
    bool useDewarping(){ return m_useDewarping; }
    FisheyeLensLocation fisheyeMount() { return m_fisheyeMount; }
    QString fisheyeLensType() { return m_fisheyeLensType; }

    //setter
    void setHighProfile(const QString value);
    void setLowProfile(const QString value);
    void setUseChannel(const bool value);
    void setUsePtz(const bool value);
    void setUseDewarping(const bool value);
    void setFisheyeMount(const FisheyeLensLocation value);
    void setFisheyeLensType(const QString value);

    int rowCount(const QModelIndex& parent = QModelIndex()) const;
    int columnCount(const QModelIndex& parent = QModelIndex()) const;
    QVariant data(const QModelIndex& index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    void setChannelName(QString name);
    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void resGetDeviceProfile(const QCoreServiceReplyPtr& reply);

    void GetDeviceProfile();

signals:
    void deviceIdChanged();
    void channelIdChanged();
    void channelCountChanged();
    void channelGuidListChanged();
    void channelNameChanged();
    void useChannelChanged();
    void usePtzChanged();
    void useDewarpingChanged();
    void fisheyeMountChanged();
    void fisheyeLensTypeChanged();

    void highProfileChanged(QString profile);
    void lowProfileChanged(QString profile);
    void applyResult(const bool success);

private:
    QVector<QString>    m_channelGuidList;

    QString m_deviceId;
    QString m_channelId;
    QString m_channelName;
    bool m_useChannel = false;
    bool m_usePtz = false;
    bool m_useDewarping = false;
    FisheyeLensLocation m_fisheyeMount = FisheyeLensLocation::L_Ceiling;
    QString m_fisheyeLensType;

    QString m_highProfile;
    QString m_lowProfile;


    QList<DeviceListProfile> m_datas;
    std::map<int, DeviceListProfile> m_mapData;
    QHash<int, QByteArray> m_roles;
};


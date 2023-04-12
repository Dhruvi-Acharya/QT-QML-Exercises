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
#include <QDateTime>
#include <QUrl>

class WisenetMediaParam : public QObject
{
    friend class WisenetMediaPlayer;

    Q_OBJECT
    Q_PROPERTY(StreamType streamType READ streamType NOTIFY streamTypeChanged)
    Q_PROPERTY(SourceType sourceType READ sourceType NOTIFY sourceTypeChanged)
    Q_PROPERTY(QUrl localResource READ localResource NOTIFY localResourceChanged)
    Q_PROPERTY(QString serviceId READ serviceId NOTIFY serviceIdChanged)
    Q_PROPERTY(QString deviceId READ deviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString channelId READ channelId NOTIFY channelIdChanged)
    Q_PROPERTY(QString profileId READ profileId NOTIFY profileIdChanged)
    Q_PROPERTY(QString sessionId READ sessionId NOTIFY sessionIdChanged)    
    Q_PROPERTY(QString posId READ posId NOTIFY posIdChanged)                        // 2022.10.12. added
    Q_PROPERTY(QString isPosPlayback READ isPosPlayback NOTIFY posPlaybackChanged)  // 2022.10.12. added
    Q_PROPERTY(qint64 startTime READ startTime NOTIFY startTimeChanged)
    Q_PROPERTY(qint64 endTime READ endTime NOTIFY endTimeChanged)
    Q_PROPERTY(qreal speed READ speed NOTIFY speedChanged)
    Q_PROPERTY(int trackId READ trackId NOTIFY trackIdChanged)
    Q_PROPERTY(bool isLive READ isLive CONSTANT)
public:
    enum SourceType
    {
        Unknown,
        Camera,
        LocalResource,
        RefSource
    };
    Q_ENUM(SourceType)

    enum StreamType
    {
        LiveAuto,
        PlaybackAuto,
        LiveLow,
        LiveHigh,
        PlaybackLow,
        PlaybackHigh,
        BackupLow,
        BackupHigh,
        PosLive,
        PosPlayback,
        PosBackup
    };
    Q_ENUM(StreamType)

    explicit WisenetMediaParam(QObject* parent = nullptr)
        : QObject(parent)
    {

    }
    ~WisenetMediaParam() {}

    struct ItemData
    {
        StreamType m_streamType = StreamType::LiveAuto;
        SourceType m_sourceType = SourceType::Unknown;
        QUrl m_localResource;
        QString m_serviceId;
        QString m_deviceId;
        QString m_channelId;
        QString m_profileId;
        QString m_sessionId;
        QString m_posId;    // 2022.10.12. added

        qint64 m_startTime = 0;
        qint64 m_endTime = 0;
        qreal m_speed = 1.0f;
        int m_trackId = -1;
        bool m_isPosPlayback = false;   // 2022.10.12. added
        bool m_pausedOpen = false;
        bool isLive() const
        { return WisenetMediaParam::isLive(m_streamType);}
        bool isPlayback() const
        { return WisenetMediaParam::isPlayback(m_streamType);}
        bool isBackup() const
        { return WisenetMediaParam::isBackup(m_streamType);}
        bool isAuto() const
        { return WisenetMediaParam::isAuto(m_streamType);}
        bool isPosPlayback() const
        { return WisenetMediaParam::isPosPlayback(m_streamType);}
    };

    StreamType streamType() const {return m_data.m_streamType;}
    SourceType sourceType() const {return m_data.m_sourceType;}
    QUrl localResource() const {return m_data.m_localResource;}
    QString serviceId() const {return m_data.m_serviceId;}
    QString deviceId() const {return m_data.m_deviceId;}
    QString channelId() const {return m_data.m_channelId;}
    QString profileId() const {return m_data.m_profileId;}
    QString sessionId() const {return m_data.m_sessionId;}
    QString posId() const {return m_data.m_posId;}  // 2022.10.12. added
    bool isPosPlayback() const {return m_data.m_isPosPlayback;} // 2022.10.12. added
    qint64 startTime() const {return m_data.m_startTime;}
    qint64 endTime() const {return m_data.m_endTime;}
    qreal speed() const {return m_data.m_speed;}
    int trackId() const {return m_data.m_trackId;}
    bool isPausedOpen() const {return m_data.m_pausedOpen;}
    ItemData data() const {return m_data;}
    bool isLive() const {return m_data.isLive();}

    static bool isLive(const WisenetMediaParam::StreamType streamType)
    {
        return (streamType == WisenetMediaParam::StreamType::LiveAuto ||
                streamType == WisenetMediaParam::StreamType::LiveHigh ||
                streamType == WisenetMediaParam::StreamType::LiveLow);
    }
    static bool isPlayback(const WisenetMediaParam::StreamType streamType)
    {
        return (streamType == WisenetMediaParam::StreamType::PlaybackAuto ||
                streamType == WisenetMediaParam::StreamType::PlaybackHigh ||
                streamType == WisenetMediaParam::StreamType::PlaybackLow);
    }
    static bool isPosPlayback(const WisenetMediaParam::StreamType streamType)
    {
        return (streamType == WisenetMediaParam::StreamType::PosPlayback);
    }
    static bool isBackup(const WisenetMediaParam::StreamType streamType)
    {
        return (streamType == WisenetMediaParam::StreamType::BackupHigh ||
                streamType == WisenetMediaParam::StreamType::BackupLow);
    }
    static bool isAuto(const WisenetMediaParam::StreamType streamType)
    {
        return (streamType == WisenetMediaParam::StreamType::PlaybackAuto ||
                streamType == WisenetMediaParam::StreamType::LiveAuto);
    }

signals:
    void streamTypeChanged();
    void sourceTypeChanged();
    void localResourceChanged();
    void serviceIdChanged();
    void deviceIdChanged();
    void channelIdChanged();
    void profileIdChanged();
    void startTimeChanged();
    void endTimeChanged();
    void trackIdChanged();
    void sessionIdChanged();
    void speedChanged();
    void posIdChanged();
    void posPlaybackChanged();

private:
    void setStreamType(const StreamType& streamType)
    {
        if (m_data.m_streamType != streamType) {
            m_data.m_streamType = streamType;
            emit streamTypeChanged();
        }
    }
    void setSourceType(const SourceType& sourceType)
    {
        if (m_data.m_sourceType != sourceType) {
            m_data.m_sourceType = sourceType;
            emit sourceTypeChanged();
        }
    }
    void setLocalResource(const QUrl& localResource)
    {
        if (m_data.m_localResource != localResource) {
            m_data.m_localResource = localResource;
            emit localResourceChanged();
        }
    }
    void setServiceId(const QString& serviceId)
    {
        if (m_data.m_serviceId != serviceId) {
            m_data.m_serviceId = serviceId;
            emit serviceIdChanged();
        }
    }
    void setDeviceId(const QString& deviceId)
    {
        if (m_data.m_deviceId != deviceId) {
            m_data.m_deviceId = deviceId;
            emit deviceIdChanged();
        }
    }

    void setChannelId(const QString& channelId)
    {
        if (m_data.m_channelId != channelId) {
            m_data.m_channelId = channelId;
            emit channelIdChanged();
        }
    }
    void setProfileId(const QString& profileId)
    {
        if (m_data.m_profileId != profileId) {
            m_data.m_profileId = profileId;
            emit profileIdChanged();
        }
    }
    void setStartTime(const qint64 startTime)
    {
        if (m_data.m_startTime != startTime) {
            m_data.m_startTime = startTime;
            emit startTimeChanged();
        }
    }
    void setEndTime(const qint64 endTime)
    {
        if (m_data.m_endTime != endTime) {
            m_data.m_endTime = endTime;
            emit endTimeChanged();
        }
    }
    void setSpeed(const qreal speed)
    {
        if (m_data.m_speed != speed) {
            m_data.m_speed = speed;
            emit speedChanged();
        }
    }
    void setTrackId(const int trackId)
    {
        if (m_data.m_trackId != trackId) {
            m_data.m_trackId = trackId;
            emit trackIdChanged();
        }
    }
    void setPausedOpen(const bool isPausedOpen)
    {
        m_data.m_pausedOpen = isPausedOpen;
    }
    void setSessionId(const QString& sessionId)
    {
        if (m_data.m_sessionId != sessionId) {
            m_data.m_sessionId = sessionId;
            emit sessionIdChanged();
        }
    }
    void setPosId(const QString& posId)
    {
        if (m_data.m_posId != posId) {
            m_data.m_posId = posId;
            emit posIdChanged();
        }
    }
    void setPosPlayback(const bool isPosPlayback)
    {
        if (m_data.m_isPosPlayback != isPosPlayback) {
            m_data.m_isPosPlayback = isPosPlayback;
            emit posPlaybackChanged();
        }
    }
    bool seekable() const
    {
        return m_data.isPlayback();
    }

    ItemData m_data;
};

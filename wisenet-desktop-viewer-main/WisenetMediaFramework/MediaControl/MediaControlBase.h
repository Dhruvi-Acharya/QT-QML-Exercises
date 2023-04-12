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
#include <QSize>
#include "WisenetMediaParam.h"
#include "QCoreServiceManager.h"
#include "FrameSourceBase.h"

class MediaControlBase : public QObject
{
    Q_OBJECT
public:
    explicit MediaControlBase(QObject *parent = nullptr)
        : QObject(parent)
    {
        m_mediaId = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    virtual ~MediaControlBase()
    {

    }

    enum Error
    {
        NoError,            // 성공
        FormatError,        // 지원하지 않는 파일 포맷
        ResourceError,      // 리소스 접근, 생성 실패
        StreamTypeError,    // 지원하지 않는 스트림 타입
        PermissionError,    // 권한없음
        SessionFullError,   // 사용자 수 초과
        PasswordRequired,   // 패스워드 필요
        ChannelClosedError  // 채널 연결이 끊어져 있음
    };
    enum PlaybackState
    {
        PlayingState,
        PausedState
    };
    enum LastStepDirection
    {
        None,
        Forward,
        Backward
    };

    Q_ENUM(Error)
    Q_ENUM(PlaybackState)

    QString mediaId() const {
        return m_mediaId;
    }

    virtual FrameSourceBase* source() const = 0;
signals:
    void openResult(MediaControlBase::Error error);
    void localRecordingTimeout();
    void mediaIdChanged();

public slots:
    virtual void open() = 0;
    virtual void close() = 0;
    virtual void setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec) = 0;
    virtual void seek(const qint64 playbackTimeMsec) = 0;
    virtual void play(const qint64 lastPlaybackTimeMsec) = 0;
    virtual void pause() = 0;
    virtual void step(const bool isForward, const qint64 lastPlaybackTimeMsec) = 0;
    virtual bool checkDualStream(){return false;}
    virtual bool changeStreamType(const WisenetMediaParam::StreamType streamType)
    {
        Q_UNUSED(streamType);
        return false;
    }
    virtual bool switchingOpen(const WisenetMediaParam* param)
    {
        Q_UNUSED(param);
        return false;
    }
    virtual bool setLocalRecording(const bool recordingStart, QString filePath, QString fileName, int duration = 0)
    {
        Q_UNUSED(recordingStart);
        Q_UNUSED(filePath);
        Q_UNUSED(fileName);
        Q_UNUSED(duration);
        return false;
    }
    virtual void checkPassword(QString password){}
    virtual QMap<qint64, qint64> getTimeline(){return QMap<qint64, qint64>();}

    virtual void updatePlaybackStatus(const bool isPlay, const float speed)
    {Q_UNUSED(isPlay); Q_UNUSED(speed);}

    void setMediaParam(const WisenetMediaParam* param)
    {
        m_mediaParam = param->data();
    }

    void updateOutputParam(const qreal screenPixelRatio,
                           const int width, const int height)
    {
        // 화면분할 사이즈에 대한 high/low 전환은 outputSize 기준,
        // 스케일링 사이즈는 outputSize*screenPixelRatio 기준.
        m_outputSize = QSize(width, height);
        m_screenPixelRatio = screenPixelRatio;
        QSize scaleSize = QSize(width, height);

        if (m_screenPixelRatio > 1.0f) {
            scaleSize = QSize(width*m_screenPixelRatio, height*m_screenPixelRatio);
        }
        if (source() != nullptr) {
            source()->setOutputSize(scaleSize);
        }
    }

    void setKeepOriginalSize(const bool keep)
    {
        if (source() != nullptr) {
            source()->setKeepOriginalSize(keep);
        }
    }
    virtual void sleep(const bool isOn)
    {
        Q_UNUSED(isOn);
    }

protected:
    void setMediaId(const QString mediaId) {
        if (m_mediaId != mediaId) {
            m_mediaId = mediaId;
            emit mediaIdChanged();
        }
    }

protected:
    WisenetMediaParam::ItemData m_mediaParam;
    QString m_mediaId;
    QSize m_outputSize = QSize(1920,1080);
    qreal m_screenPixelRatio = 1.0f;
};

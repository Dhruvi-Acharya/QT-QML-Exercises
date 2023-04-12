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

#include "MediaControlBase.h"
#include "CoreServiceFrameSource.h"
#include "FFmpegStruct.h"
#include "SafeMediaSourceFrameHandler.h"
#include "MediaWriter/FFmpegFileWriter.h"
#include "MediaWriter/WnmFileWriter.h"
#include <QScopedPointer>

class CoreServiceMediaControl : public MediaControlBase
{
public:
    CoreServiceMediaControl(QObject *parent = nullptr, bool isSequence = false);
    virtual ~CoreServiceMediaControl();

    FrameSourceBase* source() const override;
public slots:
    void open() override;
    void close() override;
    void setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec) override;
    void seek(const qint64 playbackTimeMsec) override;
    void play(const qint64 lastPlaybackTimeMsec) override;
    void pause() override;
    void step(const bool isForward, const qint64 lastPlaybackTimeMsec) override;
    void sleep(const bool isOn) override;
    bool changeStreamType(const WisenetMediaParam::StreamType streamType) override;
    bool checkDualStream() override;
    bool switchingOpen(const WisenetMediaParam* param) override;
    bool setLocalRecording(const bool recordingStart, QString filePath, QString fileName, int duration = 0) override; // return localRecordingStatus
    void updatePlaybackStatus(const bool isPlay, const float speed) override;
    /* timer expired */
    void onWriteDurationTimerTimeout();
    void onDisconnectedReopenTimerExpired();
    void onVideoLossReopenTimerExpired();

    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);
    void onVideoLossTriggered();

private:
    void newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame);

    Wisenet::Device::StreamType convertStreamType(const WisenetMediaParam::StreamType streamType);
    QString streamTypeToQString(const Wisenet::Device::StreamType streamType);
    bool isHigh();

    void setMediaInfo(const Wisenet::Device::Device::Channel& channel);
    bool startLocalRecording(QString filePath, QString fileName, int duration = 0);
    bool stopLocalRecording();
    void reopen(const bool resetSpeed = false);

    bool checkFrameCseq(const unsigned short cSeq);
    void resetCseq(const unsigned short cSeq);
    void invalidateCseq();

    /** Timer **/
    void startReopenTimer(const bool isDisconnected);

    void onChannelStatusChanged();
    void onChannelConfigChanged();
    void onMediaDisconnected();

private:
    SourceFrameQueueSharedPtr m_sourceFramesQueue;
    QScopedPointer<CoreServiceFrameSource> m_frameSource;
    SafeMediaHandlerSharedPtr m_safeHandler;
    Wisenet::Device::MediaControlInfo m_lastControlInfo;
    std::atomic<int64_t> m_lastVideoFrameTimeMsec;
    Wisenet::Device::StreamType m_lastStreamType;
    Wisenet::Device::ChannelStatus m_lastChannelStatus;
    std::atomic<bool> m_isClosed;
    std::atomic<bool> m_isSleep;

    QScopedPointer<FileWriterBase> m_writer;
    QScopedPointer<QTimer>  m_writeDurationTimer;
    QScopedPointer<QTimer>  m_mediaReopenTimer;
    bool m_writeStarted = false;

    unsigned char   m_lastCommandSeq = 0;
    unsigned char   m_compareCommandSeq = 0;

    std::string m_stdDeviceId;
    std::string m_stdChannelId;
    std::string m_stdMediaId;
    std::string m_lastLiveHighProfile;
    std::string m_lastLiveLowProfile;
    int m_mediaReconnectPeriodMsec = 10000;

    long long m_mediaRequestTime = 0;
    bool m_writeResponseTime = false;
};


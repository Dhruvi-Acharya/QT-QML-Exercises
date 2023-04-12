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
#include "MediaRunnable.h"
#include "FFmpegStruct.h"
#include "MediaControlBase.h"

class FFmpegReader : public QObject, public MediaRunnable
{
    Q_OBJECT
public:
    explicit FFmpegReader(QObject* parent=nullptr);
    ~FFmpegReader();
    SourceFrameQueueSharedPtr getMediaSourceQueue();
    void open(QString& localResource);
    void close();

    void setSpeed(const float speed, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq);
    void seek(const qint64 playbackTimeMsec, const unsigned char commandSeq);
    void play(const qint64 lastPlaybackTimeMsec, const float lastSpeed, const unsigned char commandSeq);
    void pause(const unsigned char commandSeq);
    void step(const bool isForward, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq);
    void sleep(const bool isOn);

    qint64 duration() const;
    std::string localResource() const;

    static bool isAudioPlay(const float speed);
    static bool needToResetPlay(const float lastSpeed, const float newSpeed);
    static bool isFullFramePlay(const float speed);

protected:
    void loopTask() override;

private:
    void _open();
    void _release();
    void _checkCommands();
    void _pushVideo(AVPacket* packet);
    void _pushAudio(AVPacket* packet);
    void _resetPlay();
    bool _seek(const qint64 timeStampMsec, const bool isBackward = false);

signals:
    void openResult(MediaControlBase::Error error);

private:
    SourceFrameQueueSharedPtr m_mediaSourceFramesQueue;

    std::string         m_localResource;

    std::mutex          m_mutex;
    AVPacket*           m_packet = nullptr;
    AVPacket*           m_packetFiltered = nullptr;
    AVFormatContext*    m_fmtContext = nullptr;
    AVStream*           m_videoStream = nullptr;
    AVStream*           m_audioStream = nullptr;
    int                 m_videoStreamIndex = -1;
    int                 m_audioStreamIndex = -1;

    float               m_avgFps = 0.f;
    int                 m_width = 0;
    int                 m_height = 0;
    qint64              m_durationMsec = 0;

    unsigned int        m_videoFrameSequence = 0;
    unsigned int        m_audioFrameSequence = 0;
    bool                m_isMp4MPEG4FirstTime = false;
    bool                m_loopPlay = true;
    bool                m_isFirstVideo = true;
    qint64              m_firstVideoTimeStamp = 0;
    bool                m_isPaused = false;
    bool                m_isSleep = false;

    // command control
    bool                m_isOpenCommand = false;
    bool                m_isCloseCommand = false;
    bool                m_isSetSpeedCommand = false;
    bool                m_isSeekCommand = false;
    bool                m_isPlayCommand = false;
    bool                m_isPauseCommand = false;
    bool                m_isStepCommand = false;
    bool                m_isSleepCommand = false;

    unsigned char   m_newCommandSeq = 0;
    float   m_newSpeed = 1.0f;
    qint64  m_newSeekTimeMsec = 0;
    bool    m_newStepForwardStatus = false;
    bool    m_newSleepStatus = false;
    unsigned char   m_lastCommandSeq = 0;
    float   m_lastSpeed = 1.0f;
    qint64  m_lastVideoTimeStamp = 0;
    qint64  m_lastSleepVideoTimeStamp = 0;
};


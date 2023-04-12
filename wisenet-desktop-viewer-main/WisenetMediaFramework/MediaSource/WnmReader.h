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
#include "WnmStruct.h"
#include "MediaControlBase.h"
#include "CryptoAdapter.h"
#include "DigitalSignature.h"

class WnmReader : public QObject, public MediaRunnable
{
    Q_OBJECT
public:
    explicit WnmReader(QObject* parent=nullptr);
    ~WnmReader();
    SourceFrameQueueSharedPtr getSourceFrameQueue();
    void open(QString& localResource);    
    void close();

    void setSpeed(const float speed, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq);
    void seek(const qint64 playbackTimeMsec, const unsigned char commandSeq);
    void play(const qint64 lastPlaybackTimeMsec, const float lastSpeed, const unsigned char commandSeq);
    void pause(const unsigned char commandSeq);
    void step(const bool isForward, const qint64 lastPlaybackTimeMsec, const unsigned char commandSeq);
    void sleep(const bool isOn);

    void checkPassword(QString& password);
    QMap<qint64, qint64> getTimeline();
    QString getChannelName();

    static bool isAudioPlay(const float speed);
    static bool needToResetPlay(const float lastSpeed, const float newSpeed);
    static bool isFullFramePlay(const float speed);

    bool verifyDigitalSignature();

protected:
    void loopTask() override;

private:
    void _open();
    void release();
    void _checkPassword();
    void _resetPlay();
    void _seek(const qint64 timeStampMsec);
    void checkCommands();
    void pushVideo();
    void pushAudio();
    bool readNextFrame();

    void loadInfos();
    void loadIFrameIndex();
    void loadTimelines();
    void loadEncryption();
    void loadDigitalSigning();

signals:
    void openResult(MediaControlBase::Error error);

private:
    SourceFrameQueueSharedPtr m_sourceFramesQueue;

    QString                 m_localResource;
    std::mutex              m_mutex;

    QFile                   m_file;
    QDataStream             m_dataStream;

    qint64                  m_nextFrameOffset = 0;  // 다음 읽을 frame의 offset
    WnmFramePacket          m_packet;               // 마지막 읽은 Frame packet
    qint64                  m_lastVideoPts = 0;     // 이전 VideoFrame의 PTS
    qint64                  m_lastSleepVideoPts = 0;

    // Frames를 제외한, 파일에서 로드한 데이터들
    WnmInitalizeParameter   m_wnmParameter;
    int                     m_encryptionDataSize = 0;
    char *                  m_encryptionDataPtr = nullptr;  // 패스워드로 암호화 된 데이터
    int                     m_signingDataSize = 0;
    char *                  m_signingDataPtr = nullptr;     // 디지털 서명 데이터
    int                     m_certificateDataSize = 0;
    char *                  m_certificateDataPtr = nullptr; // 디지털 인증서 데이터
    QList<IFrameIndex>      m_iFrameIndexList;
    QList<Timeline>         m_timelineList;
    QMap<WnmElementType, WnmOffsetEntry> m_fileOffsetIndex;

    CryptoAdapter                 m_crypto;   // 패스워드 복호화용
    QString                 m_tempPassword = ""; // 패스워드 유효성 체크 전 임시 저장

    float                   m_avgFps = 0.f;
    int                     m_width = 0;
    int                     m_height = 0;
    qint64                  m_lastIFrameTimestamp = 0;
    qint64                  m_govMsec = 0;

    unsigned int            m_videoFrameSequence = 0;
    unsigned int            m_audioFrameSequence = 0;

    // command control
    std::atomic<bool>       m_isOpenCommand;
    std::atomic<bool>       m_isCloseCommand;
    std::atomic<bool>       m_isSetSpeedCommand;
    std::atomic<bool>       m_isSeekCommand;
    std::atomic<bool>       m_isPlayCommand;
    std::atomic<bool>       m_isPauseCommand;
    std::atomic<bool>       m_isStepCommand;
    std::atomic<bool>       m_isSleepCommand;
    std::atomic<bool>       m_isCheckPasswordCommand;

    unsigned char   m_newCommandSeq = 0;
    bool    m_isPaused = false;
    bool    m_isSleep = false;
    float   m_newSpeed = 1.0f;
    qint64  m_newSeekTimeMsec = 0;
    bool    m_newStepForwardStatus = false;
    bool    m_newSleepStatus = false;
    unsigned char   m_lastCommandSeq = 0;
    float   m_lastSpeed = 1.0f;

};


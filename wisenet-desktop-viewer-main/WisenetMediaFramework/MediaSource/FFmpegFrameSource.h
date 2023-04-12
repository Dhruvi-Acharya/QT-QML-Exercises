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
#include "FrameSourceBase.h"
#include "MediaRunnable.h"
#include "BaseDecoder.h"
#include "AudioDecoder.h"
#include <QPointer>
#include <QScopedPointer>
#include <QTimer>
#include <chrono>


class FFmpegFrameSource : public FrameSourceBase, public MediaRunnable
{
    Q_OBJECT
public:
    explicit FFmpegFrameSource(SourceFrameQueueSharedPtr mediaSourceFramesQueue,
                               bool isSequence = false,
                               QObject *parent = nullptr);

    void setAttributes(const QString& name);
    ~FFmpegFrameSource();

    void setPlaySpeed(const float speed, const unsigned char commandSeq);
    void seek(const qint64 playbackTimeMsec, const unsigned char commandSeq);
    void play(const float lastSpeed, const unsigned char commandSeq);
    void pause(const unsigned char commandSeq);
    void step(const bool isForward, const unsigned char commandSeq);
    void sleep(const bool isOn);
    void setOutputSize(const QSize size) override;
    void setKeepOriginalSize(const bool keep) override;
    void requestLastVideoFrame() override;
    void requestLastOriginalVideoImage() override;
    void setOutPixelFormat(const VideoOutPixelFormat pixelFormat) override;
    void setMaximumVideoSize(const int size) override;

protected:
    void loopTask() override;

private:
    void checkVideo(const std::chrono::steady_clock::time_point& now);
    void checkAudio(const std::chrono::steady_clock::time_point& now);
    void checkMeta(const std::chrono::steady_clock::time_point& now);
    void wakeupTask();
    void checkVideoStatus(const Wisenet::Media::VideoSourceFrameSharedPtr& videoFrame,
                          const VideoDecodeMethod decodeMethod);
    void checkAudioStatus(const Wisenet::Media::AudioSourceFrameSharedPtr& audioFrame);
    void checkFrameStat(const std::chrono::steady_clock::time_point& now);
    void resetFrameStat();

    void resetPlay();
    bool calcPlayTime(const qint64 currentTimeStamp,
                      const std::chrono::steady_clock::time_point& now);
    void checkCommands();
    void getLastOriginalVideoImage();

private:
    SourceFrameQueueSharedPtr m_mediaSourceFramesQueue;
    QList<Wisenet::Media::MediaSourceFrameBaseSharedPtr> m_lastVideoFramesList;   // 마지막 I-Frame부터의 원본 Video Frame을 유지하는 List
    QScopedPointer<BaseVideoDecoder> m_videoDecoder;
    QScopedPointer<AudioDecoder> m_audioDecoder;
    QScopedPointer<BaseVideoDecoder> m_captureDecoder;  // 원본 이미지 캡쳐용 Video Decoder

    int64_t m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
    std::chrono::steady_clock::time_point m_lastVideoClock;
    long long m_lastVideoClockGap = 0;

    MediaRunnableTaskPtr m_taskCondition;
    std::chrono::steady_clock::time_point m_lastNotifyClock;
    std::chrono::steady_clock::time_point m_lastTaskClock;

    QTimer *m_wakeTimer = nullptr;

    /* 1초 동안의 framerate 및 bitrate 계산 */
    Wisenet::Media::VideoCodecType m_videoCodec = Wisenet::Media::VideoCodecType::UNKNOWN;
    Wisenet::Media::AudioCodecType m_audioCodec = Wisenet::Media::AudioCodecType::UNKNOWN;
    VideoDecodeMethod m_videoDecodeMethod = VideoDecodeMethod::SW;
    std::chrono::steady_clock::time_point m_checkTimePoint;
    size_t m_frameCheckCount = 0;
    size_t m_frameCheckBytes = 0;

    std::mutex          m_mutex;
    bool    m_isSleep = false;
    float   m_newSpeed = 1.0f;
    float   m_lastSpeed = 1.0f;
    unsigned char   m_newCommandSeq = 0;
    unsigned char   m_lastCommandSeq = 0;
    bool    m_newStepForwardStatus = false;
    bool    m_newSleepStatus = false;
    VideoOutPixelFormat m_newOutPixelFormat = VideoOutPixelFormat::Unknown;
    int     m_newMaxinumVideoSize = 0;

    bool    m_isSetSpeedCommand = false;
    bool    m_isSeekCommand = false;
    bool    m_isPlayCommand = false;
    bool    m_isPauseCommand = false;
    bool    m_isStepCommand = false;
    bool    m_doForwardStep = false;
    bool    m_isSleepCommand = false;
    // 마지막 영상 요청 명령
    bool    m_isRequestLastVideoCommand = false;
    bool    m_isRequestLastOriginalVideoCommand = false;
    bool    m_isSetOutPixelFormatCommand = false;
    bool    m_isSetMaximumVideoSizeCommand = false;

    bool    m_lastEnableAudioOut = false;
    QSize   m_outputSize;
    QSize   m_lastOutputSize;
    bool    m_keepOriginalSize = false;
    bool    m_lastKeepOriginalSize = false;
    std::chrono::steady_clock::time_point m_lastDecodeErrorClock;
};


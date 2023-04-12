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
#include "QCoreServiceManager.h"
#include "MediaRunnable.h"
#include "BaseDecoder.h"
#include "AudioDecoder.h"
#include "MediaRunnable.h"
#include "Runnable.h"
#include <QScopedPointer>
#include "AudioInputManager.h"
#include <chrono>

struct _VideoOutBuffer;

class CoreServiceFrameSource : public FrameSourceBase, public MediaRunnable, public std::enable_shared_from_this<CoreServiceFrameSource>
{
    Q_OBJECT
public:
    struct VideoOutDoubleBuffer {
        bool isNew = false;
        VideoOutputFrameSharedPtr data = nullptr;
        bool valid() {
            return (isNew && data != nullptr);
        }
        void set(VideoOutputFrameSharedPtr& newData) {
            data = newData;
            isNew = true;
        }

        void reset() {
            isNew = false;
            data = nullptr;
        }
    };

    explicit CoreServiceFrameSource(SourceFrameQueueSharedPtr sourceFramesQueue,
                                    bool isSequence = false,
                                    QObject *parent = nullptr);
    ~CoreServiceFrameSource();

    // todo:: 코어서비스 이벤트를 받았을 때 갱신하는 부분 추가 필요
    void updateChannel(const Wisenet::Device::Device::Channel& channel);
    void updateUserPermission(const Wisenet::Core::UserGroup::Permission& permission);
    void setup(const Wisenet::Device::StreamType streamType);

    void devicePtzContinuous(const int pan, const int tilt, const int zoom) override;
    void devicePtzStop() override;
    void devicePtzPresetGet() override;
    void devicePtzPresetMove(const int presetNumber) override;
    void devicePtzPresetDelete(const int presetNumber) override;
    void devicePtzPresetAdd(const int presetNumber, const QString& presetName) override;
    // swing
    void devicePtzSwingGet() override;
    void devicePtzSwingMoveStop(const Wisenet::Device::SwingActionType swingActionType) override;
    // group
    void devicePtzGroupGet() override;
    void devicePtzGroupMove(const int groupActionType) override;
    void devicePtzGroupStop(const int groupActionType) override;
    // tour
    void devicePtzTourGet() override;
    void devicePtzTourMove() override;
    void devicePtzTourStop() override;
    // trace
    void devicePtzTraceGet() override;
    void devicePtzTraceMove(const int traceNumber) override;
    void devicePtzTraceStop(const int traceNumber) override;
    void deviceFocusContinuous(const int command) override;
    void deviceFocusModeControl(const int focusMode) override;
    void setOutputSize(const QSize size) override;
    void setKeepOriginalSize(const bool keep) override;
    void requestLastVideoFrame() override;
    void requestLastOriginalVideoImage() override;
    void deviceTalk(const bool on) override;

    void updatePlaybackStatus(const bool isPlay, const float speed, const bool playWait) override;
    void open();
    void close();
    void seek();
    void play(const float lastSpeed);
    void pause();
    void setPlaySpeed(const float speed);
    void step(const bool isForward);
    void sleep(const bool isOn);

    bool deviceUsingSetup() const;
signals:
    void videoLossTriggered();

protected:
    void loopTask() override;

private:
    void _open(const std::chrono::steady_clock::time_point& now);
    void _close();
    void checkCommands(const std::chrono::steady_clock::time_point& now);
    void checkVideoStatus(const Wisenet::Media::VideoSourceFrameSharedPtr& videoFrame,
                          const std::chrono::steady_clock::time_point &now,
                          const VideoDecodeMethod decodeMethod);
    void checkAudioStatus(const Wisenet::Media::AudioSourceFrameSharedPtr& audioFrame);
    void checkFrameStat(const std::chrono::steady_clock::time_point& now);
    void resetFrameStat();
    void resetPlay(const int newStatus, const std::chrono::steady_clock::time_point &now);
    void checkVideo(const std::chrono::steady_clock::time_point& now,
                    Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr);
    void checkAudio(const std::chrono::steady_clock::time_point& now,
                    Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr);
    void checkMeta(const std::chrono::steady_clock::time_point& now,
                    Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr);
    bool calcPlayTime(const int64_t currentTimeStamp,
                      const size_t queueSize);
    void checkVideoLoss(const std::chrono::steady_clock::time_point &now);
    void getLastOriginalVideoImage();

private:
    bool m_ptzRunning = false; /* todo */
    Wisenet::Device::Device::Channel m_channel;
    SourceFrameQueueSharedPtr m_sourceFramesQueue;
    QList<Wisenet::Media::MediaSourceFrameBaseSharedPtr> m_lastVideoFramesList;   // 마지막 I-Frame부터의 원본 Video Frame을 유지하는 List
    QScopedPointer<BaseVideoDecoder> m_videoDecoder;
    QScopedPointer<AudioDecoder> m_audioDecoder;
    QScopedPointer<BaseVideoDecoder> m_captureDecoder;  // 원본 이미지 캡쳐용 Video Decoder

    /* 1초 동안의 framerate 및 bitrate 계산 */
    Wisenet::Media::VideoCodecType m_videoCodec = Wisenet::Media::VideoCodecType::UNKNOWN;
    Wisenet::Media::AudioCodecType m_audioCodec = Wisenet::Media::AudioCodecType::UNKNOWN;
    VideoDecodeMethod m_videoDecodeMethod = VideoDecodeMethod::SW;
    std::chrono::steady_clock::time_point m_checkTimePoint;
    size_t m_frameCheckCount = 0;
    size_t m_frameCheckBytes = 0;

    std::atomic<bool> m_isClosed;
    std::atomic<Wisenet::Device::StreamType> m_deviceStreamType;
    bool    m_isLive = true;

    std::mutex  m_mutex;
    bool    m_isCloseCommand = false;
    bool    m_isOpenCommand = false;
    bool    m_isSetSpeedCommand = false;
    bool    m_isSeekCommand = false;
    bool    m_isPlayCommand = false;
    bool    m_isPauseCommand = false;
    bool    m_isStepCommand = false;
    bool    m_isSleepCommand = false;
    bool    m_isUpdatePlaybackStatusCommand = false;
    // 마지막 영상 요청 명령
    bool    m_isRequestLastVideoCommand = false;
    bool    m_isRequestLastOriginalVideoCommand = false;
    float   m_newSpeed = 1.0f;
    float   m_lastSpeed = 1.0f;
    bool    m_newSleepStatus = false;
    bool    m_isSleep = false;
    bool    m_lastEnableAudioOut = false;
    QSize   m_outputSize;
    QSize   m_lastOutputSize;
    bool    m_keepOriginalSize = false;
    bool    m_lastKeepOriginalSize = false;
    bool    m_newPlayWait = false;

    int64_t m_liveBufferingUsec = 0;
    int64_t m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;

    std::chrono::steady_clock::time_point m_lastLoopPoint;
    int64_t m_lastTickGap = 0;

    std::chrono::steady_clock::time_point m_lastVideoClock;
    long long m_lastVideoClockGap = 0;
    std::chrono::steady_clock::time_point m_lastDecodeErrorClock;

    // 라이브 영상 균등성을 위한 video out 버퍼
    bool    m_enableVideoOutDoubleBuffer = false;
    VideoOutputFrameSharedPtr m_VideoOutBuffer = nullptr;

    // 비디오데이터를 받은 마지막 시점 저장
     std::chrono::steady_clock::time_point m_lastVideoFrameClock;
     const int m_openTimeoutMsec = 5000;
     int m_playTimeoutMsec = 3000;
     int m_videoWaitMsec = 3000;

     // 뷰어 사용자 권한
     Wisenet::Core::UserGroup::Permission m_userPermission;
};


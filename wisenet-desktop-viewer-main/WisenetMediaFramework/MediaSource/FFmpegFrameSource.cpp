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
#include "FFmpegFrameSource.h"

#include "LogSettings.h"

#include "AudioDecoder.h"
#include "VideoDecoder.h"
#include <cstdlib>

// 우분투도 잘동작해서 일단 막아놓겠음
#ifdef Q_OS_LINUX1 /* Q_OS_LINUX */
#define USE_QTIMER_NOTIFY
#endif

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFmpegFrameSource] "}, level, __VA_ARGS__)

#define HW_DECODER_TEST 1

FFmpegFrameSource::FFmpegFrameSource(SourceFrameQueueSharedPtr mediaSourceFramesQueue,
                                     bool isSequence,
                                     QObject *parent)
    : FrameSourceBase(parent)
    , m_mediaSourceFramesQueue(mediaSourceFramesQueue)
    , m_videoDecoder(new VideoDecoder(nullptr, false, isSequence))
    , m_audioDecoder(new AudioDecoder())
    , m_captureDecoder(new VideoDecoder(nullptr, true, true))
{
    SPDLOG_DEBUG("FFmpegFrameSource() THREAD START");
    m_checkTimePoint = std::chrono::steady_clock::now();

#ifdef USE_QTIMER_NOTIFY
    m_taskCondition = std::make_shared<MediaRunnableTask>();
    m_wakeTimer = new QTimer(this);
    m_wakeTimer->setInterval(1);
    m_wakeTimer->setTimerType(Qt::TimerType::PreciseTimer);
    connect(m_wakeTimer, &QTimer::timeout, this, [=]() {
        wakeupTask();
    });
    m_wakeTimer->start();
#endif
    m_lastNotifyClock = std::chrono::steady_clock::now();
    m_lastTaskClock = m_lastNotifyClock;
}

void FFmpegFrameSource::setAttributes(const QString &name)
{
    setMediaName(name);
    m_videoDecoder->setMediaName(name);
    setSupportPlayback(true);
}

FFmpegFrameSource::~FFmpegFrameSource()
{
    stop();
    m_lastVideoFramesList.clear();
    SPDLOG_DEBUG("~FFmpegFrameSource() THREAD END");
}

void FFmpegFrameSource::wakeupTask()
{
#if 0
    auto now = std::chrono::steady_clock::now();
    auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastNotifyClock).count();
    if (elapsedMsec >= 5)
        SPDLOG_DEBUG("[T] TASK NOTIFY, gap={}", elapsedMsec);
    m_lastNotifyClock = now;
#endif

    if (m_taskCondition)
        m_taskCondition->Notify();
}

void FFmpegFrameSource::checkVideoStatus(const Wisenet::Media::VideoSourceFrameSharedPtr &videoFrame,
                                         const VideoDecodeMethod decodeMethod)
{
    if (videoFrame->videoWidth != m_videoSize.width() || videoFrame->videoHeight != m_videoSize.height()) {
        SPDLOG_DEBUG("Resolution changed, {}x{}", videoFrame->videoWidth, videoFrame->videoHeight);
        setVideoSize(videoFrame->videoWidth, videoFrame->videoHeight);
    }
    if (videoFrame->videoCodecType != m_videoCodec || m_videoDecodeMethod != decodeMethod) {
        m_videoCodec = videoFrame->videoCodecType;
        m_videoDecodeMethod = decodeMethod;
        setVideoCodec(QString::fromStdString(makeVideoCodecDescription(m_videoCodec, decodeMethod)));
    }
    m_frameCheckCount++;
    m_frameCheckBytes += videoFrame->getDataSize();
}

void FFmpegFrameSource::checkAudioStatus(const Wisenet::Media::AudioSourceFrameSharedPtr &audioFrame)
{
    if (audioFrame->audioCodecType != m_audioCodec) {
        m_audioCodec = audioFrame->audioCodecType;
        QString codecName = QString::fromStdString(Wisenet::Media::AudioCodecTypeString(m_audioCodec));
        setAudioCodec(codecName);
    }
}


void FFmpegFrameSource::checkFrameStat(const std::chrono::steady_clock::time_point &now)
{
    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_checkTimePoint).count();
    if (elapsedMsec >= 1000) {
        // calculate frame rate and bit rate
        float tick = (float)(elapsedMsec)/1000;
        float fps = (float)m_frameCheckCount/tick;
        float kbps = (float)m_frameCheckBytes/tick*8/1000;

        STATLOG_DEBUG("VideoFile Frame Stat::name={}, tick={:.2f}, fps={:.2f}, kbps={:.2f}, frames={}, bytes={}",
                     m_mediaStdName, tick, fps, kbps, m_frameCheckCount, m_frameCheckBytes);

        setVideoFps(fps);
        setVideoKbps(kbps);

        m_frameCheckCount = 0;
        m_frameCheckBytes = 0;
        m_checkTimePoint = now;
    }
}

void FFmpegFrameSource::resetFrameStat()
{
    m_frameCheckCount = 0;
    m_frameCheckBytes = 0;
    m_checkTimePoint = std::chrono::steady_clock::now();
}

bool FFmpegFrameSource::calcPlayTime(const qint64 currentTimeStamp,
                                     const std::chrono::steady_clock::time_point& now)
{
    //SPDLOG_DEBUG("calcPlayTime(), currentTimeStamp={}, lastTimeStamp={}", currentTimeStamp, m_lastVideoPtsTimestampMsec);

    // reset timestamp
    if (m_lastVideoPtsTimestampMsec == AV_NOPTS_VALUE) {
        SPDLOG_DEBUG("calcPlayTime(), reset timestamp value, return true, name={}", m_mediaStdName);
        m_lastVideoClockGap = 0;
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        return true;
    }
    // overflow timestamp
    if ( (m_lastSpeed > 0 && m_lastVideoPtsTimestampMsec > currentTimeStamp) ||
         (m_lastSpeed < 0 && m_lastVideoPtsTimestampMsec < currentTimeStamp)) {
        m_lastVideoClockGap = 0;
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        return true;
    }

    if (m_lastSpeed == 0.0f) {
        return false;
    }

#if 0 // use millisecond (재생 성능 조금 떨어지는 듯..소리가 끊김)
    auto absTimeStampGap = std::abs(currentTimeStamp-m_lastVideoPtsTimestampMsec);

    auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastVideoClock).count();
    auto calcElapsedMsec = elapsedMsec*std::fabs(m_lastSpeed) + m_lastVideoClockGap;
    if (calcElapsedMsec >= absTimeStampGap) {
        SPDLOG_DEBUG("VIDEO PLAY TIME GOGO, calcElapsedMsec={}, absTimeStampGap={}", calcElapsedMsec, absTimeStampGap);
        m_lastVideoClockGap = calcElapsedMsec - absTimeStampGap;
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        return true;
    }
#else
    auto absTimeStampGap = std::abs(currentTimeStamp-m_lastVideoPtsTimestampMsec) * 1000;
    auto elapsedMicrosec = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastVideoClock).count();
    auto calcElapsedMicrosec = elapsedMicrosec*std::fabs(m_lastSpeed) + m_lastVideoClockGap;
    if (calcElapsedMicrosec >= absTimeStampGap) {
        //SPDLOG_DEBUG("VIDEO PLAY TIME GOGO, calcElapsedMsec={}, absTimeStampGap={}", calcElapsedMicrosec, absTimeStampGap);
        m_lastVideoClockGap = calcElapsedMicrosec - absTimeStampGap;
        if (m_lastVideoClockGap > absTimeStampGap) {
            m_lastVideoClockGap = absTimeStampGap;
        }
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        return true;
    }
#endif

    return false;
}


void FFmpegFrameSource::setPlaySpeed(const float speed, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetSpeedCommand = true;
    m_newSpeed = speed;
    m_newCommandSeq = commandSeq;
    setSpeed(speed);
}

void FFmpegFrameSource::seek(const qint64 playbackTimeMsec, const unsigned char commandSeq)
{
    Q_UNUSED(playbackTimeMsec);
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSeekCommand = true;
    m_newCommandSeq = commandSeq;
}

void FFmpegFrameSource::play(const float lastSpeed, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlayCommand = true;
    m_newSpeed = lastSpeed;
    m_newCommandSeq = commandSeq;
    setSpeed(lastSpeed);
}

void FFmpegFrameSource::pause(const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPauseCommand = true;
    m_newCommandSeq = commandSeq;
}

void FFmpegFrameSource::step(const bool isForward, const unsigned char commandSeq)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStepCommand = true;
    m_newCommandSeq = commandSeq;
    m_newStepForwardStatus = isForward;
}

void FFmpegFrameSource::sleep(const bool isOn)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

void FFmpegFrameSource::setOutputSize(const QSize size)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    //SPDLOG_DEBUG("setOutputSize, WIDTH={}, HEIGHT={}", size.width(), size.height());
    if (m_outputSize != size) {
        m_outputSize = size;
    }
}

void FFmpegFrameSource::setKeepOriginalSize(const bool keep)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_keepOriginalSize != keep) {
        m_keepOriginalSize = keep;
    }
}

void FFmpegFrameSource::requestLastVideoFrame()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRequestLastVideoCommand = true;
}

void FFmpegFrameSource::requestLastOriginalVideoImage()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRequestLastOriginalVideoCommand = true;
}

void FFmpegFrameSource::setOutPixelFormat(const VideoOutPixelFormat pixelFormat)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetOutPixelFormatCommand = true;
    m_newOutPixelFormat = pixelFormat;
}

void FFmpegFrameSource::setMaximumVideoSize(const int size)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetMaximumVideoSizeCommand = true;
    m_newMaxinumVideoSize = size;
}

/**
 * @brief FFmpegFrameSource::checkCommands
 * 쓰레드로 들어온 재생 명령 처리
 */
void FFmpegFrameSource::checkCommands()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_lastEnableAudioOut != m_enableAudioOut) {
        SPDLOG_DEBUG("Audio Enable Status Changed, name={}, before={}, now={}",
                     m_mediaStdName, m_lastEnableAudioOut, m_enableAudioOut);
        m_lastEnableAudioOut = m_enableAudioOut;
    }
    if (m_outputSize != m_lastOutputSize) {
        SPDLOG_DEBUG("Video Output Status Changed, name={}, before={}x{}, now={}x{}",
                     m_mediaStdName,
                     m_lastOutputSize.width(), m_lastOutputSize.height(),
                     m_outputSize.width(), m_outputSize.height());
        m_lastOutputSize = m_outputSize;
    }
    if (m_keepOriginalSize != m_lastKeepOriginalSize) {
        m_lastKeepOriginalSize = m_keepOriginalSize;
    }

    if(m_isSetOutPixelFormatCommand) {
        m_isSetOutPixelFormatCommand = false;
        m_videoDecoder->setPixelFormat(m_newOutPixelFormat);
    }

    if(m_isSetMaximumVideoSizeCommand) {
        m_isSetMaximumVideoSizeCommand = false;
        m_videoDecoder->setMaximumVideoOutSize(m_newMaxinumVideoSize);
    }

    // step forward, pause, full/intra play가 바뀌지 않는 speed 변경은 cSeq를 증가하지 않는다.
    if (m_newCommandSeq != m_lastCommandSeq) {
        m_lastCommandSeq = m_newCommandSeq;
        resetPlay();
        m_doForwardStep = false;
    }

    // 재생 타임스탬프 및 디코더 버퍼 초기화
    if (m_isSetSpeedCommand) {
        m_isSetSpeedCommand = false;
        m_lastSpeed = m_newSpeed;
    }
    if (m_isSeekCommand) {
        m_isSeekCommand = false;
    }
    if (m_isPauseCommand) {
        m_isPauseCommand = false;
        m_lastSpeed = 0.0f;
    }
    if (m_isPlayCommand) {
        m_isPlayCommand = false;
        m_lastSpeed = m_newSpeed;
        m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
    }
    if (m_isStepCommand) {
        m_isStepCommand = false;
        m_doForwardStep = m_newStepForwardStatus;
        m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
    }
    if (m_isSleepCommand) {
        m_isSleepCommand = false;
        if (m_isSleep != m_newSleepStatus) {
            m_isSleep = m_newSleepStatus;
            m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
            resetFrameStat();
            m_lastSpeed = 1.0f; // sleep->wake시에는 1배속으로 고정한다.
        }
    }
    // 마지막 영상 요청
    if (m_isRequestLastVideoCommand) {
        m_isRequestLastVideoCommand = false;
        SPDLOG_DEBUG("Last Video Frame requested!");
        onLastVideoFrame();
    }
    if (m_isRequestLastOriginalVideoCommand) {
        m_isRequestLastOriginalVideoCommand = false;
        getLastOriginalVideoImage();
    }
}

void FFmpegFrameSource::resetPlay()
{
    SPDLOG_DEBUG("resetPlay(), name={}", m_mediaStdName);
    m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
    if (m_videoDecoder)
        m_videoDecoder->flushBuffers();
    if (m_audioDecoder)
        m_audioDecoder->flushBuffers();
    if (m_captureDecoder)
        m_captureDecoder->flushBuffers();
}

/**
 * @brief FFmpegFrameSource::loopTask
 * Thread Main Loop
 */
void FFmpegFrameSource::loopTask()
{
#ifdef USE_QTIMER_NOTIFY
    const int waitMsec = 8;
    m_taskCondition->Wait(waitMsec);
#else
    const int waitMsec = m_isSleep ? 16 : 1;
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMsec));
#endif

    auto now = std::chrono::steady_clock::now();
#if 0
    auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastTaskClock).count();
    if (elapsedMsec >= 5)
        SPDLOG_DEBUG("[T] TASK LOOP TASK, gap={}", elapsedMsec);
    m_lastTaskClock = now;
#endif
    checkCommands();

    // do nothing when sleep
    if (m_isSleep) {
        //SPDLOG_DEBUG("sleep mode On!!");
        return;
    }

    checkFrameStat(now);
    checkVideo(now);
    checkAudio(now);
    checkMeta(now);
}

void FFmpegFrameSource::checkVideo(const std::chrono::steady_clock::time_point& now)
{
    Wisenet::Media::MediaSourceFrameBaseSharedPtr dataPtr = nullptr;

    if (!m_mediaSourceFramesQueue->front(dataPtr)) {
        return;
    }
    if (dataPtr->getMediaType() != Wisenet::Media::MediaType::VCODEC_PARAM &&
        dataPtr->getMediaType() != Wisenet::Media::MediaType::VIDEO) {
        return;
    }

    // codec param인 경우 디코더 파라미터 설정.
    if (dataPtr->getMediaType() == Wisenet::Media::MediaType::VCODEC_PARAM) {
        m_mediaSourceFramesQueue->pop();
        auto vcodecParam = std::static_pointer_cast<FFmpegCodecParam>(dataPtr);
        m_videoDecoder->setVideoOutSize(m_lastOutputSize.width(),
                                        m_lastOutputSize.height(),
                                        m_lastKeepOriginalSize);
        m_videoDecoder->openWithFFmpegCodecParam(vcodecParam);
        m_captureDecoder->setVideoOutSize(m_lastOutputSize.width(),
                                        m_lastOutputSize.height(),
                                        true);
        m_captureDecoder->openWithFFmpegCodecParam(vcodecParam);
        return;
    }

    // sequence 번호가 맞지 않는 프레임은 뽑아내서 버린다.
    if (dataPtr->commandSeq != m_lastCommandSeq) {
        // reset 된 경우에는 이전 프레임들을 버린다.
        if (m_lastVideoPtsTimestampMsec == AV_NOPTS_VALUE) {
            m_mediaSourceFramesQueue->pop();
        }
        // reset되지 않은 경우에는 다음에 reset후에 읽는다.
        return;
    }

    // 비디오인 경우에는 timestamp값을 확인하여 재생속도에 맞춰서 꺼낸다.
    auto videoFramePtr = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(dataPtr);
    auto currentPtsTimeStampMsec = videoFramePtr->dtsTimestampMsec != -1 ?
                videoFramePtr->dtsTimestampMsec :
                videoFramePtr->frameTime.ptsTimestampMsec;

    bool isKeyFrame = (videoFramePtr->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME);

    if(videoFramePtr->immediate)
        m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;

    if (calcPlayTime(currentPtsTimeStampMsec, now)) {
        m_mediaSourceFramesQueue->pop();
        checkVideoStatus(videoFramePtr, m_videoDecoder->getMethod());
        VideoOutputFrameSharedPtr outVideo = nullptr;
        std::string errMsg;

        bool noDelay = (m_lastSpeed == 0.0f && !m_doForwardStep);

        auto res = m_videoDecoder->decodeStart(videoFramePtr, outVideo,
                                               m_lastKeepOriginalSize,
                                               m_lastOutputSize.width(),
                                               m_lastOutputSize.height(),
                                               noDelay, errMsg);
        if (res == DecodeResult::Success) {
            if (outVideo) {
                if (m_status != PlayStatus::Loaded) {
                    setStatus(PlayStatus::Loaded);
                }
                onNewVideoFrame(outVideo);
            }
            else {
                //SPDLOG_DEBUG("decode success, but frame may not be ready");
            }
        }
        else {
            auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastDecodeErrorClock).count();
            if (elapsedMsec > 5000) {
                SPDLOG_WARN("video decode failed, name={}, msg={}", m_mediaStdName, errMsg);
                m_lastDecodeErrorClock = now;
            }
        }

        // CUDA Decoder인 경우 마지막 I-Frame부터 다음 I-Frame까지 보관 (원본 이미지 캡쳐용)
        if(m_videoDecodeMethod == VideoDecodeMethod::CUVID) {
            if(isKeyFrame) {
                m_lastVideoFramesList.clear();
            }
            m_lastVideoFramesList.push_back(dataPtr);
        }
    }
}


// 오디오는 비디오와 동일한 큐를 사용하고, 비디오가 나올때 재생가능하면 무조건 디코딩&재생한다.
void FFmpegFrameSource::checkAudio(const std::chrono::steady_clock::time_point& now)
{
    Q_UNUSED(now);
    Wisenet::Media::MediaSourceFrameBaseSharedPtr dataPtr = nullptr;
    if (!m_mediaSourceFramesQueue->front(dataPtr)) {
        return;
    }
    if (dataPtr->getMediaType() != Wisenet::Media::MediaType::ACODEC_PARAM &&
        dataPtr->getMediaType() != Wisenet::Media::MediaType::AUDIO) {
        return;
    }
    m_mediaSourceFramesQueue->pop();

    // codec param인 경우 디코더 파라미터 설정.
    if (dataPtr->getMediaType() == Wisenet::Media::MediaType::ACODEC_PARAM) {
        auto acodecParam = std::static_pointer_cast<FFmpegCodecParam>(dataPtr);
        m_audioDecoder->openWithFFmpegCodecParam(acodecParam);
        return;
    }

    if (dataPtr->commandSeq != m_lastCommandSeq) {
        return;
    }

    auto audioFramePtr = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(dataPtr);
    checkAudioStatus(audioFramePtr);

    if (m_lastEnableAudioOut && m_lastSpeed == 1.0f) {
        AudioOutputFrameSharedPtr outAudio = nullptr;
        auto res = m_audioDecoder->decodeStart(audioFramePtr, outAudio);
        if (res == DecodeResult::Success && outAudio) {
            outAudio->setSourceID(m_sourceId);
            onNewAudioFrame(outAudio);
        }
    }
}

void FFmpegFrameSource::getLastOriginalVideoImage()
{
    VideoOutputFrameSharedPtr videoOutputPtr = nullptr;

    if(m_videoDecodeMethod == VideoDecodeMethod::CUVID) {
        // CUDA 디코더이면 m_lastVideoFramesList를 원본사이즈로 다시 디코딩.
        for(auto& framePtr : m_lastVideoFramesList) {
            auto videoFramePtr = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(framePtr);
            VideoOutputFrameSharedPtr tempOutPtr = nullptr;
            std::string errMsg;
            auto res = m_captureDecoder->decodeStart(videoFramePtr, tempOutPtr, true,
                                                     m_lastOutputSize.width(),
                                                     m_lastOutputSize.height(),
                                                     true, errMsg);
            if (res == DecodeResult::Success && tempOutPtr) {
                //SPDLOG_DEBUG("CaptureDecode succeed");
                videoOutputPtr = tempOutPtr;
            }
            else {
                //SPDLOG_DEBUG("CaptureDecode failed or not ready");
            }
        }
        m_captureDecoder->flushBuffers();
    }
    else {
        // 나머지 디코더는 디코더에서 마지막 원본사이즈 프레임을 받아 옴.
        videoOutputPtr = m_videoDecoder->getLastOriginalVideoImage();
    }

    if(!videoOutputPtr) {
        emit lastOriginalVideoImage(QImage());
        return;
    }

    auto videoBuffer = new WisenetVideoBuffer();
    videoBuffer->moveDataFrom(videoOutputPtr);
    QSize videoSize(videoOutputPtr->width(), videoOutputPtr->height());
    QVideoFrame frame(videoBuffer, videoSize, videoBuffer->pixelFormat());
    QImage image = frame.image();

    emit lastOriginalVideoImage(image);
}

void FFmpegFrameSource::checkMeta(const std::chrono::steady_clock::time_point& now)
{
    Q_UNUSED(now);
    Wisenet::Media::MediaSourceFrameBaseSharedPtr dataPtr = nullptr;
    if (!m_mediaSourceFramesQueue->front(dataPtr)) {
        return;
    }

    if (dataPtr->getMediaType() != Wisenet::Media::MediaType::META_DATA) {
        return;
    }

    m_mediaSourceFramesQueue->pop();

    if (dataPtr->commandSeq != m_lastCommandSeq) {
        return;
    }

    auto metaFramePtr = std::static_pointer_cast<Wisenet::Media::MetadataSourceFrame>(dataPtr);

    std::string metaData((char*)dataPtr->getDataPtr());
    auto metaFrame = std::make_shared<MetaFrame>(metaData, dataPtr->getDataSize());

    onNewMetaFrame((MetaFrameSharedPtr)metaFrame);
}

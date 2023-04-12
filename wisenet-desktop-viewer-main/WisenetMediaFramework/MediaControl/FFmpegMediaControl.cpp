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
#include "FFmpegMediaControl.h"
#include "FFmpegFrameSource.h"
#include "LogSettings.h"

#include <chrono>
#include <QDir>
#include <QFileInfo>

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[FFmpegMediaControl] "}, level, __VA_ARGS__)

FFmpegMediaControl::FFmpegMediaControl(QObject *parent, bool isSequence)
    : MediaControlBase(parent)
    , m_reader(new FFmpegReader())
    , m_frameSource(new FFmpegFrameSource(m_reader->getMediaSourceQueue(), isSequence))
{
    connect(m_reader.get(), &FFmpegReader::openResult, this, &FFmpegMediaControl::onOpenResult);
    SPDLOG_DEBUG("FFmpegMediaControl()");

    m_reader->start();
    m_frameSource->start();
}

FFmpegMediaControl::~FFmpegMediaControl()
{
    SPDLOG_DEBUG("~FFmpegMediaControl()");
    close();
}

FrameSourceBase *FFmpegMediaControl::source() const
{
    if (m_frameSource)
        return m_frameSource.get();
    return nullptr;
}

void FFmpegMediaControl::open()
{
    if (m_reader) {
        QString filePath = m_mediaParam.m_localResource.toLocalFile();
        QFileInfo fileInfo(filePath);
        QString nativePath = QDir::toNativeSeparators(filePath);
        QString mediaName = fileInfo.fileName();

        if (m_frameSource) {
            m_frameSource->setAttributes(mediaName);
            m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
            m_frameSource->setStatus(FrameSourceBase::Loading);

            if(m_mediaParam.m_pausedOpen) {
                pause();    // pause 상태로 Open 처리
            }
        }

        m_reader->open(nativePath);
        SPDLOG_DEBUG("open(), path={}", nativePath.toLocal8Bit().toStdString());
    }
}

void FFmpegMediaControl::close()
{
    if (m_reader) {
        SPDLOG_DEBUG("close(), path={}", m_reader->localResource());
        m_reader->close();
    }
}

void FFmpegMediaControl::setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec)
{
    SPDLOG_DEBUG("setSpeed(), path={}, speed={}", m_reader->localResource(), playbackSpeed);
    if (m_lastSpeed != playbackSpeed) {
        if (FFmpegReader::needToResetPlay(m_lastSpeed, playbackSpeed)) {
            m_commandSeq++;
        }
        m_lastSpeed = playbackSpeed;
    }
    m_reader->setSpeed(playbackSpeed, lastPlaybackTimeMsec, m_commandSeq);
    m_frameSource->setPlaySpeed(playbackSpeed, m_commandSeq);
}

void FFmpegMediaControl::seek(const qint64 playbackTimeMsec)
{
    SPDLOG_DEBUG("seek(), path={}, timeMsec={}", m_reader->localResource(), playbackTimeMsec);
    m_commandSeq++;
    m_reader->seek(playbackTimeMsec, m_commandSeq);
    m_frameSource->seek(playbackTimeMsec, m_commandSeq);
}

void FFmpegMediaControl::play(const qint64 lastPlaybackTimeMsec)
{
    SPDLOG_DEBUG("play(), lastSpeed={}, path={}", m_lastSpeed, m_reader->localResource());
    // 마지막 속도로 재생한다.
    if(m_lastSpeed < 0)
        m_commandSeq++;
    m_reader->play(lastPlaybackTimeMsec, m_lastSpeed, m_commandSeq);
    m_frameSource->play(m_lastSpeed, m_commandSeq);
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
}

void FFmpegMediaControl::pause()
{
    SPDLOG_DEBUG("pause(), path={}", m_reader->localResource());
    m_lastStepDirection = m_lastSpeed >= 0 ? LastStepDirection::Forward : LastStepDirection::Backward;  // pause 시 마지막 Step 방향 초기화
    m_reader->pause(m_commandSeq);
    m_frameSource->pause(m_commandSeq);
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PausedState));
}

void FFmpegMediaControl::step(const bool isForward, const qint64 lastPlaybackTimeMsec)
{
    SPDLOG_DEBUG("step(), path={}, isForward={}", m_reader->localResource(), isForward);
    // forward는 full frame play를 위해서 cSeq를 증가하지 않는다.
    // backward는 기본적으로 pasue+seek를 동작 동일하다.
    // 역방향 -> 정방향으로 step 방향이 바뀔 때도 cSeq를 증가시킨다.
    if (!isForward || m_lastStepDirection != LastStepDirection::Forward)
        m_commandSeq++;
    m_lastStepDirection = isForward ? LastStepDirection::Forward : LastStepDirection::Backward;
    m_reader->step(isForward, lastPlaybackTimeMsec, m_commandSeq);
    m_frameSource->step(isForward, m_commandSeq);
}

void FFmpegMediaControl::sleep(const bool isOn)
{
    SPDLOG_DEBUG("sleep(), path={}, isOn={}", m_reader->localResource(), isOn);
    m_reader->sleep(isOn);
    m_frameSource->sleep(isOn);
    if (!isOn) {
        m_frameSource->setStatus(FrameSourceBase::Loading);
    }
}

void FFmpegMediaControl::onOpenResult(Error error)
{
    if (error == Error::NoError) {
        m_frameSource->setDuration(m_reader->duration());
        m_frameSource->setSupportDuration(true);
    }
    else {
        if (error == Error::FormatError) {
            m_frameSource->setStatus(FrameSourceBase::Failed_InvalidFormat);
        }
        else {
            m_frameSource->setStatus(FrameSourceBase::Failed_InvalidMedia);
        }
    }
    emit openResult(error);
}

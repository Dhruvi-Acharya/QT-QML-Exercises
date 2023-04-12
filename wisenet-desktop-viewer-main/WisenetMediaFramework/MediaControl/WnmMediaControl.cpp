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

#include "WnmMediaControl.h"
#include "LogSettings.h"
#include <QDir>
#include <QFileInfo>

WnmMediaControl::WnmMediaControl(QObject *parent)
    : MediaControlBase(parent)
    , m_reader(new WnmReader())
    , m_frameSource(new FFmpegFrameSource(m_reader->getSourceFrameQueue()))
{
    connect(m_reader.get(), &WnmReader::openResult, this, &WnmMediaControl::onOpenResult);
    SPDLOG_DEBUG("WnmMediaControl::WnmMediaControl()");

    m_reader->start();
    m_frameSource->start();
}

WnmMediaControl::~WnmMediaControl()
{
    SPDLOG_DEBUG("WnmMediaControl::~WnmMediaControl()");
    close();
}

FrameSourceBase* WnmMediaControl::source() const
{
    if (m_frameSource)
        return m_frameSource.get();
    return nullptr;
}

void WnmMediaControl::open()
{
    if (m_reader) {
        if (m_frameSource) {
            m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
            m_frameSource->setStatus(FrameSourceBase::Loading);

            if(m_mediaParam.m_pausedOpen) {
                pause();    // pause 상태로 Open 처리
            }
        }

        QString filePath = m_mediaParam.m_localResource.toLocalFile();
        QFileInfo fileInfo(filePath);
        QString nativePath = QDir::toNativeSeparators(filePath);
        m_reader->open(nativePath);

        QString mediaName = fileInfo.fileName();
        if (m_frameSource) {
            m_frameSource->setAttributes(mediaName);
        }
    }
}

void WnmMediaControl::onOpenResult(MediaControlBase::Error error)
{
    if (error == Error::NoError) {
        /* wnm도 파일명으로 표시하도록 원복 (mkv와 통일)
        QString channelName = m_reader->getChannelName();
        m_frameSource->setAttributes(channelName);
        */
        m_frameSource->setSupportTimeline(true);
    }
    else {
        if (error == Error::FormatError) {
            m_frameSource->setStatus(FrameSourceBase::Failed_InvalidFormat);
        }
        else if (error == Error::PasswordRequired) {
            m_frameSource->setStatus(FrameSourceBase::Failed_PasswordRequired);
        }
        else {
            m_frameSource->setStatus(FrameSourceBase::Failed_InvalidMedia);
        }
    }

    emit openResult(error);
}

void WnmMediaControl::close()
{
    if (m_reader) {
        m_reader->close();
    }
}

void WnmMediaControl::setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec)
{
    if (m_lastSpeed != playbackSpeed) {
        if (WnmReader::needToResetPlay(m_lastSpeed, playbackSpeed)) {
            m_commandSeq++;
        }
        m_lastSpeed = playbackSpeed;
    }
    m_reader->setSpeed(playbackSpeed, lastPlaybackTimeMsec, m_commandSeq);
    m_frameSource->setPlaySpeed(playbackSpeed, m_commandSeq);
}

void WnmMediaControl::seek(const qint64 playbackTimeMsec)
{
    m_commandSeq++;
    m_reader->seek(playbackTimeMsec, m_commandSeq);
    m_frameSource->seek(playbackTimeMsec, m_commandSeq);
}

void WnmMediaControl::play(const qint64 lastPlaybackTimeMsec)
{
    // 마지막 속도로 재생한다.
    if(m_lastSpeed < 0)
        m_commandSeq++;
    m_reader->play(lastPlaybackTimeMsec, m_lastSpeed, m_commandSeq);
    m_frameSource->play(m_lastSpeed, m_commandSeq);
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
}

void WnmMediaControl::pause()
{
    m_lastStepDirection = m_lastSpeed >= 0 ? LastStepDirection::Forward : LastStepDirection::Backward;  // pause 시 마지막 Step 방향 초기화
    m_reader->pause(m_commandSeq);
    m_frameSource->pause(m_commandSeq);
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PausedState));
}

void WnmMediaControl::step(const bool isForward, const qint64 lastPlaybackTimeMsec)
{
    // forward는 full frame play를 위해서 cSeq를 증가하지 않는다.
    // backward는 기본적으로 pasue+seek를 동작 동일하다.
    // 역방향 -> 정방향으로 step 방향이 바뀔 때도 cSeq를 증가시킨다.
    if (!isForward || m_lastStepDirection != LastStepDirection::Forward)
        m_commandSeq++;
    m_lastStepDirection = isForward ? LastStepDirection::Forward : LastStepDirection::Backward;
    m_reader->step(isForward, lastPlaybackTimeMsec, m_commandSeq);
    m_frameSource->step(isForward, m_commandSeq);

}

void WnmMediaControl::sleep(const bool isOn)
{
    m_reader->sleep(isOn);
    m_frameSource->sleep(isOn);
    if (!isOn) {
        m_frameSource->setStatus(FrameSourceBase::Loading);
    }
}

void WnmMediaControl::checkPassword(QString password)
{
    if (m_reader) {
        m_reader->checkPassword(password);
    }
}

QMap<qint64, qint64> WnmMediaControl::getTimeline()
{
    if (m_reader) {
        return m_reader->getTimeline();
    }
    return QMap<qint64, qint64>();
}

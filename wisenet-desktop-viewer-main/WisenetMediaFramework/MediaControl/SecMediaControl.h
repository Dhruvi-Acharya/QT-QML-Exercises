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
#include "SecReader.h"
#include "FFmpegFrameSource.h"
#include <QScopedPointer>

class SecMediaControl : public MediaControlBase
{
public:
    SecMediaControl(QObject *parent = nullptr);
     ~SecMediaControl();

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
    void checkPassword(QString password) override;
    QMap<qint64, qint64> getTimeline() override;

    void onOpenResult(MediaControlBase::Error error);

private:
    QScopedPointer<SecReader> m_reader;
    QScopedPointer<FFmpegFrameSource> m_frameSource;
    unsigned char   m_commandSeq = 0;
    float           m_lastSpeed = 1.0f;
    LastStepDirection   m_lastStepDirection = LastStepDirection::None;
};


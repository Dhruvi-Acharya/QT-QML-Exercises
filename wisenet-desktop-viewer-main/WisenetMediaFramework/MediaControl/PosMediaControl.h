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
#include "FFmpegStruct.h"
#include "SafeMediaSourceFrameHandler.h"
#include "MediaWriter/FFmpegFileWriter.h"
#include "MediaWriter/WnmFileWriter.h"

class PosMediaControl : public MediaControlBase
{
    Q_OBJECT

public:
    PosMediaControl(QObject *parent = nullptr);
    virtual ~PosMediaControl();

    FrameSourceBase* source() const override;

    void onNewMetaFrame(const MetaFrameSharedPtr& metaData)
    {
        emit newMetaFrame(metaData);
    }

    void newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame);

signals:
    void newMetaFrame(const MetaFrameSharedPtr& metaFrame);

public slots:
    void open() override;
    void close() override;
    void setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec) override;
    void seek(const qint64 playbackTimeMsec) override;
    void play(const qint64 lastPlaybackTimeMsec) override;
    void pause() override;
    void step(const bool isForward, const qint64 lastPlaybackTimeMsec) override;

private:
    SourceFrameQueueSharedPtr m_sourceFramesQueue;
    SafeMediaHandlerSharedPtr m_safeHandler;
};


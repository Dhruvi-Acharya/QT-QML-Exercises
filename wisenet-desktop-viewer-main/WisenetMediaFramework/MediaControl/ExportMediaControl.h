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

class ExportMediaControl : public MediaControlBase
{
    Q_OBJECT
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)

public:
    ExportMediaControl(QObject *parent = nullptr);
    virtual ~ExportMediaControl();

    FrameSourceBase* source() const override;
    int progress(){return m_progress;}

signals:
    void progressChanged(int progress);
    void writeFinished(FileWriterBase::ErrorCode errorCode);

public slots:
    void open() override;
    void close() override;
    void setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec) override;
    void seek(const qint64 playbackTimeMsec) override;
    void play(const qint64 lastPlaybackTimeMsec) override;
    void pause() override;
    void step(const bool isForward, const qint64 lastPlaybackTimeMsec) override;
    void setExportParam(WisenetMediaParam::StreamType streamType,
                        QString deviceId,
                        QString channeId,
                        int trackId,
                        qint64 startTime,
                        qint64 endTime);
    void initializeFileWriter(QString fileDir, QString fileName, bool usePassword, QString password, bool useDigitalSignature);

    void onWriteFinished(FileWriterBase::ErrorCode errorCode);
    void onReceiveTimerTimeout();
    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);

private:
    void newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame);

private:
    SourceFrameQueueSharedPtr m_sourceFramesQueue;
    SafeMediaHandlerSharedPtr m_safeHandler;
    QScopedPointer<FileWriterBase> m_writer;
    int m_progress = 0;
    QTimer m_receiveTimer;
    std::atomic<bool> m_receiveFlag ;//= false;
    QString m_fileDir = "";
};


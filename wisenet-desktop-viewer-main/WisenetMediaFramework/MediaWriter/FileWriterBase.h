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
#include "FFmpegStruct.h"

class FileWriterBase : public QObject
{
    Q_OBJECT

public:
    enum class ErrorCode
    {
        NoError = 0,
        SetupFailed,
        FileWriteFailed,
        Timeout,
        DiskFull,
        Disconnected,
    };
    Q_ENUM(ErrorCode)

    enum class WriterType
    {
        None = 0,
        FFmpeg,
        Wnm,
    };

    explicit FileWriterBase(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent = nullptr);
    ~FileWriterBase();

    virtual const WriterType GetWriterType() = 0;
    bool PushMediaSourceFrame(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& sourceFrame);
    virtual bool StopFileWrite() = 0;

public slots:
    void onWriteFailed(const QString &message);

signals:
    void writeFinished(FileWriterBase::ErrorCode errorCode);

protected:
    virtual void loopTask() = 0;
    SourceFrameQueueSharedPtr m_sourceFramesQueue;  // media queue
};

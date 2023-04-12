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
#include "FileWriterBase.h"
#include "FFmpegFileOutContext.h"

class FFmpegFileWriter : public FileWriterBase, public MediaRunnable
{
    Q_OBJECT
public:
    enum class ErrorCode
    {
        NoError = 0,
        SetupFailed,
        FileWriteFailed,
        Timeout,
    };
    Q_ENUM(ErrorCode)

    FFmpegFileWriter(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent = nullptr);
    ~FFmpegFileWriter();

    const WriterType GetWriterType() override {return WriterType::FFmpeg;}
    bool StopFileWrite() override;
    void SetOutFileInfo(QString fileDir, QString fileName);

protected:
    void loopTask() override;

private:
    void checkCommands();

private:
    // file context for output file
    // loopTask 안에서만 접근하여 Thread 동기화 문제가 없도록 해야 함.
    FFmpegFileOutContext m_avOutContext;

    std::mutex m_mutex;
    std::atomic<bool> m_isStopCommand;
};

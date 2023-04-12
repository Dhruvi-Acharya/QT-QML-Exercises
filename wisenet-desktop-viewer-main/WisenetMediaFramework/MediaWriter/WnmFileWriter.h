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
#include "FileWriterBase.h"
#include "MediaRunnable.h"
#include "WnmFileOutContext.h"
#include "WnmStruct.h"

class WnmFileWriter : public FileWriterBase, public MediaRunnable
{
    Q_OBJECT
public:
    enum class FileWriteStatus {
        Wait = 0,
        InitializeStart,
        WriteFrames,
        FileClose,
        Finished,
    };
    Q_ENUM(FileWriteStatus)

    WnmFileWriter(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent = nullptr);
    ~WnmFileWriter();

    const WriterType GetWriterType() override {return WriterType::Wnm;}
    bool StopFileWrite() override;
    void SetInitializeParameter(QString fileDir, QString fileName, WnmInitalizeParameter initParam);

public slots:
    void onFileSplitRequest();

protected:
    void loopTask() override;

private:
    void checkFileWriteStatus();

private:
    // file context for output file
    // loopTask 안에서만 접근하여 Thread 동기화 문제가 없도록 해야 함.
    WnmFileOutContext m_fileOutContext;

    std::mutex m_mutex;
    std::atomic<FileWriteStatus> m_fileWriteStatus;
};

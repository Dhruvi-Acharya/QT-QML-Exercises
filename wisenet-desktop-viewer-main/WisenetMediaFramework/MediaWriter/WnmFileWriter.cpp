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

#include "WnmFileWriter.h"
#include "LogSettings.h"

WnmFileWriter::WnmFileWriter(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent)
    : FileWriterBase(sourceFramesQueue, parent)
    , m_fileWriteStatus(FileWriteStatus::Wait)
{
    connect(&m_fileOutContext, &WnmFileOutContext::writeFailed, this, &FileWriterBase::onWriteFailed);
    connect(&m_fileOutContext, &WnmFileOutContext::fileSplitRequest, this, &WnmFileWriter::onFileSplitRequest);

    start();
    SPDLOG_DEBUG("WnmFileWriter::WnmFileWriter() THREAD START");
}

WnmFileWriter::~WnmFileWriter()
{
    stop();
    SPDLOG_DEBUG("WnmFileWriter::~WnmFileWriter() THREAD END");
}

void WnmFileWriter::SetInitializeParameter(QString fileDir, QString fileName, WnmInitalizeParameter initParam)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_fileOutContext.SetInitializeParameter(fileDir, fileName, initParam);
    m_fileWriteStatus = FileWriteStatus::InitializeStart;
}

bool WnmFileWriter::StopFileWrite()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_fileWriteStatus = FileWriteStatus::FileClose;

    return true;
}

void WnmFileWriter::onFileSplitRequest()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_fileWriteStatus = FileWriteStatus::InitializeStart;
}

// Thread Main Loop
void WnmFileWriter::loopTask()
{
    checkFileWriteStatus();

    if (m_fileWriteStatus != FileWriteStatus::WriteFrames || m_sourceFramesQueue->size() == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // 2023.01.11. coverity (ubuntu)
    Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData(nullptr);
    while (m_sourceFramesQueue->pop_front(mediaData))
    {
        bool ret = m_fileOutContext.WriteFrame(mediaData);
        if(!ret)
        {
            StopFileWrite();
            break;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void WnmFileWriter::checkFileWriteStatus()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    switch(m_fileWriteStatus.load())
    {
    case FileWriteStatus::Wait:
    case FileWriteStatus::Finished:
        return;
    case FileWriteStatus::InitializeStart:
        if(m_sourceFramesQueue->size() == 0)
            break;

        // 영상 수신 시작 이후 파일 초기화
        if(m_fileOutContext.InitializeFile())
        {
            m_fileWriteStatus = FileWriteStatus::WriteFrames;
        }
        else
        {
            // 파일 초기화 실패 시 쓰기 종료
            m_sourceFramesQueue->clear();
            m_fileOutContext.CloseFile();
            m_fileWriteStatus = FileWriteStatus::Finished;
            emit writeFinished(FileWriterBase::ErrorCode::FileWriteFailed);
        }
        break;
    case FileWriteStatus::FileClose:
        m_sourceFramesQueue->clear();
        m_fileOutContext.CloseFile();
        m_fileWriteStatus = FileWriteStatus::Finished;
        break;
    default:
        break;
    }
}

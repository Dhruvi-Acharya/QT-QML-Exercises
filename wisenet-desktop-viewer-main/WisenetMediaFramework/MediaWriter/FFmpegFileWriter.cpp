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

#include "FFmpegFileWriter.h"
#include "LogSettings.h"

//bool FFmpegFileWriter::ms_avRegisterFlag = false;

// callback
void my_ffmpeg_log(void *ptr, int level, const char *fmt, va_list vl)
{
    /// Here you can set a more detailed level
    if (level < AV_LOG_VERBOSE)
    {
        static char message[8192];
        const char *module = NULL;

        if (ptr)
        {
            AVClass *avc = *(AVClass**) ptr;
            if (avc->item_name)
            module = avc->item_name(ptr);
        }
        vsnprintf(message, sizeof message, fmt, vl);
        // you can set own function here, for example LOGI, as you have in your example
        if (module != NULL)
            SPDLOG_DEBUG("ffmpeg message : {} {} {}", module, level, message);
        else
            SPDLOG_DEBUG("ffmpeg message : {} {}", level, message);
    }
}

FFmpegFileWriter::FFmpegFileWriter(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent)
    : FileWriterBase(sourceFramesQueue, parent)
    , m_isStopCommand(false)
{
    connect(&m_avOutContext, &FFmpegFileOutContext::writeFailed, this, &FileWriterBase::onWriteFailed);

    start();
    SPDLOG_DEBUG("FFmpegFileWriter::FFmpegFileWriter()");
}

FFmpegFileWriter::~FFmpegFileWriter()
{
    stop();
    SPDLOG_DEBUG("FFmpegFileWriter::~FFmpegFileWriter()");
}

void FFmpegFileWriter::SetOutFileInfo(QString fileDir, QString fileName)
{
    m_avOutContext.SetOutFileInfo(fileDir, fileName);
}

bool FFmpegFileWriter::StopFileWrite()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStopCommand = true;

    return true;
}

// Thread Main Loop
void FFmpegFileWriter::loopTask()
{
    checkCommands();

    if (m_sourceFramesQueue->size() == 0)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
        return;
    }

    // 2023.01.11. coverity (ubuntu)
    Wisenet::Media::MediaSourceFrameBaseSharedPtr mediaData(nullptr);
    while (m_sourceFramesQueue->pop_front(mediaData))
    {
        bool ret = m_avOutContext.DoWrite(mediaData);
        if(!ret)
        {
            StopFileWrite();
            emit writeFinished(FileWriterBase::ErrorCode::FileWriteFailed);
            break;
        }
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void FFmpegFileWriter::checkCommands()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isStopCommand) {
        m_isStopCommand = false;
        m_sourceFramesQueue->clear();
        m_avOutContext.CloseFile();
    }
}

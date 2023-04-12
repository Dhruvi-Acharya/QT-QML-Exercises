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

#include "FileWriterBase.h"
#include "LogSettings.h"

FileWriterBase::FileWriterBase(SourceFrameQueueSharedPtr sourceFramesQueue, QObject* parent)
    : QObject(parent)
    , m_sourceFramesQueue(sourceFramesQueue)
{
    SPDLOG_DEBUG("FileWriterBase::FileWriterBase()");
}

FileWriterBase::~FileWriterBase()
{
    SPDLOG_DEBUG("FileWriterBase::~FileWriterBase()");
}

bool FileWriterBase::PushMediaSourceFrame(const Wisenet::Media::MediaSourceFrameBaseSharedPtr& sourceFrame)
{
    if(sourceFrame->getDataSize() == 0)
    {
        SPDLOG_DEBUG("media source frame data size is 0.. skip");
        return false;
    }

    Wisenet::Media::MediaType mediaType = sourceFrame->getMediaType();
    if(mediaType == Wisenet::Media::MediaType::VIDEO)
    {
        auto videoData = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(sourceFrame);
        if(videoData->videoCodecType == Wisenet::Media::VideoCodecType::UNKNOWN)
        {
            SPDLOG_DEBUG("unknown video codec.. skip");
            return false;
        }
        m_sourceFramesQueue->push(sourceFrame);
        return true;
    }
    else if(mediaType == Wisenet::Media::MediaType::AUDIO)
    {
        auto audioData = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(sourceFrame);
        if (audioData->audioCodecType == Wisenet::Media::AudioCodecType::UNKNOWN)
        {
            SPDLOG_DEBUG("unknown audio codec.. skip");
            return false;
        }
        m_sourceFramesQueue->push(sourceFrame);
        return true;
    }

    return false;
}

void FileWriterBase::onWriteFailed(const QString &message)
{
    SPDLOG_DEBUG("FileWriterBase::onWriteFailed() {}", message.toStdString());
    //StopFileWrite();
    emit writeFinished(FileWriterBase::ErrorCode::FileWriteFailed);
}

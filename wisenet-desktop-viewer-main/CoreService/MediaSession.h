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

#include <cstdint>

#include "DeviceClient/DeviceStructure.h"
#include "LogManager.h"

namespace Wisenet
{
namespace Core
{

class MediaSession : public std::enable_shared_from_this<MediaSession>
{
public:
    MediaSession(LogManager& logManager,
                 const std::string &sessionID,
                 const std::string &userName,
                 const std::string &host);

    Wisenet::Media::MediaSourceFrameHandler GetMediaSourceFrameHandler();
    void Open(Device::StreamType streamType, unsigned int trackID, const std::string& deviceID,
              const std::string& channelID, Media::MediaSourceFrameHandler mediaStreamHandler);
    void Seek(int64_t utcTimeMsec);
    void OnSeek();
    void Close();

private:
    void writeAuditLog();

    LogManager& m_logManager;
    std::string m_sessionID;
    std::string m_userName;
    std::string m_host;

    Device::StreamType m_streamType;

    int64_t m_startUtcTimeMsec;
    int64_t m_endUtcTimeMsec;
    unsigned int m_trackID;
    int64_t m_updateUtcTimeMsec;
    std::string m_deviceID;
    std::string m_channelID;

    std::atomic<bool> m_isSeeking;

    Media::MediaSourceFrameHandler m_mediaStreamHandler;



};

typedef std::shared_ptr<MediaSession> MediaSessionSharedPtr;

}
}

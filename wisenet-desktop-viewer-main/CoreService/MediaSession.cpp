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
#include "MediaSession.h"

#include "WeakCallback.h"
#include "LogSettings.h"
namespace Wisenet
{
namespace Core
{

MediaSession::MediaSession(LogManager& logManager,
                           const std::string &sessionID,
                           const std::string &userName,
                           const std::string &host)
    :m_logManager(logManager)
    ,m_sessionID(sessionID)
    ,m_userName(userName)
    ,m_host(host)
    ,m_streamType(Device::StreamType::none)
    ,m_startUtcTimeMsec(0)
    ,m_endUtcTimeMsec(0)
    ,m_trackID(0)
    ,m_updateUtcTimeMsec(0)
    ,m_deviceID("")
    ,m_channelID("")
    ,m_isSeeking(false)
    ,m_mediaStreamHandler(nullptr)

{

}

Media::MediaSourceFrameHandler MediaSession::GetMediaSourceFrameHandler()
{
    auto handler =
            WeakCallback(shared_from_this(),
                         [this](const Wisenet::Media::MediaSourceFrameBaseSharedPtr& mediaSourceFrame)
    {
        if (!m_isSeeking && (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO)) {
            auto videoFrame = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaSourceFrame);
            long long ptsTimestampMsec = videoFrame->frameTime.ptsTimestampMsec;
            //SPDLOG_DEBUG("MediaSession::GetMediaSourceFrameHandler[{}] pts:{}",m_sessionID, pts);
            if(m_startUtcTimeMsec == 0) {
                m_startUtcTimeMsec = ptsTimestampMsec;
                m_endUtcTimeMsec = ptsTimestampMsec;
            } else if(m_startUtcTimeMsec > ptsTimestampMsec) {
                m_startUtcTimeMsec = ptsTimestampMsec;
            } else if(m_endUtcTimeMsec < ptsTimestampMsec) {
                m_endUtcTimeMsec = ptsTimestampMsec;
            }
        }

        if (m_mediaStreamHandler) {
            m_mediaStreamHandler(mediaSourceFrame);
        }
    });

    return handler;
}

void MediaSession::Open(Device::StreamType streamType, unsigned int trackID, const std::string& deviceID,
                        const std::string& channelID, Media::MediaSourceFrameHandler mediaStreamHandler)
{
    m_streamType = streamType;

    m_startUtcTimeMsec = 0;
    m_endUtcTimeMsec = 0;
    m_trackID = trackID;
    m_mediaStreamHandler = mediaStreamHandler;
    m_deviceID = deviceID;
    m_channelID = channelID;

    SPDLOG_DEBUG("MediaSession::Open[{}] : m_streamType:{}",m_sessionID, m_streamType);
}

void MediaSession::Seek(int64_t utcTimeMsec)
{

    SPDLOG_DEBUG("MediaSession::Seek[{}] utcTimeMsec:{}",m_sessionID ,utcTimeMsec);
    // 60초 이상 차이가 나면 m_updateUtcTimeMesc을 업데이트 한다.
    // 응답을 받으면 기존 기록을 로그로 남기고 다른 기록을 다시 만든다.
    if(((m_startUtcTimeMsec - 60000)> utcTimeMsec) || ((m_endUtcTimeMsec + 60000) < utcTimeMsec)) {
        m_updateUtcTimeMsec = utcTimeMsec;
        m_isSeeking = true;
    }
}

void MediaSession::OnSeek()
{
    SPDLOG_DEBUG("MediaSession::OnSeek[{}]",m_sessionID );
    if(!m_isSeeking) {
        return;
    }

    writeAuditLog();

    m_startUtcTimeMsec = m_endUtcTimeMsec = m_updateUtcTimeMsec;
    m_isSeeking = false;
}

void MediaSession::Close()
{
    SPDLOG_DEBUG("MediaSession::Close[{}]",m_sessionID);
    writeAuditLog();
}

void MediaSession::writeAuditLog()
{
    SPDLOG_DEBUG("MediaSession::writeAuditLog[{}] m_streamType:{}, m_startUtcTimeMsec:{}, m_endUtcTimeMsec:{}",m_sessionID, m_streamType, m_startUtcTimeMsec, m_endUtcTimeMsec);
    m_logManager.AddMediaAuditLog(m_sessionID,m_userName, m_host, m_streamType, m_startUtcTimeMsec, m_endUtcTimeMsec, m_trackID, m_deviceID, m_channelID);
}

}
}

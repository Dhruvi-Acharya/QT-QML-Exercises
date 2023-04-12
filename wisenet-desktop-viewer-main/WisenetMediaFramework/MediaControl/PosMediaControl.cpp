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
#include "PosMediaControl.h"
#include "QCoreServiceManager.h"
#include "StorageInfo.h"
#include "LogSettings.h"

PosMediaControl::PosMediaControl(QObject *parent)
    : MediaControlBase(parent)
    , m_sourceFramesQueue(std::make_shared<MediaSourceFrameQueue>())
    , m_safeHandler(nullptr)    
{
    Wisenet::Media::MediaSourceFrameHandler trueHandler =
            std::bind(&PosMediaControl::newMediaFromCoreService, this, std::placeholders::_1);
    m_safeHandler = SafeMediaSourceFrameHandler::Create();
    m_safeHandler->SetMediaHandler(trueHandler);

    SPDLOG_DEBUG("PosMediaControl::PosMediaControl()");
}

PosMediaControl::~PosMediaControl()
{
    SPDLOG_DEBUG("PosMediaControl::~PosMediaControl()");
    close();
}

void PosMediaControl::open()
{
    Wisenet::Device::DeviceMediaOpenRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMediaOpenRequest>();

    reqParam->mediaRequestInfo.streamType = Wisenet::Device::StreamType::posPlayback;

    // 미디어 아이디 초기화
#ifndef MEDIA_FILE_ONLY
    setMediaId(QCoreServiceManager::CreateNewUUidQString());
#endif

    reqParam->deviceID = m_mediaParam.m_deviceId.toStdString();
    reqParam->channelID = m_mediaParam.m_posId.toStdString();
    reqParam->mediaID = m_mediaId.toStdString();
    reqParam->mediaRequestInfo.playbackSessionId = m_mediaParam.m_sessionId.toStdString();
    reqParam->mediaRequestInfo.startTime = m_mediaParam.m_startTime;
    reqParam->mediaRequestInfo.endTime = 0; // 백업일때만 설정하도록..
    reqParam->mediaRequestInfo.trackId = m_mediaParam.m_trackId;

    reqParam->mediaRequestInfo.streamHandler = m_safeHandler->GetSafeMediaHandler();
    SPDLOG_DEBUG("PosMediaControl::open(), streamType={}, deviceUUID={}, channelID={}, startTime={}",
                 reqParam->mediaRequestInfo.streamType,
                 reqParam->deviceID, reqParam->channelID,
                 reqParam->mediaRequestInfo.startTime);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaOpen,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_DEBUG("CoreServiceMediaControl MediaOpen, isSuccess={}", response->isSuccess());
        Error error = Error::NoError;
        if (response->isFailed()) {
            error = Error::ResourceError;
        }
        emit openResult(error);
    });
#endif
}

void PosMediaControl::close()
{
    auto closeRequest = std::make_shared<Wisenet::Device::DeviceMediaCloseRequest>();
    closeRequest->deviceID = m_mediaParam.m_deviceId.toStdString();
    closeRequest->channelID = m_mediaParam.m_posId.toStdString();
    closeRequest->mediaID = m_mediaId.toStdString();
    SPDLOG_DEBUG("CoreServiceMediaControl::close(), deviceUUID={}, channelID={}",
                 closeRequest->deviceID, closeRequest->channelID);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaClose,
                this, closeRequest, nullptr);
#endif
}

FrameSourceBase* PosMediaControl::source() const
{
    return nullptr;
}

void PosMediaControl::setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(playbackSpeed);
    Q_UNUSED(lastPlaybackTimeMsec);
}

void PosMediaControl::seek(const qint64 playbackTimeMsec)
{
    Q_UNUSED(playbackTimeMsec);
}

void PosMediaControl::play(const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(lastPlaybackTimeMsec);
}

void PosMediaControl::pause()
{

}

void PosMediaControl::step(const bool isForward, const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(isForward);
    Q_UNUSED(lastPlaybackTimeMsec);
}

void PosMediaControl::newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr &mediaSourceFrame)
{
    if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        auto metaFramePtr = std::static_pointer_cast<Wisenet::Media::MetadataSourceFrame>(mediaSourceFrame);

        std::string metaData((char*)mediaSourceFrame->getDataPtr());
        auto metaFrame = std::make_shared<MetaFrame>(metaData, mediaSourceFrame->getDataSize());

        onNewMetaFrame((MetaFrameSharedPtr)metaFrame);
    }
}

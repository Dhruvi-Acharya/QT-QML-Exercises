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
#include "ExportMediaControl.h"
#include "QCoreServiceManager.h"
#include "StorageInfo.h"
#include "LogSettings.h"

ExportMediaControl::ExportMediaControl(QObject *parent)
    : MediaControlBase(parent)
    , m_sourceFramesQueue(std::make_shared<MediaSourceFrameQueue>())
    , m_safeHandler(nullptr)    
    , m_receiveFlag(false)
{
    Wisenet::Media::MediaSourceFrameHandler trueHandler =
            std::bind(&ExportMediaControl::newMediaFromCoreService, this, std::placeholders::_1);
    m_safeHandler = SafeMediaSourceFrameHandler::Create();
    m_safeHandler->SetMediaHandler(trueHandler);

    m_receiveTimer.setInterval(30000);
    connect(&m_receiveTimer, &QTimer::timeout, this, &ExportMediaControl::onReceiveTimerTimeout, Qt::QueuedConnection);

#ifndef MEDIA_FILE_ONLY
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &ExportMediaControl::onCoreServiceEventTriggered, Qt::QueuedConnection);
#endif

    SPDLOG_DEBUG("ExportMediaControl::ExportMediaControl()");
}

ExportMediaControl::~ExportMediaControl()
{
    SPDLOG_DEBUG("ExportMediaControl::~ExportMediaControl()");
    close();
}

void ExportMediaControl::open()
{
    Wisenet::Device::DeviceMediaOpenRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMediaOpenRequest>();

    if(m_mediaParam.m_streamType == WisenetMediaParam::StreamType::BackupHigh)
        reqParam->mediaRequestInfo.streamType = Wisenet::Device::StreamType::backupHigh;
    else if(m_mediaParam.m_streamType == WisenetMediaParam::StreamType::BackupLow)
        reqParam->mediaRequestInfo.streamType = Wisenet::Device::StreamType::backupLow;

    if(reqParam->mediaRequestInfo.isBackup() == false)
    {
        emit openResult(Error::StreamTypeError);
        return;
    }

    // 미디어 아이디 초기화
#ifndef MEDIA_FILE_ONLY
    setMediaId(QCoreServiceManager::CreateNewUUidQString());
#endif

    reqParam->deviceID = m_mediaParam.m_deviceId.toStdString();
    reqParam->channelID = m_mediaParam.m_channelId.toStdString();
    reqParam->mediaID = m_mediaId.toStdString();
    reqParam->mediaRequestInfo.playbackSessionId = m_mediaParam.m_sessionId.toStdString();
    reqParam->mediaRequestInfo.startTime = m_mediaParam.m_startTime;
    reqParam->mediaRequestInfo.endTime = m_mediaParam.m_endTime + 1000;
    reqParam->mediaRequestInfo.trackId = m_mediaParam.m_trackId;

    reqParam->mediaRequestInfo.streamHandler = m_safeHandler->GetSafeMediaHandler();
    SPDLOG_DEBUG("ExportMediaControl::open(), streamType={}, deviceUUID={}, channelID={}, startTime={}",
                 reqParam->mediaRequestInfo.streamType,
                 reqParam->deviceID, reqParam->channelID,
                 reqParam->mediaRequestInfo.startTime);

    m_receiveFlag = false;
    m_receiveTimer.start();

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

void ExportMediaControl::close()
{
    m_receiveFlag = false;
    m_receiveTimer.stop();

    auto closeRequest = std::make_shared<Wisenet::Device::DeviceMediaCloseRequest>();
    closeRequest->deviceID = m_mediaParam.m_deviceId.toStdString();
    closeRequest->channelID = m_mediaParam.m_channelId.toStdString();
    closeRequest->mediaID = m_mediaId.toStdString();
    SPDLOG_DEBUG("CoreServiceMediaControl::close(), deviceUUID={}, channelID={}",
                 closeRequest->deviceID, closeRequest->channelID);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaClose,
                this, closeRequest, nullptr);
#endif

    if(m_writer.data() != nullptr)
        m_writer->StopFileWrite();
}

void ExportMediaControl::setExportParam(WisenetMediaParam::StreamType streamType,
                    QString deviceId,
                    QString channeId,
                    int trackId,
                    qint64 startTime,
                    qint64 endTime)
{
    m_mediaParam.m_streamType = streamType;
    m_mediaParam.m_deviceId = deviceId;
    m_mediaParam.m_channelId = channeId;
    m_mediaParam.m_trackId = trackId;
    m_mediaParam.m_startTime = startTime;
    m_mediaParam.m_endTime = endTime;
}

void ExportMediaControl::initializeFileWriter(QString fileDir, QString fileName, bool usePassword, QString password, bool useDigitalSignature)
{
    // 파일 확장자 체크 및 Writer 초기화
    int extensionIndex = fileName.lastIndexOf(".");
    if(extensionIndex == -1 || extensionIndex == fileName.length() - 1)
    {
        emit writeFinished(FileWriterBase::ErrorCode::SetupFailed);
        return;
    }

    QString name = fileName.left(extensionIndex);
    QString extension = fileName.right(fileName.length() - extensionIndex - 1);
    if(extension.toLower() == "mkv")
    {
        m_writer.reset(new FFmpegFileWriter(m_sourceFramesQueue));
    }
    else if(extension.toLower() == "wnm")
    {
        m_writer.reset(new WnmFileWriter(m_sourceFramesQueue));
    }
    else
    {
        emit writeFinished(FileWriterBase::ErrorCode::SetupFailed);
        return;
    }

    // 파일명에 IP 추가
    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->FindDevice(m_mediaParam.m_deviceId.toStdString(), device);
#endif
    fileName = name + "(" + QString::fromStdString(device.connectionInfo.ip) + ")." + extension;

    if(m_writer->GetWriterType() == FileWriterBase::WriterType::FFmpeg)
    {
        FFmpegFileWriter* ffmpegWriter = (FFmpegFileWriter*)m_writer.data();
        ffmpegWriter->SetOutFileInfo(fileDir, fileName);
    }
    else if(m_writer->GetWriterType() == FileWriterBase::WriterType::Wnm)
    {
        WnmFileWriter* wnmWriter = (WnmFileWriter*)m_writer.data();
        Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().DB()->FindChannel(m_mediaParam.m_deviceId.toStdString(), m_mediaParam.m_channelId.toStdString(), channel);
#endif
        WnmInitalizeParameter initParam;
        initParam.channelName = QString::fromStdString(channel.name);
        initParam.usePassword = usePassword;
        initParam.password = password;
        initParam.useDigitalSignature = useDigitalSignature;
        wnmWriter->SetInitializeParameter(fileDir, fileName, initParam);
    }

    connect(m_writer.data(), &FileWriterBase::writeFinished, this, &ExportMediaControl::onWriteFinished, Qt::QueuedConnection);

    m_fileDir = fileDir;
}

void ExportMediaControl::newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr &mediaSourceFrame)
{
    m_receiveFlag = true;
    /*
    if(m_receiveTimer.interval() == 10000)
        m_receiveTimer.start(5000);
        */

    if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        // 내보내기 도중 용량 체크
        qint64 bytes = GetMbytesAvailable(m_fileDir);
        if(bytes == -1 || bytes < 50)
        {
            emit writeFinished(FileWriterBase::ErrorCode::DiskFull);
            return;
        }

        auto videoFrame = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaSourceFrame);
        m_sourceFramesQueue->push(videoFrame);

        float progress = (float)(m_mediaParam.m_startTime - mediaSourceFrame->frameTime.ptsTimestampMsec)
                / (float)(m_mediaParam.m_startTime - m_mediaParam.m_endTime) * 100;

        if(progress < 1)
            progress = 1;

        if(progress > 100)
            progress = 100;

        if(m_progress != (int)progress)
        {
            m_progress = (int)progress;
            emit progressChanged(m_progress);

            if(progress == 100)
                emit writeFinished(FileWriterBase::ErrorCode::NoError);
        }
    }
    else if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        auto audioFrame = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(mediaSourceFrame);
        m_sourceFramesQueue->push(audioFrame);
    }
}

void ExportMediaControl::onWriteFinished(FileWriterBase::ErrorCode errorCode)
{
    close();
}

void ExportMediaControl::onReceiveTimerTimeout()
{
    if(!m_receiveFlag)
    {
        m_progress = 100;
        emit progressChanged(m_progress);
        emit writeFinished(FileWriterBase::ErrorCode::NoError);
        close();
    }

    m_receiveFlag = false;
}

FrameSourceBase* ExportMediaControl::source() const
{
    return nullptr;
}

void ExportMediaControl::setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(playbackSpeed);
    Q_UNUSED(lastPlaybackTimeMsec);
}

void ExportMediaControl::seek(const qint64 playbackTimeMsec)
{
    Q_UNUSED(playbackTimeMsec);
}

void ExportMediaControl::play(const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(lastPlaybackTimeMsec);
}

void ExportMediaControl::pause()
{

}

void ExportMediaControl::step(const bool isForward, const qint64 lastPlaybackTimeMsec)
{
    Q_UNUSED(isForward);
    Q_UNUSED(lastPlaybackTimeMsec);
}

/**
 * @brief ExportMediaControl::onCoreServiceEventTriggered
 * 코어서비스 이벤트 처리 루틴
 * @param event
 */
void ExportMediaControl::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    bool disconnected = false;
    auto serviceEvent = event->eventDataPtr;

    if (serviceEvent->EventTypeId() ==  Wisenet::Device::DeviceStatusEventType) {
        auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(serviceEvent);
        if(deviceStatusEvent->deviceID == m_mediaParam.m_deviceId.toStdString() && !deviceStatusEvent->deviceStatus.isGood()) {
            disconnected = true;    // 내보내기 중인 Device Disconnect
            SPDLOG_DEBUG("ExportMediaControl::onCoreServiceEventTriggered() DeviceStatusEventType!, deviceID={} status={}",
                     deviceStatusEvent->deviceID, deviceStatusEvent->deviceStatus.status);
        }
    }
    else if(serviceEvent->EventTypeId() ==  Wisenet::Device::MediaDisconnectedEventType) {
        auto mediaDisconnectEvent = std::static_pointer_cast<Wisenet::Device::MediaDisconnectedEvent>(serviceEvent);
        if(m_mediaId.toStdString() == mediaDisconnectEvent->mediaID) {
            disconnected = true;    // 내보내기 중인 RTSP Media Disconnect
            SPDLOG_DEBUG("ExportMediaControl::onCoreServiceEventTriggered() MediaDisconnectedEventType!, mediaID={}", mediaDisconnectEvent->mediaID);
        }
    }

    if(disconnected) {
        // 접속 해제 시 내보내기 실패 처리
        emit writeFinished(FileWriterBase::ErrorCode::Disconnected);
        close();
    }
}

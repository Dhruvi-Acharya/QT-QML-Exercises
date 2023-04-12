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
#include "CoreServiceMediaControl.h"
#include "CoreServiceFrameSource.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include "FileWriteManager.h"
#include <chrono>
#include <QFileInfo>


#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[CsMediaControl] "}, level, __VA_ARGS__)



CoreServiceMediaControl::CoreServiceMediaControl(QObject *parent, bool isSequence)
    : MediaControlBase(parent)
    , m_sourceFramesQueue(std::make_shared<MediaSourceFrameQueue>())
    , m_frameSource(new CoreServiceFrameSource(m_sourceFramesQueue, isSequence))
    , m_safeHandler(nullptr)
    , m_lastVideoFrameTimeMsec(0)
    , m_writer(nullptr)
    , m_writeDurationTimer(nullptr)
    , m_writeStarted(false)
    , m_isClosed(true)
    , m_isSleep(false)
    , m_lastStreamType(Wisenet::Device::StreamType::none)
{
#ifndef MEDIA_FILE_ONLY
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &CoreServiceMediaControl::onCoreServiceEventTriggered, Qt::QueuedConnection);

    connect(m_frameSource.data(), &CoreServiceFrameSource::videoLossTriggered,
            this, &CoreServiceMediaControl::onVideoLossTriggered, Qt::QueuedConnection);

    m_mediaReconnectPeriodMsec = QCoreServiceManager::Instance().Settings()->mediaReconnectPeriod()*1000;
#endif
    m_lastControlInfo.controlType = Wisenet::Device::ControlType::play;
    m_lastControlInfo.speed = 1.0f;

    Wisenet::Media::MediaSourceFrameHandler trueHandler =
            std::bind(&CoreServiceMediaControl::newMediaFromCoreService, this, std::placeholders::_1);
    m_safeHandler = SafeMediaSourceFrameHandler::Create();
    m_safeHandler->SetMediaHandler(trueHandler);

    SPDLOG_DEBUG("CoreServiceMediaControl()");
}

CoreServiceMediaControl::~CoreServiceMediaControl()
{
    SPDLOG_DEBUG("~CoreServiceMediaControl()");
    close();
}

FrameSourceBase *CoreServiceMediaControl::source() const
{
    if (m_frameSource)
        return m_frameSource.get();
    return nullptr;
}



bool CoreServiceMediaControl::isHigh()
{
    bool isHigh = false;
    int baseSize = 640*360;
    int calcSize = m_outputSize.width() * m_outputSize.height();
    if (calcSize > baseSize) {
        isHigh = true;
    }
    SPDLOG_DEBUG("isHigh(), res={}x{}, isHigh={}",
                 m_outputSize.width(), m_outputSize.height(), isHigh);
    return isHigh;
}



Wisenet::Device::StreamType CoreServiceMediaControl::convertStreamType(const WisenetMediaParam::StreamType streamType)
{
    switch(streamType) {
    case WisenetMediaParam::StreamType::LiveHigh:
        return Wisenet::Device::StreamType::liveHigh;
    case WisenetMediaParam::StreamType::LiveLow:
        return Wisenet::Device::StreamType::liveLow;
    case WisenetMediaParam::StreamType::PlaybackHigh:
        return Wisenet::Device::StreamType::playbackHigh;
    case WisenetMediaParam::StreamType::PlaybackLow:
        return Wisenet::Device::StreamType::playbackLow;
    case WisenetMediaParam::StreamType::BackupHigh:
        return Wisenet::Device::StreamType::backupHigh;
    case WisenetMediaParam::StreamType::BackupLow:
        return Wisenet::Device::StreamType::backupLow;
    case WisenetMediaParam::StreamType::PosLive:
        return Wisenet::Device::StreamType::posLive;
    case WisenetMediaParam::StreamType::PosPlayback:
        return Wisenet::Device::StreamType::posPlayback;
    case WisenetMediaParam::StreamType::PosBackup:
        return Wisenet::Device::StreamType::posBackup;
    case WisenetMediaParam::StreamType::LiveAuto:
        return isHigh() ? Wisenet::Device::StreamType::liveHigh : Wisenet::Device::StreamType::liveLow;
    case WisenetMediaParam::StreamType::PlaybackAuto:
        return isHigh() ? Wisenet::Device::StreamType::playbackHigh : Wisenet::Device::StreamType::playbackLow;;
    default:
        return Wisenet::Device::StreamType::liveHigh;
    }
}

/*
 * CAUTION::VideoItem.qml에서 사용중이므로 해당 텍스트를 임의로 변경하지 말것.
 * 변경시에는 Live(H)로 검색해서, 해당 qml부분도 함께 수정필요.
 * TODO::나중에 ENUM값으로 정리하고, QML에서 텍스트로 표시하는게 유지보수에 좋을것.
 */
QString CoreServiceMediaControl::streamTypeToQString(const Wisenet::Device::StreamType streamType)
{
    if (streamType == Wisenet::Device::StreamType::liveHigh)
        return "Live(H)";
    if (streamType == Wisenet::Device::StreamType::liveLow)
        return "Live(L)";
    if (streamType == Wisenet::Device::StreamType::playbackHigh)
        return "Playback(H)";
    if (streamType == Wisenet::Device::StreamType::playbackLow)
        return "Playback(L)";
    if (streamType == Wisenet::Device::StreamType::backupHigh)
        return "Backup(H)";
    if (streamType == Wisenet::Device::StreamType::backupLow)
        return "Backup(L)";
    if (streamType == Wisenet::Device::StreamType::posLive)
        return "PosLive";
    if (streamType == Wisenet::Device::StreamType::posPlayback)
        return "PosPlayback";
    if (streamType == Wisenet::Device::StreamType::posBackup)
        return "PosBackup";

    return "Unknown";
}

void CoreServiceMediaControl::open()
{
#ifndef MEDIA_FILE_ONLY
    if (!m_isClosed) {
        SPDLOG_WARN("open(), should not be opened before close() call, do nothing, deviceID={}, channelID={}",
                    m_mediaParam.m_deviceId.toStdString(), m_mediaParam.m_channelId.toStdString());
        return;
    }

    Wisenet::Device::DeviceMediaOpenRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMediaOpenRequest>();

    // 미디어 아이디 초기화
    setMediaId(QCoreServiceManager::CreateNewUUidQString());
    resetCseq(0);
    m_sourceFramesQueue->clear();

    // 나중에 코어서비스로 명령내리기 편하도록 std::string으로 변환한 값을 저장하고 있는다.
    m_stdDeviceId = m_mediaParam.m_deviceId.toStdString();
    m_stdChannelId = m_mediaParam.m_channelId.toStdString();
    m_stdMediaId = m_mediaId.toStdString();

    reqParam->mediaRequestInfo.streamType = convertStreamType(m_mediaParam.m_streamType);
    reqParam->deviceID = m_stdDeviceId;
    reqParam->channelID = m_stdChannelId;
    reqParam->mediaID = m_stdMediaId;

    if (!m_mediaParam.m_profileId.isEmpty())
        reqParam->mediaRequestInfo.profileId = m_mediaParam.m_profileId.toStdString();

    // frame source의 스트림 타입 업데이트 (qml attribute 용)
    if (m_frameSource) {
        m_frameSource->setStreamType(m_mediaParam.m_streamType);
        m_frameSource->setSpeed(m_mediaParam.m_speed);
        m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
    }

    // 플레이백/백업인 경우, 재생시간, session id, track id 설정
    if (!reqParam->mediaRequestInfo.isLive()) {
        m_lastControlInfo.controlType = Wisenet::Device::ControlType::play;
        m_lastControlInfo.speed = m_mediaParam.m_speed;
        m_lastControlInfo.time = m_mediaParam.m_startTime;
        m_lastVideoFrameTimeMsec = m_mediaParam.m_startTime;

        reqParam->mediaRequestInfo.playbackSessionId = m_mediaParam.m_sessionId.toStdString();
        reqParam->mediaRequestInfo.trackId = m_mediaParam.m_trackId;

        reqParam->mediaRequestInfo.startTime = m_lastControlInfo.time;
        reqParam->mediaRequestInfo.speed = m_lastControlInfo.speed;

        if (reqParam->mediaRequestInfo.isBackup()) {
            reqParam->mediaRequestInfo.endTime = m_mediaParam.m_endTime;
        }

        if(m_mediaParam.m_pausedOpen) {
            // pause 상태로 Open 처리
            m_frameSource->pause();
            m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PausedState));
            m_lastControlInfo.controlType = Wisenet::Device::ControlType::pause;
            reqParam->mediaRequestInfo.isPausedOpen = true;
        }
    }

    reqParam->mediaRequestInfo.streamHandler = m_safeHandler->GetSafeMediaHandler();
    SPDLOG_INFO("open(), streamType={}, deviceUUID={}, channelID={}, mediaID={}, startTime={}",
                 reqParam->mediaRequestInfo.streamType,
                 reqParam->deviceID, reqParam->channelID, reqParam->mediaID,
                 reqParam->mediaRequestInfo.startTime);

    Wisenet::Device::Device::Channel channel;
    if (!QCoreServiceManager::Instance().DB()->FindChannel(reqParam->deviceID, reqParam->channelID, channel)) {
        SPDLOG_ERROR("open(), cannot find deviceID={}, channelID={}", reqParam->deviceID, reqParam->channelID);
        m_frameSource->setStatus(FrameSourceBase::Failed_ChannelDisconnected);
        Error error = Error::ChannelClosedError;
        emit openResult(error);
        return;
    }

    Wisenet::Device::ChannelStatus channelStatus;
    if (!QCoreServiceManager::Instance().DB()->FindChannelStatus(reqParam->deviceID, reqParam->channelID, channelStatus)) {
        SPDLOG_ERROR("open(), cannot find status of deviceID={}, channelID={}", reqParam->deviceID, reqParam->channelID);
        m_frameSource->setStatus(FrameSourceBase::Failed_ChannelDisconnected);
        Error error = Error::ChannelClosedError;
        emit openResult(error);
        return;
    }
    m_lastChannelStatus = channelStatus;

    auto userPermssion = QCoreServiceManager::Instance().DB()->GetLoginUserPermission();
    if (m_frameSource) {
        m_frameSource->updateChannel(channel);
        m_frameSource->updateUserPermission(userPermssion);
        if (m_mediaParam.isLive()) {
            m_lastLiveHighProfile = channel.highProfile;
            m_lastLiveLowProfile = channel.lowProfile;
        }
    }

    // 채널 상태는 Live일때만 확인한다.
    if (reqParam->mediaRequestInfo.isLive() && !channelStatus.isGood()) {
        SPDLOG_INFO("Live video channel status is abnormal, deviceID={}, channelID={}, status={}",
                    reqParam->deviceID, reqParam->channelID, channelStatus.status);
        m_frameSource->setStatus(FrameSourceBase::Failed_ChannelDisconnected);

        Error error = Error::ChannelClosedError;
        emit openResult(error);
        return;
    }

    // 채널이 PLAYBACK을 지원하지 않거나, 뷰어 사용자의 권한 없음 체크
    if (reqParam->mediaRequestInfo.isPlayback()) {
        bool supportPlayback = m_frameSource->supportPlayback();
        bool hasPermission = userPermssion.playback;

        if (!supportPlayback || !hasPermission) {
            SPDLOG_INFO("no permission playback channel, deviceID={}, channelID={}, support={}, userPermission={}",
                        reqParam->deviceID, reqParam->channelID, supportPlayback, hasPermission);
            m_frameSource->setStatus(FrameSourceBase::Failed_NoPermission);

            Error error = Error::PermissionError;
            emit openResult(error);
            return;
        }
    }

    m_isClosed = false;
    m_lastStreamType = reqParam->mediaRequestInfo.streamType;
    if (m_frameSource) {
        m_frameSource->setup(m_lastStreamType);
        m_frameSource->setVideoProfile(streamTypeToQString(m_lastStreamType));
        m_frameSource->open();
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaOpen,
                this, reqParam,
                [this, reqParam](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_DEBUG("MediaOpen, isSuccess={}, errorCode={}/{}, deviceId={}, channelId={}, mediaId={}",
                     response->isSuccess(), response->errorCode, Wisenet::GetErrorString(response->errorCode),
                     m_stdDeviceId, m_stdChannelId, m_stdMediaId);

        // 장비상태 변경등으로 인해서, 이미 close를 한 상태이고, 뒤늦게 올라온 경우에는 예외처리 한다.
        if (m_isClosed) {
            SPDLOG_INFO("MediaOpen() but already closed session, ignore it! isSuccess={}, deviceId={}, channelId={}, mediaId={}",
                        response->isSuccess(), m_stdDeviceId, m_stdChannelId, m_stdMediaId);
        }

        Error error = Error::NoError;
        if (response->isFailed()) {
            if (response->errorCode == Wisenet::ErrorCode::UserFull) {
                m_frameSource->setStatus(FrameSourceBase::Failed_SessionFull);
                error = Error::SessionFullError;
            }
            else if (response->errorCode == Wisenet::ErrorCode::PermissionDenied) {
                m_frameSource->setStatus(FrameSourceBase::Failed_NoPermission);
                error = Error::PermissionError;
            }
            else if (response->errorCode == Wisenet::ErrorCode::FwupdateMediaError) {
                m_frameSource->setStatus(FrameSourceBase::Failed_FirmwareUpdating);
                error = Error::ResourceError;
            }
            else {
                m_frameSource->setStatus(FrameSourceBase::Failed_InvalidMedia);
                error = Error::ResourceError;
            }
            // open()이 실패한 경우, close()를 바로 호출해서 CoreService의 세션을 정리한다.
            close();
        }

        if(error == Error::NoError)
        {
            m_mediaRequestTime = QDateTime::currentMSecsSinceEpoch();
        }

        emit openResult(error);

        // firmware 업데이트 중이거나 network error, 응답 타임아웃인 경우, reopen timer를 시작시킨다.
        // Live인 경우에만
        if (reqParam->mediaRequestInfo.isLive() && response->isFailed() ) {
            if (response->errorCode == Wisenet::ErrorCode::NetworkError ||
                response->errorCode == Wisenet::ErrorCode::FwupdateMediaError ||
                response->errorCode == Wisenet::ErrorCode::NoResponse) {
                startReopenTimer(true);
            }
        }
    });
#endif
}

void CoreServiceMediaControl::onChannelStatusChanged()
{
#ifndef MEDIA_FILE_ONLY
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    if (!m_mediaParam.isLive()) {
        // playback channel은 무시
        SPDLOG_INFO("video channel status changed, but playback will be continued. deviceID={}, channeID={}, mediaID={}",
                    m_stdDeviceId, m_stdChannelId, m_stdMediaId);
        return;
    }

    // 2023.01.03. coverity
    Wisenet::Device::ChannelStatus channelStatus;
    bool found = false;
    if (db != nullptr) {
        std::mutex mutex;
        mutex.lock();
        found = db->FindChannelStatus(m_stdDeviceId, m_stdChannelId, channelStatus);
        mutex.unlock();
    }

    if (!found) {
        SPDLOG_ERROR("onChannelStatusChanged(), cannot find status of deviceID={}, channelID={}, mediaID={}", m_stdDeviceId, m_stdChannelId, m_stdMediaId);
        return;
    }

    SPDLOG_INFO("video channel status changed, deviceID={}, channeID={}, mediaID={}, good={}", m_stdDeviceId, m_stdChannelId, m_stdMediaId, channelStatus.isGood());

    m_lastChannelStatus = channelStatus;
    if (m_lastChannelStatus.isGood()) {
        if (m_isClosed && !m_isSleep) {
            // 영상 재요청 타이머를 기동한다. (채널 ON되더라도 바로 NVR에 요청하면 실패해서 타이머를 준다)
            startReopenTimer(true);
        }
    }
    else {
        if (!m_isClosed) {
            close();
            SPDLOG_DEBUG("onChannelStatusChanged(), CLOSE deviceID={}, channeID={}", m_stdDeviceId, m_stdChannelId);
        }
        m_frameSource->setStatus(FrameSourceBase::Failed_ChannelDisconnected);
    }
#endif
}

void CoreServiceMediaControl::onChannelConfigChanged()
{
#ifndef MEDIA_FILE_ONLY
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.03. coverity
    Wisenet::Device::Device::Channel channel;
    bool found = false;
    if (db != nullptr) {
        std::mutex mutex;
        mutex.lock();
        found = db->FindChannel(m_stdDeviceId, m_stdChannelId, channel);
        mutex.unlock();
    }

    if (!found) {
        SPDLOG_ERROR("onChannelConfigChanged(), cannot find deviceID={}, channelID={}", m_stdDeviceId, m_stdChannelId);
        return;
    }

    SPDLOG_DEBUG("onChannelConfigChanged(), deviceID={}, channeID={}", m_stdDeviceId, m_stdChannelId);
    if (m_frameSource) {
        m_frameSource->updateChannel(channel);
    }
    // 라이브인 경우 primary/secondary 프로파일 변경사항을 체크해서 변경되었으면 다시 요청해야 한다.
    if (m_mediaParam.isLive()) {
        if ( (m_lastLiveHighProfile != channel.highProfile &&
              m_lastStreamType == Wisenet::Device::StreamType::liveHigh) ||
             (m_lastLiveLowProfile != channel.lowProfile &&
              m_lastStreamType == Wisenet::Device::StreamType::liveLow)) {
            SPDLOG_DEBUG("onChannelConfigChanged(), stream profile changed reopen!!, deviceID={}, channelID={}",
                         m_stdDeviceId, m_stdChannelId);
            reopen();
        }
    }
#endif
}

void CoreServiceMediaControl::onMediaDisconnected()
{
    if (!m_isClosed) {
        close();
        SPDLOG_DEBUG("onMediaDisconnected(), CLOSE deviceID={}, channeID={}", m_stdDeviceId, m_stdChannelId);
        m_frameSource->setStatus(FrameSourceBase::Failed_MediaDisconnected);

        // 영상 재요청 타이머를 기동한다.
        startReopenTimer(true);
    }
}

void CoreServiceMediaControl::startReopenTimer(const bool isDisconnected)
{
    if (m_mediaReopenTimer) {
        m_mediaReopenTimer->stop();
    }

    int msec = isDisconnected ? m_mediaReconnectPeriodMsec : 30000; // 채널상태 변경후 재요청은 10초, vloss 재요청은 30초
    m_mediaReopenTimer.reset(new QTimer());
    m_mediaReopenTimer->setSingleShot(true);
    m_mediaReopenTimer->setInterval(msec); // 3초 후 영상 재요청
    SPDLOG_DEBUG("startReopenTimer(), deviceID={}, channelID={}, mediaID={}, expireAfter={}ms",
                 m_stdDeviceId, m_stdChannelId, m_stdMediaId, msec);

    if (isDisconnected) {
        connect(m_mediaReopenTimer.data(), &QTimer::timeout, this,
                &CoreServiceMediaControl::onDisconnectedReopenTimerExpired,
                Qt::QueuedConnection);
    }
    else {
        connect(m_mediaReopenTimer.data(), &QTimer::timeout, this,
                &CoreServiceMediaControl::onVideoLossReopenTimerExpired,
                Qt::QueuedConnection);
    }
    m_mediaReopenTimer->start();
}

void CoreServiceMediaControl::onDisconnectedReopenTimerExpired()
{
    SPDLOG_DEBUG("onDisconnectedReopenTimerExpired(), deviceId={}, channelId={}, mediaId={}, good={}, closed={}, sleep={}",
                 m_stdDeviceId, m_stdChannelId, m_stdMediaId, m_lastChannelStatus.isGood(),
                 m_isClosed, m_isSleep);

    if (!m_lastChannelStatus.isGood()) {
        return;
    }

    if (m_isClosed && !m_isSleep) {
        SPDLOG_DEBUG("onDisconnectedReopenTimerExpired(), start reopen(), deviceId={}, channelId={}, mediaId={}",
                     m_stdDeviceId, m_stdChannelId, m_stdMediaId);
        reopen(true);
    }
}

void CoreServiceMediaControl::onVideoLossReopenTimerExpired()
{
    if (!m_lastChannelStatus.isGood())
        return;
    if (!m_isClosed && !m_isSleep && m_frameSource->status() == FrameSourceBase::Failed_Timedout) {
        SPDLOG_DEBUG("onVideoLossReopenTimerExpired()");
        reopen(true);
    }
}


void CoreServiceMediaControl::close()
{
    if (!m_isClosed) {
        m_isClosed = true;
        m_frameSource->close();
        m_sourceFramesQueue->clear();

        auto closeRequest = std::make_shared<Wisenet::Device::DeviceMediaCloseRequest>();
        closeRequest->deviceID = m_stdDeviceId;
        closeRequest->channelID = m_stdChannelId;
        closeRequest->mediaID = m_stdMediaId;
        SPDLOG_INFO("close(), deviceUUID={}, channelID={}, mediaID={}",
                     closeRequest->deviceID, closeRequest->channelID, closeRequest->mediaID);

#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().RequestToCoreService(
                    &Wisenet::Core::ICoreService::DeviceMediaClose,
                    this, closeRequest, nullptr);
#endif
    }
    if (m_mediaReopenTimer) {
        m_mediaReopenTimer->stop();
    }
    stopLocalRecording();
}

void CoreServiceMediaControl::setSpeed(const float playbackSpeed, const qint64 lastPlaybackTimeMsec)
{
    auto controlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    controlRequest->deviceID = m_stdDeviceId;
    controlRequest->channelID =  m_stdChannelId;
    controlRequest->mediaID = m_stdMediaId;
    SPDLOG_DEBUG("setSpeed(), deviceUUID={}, channelID={}",
                 controlRequest->deviceID, controlRequest->channelID);

    m_lastControlInfo.controlType = Wisenet::Device::ControlType::play;
    m_lastControlInfo.time = lastPlaybackTimeMsec;
    m_lastControlInfo.speed = playbackSpeed;

    controlRequest->mediaControlInfo = m_lastControlInfo;

    invalidateCseq();
    m_frameSource->setPlaySpeed(playbackSpeed);
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, controlRequest, nullptr);
#endif
}

void CoreServiceMediaControl::seek(const qint64 playbackTimeMsec)
{
    auto controlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    controlRequest->deviceID = m_stdDeviceId;
    controlRequest->channelID =  m_stdChannelId;
    controlRequest->mediaID = m_stdMediaId;
    SPDLOG_DEBUG("seek(), deviceUUID={}, channelID={}",
                 controlRequest->deviceID, controlRequest->channelID);

    m_lastControlInfo.time = playbackTimeMsec;
    controlRequest->mediaControlInfo = m_lastControlInfo;
    if (m_lastControlInfo.controlType != Wisenet::Device::ControlType::play)
    {
        controlRequest->mediaControlInfo.controlType = Wisenet::Device::ControlType::play;
        controlRequest->mediaControlInfo.speed = 0.0f;
    }

    invalidateCseq();
    m_frameSource->seek();
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, controlRequest, nullptr);
#endif
}

void CoreServiceMediaControl::play(const qint64 lastPlaybackTimeMsec)
{
    auto controlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    controlRequest->deviceID = m_stdDeviceId;
    controlRequest->channelID = m_stdChannelId;
    controlRequest->mediaID = m_stdMediaId;
    SPDLOG_DEBUG("play(), deviceUUID={}, channelID={}",
                 controlRequest->deviceID, controlRequest->channelID);

    m_lastControlInfo.controlType = Wisenet::Device::ControlType::play;
    m_lastControlInfo.time = lastPlaybackTimeMsec;
    controlRequest->mediaControlInfo = m_lastControlInfo;

    invalidateCseq();
    m_frameSource->play(m_lastControlInfo.speed);
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PlayingState));
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, controlRequest, nullptr);
#endif
}

void CoreServiceMediaControl::pause()
{
    auto controlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    controlRequest->deviceID = m_stdDeviceId;
    controlRequest->channelID = m_stdChannelId;
    controlRequest->mediaID = m_stdMediaId;
    SPDLOG_DEBUG("pause(), deviceUUID={}, channelID={}",
                 controlRequest->deviceID, controlRequest->channelID);

    m_lastControlInfo.controlType = Wisenet::Device::ControlType::pause;
    controlRequest->mediaControlInfo = m_lastControlInfo;

    invalidateCseq();
    m_frameSource->pause();
    m_frameSource->setPlaybackState(static_cast<int>(PlaybackState::PausedState));

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, controlRequest, nullptr);
#endif
}

void CoreServiceMediaControl::step(const bool isForward, const qint64 lastPlaybackTimeMsec)
{
    auto controlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    controlRequest->deviceID = m_stdDeviceId;
    controlRequest->channelID = m_stdChannelId;
    controlRequest->mediaID = m_stdMediaId;
    SPDLOG_DEBUG("step(), deviceUUID={}, channelID={}, isForward={}",
                 controlRequest->deviceID, controlRequest->channelID, isForward);

    m_lastControlInfo.time = lastPlaybackTimeMsec;
    controlRequest->mediaControlInfo = m_lastControlInfo;
    controlRequest->mediaControlInfo.controlType = isForward
            ? Wisenet::Device::ControlType::stepForward
            : Wisenet::Device::ControlType::stepBackward;

    invalidateCseq();
    m_frameSource->step(isForward);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, controlRequest, nullptr);
#endif
}

void CoreServiceMediaControl::sleep(const bool isOn)
{
    if (m_isSleep != isOn) {
        SPDLOG_DEBUG("Sleep Mode Changed, deviceID={}, channelID={}, isOn={}", m_stdDeviceId, m_stdChannelId, isOn);
        m_isSleep = isOn;
        // live/playback 무조건 close후 open
        if (isOn) {
            close();
        }
        else {
            reopen(true);
        }
        m_frameSource->sleep(isOn);
    }
}


bool CoreServiceMediaControl::checkFrameCseq(const unsigned short cSeq)
{
    // live는 무시한다.
    if (m_mediaParam.isLive()) {
        return true;
    }

    if (m_lastCommandSeq == m_compareCommandSeq) {
        resetCseq(cSeq); // 방어코드이고 여기가 normal case임.
        return true;
    }

    if (m_lastCommandSeq == cSeq) {
        return false;
    }

    SPDLOG_DEBUG("checkFrameCseq() receive new sequence frame, cSeq={}, lastSeq={}, compSeq={}, deviceID={}, channelID={}",
                 cSeq, m_lastCommandSeq, m_compareCommandSeq, m_stdDeviceId, m_stdChannelId);
    resetCseq(cSeq);
    return true;
}

// cSeq 값을 초기화한다.
void CoreServiceMediaControl::resetCseq(const unsigned short cSeq)
{
    /*
    if(m_lastCommandSeq != cSeq || m_compareCommandSeq != cSeq) {
        SPDLOG_DEBUG("resetCseq() cSeq={}, deviceID={}, channelID={}", cSeq, m_stdDeviceId, m_stdChannelId);
    }
    */

    m_lastCommandSeq = cSeq;
    m_compareCommandSeq = cSeq;
}

// 다음 명령에 대한 프레임을 받을 준비를 하고, 이전 프레임은 무효화한다.
// Source Queue를 함께 초기화한다.
// command sequence number는 CoreService(Device Client)에서 관리한다.
void CoreServiceMediaControl::invalidateCseq()
{
    SPDLOG_DEBUG("invalidateCseq()");
    m_compareCommandSeq = m_lastCommandSeq+1;
    m_sourceFramesQueue->clear();
}

bool CoreServiceMediaControl::checkDualStream()
{
    bool supportDualStreamLive = false;
    bool supportDualStreamPlayback = false;
    if (m_frameSource) {
        supportDualStreamLive = m_frameSource->supportDualStreamLive();
        supportDualStreamPlayback = m_frameSource->supportDualStreamPlayback();
    }

    if ((supportDualStreamLive && m_mediaParam.isLive()) ||
        (supportDualStreamPlayback && m_mediaParam.isPlayback()))
    {
        auto newStreamType = convertStreamType(m_mediaParam.m_streamType);
        if (newStreamType != m_lastStreamType) {
             SPDLOG_DEBUG("checkDualStream(), change dual stream source, {},{}",
                          newStreamType, m_lastStreamType);
             // 내부에서 reopen 동작을 이미 수행하고 있는 경우는 하지 않는다.
             if (m_frameSource->status() == FrameSourceBase::Loading) {
                 SPDLOG_DEBUG("checkDualStream(), internal reopen is doing do nothing");
                 return false;
             }
             reopen();
             return true;
        }
    }
    return false;
}

bool CoreServiceMediaControl::switchingOpen(const WisenetMediaParam* param)
{
    close();
    setMediaParam(param);

    open();
    return true;
}

bool CoreServiceMediaControl::changeStreamType(const WisenetMediaParam::StreamType streamType)
{
    m_mediaParam.m_streamType = streamType;
    auto newStreamType = convertStreamType(m_mediaParam.m_streamType);
    if (newStreamType != m_lastStreamType) {
        SPDLOG_DEBUG("changeStreamType(), change dual stream source, {},{}",
                     newStreamType, m_lastStreamType);
        reopen();
        return true;
    }

    return false;
}


void CoreServiceMediaControl::reopen(const bool resetSpeed)
{
    close();

    if (!m_mediaParam.isLive()) {
        m_mediaParam.m_startTime = m_lastVideoFrameTimeMsec;
        if (resetSpeed) {
            m_mediaParam.m_speed = 1.0f;
            m_mediaParam.m_pausedOpen = false;  // pausedOpen 상태도 초기화
        }
        else {
            m_mediaParam.m_speed = m_lastControlInfo.speed;
            m_mediaParam.m_pausedOpen = m_lastControlInfo.controlType == Wisenet::Device::ControlType::pause;   // reopen 시 pause 상태 유지
        }
    }

    open();
}


/**
 * @brief CoreServiceMediaControl::onCoreServiceEventTriggered
 * 코어서비스 이벤트 처리 루틴
 * 영상 끊김
 * 장비/채널 상태 변경
 * 채널 저장 정보 변경
 * @param event
 */
void CoreServiceMediaControl::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    // SLEEP 모드에서는 이벤트를 별도로 처리하지 않는다.
    if (m_isSleep) {
        return;
    }

    auto serviceEvent = event->eventDataPtr;
    if (serviceEvent->EventTypeId() ==  Wisenet::Device::DeviceStatusEventType) {
        auto deviceStatusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(serviceEvent);
        if (deviceStatusEvent->deviceID == m_stdDeviceId) {
            // 디바이스 상태 변경시에는 이벤트를 무조건 처리한다.
            SPDLOG_DEBUG("onCoreServiceEventTriggered():: DeviceStatusEventType!, deviceID={}", m_stdDeviceId);
            onChannelStatusChanged();
        }
    }
    else if (serviceEvent->EventTypeId() ==  Wisenet::Device::ChannelStatusEventType) {
        auto channelStatusEvent = std::static_pointer_cast<Wisenet::Device::ChannelStatusEvent>(serviceEvent);
        if (channelStatusEvent->deviceID != m_stdDeviceId)
            return;

        // 채널 상태 변경시에는 자기 채널인지 확인후 처리한다.
        if (channelStatusEvent->channelsStatus.find(m_stdChannelId) != channelStatusEvent->channelsStatus.end()) {
            SPDLOG_DEBUG("onCoreServiceEventTriggered():: ChannelStatusEventType!, deviceID={}, channelID={}", m_stdDeviceId, m_stdChannelId);
            onChannelStatusChanged();
        }
    }
    else if(serviceEvent->EventTypeId() ==  Wisenet::Device::MediaDisconnectedEventType) {
        auto mediaDisconnectEvent = std::static_pointer_cast<Wisenet::Device::MediaDisconnectedEvent>(serviceEvent);
        // 미디어 상태 변경시에는 자기 채널인지 확인후 처리한다.
        if (mediaDisconnectEvent->deviceID == m_stdDeviceId &&
            mediaDisconnectEvent->channelID == m_stdChannelId &&
            mediaDisconnectEvent->mediaID == m_stdMediaId) {
            onMediaDisconnected();
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::SaveChannelsEventType) {
        auto channelsEvent = std::static_pointer_cast<Wisenet::Core::SaveChannelsEvent>(serviceEvent);
        for (auto& channel : channelsEvent->channels) {
            if (channel.deviceID == m_stdDeviceId && channel.channelID == m_stdChannelId) {
                onChannelConfigChanged();
                break;
            }
        }
    }
}

void CoreServiceMediaControl::onVideoLossTriggered()
{
    if (m_mediaParam.isLive()) {
        SPDLOG_INFO("[LIVE] onVideoLossTriggered(), deviceID={}, channelID={}", m_stdDeviceId, m_stdChannelId);
    }
    else {
        SPDLOG_INFO("[PLAY] onVideoLossTriggered(), deviceID={}, channelID={}", m_stdDeviceId, m_stdChannelId);
    }

    // live인 경우에만 vloss에 대해서 media reopen
    if (m_mediaParam.isLive() && !m_isClosed && !m_isSleep) {
        startReopenTimer(false);
    }
}


/**
 * @brief CoreServiceMediaControl::newMediaFromCoreService
 * 미디어 데이터를 코어서비스로부터 받음
 * @param mediaSourceFrame
 * 비디오/오디오/메타
 */
void CoreServiceMediaControl::newMediaFromCoreService(const Wisenet::Media::MediaSourceFrameBaseSharedPtr &mediaSourceFrame)
{
    if (m_isClosed) {
        return;
    }

    if (m_isSleep) {
        //SPDLOG_DEBUG("sleep Mode ON!!!");
        return;
    }
#ifndef MEDIA_FILE_ONLY
    auto db = QCoreServiceManager::Instance().DB();
    if(db != nullptr && m_mediaRequestTime > 0 && !m_writeResponseTime)
    {
        int responseTime = (int)(QDateTime::currentMSecsSinceEpoch() - m_mediaRequestTime);
        db->countUpMediaResponseTime(responseTime);
        m_writeResponseTime = true;
    }
#endif
    constexpr size_t maxQueueLimit = 100;
    auto videoQsize = m_sourceFramesQueue->videoSize();
    if (videoQsize > maxQueueLimit) {
        STATLOG_WARN("VIDEO Queue overflow, clear queue, deviceID={}, channelID={}, totalQueueCnt={}, videoQueueCnt={}",
                     m_stdDeviceId, m_stdChannelId, m_sourceFramesQueue->size(), videoQsize);
        m_sourceFramesQueue->clear();
    }

    if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        auto videoFrame = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(mediaSourceFrame);
        //SPDLOG_DEBUG("newMediaFromCoreService(), ptsMsec={}, cSeq={}", videoFrame->frameTime.ptsTimestampMsec, videoFrame->commandSeq);

        if (!checkFrameCseq(videoFrame->commandSeq)) {
            SPDLOG_DEBUG("checkFrameCseq() Invalid frame, drop it, cSeq={}, lastSeq={}, compSeq={}, ptsMsec={}, device={}/{}",
                         videoFrame->commandSeq, m_lastCommandSeq, m_compareCommandSeq,
                         videoFrame->frameTime.ptsTimestampMsec, m_stdDeviceId, m_stdChannelId);
            return;
        }

        m_lastVideoFrameTimeMsec = videoFrame->frameTime.ptsTimestampMsec; // 마지막 비디오 수신시간 저장
        m_sourceFramesQueue->push(videoFrame);
    }
    else if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        auto audioFrame = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(mediaSourceFrame);
        m_sourceFramesQueue->push(audioFrame);
    }
    else if (mediaSourceFrame->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        auto metaFrame = std::static_pointer_cast<Wisenet::Media::MetadataSourceFrame>(mediaSourceFrame);
        m_sourceFramesQueue->push(metaFrame);
    }

    if(m_writer && m_writeStarted)
        m_writer->PushMediaSourceFrame(mediaSourceFrame);
}

//////////////////////////////////////////////////////////////////
/// 로컬레코딩 APIs
/////////////////////////////////////////////////////////////////
bool CoreServiceMediaControl::setLocalRecording(const bool recordingStart, QString filePath, QString fileName, int duration)
{
    if(recordingStart)
        return startLocalRecording(filePath, fileName, duration);
    else
        return stopLocalRecording();
}

void CoreServiceMediaControl::updatePlaybackStatus(const bool isPlay, const float speed)
{
    m_lastControlInfo.controlType = isPlay? Wisenet::Device::ControlType::play : Wisenet::Device::ControlType::pause;
    m_lastControlInfo.speed = speed;

    // CommandSequence와 Buffer도 함께 초기화
    // Pause -> 다른채널 선택 -> Play 시 Seq 차이로 영상 안나오는 문제 수정
    m_compareCommandSeq = m_lastCommandSeq;
    m_sourceFramesQueue->clear();
}


bool CoreServiceMediaControl::startLocalRecording(QString filePath, QString fileName, int duration)
{
    if(m_writeStarted)
        stopLocalRecording();

    if(m_isClosed || !m_mediaParam.isLive() || FileWriteManager::getInstance()->isLocalRecordingFull())
        return false;

    // 파일 확장자 체크 및 Writer 초기화
    int extensionIndex = fileName.lastIndexOf(".");
    if(extensionIndex == -1 || extensionIndex == fileName.length() - 1)
        return false;

    QString name = fileName.left(extensionIndex);
    QString extension = fileName.right(fileName.length() - extensionIndex - 1);
    if(extension.toLower() == "mkv")
        m_writer.reset(new FFmpegFileWriter(std::make_shared<MediaSourceFrameQueue>()));
    else if(extension.toLower() == "wnm")
        m_writer.reset(new WnmFileWriter(std::make_shared<MediaSourceFrameQueue>()));
    else
        return false;

    // 파일명에 IP 추가
    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->FindDevice(m_stdDeviceId, device);
#endif
    fileName = name + "(" + QString::fromStdString(device.connectionInfo.ip) + ")." + extension;

    FileWriteManager::getInstance()->increaseLocalRecordingCount();    

    if(m_writer->GetWriterType() == FileWriterBase::WriterType::FFmpeg)
    {
        FFmpegFileWriter* ffmpegWriter = (FFmpegFileWriter*)m_writer.data();
        ffmpegWriter->SetOutFileInfo(filePath, fileName);
    }
    else if(m_writer->GetWriterType() == FileWriterBase::WriterType::Wnm)
    {
        WnmFileWriter* wnmWriter = (WnmFileWriter*)m_writer.data();
        WnmInitalizeParameter initParam;
        Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().DB()->FindChannel(m_stdDeviceId, m_stdChannelId, channel);
#endif
        initParam.channelName = QString::fromStdString(channel.name);
        wnmWriter->SetInitializeParameter(filePath, fileName, initParam);
    }

    if(duration > 0)
    {
        m_writeDurationTimer.reset(new QTimer());
        m_writeDurationTimer->setInterval(duration * 60000);
        connect(m_writeDurationTimer.data(), &QTimer::timeout, this, &CoreServiceMediaControl::onWriteDurationTimerTimeout, Qt::QueuedConnection);
        m_writeDurationTimer->start();
    }
    m_writeStarted = true;

    return m_writeStarted;
}

bool CoreServiceMediaControl::stopLocalRecording()
{
    if(m_writeStarted) {
        FileWriteManager::getInstance()->decreaseLocalRecordingCount();
        m_writeStarted = false;
        emit localRecordingTimeout();
    }

    if(m_writer.data() != nullptr)
        m_writer.reset();

    if(m_writeDurationTimer.data() != nullptr)
        m_writeDurationTimer.reset();

    return m_writeStarted;
}


void CoreServiceMediaControl::onWriteDurationTimerTimeout()
{
    stopLocalRecording();
}

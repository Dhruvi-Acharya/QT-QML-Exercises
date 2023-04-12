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
#include "CoreServiceFrameSource.h"
#include "QCoreServiceManager.h"
#include "WeakCallback.h"
#include "LogSettings.h"

#include "VideoDecoder.h"
#include "AudioDecoder.h"

#undef SPDLOG_LOGGER_CALL
#define SPDLOG_LOGGER_CALL(logger, level, ...) (logger)->log(spdlog::source_loc{__FILE__, __LINE__, "[CSFrameSource] "}, level, __VA_ARGS__)

#define HW_DECODER_TEST 1
CoreServiceFrameSource::CoreServiceFrameSource(SourceFrameQueueSharedPtr sourceFramesQueue,
                                               bool isSequence,
                                               QObject *parent)
    : FrameSourceBase(parent)
    , m_sourceFramesQueue(sourceFramesQueue)
    , m_videoDecoder(new VideoDecoder(nullptr, false, isSequence))
    , m_audioDecoder(new AudioDecoder())
    , m_captureDecoder(new VideoDecoder(nullptr, true, true))
    , m_deviceStreamType(Wisenet::Device::StreamType::none)
    , m_isClosed(true)
{
    start();
    SPDLOG_DEBUG("CoreServiceFrameSource() THREAD START. bufferingMsec={}", m_liveBufferingUsec/1000);
    m_checkTimePoint = std::chrono::steady_clock::now();
}

CoreServiceFrameSource::~CoreServiceFrameSource()
{
    stop();
    qDeleteAll(m_ptzPresets);
    m_ptzPresets.clear();
    m_lastVideoFramesList.clear();
    SPDLOG_DEBUG("~CoreServiceFrameSource() THREAD END");
}

void CoreServiceFrameSource::updateChannel(const Wisenet::Device::Device::Channel &channel)
{
    m_channel = channel;

    QString mediaName = QString::fromUtf8(channel.name.c_str());
    bool supportDualStreamLive = (channel.lowProfile != channel.highProfile);
    bool supportDualStreamPlayback = channel.mediaCapabilities.recordingDualTrack;
    bool supportPanTilt = channel.ptzCapabilities.continousPanTilt || channel.ptzEnable;
    bool supportZoom = channel.ptzCapabilities.continousZoom || channel.ptzEnable;
    bool supportZoom1x = supportZoom; // todo
    bool supportAreaZoom = channel.ptzCapabilities.areaZoom;
    bool supportPreset = channel.ptzCapabilities.preset;
    bool supportSwing = channel.ptzCapabilities.swing;
    bool supportGroup = channel.ptzCapabilities.group;
    bool supportTour = channel.ptzCapabilities.tour;
    bool supportTrace = channel.ptzCapabilities.trace;
    bool supportPtzHome = supportPreset; // todo
    bool supportLive = channel.mediaCapabilities.live;
    bool supportPlayback = channel.mediaCapabilities.playback;
    bool supportBackup = channel.mediaCapabilities.backup;
    bool supportSimpleFocus = channel.imageCapabilities.simpleFocus;
    bool supportAutoFocus = channel.imageCapabilities.autoFocus;
    bool supportResetFocus = channel.imageCapabilities.resetFocus;
    bool supportDefog = channel.imageCapabilities.defog;
    bool supportIris = channel.ptzCapabilities.continousIris;
    bool supportFocus = channel.ptzCapabilities.continousFocus;
    bool enablePtz = channel.ptzEnable;
    bool supportFisheyeDewarp = channel.fisheyeSettings.fisheyeEnable;
    bool supportMic = channel.mediaCapabilities.audioOut;
    bool supportSmartSearch = channel.mediaCapabilities.smartSearch;
    bool supportSmartSearchFilter = channel.mediaCapabilities.smartSearchAiFilter;

    SPDLOG_DEBUG("CoreServiceFrameSource::updateChannel -{} - audioOut:{} supportLive={} supportPlayback={}",
                 channel.channelID, channel.mediaCapabilities.audioOut, supportLive, supportPlayback);

    setMediaName(mediaName);
    setSupportDualStreamPlayback(supportDualStreamPlayback);
    setSupportDualStreamLive(supportDualStreamLive);
    setSupportPanTilt(supportPanTilt);
    setSupportZoom(supportZoom);
    setSupportZoom1x(supportZoom1x);
    setSupportAreaZoom(supportAreaZoom);
    setSupportPreset(supportPreset);
    setSupportSwing(supportSwing);
    setSupportGroup(supportGroup);
    setSupportTour(supportTour);
    setSupportTrace(supportTrace);
    setSupportPtzHome(supportPtzHome);
    setSupportSimpleFocus(supportSimpleFocus);
    setSupportAutoFocus(supportAutoFocus);
    setSupportResetFocus(supportResetFocus);
    setSupportDefog(supportDefog);
    setSupportFocus(supportFocus);
    setSupportIris(supportIris);
    setSupportLive(supportLive);
    setSupportPlayback(supportPlayback);
    setSupportBackup(supportBackup);
    setEnablePtz(enablePtz);
    setSupportMic(supportMic);
    setSupportSmartSearch(supportSmartSearch);
    setSupportSmartSearchFilter(supportSmartSearchFilter);


    setSupportFisheyeDewarp(supportFisheyeDewarp);
    if (supportFisheyeDewarp) {
        QString lensType = QString::fromStdString(channel.fisheyeSettings.fisheyeLensType);
        setFisheyeLensType(lensType);
        Wisenet::FisheyeLensLocation lensLocation = channel.fisheyeSettings.fisheyeLensLocation;
        if (lensLocation == Wisenet::FisheyeLensLocation::Ceiling) {
            setFisheyeLocation((int)MediaLayoutItemViewModel::L_Ceiling);
        }
        else if (lensLocation == Wisenet::FisheyeLensLocation::Wall) {
            setFisheyeLocation((int)MediaLayoutItemViewModel::L_Wall);
        }
        else if (lensLocation == Wisenet::FisheyeLensLocation::Ground) {
            setFisheyeLocation((int)MediaLayoutItemViewModel::L_Ground);
        }
    }
    emit fisheyeParamChanged();

    m_videoDecoder->setMediaName(mediaName);
}

void CoreServiceFrameSource::updateUserPermission(const Wisenet::Core::UserGroup::Permission &permission)
{
    m_userPermission = permission;
    setPermissionPlayback(m_userPermission.playback);
    setPermissionLocalRecording(m_userPermission.localRecording);
    setPermissionPtzControl(m_userPermission.ptzControl);
    setPermissionAudioIn(m_userPermission.audio);
    setPermissionMic(m_userPermission.mic);
}

void CoreServiceFrameSource::setup(const Wisenet::Device::StreamType streamType)
{
    m_deviceStreamType = streamType;
}


void CoreServiceFrameSource::devicePtzContinuous(const int pan, const int tilt, const int zoom)
{
    SPDLOG_DEBUG("devicePtzContinuous(), name={}, pan={}, tilt={}, zoom={}", m_mediaStdName, pan, tilt, zoom);
    Wisenet::Device::DevicePTZContinuousRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DevicePTZContinuousRequest>();

    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->pan = pan;
    reqParam->tilt = tilt;
    reqParam->zoom = zoom;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DevicePTZContinuous,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzStop()
{
    SPDLOG_DEBUG("devicePtzStop(), name={}", m_mediaStdName);
    Wisenet::Device::DevicePTZStopRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DevicePTZStopRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DevicePTZStop,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzPresetGet()
{
    SPDLOG_DEBUG("devicePtzPresetGet(), name={}", m_mediaStdName);
    Wisenet::Device::DeviceGetPresetRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceGetPresetRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetPreset,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetPresetResponse>(reply->responseDataPtr);
        if (response->isSuccess()) {
            qDeleteAll(m_ptzPresets);
            m_ptzPresets.clear();

            for (auto& kv : response->presets) {
                PtzPresetItemModel* itemModel = new PtzPresetItemModel(this);
                unsigned int number = kv.first;
                QString name = QString::fromStdString(kv.second);
                itemModel->setPresetName(name);
                itemModel->setPresetNo((int)number);
                m_ptzPresets << itemModel;
            }
            setMaxPtzPresets(response->maxPreset);
            emit ptzPresetsChanged();
            SPDLOG_DEBUG("devicePtzPresetGet() preset list, name={}, max={}, count={}", m_mediaStdName, response->maxPreset, response->presets.size());
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzPresetGet() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzPresetMove(const int presetNumber)
{
    SPDLOG_DEBUG("devicePtzPresetMove(), name={} number={}", m_mediaStdName, presetNumber);
    Wisenet::Device::DeviceMovePresetRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMovePresetRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->preset = presetNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMovePreset,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzPresetDelete(const int presetNumber)
{
    SPDLOG_DEBUG("devicePtzPresetDelete(), name={} number={}", m_mediaStdName, presetNumber);
    Wisenet::Device::DeviceRemovePresetRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceRemovePresetRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->preset = presetNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceRemovePreset,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isSuccess()) {
            // udpate preset list
            devicePtzPresetGet();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzPresetDelete() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzPresetAdd(const int presetNumber, const QString &presetName)
{
    Wisenet::Device::DeviceAddPresetRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceAddPresetRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->preset = presetNumber;
    reqParam->name = presetName.toStdString();
    SPDLOG_DEBUG("devicePtzPresetAdd(), name={}, number={}, name={}", m_mediaStdName,presetNumber, reqParam->name);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceAddPreset,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if (response->isSuccess()) {
            // udpate preset list
            devicePtzPresetGet();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzPresetAdd() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

// swing
void CoreServiceFrameSource::devicePtzSwingGet()
{
    SPDLOG_DEBUG("devicePtzSwingGet() devID={} chID={}",m_channel.deviceID,m_channel.channelID);
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetSwing,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetSwingResponse>(reply->responseDataPtr);
        SPDLOG_DEBUG("devicePtzSwingGet() result={} panStart={} panEnd={} pantiltStart={} pantiltEnd={}", response->isSuccess(), response->swingPanStartPreset, response->swingPanEndPreset, response->swingPanTiltStartPreset, response->swingPanTiltEndPreset);

        if (response->isSuccess()) {
            m_swingPanStartPreset = response->swingPanStartPreset;
            m_swingPanEndPreset = response->swingPanEndPreset;
            m_swingTiltStartPreset = response->swingTiltStartPreset;
            m_swingTiltEndPreset = response->swingTiltEndPreset;
            m_swingPanTiltStartPreset = response->swingPanTiltStartPreset;
            m_swingPanTiltEndPreset = response->swingPanTiltEndPreset;

            emit swingPanStartPresetChanged();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzSwingGet() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzSwingMoveStop(const Wisenet::Device::SwingActionType swingActionType)
{
    SPDLOG_DEBUG("devicePtzSwingMoveStop() swingActionType={}",swingActionType);
    Wisenet::Device::DeviceMoveStopSwingRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMoveStopSwingRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->swingActionType = swingActionType;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMoveStopSwing,
                this, reqParam, nullptr);
#endif
}

// group
void CoreServiceFrameSource::devicePtzGroupGet()
{
    SPDLOG_DEBUG("devicePtzGroupGet() devID={} chID={}",m_channel.deviceID,m_channel.channelID);
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetGroup,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetGroupResponse>(reply->responseDataPtr);
        SPDLOG_DEBUG("devicePtzGroupGet() result={}", response->isSuccess());

        if (response->isSuccess()) {
            m_groupPresets.clear();

            SPDLOG_DEBUG("devicePtzGroupGet() response->groupPresets.size:{}", response->groupPresets.size());

            for(auto& group : response->groupPresets) {
                SPDLOG_DEBUG("devicePtzGroupGet() groupNumber={}",group.first);
                int groupNumber = group.first;
                for(auto& preset : group.second) {
                    SPDLOG_DEBUG("devicePtzGroupGet() presetNumber={}",preset);
                    m_groupPresets[groupNumber].append(preset);
                }

                // group 정보 요청 cgi를 보냈을 때 그룹 번호만 보내는 장비에 대한 처리
                // cgi : /stw-cgi/ptzconfig.cgi?msubmenu=group&action=view&Channel=#
                if(group.second.empty()) {
                    QList<int> presetList;
                    m_groupPresets.insert(groupNumber, presetList);
                }
            }
            emit groupPresetsChanged();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzGroupGet() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzGroupMove(const int groupNumber)
{
    SPDLOG_DEBUG("devicePtzGroupMove() groupNumber={}",groupNumber);
    Wisenet::Device::DeviceMoveGroupRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMoveGroupRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->groupNumber = groupNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMoveGroup,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzGroupStop(const int groupNumber)
{
    SPDLOG_DEBUG("devicePtzGroupStop() groupNumber={}",groupNumber);
    Wisenet::Device::DeviceStopGroupRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceStopGroupRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->groupNumber = groupNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceStopGroup,
                this, reqParam, nullptr);
#endif
}

// tour
void CoreServiceFrameSource::devicePtzTourGet()
{
    SPDLOG_DEBUG("devicePtzTourGet() devID={} chID={}",m_channel.deviceID,m_channel.channelID);
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetTour,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetTourResponse>(reply->responseDataPtr);
        SPDLOG_DEBUG("devicePtzTourGet() result={}", response->isSuccess());

        if (response->isSuccess()) {
            m_tourPresets.clear();

            for(auto& group : response->tourPresets) {
                SPDLOG_DEBUG("devicePtzTourGet() groupNumber={}",group);
                m_tourPresets.append(group);
            }
            emit tourPresetsChanged();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzTourGet() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzTourMove()
{
    SPDLOG_DEBUG("devicePtzTourMove()");
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMoveTour,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzTourStop()
{
    SPDLOG_DEBUG("devicePtzTourStop()");
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceStopTour,
                this, reqParam, nullptr);
#endif
}

// trace
void CoreServiceFrameSource::devicePtzTraceGet()
{
    SPDLOG_DEBUG("devicePtzTraceGet() devID={} chID={}",m_channel.deviceID,m_channel.channelID);
    Wisenet::Device::ChannelRequestBaseSharedPtr reqParam =
            std::make_shared<Wisenet::Device::ChannelRequestBase>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetTrace,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetTraceResponse>(reply->responseDataPtr);
        SPDLOG_DEBUG("devicePtzTraceGet() result={}", response->isSuccess());

        if (response->isSuccess()) {
            m_tracePresets.clear();

            for(auto& group : response->tracePresets) {
                SPDLOG_DEBUG("devicePtzTraceGet() groupNumber={}",group);
                m_tracePresets.append(group);
            }
            emit tracePresetsChanged();
        }
        else if (response->isFailed())
        {
            SPDLOG_DEBUG("devicePtzTraceGet() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

void CoreServiceFrameSource::devicePtzTraceMove(const int traceNumber)
{
    SPDLOG_DEBUG("devicePtztraceMove()");
    Wisenet::Device::DeviceMoveTraceRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceMoveTraceRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->traceNumber = traceNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMoveTrace,
                this, reqParam, nullptr);
#endif
}

void CoreServiceFrameSource::devicePtzTraceStop(const int traceNumber)
{
    SPDLOG_DEBUG("devicePtztraceStop()");
    Wisenet::Device::DeviceStopTraceRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceStopTraceRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->traceNumber = traceNumber;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceStopTrace,
                this, reqParam, nullptr);
#endif
}

/* 0 : stop, 1: near, 2: far */
void CoreServiceFrameSource::deviceFocusContinuous(const int command)
{
    Wisenet::Device::DevicePTZFocusRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DevicePTZFocusRequest>();

    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->focus = Wisenet::Device::DevicePTZFocusRequest::Focus::Stop;
    if (command == 1) {
        reqParam->focus = Wisenet::Device::DevicePTZFocusRequest::Focus::Near;
    }
    else if (command == 2) {
        reqParam->focus = Wisenet::Device::DevicePTZFocusRequest::Focus::Far;
    }

    SPDLOG_DEBUG("deviceFocusContinuous(), name={}, command={}", m_mediaStdName, command);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DevicePTZFocus,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_DEBUG("deviceFocusModeControl() result={}", response->isSuccess());

        if (response->isFailed())
        {
            SPDLOG_DEBUG("deviceFocusModeControl() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}

/* 0 : simpleFocus, 1: reset, 2: autoFocus */
void CoreServiceFrameSource::deviceFocusModeControl(const int focusMode)
{
    Wisenet::Device::DeviceImageFocusModeRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceImageFocusModeRequest>();

    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->focusMode = Wisenet::Device::DeviceImageFocusModeRequest::FocusMode::SimpleFocus;
    if (focusMode == 1) {
        reqParam->focusMode = Wisenet::Device::DeviceImageFocusModeRequest::FocusMode::Reset;
    }
    else if (focusMode == 2) {
        reqParam->focusMode = Wisenet::Device::DeviceImageFocusModeRequest::FocusMode::AutoFocus;
    }

    SPDLOG_DEBUG("deviceFocusModeControl(), name={}, focusMode={}", m_mediaStdName, focusMode);

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceImageFocusMode,
                this, reqParam,
                [this](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_DEBUG("deviceFocusModeControl() result={}", response->isSuccess());

        if (response->isFailed())
        {
            SPDLOG_DEBUG("deviceFocusModeControl() - response->error:{}", response->errorString());

            switch (response->errorCode) {
            case Wisenet::ErrorCode::SystemMenuUsed:
                emit deviceUsingSetupChanged();
                break;
            default:
                break;
            }
        }
    });
#endif
}


/**
 * @brief CoreServiceFrameSource::loopTask
 * 쓰레드 매인 loop
 */
void CoreServiceFrameSource::loopTask()
{
    const int waitMsec = m_isSleep ? 16 : (m_isClosed ? 2 : 1);
    std::this_thread::sleep_for(std::chrono::milliseconds(waitMsec));

    auto now = std::chrono::steady_clock::now();

    m_lastTickGap = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastLoopPoint).count();
    m_lastLoopPoint = now;

    checkCommands(now);

    // do nothing when sleep
    if (m_isSleep) {
        //SPDLOG_DEBUG("sleep mode On!!");
        return;
    }

    if (m_isClosed) {
        //SPDLOG_DEBUG("is closed!!, name={}", m_mediaStdName);
        return;
    }

    checkFrameStat(now);

    Wisenet::Media::MediaSourceFrameBaseSharedPtr dataPtr = nullptr;
    if (!m_sourceFramesQueue->front(dataPtr)) {
        // video loading/loss 체크
        checkVideoLoss(now);
        return;
    }
    if (dataPtr->getMediaType() == Wisenet::Media::MediaType::VIDEO) {
        checkVideo(now, dataPtr);
    }
    else if (dataPtr->getMediaType() == Wisenet::Media::MediaType::AUDIO) {
        checkAudio(now, dataPtr);
    }
    else if (dataPtr->getMediaType() == Wisenet::Media::MediaType::META_DATA) {
        checkMeta(now, dataPtr);
    }
    else {
        /* unknown media type maybe meta */
        m_sourceFramesQueue->pop();
    }
}


void CoreServiceFrameSource::checkVideoLoss(const std::chrono::steady_clock::time_point &now)
{
    if (m_status != PlayStatus::Loaded &&
        m_status != PlayStatus::ReadyToLoaded &&
        m_status != PlayStatus::Loading) {
        return;
    }

    if (m_status == PlayStatus::Loaded && m_lastSpeed == 0.0f) {
        return;
    }

    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastVideoFrameClock).count();
    if (m_status == PlayStatus::ReadyToLoaded) {
        if (elapsedMsec > 500) {
            setStatus(PlayStatus::Loading);
            m_lastVideoFrameClock = now;
        }
    }
    if (m_status == PlayStatus::Loading || m_status == PlayStatus::Loaded) {
        // 저배속인 경우 speed값을 계산해서 wait time 비교한다.
        if ( (m_lastSpeed > 0 && m_lastSpeed < 1) || (m_lastSpeed < 0 && m_lastSpeed > -1)) {
            auto cvtElapsedMsec = std::abs((int)(m_lastSpeed*elapsedMsec));
            //SPDLOG_INFO("checkVideoLoss(), low speed, convert value, {}=>{}", elapsedMsec, cvtElapsedMsec);
            elapsedMsec = cvtElapsedMsec;
        }
        if (elapsedMsec > m_videoWaitMsec) {
            SPDLOG_DEBUG("checkVideoLoss(), VIDEO LOSS, elapsedMsec={}, waitMsec={}, mediaName={}",
                         elapsedMsec, m_videoWaitMsec, m_mediaStdName);
            setStatus(PlayStatus::Failed_Timedout);
            emit videoLossTriggered();
        }
    }
}

void CoreServiceFrameSource::checkVideo(
        const std::chrono::steady_clock::time_point &now,
        Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr)
{
    auto videoFramePtr = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(dataPtr);
    bool isKeyFrame = (videoFramePtr->videoFrameType == Wisenet::Media::VideoFrameType::I_FRAME);
    auto qSize = m_sourceFramesQueue->videoSize();

    // playback 영상의 키프레임이 들어왔을 때 full frame 재생 케이스가 아니면 nodelay play
    bool noDelay = (!m_isLive && isKeyFrame && (m_lastSpeed <= 0.0f || m_lastSpeed >= 4.0)) ? true : false;
    // PLAYBACK 또는 buffereing msec이 0인 경우에는 바로 영상을 처리하고 표시한다.
    bool playTime = (!m_isLive || m_liveBufferingUsec == 0);

    //SPDLOG_DEBUG("video timestamp={}, {}", videoFramePtr->frameTime.rtpTimestampMsec, videoFramePtr->frameTime.ptsTimestampMsec);

    // LIVE 영상균등성 ON 처리
    if (m_enableVideoOutDoubleBuffer) {
        int64_t currentPtsTimeStampMsec = videoFramePtr->frameTime.rtpTimestampMsec;
        playTime = calcPlayTime(currentPtsTimeStampMsec, qSize);
    }

    // Play시간이 아닌 경우 skip
    if (!playTime) {
        return;
    }

    // 디코딩
    // LIVE 영상균등성을 위해 playtime에 아웃버퍼에 저장된 데이터를 하나 올려준다.
    if (m_enableVideoOutDoubleBuffer && m_VideoOutBuffer) {
        //SPDLOG_DEBUG("VIDEO BUFFER ONVIDEO");
        onNewVideoFrame(m_VideoOutBuffer);
    }

    m_VideoOutBuffer.reset();
    m_sourceFramesQueue->pop();
    checkVideoStatus(videoFramePtr, now, m_videoDecoder->getMethod());

    std::string errMsg;
    auto res = m_videoDecoder->decodeStart(videoFramePtr, m_VideoOutBuffer,
                                           m_lastKeepOriginalSize,
                                           m_lastOutputSize.width(),
                                           m_lastOutputSize.height(),
                                           noDelay, errMsg);
    if (res == DecodeResult::Success) {
        if (m_VideoOutBuffer) {
            // Live 영상균등성 처리시에는 다음 playtime에 올려준다.
            if (!m_enableVideoOutDoubleBuffer) {
                onNewVideoFrame(m_VideoOutBuffer);
                m_VideoOutBuffer.reset();
            }
        }
        else {
            //SPDLOG_DEBUG("decode success, but frame may not be ready");
        }
    }
    else {
        auto elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_lastDecodeErrorClock).count();
        if (elapsedMsec > 5000) {
            SPDLOG_WARN("video decode failed, name={}, msg={}", m_mediaStdName, errMsg);
            m_lastDecodeErrorClock = now;
        }
    }

    // CUDA Decoder인 경우 마지막 I-Frame부터 다음 I-Frame까지 보관 (원본 이미지 캡쳐용)
    if(m_videoDecodeMethod == VideoDecodeMethod::CUVID) {
        if(isKeyFrame) {
            m_lastVideoFramesList.clear();
        }
        m_lastVideoFramesList.push_back(dataPtr);
    }
}

static int64_t calculateExpectedQueueSize(const int64_t absTimeStampGapUsec, const int64_t bufferingUsec)
{
    if (absTimeStampGapUsec > 0 && bufferingUsec >= absTimeStampGapUsec) {
        float fq = (float)bufferingUsec / absTimeStampGapUsec;
        return int64_t(fq+1.5); //1frame + 0.5
    }
    return 0;
}

//#define BUFFERING_TEST
/**
 * @brief CoreServiceFrameSource::calcPlayTime
 * 라이브 영상균등성 계산 루틴
 * @param currentTimeStamp
 * @param now
 * @param queueSize
 * @return
 */
bool CoreServiceFrameSource::calcPlayTime(const int64_t currentTimeStamp, const size_t queueSize)
{
    // now값은 정확한 시간 계산을 위해서 다시 체크한다.
    auto now = std::chrono::steady_clock::now();

    // reset timestamp
    if (m_lastVideoPtsTimestampMsec == AV_NOPTS_VALUE) {
        // 타임스탬프 초기화 루틴에서는 buffering msec값을 함께 계산해서 최초에 읽어들인 clock값을 조정한다.
        m_lastVideoClockGap = -1 * m_liveBufferingUsec;
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        SPDLOG_DEBUG("calcPlayTime(), reset timestamp value, return true, name={}, waitTime={}ms",
                     m_mediaStdName, m_liveBufferingUsec/1000);

        return false;
    }

    // 타임스탬프가 3초이상 차이가 나거나, 이전값보다 작은 경우 예외처리한다.
    if (currentTimeStamp >= (m_lastVideoPtsTimestampMsec+3000) ||
       (m_lastVideoPtsTimestampMsec > currentTimeStamp))
    {
        SPDLOG_DEBUG("calcPlayTime(), invalid timestamp or overflowed, name={}, last={}, now={}, diff={}",
                     m_mediaStdName, m_lastVideoPtsTimestampMsec, currentTimeStamp, (currentTimeStamp-m_lastVideoPtsTimestampMsec));
        m_lastVideoClockGap = 0;
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;
        return true;
    }

    auto absTimeStampGap = (currentTimeStamp-m_lastVideoPtsTimestampMsec) * 1000;
    auto elapsedMicrosec = std::chrono::duration_cast<std::chrono::microseconds>(now - m_lastVideoClock).count();
    auto calcElapsedMicrosec = elapsedMicrosec + m_lastVideoClockGap;

    if (calcElapsedMicrosec >= (absTimeStampGap)) {
        m_lastVideoClockGap = calcElapsedMicrosec - absTimeStampGap;
        if (m_lastVideoClockGap > absTimeStampGap) {
            if (absTimeStampGap > 0) {
                SPDLOG_DEBUG("calcPlayTime(), videoClockGap high::elapsed={}, calcElapsed={}, timeGap={}, clockGap={}, tickGap={}",
                             elapsedMicrosec, calcElapsedMicrosec, absTimeStampGap, m_lastVideoClockGap, m_lastTickGap);
            }
            m_lastVideoClockGap = absTimeStampGap;
        }
        m_lastVideoPtsTimestampMsec = currentTimeStamp;
        m_lastVideoClock = now;

#ifdef BUFFERING_TEST
        SPDLOG_INFO("[LIVE] calcPlayTime(), VIDEO PLAY, calcElapsedUsec={}, absTimeStampGapUsec={}, timestamp={}, qSize={}, clockGap={}",
                     calcElapsedMicrosec, absTimeStampGap, currentTimeStamp, queueSize, m_lastVideoClockGap);
#endif
        if (absTimeStampGap == 0)
            return true;

        // 앞 프레임이 있는 경우 큐를 체크하여 너무 많이 쌓여있거나, 비어있으면 시간계산 보정을 수행한다.
        int64_t expectedQueueSize = calculateExpectedQueueSize(absTimeStampGap, m_liveBufferingUsec);
        int64_t qGapCnt = (int64_t)queueSize - expectedQueueSize;

        // 쌓여 있으면, 조금 더 빨리 비운다.
        if (qGapCnt > 3) {
            int64_t jitterUsec = absTimeStampGap*(0.1);
            m_lastVideoClockGap += jitterUsec;
#ifdef BUFFERING_TEST
            SPDLOG_INFO("[LIVE] Consume MORE++ Expected QSize={}, real={}, timeStampGap={}, bufferingMsec={}, jitterUsec={}",
                         expectedQueueSize, queueSize, absTimeStampGap/1000, m_liveBufferingUsec/1000, jitterUsec);
#endif
        }
        // 비어 있으면, 조금 더 천천히 비운다.
        else if (queueSize == 1 || qGapCnt < -1) {
            int64_t jitterUsec = absTimeStampGap*(0.1);
            m_lastVideoClockGap -= jitterUsec;
            // 1초이상 대기하지는 않도록 하자.
            if (m_lastVideoClockGap < -1000000) {
                m_lastVideoClockGap = -1000000;
            }
#ifdef BUFFERING_TEST
            SPDLOG_INFO("[LIVE] Consume LESS-- Expected QSize={}, real={}, timeStampGap={}, bufferingMsec={}, jitterUsec={}",
                        expectedQueueSize, queueSize, absTimeStampGap/1000, m_liveBufferingUsec/1000, jitterUsec);
#endif
        }
        return true;
    }


    return false;
}

void CoreServiceFrameSource::checkAudio(const std::chrono::steady_clock::time_point &now,
                                        Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr)
{
    Q_UNUSED(now);
    m_sourceFramesQueue->pop();

    auto audioFramePtr = std::static_pointer_cast<Wisenet::Media::AudioSourceFrame>(dataPtr);
    checkAudioStatus(audioFramePtr);

    if (m_lastEnableAudioOut && m_lastSpeed == 1.0f) {
        AudioOutputFrameSharedPtr outAudio = nullptr;
        auto res = m_audioDecoder->decodeStart(audioFramePtr, outAudio);
        if (res == DecodeResult::Success && outAudio) {
            outAudio->setSourceID(m_sourceId);
            onNewAudioFrame(outAudio);
        }
    }
}

void CoreServiceFrameSource::checkMeta(const std::chrono::steady_clock::time_point& now,
                                       Wisenet::Media::MediaSourceFrameBaseSharedPtr& dataPtr)
{
    Q_UNUSED(now);
    m_sourceFramesQueue->pop();

    auto metaFramePtr = std::static_pointer_cast<Wisenet::Media::MetadataSourceFrame>(dataPtr);

    std::string metaData((char*)dataPtr->getDataPtr());
    auto metaFrame = std::make_shared<MetaFrame>(metaData, dataPtr->getDataSize());

    onNewMetaFrame((MetaFrameSharedPtr)metaFrame);
}

void CoreServiceFrameSource::open()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isOpenCommand = true;
    m_isCloseCommand = false;
}

void CoreServiceFrameSource::close()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isCloseCommand = true;
    m_isOpenCommand = false;
}

void CoreServiceFrameSource::setPlaySpeed(const float speed)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSetSpeedCommand = true;
    m_newSpeed = speed;
    setSpeed(speed);
}

void CoreServiceFrameSource::requestLastVideoFrame()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRequestLastVideoCommand = true;
}

void CoreServiceFrameSource::requestLastOriginalVideoImage()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isRequestLastOriginalVideoCommand = true;
}

void CoreServiceFrameSource::deviceTalk(const bool on)
{
#ifndef MEDIA_FILE_ONLY
    SPDLOG_DEBUG("deviceTalk(), deviceId={} channelID ={} on={}", m_channel.deviceID, m_channel.channelID, on);
    Wisenet::Device::DeviceTalkRequestSharedPtr reqParam =
            std::make_shared<Wisenet::Device::DeviceTalkRequest>();
    reqParam->deviceID = m_channel.deviceID;
    reqParam->channelID = m_channel.channelID;
    reqParam->talkStart = on;
    //auto callback = sendAudioTalk();

   QCoreServiceManager::Instance().RequestToCoreService(
               &Wisenet::Core::ICoreService::DeviceTalk,
               this, reqParam, [this, on, reqParam](const QCoreServiceReplyPtr& reply)
   {
       auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
       SPDLOG_DEBUG("CoreServiceFrameSource deviceTalk, isSuccess={} error={}",
                    response->isSuccess(), response->errorString());

       if(!response->isSuccess())
            emit deviceTalkResult(reqParam->talkStart ,response->isSuccess(),1);
       if(response->isSuccess()){
           bool ret = AudioInputManager::Instance()->setStatus(m_channel.deviceID, m_channel.channelID, on);
           SPDLOG_DEBUG("CoreServiceFrameSource::deviceTalk AudioInput={}",ret);
           if(!ret)
               emit deviceTalkResult(reqParam->talkStart ,false, 2);
           else
               emit deviceTalkResult(reqParam->talkStart ,response->isSuccess(),0);
       }

   });
#endif
}

void CoreServiceFrameSource::seek()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSeekCommand = true;
}

void CoreServiceFrameSource::play(const float lastSpeed)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPlayCommand = true;
    m_newSpeed = lastSpeed;
    setSpeed(lastSpeed);
}

void CoreServiceFrameSource::pause()
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isPauseCommand = true;
}

void CoreServiceFrameSource::step(const bool isForward)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isStepCommand = true;
}

void CoreServiceFrameSource::sleep(const bool isOn)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    m_isSleepCommand = true;
    m_newSleepStatus = isOn;
}

void CoreServiceFrameSource::setOutputSize(const QSize size)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_outputSize != size) {
        m_outputSize = size;
    }
}

void CoreServiceFrameSource::setKeepOriginalSize(const bool keep)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (m_keepOriginalSize != keep) {
        m_keepOriginalSize = keep;
    }
}

void CoreServiceFrameSource::updatePlaybackStatus(const bool isPlay, const float speed, const bool playWait)
{
    const std::lock_guard<std::mutex> lock(m_mutex);
    if (!isPlay) {
        m_newSpeed = 0.0f;
    }
    else {
        m_newSpeed = speed;
    }
    m_newPlayWait = playWait;
    m_isUpdatePlaybackStatusCommand = true;
}

void CoreServiceFrameSource::_open(const std::chrono::steady_clock::time_point& now)
{
    SPDLOG_DEBUG("_open(), name={}, isClosed={}", m_mediaStdName, m_isClosed);
    if (!m_isClosed) {
        _close();
    }
    m_isClosed = false;

    m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;

    m_isLive = (m_deviceStreamType == Wisenet::Device::StreamType::liveHigh || m_deviceStreamType == Wisenet::Device::StreamType::liveLow);

#ifndef MEDIA_FILE_ONLY
    auto setting = QCoreServiceManager::Instance().Settings();

    if (setting != nullptr)
    {
        m_liveBufferingUsec = setting->liveBufferLength() * (int64_t)1000; //usec (msec*1000)

        if (m_isLive) {
            m_playTimeoutMsec = setting->liveVideoLossTimeout();
        }
        else {
            m_playTimeoutMsec = setting->playVideoLossTimeout();
        }
    }
#endif

    m_enableVideoOutDoubleBuffer = (m_isLive && m_liveBufferingUsec > 0);

    m_VideoOutBuffer = nullptr;

    resetFrameStat();

    m_videoWaitMsec = m_openTimeoutMsec;
    setStatus(PlayStatus::Loading);
    m_lastVideoFrameClock = now;
    SPDLOG_DEBUG("_open() finished, name={}, isLive={}, bufferingUsec={}, playTimeoutMsec={}",
                 m_mediaStdName, m_isLive, m_liveBufferingUsec, m_playTimeoutMsec);
}

void CoreServiceFrameSource::_close()
{
    SPDLOG_DEBUG("_close(), name={}", m_mediaStdName);
    // 디코더를 삭제한다.
    if (m_videoDecoder)
        m_videoDecoder->release();
    if (m_audioDecoder)
        m_audioDecoder->release();
    if (m_captureDecoder)
        m_captureDecoder->release();

    SPDLOG_DEBUG("CoreServiceFrameSource::_close() devId={} channelId={}",
                 m_channel.deviceID, m_channel.channelID);
    if(AudioInputManager::Instance()->getStatus(m_channel.deviceID, m_channel.channelID)){
        SPDLOG_DEBUG("oreServiceFrameSource::_close() AudioInputManager::Instance()->getStatus true");
        deviceTalk(false);
    }
    m_lastVideoFramesList.clear();

    resetFrameStat();

    m_lastVideoPtsTimestampMsec = AV_NOPTS_VALUE;
    m_VideoOutBuffer = nullptr;
    m_isClosed = true;
}

void CoreServiceFrameSource::checkCommands(const std::chrono::steady_clock::time_point& now)
{
    const std::lock_guard<std::mutex> lock(m_mutex);

    if (m_lastEnableAudioOut != m_enableAudioOut) {
        SPDLOG_DEBUG("Audio Enable Status Changed, Name={}, before={}, now={}",
                     m_mediaStdName, m_lastEnableAudioOut, m_enableAudioOut);
        m_lastEnableAudioOut = m_enableAudioOut;
    }
    if (m_outputSize != m_lastOutputSize) {
        SPDLOG_DEBUG("Video Output Status Changed, Name={}, before={}x{}, now={}x{}",
                     m_mediaStdName,
                     m_lastOutputSize.width(), m_lastOutputSize.height(),
                     m_outputSize.width(), m_outputSize.height());
        m_lastOutputSize = m_outputSize;
    }
    if (m_keepOriginalSize != m_lastKeepOriginalSize) {
        m_lastKeepOriginalSize = m_keepOriginalSize;
    }

    if (m_isOpenCommand) {
        m_isOpenCommand = false;
        _open(now);
    }

    if (m_isCloseCommand) {
        m_isCloseCommand = false;
        _close();
    }

    bool needToReset = false;
    int newStatus = PlayStatus::ReadyToLoaded;
    if (m_isSetSpeedCommand) {
        needToReset = true;
        m_isSetSpeedCommand = false;
        m_lastSpeed = m_newSpeed;
    }
    if (m_isSeekCommand) {
        needToReset = true;
        m_isSeekCommand = false;
    }
    if (m_isPauseCommand) {
        needToReset = true;
        m_isPauseCommand = false;
        m_lastSpeed = 0.0f;
        newStatus = m_status; // last status when paused
    }
    if (m_isPlayCommand) {
        needToReset = true;
        m_isPlayCommand = false;
        m_lastSpeed = m_newSpeed;
    }
    if (m_isStepCommand) {
        needToReset = true;
        m_isStepCommand = false;
        newStatus = m_status; // last status when paused
    }
    if (m_isUpdatePlaybackStatusCommand) {
        needToReset = true;
        m_isUpdatePlaybackStatusCommand = false;
        m_lastSpeed = m_newSpeed;
        if (!m_newPlayWait) {
            newStatus = m_status; // last status when new playwait
        }
    }
    if (m_isSleepCommand) {
        m_isSleepCommand = false;
        if (m_isSleep != m_newSleepStatus) {
            m_isSleep = m_newSleepStatus;
        }
    }

    if (needToReset) {
        resetPlay(newStatus, now);
    }

    // 마지막 영상 요청
    if (m_isRequestLastVideoCommand) {
        m_isRequestLastVideoCommand = false;
        SPDLOG_DEBUG("Last Video Frame requested!");
        onLastVideoFrame();
    }
    if (m_isRequestLastOriginalVideoCommand) {
        m_isRequestLastOriginalVideoCommand = false;
        getLastOriginalVideoImage();
    }
}

void CoreServiceFrameSource::checkVideoStatus(const Wisenet::Media::VideoSourceFrameSharedPtr &videoFrame,
                                              const std::chrono::steady_clock::time_point &now,
                                              const VideoDecodeMethod decodeMethod)
{
    if (videoFrame->videoWidth != m_videoSize.width() || videoFrame->videoHeight != m_videoSize.height()) {
        SPDLOG_DEBUG("Resolution changed, Name={}, {}x{}", m_mediaStdName, videoFrame->videoWidth, videoFrame->videoHeight);
        setVideoSize(videoFrame->videoWidth, videoFrame->videoHeight);
    }
    if (videoFrame->videoCodecType != m_videoCodec || m_videoDecodeMethod != decodeMethod) {
        m_videoCodec = videoFrame->videoCodecType;
        m_videoDecodeMethod = decodeMethod;
        setVideoCodec(QString::fromStdString(makeVideoCodecDescription(m_videoCodec, decodeMethod)));
    }
    m_frameCheckCount++;
    m_frameCheckBytes += videoFrame->getDataSize();

    m_lastVideoFrameClock = now;
    if (m_status != PlayStatus::Loaded) {
        setStatus(PlayStatus::Loaded);
        m_videoWaitMsec = m_playTimeoutMsec;
    }
}

void CoreServiceFrameSource::checkAudioStatus(const Wisenet::Media::AudioSourceFrameSharedPtr &audioFrame)
{
    if (audioFrame->audioCodecType != m_audioCodec) {
        m_audioCodec = audioFrame->audioCodecType;
        QString codecName = QString::fromStdString(Wisenet::Media::AudioCodecTypeString(m_audioCodec));
        setAudioCodec(codecName);
    }
}

void CoreServiceFrameSource::checkFrameStat(const std::chrono::steady_clock::time_point &now)
{
    int elapsedMsec = std::chrono::duration_cast<std::chrono::milliseconds>(now - m_checkTimePoint).count();
    if (elapsedMsec >= 1000) {
        // calculate frame rate and bit rate
        float tick = (float)(elapsedMsec)/1000;
        float fps = (float)m_frameCheckCount/tick;
        float kbps = (float)m_frameCheckBytes/tick*8/1000;
        size_t bufferingCount = m_sourceFramesQueue->size();

        STATLOG_DEBUG("Device Frame Stat::name={}, tick={:.2f}, fps={:.2f}, kbps={:.2f}, frames={}, bytes={}, bufferingCount={}",
                      m_mediaStdName, tick, fps, kbps, m_frameCheckCount, m_frameCheckBytes, bufferingCount);

        setVideoFps(fps);
        setVideoKbps(kbps);
        if (m_isLive) {
            setVideoBufferingCount((int)bufferingCount);
        }

        m_frameCheckCount = 0;
        m_frameCheckBytes = 0;
        m_checkTimePoint = now;
    }
}

void CoreServiceFrameSource::resetFrameStat()
{
    m_frameCheckCount = 0;
    m_frameCheckBytes = 0;
    m_checkTimePoint = std::chrono::steady_clock::now();
}


void CoreServiceFrameSource::resetPlay(const int newStatus,
                                       const std::chrono::steady_clock::time_point &now)
{
    // buffer clear
    if (m_videoDecoder)
        m_videoDecoder->flushBuffers();
    if (m_audioDecoder)
        m_audioDecoder->flushBuffers();
    if (m_captureDecoder)
        m_captureDecoder->flushBuffers();

    m_status = newStatus;
    m_videoWaitMsec = m_playTimeoutMsec;

    m_lastVideoFrameClock = now;
}

void CoreServiceFrameSource::getLastOriginalVideoImage()
{
    VideoOutputFrameSharedPtr videoOutputPtr = nullptr;

    if(m_videoDecodeMethod == VideoDecodeMethod::CUVID) {
        // CUDA 디코더이면 m_lastVideoFramesList를 원본사이즈로 다시 디코딩.
        for(auto& framePtr : m_lastVideoFramesList) {
            auto videoFramePtr = std::static_pointer_cast<Wisenet::Media::VideoSourceFrame>(framePtr);
            VideoOutputFrameSharedPtr tempOutPtr = nullptr;
            std::string errMsg;
            auto res = m_captureDecoder->decodeStart(videoFramePtr, tempOutPtr, true,
                                                   m_lastOutputSize.width(),
                                                   m_lastOutputSize.height(),
                                                   true, errMsg);
            if (res == DecodeResult::Success && tempOutPtr) {
                //SPDLOG_DEBUG("CaptureDecode succeed");
                videoOutputPtr = tempOutPtr;
            }
            else {
                //SPDLOG_DEBUG("CaptureDecode failed or not ready");
            }
        }
        m_captureDecoder->flushBuffers();
    }
    else {
        // 나머지 디코더는 디코더에서 마지막 원본사이즈 프레임을 받아 옴.
        videoOutputPtr = m_videoDecoder->getLastOriginalVideoImage();
    }

    if(!videoOutputPtr) {
        emit lastOriginalVideoImage(QImage());
        return;
    }

    auto videoBuffer = new WisenetVideoBuffer();
    videoBuffer->moveDataFrom(videoOutputPtr);
    QSize videoSize(videoOutputPtr->width(), videoOutputPtr->height());
    QVideoFrame frame(videoBuffer, videoSize, videoBuffer->pixelFormat());
    QImage image = frame.image();

    emit lastOriginalVideoImage(image);
}


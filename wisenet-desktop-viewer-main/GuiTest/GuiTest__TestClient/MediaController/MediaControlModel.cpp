/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/

#include <QQmlEngine>
#include "QCoreServiceManager.h"
#include "MediaController/MediaControlModel.h"
#include "LogSettings.h"

MediaControlModel::MediaControlModel()
{
    m_recordingStartTime = 1612224000000;
    m_recordingEndTime = 1617666109000;
    m_visibleStartTime = 1612224000000;
    m_visibleEndTime = 1617666109000;
    m_visibleTimeRange = m_visibleEndTime - m_visibleStartTime;
}

MediaControlModel::~MediaControlModel()
{
}

void MediaControlModel::registerQml()
{
    qmlRegisterType<MediaControlModel>("Wisenet.Qmls", 0, 1, "MediaControlModel");
}

void MediaControlModel::requestDeviceMediaControl(qControlType type)
{
    auto mediaControlRequest = std::make_shared<Wisenet::Device::DeviceMediaControlRequest>();
    mediaControlRequest->deviceID;
    mediaControlRequest->mediaID;
    mediaControlRequest->mediaControlInfo.controlType = convertMediaControlType(type);
    mediaControlRequest->mediaControlInfo.speed;
    mediaControlRequest->mediaControlInfo.time;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceMediaControl,
                this, mediaControlRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        SPDLOG_INFO("MediaControl, isFailed={}", response->isFailed());
    });
}

Wisenet::Device::ControlType MediaControlModel::convertMediaControlType(qControlType type)
{
    if(type == qControlType::pause)
        return Wisenet::Device::ControlType::pause;
    else if(type == qControlType::play)
        return Wisenet::Device::ControlType::play;
    else if(type == qControlType::stepForward)
        return Wisenet::Device::ControlType::stepForward;
    else if(type == qControlType::stepBackward)
        return Wisenet::Device::ControlType::stepBackward;
    else
        return Wisenet::Device::ControlType::play;
}

void MediaControlModel::setVisibleTimeRange(qulonglong startTimeDelta, qulonglong rangeDelta)
{
    if(m_visibleTimeRange + rangeDelta < 0)
        return; // 시간 역전 발생 시 리턴

    qulonglong recordingRange = m_recordingEndTime - m_recordingStartTime;

    qulonglong visibleTimeRange = m_visibleTimeRange;
    visibleTimeRange += rangeDelta;
    if(visibleTimeRange > recordingRange)
        visibleTimeRange = recordingRange;    // 표시 영역이 전체 녹화 구간을 넘지 않도록 함

    // 표시 시작 시간
    m_visibleStartTime += startTimeDelta;
    if(m_visibleStartTime < m_recordingStartTime)
        m_visibleStartTime = m_recordingStartTime;

    // 표시 끝 시간
    m_visibleEndTime = m_visibleStartTime + visibleTimeRange;
    if(m_visibleEndTime > m_recordingEndTime)
    {
        m_visibleEndTime = m_recordingEndTime;
        m_visibleStartTime = m_visibleEndTime - visibleTimeRange;
    }

    m_visibleTimeRange = visibleTimeRange;

    emit visibleTimeRangeChanged(visibleTimeRange);
}

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

#include <QQmlEngine>
#include "QCoreServiceManager.h"
#include "MediaControlModel.h"
#include "LogSettings.h"
#include "DigestAuthenticator.h"

MediaControlModel::MediaControlModel()
{
    resetTimeline();

#ifndef MEDIA_FILE_ONLY
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered, this, &MediaControlModel::onCoreServiceEventTriggered, Qt::QueuedConnection);
#endif
}

MediaControlModel::~MediaControlModel()
{
}

void MediaControlModel::registerQml()
{
    qmlRegisterType<MediaControlModel>("Wisenet.Qmls", 0, 1, "MediaControlModel");
}

void MediaControlModel::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    if (event->eventDataPtr->EventTypeId() == Wisenet::Device::DeviceStatusEventType)
    {
        Wisenet::Core::UserGroup usergroup;
#ifndef MEDIA_FILE_ONLY
        QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);
        if(!usergroup.userPermission.playback)
            return; // playback 권한이 없는 사용자면 타임라인 관련 동작 하지 않고 리턴
#endif
        auto statusEvent = std::static_pointer_cast<Wisenet::Device::DeviceStatusEvent>(event->eventDataPtr);
        if(statusEvent->deviceStatus.isGood())
        {
            // 선택 장비 재접속 되면 전체 타임라인 리로드
            if(m_selectedDeviceId == statusEvent->deviceID)
            {
                QList<QString> channelIdList;
                channelIdList.push_back(selectedChannelID());
                getAllPeriodTimeline(selectedDeviceID(), channelIdList, 0);
            }
        }
        else
        {
            // 장비 접속 끊어질 때 타임라인 삭제
            std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(statusEvent->deviceID);
            if (deviceIter != m_deviceTimeline.end())
            {
                m_deviceTimeline.erase(statusEvent->deviceID);
            }

            // 선택 장비 접속 끊어지면 타임라인 clear signal
            if(m_selectedDeviceId == statusEvent->deviceID)
            {
                emit selectedTimelineUpdated(std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>(),
                                             std::vector<int>());
            }
        }
    }
}

void MediaControlModel::setTimelineStartTime(qulonglong time)
{
    QDateTime minimumStartTime = QDateTime::fromMSecsSinceEpoch(m_recordingStartTime).addDays(-1);
    minimumStartTime.setTime(QTime(0,0,0,0));   // minimumStartTime은 녹화 시작 하루 전의 0시
    if(time < minimumStartTime.toMSecsSinceEpoch())
        time = minimumStartTime.toMSecsSinceEpoch();

    if(m_timelineStartTime != time) {
        m_timelineStartTime = time;
        emit timelineStartTimeChanged();
    }
}

void MediaControlModel::setTimelineEndTime(qulonglong time)
{
    QDateTime maximumEndTime = QDateTime::fromMSecsSinceEpoch(m_recordingEndTime).addDays(2);
    maximumEndTime.setTime(QTime(0,0,0,0));   // maximumEndTime 녹화 끝 다음 날의 24시
    if(time > maximumEndTime.toMSecsSinceEpoch())
        time = maximumEndTime.toMSecsSinceEpoch();

    if(m_timelineEndTime != time) {
        m_timelineEndTime = time;
        emit timelineEndTimeChanged();
    }
}

void MediaControlModel::setvisibleStartTime(qulonglong time)
{
    m_visibleStartTime = time;
}

void MediaControlModel::setvisibleEndTime(qulonglong time)
{
    m_visibleEndTime = time;
}

void MediaControlModel::setVisibleTimeRange(qlonglong startTimeDelta, qlonglong rangeDelta)
{
    if(m_visibleTimeRange + rangeDelta < 0)
        return; // 시간 역전 발생 시 리턴

    qlonglong maximunRange = m_timelineEndTime - m_timelineStartTime;
    if(maximunRange > (qlonglong)365*24*3600*1000)
        maximunRange = (qlonglong)365*24*3600*1000;    // 최대 365일 범위까지 표시하도록 제한

    qlonglong visibleTimeRange = m_visibleTimeRange;
    visibleTimeRange += rangeDelta;
    if(visibleTimeRange > maximunRange)
        visibleTimeRange = maximunRange;  // 표시 영역이 최대 범위를 넘지 않도록 함
    else if(visibleTimeRange < 10000)
        visibleTimeRange = 10000;   // 10초 미만으로 표시되지 않도록 함

    // 표시 시작 시간
    m_visibleStartTime += startTimeDelta;
    if(m_visibleStartTime < m_timelineStartTime)
        m_visibleStartTime = m_timelineStartTime;

    // 표시 끝 시간
    m_visibleEndTime = m_visibleStartTime + visibleTimeRange;
    if(m_visibleEndTime > m_timelineEndTime)
    {
        m_visibleEndTime = m_timelineEndTime;
        m_visibleStartTime = m_visibleEndTime - visibleTimeRange;
    }

    m_visibleTimeRange = visibleTimeRange;

    emit visibleTimeRangeChanged(visibleTimeRange);
}

void MediaControlModel::getAllPeriodTimeline(QString deviceId, QList<QString> channelIdList, qint64 currentTime)
{
    auto request = std::make_shared<Wisenet::Device::DeviceGetAllPeriodTimelineRequest>();
    request->deviceID = deviceId.toStdString();
    for(auto& channel : channelIdList)
    {
        request->ChannelIDList.push_back(channel.toStdString());
    }

    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    if(QCoreServiceManager::Instance().DB()->FindDevice(request->deviceID, device))
    {
        if(device.deviceCapabilities.recordingSearchPeriod == false)
        {
            if(currentTime == 0)
                return;
            request->Month = currentTime;
            request->Period = 3;
        }
        else
        {
            if(currentTime != 0)
                return;
        }
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetAllPeriodTimeline, this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetAllPeriodTimelineResponse>(reply->responseDataPtr);
        if(response->isSuccess())
        {
            // save device timeline
            if(response->recordingPeriod.startTime == 0) {
                // 녹화 구간이 하나도 없는 장비인 경우 default 녹화구간 입력
                QDateTime now = QDateTime::currentDateTime();
                response->recordingPeriod.startTime = now.addDays(-1).toMSecsSinceEpoch();
                response->recordingPeriod.endTime = now.toMSecsSinceEpoch();
            }
            m_deviceTimeline[request->deviceID].recordingPeriod = response->recordingPeriod;
            m_deviceTimeline[request->deviceID].overlappedId = response->overlappedId;

            for(auto& overlappedIdList : response->channelOverlappedIdList)
            {
                // 채널 별 OverlappedId List 저장
                m_deviceTimeline[request->deviceID].channelOverlappedIdList[overlappedIdList.first] = overlappedIdList.second;
            }

            for(auto& timeline : response->timeline.channelTimelineList)
            {
                m_deviceTimeline[request->deviceID].timeline.channelTimelineList[timeline.first] = timeline.second;

                // update timeline UI
                if(m_selectedDeviceId == request->deviceID && m_selectedChannelId == timeline.first)
                {
                    SPDLOG_DEBUG("MediaControlModel::getAllPeriodTimeline 0 m_selectedOverlappedId={}", m_selectedOverlappedId);
                    if(m_selectedOverlappedId == -1) {
                        // 선택 된 OverlappedId가 없으면 첫 번째 OverlappedId를 Default로 선택
                        for(auto& overlappedId : response->channelOverlappedIdList[timeline.first]) {
                            for(auto& track : timeline.second) {
                                if(track.first == overlappedId) {
                                    m_selectedOverlappedId = overlappedId;
                                    SPDLOG_DEBUG("MediaControlModel::getAllPeriodTimeline 1 m_selectedOverlappedId={}",m_selectedOverlappedId);
                                    break;
                                }
                            }
                            if(m_selectedOverlappedId != -1)
                                break;
                        }
                    }

                    if(m_selectedOverlappedId == -1) {
                        // 첫 번째 OverlappedId를 Default로 선택하므로 불필요한 코드이나, 방어 코드로 남겨둠.
                        for(auto& track : timeline.second){
                            if(m_selectedOverlappedId == -1 || m_selectedOverlappedId > track.first){
                                m_selectedOverlappedId = track.first;
                                SPDLOG_DEBUG("MediaControlModel::getAllPeriodTimeline 3 m_selectedOverlappedId={}",m_selectedOverlappedId);
                            }
                        }
                    }

                    if(!m_isMultiTimelineMode) {
                        m_recordingStartTime = response->recordingPeriod.startTime;
                        m_recordingEndTime = response->recordingPeriod.endTime;
                        setDefaultTimelineStartEndTime();
                        checkVisibleTimelineOverlflow();
                    }

                    // 선택 타임라인 변경 signal
                    emit selectedTimelineUpdated(timeline.second, response->channelOverlappedIdList[timeline.first]);
                }
            }
        }
    });
#endif
}

void MediaControlModel::setSelectedChannel(const QString& deviceID, const QString& channelID, const int overlappedId, qint64 currentTime)
{
    if(deviceID == "" || channelID == "")
    {
        resetTimeline();
        return;
    }

    m_selectedDeviceId = deviceID.toStdString();
    m_selectedChannelId = channelID.toStdString();
    m_selectedOverlappedId = overlappedId;

    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    if(QCoreServiceManager::Instance().DB()->FindDevice(m_selectedDeviceId, device)
            && device.deviceCapabilities.recordingSearchPeriod)
    {
        currentTime = 0;    // recordingSearchPeriod Cap이 있으면 기준 시간 입력 없이 전체 구간 검색
    }
#endif

    Wisenet::Core::UserGroup usergroup;
#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().DB()->GetLoginUserGroup(usergroup);
#endif
    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>> selectedTimeline;
    std::vector<int> overlappedIdList;

    // 선택 된 Device의 타임라인 데이터 검색
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(deviceID.toStdString());
    if (!usergroup.userPermission.playback)
    {
        // Playback 권한이 없으면 타임라인 로드하지 않음
    }
    else if (deviceIter == m_deviceTimeline.end())
    {
        // 선택 된 Device의 타임라인이 없는 경우 타임라인 요청
        QList<QString> channelIdList;
        channelIdList.push_back(channelID);
        getAllPeriodTimeline(deviceID, channelIdList, currentTime);
    }
    else
    {
        if(!m_isMultiTimelineMode) {
            m_recordingStartTime = deviceIter->second.recordingPeriod.startTime;
            m_recordingEndTime = deviceIter->second.recordingPeriod.endTime;
            setDefaultTimelineStartEndTime();
            checkVisibleTimelineOverlflow();
        }

        // 선택 된 Channel의 타임라인 데이터 검색
        std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
        channelIter = deviceIter->second.timeline.channelTimelineList.find(m_selectedChannelId);
        if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
        {
            // 선택 된 Channel의 타임라인이 없는 경우 타임라인 요청
            QList<QString> channelIdList;
            channelIdList.push_back(channelID);
            getAllPeriodTimeline(deviceID, channelIdList, currentTime);
        }
        else
        {
            selectedTimeline = channelIter->second;
            overlappedIdList = deviceIter->second.channelOverlappedIdList[m_selectedChannelId];

            if(m_selectedOverlappedId == -1) {
                // 선택 된 OverlappedId가 없으면 첫 번째 OverlappedId를 Default로 선택
                for(auto& trackId : deviceIter->second.channelOverlappedIdList[m_selectedChannelId]) {
                    for(auto& track : channelIter->second) {
                        if(track.first == trackId) {
                            m_selectedOverlappedId = trackId;
                            break;
                        }
                    }
                    if(m_selectedOverlappedId != -1)
                        break;
                }
            }

            if(m_selectedOverlappedId == -1) {
                // 첫 번째 OverlappedId를 Default로 선택하므로 불필요한 코드이나, 방어 코드로 남겨둠.
                for(auto& track : channelIter->second){
                    if(m_selectedOverlappedId == -1 || m_selectedOverlappedId > track.first){
                        m_selectedOverlappedId = track.first;
                    }
                }
            }

            additionalTimelineReload(true); // 추가 녹화 데이터 로딩
        }
    }

    // 선택 타임라인 변경 signal
    emit selectedChannelChanged(selectedDeviceID(), selectedChannelID());
    emit selectedTimelineUpdated(selectedTimeline, overlappedIdList);

    // 타임라인 자동 로딩 타이머 시작
    m_timelineReloadTimer.reset(new QTimer());
    m_timelineReloadTimer->setInterval(60000);
    connect(m_timelineReloadTimer.data(), &QTimer::timeout, this, &MediaControlModel::onTimelineReloadTimerTimeout, Qt::QueuedConnection);
    m_timelineReloadTimer->start();
}

int MediaControlModel::getDefaultOverlappedId(const QString& deviceID, const QString& channelID)
{
    // 이미 선택 된 OverlappedId가 있으면 리턴
    if(m_selectedDeviceId == deviceID.toStdString() && m_selectedChannelId == channelID.toStdString() && m_selectedOverlappedId != -1)
        return m_selectedOverlappedId;

    // Device 타임라인 검색
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(deviceID.toStdString());

    if (deviceIter == m_deviceTimeline.end())
        return -1;  // Device의 타임라인이 없는 경우 -1 리턴

    // Channel 타임라인 검색
    std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
    channelIter = deviceIter->second.timeline.channelTimelineList.find(channelID.toStdString());

    if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
        return -1;  // Channel 타임라인이 없는 경우 -1 리턴

    // Channel의 OverlappedId List 검색
    if(deviceIter->second.channelOverlappedIdList.find(channelID.toStdString()) == deviceIter->second.channelOverlappedIdList.end() ||
            deviceIter->second.channelOverlappedIdList[channelID.toStdString()].size() == 0)
        return -1;    // Channel의 OverlappedId List가 없는 경우

    return deviceIter->second.channelOverlappedIdList[channelID.toStdString()][0];
}

qint64 MediaControlModel::getLiveToPlaybackTime(const QString& deviceID, const QString& channelID, const int overlappedId, const qint64 currentTime)
{
    qint64 playbackTime = currentTime - 300000; // Default 5분 전

    // Device 타임라인 검색
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(deviceID.toStdString());
    if (deviceIter == m_deviceTimeline.end())
        return playbackTime;  // Device의 타임라인이 없는 경우 default 값 리턴

    // Channel 타임라인 검색
    std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
    channelIter = deviceIter->second.timeline.channelTimelineList.find(channelID.toStdString());
    if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
        return playbackTime;  // Channel 타임라인이 없는 경우 default 값 리턴

    // Overlapped 타임라인 검색
    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>::iterator overlappedIter = channelIter->second.find(overlappedId);
    if(overlappedIter == channelIter->second.end() || overlappedIter->second.size() == 0)
        return playbackTime;   // overlapped 타임라인이 없는 경우 default 값 리턴

    std::vector<Wisenet::Device::ChannelTimeline> timelineList = overlappedIter->second;

    int prevRecordIndex = -1;
    for(int i=0 ; i<timelineList.size() ; i++)
    {
        if(timelineList[i].startTime <= playbackTime && timelineList[i].endTime >= playbackTime)
            return playbackTime;

        if(timelineList[i].endTime < playbackTime)
            prevRecordIndex = i;
    }

    if(prevRecordIndex != -1)
        return timelineList[prevRecordIndex].endTime - 300000;  // 가장 가까운 이전 레코드의 5분 전

    return playbackTime;
}

void MediaControlModel::resetTimeline()
{
    m_selectedDeviceId = "";
    m_selectedChannelId = "";
    m_selectedOverlappedId = -1;
    emit selectedChannelChanged(selectedDeviceID(), selectedChannelID());
    emit selectedTimelineUpdated(std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>(),
                                 std::vector<int>());

    if(m_isMultiTimelineMode) {
        // 멀티 타임라인 모드이면 달력 선택 날짜로 타임라인 스크롤 범위 제한
        m_timelineStartTime = m_recordingStartTime = m_calendarSelectedDate.toMSecsSinceEpoch();
        m_timelineEndTime = m_recordingEndTime = m_calendarSelectedDate.addDays(1).toMSecsSinceEpoch();
    }
    else {
        QDateTime now = QDateTime::currentDateTime();
        now.setTime(QTime(0,0,0,0));
        m_timelineStartTime = m_recordingStartTime = now.toMSecsSinceEpoch();
        m_timelineEndTime = m_recordingEndTime = now.addDays(1).toMSecsSinceEpoch();
    }
    setvisibleStartTime(m_timelineStartTime);
    setvisibleEndTime(m_timelineEndTime);
    m_visibleTimeRange = m_visibleEndTime - m_visibleStartTime;
    emit visibleTimeRangeChanged(m_visibleTimeRange);

    m_timelineReloadTimer.reset();
}

QVariantList MediaControlModel::getOverlappedIdList()
{
    QVariantList list;
    list.push_back(-1);

    // Device 타임라인 검색
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(m_selectedDeviceId);

    if (deviceIter == m_deviceTimeline.end())
        return list;  // Device의 타임라인이 없는 경우

    // Channel 타임라인 검색
    std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
    channelIter = deviceIter->second.timeline.channelTimelineList.find(m_selectedChannelId);

    if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
        return list;  // Channel 타임라인이 없는 경우

    // Channel의 OverlappedId List 검색
    if(deviceIter->second.channelOverlappedIdList.find(m_selectedChannelId) == deviceIter->second.channelOverlappedIdList.end() ||
            deviceIter->second.channelOverlappedIdList[m_selectedChannelId].size() == 0)
        return list;    // Channel의 OverlappedId List가 없는 경우

    list.clear();
    for(auto& overlappedId : deviceIter->second.channelOverlappedIdList[m_selectedChannelId])
        list.push_back(overlappedId);
    return list;
}

void MediaControlModel::setSelectedOverlappedID(int overlappedId)
{
    if(m_selectedOverlappedId == overlappedId)
        return;

    m_selectedOverlappedId = overlappedId;
    emit selectedOverlappedIDChanged();

    /* OverlappedId 통합 타임라인으로 주석 처리 (선택 OverlappedId가 변경되어도 선택 된 타임라인 데이터는 동일하기 때문)
    // 선택 된 Device의 타임라인 데이터 검색
    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(m_selectedDeviceId);

    if (deviceIter == m_deviceTimeline.end())
        return; // 선택 된 Device의 타임라인이 없는 경우 리턴

    // 선택 된 Channel의 타임라인 데이터 검색
    std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
    channelIter = deviceIter->second.timeline.channelTimelineList.find(m_selectedChannelId);

    if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
        return; // 선택 된 Channel의 타임라인이 없는 경우 리턴

    auto overlappedIter = channelIter->second.find(m_selectedOverlappedId);
    if(overlappedIter != channelIter->second.end())
        emit selectedTimelineUpdated(overlappedIter->second);
    */
}

void MediaControlModel::setIsMultiTimelineMode(bool multiTimelineMode)
{
    if(m_isMultiTimelineMode == multiTimelineMode)
        return;

    m_isMultiTimelineMode = multiTimelineMode;
    emit isMultiTimelineModeChanged(m_isMultiTimelineMode);

    // 멀티 타임라인 모드이면 달력 선택 날짜로 타임라인 스크롤 범위 제한
    if(m_isMultiTimelineMode) {
        m_timelineStartTime = m_recordingStartTime = m_calendarSelectedDate.toMSecsSinceEpoch();
        m_timelineEndTime = m_recordingEndTime = m_calendarSelectedDate.addDays(1).toMSecsSinceEpoch();
        checkVisibleTimelineOverlflow();
    }
}

void MediaControlModel::setCalendarSelectedDate(QDateTime selectedDate)
{
    selectedDate.setTime(QTime(0,0,0,0));
    //SPDLOG_DEBUG("MediaControlModel::setCalendarSelectedDate() date : {}", selectedDate.toString().toStdString());

    if(m_calendarSelectedDate == selectedDate)
        return;

    m_calendarSelectedDate = selectedDate;
    emit calendarSelectedDateChanged();

    // 멀티 타임라인 모드이면 달력 선택 날짜로 타임라인 스크롤 범위 제한
    if(m_isMultiTimelineMode) {
        m_timelineStartTime = m_recordingStartTime = selectedDate.toMSecsSinceEpoch();
        m_timelineEndTime = m_recordingEndTime = selectedDate.addDays(1).toMSecsSinceEpoch();
    }
}

void MediaControlModel::setCalendarMinimumDate(QDateTime minimumDate)
{
    minimumDate.setTime(QTime(0,0,0,0));
    if(m_calendarMinimumDate == minimumDate)
        return;

    m_calendarMinimumDate = minimumDate;
    emit calendarMinimumDateChanged();
}

void MediaControlModel::setCalendarMaximumDate(QDateTime maximumDate)
{
    maximumDate.setTime(QTime(0,0,0,0));
    if(m_calendarMaximumDate == maximumDate)
        return;

    m_calendarMaximumDate = maximumDate;
    emit calendarMaximumDateChanged();
}

void MediaControlModel::setRecordingTypeFilter(quint32 filter)
{
    if(m_recordingTypeFilter == (Wisenet::Device::RecordingType)filter)
        return;

    m_recordingTypeFilter = (Wisenet::Device::RecordingType)filter;
    emit recordingTypeFilterChanged(filter);
}

void MediaControlModel::getLocalResourceTimeline(WisenetMediaPlayer* mediaPlayer)
{
    // 로컬리소스 선택 시, 선택 device 정보 초기화
    m_selectedDeviceId = "";
    m_selectedChannelId = "";
    m_selectedOverlappedId = -1;
    emit selectedChannelChanged(selectedDeviceID(), selectedChannelID());

    QMap<qint64, qint64> timelineMap;   // <key:starTtime, value:endTime>
    if(mediaPlayer->supportTimeline())
    {
        // get Timeline
        timelineMap = mediaPlayer->getTimeline();
    }
    else if(mediaPlayer->supportDuration())
    {
        // get Duration
        QDateTime today = QDateTime::currentDateTime();
        today.setTime(QTime(0,0,0,0));
        timelineMap.insert(today.toMSecsSinceEpoch(), today.toMSecsSinceEpoch() + mediaPlayer->duration());
    }

    if(timelineMap.count() == 0)
    {
        resetTimeline();
        return;
    }

    m_timelineReloadTimer.reset();  // LocalResourceTimeline 표시 중에는 타임라인 리로드 타이머 중지

    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>> selectedTimeline;
    for (auto iter = timelineMap.constBegin(); iter != timelineMap.constEnd(); ++iter)
    {
        Wisenet::Device::ChannelTimeline timeline;
        timeline.startTime = iter.key();
        timeline.endTime = iter.value();
        timeline.type = Wisenet::Device::RecordingType::Normal;
        selectedTimeline[-1].push_back(timeline);
    }

    // 타임라인 표시 정보 업데이트
    m_timelineStartTime = m_recordingStartTime = timelineMap.firstKey();
    m_timelineEndTime = m_recordingEndTime = timelineMap.last();
    checkVisibleTimelineOverlflow();

    // 선택 타임라인 변경 signal
    emit selectedTimelineUpdated(selectedTimeline, std::vector<int>{-1});
}

void MediaControlModel::onTimelineReloadTimerTimeout()
{
    additionalTimelineReload(false);
}

void MediaControlModel::additionalTimelineReload(bool fromLastRecordTime)
{
    auto request = std::make_shared<Wisenet::Device::DeviceGetRecordingPeriodRequest>();
    request->deviceID = m_selectedDeviceId;

#ifndef MEDIA_FILE_ONLY
    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetRecordingPeriod, this, request,
                [this, request, fromLastRecordTime](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetRecordingPeriodResponse>(reply->responseDataPtr);
        if(response->isSuccess())
        {
            // Device 타임라인 검색
            std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse>::iterator deviceIter = m_deviceTimeline.find(request->deviceID);
            if (deviceIter == m_deviceTimeline.end())
                return;  // Device의 타임라인이 없는 경우 리턴

            // Channel 타임라인 검색
            std::unordered_map<std::string, std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>>::iterator channelIter;
            channelIter = deviceIter->second.timeline.channelTimelineList.find(m_selectedChannelId);
            if(channelIter == deviceIter->second.timeline.channelTimelineList.end())
                return;  // Channel 타임라인이 없는 경우 리턴

            // Overlapped 타임라인 검색
            std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>::iterator overlappedIter = channelIter->second.find(m_selectedOverlappedId);
            if(overlappedIter == channelIter->second.end())
                return;   // overlapped 타임라인이 없는 경우 리턴

            // 녹화 구간 변경사항이 없으면 리턴
            if(!fromLastRecordTime && response->result.startTime == deviceIter->second.recordingPeriod.startTime
                    && response->result.endTime == deviceIter->second.recordingPeriod.endTime)
            {
                return;
            }

            // 신규 로딩 할 구간
            int64_t reloadStartTime = deviceIter->second.recordingPeriod.endTime - 60000;
            int64_t reloadEndTime = response->result.endTime;
            if(overlappedIter->second.size() > 0
                    && (fromLastRecordTime || overlappedIter->second[overlappedIter->second.size()-1].endTime > reloadStartTime))
            {
                // 데이터 중복 방지를 위해 마지막 Record의 endTime 체크
                reloadStartTime = overlappedIter->second[overlappedIter->second.size()-1].endTime;
            }

            // 녹화 구간 데이터 업데이트
            deviceIter->second.recordingPeriod = response->result;
            if(!m_isMultiTimelineMode) {
                m_recordingStartTime = deviceIter->second.recordingPeriod.startTime;
                m_recordingEndTime = deviceIter->second.recordingPeriod.endTime;
                setDefaultTimelineStartEndTime(false);
                checkVisibleTimelineOverlflow();
            }

            // 신규 녹화 구간 데이터 로딩
            auto request = std::make_shared<Wisenet::Device::DeviceGetTimelineRequest>();
            request->deviceID = m_selectedDeviceId;
            request->param.FromDate = reloadStartTime;
            request->param.ToDate = reloadEndTime;
            request->param.OverlappedID = m_selectedOverlappedId;
            request->param.ChannelIDList.push_back(m_selectedChannelId);

            SPDLOG_DEBUG("MediaControlModel::additionalTimelineReload() from:{} to:{}",
                         QDateTime::fromMSecsSinceEpoch(reloadStartTime).toString().toStdString(),
                         QDateTime::fromMSecsSinceEpoch(reloadEndTime).toString().toStdString());

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceGetTimeline, this, request,
                        [this, deviceIter, channelIter, overlappedIter, request](const QCoreServiceReplyPtr& reply)
            {
                auto timelineResponse = std::static_pointer_cast<Wisenet::Device::DeviceGetTimelineResponse>(reply->responseDataPtr);

                // 성공 시 Timeline 저장
                std::string channelId = "";
                int overlappedId = -1;
                if(timelineResponse->isSuccess())
                {
                    for(auto& channelMap : timelineResponse->result.channelTimelineList)
                    {
                        channelId = channelMap.first;
                        for(auto& timelineMap : channelMap.second)
                        {
                            overlappedId = timelineMap.first;
                            int lastNormalRecordIndex = -1;
                            for(int i=overlappedIter->second.size() - 1 ; i >= 0 ; i--)
                            {
                                if(overlappedIter->second[i].type == Wisenet::Device::RecordingType::Normal)
                                {
                                    lastNormalRecordIndex = i;
                                    break;
                                }
                            }

                            for(auto& timeline : timelineMap.second)
                            {
                                if(lastNormalRecordIndex != -1 && timeline.type == Wisenet::Device::RecordingType::Normal
                                        && overlappedIter->second[lastNormalRecordIndex].endTime >= timeline.startTime)
                                {
                                    // 마지막 Normal Record와 연속 된 데이터는 endTime만 업데이트
                                    overlappedIter->second[lastNormalRecordIndex].endTime = timeline.endTime;
                                }
                                else
                                {
                                    // 추가 로딩 된 Record 저장
                                    overlappedIter->second.push_back(timeline);
                                    //m_deviceTimeline[request->deviceID].timeline.channelTimelineList[channelId][overlappedId].push_back(timeline);
                                }
                            }
                        }
                    }

                    // 선택 타임라인 업데이트 signal
                    if(channelId != "" && overlappedId != -1)
                    {
                        std::vector<int> overlappedIdList;
                        if(deviceIter->second.channelOverlappedIdList.find(channelIter->first) != deviceIter->second.channelOverlappedIdList.end())
                            overlappedIdList = deviceIter->second.channelOverlappedIdList[channelIter->first];
                        emit selectedTimelineUpdated(channelIter->second, overlappedIdList);
                    }
                }
            });
        }
    });
#endif
}

QString MediaControlModel::getHashPassword(QString userGuid, QString password)
{
    return QString::fromStdString(DigestAuthenticator::hashSpecialSha256(userGuid.toStdString(), password.toStdString()));
}

/// deviceChannelList의 모든 Device/Channel에 대한 녹화 구간과 Calendar 검색 함수
void MediaControlModel::refreshCalendar(QList<QString> deviceChannelList, int visibleYear, int visibleMonth, bool recordMapOnly)
{
    SPDLOG_DEBUG("MediaControlModel::refreshCalendar() visibleYear:{} visibleMonth:{}", visibleYear, visibleMonth);

#ifndef MEDIA_FILE_ONLY
    QMap<QString, QSet<QString>> deviceChannelMap;
    for(auto& deviceChannel : deviceChannelList) {
        QStringList tokens = deviceChannel.split("_");
        if(tokens.size() != 2)
            continue;
        deviceChannelMap[tokens[0]].insert(tokens[1]);
    }

    // 기존 calendar 범위 초기화
    if(!recordMapOnly) {
        m_calendarMinimumDate.setMSecsSinceEpoch(0);
        m_calendarMaximumDate.setMSecsSinceEpoch(0);
    }

    for(auto itor = deviceChannelMap.begin(); itor != deviceChannelMap.end(); itor++) {
        if(!recordMapOnly) {
            // 장비별 녹화 구간 요청
            auto periodRequest = std::make_shared<Wisenet::Device::DeviceGetRecordingPeriodRequest>();
            periodRequest->deviceID = itor.key().toStdString();

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceGetRecordingPeriod, this, periodRequest,
                        [this, periodRequest](const QCoreServiceReplyPtr& reply)
            {
                auto periodResponse = std::static_pointer_cast<Wisenet::Device::DeviceGetRecordingPeriodResponse>(reply->responseDataPtr);
                if(!periodResponse->isSuccess()) {
                    SPDLOG_INFO("MediaControlModel::refreshCalendar() DeviceGetRecordingPeriodRequest failed ({})", periodResponse->errorString());
                }
                else {
                    /*
                SPDLOG_DEBUG("MediaControlModel::refreshCalendar() deviceId:{}, startTime:{}, endTime:{}",
                             periodRequest->deviceID, periodResponse->result.startTime, periodResponse->result.endTime);
                             */
                    // 녹화 구간이 없는 경우 default 값 적용
                    if(periodResponse->result.startTime == 0)
                        periodResponse->result.startTime = QDateTime::currentMSecsSinceEpoch();
                    if(periodResponse->result.endTime == 0)
                        periodResponse->result.endTime = QDateTime::currentMSecsSinceEpoch();

                    // 전체 장비 통합 녹화 구간 변경 체크
                    if(m_calendarMinimumDate.toMSecsSinceEpoch() == 0 || m_calendarMinimumDate.toMSecsSinceEpoch() > periodResponse->result.startTime) {
                        m_calendarMinimumDate = QDateTime::fromMSecsSinceEpoch(periodResponse->result.startTime);
                        emit calendarMinimumDateChanged();
                    }

                    if(m_calendarMaximumDate.toMSecsSinceEpoch() == 0 || m_calendarMaximumDate.toMSecsSinceEpoch() < periodResponse->result.endTime) {
                        m_calendarMaximumDate = QDateTime::fromMSecsSinceEpoch(periodResponse->result.endTime);
                        emit calendarMaximumDateChanged();
                    }
                }
            });
        }

        // 장비별 캘린더 검색 (선택 월 기준 +-1달)
        QDateTime month(QDate(visibleYear, visibleMonth, 15), QTime(0,0));

        for(int i=0 ; i<3 ; i++) {
            auto calendarRequest = std::make_shared<Wisenet::Device::DeviceCalendarSearchRequest>();
            calendarRequest->deviceID = itor.key().toStdString();
            calendarRequest->param.Month = month.addMonths(i-1).toMSecsSinceEpoch();
            for(auto& channelId : itor.value().values()) {
                calendarRequest->param.ChannelIDList.push_back(channelId.toStdString());
            }

            QCoreServiceManager::Instance().RequestToCoreService(
                        &Wisenet::Core::ICoreService::DeviceCalendarSearch, this, calendarRequest,
                        [this, calendarRequest](const QCoreServiceReplyPtr& reply)
            {
                auto calendarResponse = std::static_pointer_cast<Wisenet::Device::DeviceCalendarSearchResponse>(reply->responseDataPtr);
                if(!calendarResponse->isSuccess()) {
                    SPDLOG_INFO("MediaControlModel::refreshCalendar() DeviceCalendarSearchRequest failed ({})", calendarResponse->errorString());
                }
                else {
                    /*
                    SPDLOG_DEBUG("MediaControlModel::refreshCalendar() deviceId:{}, month:{}, channelCount:{}",
                                 calendarRequest->deviceID, calendarRequest->param.Month, calendarResponse->result.channelReslut.size());
                                 */
                    std::vector<bool> dayRecordExist(31, false);
                    for(auto itor = calendarResponse->result.channelReslut.begin() ; itor != calendarResponse->result.channelReslut.end() ; itor++) {
                        for(int i=0 ; i<31 ; i++) {
                            if(itor->second.dayRecordExist[i])
                                dayRecordExist[i] = true;
                        }
                    }

                    emit calendarSearchFinished(calendarRequest->param.Month, dayRecordExist);
                }
            });
        }
    }
#endif
}

void MediaControlModel::setDefaultTimelineStartEndTime(bool reset)
{
    // 최대 타임라인 표시 범위를 녹화 시작 날짜 0시 ~ 녹화 끝 날짜 24시까지로 초기화
    QDateTime startTime = QDateTime::fromMSecsSinceEpoch(m_recordingStartTime);
    QDateTime endTime = QDateTime::fromMSecsSinceEpoch(m_recordingEndTime).addDays(1);
    startTime.setTime(QTime(0,0,0,0));
    endTime.setTime(QTime(0,0,0,0));

    /*
    SPDLOG_DEBUG("MediaControlModel::setDefaultTimelineStartEndTime() startTime:{}, endTime:{}",
                 startTime.toString().toStdString(), endTime.toString().toStdString());
    */

    // reset인 경우, 신규 값 할당
    // reset이 아닌 경우, 더 넓은 표시범위를 유지
    if(reset || m_timelineStartTime > startTime.toMSecsSinceEpoch())
        m_timelineStartTime = startTime.toMSecsSinceEpoch();
    if(reset || m_timelineEndTime < endTime.toMSecsSinceEpoch())
        m_timelineEndTime = endTime.toMSecsSinceEpoch();
}

void MediaControlModel::checkVisibleTimelineOverlflow()
{
    // 현재 표시 구간이 타임라인 최대 구간을 넘어가는지 체크하는 함수
    qlonglong maximunRange = m_timelineEndTime - m_timelineStartTime;
    if(maximunRange > (qlonglong)365*24*3600*1000)
        maximunRange = (qlonglong)365*24*3600*1000;    // 최대 365일 범위까지 표시하도록 제한
    if(m_visibleTimeRange > maximunRange)
        m_visibleTimeRange = maximunRange;

    if(m_visibleStartTime < m_timelineStartTime) {
        // 최대 구간 좌측으로 벗어난 경우
        m_visibleStartTime = m_timelineStartTime;
        m_visibleEndTime = m_visibleStartTime + m_visibleTimeRange;
    }
    else if(m_visibleEndTime > m_timelineEndTime) {
        // 최대 구간 우측으로 벗어난 경우
        m_visibleEndTime = m_timelineEndTime;
        m_visibleStartTime = m_visibleEndTime - m_visibleTimeRange;
    }

    emit visibleTimeRangeChanged(m_visibleTimeRange);
}

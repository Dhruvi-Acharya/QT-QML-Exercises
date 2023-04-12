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

#include "MultiGraphAreaModel.h"
#include <QPainter>
#include <QDebug>

MultiGraphAreaModel::MultiGraphAreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[ChannelNameRole] = "channelName";
    m_roleNames[DeviceIdRole] = "deviceId";
    m_roleNames[ChannelIdRole] = "channelId";
    m_roleNames[OverlappedIdRole] = "overlappedId";
    m_roleNames[GraphImageRole] = "graphImage";
    m_roleNames[OverlappedBlockCountRole] = "overlappedBlockCount";

    m_requestTimer.setInterval(90000);
    connect(&m_requestTimer, &QTimer::timeout, this, &MultiGraphAreaModel::onRequestTimerTimeout, Qt::QueuedConnection);
}

MultiGraphAreaModel::~MultiGraphAreaModel()
{
    clear();
}

QHash<int, QByteArray> MultiGraphAreaModel::roleNames() const
{
    return m_roleNames;
}

int MultiGraphAreaModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_displayItemIdList.size();
}

QVariant MultiGraphAreaModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_displayItemIdList.size())
        return QVariant();

    QString itemId = m_displayItemIdList[row];
    if(m_graphDataMap.find(itemId) == m_graphDataMap.end())
        return QVariant();

    switch(role) {
    case ChannelNameRole:
        return m_graphDataMap[itemId].channelName;
    case DeviceIdRole:
        return QString::fromStdString(m_graphDataMap[itemId].deviceId);
    case ChannelIdRole:
        return QString::fromStdString(m_graphDataMap[itemId].channelId);
    case OverlappedIdRole:
        return m_graphDataMap[itemId].overlappedId;
    case GraphImageRole:
        return m_graphDataMap[itemId].graphImage;
    case OverlappedBlockCountRole:
        return m_graphDataMap[itemId].overlappedBlockList.count();
    }

    return QVariant();
}

/*
void MultiGraphAreaModel::insert(int index, const MultiGraphAreaData& item)
{
    if(index < 0 || index > m_data.size()) {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, item);
    endInsertRows();
}

void MultiGraphAreaModel::append(const MultiGraphAreaData& item)
{
    insert(m_data.size(), item);
}

void MultiGraphAreaModel::remove(int index)
{
    if(index < 0 || index > m_data.size() - 1) {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}
*/

void MultiGraphAreaModel::clear()
{
    if(m_displayItemIdList.size() != 0) {
        beginRemoveRows(QModelIndex(), 0, m_displayItemIdList.size()-1);
        m_displayItemIdList.clear();
        endRemoveRows();
    }
    m_graphDataMap.clear();
}

bool MultiGraphAreaModel::isLoading() const
{
    return m_isLoading;
}

void MultiGraphAreaModel::setIsLoading(const bool loading)
{
    if(m_isLoading != loading) {
        m_isLoading = loading;
        emit isLoadingChanged();
    }
}


// 전체 데이터 갱신 함수 (전체 item의 graphImage를 다시 그리는 함수)
void MultiGraphAreaModel::refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth,
                                               QString qDeviceId, QList<QString> channelIdList, QVariantMap overlappedIdMap)
{
    //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList(1) deviceId:{}", qDeviceId.toStdString());
    /*
    SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList() overlappedIdMap count:{}", overlappedIdMap.count());
    auto itor = overlappedIdMap.begin();
    while(itor != overlappedIdMap.end()) {
        SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList() itemId:{}, overlappedId:{}",
                     itor.key().toStdString(), itor.value().toInt());
        itor++;
    }
    */

    QDateTime fromDate = m_selectedDate;//QDateTime::fromMSecsSinceEpoch(visibleStartTime);
    fromDate.setTime(QTime(0,0,0,0));
    int64_t dateKey = fromDate.toMSecsSinceEpoch(); // 시작 날짜의 0시0분0초    
    QMap<Wisenet::uuid_string, QMap<std::string, std::string>> deviceMap;    // 같은 채널을 중복으로 그리지 않기 위한 Map

    // refresh할 ChannelIdList를 Map으로 변환
    QMap<std::string, std::string> channelMap;
    for(auto& channelId : channelIdList) {
        channelMap[channelId.toStdString()] = channelId.toStdString();
        //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList(2) channelId:{}", channelId.toStdString());
    }

    for(int i = 0 ; i < m_displayItemIdList.count() ; i++) {
        QString itemId = m_displayItemIdList[i];

        if(m_graphDataMap.find(itemId) == m_graphDataMap.end()) {
            //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList() itemId not found");
            continue;   // item의 deviceId, channelId 정보가 없을 때
        }

        Wisenet::uuid_string deviceId = m_graphDataMap[itemId].deviceId;
        std::string channelId = m_graphDataMap[itemId].channelId;

        if((qDeviceId != "" && qDeviceId.toStdString() != deviceId) || (channelMap.count() != 0 && channelMap.find(channelId) == channelMap.end())) {
            //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList() deviceId, channelId not found");
            continue;   // refresh 대상 device, channel이 아닌 경우
        }

        deviceMap[deviceId][channelId] = channelId;

        // 타임라인 유무 확인
        if(m_deviceTimeline.find(deviceId) == m_deviceTimeline.end()
                || m_deviceTimeline[deviceId].onedayTimelineList.find(channelId) == m_deviceTimeline[deviceId].onedayTimelineList.end()
                || m_deviceTimeline[deviceId].onedayTimelineList[channelId].find(dateKey) == m_deviceTimeline[deviceId].onedayTimelineList[channelId].end()) {
            // 타임라인이 없으면 빈 이미지로 업데이트
            //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList() timeline is not loaded");
            QImage newImage = QImage(controlWidth, 10, QImage::Format_ARGB32);
            newImage.fill(QColor(49, 49, 49));
            m_graphDataMap[itemId].graphImage = newImage;
            m_graphDataMap[itemId].overlappedBlockList.clear();

            QVector<int> rols {GraphImageRole, OverlappedBlockCountRole};
            emit dataChanged(index(i,0,QModelIndex()), index(i,0,QModelIndex()), rols);
        }
        else {
            Wisenet::Device::OneDayTimeline& oneDayTimeline = m_deviceTimeline[deviceId].onedayTimelineList[channelId][dateKey];
            //SPDLOG_DEBUG("MultiGraphAreaModel::refreshGraphDataList(3) deviceId:{}, channelId:{}", deviceId, channelId);
            refreshGraphImage(visibleStartTime, visibleEndTime, controlWidth, i, m_graphDataMap[itemId], oneDayTimeline, overlappedIdMap);
        }
    }
}

void MultiGraphAreaModel::refreshGraphImage(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, int modelIndex,
                                            MultiGraphAreaData& graphData, Wisenet::Device::OneDayTimeline& oneDayTimeline, QVariantMap overlappedIdMap)
{
    qulonglong visibleRange = visibleEndTime - visibleStartTime;

    // Overlapped 영역 데이터 업데이트
    graphData.overlappedBlockList.clear();
    QList<RecordTime> overlappedTimeList;
    QList<Wisenet::Device::ChannelTimeline> periodList;

    for(auto itor = oneDayTimeline.recordingPeriodMap.begin() ; itor != oneDayTimeline.recordingPeriodMap.end() ; itor++) {
        periodList.push_back(itor->second);
    }

    for(int i=0 ; i<periodList.count() ; i++) {
        for(int j=i ; j<periodList.count() ; j++) {
            if(i == j)
                continue;

            if(periodList[i].endTime < periodList[j].startTime || periodList[i].startTime > periodList[j].endTime)
                continue;   // i track과 j track에 겹치는 구간이 없음

            RecordTime overlappedTime;
            overlappedTime.startTime = std::max(periodList[i].startTime, periodList[j].startTime);
            overlappedTime.endTime = std::min(periodList[i].endTime, periodList[j].endTime);
            overlappedTimeList.push_back(overlappedTime);
        }
    }

    std::sort(overlappedTimeList.begin(), overlappedTimeList.end(), [](const RecordTime data1, const RecordTime data2) {
        return data1.startTime < data2.startTime;
    });

    for(int i = overlappedTimeList.count() - 1 ; i >= 0 ; i--) {
        if(overlappedTimeList[i].endTime <= visibleStartTime || overlappedTimeList[i].startTime >= visibleEndTime) {
            overlappedTimeList.removeAt(i); // 화면에 표시되지 않는 중복구간 데이터 제거
        }
    }

    int overlapStartPos = 0;
    int overlapEndPos = 0;
    for(auto& overlappedTime : overlappedTimeList) {
        float startPos = overlappedTime.startTime <= visibleStartTime ? 0 : controlWidth * (overlappedTime.startTime - visibleStartTime) / visibleRange;
        float endPos = overlappedTime.endTime >= visibleEndTime ? controlWidth : controlWidth * (overlappedTime.endTime - visibleStartTime) / visibleRange;
        if(endPos != controlWidth && endPos - startPos < 1)
            endPos += 1;

        if(endPos < overlapStartPos || startPos > overlapEndPos) {
            // 바로 앞 데이터와 인접하지 않은 경우
            if(overlapEndPos != 0) {
                // 마지막 데이터를 graphData에 추가
                OverlappedBlock block;
                block.startPosition = overlapStartPos;
                block.endPosition = overlapEndPos;
                graphData.overlappedBlockList.push_back(block);
            }

            // 신규 데이터
            overlapStartPos = startPos;
            overlapEndPos = endPos;
        }
        else
        {
            // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
            if(startPos <= overlapStartPos)
                overlapStartPos = startPos;
            if(endPos >= overlapEndPos)
                overlapEndPos = endPos;
        }
    }

    if(overlapEndPos != 0) {
        // 마지막 데이터를 graphData에 추가
        OverlappedBlock block;
        block.startPosition = overlapStartPos;
        block.endPosition = overlapEndPos;
        graphData.overlappedBlockList.push_back(block);
    }

    // 선택 된 overlappedId 확인
    int selectedOverlappedId = -1;
    if(overlappedIdMap.contains(graphData.itemId) && overlappedIdMap[graphData.itemId].canConvert<int>())
        selectedOverlappedId = overlappedIdMap[graphData.itemId].toInt();

    // graph 이미지 업데이트
    QImage newImage = QImage(controlWidth, 10, QImage::Format_ARGB32);
    newImage.fill(QColor(49, 49, 49));
    QPainter qPainter(&newImage);

    // Normal Record를 먼저 그림
    float normalStartPos = 0;
    float normalEndPos = 0;
    qRecordType normalRecordType = qRecordType::RecType_Normal;
    QColor normalColor = QColor(141, 198, 63);
    QColor normalFilteredColor = QColor(141, 198, 63, 76);

    for(auto itor = oneDayTimeline.timelineList.begin() ; itor != oneDayTimeline.timelineList.end() ; itor++) {
        bool isSelectedOverlappedId = selectedOverlappedId == itor->first || selectedOverlappedId == -1;
        for(auto& timeline : itor->second) {
            if(timeline.type != Wisenet::Device::RecordingType::Normal && timeline.type != Wisenet::Device::RecordingType::Manual)
                continue;   // Event Record

            appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth,
                                timeline, overlappedTimeList, isSelectedOverlappedId,
                                qPainter, normalRecordType, normalStartPos, normalEndPos);
        }
    }

    if(normalEndPos != 0) {
        // 마지막 Normal Record를 image에 그림
        qPainter.fillRect(normalStartPos, 0, normalEndPos - normalStartPos, 10, normalRecordType == qRecordType::RecType_Normal ? normalColor : normalFilteredColor);
    }

    // Event Record를 Normal Record 위에 그림
    float eventStartPos = 0;
    float eventEndPos = 0;
    qRecordType eventRecordType = qRecordType::RecType_Event;
    QColor eventColor = QColor(255, 0, 0);
    QColor eventFilteredColor = QColor(255, 0, 0, 76);

    for (auto itor = oneDayTimeline.timelineList.begin(); itor != oneDayTimeline.timelineList.end() ; itor++) {
        bool isSelectedOverlappedId = selectedOverlappedId == itor->first || selectedOverlappedId == -1;
        for(auto& timeline : itor->second) {
            if(timeline.type == Wisenet::Device::RecordingType::Normal || timeline.type == Wisenet::Device::RecordingType::Manual)
                continue;   // Normal Record

            appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth,
                                timeline, overlappedTimeList, isSelectedOverlappedId,
                                qPainter, eventRecordType, eventStartPos, eventEndPos);
        }
    }

    if(eventEndPos != 0) {
        // 마지막 Event Record를 image에 그림
        qPainter.fillRect(eventStartPos, 0, eventEndPos - eventStartPos, 10, eventRecordType == qRecordType::RecType_Event ? eventColor : eventFilteredColor);
    }


    // 최종 이미지 업데이트
    qPainter.end();
    graphData.graphImage = newImage;

    // dataChanged 발생
    QVector<int> rols {GraphImageRole, OverlappedBlockCountRole};
    emit dataChanged(index(modelIndex,0,QModelIndex()), index(modelIndex,0,QModelIndex()), rols);
}

void MultiGraphAreaModel::appendGraphAreaData(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth,
                                              Wisenet::Device::ChannelTimeline& timeline, QList<RecordTime>& overlappedTimeList, bool isSelectedOverlappedId,
                                              QPainter& qPainter, qRecordType& lastRecordType, float& lastStartPos, float& lastEndPos)
{
    if(timeline.endTime <= visibleStartTime || timeline.startTime >= visibleEndTime)
        return;   // 화면을 벗어난 Record

    // 선택 된 overlappedId가 아니면 중복구간에 겹치는 부분을 제외
    if(!isSelectedOverlappedId) {
        for(auto& overlappedTime : overlappedTimeList) {
            if(timeline.endTime <= overlappedTime.startTime || timeline.startTime >= overlappedTime.endTime)
                continue;

            // 겹치는 부분을 제외하고 앞쪽 뒤쪽 데이터를 추가
            if(timeline.startTime <= overlappedTime.startTime) {
                Wisenet::Device::ChannelTimeline frontTimeline = timeline;
                frontTimeline.endTime = overlappedTime.startTime;
                if(frontTimeline.endTime - frontTimeline.startTime > 0) {
                    appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth,
                                        frontTimeline, overlappedTimeList, isSelectedOverlappedId,
                                        qPainter, lastRecordType, lastStartPos, lastEndPos);
                }
            }
            if(timeline.endTime >= overlappedTime.endTime) {
                Wisenet::Device::ChannelTimeline rearTimeline = timeline;
                rearTimeline.startTime = overlappedTime.endTime;
                if(rearTimeline.endTime - rearTimeline.startTime > 0) {
                    appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth,
                                        rearTimeline, overlappedTimeList, isSelectedOverlappedId,
                                        qPainter, lastRecordType, lastStartPos, lastEndPos);
                }
            }

            return;
        }
    }

    qulonglong visibleRange = visibleEndTime - visibleStartTime;
    QColor normalColor = QColor(141, 198, 63);
    QColor normalFilteredColor = QColor(141, 198, 63, 76);
    QColor eventColor = QColor(255, 0, 0);
    QColor eventFilteredColor = QColor(255, 0, 0, 76);

    // 현재 데이터의 위치 계산
    float startPos = timeline.startTime <= visibleStartTime ? 0 : controlWidth * (timeline.startTime - visibleStartTime) / visibleRange;
    float endPos = timeline.endTime >= visibleEndTime ? controlWidth : controlWidth * (timeline.endTime - visibleStartTime) / visibleRange;
    if(endPos != controlWidth && endPos - startPos < 1)
        endPos += 1;

    qRecordType recordType = qRecordType::RecType_None;
    if(timeline.type == Wisenet::Device::RecordingType::Normal || timeline.type == Wisenet::Device::RecordingType::Manual) {
        recordType = (timeline.type & m_recordingTypeFilter) == timeline.type ? qRecordType::RecType_Normal : qRecordType::RecType_NormalFiltered;
    }
    else {
        recordType = (timeline.type & m_recordingTypeFilter) == timeline.type ? qRecordType::RecType_Event : qRecordType::RecType_EventFiltered;
    }

    if(recordType != lastRecordType || endPos < lastStartPos || startPos > lastEndPos)
    {
        // 바로 앞 데이터와 인접하지 않은 경우
        if(lastEndPos != 0) {
            // 마지막 데이터를 image에 그림
            QColor color = QColor(0,0,0,0);
            switch(lastRecordType) {
            case qRecordType::RecType_Normal:
                color = normalColor;
                break;
            case qRecordType::RecType_NormalFiltered:
                color = normalFilteredColor;
                break;
            case qRecordType::RecType_Event:
                color = eventColor;
                break;
            case RecType_EventFiltered:
                color = eventFilteredColor;
                break;
            default:
                break;
            }

            qPainter.fillRect(lastStartPos, 0, lastEndPos - lastStartPos, 10, color);
        }

        // 신규 데이터
        lastStartPos = startPos;
        lastEndPos = endPos;
        lastRecordType = recordType;
    }
    else
    {
        // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
        if(startPos <= lastStartPos)
            lastStartPos = startPos;
        if(endPos >= lastEndPos)
            lastEndPos = endPos;
    }
}

// 현재의 모든 LayoutItem List를 입력받아 기존 ItemList를 갱신하는 함수
void MultiGraphAreaModel::refreshLayoutItems(QVariantList items, QDateTime selectedDate)
{
    QMap<Wisenet::uuid_string, QMap<std::string, std::string>> deviceMap;
    QList<QString> removedItemIds = m_graphDataMap.keys();
    int newChannelCount = 0;
    int copiedChannelCount = 0;

    foreach(QVariant v, items)
    {
        if (!v.canConvert<MediaLayoutItemViewModel*>())
            continue;

        MediaLayoutItemViewModel* srcItem = v.value<MediaLayoutItemViewModel*>();
        QString itemId = srcItem->itemId();
        Wisenet::uuid_string deviceId = srcItem->deviceId().toStdString();
        std::string channelId = srcItem->channelId().toStdString();
        MediaLayoutItemViewModel::ItemType itemType = srcItem->itemType();

        if(itemType != MediaLayoutItemViewModel::ItemType::Camera)
            continue;

        Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
        bool ret = QCoreServiceManager::Instance().DB()->FindChannel(deviceId, channelId, channel);
        if(!ret)
            continue;
#endif

        if(m_graphDataMap.contains(itemId)) {
            // 이미 있는 item인 경우 삭제하지 않음
            removedItemIds.removeOne(itemId);
        }
        else {
            // 없는 item인 경우 타임라인 신규 로드
            deviceMap[deviceId][channelId] = channelId;
            newChannelCount++;
        }

        m_graphDataMap[itemId].itemId = itemId;
        m_graphDataMap[itemId].deviceId = deviceId;
        m_graphDataMap[itemId].channelId = channelId;
        m_graphDataMap[itemId].channelName = QString::fromUtf8(channel.name.c_str());
    }

    SPDLOG_DEBUG("MultiGraphAreaModel::refreshLayoutItems() added:{}, removed:{}", newChannelCount, removedItemIds.count());

    if(deviceMap.count() == 0 && removedItemIds.count() == 0)
        return; // 추가, 삭제 된 item이 없으면 리턴

    // 삭제 된 item 제거
    foreach(QString removeId, removedItemIds) {
        if(!m_graphDataMap.contains(removeId))
            continue;

        for(auto itor = m_graphDataMap.begin() ; itor != m_graphDataMap.end() ; itor++) {
            if(!m_graphDataMap[removeId].graphImage.isNull() && itor.key() != removeId
                    && itor.value().deviceId == m_graphDataMap[removeId].deviceId
                    && itor.value().channelId == m_graphDataMap[removeId].channelId) {
                // 삭제 할 item과 같은 채널의 item이 있으면 graphImage 복사
                itor.value().graphImage = m_graphDataMap[removeId].graphImage;
                copiedChannelCount++;
            }
        }

        m_graphDataMap.remove(removeId);
    }

    SPDLOG_DEBUG("MultiGraphAreaModel::refreshLayoutItems() copied:{},", copiedChannelCount);

    // GraphData 추가/삭제 후 정렬하여 UI 업데이트
    sortGraphData();

    // 신규 Item에 대한 장비별 타임라인 요청
    QMapIterator<Wisenet::uuid_string, QMap<std::string, std::string>> itor(deviceMap);
    while (itor.hasNext()) {
        itor.next();
        getOneDayTimeline(itor.key(), itor.value().values(), selectedDate);
    }
}

// 이름순 정렬 및 중복채널 제거하여 전체 데이터를 갱신하는 함수
void MultiGraphAreaModel::sortGraphData()
{
    beginResetModel();

    QMap<Wisenet::uuid_string, QMap<std::string, std::string>> deviceMap;
    m_displayItemIdList.clear();

    QList<MultiGraphAreaData> graphDataList = m_graphDataMap.values();
    std::sort(graphDataList.begin(), graphDataList.end(), [](const MultiGraphAreaData data1, const MultiGraphAreaData data2) {
        return data1.channelName < data2.channelName;
    });

    for(auto& graphData : graphDataList) {
        if(deviceMap.find(graphData.deviceId) == deviceMap.end()
                || deviceMap[graphData.deviceId].find(graphData.channelId) == deviceMap[graphData.deviceId].end()) {
            deviceMap[graphData.deviceId][graphData.channelId] = graphData.channelId;
            m_displayItemIdList.push_back(graphData.itemId);
        }
    }

    endResetModel();
}

void MultiGraphAreaModel::getOneDayTimeline(Wisenet::uuid_string deviceId, QList<std::string> channelIdList, QDateTime selectedDate)
{
    auto request = std::make_shared<Wisenet::Device::DeviceGetTimelineRequest>();

    request->deviceID = deviceId;
    for(auto& channelId : channelIdList)
    {
        request->param.ChannelIDList.push_back(channelId);
    }

    QDateTime fromDate = selectedDate;
    fromDate.setTime(QTime(0,0,0,0));
    request->param.FromDate = fromDate.toMSecsSinceEpoch(); // 시작 날짜의 0시0분0초
    QDateTime toDate = fromDate.addDays(1);
    request->param.ToDate = toDate.toMSecsSinceEpoch();   // 시작 날짜 + 1일
    request->param.OverlappedID = -1;   // 모든 OverlappedID의 타임라인 로드
    //SPDLOG_DEBUG("MultiGraphAreaModel::getOneDayTimeline() fromDate:{}, toDate:{}", request->param.FromDate, request->param.ToDate);

#ifndef MEDIA_FILE_ONLY
    m_requestQueue.push_back(request);
    setIsLoading(true);
    m_requestTimer.start();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceGetTimeline, this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        m_requestQueue.removeOne(request);
        if(m_requestQueue.count() == 0)
            setIsLoading(false);

        auto timelineResponse = std::static_pointer_cast<Wisenet::Device::DeviceGetTimelineResponse>(reply->responseDataPtr);

        // 성공 시 Timeline 저장
        if(timelineResponse->isSuccess())
        {
            std::string channelId = "";
            int overlappedId = -1;
            QList<QString> channelIdList;

            for(auto& channelMap : timelineResponse->result.channelTimelineList)
            {
                channelId = channelMap.first;
                channelIdList.push_back(QString::fromStdString(channelId));
                for(auto& timelineMap : channelMap.second)
                {
                    overlappedId = timelineMap.first;
                    /*
                    SPDLOG_DEBUG("MultiGraphAreaModel::getOneDayTimeline() Response channelId:{}, overlappedId:{}, dateKey:{}, count:{}",
                                 channelId, overlappedId, request->param.FromDate, timelineMap.second.size());
                    */
                    m_deviceTimeline[request->deviceID].onedayTimelineList[channelId][request->param.FromDate].timelineList[overlappedId] = timelineMap.second;
                    Wisenet::Device::ChannelTimeline recordingPeriod;
                    for(auto& timeline : timelineMap.second) {
                        // Overlapped Track의 시작/끝 시간을 Map에 저장
                        if(recordingPeriod.startTime == 0 || recordingPeriod.startTime > timeline.startTime) {
                            recordingPeriod.startTime = timeline.startTime;
                        }
                        if(recordingPeriod.endTime == 0 || recordingPeriod.endTime < timeline.endTime) {
                            recordingPeriod.endTime = timeline.endTime;
                        }
                    }
                    m_deviceTimeline[request->deviceID].onedayTimelineList[channelId][request->param.FromDate].recordingPeriodMap[overlappedId] = recordingPeriod;
                }
            }

            emit timelineUpdated(QString::fromStdString(request->deviceID), channelIdList);
        }
    });
#endif
}

/*
void MultiGraphAreaModel::addLayoutItems(QVariantList items, QDateTime selectedDate)
{
    QMap<Wisenet::uuid_string, QMap<std::string, std::string>> deviceMap;;

    foreach(QVariant v, items)
    {
        if (!v.canConvert<MediaLayoutItemViewModel*>())
            continue;

        MediaLayoutItemViewModel* srcItem = v.value<MediaLayoutItemViewModel*>();
        QString itemId = srcItem->itemId();
        Wisenet::uuid_string deviceId = srcItem->deviceId().toStdString();
        std::string channelId = srcItem->channelId().toStdString();
        MediaLayoutItemViewModel::ItemType itemType = srcItem->itemType();

        if(itemType != MediaLayoutItemViewModel::ItemType::Camera)
            continue;

        m_graphDataMap[itemId].itemId = itemId;
        m_graphDataMap[itemId].deviceId = deviceId;
        m_graphDataMap[itemId].channelId = channelId;
        Wisenet::Device::Device::Channel channel;
#ifndef MEDIA_FILE_ONLY
        bool ret = QCoreServiceManager::Instance().DB()->FindChannel(deviceId, channelId, channel);
        if(!ret)
            continue;
#endif
        m_graphDataMap[itemId].channelName = QString::fromUtf8(channel.name.c_str());
        deviceMap[deviceId][channelId] = channelId;
    }

    // 신규 GraphData 추가 후 정렬하여 UI 업데이트
    sortGraphData();

    // 장비별 타임라인 요청
    QMapIterator<Wisenet::uuid_string, QMap<std::string, std::string>> itor(deviceMap);
    while (itor.hasNext()) {
        itor.next();
        getOneDayTimeline(itor.key(), itor.value().values(), selectedDate);
    }
}
*/

/*
void MultiGraphAreaModel::removeLayoutItems(QList<QString> itemIds)
{
    foreach(QString itemId, itemIds) {
        if(!m_graphDataMap.contains(itemId))
            continue;

        for(auto itor = m_graphDataMap.begin() ; itor != m_graphDataMap.end() ; itor++) {
            if(!m_graphDataMap[itemId].graphImage.isNull() && itor.key() != itemId
                    && itor.value().deviceId == m_graphDataMap[itemId].deviceId
                    && itor.value().channelId == m_graphDataMap[itemId].channelId) {
                // 삭제 할 item과 같은 채널의 item이 있으면 graphImage 복사
                itor.value().graphImage = m_graphDataMap[itemId].graphImage;
            }
        }

        m_graphDataMap.remove(itemId);  // item 삭제
    }

    // Item 삭제 후 정렬하여 UI 업데이트
    sortGraphData();
}
*/

// 선택 날짜 변경 처리 함수 (표시 중인 graph의 선택 날짜 타임라인 추가 로딩)
void MultiGraphAreaModel::selectedDateChanged(QDateTime selectedDate)
{
    m_selectedDate = selectedDate;
    selectedDate.setTime(QTime(0,0,0,0));
    int64_t dateKey = selectedDate.toMSecsSinceEpoch();

    QMap<Wisenet::uuid_string, QMap<std::string, std::string>> deviceMap;

    for(auto& itemId : m_displayItemIdList) {
        if(m_graphDataMap.find(itemId) == m_graphDataMap.end()) {
            continue;   // item의 deviceId, channelId 정보가 없을 때
        }

        Wisenet::uuid_string deviceId = m_graphDataMap[itemId].deviceId;
        std::string channelId = m_graphDataMap[itemId].channelId;

        // 타임라인 유무 확인. 타임라인이 없으면 Map에 추가하여 장비별 일괄 로딩
        if(m_deviceTimeline.find(deviceId) == m_deviceTimeline.end()) {
            // device의 타임라인이 하나도 없는 경우
            deviceMap[deviceId][channelId] = channelId;
            continue;
        }
        else if(m_deviceTimeline[deviceId].onedayTimelineList.find(channelId) == m_deviceTimeline[deviceId].onedayTimelineList.end()) {
            // channel의 타임라인이 하나도 없는 경우
            deviceMap[deviceId][channelId] = channelId;
            continue;
        }
        else if(m_deviceTimeline[deviceId].onedayTimelineList[channelId].find(dateKey) == m_deviceTimeline[deviceId].onedayTimelineList[channelId].end()) {
            // 선택 날짜의 타임라인이 없는 경우
            deviceMap[deviceId][channelId] = channelId;
            continue;
        }
    }

    // 장비별 타임라인 요청
    QMapIterator<Wisenet::uuid_string, QMap<std::string, std::string>> itor(deviceMap);
    while (itor.hasNext()) {
        itor.next();
        getOneDayTimeline(itor.key(), itor.value().values(), selectedDate);
    }
}

void MultiGraphAreaModel::focusedItemChanged(QString deviceId, QString channelId)
{
    // 신규 focusedItem의 index를 찾아 signal로 전달 -> qml에서 해당 위치로 스크롤
    for(int index = 0 ; index < m_displayItemIdList.count() ; index++) {
        QString itemId = m_displayItemIdList[index];
        if(m_graphDataMap.find(itemId) == m_graphDataMap.end()) {
            continue;   // item의 deviceId, channelId 정보가 없을 때
        }

        if(m_graphDataMap[itemId].deviceId == deviceId.toStdString()
                && m_graphDataMap[itemId].channelId == channelId.toStdString()) {
            emit focusedItemIndexChanged(index);
            return;
        }
    }
}

void MultiGraphAreaModel::recordingTypeFilterChanged(const quint32 filter)
{
    m_recordingTypeFilter = (Wisenet::Device::RecordingType)filter;
}

float MultiGraphAreaModel::getOverlappedBlockPos(int modelIndex, int overlapIndex)
{
    if(modelIndex < 0 || modelIndex >= m_displayItemIdList.size())
        return 0;

    QString itemId = m_displayItemIdList[modelIndex];
    if(m_graphDataMap.find(itemId) == m_graphDataMap.end())
        return 0;

    if(overlapIndex < 0 || overlapIndex >= m_graphDataMap[itemId].overlappedBlockList.count())
        return 0;

    return m_graphDataMap[itemId].overlappedBlockList[overlapIndex].startPosition;
}

float MultiGraphAreaModel::getOverlappedBlockWidth(int modelIndex, int overlapIndex)
{
    if(modelIndex < 0 || modelIndex >= m_displayItemIdList.size())
        return 0;

    QString itemId = m_displayItemIdList[modelIndex];
    if(m_graphDataMap.find(itemId) == m_graphDataMap.end())
        return 0;

    if(overlapIndex < 0 || overlapIndex >= m_graphDataMap[itemId].overlappedBlockList.count())
        return 0;

    float startPos = m_graphDataMap[itemId].overlappedBlockList[overlapIndex].startPosition;
    float endPos = m_graphDataMap[itemId].overlappedBlockList[overlapIndex].endPosition;

    return endPos - startPos;
}

QVariantList MultiGraphAreaModel::getOverlappedIdList(QString deviceId, QString channelId, qulonglong currentTime)
{
    QVariantList overlappedIdList;

    auto deviceItor = m_deviceTimeline.find(deviceId.toStdString());
    if(deviceItor == m_deviceTimeline.end())
        return overlappedIdList;

    auto channelItor = deviceItor->onedayTimelineList.find(channelId.toStdString());
    if(channelItor == deviceItor->onedayTimelineList.end())
        return overlappedIdList;

    QDateTime date = QDateTime::fromMSecsSinceEpoch(currentTime);
    date.setTime(QTime(0,0,0,0));
    auto dateItor = channelItor->second.find(date.toMSecsSinceEpoch());
    if(dateItor == channelItor->second.end())
        return overlappedIdList;

    for(auto itor = dateItor->second.recordingPeriodMap.begin() ; itor != dateItor->second.recordingPeriodMap.end() ; itor++) {
        if(itor->second.startTime <= currentTime && currentTime <= itor->second.endTime)
            overlappedIdList.push_back(itor->first);
    }

    return overlappedIdList;
}

qulonglong MultiGraphAreaModel::getValidSeekTime(QString deviceId, QString channelId, int overlappedId, qulonglong seekTime, bool searchForward, bool ignoreTypeFilter)
{
    qulonglong validSeekTime = 0;

    auto deviceItor = m_deviceTimeline.find(deviceId.toStdString());
    if(deviceItor == m_deviceTimeline.end())
        return 0;

    auto channelItor = deviceItor->onedayTimelineList.find(channelId.toStdString());
    if(channelItor == deviceItor->onedayTimelineList.end())
        return 0;

    QDateTime date = QDateTime::fromMSecsSinceEpoch(seekTime);
    date.setTime(QTime(0,0,0,0));
    auto dateItor = channelItor->second.find(date.toMSecsSinceEpoch());
    if(dateItor == channelItor->second.end())
        return 0;

    auto itor = dateItor->second.timelineList.find(overlappedId);
    if(itor == dateItor->second.timelineList.end())
        return 0;

    for(auto& record : itor->second)
    {
        if((searchForward && record.endTime < seekTime) || (!searchForward && record.startTime > seekTime))
            continue; // 검색 방향이 아니면 continue

        if(!ignoreTypeFilter && (record.type & m_recordingTypeFilter) != record.type)
            continue;   // 필터 된 녹화타입이면 continue

        if(record.startTime <= seekTime && seekTime <= record.endTime)
            return seekTime;   // seekTime을 포함하는 record가 있으면 그대로 리턴

        if(searchForward && (validSeekTime == 0 || validSeekTime > record.startTime))
            validSeekTime = record.startTime;   // 정방향으로 가장 가까운 record의 시작 시간
        else if(!searchForward && (validSeekTime == 0 || validSeekTime < record.endTime))
            validSeekTime = record.endTime;   // 역방향으로 가장 가까운 record의 끝 시간
    }

    return validSeekTime;
}

void MultiGraphAreaModel::onRequestTimerTimeout()
{
    qDebug() << "MultiGraphAreaModel::onRequestTimerTimeout()";
    m_requestTimer.stop();
    m_requestQueue.clear();
    setIsLoading(false);
}

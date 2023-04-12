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

#include "GraphAreaModel.h"
#include <QPainter>
#include <QDebug>

GraphAreaModel::GraphAreaModel()
{
    ::qRegisterMetaType<GraphAreaModel::qRecordType>("GraphAreaModel::qRecordType");
    /*
    qulonglong startTime = 1614524400000;
    qulonglong endTime = 1617202800000;

    int index = 0;
    std::vector<Wisenet::Device::ChannelTimeline> timelineList;
    m_sampleTimeline.channelTimelineList["1"][1] = timelineList;
    while(startTime < endTime)
    {
        Wisenet::Device::ChannelTimeline timeline;
        timeline.startTime = startTime;
        timeline.endTime = startTime + 60000;
        timeline.type = index%2 == 0 ? Wisenet::Device::RecordingType::Normal : Wisenet::Device::RecordingType::Event;
        //timeline.type = Wisenet::Device::RecordingType::Normal;

        m_sampleTimeline.channelTimelineList["1"][1].push_back(timeline);

        startTime += 120000;
        index++;
    }

    qDebug("[timeline] timeline Data Count:%d", index);
    */
}

GraphAreaModel::~GraphAreaModel()
{
}

void GraphAreaModel::connectMediaControlModelSignals(MediaControlModel* sender)
{
    connect(sender, &MediaControlModel::selectedTimelineUpdated, this, &GraphAreaModel::onSelectedChannelTimelineUpdated);
    connect(sender, &MediaControlModel::recordingTypeFilterChanged, this, &GraphAreaModel::onRecordingTypeFilterChanged);
}

void GraphAreaModel::onSelectedChannelTimelineUpdated(const std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>& selectedTimeline,
                                                      const std::vector<int>& overlappedIdList)
{
    m_selectedTimeline = selectedTimeline;
    m_overlappedIdList = overlappedIdList;

    // 각 Overlapped Track의 시작/끝 시간을 Map에 저장
    m_recordingPeriodMap.clear();
    auto itor = selectedTimeline.begin();
    while(itor != selectedTimeline.end()) {        
        GraphAreaData recordingPeriod;
        for(auto& timeline : itor->second) {
            if(recordingPeriod.startTime == 0 || recordingPeriod.startTime > timeline.startTime) {
                recordingPeriod.startTime = timeline.startTime;
            }
            if(recordingPeriod.endTime == 0 || recordingPeriod.endTime < timeline.endTime) {
                recordingPeriod.endTime = timeline.endTime;
            }
        }
        m_recordingPeriodMap[itor->first] = recordingPeriod;

        itor++;
    }

    // 각 Overlapped Track이 실제로 겹치는 구간을 리스트로 저장
    m_overlappedTimeList.clear();
    QList<GraphAreaData> recordingPeriodList = m_recordingPeriodMap.values();
    for(int i=0 ; i<recordingPeriodList.count() ; i++) {
        for(int j=i ; j<recordingPeriodList.count() ; j++) {
            if(i == j)
                continue;

            if(recordingPeriodList[i].endTime < recordingPeriodList[j].startTime
                    || recordingPeriodList[i].startTime > recordingPeriodList[j].endTime)
                continue;   // i track과 j track에 겹치는 구간이 없음

            GraphAreaData overlappedTime;
            overlappedTime.startTime = std::max(recordingPeriodList[i].startTime, recordingPeriodList[j].startTime);
            overlappedTime.endTime = std::min(recordingPeriodList[i].endTime, recordingPeriodList[j].endTime);
            m_overlappedTimeList.push_back(overlappedTime);
        }
    }
    std::sort(m_overlappedTimeList.begin(), m_overlappedTimeList.end(), [](const GraphAreaData data1, const GraphAreaData data2) {
        return data1.startTime < data2.startTime;
    });

    emit timelineUpdated();
}

void GraphAreaModel::onRecordingTypeFilterChanged(const quint32 filter)
{
    if(m_recordingTypeFilter == (Wisenet::Device::RecordingType)filter)
        return;

    m_recordingTypeFilter = (Wisenet::Device::RecordingType)filter;
    emit timelineUpdated();
}

// 전체 데이터 갱신 함수
void GraphAreaModel::refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, int overlappedId)
{
    m_graphData.clear();
    m_overlappedData.clear();

    qulonglong visibleRange = visibleEndTime - visibleStartTime;

    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>> selectedTimeline = m_selectedTimeline;
    QList<GraphAreaData> overlappedTimeList = m_overlappedTimeList;
    for(int i = overlappedTimeList.count() - 1 ; i >= 0 ; i--) {
        if(overlappedTimeList[i].endTime <= visibleStartTime || overlappedTimeList[i].startTime >= visibleEndTime) {
            overlappedTimeList.removeAt(i); // 화면에 표시되지 않는 중복구간 데이터 제거
        }
    }

    // graphData 갱신
    auto itor = selectedTimeline.begin();
    while(itor != selectedTimeline.end()) {
        GraphAreaData normalData;
        normalData.recordType = qRecordType::RecType_Normal;
        GraphAreaData eventData;
        eventData.recordType = qRecordType::RecType_Event;
        QVector<GraphAreaData> normalDataList;
        QVector<GraphAreaData> eventDataList;
        bool isSelectedOverlappedId = overlappedId == itor->first || overlappedId == -1;

        for(auto& timeline : itor->second) {
            appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth, isSelectedOverlappedId,
                                overlappedTimeList, timeline,
                                normalData, eventData, normalDataList, eventDataList);
        }

        if(eventData.startTime != 0)
            eventDataList.append(eventData);
        if(normalData.startTime != 0)
            normalDataList.append(normalData);

        for(auto& data : normalDataList)
            m_graphData.append(data);
        for(auto& data : eventDataList)
            m_graphData.append(data);

        itor++;
    }

    // overlapped data 갱신
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
                // 마지막 데이터를 m_overlappedData에 추가
                GraphAreaData overlappedData;
                overlappedData.startPosition = overlapStartPos;
                overlappedData.endPosition = overlapEndPos;
                m_overlappedData.push_back(overlappedData);
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
        // 마지막 데이터를 m_overlappedData에 추가
        GraphAreaData overlappedData;
        overlappedData.startPosition = overlapStartPos;
        overlappedData.endPosition = overlapEndPos;
        m_overlappedData.push_back(overlappedData);
    }

    // 단일 QImage로 그림
    QImage newImage = QImage(controlWidth, 6, QImage::Format_ARGB32);
    newImage.fill(QColor(49, 49, 49));
    QPainter qPainter(&newImage);
    QColor normalColor = QColor(141, 198, 63);
    QColor normalFilteredColor = QColor(141, 198, 63, 76);
    QColor eventColor = QColor(255, 0, 0);
    QColor eventFilteredColor = QColor(255, 0, 0, 76);

    for(auto& data : m_graphData) {
        QColor color = normalColor;
        if(data.recordType == qRecordType::RecType_NormalFiltered)
            color = normalFilteredColor;
        else if(data.recordType == qRecordType::RecType_Event)
            color = eventColor;
        else if(data.recordType == qRecordType::RecType_EventFiltered)
            color = eventFilteredColor;

        qPainter.fillRect(data.startPosition, 0, data.endPosition - data.startPosition, 6, color);
    }

    qPainter.end();
    setGraphImage(newImage);

    //SPDLOG_DEBUG("GraphAreaModel::refreshGraphDataList() after image draw loop. count:{}", m_graphData.count());
}

void GraphAreaModel::appendGraphAreaData(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, bool isSelectedOverlappedId,
                         QList<GraphAreaData>& overlappedTimeList, Wisenet::Device::ChannelTimeline& timeline,
                         GraphAreaData& normalData, GraphAreaData& eventData, QVector<GraphAreaData>& normalDataList, QVector<GraphAreaData>& eventDataList)
{
    if(timeline.endTime <= visibleStartTime || timeline.startTime >= visibleEndTime)
        return;

    qulonglong visibleRange = visibleEndTime - visibleStartTime;

    // 선택 된 overlappedId가 아니면 중복구간에 겹치는 부분을 제외
    if(!m_isSmartSearchMode && !isSelectedOverlappedId) {
        for(auto& overlappedTime : overlappedTimeList) {
            if(timeline.endTime <= overlappedTime.startTime || timeline.startTime >= overlappedTime.endTime)
                continue;

            // 겹치는 부분을 제외하고 앞쪽 뒤쪽 데이터를 추가
            if(timeline.startTime <= overlappedTime.startTime) {
                Wisenet::Device::ChannelTimeline frontTimeline = timeline;
                frontTimeline.endTime = overlappedTime.startTime;
                if(frontTimeline.endTime - frontTimeline.startTime > 0) {
                    appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth, isSelectedOverlappedId,
                                        overlappedTimeList, frontTimeline,
                                        normalData, eventData, normalDataList, eventDataList);
                }
            }
            if(timeline.endTime >= overlappedTime.endTime) {
                Wisenet::Device::ChannelTimeline rearTimeline = timeline;
                rearTimeline.startTime = overlappedTime.endTime;
                if(rearTimeline.endTime - rearTimeline.startTime > 0) {
                    appendGraphAreaData(visibleStartTime, visibleEndTime, controlWidth, isSelectedOverlappedId,
                                        overlappedTimeList, rearTimeline,
                                        normalData, eventData, normalDataList, eventDataList);
                }
            }

            return;
        }
    }

    float startPos = timeline.startTime <= visibleStartTime ? 0 : controlWidth * (timeline.startTime - visibleStartTime) / visibleRange;
    float endPos = timeline.endTime >= visibleEndTime ? controlWidth : controlWidth * (timeline.endTime - visibleStartTime) / visibleRange;
    if(endPos - startPos < 1)
        endPos += 1;

    qRecordType recordType = qRecordType::RecType_None;
    if(!m_isSmartSearchMode) {
        if(timeline.type == Wisenet::Device::RecordingType::Normal || timeline.type == Wisenet::Device::RecordingType::Manual) {
            if((timeline.type & m_recordingTypeFilter) == timeline.type)
                recordType = qRecordType::RecType_Normal;
            else
                recordType = qRecordType::RecType_NormalFiltered;
        }
        else {
            if((timeline.type & m_recordingTypeFilter) == timeline.type || timeline.type == Wisenet::Device::RecordingType::None)
                recordType = qRecordType::RecType_Event;
            else
                recordType = qRecordType::RecType_EventFiltered;
        }
    }
    else {
        // SmartSearchMode인 경우 타입을 별도 처리
        if(m_smartSearchTypeFilter.contains((int)timeline.type))
            recordType = qRecordType::RecType_Event;
        else
            recordType = qRecordType::RecType_EventFiltered;
    }

    if(recordType == qRecordType::RecType_Event || recordType == qRecordType::RecType_EventFiltered) {
        if(eventData.recordType != recordType ||
                endPos < eventData.startPosition || startPos > eventData.endPosition)
        {
            // 바로 앞 데이터와 인접하지 않은 경우
            if(eventData.startTime != 0)
                eventDataList.append(eventData); // 마지막 데이터 append

            eventData = GraphAreaData(); // 신규 데이터 생성
            eventData.startTime = timeline.startTime;
            eventData.endTime = timeline.endTime;
            eventData.startPosition = startPos;
            eventData.endPosition = endPos;
            eventData.recordType = recordType;
        }
        else {
            // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
            if(startPos <= eventData.startPosition) {
                eventData.startTime = timeline.startTime;
                eventData.startPosition = startPos;
            }
            if(endPos >= eventData.endPosition) {
                eventData.endTime = timeline.endTime;
                eventData.endPosition = endPos;
            }
        }
    }
    else {
        if(normalData.recordType != recordType || endPos < normalData.startPosition || startPos > normalData.endPosition) {
            // 바로 앞 데이터와 인접하지 않은 경우
            if(normalData.startTime != 0)
                normalDataList.append(normalData); // 마지막 데이터 append

            normalData = GraphAreaData(); // 신규 데이터 생성
            normalData.startTime = timeline.startTime;
            normalData.endTime = timeline.endTime;
            normalData.startPosition = startPos;
            normalData.endPosition = endPos;
            normalData.recordType = recordType;
        }
        else {
            // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
            if(startPos <= normalData.startPosition) {
                normalData.startTime = timeline.startTime;
                normalData.startPosition = startPos;
            }
            if(endPos >= normalData.endPosition) {
                normalData.endTime = timeline.endTime;
                normalData.endPosition = endPos;
            }
        }
    }
}

// 특정 index의 데이터 획득 함수들
float GraphAreaModel::graphStartPosition(int index)
{
    if(m_graphData.count() > index)
        return m_graphData[index].startPosition;

    return 0;
}

float GraphAreaModel::graphWidth(int index)
{
    if(m_graphData.count() > index)
        return m_graphData[index].endPosition - m_graphData[index].startPosition;

    return 0;
}

float GraphAreaModel::overlappedStartPosition(int index)
{
    if(m_overlappedData.count() > index)
        return m_overlappedData[index].startPosition;

    return 0;
}

float GraphAreaModel::overlappedWidth(int index)
{
    if(m_overlappedData.count() > index)
        return m_overlappedData[index].endPosition - m_overlappedData[index].startPosition;

    return 0;
}

GraphAreaModel::qRecordType GraphAreaModel::recordType(int index)
{
    if(m_graphData.count() > index)
        return m_graphData[index].recordType;

    return GraphAreaModel::qRecordType::RecType_None;
}
//

qulonglong GraphAreaModel::getNextRecordBlockTime(qulonglong currentTime, bool searchForward, bool getStartTime, bool ignoreTypeFilter, int overlappedId)
{
    m_nextRecordOverlappedId = overlappedId;
    qulonglong recordTime = 0;

    // 입력 받은 OverlappedId 먼저 검색
    recordTime = _getNextRecordBlockTime(currentTime, searchForward, getStartTime, ignoreTypeFilter, overlappedId);
    if(recordTime != 0 || m_isSmartSearchMode) {
        return recordTime;
    }

    // 결과가 없으면 나머지 OverlappedId 검색
    for(auto itor = m_recordingPeriodMap.begin() ; itor != m_recordingPeriodMap.end() ; itor++) {
        if(itor.key() == overlappedId)
            continue;

        recordTime = _getNextRecordBlockTime(currentTime, searchForward, getStartTime, ignoreTypeFilter, itor.key());
        if(recordTime != 0) {
            m_nextRecordOverlappedId = itor.key();
            return recordTime;
        }
    }

    return 0;
}

qulonglong GraphAreaModel::_getNextRecordBlockTime(qulonglong currentTime, bool searchForward, bool getStartTime, bool ignoreTypeFilter, int overlappedId)
{
    if(m_selectedTimeline.find(overlappedId) == m_selectedTimeline.end()) {
        return 0;   // overlappedId의 타임라인이 없는 경우
    }

    if(!m_isSmartSearchMode) {
        if(m_recordingPeriodMap.find(overlappedId) == m_recordingPeriodMap.end()) {
            return 0;   // overlappedId의 녹화 구간이 없는 경우
        }

        qulonglong startTime = m_recordingPeriodMap[overlappedId].startTime;
        qulonglong endTime = m_recordingPeriodMap[overlappedId].endTime;
        if((searchForward && endTime < currentTime) || (!searchForward && startTime > currentTime)) {
            return 0;   // 녹화 구간에 검색 방향의 데이터가 없는 경우
        }
    }

    std::vector<Wisenet::Device::ChannelTimeline> timeline = m_selectedTimeline[overlappedId];

    if(searchForward) {
        // 정방향 검색
        for(auto& record : timeline) {
            if(record.startTime < currentTime)
                continue;   // 과거 record면 continue

            if(!ignoreTypeFilter && isFilteredType(record.type))
                continue;   // 필터 된 녹화타입이면 continue

            if(getStartTime)
                return record.startTime;
            else
                return record.endTime;
        }
    }
    else {
        // 역방향 검색
        std::vector<Wisenet::Device::ChannelTimeline>::reverse_iterator rit;
        for (rit = timeline.rbegin() ; rit != timeline.rend() ; rit++) {
            if(rit->endTime > currentTime)
                continue;   // 미래 record면 continue

            if(!ignoreTypeFilter && isFilteredType(rit->type))
                continue;   // 필터 된 녹화타입이면 continue

            if(getStartTime)
                return rit->startTime;
            else
                return rit->endTime;
        }
    }

    return 0;
}

qulonglong GraphAreaModel::getCurrentRecordBlockEndTime(qulonglong currentTime, bool searchForward, bool ignoreTypeFilter, int overlappedId)
{
    if(m_selectedTimeline.find(overlappedId) == m_selectedTimeline.end())
        return 0;

    Wisenet::Device::ChannelTimeline foundedRecord;

    std::vector<Wisenet::Device::ChannelTimeline> timeline = m_selectedTimeline[overlappedId];

    for(auto& record : timeline)
    {
        if(record.endTime < currentTime || record.startTime > currentTime)
            continue;   // 현재 시간을 포함하지 않는 record면 continue

        if(!ignoreTypeFilter && isFilteredType(record.type))
            continue;   // 필터 된 녹화타입이면 continue

        if(foundedRecord.startTime == 0)
        {
            foundedRecord = record;
            continue;   // 첫 번쨰로 찾은 Record 저장
        }

        if((searchForward && record.endTime > foundedRecord.endTime)
                || (!searchForward && record.startTime < foundedRecord.startTime))
        {
            foundedRecord = record; // 진행 방향으로, 더 나중에 끝나는 Record 저장
        }
    }

    if(searchForward)
        return foundedRecord.endTime;
    else
        return foundedRecord.startTime;
}

/// overlappedId 전체 녹화 데이터의 시작시간 or 끝시간 획득 함수
qulonglong GraphAreaModel::getCurrentOverlappedIdTime(bool searchForward, int overlappedId)
{
    if(!m_recordingPeriodMap.contains(overlappedId))
        return 0;

    if(searchForward)
        return m_recordingPeriodMap[overlappedId].endTime;
    else
        return m_recordingPeriodMap[overlappedId].startTime;
}

/// currentTime 기준으로 다음 재생 할 overlappedId와 시간을 찾는 함수
qulonglong GraphAreaModel::getNextOverlappedIdTime(qulonglong currentTime, bool searchForward, int overlappedId)
{
    m_nextRecordOverlappedId = overlappedId;

    // currentTime을 포함하고, 진행 방향으로 가장 늦게 끝나는 overlappedId 검색
    qulonglong endTime = 0;
    for(auto itor = m_recordingPeriodMap.begin() ; itor != m_recordingPeriodMap.end() ; itor++) {
        if(itor.key() == overlappedId)
            continue;   // 현재 선택 된 overlappedId 제외

        if(itor.value().endTime < currentTime || itor.value().startTime > currentTime)
            continue;   // currentTime을 포함하지 않는 overlappedId 제외

        // 진행 방향으로 가장 늦게 끝나는 overlappedId 저장
        if(searchForward) {
            if(endTime <= itor.value().endTime) {
                endTime = itor.value().endTime;
                m_nextRecordOverlappedId = itor.key();
            }
        }
        else {
            if(endTime == 0 || endTime >= itor.value().startTime) {
                endTime = itor.value().startTime;
                m_nextRecordOverlappedId = itor.key();
            }
        }
    }

    if(m_nextRecordOverlappedId == overlappedId)
        return 0;   // 다음 재생 할 overlappedId가 없는 경우

    // 다음 재생 할 overlappedId의 녹화 데이터에서 다음 재생 시간 검색
    if(m_selectedTimeline.find(m_nextRecordOverlappedId) == m_selectedTimeline.end())
        return 0;

    std::vector<Wisenet::Device::ChannelTimeline> timeline = m_selectedTimeline[m_nextRecordOverlappedId];
    for(auto& record : timeline) {
        if(record.startTime <= currentTime && currentTime <= record.endTime)
            return currentTime;   // currentTime을 포함하는 record가 있으면 currentTime 리턴
    }

    // currentTime을 포함하는 record가 없으면 진행 방향의 가장 빠른 record 검색
    return _getNextRecordBlockTime(currentTime, searchForward, searchForward, true, m_nextRecordOverlappedId);
}

bool GraphAreaModel::isFilteredType(Wisenet::Device::RecordingType type)
{
    // 필터 된 녹화타입이면 true, 아니면 false 리턴
    if(!m_isSmartSearchMode && (type & m_recordingTypeFilter) != type)
        return true;

    if(m_isSmartSearchMode && !m_smartSearchTypeFilter.contains((int)type))
        return true;

    return false;
}

qulonglong GraphAreaModel::firstRecordTime()
{
    /*
    if(m_selectedTimeline.size() == 0)
        return 0;

    return m_selectedTimeline[0].startTime;
    */

    return 0;
}

qulonglong GraphAreaModel::lastRecordTime()
{
    /*
    if(m_selectedTimeline.size() == 0)
        return 0;

    return m_selectedTimeline[m_selectedTimeline.size()-1].endTime;
    */

    return 0;
}

void GraphAreaModel::setSmartSearchResult(QVariantList resultList)
{
    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>> resultTimeline;

    foreach(QVariant v, resultList)
    {
        if (v.canConvert<QVariantList>())
        {
            // 시간, type 순서로 리스트에서 읽어서 저장
            QVariantList result = v.value<QVariantList>();
            if(result.count() < 2)
                continue;

            Wisenet::Device::ChannelTimeline timeline;

            if(result[0].canConvert<qint64>()) {
                // 이벤트 발생 이전 3초 ~ 이후 3초 구간 타임라인 추가
                timeline.startTime = result[0].value<qint64>() - 3000;
                timeline.endTime = timeline.startTime + 6000;
            }

            if(result[1].canConvert<int>()) {
                timeline.type = (Wisenet::Device::RecordingType)result[1].value<int>();
            }

            resultTimeline[-1].push_back(timeline);
        }
    }

    std::sort(resultTimeline[-1].begin(), resultTimeline[-1].end(), [](const Wisenet::Device::ChannelTimeline r1, const Wisenet::Device::ChannelTimeline r2) {
        return r1.startTime < r2.startTime;
    });
    m_selectedTimeline = resultTimeline;

    emit timelineUpdated();
}

void GraphAreaModel::setSmartSearchTypeFilter(QVariantList filter)
{
    m_smartSearchTypeFilter = filter;
    emit timelineUpdated();
}

void GraphAreaModel::setGraphImage(const QImage &image)
{
    if(m_graphImage != image) {
        m_graphImage = image;
        emit graphImageChanged();
    }
}

/// currentTime을 포함하는 overlapped Id list 획득 함수
QVariantList GraphAreaModel::getOverlappedIdList(qulonglong currentTime)
{
    QVariantList overlappedIdList;

    for(auto itor = m_recordingPeriodMap.begin() ; itor != m_recordingPeriodMap.end() ; itor++) {
        if(itor.value().startTime <= currentTime && currentTime <= itor.value().endTime)
            overlappedIdList.push_back(itor.key());
    }

    // 최신 overlappedId가 먼저 나오게 정렬
    sortOverlappedIdList(overlappedIdList);

    return overlappedIdList;
}

/// startTime~endTime 구간과 겹치는 구간이 있는 overlapped Id list 획득 함수
QVariantList GraphAreaModel::getOverlappedIdList(qulonglong startTime, qulonglong endTime)
{
    QVariantList overlappedIdList;

    for(auto itor = m_recordingPeriodMap.begin() ; itor != m_recordingPeriodMap.end() ; itor++) {
        if(startTime < itor.value().endTime && endTime > itor.value().startTime)
            overlappedIdList.push_back(itor.key());
    }

    // 최신 overlappedId가 먼저 나오게 정렬
    sortOverlappedIdList(overlappedIdList);

    return overlappedIdList;
}

void GraphAreaModel::sortOverlappedIdList(QVariantList& overlappedIdList)
{
    if(overlappedIdList.count() == 0)
        return;

    std::sort(overlappedIdList.begin(), overlappedIdList.end(), [this](const QVariant data1, const QVariant data2) {
        if(!data1.canConvert<int>() || !data2.canConvert<int>())
            return true;

        int track1 = data1.toInt();
        int track2 = data2.toInt();
        int overlappedIdCount = (int)m_overlappedIdList.size();
        int track1Index = overlappedIdCount;
        int track2Index = overlappedIdCount;

        for(int i=0 ; i<overlappedIdCount ; i++) {
            if(m_overlappedIdList[i] == track1)
                track1Index = i;
            if(m_overlappedIdList[i] == track2)
                track2Index = i;
        }

        return track1Index <= track2Index;
    });
}

qulonglong GraphAreaModel::getValidSeekTime(int overlappedId, qulonglong seekTime, bool searchForward, bool ignoreTypeFilter)
{
    qulonglong validSeekTime = 0;

    auto itor = m_selectedTimeline.find(overlappedId);
    if(itor == m_selectedTimeline.end())
        return 0; // overlappedId의 타임라인이 없으면 리턴

    std::vector<Wisenet::Device::ChannelTimeline> timeline = m_selectedTimeline[overlappedId];
    for(auto& record : timeline)
    {
        if((searchForward && record.endTime < seekTime) || (!searchForward && record.startTime > seekTime))
            continue; // 검색 방향이 아니면 continue

        if(!ignoreTypeFilter && isFilteredType(record.type))
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

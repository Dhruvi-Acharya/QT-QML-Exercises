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

#ifndef GRAPHAREAMODEL_H
#define GRAPHAREAMODEL_H

#include <QObject>
#include "DeviceClient/DeviceStructure.h"
#include "MediaControlModel.h"

class GraphAreaModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int graphDataCount READ graphDataCount CONSTANT)
    Q_PROPERTY(qulonglong firstRecordTime READ firstRecordTime CONSTANT)
    Q_PROPERTY(qulonglong lastRecordTime READ lastRecordTime CONSTANT)
    Q_PROPERTY(bool isSmartSearchMode READ isSmartSearchMode WRITE setIsSmartSearchMode)
    Q_PROPERTY(QVariantList smartSearchTypeFilter READ smartSearchTypeFilter WRITE setSmartSearchTypeFilter)
    Q_PROPERTY(QImage graphImage READ graphImage WRITE setGraphImage NOTIFY graphImageChanged)
    Q_PROPERTY(int overlappedDataCount READ overlappedDataCount CONSTANT)
    Q_PROPERTY(int nextRecordOverlappedId READ nextRecordOverlappedId CONSTANT)    // getNextRecordBlockTime()으로 다음 record를 검색했을 때, 해당 위치의 overlappedId

public:
    enum qRecordType
    {
        RecType_None,
        RecType_Normal,
        RecType_Event,
        RecType_NormalFiltered,
        RecType_EventFiltered,
    };
    Q_ENUM(qRecordType)

    struct GraphAreaData
    {
        float startPosition = 0;
        float endPosition = 0;
        qRecordType recordType = qRecordType::RecType_None;
        qulonglong startTime = 0;
        qulonglong endTime = 0;
    };

    GraphAreaModel();
    ~GraphAreaModel();

    // Q_PROPERTY
    int graphDataCount() {return m_graphData.count();}
    qulonglong firstRecordTime();
    qulonglong lastRecordTime();
    bool isSmartSearchMode() {return m_isSmartSearchMode;}
    void setIsSmartSearchMode(bool modeOn) {m_isSmartSearchMode = modeOn;}
    QVariantList smartSearchTypeFilter() {return m_smartSearchTypeFilter;}
    void setSmartSearchTypeFilter(QVariantList filter);
    QImage graphImage() {return m_graphImage;}
    void setGraphImage(const QImage &image);
    int overlappedDataCount() {return m_overlappedData.count();}
    int nextRecordOverlappedId() {return m_nextRecordOverlappedId;}

    // Q_INVOKABLE
    Q_INVOKABLE void refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, int overlappedId = -1);
    Q_INVOKABLE float graphStartPosition(int index);
    Q_INVOKABLE float graphWidth(int index);
    Q_INVOKABLE GraphAreaModel::qRecordType recordType(int index);
    Q_INVOKABLE void connectMediaControlModelSignals(MediaControlModel* sender);
    Q_INVOKABLE qulonglong getCurrentRecordBlockEndTime(qulonglong currentTime, bool searchForward, bool ignoreTypeFilter = false, int overlappedId = -1);
    Q_INVOKABLE qulonglong getNextRecordBlockTime(qulonglong currentTime, bool searchForward, bool getStartTime, bool ignoreTypeFilter = false, int overlappedId = -1);
    Q_INVOKABLE qulonglong getCurrentOverlappedIdTime(bool searchForward, int overlappedId);
    Q_INVOKABLE qulonglong getNextOverlappedIdTime(qulonglong currentTime, bool searchForward, int overlappedId);
    Q_INVOKABLE void setSmartSearchResult(QVariantList resultList);    // 스마트 검색 결과를 graph 자료구조로 변환하여 저장하는 함수
    Q_INVOKABLE float overlappedStartPosition(int index);
    Q_INVOKABLE float overlappedWidth(int index);
    Q_INVOKABLE QVariantList getOverlappedIdList(qulonglong currentTime);
    Q_INVOKABLE QVariantList getOverlappedIdList(qulonglong startTime, qulonglong endTime);
    Q_INVOKABLE qulonglong getValidSeekTime(int overlappedId, qulonglong seekTime, bool searchForward, bool ignoreTypeFilter = false);

signals:
    void timelineUpdated();
    void graphImageChanged();

public slots:
    void onSelectedChannelTimelineUpdated(const std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>& selectedTimeline,
                                          const std::vector<int>& overlappedIdList);
    void onRecordingTypeFilterChanged(const quint32 filter);

private:
    QList<GraphAreaData> m_graphData;   // 화면에 최종 표출하는 타임라인 데이터
    QList<GraphAreaData> m_overlappedData;  // 화면에 최종 표출하는 중복구간 데이터
    QImage m_graphImage;

    // 현재 선택 채널에 대한 실제 타임라인 / 중복구간 데이터들
    Wisenet::Device::RecordingTimelineViewResult m_sampleTimeline;
    std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>> m_selectedTimeline;  // 전체 타임라인 데이터. key : Overlapped ID, value : Timeline List
    std::vector<int> m_overlappedIdList;    // overlapped Id의 리스트 (순서대로 최신 Track)
    QMap<int, GraphAreaData> m_recordingPeriodMap;    // overlapped Id 별 녹화 구간. key: overlappedId, value: RecordingPeriod
    QList<GraphAreaData> m_overlappedTimeList;  // 중복구간이 실제로 겹치는 구간의 리스트

    Wisenet::Device::RecordingType m_recordingTypeFilter = Wisenet::Device::RecordingType::All;
    int m_nextRecordOverlappedId = -1;

    // SmartSearch
    bool m_isSmartSearchMode = false;
    QVariantList m_smartSearchTypeFilter;   // 필터 체크 된 Wisenet::Device::SmartSearchResultType의 리스트

    bool isFilteredType(Wisenet::Device::RecordingType type);
    qulonglong _getNextRecordBlockTime(qulonglong currentTime, bool searchForward, bool getStartTime, bool ignoreTypeFilter, int overlappedId);
    void appendGraphAreaData(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, bool isSelectedOverlappedId,
                             QList<GraphAreaData>& overlappedTimeList, Wisenet::Device::ChannelTimeline& timeline,
                             GraphAreaData& normalData, GraphAreaData& eventData, QVector<GraphAreaData>& normalDataList, QVector<GraphAreaData>& eventDataList);
    void sortOverlappedIdList(QVariantList& overlappedIdList);
};

#endif // GRAPHAREAMODEL_H

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

#include <QObject>
#include "DeviceClient/DeviceStructure.h"
#include "MediaControlModel.h"

class MultiGraphAreaModel : public QAbstractListModel
{
    Q_OBJECT

    /* 필요 시 Item별 Role로 추가
    Q_PROPERTY(qulonglong firstRecordTime READ firstRecordTime CONSTANT)
    Q_PROPERTY(qulonglong lastRecordTime READ lastRecordTime CONSTANT)
    */
    Q_PROPERTY(bool isLoading READ isLoading WRITE setIsLoading NOTIFY isLoadingChanged)

public:
    enum RoleNames
    {
        ChannelNameRole = Qt::UserRole,
        DeviceIdRole,
        ChannelIdRole,
        OverlappedIdRole,
        GraphImageRole,
        OverlappedBlockCountRole,
    };

    enum qRecordType
    {
        RecType_None,
        RecType_Normal,
        RecType_Event,
        RecType_NormalFiltered,
        RecType_EventFiltered,
    };
    Q_ENUM(qRecordType)

    struct RecordTime
    {
        int64_t startTime = 0;
        int64_t endTime = 0;
    };

    struct OverlappedBlock
    {
        float startPosition = 0;
        float endPosition = 0;
    };

    struct MultiGraphAreaData
    {
        QString itemId = "";
        QString channelName = "";
        Wisenet::uuid_string deviceId = "";
        std::string channelId = "";
        int overlappedId = -1;
        QImage graphImage;
        QList<OverlappedBlock> overlappedBlockList;
    };

    explicit MultiGraphAreaModel(QObject *parent = nullptr);
    ~MultiGraphAreaModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    /*
    void insert(int index, const MultiGraphAreaData& item);
    void append(const MultiGraphAreaData& item);
    void remove(int index);
    */

    // Q_PROPERTY
    bool isLoading() const;
    void setIsLoading(const bool loading);

    // Q_INVOKABLE
    //Q_INVOKABLE void connectMediaControlModelSignals(MediaControlModel* sender);
    Q_INVOKABLE void clear();
    Q_INVOKABLE void refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth,
                                          QString qDeviceId, QList<QString> channelIdList, QVariantMap overlappedIdMap);
    Q_INVOKABLE void refreshLayoutItems(QVariantList items, QDateTime selectedDate);
    /*
    Q_INVOKABLE void addLayoutItems(QVariantList items, QDateTime selectedDate);
    Q_INVOKABLE void removeLayoutItems(QList<QString> itemIds);
    */
    Q_INVOKABLE void selectedDateChanged(QDateTime selectedDate);
    Q_INVOKABLE void focusedItemChanged(QString deviceId, QString channelId);
    Q_INVOKABLE void recordingTypeFilterChanged(quint32 filter);
    /*
    Q_INVOKABLE qulonglong getCurrentRecordBlockEndTime(qulonglong currentTime, bool searchForward, bool ignoreTypeFilter = false);
    Q_INVOKABLE qulonglong getNextRecordBlockTime(qulonglong currentTime, bool searchForward, bool getStartTime, bool ignoreTypeFilter = false);
    */
    Q_INVOKABLE float getOverlappedBlockPos(int modelIndex, int overlapIndex);
    Q_INVOKABLE float getOverlappedBlockWidth(int modelIndex, int overlapIndex);
    Q_INVOKABLE QVariantList getOverlappedIdList(QString deviceId, QString channelId, qulonglong currentTime);
    Q_INVOKABLE qulonglong getValidSeekTime(QString deviceId, QString channelId, int overlappedId, qulonglong seekTime, bool searchForward, bool ignoreTypeFilter = false);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

signals:
    void timelineUpdated(QString deviceId, QList<QString> channelIdList);
    void focusedItemIndexChanged(int index);
    void isLoadingChanged();

public slots:
    void onRequestTimerTimeout();

private:
    void sortGraphData();
    void getOneDayTimeline(Wisenet::uuid_string deviceId, QList<std::string> channelIdList, QDateTime selectedDate);
    void refreshGraphImage(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth, int modelIndex,
                           MultiGraphAreaData& graphData, Wisenet::Device::OneDayTimeline& oneDayTimeline, QVariantMap overlappedIdMap);
    void appendGraphAreaData(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth,
                             Wisenet::Device::ChannelTimeline& timeline, QList<RecordTime>& overlappedTimeList, bool isSelectedOverlappedId,
                             QPainter& qPainter, qRecordType& lastRecordType, float& lastStartPos, float& lastEndPos);
    //bool isFilteredType(Wisenet::Device::RecordingType type);

    QHash<int, QByteArray> m_roleNames;
    QList<QString> m_displayItemIdList; // Graph를 표시하는 ID List (중복 채널을 제외하고 이름순으로 정렬 한 리스트)
    QMap<QString, MultiGraphAreaData> m_graphDataMap;  // key:layoutItemId. item별 실제 graph Data
    QMap<Wisenet::uuid_string, Wisenet::Device::RecordingTimelineViewResult> m_deviceTimeline;   // key:deviceId. 장비 > 채널 > 날짜 > Track별 타임라인
    QDateTime m_selectedDate;
    Wisenet::Device::RecordingType m_recordingTypeFilter = Wisenet::Device::RecordingType::All;

    QTimer m_requestTimer;
    QList<Wisenet::Device::DeviceGetTimelineRequestSharedPtr> m_requestQueue;
    bool m_isLoading = false;
};

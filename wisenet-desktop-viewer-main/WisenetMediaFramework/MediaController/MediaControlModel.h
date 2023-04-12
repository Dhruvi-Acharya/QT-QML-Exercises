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
#include <QQueue>
#include "DeviceClient/DeviceStructure.h"
#include "QCoreServiceManager.h"
#include "WisenetMediaPlayer.h"

struct visibleTimeRangeDelta
{
    qulonglong startTimeDelta = 0;
    qulonglong rangeDelta = 0;
};

class MediaControlModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qulonglong recordingStartTime READ recordingStartTime CONSTANT)
    Q_PROPERTY(qulonglong recordingEndTime READ recordingEndTime CONSTANT)
    Q_PROPERTY(qulonglong timelineStartTime READ timelineStartTime WRITE setTimelineStartTime NOTIFY timelineStartTimeChanged)
    Q_PROPERTY(qulonglong timelineEndTime READ timelineEndTime WRITE setTimelineEndTime NOTIFY timelineEndTimeChanged)
    Q_PROPERTY(qulonglong visibleStartTime READ visibleStartTime WRITE setvisibleStartTime NOTIFY visibleStartTimeChanged)
    Q_PROPERTY(qulonglong visibleEndTime READ visibleEndTime WRITE setvisibleEndTime NOTIFY visibleEndTimeChanged)
    Q_PROPERTY(qulonglong visibleTimeRange READ visibleTimeRange NOTIFY visibleTimeRangeChanged)
    Q_PROPERTY(QDateTime calendarSelectedDate READ calendarSelectedDate WRITE setCalendarSelectedDate NOTIFY calendarSelectedDateChanged)
    Q_PROPERTY(QDateTime calendarMinimumDate READ calendarMinimumDate WRITE setCalendarMinimumDate NOTIFY calendarMinimumDateChanged)
    Q_PROPERTY(QDateTime calendarMaximumDate READ calendarMaximumDate WRITE setCalendarMaximumDate NOTIFY calendarMaximumDateChanged)
    Q_PROPERTY(QString selectedDeviceID READ selectedDeviceID CONSTANT)
    Q_PROPERTY(QString selectedChannelID READ selectedChannelID CONSTANT)
    Q_PROPERTY(int selectedOverlappedID READ selectedOverlappedID WRITE setSelectedOverlappedID NOTIFY selectedOverlappedIDChanged)
    Q_PROPERTY(bool isMultiTimelineMode READ isMultiTimelineMode WRITE setIsMultiTimelineMode NOTIFY isMultiTimelineModeChanged)
    Q_PROPERTY(quint32 recordingTypeFilter READ recordingTypeFilter WRITE setRecordingTypeFilter NOTIFY recordingTypeFilterChanged)

public:
    MediaControlModel();
    ~MediaControlModel();

    static void registerQml();

    Q_INVOKABLE void setVisibleTimeRange(qlonglong startTimeDelta, qlonglong rangeDelta);
    Q_INVOKABLE void getAllPeriodTimeline(QString deviceId, QList<QString> channelIdList, qint64 currentTime = 0);
    Q_INVOKABLE void setSelectedChannel(const QString& deviceID, const QString& channelID, const int overlappedId = -1, qint64 currentTime = 0);
    Q_INVOKABLE int getDefaultOverlappedId(const QString& deviceID, const QString& channelID);
    Q_INVOKABLE qint64 getLiveToPlaybackTime(const QString& deviceID, const QString& channelID, const int overlappedId, const qint64 currentTime);
    Q_INVOKABLE QVariantList getOverlappedIdList();
    Q_INVOKABLE void getLocalResourceTimeline(WisenetMediaPlayer* mediaPlayer);
    Q_INVOKABLE QString getHashPassword(QString userGuid, QString password);
    Q_INVOKABLE void refreshCalendar(QList<QString> deviceChannelList, int visibleYear, int visibleMonth, bool recordMapOnly);

    qulonglong recordingStartTime() {return m_recordingStartTime;}
    qulonglong recordingEndTime() {return m_recordingEndTime;}
    qulonglong timelineStartTime() {return m_timelineStartTime;}
    qulonglong timelineEndTime() {return m_timelineEndTime;}
    qulonglong visibleStartTime() {return m_visibleStartTime;}
    qulonglong visibleEndTime() {return m_visibleEndTime;}
    qulonglong visibleTimeRange() {return m_visibleTimeRange;}
    QDateTime calendarSelectedDate() {return m_calendarSelectedDate;}
    QDateTime calendarMinimumDate() {return m_calendarMinimumDate;}
    QDateTime calendarMaximumDate() {return m_calendarMaximumDate;}
    QString selectedDeviceID() {return QString(m_selectedDeviceId.c_str());}
    QString selectedChannelID() {return QString(m_selectedChannelId.c_str());}
    int selectedOverlappedID() {return m_selectedOverlappedId;}
    bool isMultiTimelineMode() {return m_isMultiTimelineMode;}
    quint32 recordingTypeFilter() {return (quint32)m_recordingTypeFilter;}

    void setTimelineStartTime(qulonglong time);
    void setTimelineEndTime(qulonglong time);
    void setvisibleStartTime(qulonglong time);
    void setvisibleEndTime(qulonglong time);
    void setSelectedOverlappedID(int overlappedId);
    void setIsMultiTimelineMode(bool multiTimelineMode);
    void setCalendarSelectedDate(QDateTime selectedDate);
    void setCalendarMinimumDate(QDateTime minimumDate);
    void setCalendarMaximumDate(QDateTime maximumDate);
    void setRecordingTypeFilter(quint32 filter);

public slots:
    void onCoreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void timelineStartTimeChanged();
    void timelineEndTimeChanged();
    void visibleStartTimeChanged(qulonglong startTime);
    void visibleEndTimeChanged(qulonglong endTime);
    void visibleTimeRangeChanged(qulonglong range);
    void selectedTimelineUpdated(const std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>& selectedTimeline,    // key:Overlapped ID, value:Timeline List
                                 const std::vector<int>& overlappedIdList);  // overlappedId List (OverlappedId 순서 체크용)
    void selectedChannelChanged(const QString& deviceID, const QString& channelID);
    void selectedOverlappedIDChanged();
    void isMultiTimelineModeChanged(bool isMultiTimeline);
    void calendarSearchFinished(int64_t month, std::vector<bool> dayRecordExist);
    void calendarSelectedDateChanged();
    void calendarMinimumDateChanged();
    void calendarMaximumDateChanged();
    void recordingTypeFilterChanged(quint32 filter);

private:
    qulonglong m_recordingStartTime = 0;    // 녹화 시작 시간
    qulonglong m_recordingEndTime = 0;      // 녹화 끝 시간
    qulonglong m_timelineStartTime = 0;     // 타임라인 시작 시간
    qulonglong m_timelineEndTime = 0;       // 타임라인 끝 시간
    qulonglong m_visibleStartTime = 0;      // 화면 표시 시작 시작
    qulonglong m_visibleEndTime = 0;        // 화면 표시 끝 시간
    qulonglong m_visibleTimeRange = 0;      // 화면 표시 범위

    QDateTime m_calendarSelectedDate;
    QDateTime m_calendarMinimumDate;
    QDateTime m_calendarMaximumDate;

    std::map<Wisenet::uuid_string, Wisenet::Device::DeviceGetAllPeriodTimelineResponse> m_deviceTimeline;
    Wisenet::uuid_string m_selectedDeviceId = "";
    std::string m_selectedChannelId = "";
    int m_selectedOverlappedId = -1;

    bool m_isMultiTimelineMode = false;
    Wisenet::Device::RecordingType m_recordingTypeFilter = Wisenet::Device::RecordingType::All;

    QScopedPointer<QTimer> m_timelineReloadTimer;

    void resetTimeline();
    void onTimelineReloadTimerTimeout();
    void additionalTimelineReload(bool fromLastRecordTime);
    void setDefaultTimelineStartEndTime(bool reset = true);
    void checkVisibleTimelineOverlflow();
};

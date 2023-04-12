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

#include <QList>
#include <QObject>
#include <QDate>
#include <QDateTime>
#include <map>
#include "MediaControlModel.h"

class CalendarTimelineModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool refreshFlag READ refreshFlag NOTIFY refreshFlagChanged)
    Q_PROPERTY(QDateTime selectionStartDate READ selectionStartDate WRITE setSelectionStartDate NOTIFY selectionStartDateChanged)
    Q_PROPERTY(QDateTime selectionEndDate READ selectionEndDate WRITE setSelectionEndDate NOTIFY selectionEndDateChanged)

public:
    CalendarTimelineModel();
    ~CalendarTimelineModel();

    static void registerQml();

    Q_INVOKABLE bool hasRecordForDate(const QDate &date);
    Q_INVOKABLE bool hasRecordForTime(const QDateTime &date);
    Q_INVOKABLE quint64 getDayStartTimeMsec(QDateTime day);
    Q_INVOKABLE quint64 getDayEndTimeMsec(QDateTime day);
    Q_INVOKABLE quint64 getTimeMsec(QDateTime time);
    Q_INVOKABLE void connectMediaControlModelSignals(MediaControlModel* sender);
    Q_INVOKABLE QDateTime selectionStartDate() const;
    Q_INVOKABLE void setSelectionStartDate(const QDateTime& date);
    Q_INVOKABLE QDateTime selectionEndDate() const;
    Q_INVOKABLE void setSelectionEndDate(const QDateTime& date);
    Q_INVOKABLE void clearRecordMap();
    bool refreshFlag(){return m_refreshFlag;}

signals:
    void timelineUpdated();
    void refreshFlagChanged(bool flag);
    void selectionStartDateChanged(const QDateTime &date);
    void selectionEndDateChanged(const QDateTime &date);

public slots:
    void onSelectedChannelTimelineUpdated(const std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>& selectedTimeline,
                                          const std::vector<int>& overlappedIdList);
    void onCalendarSearchFinished(int64_t month, std::vector<bool> dayRecordExist);

private:
    std::vector<QDate> m_RecordDatelist;
    std::map<QDate, std::map<int, bool>> m_recordMap;   // Date, Hour, hasRecord
    QDateTime m_timelineStartTime;
    QDateTime m_timelineEndTime;
    bool m_refreshFlag;

    QDateTime m_selectionStartDate;
    QDateTime m_selectionEndDate;
};

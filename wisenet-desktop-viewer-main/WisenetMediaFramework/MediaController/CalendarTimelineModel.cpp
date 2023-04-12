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
#include <QDebug>
#include "CalendarTimelineModel.h"

CalendarTimelineModel::CalendarTimelineModel()
    : m_selectionStartDate(QDate(1970,1,1),QTime(0,0,0))
    , m_selectionEndDate(QDate(1970,1,1),QTime(0,0,0))
    , m_refreshFlag(false)
{
    /*
    // Test Data
    m_timelineStartTime = QDateTime(QDate(2021, 2, 2), QTime(3, 30, 0));
    m_timelineEndTime = QDateTime(QDate(2021, 2, 10), QTime(21, 30, 0));
    for(int i = 4 ; i<8 ; i++)
    {
        m_RecordDatelist.push_back(QDate(2021, 2, i));
    }
    */
}

CalendarTimelineModel::~CalendarTimelineModel()
{

}

void CalendarTimelineModel::registerQml()
{
    qmlRegisterType<CalendarTimelineModel>("Wisenet.Qmls", 0, 1, "CalendarTimelineModel");
}

void CalendarTimelineModel::connectMediaControlModelSignals(MediaControlModel* sender)
{
    connect(sender, &MediaControlModel::selectedTimelineUpdated, this, &CalendarTimelineModel::onSelectedChannelTimelineUpdated);
    connect(sender, &MediaControlModel::calendarSearchFinished, this, &CalendarTimelineModel::onCalendarSearchFinished);
}

QDateTime CalendarTimelineModel::selectionStartDate() const
{
    return m_selectionStartDate;
}

void CalendarTimelineModel::setSelectionStartDate(const QDateTime &date)
{
    if(m_selectionStartDate.date() != date.date()){
        //qDebug() << "setSelectionStartDate - setup: " << date;
        m_selectionStartDate = date;
        emit selectionStartDateChanged(m_selectionStartDate);
    }
}

QDateTime CalendarTimelineModel::selectionEndDate() const
{
    return m_selectionEndDate;
}

void CalendarTimelineModel::setSelectionEndDate(const QDateTime &date)
{
    if(m_selectionEndDate.date() != date.date()){
        //qDebug() << "setSelectionEndDate - setup: " << date;
        m_selectionEndDate = date;
        emit selectionEndDateChanged(m_selectionEndDate);
    }

}

void CalendarTimelineModel::onSelectedChannelTimelineUpdated(const std::unordered_map<int, std::vector<Wisenet::Device::ChannelTimeline>>& selectedTimeline,
                                                             const std::vector<int>& overlappedIdList)
{
    /* record가 많은경우 성능 문제로 CalendarSearch를 사용하도록 변경 (onCalendarSearchFinished()에서 녹화 날짜 업데이트 됨)
    m_recordMap.clear();

    auto itor = selectedTimeline.begin();
    while(itor != selectedTimeline.end()) {
        for (auto& record : itor->second)
        {
            QDateTime startTime = QDateTime::fromMSecsSinceEpoch(record.startTime);
            //startTime = startTime.addMSecs(-startTime.time().minute()*60000 - startTime.time().second()*1000 - startTime.time().msec());

            QDateTime endTime = QDateTime::fromMSecsSinceEpoch(record.endTime);

            while(startTime < endTime)
            {
                m_recordMap[startTime.date()][startTime.time().hour()] = true;
                //startTime = startTime.addSecs(3600);
                startTime = startTime.addDays(1);
            }
        }
        itor++;
    }
    */

    emit timelineUpdated(); // 타임라인 변경 signal은 그대로 보내서 QML에서 필요 동작 처리

    /*
    m_refreshFlag = !m_refreshFlag;
    emit refreshFlagChanged(m_refreshFlag);
    */
}

void CalendarTimelineModel::onCalendarSearchFinished(int64_t month, std::vector<bool> dayRecordExist)
{
    QDate monthDate = QDateTime::fromMSecsSinceEpoch(month).date();
    monthDate.setDate(monthDate.year(), monthDate.month(), 1);

    for (int day = 0 ; day < monthDate.daysInMonth() ; day++) {
        if(dayRecordExist[day])
            m_recordMap[monthDate][0] = true;
        monthDate = monthDate.addDays(1);
    }

    m_refreshFlag = !m_refreshFlag;
    emit refreshFlagChanged(m_refreshFlag);
}

bool CalendarTimelineModel::hasRecordForDate(const QDate &date)
{
    return m_recordMap.find(date) != m_recordMap.end();
}

bool CalendarTimelineModel::hasRecordForTime(const QDateTime &time)
{
    if(m_recordMap.find(time.date()) == m_recordMap.end())
        return false;

    auto hourRecord = m_recordMap[time.date()].find(time.time().hour());
    return hourRecord != m_recordMap[time.date()].end() && hourRecord->second == true;
}

quint64 CalendarTimelineModel::getDayStartTimeMsec(QDateTime day)
{
    QTime time;
    time.setHMS(0,0,0,0);

    day.setTime(time);
    return day.toMSecsSinceEpoch();
}

quint64 CalendarTimelineModel::getDayEndTimeMsec(QDateTime day)
{
    QTime time;
    time.setHMS(0,0,0,0);

    day.setTime(time);
    day = day.addDays(1);
    return day.toMSecsSinceEpoch();
}

quint64 CalendarTimelineModel::getTimeMsec(QDateTime time)
{
    return time.toMSecsSinceEpoch();
}

void CalendarTimelineModel::clearRecordMap()
{
    m_recordMap.clear();
    m_refreshFlag = !m_refreshFlag;
    emit refreshFlagChanged(m_refreshFlag);
}

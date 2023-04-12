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
#pragma once

#include <QList>
#include <QObject>
#include <QDate>
#include <QDateTime>
#include <map>

class CalendarTimelineModel : public QObject
{
    Q_OBJECT

public:
    CalendarTimelineModel();
    ~CalendarTimelineModel();

    static void registerQml();

    Q_INVOKABLE bool hasRecordForDate(const QDate &date);
    Q_INVOKABLE QDateTime getTimelineStartTime();
    Q_INVOKABLE QDateTime getTimelineEndTime();

private:
    std::vector<QDate> m_RecordDatelist;
    QDateTime m_timelineStartTime;
    QDateTime m_timelineEndTime;
};

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
#include "MediaController/CalendarTimelineModel.h"


CalendarTimelineModel::CalendarTimelineModel()
{
    // Test Data
    m_timelineStartTime = QDateTime(QDate(2021, 2, 2), QTime(3, 30, 0));
    m_timelineEndTime = QDateTime(QDate(2021, 2, 10), QTime(21, 30, 0));
    for(int i = 4 ; i<8 ; i++)
    {
        m_RecordDatelist.push_back(QDate(2021, 2, i));
    }
}

CalendarTimelineModel::~CalendarTimelineModel()
{

}

void CalendarTimelineModel::registerQml()
{
    qmlRegisterType<CalendarTimelineModel>("Wisenet.Qmls", 0, 1, "CalendarTimelineModel");
}

bool CalendarTimelineModel::hasRecordForDate(const QDate &date)
{
    for(QDate& recordDate : m_RecordDatelist)
    {
        if(recordDate == date)
            return true;
    }

    return false;
}

QDateTime CalendarTimelineModel::getTimelineStartTime()
{
    return m_timelineStartTime;
}

QDateTime CalendarTimelineModel::getTimelineEndTime()
{
    return m_timelineEndTime;
}

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

#include "DateAreaModel.h"
#include "QLocaleManager.h"
#include <QDateTime>

DateAreaModel::DateAreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[WidthRole] = "width";
    m_roleNames[DateTextRole] = "dateText";
    m_roleNames[TimeTextRole] = "timeText";
    m_roleNames[EvenNumberStyleRole] = "evenNumberStyle";
    m_roleNames[StartTimeRole] = "startTime";
    m_roleNames[EndTimeRole] = "endTime";
}

int DateAreaModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant DateAreaModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    const DateAreaData& dateData = m_data.at(row);

    switch(role)
    {
    case WidthRole:
        return dateData.width;
    case DateTextRole:
        return dateData.dateText;
    case TimeTextRole:
        return dateData.timeText;
    case EvenNumberStyleRole:
        return dateData.evenNumberStyle;
    case StartTimeRole:
        return dateData.startTime;
    case EndTimeRole:
        return dateData.endTime;
    }

    return QVariant();
}

QHash<int, QByteArray> DateAreaModel::roleNames() const
{
    return m_roleNames;
}

void DateAreaModel::insert(int index, const DateAreaData& timeData)
{
    if(index < 0 || index > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, timeData);
    endInsertRows();
}

void DateAreaModel::append(const DateAreaData& timeData)
{
    insert(m_data.count(), timeData);
}

void DateAreaModel::remove(int index)
{
    if(index < 0 || index > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}

void DateAreaModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count()-1);
    m_data.clear();
    endRemoveRows();
}

// 전체 데이터 clear 후 갱신하는 함수
void DateAreaModel::refreshDateDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    clear();

    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    m_dataInterval = getDataInterval(visibleStartTime, visibleEndTime, controlWidth);

    QDateTime dateTime;
    dateTime.setMSecsSinceEpoch(visibleStartTime);

    // 시작 시간 설정 (뒷자리 정리)
    if(m_dataInterval == DataIntervals::Month)
    {
        dateTime.setDate(QDate(dateTime.date().year(), dateTime.date().month(), 1));
        dateTime.setTime(QTime(0, 0, 0, 0));
    }
    else if(m_dataInterval == DataIntervals::TenDays
            || m_dataInterval == DataIntervals::ThreeDays
            || m_dataInterval == DataIntervals::Day)
    {
        dateTime.setTime(QTime(0, 0, 0, 0));
    }
    else if(m_dataInterval == DataIntervals::EightHours
            || m_dataInterval == DataIntervals::ThreeHours
            || m_dataInterval == DataIntervals::Hour)
    {
        dateTime = dateTime.addMSecs(-dateTime.time().minute()*60000 -dateTime.time().second()*1000 -dateTime.time().msec());
    }
    else if(m_dataInterval == DataIntervals::FifteenMinutes
            || m_dataInterval == DataIntervals::FourMinutes
            || m_dataInterval == DataIntervals::Minute)
    {
        dateTime = dateTime.addMSecs(-dateTime.time().second()*1000 -dateTime.time().msec());
    }

    QVector<DateAreaData> dataList;

    float currentPos = 0;
    while(currentPos < controlWidth)
    {
        DateAreaData data;
        data.dateText = getDateText(dateTime);
        data.timeText = getTimeText(dateTime);
        data.evenNumberStyle = getEvenNumberStyle(dateTime);
        data.startTime = dateTime.toMSecsSinceEpoch();

        dateTime = addDateTime(dateTime, 1);
        data.endTime = dateTime.toMSecsSinceEpoch();

        if(data.endTime >= visibleEndTime)
        {
            data.width = controlWidth - currentPos;
            currentPos = controlWidth;
        }
        else
        {
            qulonglong timeDiff = data.endTime - visibleStartTime;
            float nextPos = (float)timeDiff / (float)visibleTimeRange * controlWidth;
            data.width = nextPos - currentPos;
            currentPos = nextPos;
        }

        dataList.append(data);
    }

    if(dataList.count() == 0)
        return;

    beginInsertRows(QModelIndex(), 0, dataList.count()-1);
    for(auto& data : dataList)
    {
        m_data.append(data);
    }
    endInsertRows();
}

// 데이터 시간 간격 계산 함수
DateAreaModel::DataIntervals DateAreaModel::getDataInterval(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    const qulonglong minute = 60 * 1000;
    const qulonglong hour = minute * 60;
    const qulonglong day = hour * 24;
    const qulonglong month = day * 31;

    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;

    if(visibleTimeRange >= 2 * month)
        return DataIntervals::Month;
    else if(visibleTimeRange >= 2 * 10*day)
        return DataIntervals::TenDays;
    else if(visibleTimeRange >= 2 * 3*day)
        return DataIntervals::ThreeDays;
    else if(visibleTimeRange >= 2 * day)
        return DataIntervals::Day;
    else if(visibleTimeRange >= 2 * 8*hour)
        return DataIntervals::EightHours;
    else if(visibleTimeRange >= 2 * 3*hour)
        return DataIntervals::ThreeHours;
    else if(visibleTimeRange >= 2 * hour)
        return DataIntervals::Hour;
    else if(visibleTimeRange >= 2 * 15*minute)
        return DataIntervals::FifteenMinutes;
    else if(visibleTimeRange >= 2 * 4*minute)
        return DataIntervals::FourMinutes;
    else
        return DataIntervals::Minute;
}

QDateTime DateAreaModel::addDateTime(QDateTime dateTime, qint64 diff)
{
    if(m_dataInterval == DataIntervals::Month)
    {
        return dateTime.addMonths(diff);
    }
    else if(m_dataInterval == DataIntervals::TenDays)
    {
        while (true) {
            dateTime = dateTime.addDays(diff);

            int day = dateTime.date().day();
            if(day == 1 || day == 11 || day == 21)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::ThreeDays)
    {
        while (true) {
            dateTime = dateTime.addDays(diff);

            int day = dateTime.date().day();
            if(day%3 == 1 && day <= 28)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::Day)
    {
        return dateTime.addDays(diff);
    }
    else if(m_dataInterval == DataIntervals::EightHours)
    {
        while (true) {
            dateTime = dateTime.addSecs(diff * 3600);

            int hour = dateTime.time().hour();
            if(hour%8 == 0)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::ThreeHours)
    {
        while (true) {
            dateTime = dateTime.addSecs(diff * 3600);

            int hour = dateTime.time().hour();
            if(hour%3 == 0)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::Hour)
    {
        return dateTime.addSecs(diff * 3600);
    }
    else if(m_dataInterval == DataIntervals::FifteenMinutes)
    {
        while (true) {
            dateTime = dateTime.addSecs(diff * 60);

            int minute = dateTime.time().minute();
            if(minute%15 == 0)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::FourMinutes)
    {
        while (true) {
            dateTime = dateTime.addSecs(diff * 60);

            int minute = dateTime.time().minute();
            if(minute%4 == 0)
                return dateTime;
        }
    }
    else if(m_dataInterval == DataIntervals::Minute)
    {
        return dateTime.addSecs(diff * 60);
    }

    return dateTime;
}

QString DateAreaModel::getDateText(QDateTime dateTime)
{
    if(m_dataInterval == DataIntervals::Month)
    {
        return QLocaleManager::Instance()->getYearMonth(dateTime);
    }
    else
    {
        return QLocaleManager::Instance()->getDate(dateTime);
    }

    return "";
}

QString DateAreaModel::getTimeText(QDateTime dateTime)
{
    if( m_dataInterval == DataIntervals::EightHours
            || m_dataInterval == DataIntervals::ThreeHours
            || m_dataInterval == DataIntervals::Hour
            || m_dataInterval == DataIntervals::FifteenMinutes
            || m_dataInterval == DataIntervals::FourMinutes
            || m_dataInterval == DataIntervals::Minute )
    {
        return dateTime.toString("hh:mm");
    }

    return "";
}

bool DateAreaModel::getEvenNumberStyle(QDateTime dateTime)
{
    if(m_dataInterval == DataIntervals::Month)
    {
        return dateTime.date().month() % 2 == 0;
    }
    else if(m_dataInterval == DataIntervals::Day)
    {
        return (dateTime.date().year() + dateTime.date().dayOfYear()) % 2 == 0;
    }
    else if(m_dataInterval == DataIntervals::Hour)
    {
        return dateTime.time().hour() % 2 == 0;
    }
    else if(m_dataInterval == DataIntervals::Minute)
    {
        return dateTime.time().minute() % 2 == 0;
    }

    return false;
}

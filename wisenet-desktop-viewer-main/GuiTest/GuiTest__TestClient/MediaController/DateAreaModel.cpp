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

#include "DateAreaModel.h"
#include <QDateTime>

DateAreaModel::DateAreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[WidthRole] = "width";
    m_roleNames[TextRole] = "text";
    m_roleNames[EvenNumberStyleRole] = "evenNumberStyle";
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
    case TextRole:
        return dateData.timeText;
    case EvenNumberStyleRole:
        return dateData.evenNumberStyle;
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
    else if(m_dataInterval == DataIntervals::Day)
    {
        dateTime.setTime(QTime(0, 0, 0, 0));
    }
    else if(m_dataInterval == DataIntervals::Hour)
    {
        dateTime.setTime(QTime(dateTime.time().hour(), 0, 0, 0));
    }
    else if(m_dataInterval == DataIntervals::Minute)
    {
        dateTime.setTime(QTime(dateTime.time().hour(), dateTime.time().minute(), 0, 0));
    }

    QVector<DateAreaData> dataList;

    float currentPos = 0;
    while(currentPos < controlWidth)
    {
        DateAreaData data;
        data.timeText = getDateTimeText(dateTime);
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

    beginInsertRows(QModelIndex(), 0, dataList.count()-1);
    for(auto& data : dataList)
    {
        m_data.append(data);
    }
    endInsertRows();
}

// 이동 범위 만큼의 데이터를 업데이트 하는 함수
void DateAreaModel::updateDateDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    if(m_data.count() == 0 || m_dataInterval != getDataInterval(visibleStartTime, visibleEndTime, controlWidth))
    {
        // 데이터가 하나도 없거나 interval이 변경 된 경우 전체 데이터 refresh
        refreshDateDataList(visibleStartTime, visibleEndTime, controlWidth);
        return;
    }

    // width가 0인 데이터 삭제
    QList<int> deleteIndexList;
    for(int i=m_data.count()-1 ; i>=0 ; i--)
    {
        if(m_data[i].width == 0 || m_data[i].endTime <= visibleStartTime || m_data[i].startTime >= visibleEndTime)
            deleteIndexList.append(i);
    }

    for(int i : deleteIndexList)
    {
        remove(i);
    }

    // width가 0인 데이터 전부 제거 후 데이터가 하나도 없으면 전체 데이터 refresh
    if(m_data.count() == 0)
    {
        refreshDateDataList(visibleStartTime, visibleEndTime, controlWidth);
        return;
    }

    // 앞쪽 데이터 추가
    while(m_data[0].startTime > visibleStartTime)
    {
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(m_data[0].startTime);
        dateTime = addDateTime(dateTime, -1);

        DateAreaData data;
        data.startTime = dateTime.toMSecsSinceEpoch();
        data.endTime = m_data[0].startTime;
        data.timeText = getDateTimeText(dateTime);
        data.evenNumberStyle = getEvenNumberStyle(dateTime);

        insert(0, data);
    }

    // 뒷쪽 데이터 추가
    int endIndex = m_data.count()-1;
    while(m_data[endIndex].endTime < visibleEndTime)
    {
        QDateTime dateTime;
        dateTime.setMSecsSinceEpoch(m_data[endIndex].endTime);

        DateAreaData data;
        data.startTime = m_data[endIndex].endTime;
        data.timeText = getDateTimeText(dateTime);
        data.evenNumberStyle = getEvenNumberStyle(dateTime);
        dateTime = addDateTime(dateTime, 1);
        data.endTime = dateTime.toMSecsSinceEpoch();

        append(data);

        endIndex = m_data.count()-1;
    }

    // 전체 데이터의 Width 업데이트
    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    float currentPos = 0;

    for(int i=0 ; i<m_data.count() ; i++)
    {
        if(m_data[i].endTime <= visibleStartTime || m_data[i].startTime >= visibleEndTime)
        {
            m_data[i].width = 0;
            continue;
        }

        if(m_data[i].endTime >= visibleEndTime)
        {
            m_data[i].width = controlWidth - currentPos;
            currentPos = controlWidth;
        }
        else
        {
            qulonglong timeDiff = m_data[i].endTime - visibleStartTime;
            float nextPos = (float)timeDiff / (float)visibleTimeRange * controlWidth;
            m_data[i].width = nextPos - currentPos;
            currentPos = nextPos;
        }
    }

    QVector<int> rols {WidthRole};
    emit dataChanged(index(0,0,QModelIndex()), index(m_data.count()-1,0,QModelIndex()), rols);
}

// 데이터 시간 간격 계산 함수
DateAreaModel::DataIntervals DateAreaModel::getDataInterval(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    float timeRectWidth;

    timeRectWidth = (float)(1000*60*60*3) / (float)visibleTimeRange * controlWidth;
    if(timeRectWidth < 40)
    {
        // 1시간 눈금 표시 전 까지 월 단위 날짜 표시
        return DataIntervals::Month;
    }

    timeRectWidth = (float)(1000*60*10) / (float)visibleTimeRange * controlWidth;
    if(timeRectWidth < 40)
    {
        // 5분 눈금 표시 전 까지 일 단위 날짜 표시
        return DataIntervals::Day;
    }

    timeRectWidth = (float)(1000*5) / (float)visibleTimeRange * controlWidth;
    if(timeRectWidth < 40)
    {
        // 1초 눈금 표시 전 까지 시 단위 날짜 표시
        return DataIntervals::Hour;
    }

    return DataIntervals::Minute;
}

QDateTime DateAreaModel::addDateTime(QDateTime dateTime, int diff)
{
    if(m_dataInterval == DataIntervals::Month)
    {
        return dateTime.addMonths(diff);
    }
    else if(m_dataInterval == DataIntervals::Day)
    {
        return dateTime.addDays(diff);
    }
    else if(m_dataInterval == DataIntervals::Hour)
    {
        return dateTime.addSecs(diff * 3600);
    }
    else if(m_dataInterval == DataIntervals::Minute)
    {
        return dateTime.addSecs(diff * 60);
    }

    return dateTime;
}

QString DateAreaModel::getDateTimeText(QDateTime dateTime)
{
    if(m_dataInterval == DataIntervals::Month)
    {
        return dateTime.toString("MMMM yyyy");
    }
    else if(m_dataInterval == DataIntervals::Day)
    {
        return dateTime.toString("yyyy-MM-dd");
    }
    else if(m_dataInterval == DataIntervals::Hour)
    {
        return dateTime.toString("yyyy-MM-dd h AP");
    }
    else if(m_dataInterval == DataIntervals::Minute)
    {
        return dateTime.toString("yyyy-MM-dd AP h:mm");
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
        return dateTime.date().day() % 2 == 0;
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

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

#include "TimeAreaModel.h"
#include <QDebug>
#include <QDateTime>

TimeAreaModel::TimeAreaModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[TimeRole] = "time";
    m_roleNames[DepthRole] = "depth";
    m_roleNames[RectWidthRole] = "rectWidth";
    m_roleNames[LineHeightRole] = "lineHeight";
    m_roleNames[TimeTextRole] = "timeText";

    m_timeIntervalList.append(MSEC_SEC);
    m_timeIntervalList.append(MSEC_SEC * 5);
    m_timeIntervalList.append(MSEC_SEC * 10);
    m_timeIntervalList.append(MSEC_SEC * 30);
    m_timeIntervalList.append(MSEC_MIN);
    m_timeIntervalList.append(MSEC_MIN * 5);
    m_timeIntervalList.append(MSEC_MIN * 10);
    m_timeIntervalList.append(MSEC_MIN * 30);
    m_timeIntervalList.append(MSEC_HOUR);
    m_timeIntervalList.append(MSEC_HOUR * 3);
    m_timeIntervalList.append(MSEC_HOUR * 12);
    m_timeIntervalList.append(MSEC_HOUR * 24);

    m_timeIntervalIndex = m_timeIntervalList.count()-2;
    m_timeInterval = m_timeIntervalList[m_timeIntervalIndex];
}

int TimeAreaModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant TimeAreaModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    const TimeAreaData& timeData = m_data.at(row);

    switch(role)
    {
    case TimeRole:
        return timeData.time;
    case DepthRole:
        return timeData.depth;
    case RectWidthRole:
        return timeData.width;
    case LineHeightRole:
        return getLineHeight(timeData.depth);
    case TimeTextRole:
        return getText(timeData);
    }

    return QVariant();
}

QHash<int, QByteArray> TimeAreaModel::roleNames() const
{
    return m_roleNames;
}

void TimeAreaModel::insert(int index, const TimeAreaData& timeData)
{
    if(index < 0 || index > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, timeData);
    endInsertRows();
}

void TimeAreaModel::append(const TimeAreaData& timeData)
{
    insert(m_data.count(), timeData);
}

void TimeAreaModel::remove(int index)
{
    if(index < 0 || index > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}

void TimeAreaModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count()-1);
    m_data.clear();
    endRemoveRows();
}

// 전체 데이터 갱신 함수
void TimeAreaModel::refreshTimeDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    clear();

    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    setTimeInterval(visibleTimeRange, controlWidth);

    QVector<TimeAreaData> dataList;

    qulonglong time = visibleStartTime + m_timeInterval - (visibleStartTime % m_timeInterval);
    while(time < visibleEndTime)
    {
        TimeAreaData timeData;
        timeData.time = time;
        timeData.depth = getDepth(timeData.time);

        if(dataList.count() == 0)
        {
            // 첫 번째 Data Width
            qulonglong timeDiff = time - visibleStartTime;
            timeData.width = (float)timeDiff / (float)visibleTimeRange * controlWidth;
        }
        else
        {
            timeData.width = m_timeRectWidth;
        }

        dataList.append(timeData);

        time += m_timeInterval;
    }

    //qDebug("timeDataCount:%d", dataList.count());

    beginInsertRows(QModelIndex(), 0, dataList.count()-1);
    for(TimeAreaData& timeData : dataList)
    {
        m_data.append(timeData);
    }
    endInsertRows();
}

// 표시 시간 기준으로 눈금 간격을 계산하는 함수
void TimeAreaModel::setTimeInterval(qulonglong visibleTimeRange, float controlWidth)
{
    for(int i=1 ; i<m_timeIntervalList.count()-1 ; i++)
    {
        float timeRectWidth = controlWidth * m_timeIntervalList[i] / visibleTimeRange;
        if(timeRectWidth > 34)
        {
            m_timeIntervalIndex = i-1;
            m_timeInterval = m_timeIntervalList[i-1];
            m_timeRectWidth = (float)m_timeInterval / (float)visibleTimeRange * controlWidth;
            return;
        }
    }

    m_timeIntervalIndex = m_timeIntervalList.count()-2;
    m_timeInterval = m_timeIntervalList[m_timeIntervalIndex];
    m_timeRectWidth = (float)m_timeInterval / (float)visibleTimeRange * controlWidth;
}

// 표시 데이터 업데이트 함수
void TimeAreaModel::updateTimeDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    qulonglong lastTimeInterval = m_timeInterval;
    setTimeInterval(visibleTimeRange, controlWidth);

    if(m_data.count() == 0 || lastTimeInterval != m_timeInterval)
    {
        // 데이터가 하나도 없거나 interval이 변경 된 경우 전체 데이터 refresh
        refreshTimeDataList(visibleStartTime, visibleEndTime, controlWidth);
        return;
    }

    // width가 0인 데이터 삭제
    QList<int> deleteIndexList;
    for(int i=m_data.count()-1 ; i>=0 ; i--)
    {
        if(m_data[i].width == 0 || m_data[i].time <= visibleStartTime || m_data[i].time >= visibleEndTime)
            deleteIndexList.append(i);
    }

    for(int i : deleteIndexList)
    {
        remove(i);
    }

    // width가 0인 데이터 전부 제거 후 데이터가 하나도 없으면 전체 데이터 refresh
    if(m_data.count() == 0)
    {
        refreshTimeDataList(visibleStartTime, visibleEndTime, controlWidth);
        return;
    }

    // 앞쪽 데이터 추가
    while(m_data[0].time > visibleStartTime)
    {
        TimeAreaData data;
        data.time = m_data[0].time - m_timeInterval;
        data.depth = getDepth(data.time);

        insert(0, data);
    }

    // 뒷쪽 데이터 추가
    int endIndex = m_data.count()-1;
    while(m_data[endIndex].time < visibleEndTime)
    {
        TimeAreaData data;
        data.time = m_data[endIndex].time + m_timeInterval;
        data.depth = getDepth(data.time);

        append(data);

        endIndex = m_data.count()-1;
    }

    // 전체 데이터의 Width 업데이트
    float currentPos = 0;
    for(int i=0 ; i<m_data.count() ; i++)
    {
        if(m_data[i].time <= visibleStartTime || m_data[i].time >= visibleEndTime)
        {
            m_data[i].width = 0;
            continue;
        }

        qulonglong timeDiff = m_data[i].time - visibleStartTime;
        float nextPos = (float)timeDiff / (float)visibleTimeRange * controlWidth;
        m_data[i].width = nextPos - currentPos;
        currentPos = nextPos;
    }

    QVector<int> rols {RectWidthRole, LineHeightRole, TimeTextRole};
    emit dataChanged(index(0,0,QModelIndex()), index(m_data.count()-1,0,QModelIndex()), rols);
}

int TimeAreaModel::getDepth(const qulonglong time) const
{
    for(int i = m_timeIntervalList.count()-1 ; i > m_timeIntervalIndex ; i--)
    {
        if(time % m_timeIntervalList[i] == 0)
            return i;
    }

    return m_timeIntervalIndex;
}

int TimeAreaModel::getLineHeight(const int depth) const
{
    int lineHeight = 15;
    qulonglong interval = m_timeIntervalList[depth];
    float intervalWidth = (float)interval / (float)m_timeInterval * m_timeRectWidth;

    if(interval == MSEC_DAY) {
        lineHeight = 15;
    }
    else if(interval == MSEC_HOUR * 12) {
        if(intervalWidth < 640)
            lineHeight = 10;
    }
    else {
        float minWidth = (float)interval / (float)m_timeIntervalList[depth+1] * 40;
        if(m_timeIntervalIndex == depth && m_timeRectWidth < minWidth)
            lineHeight = 0;
        else if(intervalWidth < 80)
            lineHeight = 5;
        else if(intervalWidth < 640)
            lineHeight = 10;
    }

    return lineHeight;
}

QString TimeAreaModel::getText(const TimeAreaData timeData) const
{
    QString timeText = "";

    qulonglong interval = m_timeIntervalList[timeData.depth];
    float intervalWidth = (float)interval / (float)m_timeInterval * m_timeRectWidth;
    QDateTime datetime;
    datetime.setMSecsSinceEpoch(timeData.time);

    if(interval == MSEC_DAY && datetime.date().day() == 1)
        return QString("%1M").arg(datetime.date().month());

    if(intervalWidth < 40)
        return "";

    if(interval == MSEC_DAY) {
        timeText = QString("%1D").arg(datetime.date().day());
    }
    else if(interval == MSEC_HOUR * 12) {
        timeText = "12pm";
    }
    else if(interval == MSEC_HOUR * 3 || interval == MSEC_HOUR) {
        timeText = QString::number(datetime.time().hour()) + "h";
    }
    else if(MSEC_MIN <= interval && interval < MSEC_HOUR) {
        timeText = QString::number(datetime.time().minute()) + "m";
    }
    else if(interval < MSEC_MIN) {
        timeText = QString::number(datetime.time().second()) + "s";
    }

    return timeText;
}

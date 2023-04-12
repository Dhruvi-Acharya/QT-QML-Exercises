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

#include "TimeAreaModel.h"
#include "QLocaleManager.h"
#include <QDebug>
#include <QDateTime>
#include <chrono>

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
    m_timeIntervalList.append(MSEC_HOUR * 6);
    m_timeIntervalList.append(MSEC_HOUR * 12);
    m_timeIntervalList.append(MSEC_HOUR * 24);
    m_timeIntervalList.append(MSEC_DAY * 30);   // 실제로는 매달 1일 눈금만 그려야 함.

    m_timeIntervalIndex = m_timeIntervalList.count()-1;
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
        return getLineHeight(timeData);
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
    //auto start = std::chrono::steady_clock::now();

    clear();

    qulonglong visibleTimeRange = visibleEndTime - visibleStartTime;
    setTimeInterval(visibleTimeRange, controlWidth);

    QVector<TimeAreaData> dataList;

    QDateTime endDateTime = QDateTime::fromMSecsSinceEpoch(visibleEndTime);
    QDateTime currentTime = QDateTime::fromMSecsSinceEpoch(visibleStartTime);

    // 첫 눈금 시간 계산
    if(m_timeInterval == MSEC_DAY * 30) {
        currentTime.setDate(QDate(currentTime.date().year(), currentTime.date().month(), 1));
        currentTime.setTime(QTime(0,0,0,0));
    }
    else if(m_timeInterval >= MSEC_HOUR) {
        currentTime = currentTime.addMSecs(-currentTime.time().minute()*MSEC_MIN -currentTime.time().second()*MSEC_SEC -currentTime.time().msec());
        while(currentTime.time().hour() % (m_timeInterval/MSEC_HOUR) != 0)
            currentTime = currentTime.addMSecs(MSEC_HOUR);
    }
    else if(m_timeInterval >= MSEC_MIN) {
        currentTime = currentTime.addMSecs(-currentTime.time().second()*MSEC_SEC -currentTime.time().msec());
        while(currentTime.time().minute() % (m_timeInterval/MSEC_MIN) != 0)
            currentTime = currentTime.addMSecs(MSEC_MIN);
    }
    else {
        currentTime = currentTime.addMSecs(-currentTime.time().msec());
        while(currentTime.time().second() % (m_timeInterval/MSEC_SEC) != 0)
            currentTime = currentTime.addMSecs(MSEC_SEC);
    }

    if(currentTime.toMSecsSinceEpoch() < (qint64)visibleStartTime)
        currentTime = getPrevNextTime(currentTime, true);

    while(currentTime < endDateTime)
    {
        TimeAreaData timeData;
        timeData.time = currentTime.toMSecsSinceEpoch();
        timeData.depth = getDepth(timeData.time);

        qulonglong timeDiff = dataList.count() == 0 ? timeData.time - visibleStartTime : timeData.time - dataList.last().time;
        timeData.width = (float)timeDiff / (float)visibleTimeRange * controlWidth;

        dataList.append(timeData);

        currentTime = getPrevNextTime(currentTime, true);
    }
    //qDebug("timeDataCount:%d", dataList.count());

    if(dataList.count() == 0)
    {
        return;
    }

    beginInsertRows(QModelIndex(), 0, dataList.count()-1);
    for(TimeAreaData& timeData : dataList)
    {
        m_data.append(timeData);
    }
    endInsertRows();

    //auto end = std::chrono::steady_clock::now();
    //qDebug() << "TimeAreaModel refresh time : " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
}

// 표시 시간 기준으로 눈금 간격을 계산하는 함수
void TimeAreaModel::setTimeInterval(qulonglong visibleTimeRange, float controlWidth)
{
    m_minWidth = controlWidth / 100.0f; // 눈금 최대 개수를 100개로 제한
    if(m_minWidth < 12.0f)
        m_minWidth = 12.0f; // 눈금 최소 간격

    for(int i=0 ; i<m_timeIntervalList.count()-1 ; i++)
    {
        qulonglong interval = m_timeIntervalList[i];    // 눈금 간격(Msec)
        float timeRectWidth = controlWidth * interval / visibleTimeRange;   // 눈금 간격(Pixel)

        if((interval == MSEC_DAY && timeRectWidth > m_minWidth / 2.0f) || timeRectWidth > m_minWidth) {
            m_timeIntervalIndex = i;
            m_timeInterval = interval;
            m_timeRectWidth = timeRectWidth;

            if(interval == MSEC_DAY)
                m_minWidth = m_minWidth / 2.0f;

            return;
        }
    }

    m_timeIntervalIndex = m_timeIntervalList.count()-1;
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
        QDateTime currentTime = QDateTime::fromMSecsSinceEpoch(m_data[0].time);
        QDateTime lastTime = QDateTime(currentTime);

        TimeAreaData data;
        data.time = getPrevNextTime(currentTime, false).toMSecsSinceEpoch();
        data.depth = getDepth(data.time);

        insert(0, data);
    }

    // 뒷쪽 데이터 추가
    int endIndex = m_data.count()-1;
    while(m_data[endIndex].time < visibleEndTime)
    {
        QDateTime currentTime = QDateTime::fromMSecsSinceEpoch(m_data[endIndex].time);
        QDateTime lastTime = QDateTime(currentTime);

        TimeAreaData data;
        data.time = getPrevNextTime(currentTime, true).toMSecsSinceEpoch();
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
    QDateTime datetime = QDateTime::fromMSecsSinceEpoch(time);
    int msecOfDay = datetime.time().msecsSinceStartOfDay();

    for(int i = m_timeIntervalList.count()-2 ; i > m_timeIntervalIndex ; i--)
    {        
        if(msecOfDay == 0 || msecOfDay % m_timeIntervalList[i] == 0)
            return i;
    }

    return m_timeIntervalIndex;
}

int TimeAreaModel::getLineHeight(const TimeAreaData timeData) const
{
    int depth = timeData.depth;
    int lineHeight = 8;
    qulonglong interval = m_timeIntervalList[depth];

    if(interval == MSEC_DAY * 30)
    {
        return lineHeight;  // 월 눈금은 항상 최대 길이
    }

    float intervalWidth = (float)interval / (float)m_timeInterval * m_timeRectWidth;    // 현재 눈금과 같은 depth인 눈금의 간격(pixel)
    float minWidth = m_minWidth + 5;

    if(interval == MSEC_DAY) {
        // 날짜 눈금
        QDateTime dateTime = QDateTime::fromMSecsSinceEpoch(timeData.time);
        if(dateTime.date().day() != 1 && m_timeIntervalIndex == depth && m_timeRectWidth < minWidth)
            lineHeight = 0;
    }
    else if(interval == MSEC_HOUR * 12) {
        // 12시간 눈금
        if(m_timeIntervalIndex == depth && m_timeRectWidth < minWidth)
            lineHeight = 0;
        else if(intervalWidth < minWidth * 16)
            lineHeight = 7;
    }
    else {
        // 나머지 눈금
        if(m_timeIntervalIndex == depth && m_timeRectWidth < minWidth)
            lineHeight = 0;
        else if(intervalWidth < minWidth * 2)
            lineHeight = 5;
        else if(intervalWidth < minWidth * 16)
            lineHeight = 7;
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

    if(interval == MSEC_DAY*30 || (interval == MSEC_DAY && datetime.date().day() == 1))
        return QLocaleManager::Instance()->getMonth(datetime);  // 월 text

    if(interval == MSEC_DAY) {
        // 일 text
        if(timeData.depth == m_timeIntervalIndex && intervalWidth < (m_minWidth + 5) * 2)
            return "";
        else
            return datetime.toString("dd");
    }

    if(intervalWidth < (m_minWidth + 5) * 2)
        return "";

    if(interval < MSEC_MIN)
        timeText = QString::number(datetime.time().second()) + "s";
    else
        timeText = datetime.toString("hh:mm");

    return timeText;
}

QDateTime TimeAreaModel::getPrevNextTime(QDateTime currentTime, bool getNext) const
{
    // 인접한 눈금의 시간을 획득하는 함수

    if(getNext) {
        if(m_timeInterval == MSEC_DAY*30)
            currentTime = currentTime.addMonths(1); // 월 눈금만 표시되는 경우 1달씩 +
        else
            currentTime = currentTime.addMSecs((qint64)m_timeInterval);     // 다음 눈금 시간
    }
    else {
        if(m_timeInterval == MSEC_DAY*30)
            currentTime = currentTime.addMonths(-1); // 월 눈금만 표시되는 경우 1달씩 -
        else
            currentTime = currentTime.addMSecs(-(qint64)m_timeInterval);    // 이전 눈금 시간
    }

    if(m_timeInterval < MSEC_DAY*30 && m_timeInterval > MSEC_HOUR) {
        // m_timeInterval 만큼 시간 간격을 줄 경우 DST구간에서 오차 발생
        // 현재 시간 간격으로 나누어 떨어지는 시간이 되도록 1시간 +- 보정
        int currentHour = currentTime.time().hour();
        int hourGap = m_timeInterval / MSEC_HOUR;
        if(currentHour % hourGap == 1)
            currentTime = currentTime.addMSecs(-(qint64)MSEC_HOUR);
        else if(currentHour % hourGap == hourGap - 1)
            currentTime = currentTime.addMSecs((qint64)MSEC_HOUR);
    }

    return currentTime;
}

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

#include "GraphAreaModel.h"
#include <QDebug>

GraphAreaModel::GraphAreaModel()
{
    qulonglong startTime = 1614524400000;
    qulonglong endTime = 1617202800000;

    int index = 0;
    std::vector<Wisenet::Device::ChannelTimeline> timelineList;
    m_timeline.channelTimelineList["1"][1] = timelineList;
    while(startTime < endTime)
    {
        Wisenet::Device::ChannelTimeline timeline;
        timeline.startTime = startTime;
        timeline.endTime = startTime + 60000;
        timeline.type = index%2 == 0 ? Wisenet::Device::RecordingType::Normal : Wisenet::Device::RecordingType::Event;
        //timeline.type = Wisenet::Device::RecordingType::Normal;

        m_timeline.channelTimelineList["1"][1].push_back(timeline);

        startTime += 120000;
        index++;
    }

    qDebug("[timeline] timeline Data Count:%d", index);
}

GraphAreaModel::~GraphAreaModel()
{
}

// 전체 데이터 갱신 함수
void GraphAreaModel::refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth)
{
    m_data.clear();

    qulonglong visibleRange = visibleEndTime - visibleStartTime;

    GraphAreaData normalData;
    normalData.isEventRecord = false;
    GraphAreaData eventData;
    eventData.isEventRecord = true;
    QVector<GraphAreaData> normalDataList;
    QVector<GraphAreaData> eventDataList;

    for(auto& timeline : m_timeline.channelTimelineList["1"][1])
    {
        if(timeline.endTime <= visibleStartTime || timeline.startTime >= visibleEndTime)
            continue;

        int startPos = timeline.startTime <= visibleStartTime ? 0 : controlWidth * (timeline.startTime - visibleStartTime) / visibleRange;
        int endPos = timeline.endTime >= visibleEndTime ? controlWidth : controlWidth * (timeline.endTime - visibleStartTime) / visibleRange;
        bool isEventRec = timeline.type == Wisenet::Device::RecordingType::Normal ? false : true;
        if(startPos == endPos)
            endPos++;

        if(isEventRec)
        {
            if(endPos < eventData.startPosition || startPos > eventData.endPosition)
            {
                // 바로 앞 데이터와 인접하지 않은 경우
                if(eventData.startTime != 0)
                    eventDataList.append(eventData); // 마지막 데이터 append

                eventData = GraphAreaData(); // 신규 데이터 생성
                eventData.startTime = timeline.startTime;
                eventData.endTime = timeline.endTime;
                eventData.startPosition = startPos;
                eventData.endPosition = endPos;
                eventData.isEventRecord = isEventRec;
            }
            else
            {
                // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
                if(startPos <= eventData.startPosition)
                {
                    eventData.startTime = timeline.startTime;
                    eventData.startPosition = startPos;
                }
                if(endPos >= eventData.endPosition)
                {
                    eventData.endTime = timeline.endTime;
                    eventData.endPosition = endPos;
                }
            }
        }
        else
        {
            if(endPos < normalData.startPosition || startPos > normalData.endPosition)
            {
                // 바로 앞 데이터와 인접하지 않은 경우
                if(normalData.startTime != 0)
                    normalDataList.append(normalData); // 마지막 데이터 append

                normalData = GraphAreaData(); // 신규 데이터 생성
                normalData.startTime = timeline.startTime;
                normalData.endTime = timeline.endTime;
                normalData.startPosition = startPos;
                normalData.endPosition = endPos;
                normalData.isEventRecord = isEventRec;
            }
            else
            {
                // 바로 앞 데이터와 인접 한 경우 두 데이터를 합체
                if(startPos <= normalData.startPosition)
                {
                    normalData.startTime = timeline.startTime;
                    normalData.startPosition = startPos;
                }
                if(endPos >= normalData.endPosition)
                {
                    normalData.endTime = timeline.endTime;
                    normalData.endPosition = endPos;
                }
            }
        }
    }

    if(eventData.startTime != 0)
        eventDataList.append(eventData);
    if(normalData.startTime != 0)
        normalDataList.append(normalData);

    for(auto& data : normalDataList)
        m_data.append(data);

    for(auto& data : eventDataList)
        m_data.append(data);

    //qDebug("[timeline] timeline Rectangle Count:%d", m_data.count());
}

// 특정 index의 데이터 획득 함수들
int GraphAreaModel::startPosition(int index)
{
    if(m_data.count() > index)
        return m_data[index].startPosition;

    return 0;
}

int GraphAreaModel::width(int index)
{
    if(m_data.count() > index)
        return m_data[index].endPosition - m_data[index].startPosition;

    return 0;
}

bool GraphAreaModel::isEventRecord(int index)
{
    if(m_data.count() > index)
        return m_data[index].isEventRecord;

    return false;
}
//

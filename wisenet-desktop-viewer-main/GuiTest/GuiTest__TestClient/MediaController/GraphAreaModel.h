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

#ifndef GRAPHAREAMODEL_H
#define GRAPHAREAMODEL_H

#include <QObject>
#include "DeviceClient/DeviceStructure.h"

struct GraphAreaData
{
    int startPosition = 0;
    int endPosition = 0;
    bool isEventRecord = false;
    qulonglong startTime = 0;
    qulonglong endTime = 0;
};

class GraphAreaModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int dataCount READ dataCount CONSTANT)

public:
    GraphAreaModel();
    ~GraphAreaModel();

    // Q_PROPERTY
    int dataCount() {return m_data.count();}

    // Q_INVOKABLE
    Q_INVOKABLE void refreshGraphDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);
    Q_INVOKABLE int startPosition(int index);
    Q_INVOKABLE int width(int index);
    Q_INVOKABLE bool isEventRecord(int index);

private:
    QList<GraphAreaData> m_data;
    Wisenet::Device::RecordingTimelineViewResult m_timeline;
};

#endif // GRAPHAREAMODEL_H

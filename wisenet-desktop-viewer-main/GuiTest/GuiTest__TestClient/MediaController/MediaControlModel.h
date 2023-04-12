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

#include <QObject>
#include <QQueue>
#include "DeviceClient/DeviceStructure.h"

struct visibleTimeRangeDelta
{
    qulonglong startTimeDelta = 0;
    qulonglong rangeDelta = 0;
};

class MediaControlModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qulonglong recordingStartTime READ recordingStartTime CONSTANT)
    Q_PROPERTY(qulonglong recordingEndTime READ recordingEndTime CONSTANT)
    Q_PROPERTY(qulonglong visibleStartTime READ visibleStartTime WRITE setvisibleStartTime NOTIFY visibleStartTimeChanged)
    Q_PROPERTY(qulonglong visibleEndTime READ visibleEndTime WRITE setvisibleEndTime NOTIFY visibleEndTimeChanged)
    Q_PROPERTY(qulonglong visibleTimeRange READ visibleTimeRange NOTIFY visibleTimeRangeChanged)

public:
    MediaControlModel();
    ~MediaControlModel();

    static void registerQml();

    enum qControlType
    {
        pause,
        play,
        stepForward,
        stepBackward,
    };
    Q_ENUM(qControlType)
    
    Q_INVOKABLE void requestDeviceMediaControl(MediaControlModel::qControlType type);
    Q_INVOKABLE void setVisibleTimeRange(qulonglong startTimeDelta, qulonglong rangeDelta);

    qulonglong recordingStartTime() {return m_recordingStartTime;}
    qulonglong recordingEndTime() {return m_recordingEndTime;}
    qulonglong visibleStartTime() {return m_visibleStartTime;}
    qulonglong visibleEndTime() {return m_visibleEndTime;}
    qulonglong visibleTimeRange() {return m_visibleTimeRange;}

    void setvisibleStartTime(qulonglong time) {m_visibleStartTime = time;}
    void setvisibleEndTime(qulonglong time) {m_visibleEndTime = time;}

signals:
    void visibleStartTimeChanged(qulonglong startTime);
    void visibleEndTimeChanged(qulonglong endTime);
    void visibleTimeRangeChanged(qulonglong range);

private:
    qulonglong m_recordingStartTime = 0;    // 녹화 시작 시간
    qulonglong m_recordingEndTime = 0;      // 녹화 끝 시간
    qulonglong m_visibleStartTime = 0;      // 화면 표시 시작 시작
    qulonglong m_visibleEndTime = 0;        // 화면 표시 끝 시간
    qulonglong m_visibleTimeRange = 0;      // 화면 표시 범위

    Wisenet::Device::ControlType convertMediaControlType(qControlType type);
};

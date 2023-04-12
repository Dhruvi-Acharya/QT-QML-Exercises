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
#pragma once

#ifndef TIMEAREAMODEL_H
#define TIMEAREAMODEL_H

#include <QAbstractListModel>

struct TimeAreaData
{
    qulonglong time = 0;
    int depth = 0;
    float width = 0;
};

const qulonglong MSEC_SEC = 1000;
const qulonglong MSEC_MIN = MSEC_SEC * 60;
const qulonglong MSEC_HOUR = MSEC_MIN * 60;
const qulonglong MSEC_DAY = MSEC_HOUR * 24;

class TimeAreaModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames
    {
        TimeRole = Qt::UserRole,
        DepthRole,
        RectWidthRole,
        LineHeightRole,
        TimeTextRole,
    };

    explicit TimeAreaModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int index, const TimeAreaData& timeData);
    void append(const TimeAreaData& timeData);
    void remove(int index);
    void clear();

    // Q_INVOKABLE
    Q_INVOKABLE void refreshTimeDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);
    Q_INVOKABLE void updateTimeDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QList<TimeAreaData> m_data;
    float m_minWidth = 0;           // 최소 눈금 간격 (pixel)
    float m_timeRectWidth = 0;      // 눈금 간격 (pixel)
    qulonglong m_timeInterval = 0;  // 눈금 간격 (Msec)
    QList<qulonglong> m_timeIntervalList;   // 표시 가능 한 눈금 간격 리스트
    int m_timeIntervalIndex = -1;   // 눈금 간격 리스트의 현재 index

    void setTimeInterval(qulonglong visibleTimeRange, float controlWidth);
    int getDepth(const qulonglong time) const;
    int getLineHeight (const TimeAreaData timeData) const;
    QString getText(const TimeAreaData timeData) const;
    QDateTime getPrevNextTime(QDateTime currentTime, bool getNext) const;
};

#endif // TIMEAREAMODEL_H

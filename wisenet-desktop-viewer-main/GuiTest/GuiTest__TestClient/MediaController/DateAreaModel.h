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

#ifndef DATEAREAMODEL_H
#define DATEAREAMODEL_H

#include <QAbstractItemModel>

struct DateAreaData
{
    float width = 0;
    QString timeText = "";
    bool evenNumberStyle = false;
    qulonglong startTime = 0;
    qulonglong endTime = 0;
};

class DateAreaModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames
    {
        WidthRole = Qt::UserRole,
        TextRole,
        EvenNumberStyleRole,
    };

    enum class DataIntervals
    {
        Month,
        Day,
        Hour,
        Minute,
    };

    explicit DateAreaModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int index, const DateAreaData& timeData);
    void append(const DateAreaData& timeData);
    void remove(int index);
    void clear();

    // Q_INVOKABLE
    Q_INVOKABLE void refreshDateDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);
    Q_INVOKABLE void updateDateDataList(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QList<DateAreaData> m_data;
    DataIntervals m_dataInterval = DataIntervals::Day;

    DataIntervals getDataInterval(qulonglong visibleStartTime, qulonglong visibleEndTime, float controlWidth);
    QDateTime addDateTime(QDateTime dateTime, int diff);
    QString getDateTimeText(QDateTime dateTime);
    bool getEvenNumberStyle(QDateTime dateTime);
};

#endif // DATEAREAMODEL_H

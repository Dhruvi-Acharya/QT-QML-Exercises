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

#include <QObject>
#include <QAbstractItemModel>
#include "DeviceClient/DeviceStructure.h"
#include "WisenetViewerDefine.h"

struct RecordTypeFilterData
{
    WisenetViewerDefine::RecordFilterType recordType;
    bool checked;
};

class RecordTypeFilterViewModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames
    {
        RecordTypeRole = Qt::UserRole,
        CheckedRole,
    };

    explicit RecordTypeFilterViewModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Data edit functionality:
    void insert(int index, const RecordTypeFilterData& data);
    void append(const RecordTypeFilterData& data);
    void remove(int index);
    void clear();

    // Q_INVOKABLE
    Q_INVOKABLE void reloadRecordTypeFilter(const QString& deviceID, quint32 filter);
    Q_INVOKABLE quint32 saveRecordTypeFilter(const int& row, const bool& checked);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QList<RecordTypeFilterData> m_data;
    WisenetViewerDefine::RecordFilterType m_recordTypeFilter = WisenetViewerDefine::RecordFilterType::All;
};

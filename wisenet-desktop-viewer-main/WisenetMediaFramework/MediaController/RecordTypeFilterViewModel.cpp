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

#include "RecordTypeFilterViewModel.h"
#include "DeviceClient/DeviceStructure.h"
#include "QCoreServiceManager.h"

RecordTypeFilterViewModel::RecordTypeFilterViewModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[RecordTypeRole] = "recordType";
    m_roleNames[CheckedRole] = "checked";
}

int RecordTypeFilterViewModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant RecordTypeFilterViewModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    const RecordTypeFilterData& data = m_data.at(row);

    switch(role)
    {
    case RecordTypeRole:
        return data.recordType;
    case CheckedRole:
        return data.checked;
    }

    return QVariant();
}

QHash<int, QByteArray> RecordTypeFilterViewModel::roleNames() const
{
    return m_roleNames;
}

void RecordTypeFilterViewModel::insert(int index, const RecordTypeFilterData& data)
{
    if(index < 0 || index > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, data);
    endInsertRows();
}

void RecordTypeFilterViewModel::append(const RecordTypeFilterData& data)
{
    insert(m_data.count(), data);
}

void RecordTypeFilterViewModel::remove(int index)
{
    if(index < 0 || index > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}

void RecordTypeFilterViewModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count()-1);
    m_data.clear();
    endRemoveRows();
}

void RecordTypeFilterViewModel::reloadRecordTypeFilter(const QString& deviceID, quint32 filter)
{
    clear();

    Wisenet::Device::Device device;
#ifndef MEDIA_FILE_ONLY
    if(!QCoreServiceManager::Instance().DB()->FindDevice(deviceID.toStdString(), device))
        return;
#else
    return;
#endif

    WisenetViewerDefine::RecordFilterType m_recordTypeFilter = (WisenetViewerDefine::RecordFilterType)filter;

    for(auto& recordType : device.deviceCapabilities.recordingTypes)
    {
        RecordTypeFilterData data;
        data.recordType = (WisenetViewerDefine::RecordFilterType)recordType;
        if(data.recordType == WisenetViewerDefine::RecordFilterType::None)
            continue;
        data.checked = (m_recordTypeFilter & data.recordType) == data.recordType;

        append(data);
    }
}

quint32 RecordTypeFilterViewModel::saveRecordTypeFilter(const int& row, const bool& checked)
{
    if(row < 0 || row >= m_data.count())
        return (quint32)m_recordTypeFilter;

    m_data[row].checked = checked;

    if(checked)
        m_recordTypeFilter |= m_data[row].recordType;
    else
        m_recordTypeFilter -= m_data[row].recordType;

    return (quint32)m_recordTypeFilter;
}

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

#include "MaskItemListModel.h"
#include "LogSettings.h"

#include <QDateTime>


MaskItemListModel::MaskItemListModel(QObject *parent)
    : QAbstractListModel(parent), m_tempMaskItem(-1, WisenetMaskType::MaskType::Manual)
{
    m_roleNames[MaskIdRole] = "maskId";
    m_roleNames[MaskTypeRole] = "maskType";
    m_roleNames[SelectedRole] = "selected";
    m_roleNames[VisibleRole] = "visible";
    m_roleNames[CropImageRole] = "cropImage";
    m_roleNames[StartTimeRole] = "startTime";
    m_roleNames[EndTimeRole] = "endTime";
    m_roleNames[MaskNameRole] = "maskName";
}

MaskItemListModel::~MaskItemListModel()
{
    qDebug() << "~MaskItemListModel()";
    clear();
}

int MaskItemListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant MaskItemListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    switch(role)
    {
    case MaskIdRole:
        return m_data.values()[row]->maskId;
    case MaskTypeRole:
        return (int)m_data.values()[row]->maskType;
    case SelectedRole:
        return m_data.values()[row]->selected;
    case VisibleRole:
        return m_data.values()[row]->visible;
    case CropImageRole:
        return m_data.values()[row]->cropImage;
    case StartTimeRole:
        return m_data.values()[row]->m_maskIndicatorMap.first().ptsTimestampMsec;
    case EndTimeRole:
        return m_data.values()[row]->m_maskIndicatorMap.last().ptsTimestampMsec;
    case MaskNameRole:
        return m_data.values()[row]->name;
    }

    return QVariant();
}

bool MaskItemListModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
    {
        return false;
    }

    auto maskItem = m_data.values().at(row);

    switch(role)
    {
    case StartTimeRole:
    {
        qint64 updatedStartTime = value.toLongLong();
        auto firstIndicator = maskItem->m_maskIndicatorMap.first();

        //qDebug() << "MaskItemListModel::setData StartTimeRole updatedStartTime" << QDateTime::fromMSecsSinceEpoch(updatedStartTime);;
        //qDebug() << "MaskItemListModel::setData StartTimeRole   firstIndicator" << QDateTime::fromMSecsSinceEpoch(firstIndicator.ptsTimestampMsec);;

        if(firstIndicator.ptsTimestampMsec < updatedStartTime)
        {
            // Delete
            qDebug() << "MaskItemListModel::setData StartTimeRole Delete";
            maskItem->DeleteIndicatorsFromStartToParameter(updatedStartTime);
        }
        else if(firstIndicator.ptsTimestampMsec > updatedStartTime)
        {
            // Add
            qDebug() << "MaskItemListModel::setData StartTimeRole Add";
            MaskIndicator newIndicator(firstIndicator.maskRectangle, updatedStartTime);
            maskItem->AddKeyFrameMaskIndicator(newIndicator);
        }
        else
        {
            qDebug() << "MaskItemListModel::setData StartTimeRole No Action";
        }

        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {StartTimeRole});

        return true;
    }
    case EndTimeRole:
    {
        qint64 updatedEndTime = value.toLongLong();
        auto lastIndicator = maskItem->m_maskIndicatorMap.last();

        //qDebug() << "MaskItemListModel::setData EndTimeRole updatedStartTime" << QDateTime::fromMSecsSinceEpoch(updatedEndTime);
        //qDebug() << "MaskItemListModel::setData EndTimeRole   firstIndicator" << QDateTime::fromMSecsSinceEpoch(lastIndicator.ptsTimestampMsec);;

        if(lastIndicator.ptsTimestampMsec < updatedEndTime)
        {
            // Add
            qDebug() << "MaskItemListModel::setData EndTimeRole Add";
            MaskIndicator newIndicator(lastIndicator.maskRectangle, updatedEndTime);
            maskItem->AddKeyFrameMaskIndicator(newIndicator);
        }
        else if(lastIndicator.ptsTimestampMsec > updatedEndTime)
        {
            // Delete
            qDebug() << "MaskItemListModel::setData EndTimeRole Delete";
            maskItem->DeleteIndicatorsFromParamterToEnd(updatedEndTime);
        }
        else
        {
            qDebug() << "MaskItemListModel::setData EndTimeRole No Action";
        }

        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {EndTimeRole});

        return true;
    }
    case SelectedRole:
    {
        auto list = m_data.values();
        list[row]->selected = value.toBool();

        qDebug() << "MaskItemListModel::setData id:" << list[row]->maskId << "row:" << row << "selected:" << list[row]->selected;

        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {SelectedRole});

        return true;
    }
    case VisibleRole:
    {
        auto list = m_data.values();
        list[row]->visible = value.toBool();

        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {VisibleRole});

        return true;
    }
    case MaskNameRole:
    {
        auto list = m_data.values();
        list[row]->name = value.toString();

        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {MaskNameRole});

        return true;
    }
    default:
        break;
    }

    return false;
}


QHash<int, QByteArray> MaskItemListModel::roleNames() const
{
    return m_roleNames;
}

void MaskItemListModel::append(MaskItem* data)
{
    beginInsertRows(QModelIndex(), m_data.count(), m_data.count());
    m_data.insert(data->maskId,data);
    endInsertRows();

    emit itemAdded();
}

void MaskItemListModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count()-1);
    m_data.clear();
    endRemoveRows();
}

void MaskItemListModel::setSelectedTrueByIndex(int row)
{
    auto list = m_data.values();

    for(int i=0; i<m_data.count(); i++)
    {
        if( i == row)
            list.at(i)->selected = true;
        else
            list.at(i)->selected = false;
    }

    emit dataChanged(index(0,0,QModelIndex()), index(m_data.count(),0,QModelIndex()), {SelectedRole});
}

void MaskItemListModel::setSelectedFalseAll()
{
    auto list = m_data.values();

    for(int i=0; i<m_data.count(); i++)
    {
        list.at(i)->selected = false;
    }

    emit dataChanged(index(0,0,QModelIndex()), index(m_data.count(),0,QModelIndex()), {SelectedRole});
}

void MaskItemListModel::removeByMaskId(int maskId)
{
    if(!m_data.contains(maskId))
    {
        qDebug() << "MaskItemListModel::removeByMaskId maskId" << maskId << "is not exist return";
        return;
    }

    beginRemoveRows(QModelIndex(), m_data.keys().indexOf(maskId), m_data.keys().indexOf(maskId));
    m_data.remove(maskId);
    endRemoveRows();
}

void MaskItemListModel::AddKeyFrameMaskIndicator(int maskId, MaskIndicator newIndicator)
{
    if(m_data.contains(maskId))
    {
        auto maskItem = m_data[maskId];
        maskItem->AddKeyFrameMaskIndicator(newIndicator);

        int row = m_data.values().indexOf(maskItem);
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {StartTimeRole, EndTimeRole});
        return;
    }
}

// get mask rectangle list of input timestamp
QList<NormalMaskRect> MaskItemListModel::getMaskRectList(qint64 timestamp, bool isTracking)
{
    QList<NormalMaskRect> maskRectList;
    for(auto& maskItem : m_data) {
        if(!maskItem->visible)
            continue;
        if(isTracking && maskItem->maskType != WisenetMaskType::MaskType::Fixed && maskItem->selected)
            continue;   // do not mask selected manual/auto mask while tracking is on
        NormalMaskRect maskRect;
        if(maskItem->GetInterpolationMaskRect(timestamp, maskRect)) {
            maskRectList.push_back(maskRect);
        }
    }
    return maskRectList;
}

// get mask rectangle map of input timestamp (QVariantMap type for qml)
QVariantMap MaskItemListModel::getMaskRectMap(qint64 timestamp, bool isTracking)
{
    QVariantMap maskRectMap;
    for(auto& maskItem : m_data) {
        NormalMaskRect maskRect;
        if(maskItem->selected && isTracking) {
            if(m_tempMaskItem.GetInterpolationMaskRect(timestamp, maskRect))
                maskRectMap.insert(QString::number(maskItem->maskId), QRectF(maskRect.normalX, maskRect.normalY, maskRect.normalWidth, maskRect.normalHeight));
        }
        else {
            if(maskItem->GetInterpolationMaskRect(timestamp, maskRect)) {
                // key : maskId, value : QRectF type normalized mask rect
                maskRectMap.insert(QString::number(maskItem->maskId), QRectF(maskRect.normalX, maskRect.normalY, maskRect.normalWidth, maskRect.normalHeight));
            }
        }
    }
    return maskRectMap;
}

void MaskItemListModel::updateCropImage(QImage fullImage, int maskId, int x, int y, int width, int height)
{
    if(m_data.contains(maskId))
    {
        MaskItem* item = m_data[maskId];

        //MaskIndicator maskIndicator = item->m_maskIndicatorList.at(0);
        //MaskIndicator newIndicator(QRect(x,y,width,height), maskIndicator.ptsTimestampMsec);
        //item->m_maskIndicatorList[0] = newIndicator;

        item->fullImage = fullImage;
        item->cropImage = item->fullImage.copy(x, y, width, height);

        int row = m_data.values().indexOf(item);
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {CropImageRole});
    }
}

QList<MaskItem> MaskItemListModel::getMaskItemList()
{
    QList<MaskItem> itemList;

    for(MaskItem* item : qAsConst(m_data)) {
        itemList.push_back(*item);
    }

    return itemList;
}

bool MaskItemListModel::isNeedUpdateCrop(int maskId, qint64 timestamp)
{
    if (m_data.contains(maskId)){
        MaskItem* item = m_data[maskId];
        if (timestamp <= item->m_maskIndicatorMap.firstKey() || item->maskType == WisenetMaskType::MaskType::Fixed)
            return true;
        else
            return false;
    }
    else
        return false;
}

void MaskItemListModel::initTempMaskIndicatorList()
{
    for(auto& maskItem : m_data) {
        if(maskItem->selected) {
            SPDLOG_DEBUG("MaskItemListModel::initTempMaskIndicatorList maskId : {}", maskItem->maskId);
            m_tempMaskItem = MaskItem(maskItem->maskId, maskItem->maskType);
        }
    }
}

void MaskItemListModel::AddTempMaskIndicator(MaskIndicator newIndicator)
{
    m_tempMaskItem.AddKeyFrameMaskIndicator(newIndicator);
}

void MaskItemListModel::updateMaskIndicator(int maskId)
{
    QList<MaskItem*> values = m_data.values();
    for(int row = 0 ; row<values.size() ; row++) {
        if(values[row]->maskId != maskId)
            continue;

        // update temp mask indicator
        if(!m_tempMaskItem.m_maskIndicatorMap.isEmpty())
            values[row]->UpdateMaskIndicator(m_tempMaskItem.m_maskIndicatorMap);

        // update time range
        emit dataChanged(index(row,0,QModelIndex()), index(row,0,QModelIndex()), {StartTimeRole, EndTimeRole});

        break;
    }
    m_tempMaskItem.Reset();
}

int MaskItemListModel::getSelectedMaskItemType()
{
    for(auto& maskItem : m_data) {
        if(maskItem->selected)
            return (int)maskItem->maskType;
    }
    return -1;
}
QPair<int, NormalMaskRect> MaskItemListModel::getSelectedMaskItemRect(qint64 timestamp)
{
    QPair<int, NormalMaskRect> selectedAutoMaskItem;
    selectedAutoMaskItem.first = -1;

    // 파라미터로 받은 timestamp의 Rectangle 좌표를 반환하는 함수
    for(auto& maskItem : m_data) {
        if(maskItem->selected)  {
            selectedAutoMaskItem.first = maskItem->maskId;
            NormalMaskRect maskRect;
            if(maskItem->GetInterpolationMaskRect(timestamp, maskRect)) {
                selectedAutoMaskItem.second = maskRect;
            }
        }
    }

    return selectedAutoMaskItem;
}

QPair<int, NormalMaskRect>  MaskItemListModel::getMaskItembyId(int maskId, qint64 timestamp)
{
    QPair<int, NormalMaskRect> selectedMaskItem;
    if (m_data.contains(maskId)){
        selectedMaskItem.first = m_data[maskId]->maskId;
        NormalMaskRect maskRect;
        if(m_data[maskId]->GetInterpolationMaskRect(timestamp, maskRect)) {
            selectedMaskItem.second = maskRect;
        }
    }
    return selectedMaskItem;
}


// WisenetMediaPlayer에서 임시 트래킹 박스를 초기화하기 위해 좌표를 얻는 함수
NormalMaskRect MaskItemListModel::getManualMaskRect(qint64 timestamp)
{
    NormalMaskRect maskRect;

    for(auto& maskItem : m_data) {
        if(maskItem->selected && (maskItem->maskType == WisenetMaskType::MaskType::Manual))   {
            if (timestamp < maskItem->m_maskIndicatorMap.firstKey())
                maskRect = maskItem->m_maskIndicatorMap.first().maskRectangle;
            else if (maskItem->m_maskIndicatorMap.lastKey() < timestamp)
                maskRect = maskItem->m_maskIndicatorMap.last().maskRectangle;
            else {
                maskItem->GetInterpolationMaskRect(timestamp, maskRect);
            }
        }
    }
    return maskRect;
}


void MaskItemListModel::UpdateFixedMaskItem(int maskId, NormalMaskRect maskRect)
{
    if(m_data.contains(maskId)) {
        MaskItem* item = m_data[maskId];
        SPDLOG_DEBUG("FIXED ITEM - MaskItemListModel::UpdateFixedMaskItem maskID : {}", maskId);
        item->UpdateFixedMaskIndicator(maskRect);
    }
}


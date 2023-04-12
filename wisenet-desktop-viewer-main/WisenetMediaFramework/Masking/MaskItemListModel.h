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
#include "MaskItem.h"
#include <QMap>

class MaskItemListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum RoleNames
    {
        MaskIdRole = Qt::UserRole,
        MaskTypeRole,
        SelectedRole,
        VisibleRole,
        CropImageRole,
        StartTimeRole,
        EndTimeRole,
        MaskNameRole,
    };

    explicit MaskItemListModel(QObject *parent = nullptr);
    virtual ~MaskItemListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    // Data edit functionality:
    void append(MaskItem* data);

    void removeByMaskId(int maskId);
    void AddKeyFrameMaskIndicator(int maskId, MaskIndicator newIndicator);
    void UpdateFixedMaskItem(int maskId, NormalMaskRect maskRect);

    QList<NormalMaskRect> getMaskRectList(qint64 timestamp, bool isTracking);
    QVariantMap getMaskRectMap(qint64 timestamp, bool isTracking);
    QList<MaskItem> getMaskItemList();
    NormalMaskRect getManualMaskRect(qint64 timestamp);

    bool isNeedUpdateCrop(int maskId, qint64 timestamp);
    void updateCropImage(QImage fullImage, int maskId, int x, int y, int widht, int height);

    void initTempMaskIndicatorList();
    void AddTempMaskIndicator(MaskIndicator newIndicator);
    void updateMaskIndicator(int maskId);

    int getSelectedMaskItemType();
    QPair<int, NormalMaskRect> getSelectedMaskItemRect(qint64 timestamp);
    QPair<int, NormalMaskRect> getMaskItembyId(int maskId, qint64 timestamp);

public slots:
    void clear();
    void setSelectedTrueByIndex(int row);
    void setSelectedFalseAll();

signals:
    void itemAdded();

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QHash<int, QByteArray> m_roleNames;
    QMap<int, MaskItem*> m_data;
    MaskItem m_tempMaskItem;
};

#pragma once

#include <QAbstractListModel>
#include "BestShotItemModel.h"
#include "QCoreServiceManager.h"

class BestShotListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum RoleNames
    {
        ImageRole = Qt::UserRole,
        WidthRole,
        HeightRole,
        PointXRole,
        PointYRole,
        SequenceRole,
        VisibleRole,
        FlagCheckedRole,
        AttributeTypeRole,
        CameraNameRole,
        DisplayTimeRole,

        DeviceIdRole,
        ChannelIdRole,
        DeviceTimeRole,

        PersonGenderRole,
        PersonClothingTopColorRole,
        PersonClothingBottomColorRole,
        PersonBagRole,

        FaceGenderRole,
        FaceAgeRole,
        FaceGlassesRole,
        FaceMaskRole,

        VehicleTypeRole,
        VehicleColorRole,

        OcrNumberRole,

        MetaAttributeRole,
    };

    explicit BestShotListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    // Data edit functionality:
    void insert(int index, BestShotItemModel* bestShotItem);
    void append(BestShotItemModel* bestShotItem);
    void remove(int index);
    void clear();

    QList<BestShotItemModel*>& getData(){ return m_data; }
    void applyFilter(bool filter0, bool filter1, bool filter2);
    void applyFilter(Wisenet::Device::MetaFilter& filter);
    void setVisibleAll(bool visible);
    void setPositions(int viewWidth, int bestShotHeight);
    void refresh(int startIndex=0);

    int getRowCountList(){ return rowLastPosition.count(); }
    QList<int>& getItemCountList(){ return rowItemCountList; }

    int getVisibleCount();

    int getLastRowVisibleStartIndex(int count);

public slots:
    void itemChanged(BestShotItemModel* item);

protected:
    virtual QHash<int, QByteArray> roleNames() const override;

private:
    QList<int> rowLastPosition;
    QList<int> rowItemCountList;
    QHash<int, QByteArray> m_roleNames;
    QList<BestShotItemModel*> m_data;
};

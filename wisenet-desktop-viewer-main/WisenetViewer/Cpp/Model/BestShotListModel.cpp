#include "BestShotListModel.h"

BestShotListModel::BestShotListModel(QObject *parent)
    : QAbstractListModel{parent}
{
    m_roleNames[ImageRole] = "image";
    m_roleNames[WidthRole] = "bestShotWidth";
    m_roleNames[HeightRole] = "bestShotHeight";
    m_roleNames[PointXRole] = "pointX";
    m_roleNames[PointYRole] = "pointY";
    m_roleNames[SequenceRole] = "sequence";
    m_roleNames[VisibleRole] = "visible";
    m_roleNames[FlagCheckedRole] = "flagChecked";
    m_roleNames[AttributeTypeRole] = "attributeType";
    m_roleNames[CameraNameRole] = "cameraName";
    m_roleNames[DisplayTimeRole] = "displayTime";

    m_roleNames[DeviceIdRole] = "deviceId";
    m_roleNames[ChannelIdRole] = "channelId";
    m_roleNames[DeviceTimeRole] = "deviceTime";

    m_roleNames[PersonGenderRole] = "personGender";
    m_roleNames[PersonClothingTopColorRole] = "personClothingTopColor";
    m_roleNames[PersonClothingBottomColorRole] = "personClothingBottomColor";
    m_roleNames[PersonBagRole] = "personBag";

    m_roleNames[FaceGenderRole] = "faceGender";
    m_roleNames[FaceAgeRole] = "faceAge";
    m_roleNames[FaceGlassesRole] = "faceGlasses";
    m_roleNames[FaceMaskRole] = "faceMask";

    m_roleNames[VehicleTypeRole] = "vehicleType";
    m_roleNames[VehicleColorRole] = "vehicleColor";

    m_roleNames[OcrNumberRole] = "ocrNumber";

    m_roleNames[MetaAttributeRole] = "metaAttr";
}

int BestShotListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant BestShotListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
    {
        qDebug() << "BestShotListModel::data return";
        return QVariant();
    }

    BestShotItemModel* bestShotItem = m_data.at(row);

    //qDebug() << "BestShotListModel::data" << index << role;

    switch(role)
    {
    case ImageRole:
        return bestShotItem->image();
    case WidthRole:
        return bestShotItem->width();
    case HeightRole:
        return bestShotItem->height();
    case PointXRole:
        return bestShotItem->x();
    case PointYRole:
        return bestShotItem->y();
    case SequenceRole:
        return row;
    case VisibleRole:
        return bestShotItem->visible();
    case FlagCheckedRole:
        return bestShotItem->flagChecked();
    case AttributeTypeRole:
        return bestShotItem->attributeType();
    case CameraNameRole:
        return bestShotItem->cameraName();
    case DisplayTimeRole:
        return bestShotItem->displayTime();

    case DeviceIdRole:
        return bestShotItem->deviceId();
    case ChannelIdRole:
        return bestShotItem->channelId();
    case DeviceTimeRole:
        return bestShotItem->deviceTime();

    case PersonGenderRole:
        return bestShotItem->personGender();
    case PersonClothingTopColorRole:
        return bestShotItem->personClothingTopColor();
    case PersonClothingBottomColorRole:
        return bestShotItem->personClothingBottomColor();
    case PersonBagRole:
        return bestShotItem->personBag();
    case FaceGenderRole:
        return bestShotItem->faceGender();
    case FaceAgeRole:
        return bestShotItem->faceAge();
    case FaceGlassesRole:
        return bestShotItem->faceGlasses();
    case FaceMaskRole:
        return bestShotItem->faceMask();
    case VehicleTypeRole:
        return bestShotItem->vehicleType();
    case VehicleColorRole:
        return bestShotItem->vehicleColor();

    case OcrNumberRole:
        return bestShotItem->ocrNumber();

    case MetaAttributeRole:
        return QVariant::fromValue(bestShotItem->metaAttr());
    }

    return QVariant();
}

bool BestShotListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    int row = index.row();
    if(row < 0 || row >= m_data.count())
    {
        return false;
    }

    BestShotItemModel* item = m_data.at(row);

    switch(role)
    {
    case VisibleRole:
    {
        qDebug() << "BestShotListModel::setData VisibleRole " << value.toString();
        item->setVisible(value.toBool());

        QVector<int> role;
        role << RoleNames::VisibleRole;
        emit dataChanged(index, index, role);
        return true;
    }
    case FlagCheckedRole:
    {
        qDebug() << "BestShotListModel::setData FlagCheckedRole " << value.toString();
        item->setFlagChecked(value.toBool());

        QVector<int> role;
        role << RoleNames::FlagCheckedRole;
        emit dataChanged(index, index, role);
        return true;
    }
    default:
        break;
    }

    return false;
}

void BestShotListModel::itemChanged(BestShotItemModel* item)
{
    int idx = m_data.indexOf(item);

    //qDebug() << "BestShotListModel::item FlagCheckedRole " << idx;

    QVector<int> role;
    role << RoleNames::ImageRole;
    role << RoleNames::WidthRole;
    role << RoleNames::HeightRole;
    role << RoleNames::PointXRole;
    role << RoleNames::PointYRole;
    role << RoleNames::VisibleRole;
    role << RoleNames::FlagCheckedRole;

    emit dataChanged(QModelIndex(index(idx, 0)), QModelIndex(index(idx, 0)), role);
}

QHash<int, QByteArray> BestShotListModel::roleNames() const
{
    return m_roleNames;
}

void BestShotListModel::insert(int index, BestShotItemModel* bestShotItem)
{
    if(index < 0 || index > m_data.count())
    {
        return;
    }

    connect(bestShotItem, &BestShotItemModel::itemChanged, this, &BestShotListModel::itemChanged);

    beginInsertRows(QModelIndex(), index, index);
    m_data.insert(index, bestShotItem);
    endInsertRows();
}

void BestShotListModel::append(BestShotItemModel* bestShotItem)
{
    insert(m_data.count(), bestShotItem);
}

void BestShotListModel::remove(int index)
{
    if(index < 0 || index > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), index, index);
    m_data.removeAt(index);
    endRemoveRows();
}

void BestShotListModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginRemoveRows(QModelIndex(), 0, m_data.count()-1);
    qDeleteAll(m_data);
    m_data.clear();
    endRemoveRows();
}

void BestShotListModel::applyFilter(bool filter0, bool filter1, bool filter2)
{
    //qDebug() << "BestShotListModel::applyFilter()";

    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];

        if(i%3 == 0){
            if(filter0)
                item->setVisible(true);
            else
                item->setVisible(false);
        }
        else if(i%3 == 1){
            if(filter1)
                item->setVisible(true);
            else
                item->setVisible(false);
        }
        else{
            if(filter2)
                item->setVisible(true);
            else
                item->setVisible(false);
        }
    }
}

void BestShotListModel::applyFilter(Wisenet::Device::MetaFilter& filter)
{
    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];
        item->applyFilter(filter);
    }
}

void BestShotListModel::setVisibleAll(bool visible)
{
    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];
        item->setVisible(visible);
    }
}

void BestShotListModel::setPositions(int viewWidth, int bestShotHeight)
{    
    // Filter를 해제했을 경우 데이터가 중간에 들어와야되는 상황이 있어서 sort 함.
    // 맨 마지막 row만 보는 로직은 삭제함. 2023-01-05
    std::sort(m_data.begin(), m_data.end(), [](BestShotItemModel* m1, BestShotItemModel* m2) {
        return m1->deviceTime() > m2->deviceTime();
    });

    // rowLastPosition : 각 row별 마지막 위치
    // rowItemCountList : 각 row 당 item 갯수
    rowLastPosition.clear();
    rowItemCountList.clear();

    int row = 0;

    // 현재의 x 위치
    int xPosition = 0;

    // row에 있는 아이템 수
    int rowItemCount = 0;

    // spacing
    int additionalSpacing = bestShotHeight / 50 - 2;
    int columnSpacing = 5 + additionalSpacing;
    int rowSpacing = 8 + additionalSpacing;

    // row에서의 순서 (0 base)
    int sequenceOfRow = 0;

    // Step 1 : 각 bestshot에 row number를 매기고 기본 spacing을 적용
    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];

        if(!item->visible())
            continue;

        // 높이 고정
        item->setHeight(bestShotHeight);

        // 너비 계산
        int bestShotWidth = (int)((float)bestShotHeight * (float)item->imageWidth() / (float)item->imageHeight());
        item->setWidth(bestShotWidth);

        // 너비+여백을 현재 위치에 더한 값이 전체 영역의 너비보다 커지게 되면 다음 열로
        if(xPosition + item->width() + columnSpacing >= viewWidth)
        {
            rowLastPosition.append(xPosition);
            rowItemCountList.append(rowItemCount);

            row++;
            xPosition = 0;
            rowItemCount = 0;
            sequenceOfRow = 0;
        }

        // 행 spacing을 추가해줌.
        if(sequenceOfRow > 0)
            xPosition += columnSpacing;

        // x 위치를 잡음
        item->setX(xPosition);

        // 다음에 사용할 x위치
        xPosition += item->width();
        rowItemCount++;
        item->setY((bestShotHeight + rowSpacing) * row);
        item->setRowNumber(row);
        item->setSequenceOfRow(sequenceOfRow++);

        //qDebug() << "xPosition" << xPosition << "i" << i << "x" << item->x() << "y" << item->y() << "width" << item->width() << "height" << item->height();
    }

    // 마지막 줄
    rowLastPosition.append(xPosition);
    rowItemCountList.append(rowItemCount);

    // Step2 : row별로 분류된 아이템들에 추가 여백을 넣어 양쪽 정렬하는 단계
    // 현재 row
    int currentRow = -1;
    // 현재 몫
    int currentQuotient = -1;
    // 현재 나머지
    int currentRemainder = -1;
    // 전체 영역의 너비 - 각 row별 너비
    int remainingWidth = -1;

    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];

        if(!item->visible())
            continue;

        // row가 바뀔 때만 한번 계산
        if(currentRow != item->rowNumber())
        {
            currentRow = item->rowNumber();

            // 마지막 줄은 안맞춘다.
            if(currentRow == rowLastPosition.count()-1)
                break;

            remainingWidth = viewWidth - rowLastPosition.at(currentRow);

            // 한줄에 한개만 있을 때에 대한 예외처리 (0으로 나누어지는 상황)
            if(rowItemCountList.at(currentRow)-1 > 0)
            {
                currentQuotient = remainingWidth / (rowItemCountList.at(currentRow)-1);
                currentRemainder = remainingWidth % (rowItemCountList.at(currentRow)-1);
            }
            else
            {
                currentQuotient = remainingWidth;
                currentRemainder = 0;
            }

            //qDebug() << "rowNumber" << currentRow << "remainWidth" << remainWidth << "itemCount" << rowItemCountList.at(currentRow) << "currentQuotient" << currentQuotient << "currentRemainder" << currentRemainder;
        }

        if(remainingWidth == 0)
            continue;

        int remainder = (currentRemainder > 0 && item->sequenceOfRow() <= currentRemainder) ? item->sequenceOfRow() : currentRemainder;

        //qDebug() << "remainder" << remainder;

        // 현재 x위치 + 몫에 대한 너비 + 나머지 너비
        item->setX(item->x() + (item->sequenceOfRow() * currentQuotient) + remainder);
    }
}

void BestShotListModel::refresh(int startIndex)
{
    //qDebug() << "BestShotListModel::refresh()";
    QVector<int> role;
    role << RoleNames::ImageRole;
    role << RoleNames::WidthRole;
    role << RoleNames::HeightRole;
    role << RoleNames::PointXRole;
    role << RoleNames::PointYRole;
    role << RoleNames::VisibleRole;

    emit dataChanged(QModelIndex(index(startIndex,0)), QModelIndex(index(m_data.count()-1, 0)), role);
}

int BestShotListModel::getVisibleCount()
{
    int retCount = 0;

    for(int i=0; i<m_data.count(); i++)
    {
        BestShotItemModel* item = m_data[i];

        if(item->visible())
            retCount++;
    }

    return retCount;
}

int BestShotListModel::getLastRowVisibleStartIndex(int count)
{
    int visibleCount = count;
    int ret = 0;

    for(int i=m_data.count()-1; i>=0; i--)
    {
        BestShotItemModel* item = m_data[i];

        if(item->visible()){
            visibleCount--;
        }

        if(visibleCount == 0)
        {
            ret = i;
            break;
        }
    }

    qDebug() << "getLastRowVisibleStartIndex()" << ret;
    return ret;
}

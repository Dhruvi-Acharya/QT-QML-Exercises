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
#include "GridListViewModel.h"
#include <QtConcurrent>
#include "LogSettings.h"
#include "WisenetViewerDefine.h"

GridListViewModel::GridListViewModel(QObject *parent)
    :QAbstractTableModel(parent)
    ,m_hoveredRow(-1)
    ,m_totalCount(0)
    ,m_data(QSharedPointer<QVector<QVector<QVariant>>>(new QVector<QVector<QVariant>>))
    ,m_dateTimeDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_cameraDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_sortIndex(1)
    ,m_sortOrder(Qt::DescendingOrder)
{
    connect(&m_thread, &GridListThread::processCompleted, this, &GridListViewModel::processComplated);

    m_image = QImage(1,1, QImage::Format_RGB888);
    m_image.fill(qRgba(35, 35, 35, 255));
}

int GridListViewModel::imageWidth()
{
    return m_imageWidth;
}

int GridListViewModel::imageHeight()
{
    return m_imageHeight;
}

bool GridListViewModel::visible()
{
    return m_visible;
}

void GridListViewModel::setImage(const QImage& image)
{
    if(m_image != image)
    {
        m_image = image;
        imageChanged();
    }
}

void GridListViewModel::setImageWidth(int imageWidth)
{
    if(m_imageWidth != imageWidth)
    {
        m_imageWidth = imageWidth;
        imageWidthChanged(imageWidth);
    }
}

void GridListViewModel::setImageHeight(int imageHeight)
{
    if(m_imageHeight != imageHeight)
    {
        m_imageHeight = imageHeight;
        imageHeightChanged(imageHeight);
    }
}

void GridListViewModel::setVisible(bool visible)
{
    if(m_visible != visible)
    {
        m_visible = visible;
        visibleChanged(visible);
    }
}

void GridListViewModel::getBestShotImage(QString deviceID, QString imageUrl)
{
    qDebug() << "GridListViewModel::getBestShotImage";
//    emit openLoading();

    std::shared_ptr<Wisenet::Device::DeviceUrlSnapShotRequest> deviceUrlSnapShotRequest;

    deviceUrlSnapShotRequest = std::make_shared<Wisenet::Device::DeviceUrlSnapShotRequest>();
    deviceUrlSnapShotRequest->deviceID = deviceID.toStdString();

    deviceUrlSnapShotRequest->urlList.push_back(imageUrl.toStdString());

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceUrlSnapShot,
                this, deviceUrlSnapShotRequest,
            [=](const QCoreServiceReplyPtr& reply)
    {
        qDebug() << "DeviceUrlSnapShot - Res start";
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceUrlSnapShotResponse>(reply->responseDataPtr);
        if(!response || response->isFailed()){
            qDebug() << "DeviceUrlSnapShot, isFailed={}" << response->isFailed();
        }
        else if(response->snapShots.empty()){
            qDebug() << "DeviceUrlSnapShot, snapShots is empty";
        }
        else{
            if (response->snapShots.size() > 0) {
                if(response->snapShots[0].isSuccess)
                {
                    QImage image = QImage::fromData(response->snapShots[0].snapshotData.data(), response->snapShots[0].snapshotData.size(), "JPEG");
                    m_image = image;

                    setImageWidth(m_image.width());
                    setImageHeight(m_image.height());

                    emit imageChanged();
//                    qDebug() << "GridListViewModel::getBestShotImage : emit imageChanged";
//                    qDebug() << "m_imageWidth="<<m_imageWidth<<", m_imageHeight="<<m_imageHeight;
                }
            }
        }
    });
}

int GridListViewModel::rowCount(const QModelIndex &) const
{
//    qDebug() << "GridListViewModel::rowCount()";
    return m_index.size();
}

int GridListViewModel::columnCount(const QModelIndex &) const
{
//    qDebug() << "GridListViewModel::columnCount()";
    return 5;
}

QVariant GridListViewModel::data(const QModelIndex &index, int role) const
{
//    qDebug() << "GridListViewModel::data()";
    switch (role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    case EventImageRole:
    case DeviceIdRole:
    case ChannelIdRole:
    case ServiceTimeRole:
    case DeviceTimeRole:
    case ImageUrlRole:
        return getDataFromMetaAttrs(index, (Role)role);
    case BookmarkIdRole:
        return getDataBookmarkIdRole(index);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> GridListViewModel::roleNames() const
{
//    qDebug() << "GridListViewModel::roleNames()";
    return {
        {Qt::DisplayRole, "displayRole"},
        {HoveredRole, "hoveredRole"},
        {EventImageRole, "EventImageRole"},
        {DeviceIdRole, "deviceIdRole"},
        {ChannelIdRole, "channelIdRole"},
        {ServiceTimeRole, "serviceTimeRole"},
        {DeviceTimeRole, "deviceTimeRole"},
        {EventLogIdRole, "EventLogIdRole"},
        {BookmarkIdRole, "bookmarkIdRole"},
        {ImageUrlRole, "imageUrlRole"}
    };
}

void GridListViewModel::setData(std::vector<Wisenet::Device::MetaAttribute>* metaAttrs)
{
//    qDebug() << "GridListViewModel::setData()";

    m_metaAttrs = metaAttrs;
    SPDLOG_DEBUG("start process");
    m_thread.process(m_metaAttrs);
}

void GridListViewModel::sort(int column, Qt::SortOrder order)
{
    qDebug() << "GridListViewModel::sort()";
    m_sortIndex = column;
    m_sortOrder = order;

    sortFilter();
}

void GridListViewModel::setHoveredRow(int rowNum, bool hovered)
{
//    qDebug() << "GridListViewModel::setHoveredRow()";
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, 4)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, 4)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, 4)), role);
        }
    }
}

bool GridListViewModel::exportCSV(QString path, QString fileName)
{
//    qDebug() << "GridListViewModel::exportCSV()";
    QString filePath = path + QDir::separator() + fileName;
    QFile csvFile(filePath);

    if (!csvFile.open(QIODevice::WriteOnly))
        return false;

    QTextStream textStream(&csvFile);
    textStream.setCodec("UTF-8");
    textStream.setGenerateByteOrderMark(true);
    QStringList stringList;

    stringList << "Date&Time"<<"Device Name"<<"Event Type"<<"Decsription";
    textStream << stringList.join( ',' )+"\n";

    for(int row = 1; row < rowCount(); row++ )
    {
        stringList.clear();

        for(int column = 0; column < columnCount(); column++ )
            stringList << data(index(row, column), Qt::DisplayRole).toString();

        textStream << stringList.join( ',' )+"\n";
    }

    csvFile.close();
    return true;
}

QVariant GridListViewModel::getDataBookmarkIdRole(const QModelIndex &index) const
{
//    qDebug() << "GridListViewModel::getDataBookmarkIdRole()";
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][0];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue;
}

void GridListViewModel::filter(const QStringList &channels)
{
    qDebug() << "GridListViewModel::filter() : channels="<<channels;
    m_filterChannels = channels;

    sortFilter();
}

void GridListViewModel::processComplated(std::vector<Wisenet::Device::MetaAttribute>* metaAttrs,
                                      QSharedPointer<QVector<QVector<QVariant>>> data,
                                      QSharedPointer<QVector<int>> dateTimeDescIndex,
                                      QSharedPointer<QVector<int>> cameraDescIndex,
                                      QVector<QStringList> cameraList,
                                      QSet<QString> metaAttrSet)
{
    qDebug() << "GridListViewModel::processComplated()";

    beginResetModel();

    m_metaAttrs = metaAttrs;
    m_data = data;

    m_index.resize(dateTimeDescIndex->size());
    std::copy(dateTimeDescIndex->begin(),dateTimeDescIndex->end(),m_index.begin());

    m_dateTimeDescIndex = dateTimeDescIndex;
    m_cameraDescIndex = cameraDescIndex;

    endResetModel();

    if(!m_metaAttrs){
        m_totalCount = 0;
    }else {
        m_totalCount = (int)m_metaAttrs->size();
    }
    emit totalCountChanged();

    emit searchFinished(cameraList);

}

QVariant GridListViewModel::getDataDisplayRole(const QModelIndex &index) const
{
//    qDebug() << "GridListViewModel::getDataDisplayRole()";
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][index.column()];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue.toString();
}

QVariant GridListViewModel::getDataFromMetaAttrs(const QModelIndex &index, Role roleName) const
{
//    qDebug() << "GridListViewModel::getDataFromMetaAttrs()";
    int row = m_index[index.row()];

    if(!m_metaAttrs || m_metaAttrs->size() <= (size_t)row){
        if(roleName == ServiceTimeRole || roleName == DeviceTimeRole)
            return 0;
        else
            return QVariant(QVariant::String);
    }

    switch (roleName) {
    case DeviceIdRole:
        return QString::fromUtf8((*m_metaAttrs)[row].aiBaseAttribute.deviceID.c_str());
    case ChannelIdRole:
        return QString::number((*m_metaAttrs)[row].aiBaseAttribute.channelId);
    case DeviceTimeRole:
        return QVariant::fromValue((*m_metaAttrs)[row].aiBaseAttribute.dateTime);
    case ImageUrlRole:
        return QString::fromUtf8((*m_metaAttrs)[row].aiBaseAttribute.imageUrl.c_str());
    default:
        return QVariant(QVariant::String);
    }
}

void GridListViewModel::clearData()
{
    qDebug() << "GridListViewModel::clearData()";
    beginResetModel();

    m_sortIndex = 1;
    m_sortOrder = Qt::DescendingOrder;
    m_filterChannels.clear();

    m_data.reset();
    m_index.clear();

    m_dateTimeDescIndex.reset();
    m_cameraDescIndex.reset();

    endResetModel();
}

void GridListViewModel::sortFilter()
{
    qDebug() << "GridListViewModel::sortFilter";

    if(!m_dateTimeDescIndex){
        qDebug() << "m_dateTimeDescIndex is null";
        return;
    }
    if(!m_cameraDescIndex){
        qDebug() << "m_cameraDescIndex is null";
        return;
    }

    if(!m_metaAttrs){
        qDebug() << "m_eventDescIndex is null";
        return;
    }

    if(!m_data){
        qDebug() << "m_data is null";
        return;
    }

    beginResetModel();

    QVector<int> index;

    if(1 == m_sortIndex){
        qDebug() << "Start sort 1 : sort of Date&Time";
        // sort of Date&Time
        index.resize(m_dateTimeDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }

    }else if(2 == m_sortIndex){
        qDebug() << "Start sort 2 : sort of Channel name";
        // sort of Channel name
        index.resize(m_cameraDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }

    }

    if((m_filterChannels.size() > 0 || m_isFiltering)
            && index.size() > 0){
        qDebug() << "Start filterChannel";
        QVector<int> result = QtConcurrent::blockingFiltered(index,[this](int n){

            return containsChannelFilter(n) && containsFilteringOption(n);
        });

        if(result.size() > 0){
            qDebug() << "copy filter channel result size: " << result.size();
            m_index.reserve(result.size());
            m_index = result;
        }else {
            m_index.clear();
        }

    }else {

        m_index.clear();
        if(index.size() > 0){
            qDebug() << "copy m_index size : " << index.size();
            m_index.reserve(index.size());
            m_index = index;
        }
    }

    qDebug() << "complated sortFilter m_index size : " << m_index.size();

    endResetModel();

    // Total Count를 필터링된 Result 기준으로 반영
    if(m_index.empty()){
        m_totalCount = 0;
    }else {
        m_totalCount = (int)m_index.size();
    }
    emit totalCountChanged();
}

bool GridListViewModel::containsChannelFilter(int index)
{
//    qDebug() << "GridListViewModel::containsChannelFilter("<<index<<")";
//    qDebug() << "m_filterChannels="<<m_filterChannels;
    if(m_filterChannels.empty()){
        return true;
    }

    foreach(const QString &channel , m_filterChannels){

        QStringList id = channel.split('_');
//        qDebug() << "deviceID:"<<QString::fromUtf8((*m_metaAttrs)[index].aiBaseAttribute.deviceID.c_str());
//        qDebug() << "channelID:"<<QString::number((*m_metaAttrs)[index].aiBaseAttribute.channelId);

        if((QString::fromUtf8((*m_metaAttrs)[index].aiBaseAttribute.deviceID.c_str()) == id[0])
                &&(QString::number((*m_metaAttrs)[index].aiBaseAttribute.channelId) == id[1])){
            return true;
        }
    }

    return false;
}

// 필터링 조건에 맞도록 조건 동일한지 체크
bool GridListViewModel::containsFilteringOption(int index)
{
    qDebug() << "GridListViewModel::containsFilteringOption()";
    bool hasAttr = false;

    if((*m_metaAttrs)[index].attributeType == Wisenet::Device::AiClassType::person)
    {
        if((*m_metaAttrs)[index].personGender.size() > 0 && m_filter.personGender.size() > 0)
        {
            for(int i=0; i<(*m_metaAttrs)[index].personGender.size(); i++)
            {
                if(m_filter.personGender.find((*m_metaAttrs)[index].personGender.at(i)) != m_filter.personGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].personClothingTopsColor.size() > 0 && m_filter.personClothingTopsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].personClothingTopsColor.size(); i++)
            {
                if(m_filter.personClothingTopsColor.find((*m_metaAttrs)[index].personClothingTopsColor.at(i)) != m_filter.personClothingTopsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].personClothingBottomsColor.size() > 0 && m_filter.personClothingBottomsColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].personClothingBottomsColor.size(); i++)
            {
                if(m_filter.personClothingBottomsColor.find((*m_metaAttrs)[index].personClothingBottomsColor.at(i)) != m_filter.personClothingBottomsColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].personBelongingBag.size() > 0 && m_filter.personBelongingBag.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].personBelongingBag.size(); i++)
            {
                if(m_filter.personBelongingBag.find((*m_metaAttrs)[index].personBelongingBag.at(i)) != m_filter.personBelongingBag.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }
    }
    else if((*m_metaAttrs)[index].attributeType == Wisenet::Device::AiClassType::vehicle)
    {
        if((*m_metaAttrs)[index].vehicleType.size() > 0 && m_filter.vehicleType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].vehicleType.size(); i++)
            {
                if(m_filter.vehicleType.find((*m_metaAttrs)[index].vehicleType.at(i)) != m_filter.vehicleType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].vehicleColor.size() > 0 && m_filter.vehicleColor.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].vehicleColor.size(); i++)
            {
                if(m_filter.vehicleColor.find((*m_metaAttrs)[index].vehicleColor.at(i)) != m_filter.vehicleColor.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }
    }
    else if((*m_metaAttrs)[index].attributeType == Wisenet::Device::AiClassType::face)
    {
        if((*m_metaAttrs)[index].faceGender.size() > 0 && m_filter.faceGender.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].faceGender.size(); i++)
            {
                if(m_filter.faceGender.find((*m_metaAttrs)[index].faceGender.at(i)) != m_filter.faceGender.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].faceAgeType.size() > 0 && m_filter.faceAgeType.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].faceAgeType.size(); i++)
            {
                if(m_filter.faceAgeType.find((*m_metaAttrs)[index].faceAgeType.at(i)) != m_filter.faceAgeType.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].faceOpticals.size() > 0 && m_filter.faceOpticals.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].faceOpticals.size(); i++)
            {
                if(m_filter.faceOpticals.find((*m_metaAttrs)[index].faceOpticals.at(i)) != m_filter.faceOpticals.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }

        if((*m_metaAttrs)[index].faceMask.size() > 0 && m_filter.faceMask.size() > 0)
        {
            hasAttr = false;
            for(int i=0; i<(*m_metaAttrs)[index].faceMask.size(); i++)
            {
                if(m_filter.faceMask.find((*m_metaAttrs)[index].faceMask.at(i)) != m_filter.faceMask.end())
                {
                    hasAttr = true;
                    break;
                }
            }

            if(hasAttr == false)
            {
                return false;
            }
        }
    }
    else if((*m_metaAttrs)[index].attributeType == Wisenet::Device::AiClassType::ocr)
    {
        QString source = QString::fromUtf8((*m_metaAttrs)[index].ocrText.c_str());
        QString filterText = QString::fromUtf8(m_filter.ocrText.c_str());

        if(!filterText.contains('*'))
            filterText = "*" + filterText + "*";

        //qDebug() << "OCR filterText"<< filterText;

        QRegExp exp(filterText);
        exp.setPatternSyntax(QRegExp::Wildcard);

        if(!exp.exactMatch(source))
        {
            return false;
        }
    }

//    qDebug() << index << ": true";
    return true;
}

void GridListViewModel::applyFilter(Wisenet::Device::MetaFilter& filter)
{
    m_filter = filter;
    m_isFiltering = true;

    sortFilter();
}

QString GridListViewModel::GetDeletedDeviceName(std::string &deviceID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    name = itor->second.name.c_str();
    name += " [" + deletedItem + "]";
    return name;
}

QString GridListViewModel::GetDeletedChannelName(std::string &deviceID, std::string channelID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    auto itorChannel = itor->second.channels.find(channelID);
    if(itorChannel == itor->second.channels.end()){
        name = deletedItem;
        return name;
    }

    name = itorChannel->second.name.c_str();
    name += " [" + deletedItem + "]";

    return name;
}

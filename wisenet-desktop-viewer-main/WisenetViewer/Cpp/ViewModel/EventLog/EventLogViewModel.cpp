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
#include "EventLogViewModel.h"
#include <QtConcurrent>
#include "LogSettings.h"
#include "WisenetViewerDefine.h"

EventLogViewModel::EventLogViewModel(QObject *parent)
    :QAbstractTableModel(parent)
    ,m_hoveredRow(-1)
    ,m_totalCount(0)
    ,m_data(QSharedPointer<QVector<QVector<QVariant>>>(new QVector<QVector<QVariant>>))
    ,m_dateTimeDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_cameraDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_eventDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_sortIndex(1)
    ,m_sortOrder(Qt::DescendingOrder)
{
    connect(&m_thread, &EventLogThread::processCompleted, this, &EventLogViewModel::processComplated);
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered, this, &EventLogViewModel::onCoreServiceEventTriggered, Qt::QueuedConnection);
}

int EventLogViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int EventLogViewModel::columnCount(const QModelIndex &) const
{
    return 5;
}

QVariant EventLogViewModel::data(const QModelIndex &index, int role) const
{
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
    case EventLogIdRole:
        return getDataFromEventLogs(index, (Role)role);
    case BookmarkIdRole:
        return getDataBookmarkIdRole(index);
    case IsDeletedChannelRole:
        return getIsDeletedChannelRole(index);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> EventLogViewModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayRole"},
        {HoveredRole, "hoveredRole"},
        {EventImageRole, "EventImageRole"},
        {DeviceIdRole, "deviceIdRole"},
        {ChannelIdRole, "channelIdRole"},
        {ServiceTimeRole, "serviceTimeRole"},
        {DeviceTimeRole, "deviceTimeRole"},
        {EventLogIdRole, "eventLogIdRole"},
        {BookmarkIdRole, "bookmarkIdRole"},
        {IsDeletedChannelRole, "isDeletedChannelRole"}
    };
}

void EventLogViewModel::search(const QDateTime &from, const QDateTime &to, const bool &allChannel, QStringList channels, const bool &allEvent, QStringList events)
{
    //qDebug() << "EventLogViewModel::search from = " << from;
    //qDebug() << "EventLogViewModel::search to = " << to;
    //qDebug() << "EventLogViewModel::search allChannel = " << allChannel;
    //qDebug() << "EventLogViewModel::search channels = " << channels;
    //qDebug() << "EventLogViewModel::search allEvent = " << allEvent;
    //qDebug() << "EventLogViewModel::search allEvent = " << events;

    emit searchStarted();

    clearData();

    SPDLOG_DEBUG("start Search");
    auto getEventLogRequest = std::make_shared<Wisenet::Core::GetEventLogRequest>();

    getEventLogRequest->fromTime = from.toUTC().toMSecsSinceEpoch();
    getEventLogRequest->toTime = to.toUTC().toMSecsSinceEpoch();
    getEventLogRequest->isAllChannel = allChannel;
    getEventLogRequest->isAllType = allEvent;

    foreach(QString channel, channels){
        QStringList ids = channel.split('_');
        Wisenet::Core::GetEventLogRequest::ChannelID channelID;
        channelID.deviceId = ids[0].toStdString();
        channelID.channelId = ids[1].toStdString();
        getEventLogRequest->channelIds.push_back(channelID);
    }

    foreach(QString event, events){
        getEventLogRequest->types.push_back(event.toStdString());
    }



    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetEventLog,
                this, getEventLogRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::GetEventLogResponse>(reply->responseDataPtr);
        if(!response || response->isFailed()){
            SPDLOG_INFO("GetEventLog, isFailed={}", response->isFailed());
            QVector<QStringList> deviceList;
            QSet<QString> eventList;
            emit searchFinished(deviceList,eventList);
            return;
        }

        if(!response->eventLogs || response->eventLogs->empty()){
            SPDLOG_INFO("GetEventLog, eventlog is empty");
            QVector<QStringList> deviceList;
            QSet<QString> eventList;
            emit searchFinished(deviceList,eventList);
            return;
        }

        SPDLOG_DEBUG("start process");
        m_thread.process(response->deletedDevices, response->eventLogs);

    });
}

void EventLogViewModel::sort(int column, Qt::SortOrder order)
{
    m_sortIndex = column;
    m_sortOrder = order;

    sortFilter();
}

void EventLogViewModel::setHoveredRow(int rowNum, bool hovered)
{
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

bool EventLogViewModel::exportCSV(QString path, QString fileName)
{
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

    for(int row = 0; row < rowCount(); row++ )
    {
        stringList.clear();

        for(int column = 1; column < columnCount(); column++ )
            stringList << data(index(row, column), Qt::DisplayRole).toString();

        textStream << stringList.join( ',' )+"\n";
    }

    csvFile.close();
    return true;
}

void EventLogViewModel::filter(const QStringList &channels, const QStringList &events, const QString &description)
{
    m_filterChannels = channels;
    m_filterEvents = events;
    m_filterDescription = description;

    sortFilter();
}

void EventLogViewModel::processComplated(std::shared_ptr<std::vector<Wisenet::Core::EventLog>> eventLogs,
                                      QSharedPointer<QVector<QVector<QVariant>>> data,
                                      QSharedPointer<QVector<int>> dateTimeDescIndex,
                                      QSharedPointer<QVector<int>> cameraDescIndex,
                                      QSharedPointer<QVector<int>> eventDescIndex,
                                      QVector<QStringList> cameraList,
                                      QSet<QString> eventList)
{
    SPDLOG_DEBUG("processComplated");

    beginResetModel();

    m_eventLogs = eventLogs;
    m_data = data;

    m_index.resize(dateTimeDescIndex->size());
    std::copy(dateTimeDescIndex->begin(),dateTimeDescIndex->end(),m_index.begin());

    m_dateTimeDescIndex = dateTimeDescIndex;
    m_cameraDescIndex = cameraDescIndex;
    m_eventDescIndex = eventDescIndex;

    endResetModel();

    if(!m_eventLogs){
        m_totalCount = 0;
    }else {
        m_totalCount = (int)m_eventLogs->size();
    }
    emit totalCountChanged();

    emit searchFinished(cameraList, eventList);

}

QVariant EventLogViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][index.column()];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue.toString();
}

QVariant EventLogViewModel::getDataBookmarkIdRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][0];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue;
}

QVariant EventLogViewModel::getIsDeletedChannelRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][5];

    if(cellValue.isNull())
        return QVariant(QVariant::Bool);

    return cellValue;
}

QVariant EventLogViewModel::getDataEventImageRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    if(!m_eventLogs || m_eventLogs->size() <= (size_t)row){
        return QVariant(QVariant::String);
    }

    return QString::fromUtf8((*m_eventLogs)[row].type.c_str());
}

QVariant EventLogViewModel::getDataFromEventLogs(const QModelIndex &index, Role roleName) const
{
    int row = m_index[index.row()];

    if(!m_eventLogs || m_eventLogs->size() <= (size_t)row){
        if(roleName == ServiceTimeRole || roleName == DeviceTimeRole)
            return 0;
        else
            return QVariant(QVariant::String);
    }

    switch (roleName) {
    case EventImageRole:
        return QString::fromUtf8((*m_eventLogs)[row].type.c_str());
    case DeviceIdRole:
        return QString::fromUtf8((*m_eventLogs)[row].deviceID.c_str());
    case ChannelIdRole:
        return QString::fromUtf8((*m_eventLogs)[row].channelID.c_str());
    case ServiceTimeRole:
        return (*m_eventLogs)[row].serviceUtcTimeMsec;
    case DeviceTimeRole:
        return (*m_eventLogs)[row].deviceUtcTimeMsec;
    case EventLogIdRole:
        return QString::fromUtf8((*m_eventLogs)[row].eventLogID.c_str());
    default:
        return QVariant(QVariant::String);
    }
}

void EventLogViewModel::clearData()
{
    beginResetModel();

    m_sortIndex = 1;
    m_sortOrder = Qt::DescendingOrder;
    m_filterChannels.clear();
    m_filterEvents.clear();
    m_filterDescription = "";

    m_eventLogs.reset();
    m_data.reset();
    m_index.clear();

    m_dateTimeDescIndex.reset();
    m_cameraDescIndex.reset();
    m_eventDescIndex.reset();

    endResetModel();

    m_totalCount = 0;
    emit totalCountChanged();
}

void EventLogViewModel::sortFilter()
{
    qDebug() << "start sortFilter";

    if(!m_dateTimeDescIndex){
        qDebug() << "m_dateTimeDescIndex is null";
        return;
    }
    if(!m_cameraDescIndex){
        qDebug() << "m_cameraDescIndex is null";
        return;
    }
    if(!m_eventDescIndex){
        qDebug() << "m_eventDescIndex is null";
        return;
    }

    if(!m_eventLogs){
        qDebug() << "m_eventDescIndex is null";
        return;
    }

    if(!m_data){
        qDebug() << "m_data is null";
        return;
    }

    beginResetModel();

    QVector<int> index;

    if(0 == m_sortIndex){
        qDebug() << "Start sort 0";
        // sort of Bookmark Checked
        index.resize(m_data.data()->size());
        std::iota(index.begin(), index.end(), 0);
        std::sort(index.begin(),index.end(),[this](int indexA, int indexB){
            bool valueA = (*m_data)[indexA][0].toString() != "";
            bool valueB = (*m_data)[indexB][0].toString() != "";

            if(Qt::SortOrder::DescendingOrder == m_sortOrder)
                return (valueA == true && valueB == false) ||
                        (valueA == true && valueB == true && indexA < indexB) ||
                        (valueA == false && valueB == false && indexA < indexB);
            else
                return (valueA == false && valueB == true) ||
                        (valueA == true && valueB == true && indexA < indexB) ||
                        (valueA == false && valueB == false && indexA < indexB);
        });
    }else if(1 == m_sortIndex){
        qDebug() << "Start sort 1";
        index.resize(m_dateTimeDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }
    }else if(2 == m_sortIndex){
        qDebug() << "Start sort 2";
        index.resize(m_cameraDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }
    }else if(3 == m_sortIndex){
        qDebug() << "Start sort 3";
        index.resize(m_eventDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_eventDescIndex->begin(),m_eventDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_eventDescIndex->begin(),m_eventDescIndex->end(),index.begin());
        }
    }

    if((m_filterChannels.size() > 0 || m_filterEvents.size() > 0 || m_filterDescription.size() > 0)
            && index.size() > 0){
        qDebug() << "Start filterChannel";
        QVector<int> result = QtConcurrent::blockingFiltered(index,[this](int n){

            return containsChannelFilter(n) && containsEventFilter(n) && containsDescriptionFilter(n);
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
}

bool EventLogViewModel::containsEventFilter(int index)
{
    if(m_filterEvents.empty()){
        return true;
    }

    foreach(const QString &event , m_filterEvents){
        if((QString::fromUtf8((*m_eventLogs)[index].type.c_str()) == event)){
            return true;
        }
    }
    return false;
}

bool EventLogViewModel::containsDescriptionFilter(int index)
{
    if(0 == m_filterDescription.size()){
        return true;
    }

    if((*m_data)[index][4].toString().contains(m_filterDescription, Qt::CaseInsensitive)){
        return true;
    }

    return false;
}

bool EventLogViewModel::containsChannelFilter(int index)
{
    if(m_filterChannels.empty()){
        return true;
    }

    foreach(const QString &channel , m_filterChannels){

        QStringList id = channel.split('_');

        if((QString::fromUtf8((*m_eventLogs)[index].deviceID.c_str()) == id[0])
                &&(QString::fromUtf8((*m_eventLogs)[index].channelID.c_str()) == id[1])){
            return true;
        }
    }

    return false;
}

void EventLogViewModel::onCoreServiceEventTriggered(QCoreServiceEventPtr event)
{
    if(!m_eventLogs)
        return;

    auto serviceEvent = event->eventDataPtr;
    if (serviceEvent->EventTypeId() == Wisenet::Core::SaveBookmarkEventType) {
        auto bookmarkEvent = std::static_pointer_cast<Wisenet::Core::SaveBookmarkEvent>(event->eventDataPtr);
        Wisenet::uuid_string bookmarkId = bookmarkEvent->bookmark.bookmarkID;
        std::string eventLogId = bookmarkEvent->bookmark.eventLogID;

        if(eventLogId == "")
            return;

        int dataRow = 0;
        for(dataRow = 0 ; dataRow < (*m_eventLogs).size() ; dataRow++) {
            if((*m_eventLogs)[dataRow].eventLogID == eventLogId) {
                break;
            }
        }

        if(dataRow < (*m_data).size()) {
            (*m_data)[dataRow][0] = QString::fromUtf8(bookmarkId.c_str());
            int indexRow = m_index.indexOf(dataRow);
            if(indexRow != -1) {
                QVector<int> role;
                role << Role::BookmarkIdRole;
                emit dataChanged(QModelIndex(index(indexRow, 0)), QModelIndex(index(indexRow, 0)), role);
            }
        }
    }
    else if(serviceEvent->EventTypeId() == Wisenet::Core::RemoveBookmarksEventType)
    {
        auto bookmarkEvent = std::static_pointer_cast<Wisenet::Core::RemoveBookmarksEvent>(event->eventDataPtr);
        for(auto& bookmarkID : bookmarkEvent->bookmarkIDs)
        {
            for(int dataRow = 0 ; dataRow < (*m_data).size() ; dataRow++) {
                if((*m_data)[dataRow][0] == QString::fromUtf8(bookmarkID.c_str())) {
                    (*m_data)[dataRow][0] = "";

                    int indexRow = m_index.indexOf(dataRow);
                    if(indexRow != -1) {
                        QVector<int> role;
                        role << Role::BookmarkIdRole;
                        emit dataChanged(QModelIndex(index(indexRow, 0)), QModelIndex(index(indexRow, 0)), role);
                    }

                    break;
                }
            }
        }
    }
}

bool EventLogViewModel::isChannelExist(const QString &deviceId, const QString &channelId)
{
    Wisenet::Device::Device::Channel channel;
    return QCoreServiceManager::Instance().DB()->FindChannel(deviceId, channelId, channel);
}

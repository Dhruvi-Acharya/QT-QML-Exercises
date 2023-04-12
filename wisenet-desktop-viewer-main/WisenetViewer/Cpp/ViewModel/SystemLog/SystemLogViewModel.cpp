#include "SystemLogViewModel.h"
#include <QtConcurrent>
#include "LogSettings.h"
#include "WisenetViewerDefine.h"

SystemLogViewModel::SystemLogViewModel(QObject *parent)
    :QAbstractTableModel(parent)
    ,m_hoveredRow(-1)
    ,m_totalCount(0)
    ,m_data(QSharedPointer<QVector<QVector<QVariant>>>(new QVector<QVector<QVariant>>))
    ,m_dateTimeDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_cameraDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_eventDescIndex(QSharedPointer<QVector<int>>(new QVector<int>))
    ,m_sortIndex(0)
    ,m_sortOrder(Qt::DescendingOrder)
{
    connect(&m_thread, &SystemLogThread::processCompleted, this, &SystemLogViewModel::processComplated);
}


int SystemLogViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int SystemLogViewModel::columnCount(const QModelIndex &) const
{
    return 4;
}

QVariant SystemLogViewModel::data(const QModelIndex &index, int role) const
{
    switch (role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> SystemLogViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "displayRole"}, {HoveredRole, "hoveredRole"} };
}

void SystemLogViewModel::search(const QDateTime &from, const QDateTime &to, const bool &allChannel, QStringList channels, const bool &allEvent, QStringList events)
{
    //qDebug() << "EventLogViewModel::search from = " << from;
    //qDebug() << "EventLogViewModel::search to = " << to;
    //qDebug() << "EventLogViewModel::search allChannel = " << allChannel;
    //qDebug() << "EventLogViewModel::search channels = " << channels;
    //qDebug() << "EventLogViewModel::search allEvent = " << allEvent;
    //qDebug() << "EventLogViewModel::search allEvent = " << events;

    emit searchStarted();

    clearData();

    SPDLOG_DEBUG("start SystemLog Search");
    auto getSystemLogRequest = std::make_shared<Wisenet::Core::GetSystemLogRequest>();

    getSystemLogRequest->fromTime = from.toUTC().toMSecsSinceEpoch();
    getSystemLogRequest->toTime = to.toUTC().toMSecsSinceEpoch();
    getSystemLogRequest->isAllDevice = allChannel;
    getSystemLogRequest->isAllType = allEvent;

    foreach(QString channel, channels){
        getSystemLogRequest->deviceIds.push_back(channel.toStdString());
    }

    foreach(QString event, events){
        getSystemLogRequest->types.push_back(event.toStdString());
    }



    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::GetSystemLog,
                this, getSystemLogRequest,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Core::GetSystemLogResponse>(reply->responseDataPtr);

        if (response != nullptr)
        {
            if (response->isFailed()) {
                SPDLOG_INFO("GetEventLog, isFailed={}", response->isFailed());
                QVector<QStringList> deviceList;
                QSet<QString> eventList;
                m_totalCount = 0;
                emit totalCountChanged();
                emit searchFinished(deviceList,eventList);
                return;
            }

            if (response->eventLogs == nullptr || response->eventLogs->empty()) {
                SPDLOG_INFO("GetSystemLog, systemlog is empty, count={}", response->eventLogs->size());
                QVector<QStringList> deviceList;
                QSet<QString> eventList;
                m_totalCount = 0;
                emit totalCountChanged();
                emit searchFinished(deviceList,eventList);
                return;
            }
            else {
                // 2023.01.03. coverity
                SPDLOG_DEBUG("start process");
                m_thread.process(response->deletedDevices, response->eventLogs);
            }
        }      
    });
}

void SystemLogViewModel::sort(int column, Qt::SortOrder order)
{
    m_sortIndex = column;
    m_sortOrder = order;

    sortFilter();
}

void SystemLogViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, 3)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, 3)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, 3)), role);
        }
    }
}

bool SystemLogViewModel::exportCSV(QString path, QString fileName)
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

        for(int column = 0; column < columnCount(); column++ )
            stringList << data(index(row, column), Qt::DisplayRole).toString();

        textStream << stringList.join( ',' )+"\n";
    }

    csvFile.close();
    return true;
}

void SystemLogViewModel::filter(const QStringList &channels, const QStringList &events, const QString &description)
{
    m_filterChannels = channels;
    m_filterEvents = events;
    m_filterDescription = description;

    for(auto& event : m_filterEvents){
        if(0 == event.compare(Wisenet::AdditionalLogType::DeviceDisconnectedByService.c_str())){
            m_filterEvents.append(Wisenet::AdditionalLogType::DeviceDisconnectedWithRestriction.c_str());
            m_filterEvents.append(Wisenet::AdditionalLogType::DeviceDisconnectedWithError.c_str());
            m_filterEvents.append(Wisenet::AdditionalLogType::DeviceDisconnectedWithUnauthorized.c_str());
        }
    }

    sortFilter();
}

void SystemLogViewModel::processComplated(std::shared_ptr<std::vector<Wisenet::Core::EventLog>> eventLogs,
                                      QSharedPointer<QVector<QVector<QVariant>>> data,
                                      QSharedPointer<QVector<int>> dateTimeDescIndex,
                                      QSharedPointer<QVector<int>> cameraDescIndex,
                                      QSharedPointer<QVector<int>> eventDescIndex,
                                      QVector<QStringList> cameraList,
                                      QSet<QString> eventList)
{
    SPDLOG_DEBUG("System Event processComplated, logs({}),data({})", eventLogs->size(),data->size());

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

QVariant SystemLogViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = (*m_data)[row][index.column()];

    if(cellValue.isNull())
        return QVariant(QVariant::String);

    return cellValue.toString();
}

QVariant SystemLogViewModel::getDateEventImageRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    if(!m_eventLogs || m_eventLogs->size() <= (size_t)row){
        return QVariant(QVariant::String);
    }

    return QString::fromUtf8((*m_eventLogs)[row].type.c_str());
}

void SystemLogViewModel::clearData()
{
    beginResetModel();

    m_sortIndex = 0;
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
}

void SystemLogViewModel::sortFilter()
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
        index.resize(m_dateTimeDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_dateTimeDescIndex->begin(),m_dateTimeDescIndex->end(),index.begin());
        }

    }else if(1 == m_sortIndex){
        qDebug() << "Start sort 1";
        index.resize(m_cameraDescIndex->size());
        if(Qt::SortOrder::DescendingOrder == m_sortOrder){
            std::copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }else{
            std::reverse_copy(m_cameraDescIndex->begin(),m_cameraDescIndex->end(),index.begin());
        }

    }else if(2 == m_sortIndex){
        qDebug() << "Start sort 2";
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

bool SystemLogViewModel::containsEventFilter(int index)
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

bool SystemLogViewModel::containsDescriptionFilter(int index)
{
    if(0 == m_filterDescription.size()){
        return true;
    }

    if((*m_data)[index][3].toString().contains(m_filterDescription, Qt::CaseInsensitive)){
        return true;
    }

    return false;
}

bool SystemLogViewModel::containsChannelFilter(int index)
{
    if(m_filterChannels.empty()){
        return true;
    }

    foreach(const QString &channel , m_filterChannels){
        if((QString::fromUtf8((*m_eventLogs)[index].deviceID.c_str()) == channel)){
            return true;
        }
    }

    return false;
}


#include "SmartSearchViewModel.h"
#include "LogSettings.h"
#include "QLocaleManager.h"
#include "QtConcurrent"

SmartSearchViewModel::SmartSearchViewModel(QObject *parent)
    :m_hoveredRow(-1)
{
    SPDLOG_DEBUG("SmartSearchViewModel::SmartSearchViewModel");
}

SmartSearchViewModel::~SmartSearchViewModel()
{

}

void SmartSearchViewModel::selectChannel(const QString channelKey)
{
    m_channelKey = channelKey;
    qDebug() << "SmartSearchViewModel::selectChannel() channelKey : " << m_channelKey.toStdString().c_str();
    reloadTableData();
}

QString getTypeKey(Wisenet::Device::SmartSearchResultType type){
    QString languageKey = "Unknown";
    switch (type) {
    case Wisenet::Device::SmartSearchResultType::motion :
        languageKey = "MotionDetection";
        break;
    case Wisenet::Device::SmartSearchResultType::enter :
        languageKey = "Entering";
        break;
    case Wisenet::Device::SmartSearchResultType::exit :
        languageKey = "Exiting";
        break;
    case Wisenet::Device::SmartSearchResultType::pass :
        languageKey = "Passing";
        break;
    default:
        break;
    }

    return languageKey;
}

QString convertEventType(Wisenet::Device::SmartSearchResultType type){
    QString typeKey = getTypeKey(type);

    QString eventName = QCoreApplication::translate("WisenetLinguist",typeKey.toStdString().c_str());

    if(0 == eventName.size())
        eventName = typeKey;

    return eventName;
}

void SmartSearchViewModel::reloadTableData()
{
    m_data.clear();
    m_index.clear();
    m_hoveredRow = -1;
    m_typeSet.clear();
    m_filterTypes.clear();

    if(m_smartSearchResult.contains(m_channelKey)) {
        int i =0;
        for(auto &item : m_smartSearchResult[m_channelKey]){
            QVector<QVariant> display;
            display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(item.dateTime));    // [0] dateTime
            display.push_back(convertEventType(item.type)); // [1] event type 번역 문구

            m_data.push_back(display);
            m_index.push_back(i++);

            // 결과에 포함 된 type을 set으로 구성
            if(item.type == Wisenet::Device::SmartSearchResultType::motion)
                m_typeSet.insert("MotionDetection");
            else if(item.type == Wisenet::Device::SmartSearchResultType::enter)
                m_typeSet.insert("Entering");
            else if(item.type == Wisenet::Device::SmartSearchResultType::exit)
                m_typeSet.insert("Exiting");
            else if(item.type == Wisenet::Device::SmartSearchResultType::pass)
                m_typeSet.insert("Passing");
        }
    }

    sort(0);

    SPDLOG_DEBUG("SmartSearchViewModel::reloadTableData() result={} data size={}",
                 m_smartSearchResult[m_channelKey].size(), m_data.size());

    emit totalCountChanged();
    emit resultUpdated(m_typeSet);
}

void SmartSearchViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    sortFilter();
}

void SmartSearchViewModel::UpdateSortData(int column, Qt::SortOrder order)
{
    m_sorted.clear();

    m_sorted.fill(0, m_data.size());
    std::iota(m_sorted.begin(), m_sorted.end(), 0);

    std::sort(m_sorted.begin(),m_sorted.end(),[this, order,column](int indexA, int indexB){
        const QVariant& valueA = m_data[indexA][column];
        const QVariant& valueB = m_data[indexB][column];

        if(Qt::SortOrder::AscendingOrder == order)
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) < 0;
        else
            return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;
    });
}

void SmartSearchViewModel::filter(const QStringList & types)
{
    m_filterTypes = types;
    sortFilter();
}

void SmartSearchViewModel::sortFilter()
{
    beginResetModel();

    if(m_filterTypes.count() == 0) {
        // 필터가 없는 경우
        m_index.reserve(m_sorted.size());
        m_index = m_sorted;
    }
    else {
        // 정렬 된 인덱스 리스트 (m_sorted)에 filter 적용하여 m_index에 할당
        QVector<int> result = QtConcurrent::blockingFiltered(m_sorted,[this](int n) {
            if(!m_smartSearchResult.contains(m_channelKey) || m_smartSearchResult[m_channelKey].size() <= n)
                return false;

            foreach(const QString &type , m_filterTypes){
                if(getTypeKey(m_smartSearchResult[m_channelKey][n].type) == type) {
                    return true;
                }
            }
            return false;
        });

        if(result.size() > 0){
            m_index.reserve(result.size());
            m_index = result;
        }else {
            m_index.clear();
        }
    }

    endResetModel();
}

int SmartSearchViewModel::totalCount() const
{
    SPDLOG_DEBUG("SmartSearchViewModel::totalCount() ={}", rowCount());
    return rowCount();
}

int SmartSearchViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int SmartSearchViewModel::columnCount(const QModelIndex &) const
{
    return 2;
}

QVariant SmartSearchViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    switch (role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    case EventImageRole:
    case DeviceTimeRole:
        return getDataFromSearchResults(index, (Role)role);
     default:
        break;
    }

    return QVariant();
}

void SmartSearchViewModel::setHoveredRow(int rowNum, bool hovered)
{
    QVector<int> role;
    role << Role::HoveredRole;

    int last = m_hoveredRow;
    if(hovered){
        m_hoveredRow = rowNum;
        if(last > m_hoveredRow)
            emit dataChanged(QModelIndex(index(m_hoveredRow, 0)), QModelIndex(index(last, columnCount() -1)), role);
        else
            emit dataChanged(QModelIndex(index(last, 0)), QModelIndex(index(m_hoveredRow, columnCount() -1)), role);
    }
    else{
        if(rowNum == m_hoveredRow){
            m_hoveredRow = -1;
            emit dataChanged(QModelIndex(index(rowNum, 0)), QModelIndex(index(rowNum, columnCount() -1)), role);
        }
    }
}

QHash<int, QByteArray> SmartSearchViewModel::roleNames() const
{
    return {
        {Qt::DisplayRole, "displayRole"},
        {HoveredRole, "hoveredRole"},
        {EventImageRole, "EventImageRole"},
        {DeviceTimeRole, "deviceTimeRole"},
    };
}

QVariant SmartSearchViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull()){
        return QVariant();
    }
    else{
        return cellValue;
    }
}

QVariant SmartSearchViewModel::getDataFromSearchResults(const QModelIndex &index, Role roleName) const
{
    int row = m_index[index.row()];

    if(!m_smartSearchResult.contains(m_channelKey) || m_smartSearchResult[m_channelKey].size() <= (size_t)row) {
        if(roleName == DeviceTimeRole)
            return 0;
        else
            return QVariant(QVariant::String);
    }

    switch (roleName) {
    case EventImageRole:
        return getTypeKey(m_smartSearchResult[m_channelKey][row].type);
    case DeviceTimeRole:
        return QVariant::fromValue(m_smartSearchResult[m_channelKey][row].dateTime);
    default:
        return QVariant(QVariant::String);
    }
}

void SmartSearchViewModel::clearRequest()
{
    m_areas.clear();
    m_lines.clear();
}

void SmartSearchViewModel::setSearchParam(qint64 fromDate, qint64 toDate, int overlappedID, QString channelId)
{
    m_startTime = fromDate;
    m_endTime = toDate;
    m_overlappedID = overlappedID;
    m_deviceChannelId = channelId;
}

void SmartSearchViewModel::addLine(int eventType, QList<float> coordinates)
{
    Wisenet::Device::SmartSearchLine line;

    line.index = (int)m_lines.size() + 1;
    line.eventType = (Wisenet::Device::SmartSearchLineEventType)eventType;

    for(auto coordinate : coordinates)
    {
        line.coordinates.push_back(coordinate);
    }

    m_lines.push_back(line);
}

void SmartSearchViewModel::addArea(int areaType, QList<float> coordinates, QList<int> eventTypes, QList<int> aiTypes)
{
    Wisenet::Device::SmartSearchArea area;

    area.index = (int)m_areas.size() + 1;
    area.areaType = (Wisenet::Device::SmartSearchAreaType)areaType;

    for(auto eventType : eventTypes)
    {
        area.eventType.push_back((Wisenet::Device::SmartSearchAreaEventType)eventType);
    }

    for(auto ai : aiTypes)
    {
        area.aiType.push_back((Wisenet::Device::SmartSearchAIType)ai);
    }

    for(auto coordinate : coordinates)
    {
        area.coordinates.push_back(coordinate);
    }

    m_areas.push_back(area);
}

void SmartSearchViewModel::searchRequest()
{
    // 2022.12.28. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    QStringList idList = m_deviceChannelId.split('_');

    if(idList.length() != 2)
        return;

    Wisenet::Device::Device::Channel channel;
    if (!db->FindChannel(idList[0], idList[1], channel))
        return;

    QString channelName = QString::fromUtf8(channel.channelName.c_str());

    auto deviceSmartSearchRequest = std::make_shared<Wisenet::Device::DeviceSmartSearchRequest>();

    deviceSmartSearchRequest->deviceID = idList[0].toStdString();
    deviceSmartSearchRequest->channel = idList[1].toStdString();
    deviceSmartSearchRequest->OverlappedID = m_overlappedID;
    deviceSmartSearchRequest->areas = m_areas;
    deviceSmartSearchRequest->lines = m_lines;
    deviceSmartSearchRequest->fromDate = m_startTime;
    deviceSmartSearchRequest->toDate = m_endTime;

    QString id = QString::fromStdString(deviceSmartSearchRequest->deviceID) + "_" +
                 QString::fromStdString(deviceSmartSearchRequest->channel) + "_" +
                 QString::number(deviceSmartSearchRequest->OverlappedID);

    m_smartSearchResult.remove(id);

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceSmartSearch,
                this, deviceSmartSearchRequest,
                [this, channelName, id, deviceSmartSearchRequest](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceSmartSearchResponse>(reply->responseDataPtr);
        if(!response || response->isFailed())
        {
            SPDLOG_INFO("DeviceSmartSearchRequest, isFailed={}", response->isFailed());
            std::vector<Wisenet::Device::SmartSearchResult> emptyResult;
            m_smartSearchResult[id] = emptyResult;
        }
        else
        {
            m_smartSearchResult[id] = response->results; // 결과 저장
        }

        selectChannel(id);  // 결과 Channel을 선택
        emit smartSearchFinished(); // 검색 완료 signal -> 결과 테이블 Open

        // 2022.12.28. coverity
        if (response == nullptr) {
            SPDLOG_INFO("emit smartSearchNoData(false)");
            emit smartSearchNoData(false);   // 검색 실패 signal
            return;
        }

        if (response->isFailed()){
            SPDLOG_INFO("emit smartSearchNoData(false)");
            emit smartSearchNoData(false);   // 검색 실패 signal
        }
        else if(m_smartSearchResult[id].size() == 0){
            emit smartSearchNoData(true);   // 검색 결과 없음 signal
            SPDLOG_INFO("emit smartSearchNoData(true)");
        }
    });
}

// 현재 선택 채널의 결과 리스트를 QVariantList 타입으로 변환하여 리턴하는 함수
QVariantList SmartSearchViewModel::getSelectedChannelResult()
{
    QVariantList resultTimeList;

    if(m_smartSearchResult.contains(m_channelKey))
    {
        auto& channelResult = m_smartSearchResult[m_channelKey];
        for(auto& result : channelResult) {
            QVariantList qResult;

            // 시간, type 순서로 리스트에 넣어 전달
            qResult.push_back(QVariant::fromValue(result.dateTime));
            qResult.push_back((int)result.type);

            resultTimeList.push_back(qResult);
        }
    }

    return resultTimeList;
}

// 모든 검색 결과 초기화 함수
void SmartSearchViewModel::clear()
{
    m_channelKey = "";
    m_smartSearchResult.clear();
    selectChannel("");
}

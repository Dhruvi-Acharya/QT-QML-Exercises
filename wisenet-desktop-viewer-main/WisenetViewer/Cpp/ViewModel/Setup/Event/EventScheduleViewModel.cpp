#include "EventScheduleViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/algorithm/string.hpp>

EventScheduleViewModel::EventScheduleViewModel(QObject *parent)
    :m_hoveredRow(-1),m_highlightRow(-1)
{
    Q_UNUSED(parent);
    m_columnNames << "ID" << "Check" << "Name" << "Table";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &EventScheduleViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

int EventScheduleViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int EventScheduleViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.count();
}

QVariant EventScheduleViewModel::data(const QModelIndex &index, int role) const
{
    //SPDLOG_DEBUG("EventScheduleViewModel::data index.row {} role {}", index.row(), role);

    if(index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HighlightRole:
        if(m_highlightRow == index.row())
            return true;
        return false;
    case HoveredRole:
        if(m_hoveredRow == index.row())
            return true;
        return false;
    default:
        break;
    }
    return QVariant();
}

QHash<int, QByteArray> EventScheduleViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"} };
}

void EventScheduleViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    beginResetModel();
    UpdateSortData(column, order);
    m_sorted.move(m_sorted.indexOf(m_alwaysIndex), 0);
    m_index = m_sorted;
    endResetModel();

    if (m_sortOrder != order) {
        //qDebug() << "EventScheduleViewModel::sort() after sort highlight row -> " << abs(m_highlightRow - m_data.size()) % m_data.size();
        setHighlightRow(abs(m_highlightRow - m_data.size()) % m_data.size());
        m_sortOrder = order;
    }
}

void EventScheduleViewModel::setHighlightRow(int rowNum)
{
    if(rowNum == m_highlightRow)
        return;

    beginResetModel();

    m_highlightRow = rowNum;
    setScheduleGuid(getScheduleId(m_highlightRow));
    setScheduleName(getScheduleName(m_highlightRow));
    setScheduleTable(getScheduleTable(m_highlightRow));

    QVector<int> role;
    role << Role::HighlightRole;
    emit dataChanged(QModelIndex(index(m_highlightRow, 0)), QModelIndex(index(m_highlightRow, columnCount() -1)), role);
    emit highlightRowChanged();

    endResetModel();

    //SPDLOG_DEBUG("EventScheduleViewModel::setHighlightRow -> highlight rowNum : {}", m_highlightRow);
}

void EventScheduleViewModel::setHoveredRow(int rowNum, bool hovered)
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

int EventScheduleViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVariant EventScheduleViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull()) {
        return QVariant();
    }
    else {
        return cellValue;
    }
}

void EventScheduleViewModel::reset() // 데이터가 추가되거나 삭제되는 경우 / scheduleView의 visible이 true가 될 때 실행됨
{
    // 2023.01.02. coverity
    int newIndex = 0;
    int previousSize = m_data.size();

    beginResetModel();

    m_data.clear();
    m_index.clear();
    m_hoveredRow = -1;

    // 2023.01.02. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    std::map<Wisenet::uuid_string, Wisenet::Core::EventSchedule> eventSchedules;
    if (db != nullptr) {
        eventSchedules = db->GetEventSchedules();
    }

    int i = 0;
    for(auto& iter : eventSchedules)
    {
        QVector<QVariant> display;
        display.push_back(QString::fromStdString(iter.first));
        display.push_back(false);
        display.push_back(QString::fromStdString(iter.second.scheduleName));
        display.push_back(QString::fromStdString(iter.second.schedule));

        if(QString::fromStdString(iter.second.scheduleName) == "Always")
            m_alwaysIndex = i;

        if(QString::fromStdString(iter.second.scheduleName) == m_scheduleName)
            newIndex = i;

        m_data.push_back(display);
        m_index.push_back(i++);
    }

    sort(EventScheduleName);

    int dataSize = m_data.size();

    if(dataSize >= previousSize) // 데이터가 추가되거나 편집된 경우
        emit exist(m_sorted.indexOf(newIndex));
    else // 데이터가 삭제된 경우
    {
        setScheduleGuid(getScheduleId(0));
        setScheduleName(getScheduleName(0));
        setScheduleTable(getScheduleTable(0));
        emit deleted();
    }

    endResetModel();
}

void EventScheduleViewModel::UpdateSortData(int column, Qt::SortOrder order)
{
    SPDLOG_DEBUG("[EventScheduleViewModel]UpdateSortData col:{} order:{}",column, order);
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

bool EventScheduleViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("[EventScheduleViewModel]setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];

    if(m_data[row][EventScheduleName] == "Always") {
        return false;
    }

    m_data[row][EventScheduleCheck] = !checked;
    dataChanged(QModelIndex(index(rowNum, EventScheduleCheck)), QModelIndex(index(rowNum, EventScheduleCheck)));
    emit selectedRowCountChanged();
    return true;
}

bool EventScheduleViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index) {
        if (m_data[item][EventScheduleName] == "Always")
            continue;
        m_data[item][EventScheduleCheck] = checked;
    }
    emit dataChanged(QModelIndex(index(0, EventScheduleCheck)), QModelIndex(index(m_index.length()-1, EventScheduleCheck)));
    emit selectedRowCountChanged();

    return true;
}


int EventScheduleViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[EventScheduleCheck].toBool() == true)
            ret++;
    }
    SPDLOG_DEBUG("[EventScheduleViewModel] selectedRowCount:{}",ret);

    return ret;
}

QString EventScheduleViewModel::getScheduleId(int rowNum)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_sorted.isEmpty())
        whatRow = m_sorted[whatRow];
    else
        SPDLOG_DEBUG("EventScheduleViewModel::getScheduleId failed rowNum{}", rowNum);

    QString scheduleId = m_data[whatRow][EventScheduleTitle::EventScheduleID].toString();

    return scheduleId;
}

QString EventScheduleViewModel::getScheduleName(int rowNum)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_sorted.isEmpty())
        whatRow = m_sorted[whatRow];
    else
        SPDLOG_DEBUG("EventScheduleViewModel::getScheduleName failed rowNum{}", rowNum);

    QString scheduleName = m_data[whatRow][EventScheduleTitle::EventScheduleName].toString();

    return scheduleName;
}

QString EventScheduleViewModel::getScheduleTable(int rowNum)
{
    if(rowNum < 0 || rowNum >= m_data.size())
        return "";

    int whatRow = rowNum;
    if(!m_sorted.isEmpty())
        whatRow = m_sorted[whatRow];
    else
        SPDLOG_DEBUG("EventScheduleViewModel::getScheduleTable failed rowNum{}", rowNum);

    QString scheduleTable = m_data[whatRow][EventScheduleTitle::EventScheduleTable].toString();

    return scheduleTable;
}


void EventScheduleViewModel::deleteSchedule()
{
    auto request = std::make_shared<Wisenet::Core::RemoveEventSchedulesRequest>(); // 포인터 하나 만들어서 request에 할당

    for (int dataIndex = 0; dataIndex < m_data.size(); dataIndex++) { // m_data에 있는 값들 하나씩 Bool값 체크해서 request에 값 넣어줌
        if(m_data[dataIndex][EventScheduleCheck].toBool()==false)
            continue;

        auto scheduleId = m_data[dataIndex][EventScheduleID].toString().toStdString();
        auto scheduleName = m_data[dataIndex][EventScheduleName].toString().toStdString();
        SPDLOG_DEBUG("EventScheduleViewModel::deleteSchedule id:{} {}", scheduleId, scheduleName);

        auto eventRules = QCoreServiceManager::Instance().DB()->GetEventRules();
        for(auto& iter : eventRules) {
            if(iter.second.scheduleID == scheduleId) {
                emit resultMessage(false, "Used Schedule");
                qDebug() << "EventScheduleViewModel::deleteSchedule failed (used by eventRule) name : " << QString::fromStdString(scheduleName);
                return;
            }
        }
        request->eventScheduleIDs.push_back(scheduleId);
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveEventSchedules,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);

        if(response->isSuccess()){
            emit resultMessage(true, "Success");
            SPDLOG_DEBUG("EventScheduleViewModel::deleteSchedule : SUCCESS");
        }
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));

        SPDLOG_DEBUG("EventScheduleViewModel::deleteSchedule response:{}", response->errorString());
    });
}

void EventScheduleViewModel::readSchedule(QString scheduleId)
{
    // 2023.01.02. coverity
    auto db = QCoreServiceManager::Instance().DB();
    if (db == nullptr) {
        return;
    }

    // 2023.01.02. coverity
    if (db != nullptr) {
        SPDLOG_DEBUG("EventScheduleViewModel::readSchedule ID: {}", scheduleId.toStdString());
        auto schedules = db->GetEventSchedules();
        auto schedule = schedules.find(scheduleId.toStdString());

        // edit으로 불리는 게 아니라 새로운 스케줄 추가로 불릴 떄
        if(schedule == schedules.end() || scheduleId=="") {
            setScheduleGuid("");
            setScheduleName("");
            setScheduleTable("");
            return;
        }

        SPDLOG_DEBUG("EventScheduleViewModel::readSchedule Name: {}", schedule->second.scheduleName);
        setScheduleGuid(QString::fromStdString(schedule->first));
        setScheduleName(QString::fromStdString(schedule->second.scheduleName));
        setScheduleTable(QString::fromStdString(schedule->second.schedule));
    }
}

void EventScheduleViewModel::saveSchedule()
{
    SPDLOG_DEBUG("EventScheduleViewModel::saveSchedule:{}", m_scheduleName.toStdString());

    auto request = std::make_shared<Wisenet::Core::SaveEventScheduleRequest>();
    auto schedules = QCoreServiceManager::Instance().DB()->GetEventSchedules(); // 중복되는 이름이면 추가되지 않도록 하기

    for(auto& schedule : schedules){ // 이미 존재하는 이름인지 체크
        if(schedule.second.scheduleName == m_scheduleName.toStdString() && schedule.first != m_scheduleGuid.toStdString()){
            emit resultMessage(false, "Exist schedule name");
            return;
        }
    }

    Wisenet::Core::EventSchedule schedule;
    schedule.scheduleID = m_scheduleGuid.toStdString();
    schedule.scheduleName = m_scheduleName.toStdString();
    schedule.schedule = m_scheduleTable.toStdString();

    request->eventSchedule = schedule;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveEventSchedule,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(response->isSuccess())
            emit resultMessage(true, "Success");
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));

        SPDLOG_DEBUG("EventScheduleViewModel::saveEventSchedule response:{}", response->errorString());
    });
}

void EventScheduleViewModel::addSchedule(QString scheduleName)
{
    SPDLOG_DEBUG("EventScheduleViewModel::addSchedule:{}", scheduleName.toStdString());

    // 2023.01.02. coverity
    auto uuid = boost::uuids::random_generator()();
    auto uuidStdString = boost::uuids::to_string(uuid);
    auto uuidQString = QString::fromStdString(uuidStdString);

    setScheduleGuid(uuidQString);
    setScheduleName(scheduleName);
    setScheduleTable(QString(8*24, '1'));

    saveSchedule();
}

void EventScheduleViewModel::editSchedule(int rowNum, QString scheduleTableInfo) // ScheduleTable.qml에서 받아온 scheduleTableInfo로 스케줄 데이터 편집
{
    SPDLOG_DEBUG("EventScheduleViewModel::editSchedule");

    setScheduleGuid(getScheduleId(rowNum));
    setScheduleName(getScheduleName(rowNum));
    setScheduleTable(scheduleTableInfo);

    saveSchedule();
}

void EventScheduleViewModel::setScheduleGuid(QString scheduleGuid)
{
    SPDLOG_DEBUG("EventScheduleViewModel::setScheduleGuid:{}", scheduleGuid.toStdString());
    if(scheduleGuid != m_scheduleGuid){
        m_scheduleGuid = scheduleGuid;
        emit this-> scheduleGuidChanged();
    }
}

void EventScheduleViewModel::setScheduleName(QString scheduleName)
{
    SPDLOG_DEBUG("EventScheduleViewModel::setScheduleName:{}", scheduleName.toStdString());
    if(scheduleName != m_scheduleName){
        m_scheduleName = scheduleName;
        emit this-> scheduleNameChanged();
    }
}

void EventScheduleViewModel::setScheduleTable(QString scheduleTable)
{
    SPDLOG_DEBUG("EventScheduleViewModel::setScheduleTable:{}", scheduleTable.toStdString());
    if(scheduleTable != m_scheduleTable){
        m_scheduleTable = scheduleTable;
        emit this-> scheduleTableChanged();
    }
}

void EventScheduleViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEventType = event->eventDataPtr->EventTypeId();

    switch(serviceEventType) {
    case Wisenet::Core::SaveEventScheduleEventType:
        reset();
        break;
    case Wisenet::Core::RemoveEventSchedulesEventType:
        reset();
        break;
    }
}

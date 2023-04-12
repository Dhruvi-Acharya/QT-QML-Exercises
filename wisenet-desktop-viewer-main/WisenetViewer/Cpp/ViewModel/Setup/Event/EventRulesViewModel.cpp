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
#include "EventRulesViewModel.h"
#include "QCoreServiceManager.h"
#include "LogSettings.h"

EventRulesViewModel::EventRulesViewModel(QObject* parent)
    :m_hoveredRow(-1), m_highlightRow(-1), m_lastHighlightRow(-1)
{
    Q_UNUSED(parent);
    m_columnNames << "ID" <<"Check"<<"Use" << "Name" << "Event trigger" << "Event action";

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &EventRulesViewModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

int EventRulesViewModel::rowCount(const QModelIndex &) const
{
    return m_index.size();
}

int EventRulesViewModel::columnCount(const QModelIndex &) const
{
    return m_columnNames.count();
}

QVariant EventRulesViewModel::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() > m_data.count())
        return QVariant();

    switch(role) {
    case Qt::DisplayRole:
        return getDataDisplayRole(index);
    case HighlightRole:
        if( m_highlightRows.count( index.row() ) )
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

QHash<int, QByteArray> EventRulesViewModel::roleNames() const
{
    return { {Qt::DisplayRole, "display"}, {HighlightRole, "highlightRole"}, {HoveredRole, "hoveredRole"} };
}

void EventRulesViewModel::sort(int column, Qt::SortOrder order)
{
    Q_UNUSED(column);
    UpdateSortData(column, order);
    beginResetModel();
    m_index = m_sorted;
    endResetModel();
}

void EventRulesViewModel::setHighlightRow(int rowNum, int modifiers)
{
    bool pressedCtrl = modifiers & Qt::ControlModifier;
    bool pressedShift = modifiers & Qt::ShiftModifier;

    bool fireRowChanged(false);
    int minRow(-1), maxRow(-1);

    if(m_highlightRows.count(rowNum)) {
        if(pressedCtrl || m_highlightRows.size() == 1) {
            //deselect it
            m_highlightRows.erase(rowNum);
            if(m_highlightRows.empty())
                m_highlightRow = -1;
            else
                m_highlightRow = *m_highlightRows.begin();
            fireRowChanged = true;
            minRow = rowNum; maxRow = rowNum;
            m_lastHighlightRow = -1;
        }
        else {
            minRow = *m_highlightRows.begin();
            maxRow = *m_highlightRows.rbegin();
            m_highlightRows.clear();
            m_highlightRows.insert(rowNum);
            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
        }
    }
    else {
        if(rowNum == -1) {
            //select none
            if(!m_highlightRows.empty()) {
                //clear selection
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
                m_highlightRows.clear();
                m_highlightRow = -1;
                m_lastHighlightRow = -1;
                fireRowChanged = true;
            }
        }
        else if(rowNum == -2) {
            //select all
            for(int irow=0; irow<m_index.size(); irow++){
                m_highlightRows.insert(irow);
            }
            fireRowChanged = true;
            m_highlightRow = m_index.size()-1;
            m_lastHighlightRow = m_index.size()-1;
            if(!m_highlightRows.empty()) {
                minRow = *m_highlightRows.begin();
                maxRow = *m_highlightRows.rbegin();
            }
        }
        else {
            if((!pressedCtrl && !pressedShift)) {
                if(!m_highlightRows.empty()) {
                    minRow = *m_highlightRows.begin();
                    maxRow = *m_highlightRows.rbegin();
                    m_highlightRows.clear();
                }
            }
            if(!pressedShift || m_lastHighlightRow == -1){
                m_highlightRows.insert(rowNum);
            }
            else {
                int fromRow = m_lastHighlightRow;
                int toRow = rowNum;
                if(fromRow > toRow) std::swap(fromRow, toRow);
                minRow = fromRow;
                maxRow = toRow;
                while(fromRow <= toRow) {
                    m_highlightRows.insert(fromRow);
                    fromRow++;
                }
            }

            m_highlightRow = rowNum;
            m_lastHighlightRow = rowNum;
            fireRowChanged = true;
            if(minRow < 0 || rowNum < minRow) minRow = rowNum;
            if(maxRow < 0 || rowNum > maxRow) maxRow = rowNum;
        }
    }

    if(fireRowChanged) {
        emit highlightRowChanged();
        emit highlightRowCountChanged();
        emit selectedRowCountChanged();

        if(minRow != -1) {
            QVector<int> role;
            role << Role::HighlightRole;
            emit dataChanged(QModelIndex(index(minRow, 0)), QModelIndex(index(maxRow, columnCount() -1)), role);
        }
    }
}

void EventRulesViewModel::setHoveredRow(int rowNum, bool hovered)
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

int EventRulesViewModel::selectedRowCount(){
    int ret = 0;
    for (auto& item: m_data) {
        if(item[EventRuleCheck].toBool() == true)
            ret++;
    }
    return ret;
}

int EventRulesViewModel::highlightRow() const
{
    return m_highlightRow;
}

QVariant EventRulesViewModel::getDataDisplayRole(const QModelIndex &index) const
{
    int row = m_index[index.row()];

    const QVariant& cellValue = m_data[row][index.column()];

    if(cellValue.isNull())
        return QVariant();

    return cellValue;
}

void EventRulesViewModel::reset()
{
    m_data.clear();
    m_index.clear();
    m_highlightRows.clear();
    m_highlightRow = -1;
    m_lastHighlightRow = -1;
    m_hoveredRow = -1;

    auto eventRules = QCoreServiceManager::Instance().DB()->GetEventRules();
    auto eventSchedules = QCoreServiceManager::Instance().DB()->GetEventSchedules();

    int i = 0;

    std::set<std::string> titleEvent;

    for(auto& iter : eventRules){
//        auto schedule = eventSchedules.find(iter.second.scheduleID);
//        if(schedule == eventSchedules.end())
//            continue;

        QVector<QVariant> display;
        display.push_back(QString::fromStdString(iter.first));
        display.push_back(false);
        display.push_back(iter.second.isEnabled?true:false);
        display.push_back(QString::fromStdString(iter.second.name));


        std::set<std::string> eventSet;
        for(auto & eventTrigger : iter.second.events){
                eventSet.emplace(eventTrigger.type);
        }
        QStringList vEventList;
        std::string strEvent;
        for(auto & item : eventSet){
            strEvent += (item + ",");
            vEventList.push_back(QString::fromStdString(item));

        }
        SPDLOG_DEBUG("[EventRulesViewModel] reset:{}", strEvent);
        if(strEvent.length() > 0)
            strEvent.erase(strEvent.length() -1);

       display.push_back(vEventList);

        //display.push_back(QString::fromStdString(strEvent));

        std::string strAction = {};

        QStringList vEventActionList;

        if(iter.second.actions.alarmOutputAction.useAction){
            strAction += "Alarm out,";
            vEventActionList.push_back("Alarm output");
        }
        if(iter.second.actions.alertAlarmAction.useAction){
            strAction += "AlertAlarm,";
            vEventActionList.push_back("AlertAlarm");
        }
        if(iter.second.actions.layoutChangeAction.useAction){
            strAction += "Open layout,";
            vEventActionList.push_back("Open layout");
        }
        if (iter.second.actions.emailAction.useAction){
            strAction += "Email,";
            vEventActionList.push_back("E-mail");
        }

        if(strAction.size() > 0)
            strAction.erase(strAction.size()-1);
        display.push_back(vEventActionList);
        display.push_back(QString::fromStdString(iter.second.scheduleID));
        //display.push_back(QString::fromStdString(strAction));

        m_data.push_back(display);
        m_index.push_back(i++);
    }

    SPDLOG_DEBUG("EventRulesViewModel::reset() end {}", eventRules.size());

    sort(EventRuleName);
}

void EventRulesViewModel::UpdateSortData(int column, Qt::SortOrder order)
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

bool EventRulesViewModel::setCheckState(int rowNum, bool checked)
{
    SPDLOG_DEBUG("setCheckState row:{} checked:{}",rowNum, checked);

    int row = m_index[rowNum];
    m_data[row][EventRuleCheck] = !checked;
    emit dataChanged(QModelIndex(index(rowNum, EventRuleCheck)), QModelIndex(index(rowNum, EventRuleCheck)));
    emit selectedRowCountChanged();
    return true;
}

bool EventRulesViewModel::setAllCheckState(bool checked)
{
    SPDLOG_DEBUG("setAllCheckState checked:{}",checked);

    for(auto& item : m_index)
        m_data[item][EventRuleCheck] = checked;
    emit dataChanged(QModelIndex(index(0, EventRuleCheck)), QModelIndex(index(m_index.length()-1, EventRuleCheck)));
    emit selectedRowCountChanged();
    return true;
}

bool EventRulesViewModel::setCellValue(int rowNum, int columnNum, const QVariant &data)
{
   if(rowNum < 0 || rowNum >= m_data.size())
        return false;
    int whatRow = rowNum;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];

    beginResetModel();
    m_data[whatRow][columnNum] = data.toString();
    endResetModel();

    return true;
}

QString EventRulesViewModel::getRuleId(int rowNum)
{
   if(rowNum < 0 || rowNum >= m_data.size())
        return "";
    int whatRow = rowNum;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];

    QString ruleId = m_data[whatRow][EventRuleTitle::EventRuleID].toString();

    return ruleId;
}

void EventRulesViewModel::deleteRule()
{
    SPDLOG_DEBUG("EventRulesViewModel::deleteRule count:{}", m_highlightRows.size());

    auto request = std::make_shared<Wisenet::Core::RemoveEventRulesRequest>();

    for (int dataIndex = 0; dataIndex < m_data.size();dataIndex++) {
        if(m_data[dataIndex][EventRuleCheck].toBool() == false)
            continue;
        beginResetModel();
        auto ruleId = m_data[dataIndex][EventRuleID].toString().toStdString();
        auto ruleName = m_data[dataIndex][EventRuleName].toString().toStdString();
        auto scheduleId = m_data[dataIndex][EventScheduleID].toString().toStdString();
        SPDLOG_DEBUG("EventRulesViewModel::deleteRule id:{} {}", ruleId, ruleName);
        request->eventRuleIDs.push_back(ruleId);
        request->eventScheduleIDs.push_back(scheduleId);
    }

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::RemoveEventRules,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        if(response->isSuccess()){
            reset();
            emit resultMessage(true, "Success");
        }
        else
            emit resultMessage(false, QString::fromStdString(response->errorString()));
        qDebug() << "AddEventRuleViewModel::removeRule() response:" << QString::fromStdString(response->errorString());
    });
}

void EventRulesViewModel::editRule(int row, int col, bool checked)
{
    SPDLOG_DEBUG("EventRulesViewModel::editRule row:{} col:{} item:{}", row, col, checked);

    if(row < 0 || row >= m_data.size())
        return;

    int whatRow = row;
    if(!m_index.isEmpty())
        whatRow = m_index[whatRow];
    auto ruleId = m_data[whatRow][EventRuleTitle::EventRuleID] ;

    auto request = std::make_shared<Wisenet::Core::SaveEventRuleRequest>();
    auto eventRules = QCoreServiceManager::Instance().DB()->GetEventRules();


    auto rule = eventRules.find(ruleId.toString().toStdString());

    if(rule == eventRules.end()){
        return;
    }

    request->eventRule = rule->second;
    request->eventRule.isEnabled = checked;

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::SaveEventRule,
                this, request,
                [=](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::ResponseBase>(reply->responseDataPtr);
        qDebug() << "EventRulesViewModel::editRule() response:" << QString::fromStdString(response->errorString());

    });

}


void EventRulesViewModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    //SPDLOG_INFO("AddEventRuleViewModel coreServiceEventTriggered!! EventType={}", serviceEvent->EventType());

    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::SaveEventRuleEventType:
    case Wisenet::Core::RemoveEventRulesEventType:
        reset();
    }
}


#include "EventListFilterProxyModel.h"
#include "EventListModel.h"

EventListFilterProxyModel::EventListFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    qDebug() << "EventListFilterProxyModel()";
    //sort(0, Qt::SortOrder::DescendingOrder);
}

EventListFilterProxyModel::~EventListFilterProxyModel()
{
    qDebug() << "~EventListFilterProxyModel()";
}

bool EventListFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    auto sourceNameData = sourceModel()->data(sourceIndex, EventListModel::ModelRoles::SourceNameRole);
    QString sourceName = sourceNameData.value<QString>();

    auto alarmNameData = sourceModel()->data(sourceIndex, EventListModel::ModelRoles::AlarmNameRole);
    QString alarmName = alarmNameData.value<QString>();

    if(!sourceName.contains(filterRegExp()) && !alarmName.contains(filterRegExp()))
        return false;

    if(!m_cameraFilterEnabled && !m_eventTypeFilterEnabled)
        return true;

    if(m_cameraFilterEnabled)
    {
        auto idData = sourceModel()->data(sourceIndex, EventListModel::ModelRoles::SourceIdRole);
        QString itemId = idData.value<QString>();

        if(!m_channelList.contains(itemId))
            return false;
    }

    if(m_eventTypeFilterEnabled)
    {
        auto typeData = sourceModel()->data(sourceIndex, EventListModel::ModelRoles::AlarmTypeRole);
        QString alarmType = typeData.value<QString>();

        if(!m_eventTypeFilter.contains(alarmType))
            return false;
    }

    return true;
}

bool EventListFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftAlarmType = sourceModel()->data(left, (int)EventListModel::ModelRoles::IsPriorityAlarmRole);
    QVariant rightAlarmType = sourceModel()->data(right,  (int)EventListModel::ModelRoles::IsPriorityAlarmRole);

    bool isLeftPriorityAlarm = leftAlarmType.toBool();
    bool isRightPriorityAlarm = rightAlarmType.toBool();

    if(isLeftPriorityAlarm && !isRightPriorityAlarm)
    {
        return false;
    }

    if(!isLeftPriorityAlarm && isRightPriorityAlarm)
    {
        return true;
    }

    QVariant leftTimeData = sourceModel()->data(left, (int)EventListModel::ModelRoles::UtcTimeRole);
    QVariant rightTimeData = sourceModel()->data(right,  (int)EventListModel::ModelRoles::UtcTimeRole);

    long long leftTime = leftTimeData.toLongLong();
    long long rightTime = rightTimeData.toLongLong();

    bool ret = leftTime < rightTime;

    return ret;
}

void EventListFilterProxyModel::setChannelList(QStringList itemIds)
{
    m_channelList.clear();

    foreach(const QString& itemId, itemIds) {
        if(!m_channelList.contains(itemId))
            m_channelList.insert(itemId);
    }

    invalidateFilter();
}

void EventListFilterProxyModel::setEventTypeFilter(QStringList eventList)
{
    m_eventTypeFilter.clear();

    foreach(const QString& eventType, eventList) {
        if(!m_eventTypeFilter.contains(eventType))
            m_eventTypeFilter.insert(eventType);
    }

    invalidateFilter();
}

void EventListFilterProxyModel::setFilterText(const QString& filterText)
{
    m_filterText = filterText;

    setFilterRegExp(QRegExp(filterText, Qt::CaseInsensitive, QRegExp::FixedString));
    invalidateFilter();
}

void EventListFilterProxyModel::updateFilter()
{
    invalidateFilter();
}

bool EventListFilterProxyModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    qDebug() << "EventListFilterProxyModel::setData()" << idx << value << role;
    QModelIndex sourceIndex = mapToSource(idx);
    return sourceModel()->setData(sourceIndex, value, role);;
}

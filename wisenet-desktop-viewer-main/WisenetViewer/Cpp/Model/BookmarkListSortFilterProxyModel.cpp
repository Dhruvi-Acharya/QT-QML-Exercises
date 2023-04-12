#include "BookmarkListSortFilterProxyModel.h"
#include "BookmarkListModel.h"

BookmarkListSortFilterProxyModel::BookmarkListSortFilterProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    qDebug() << "BookmarkListSortFilterProxyModel()";
    m_startTime = 0;
    m_endTime = 0;
    sort(0, Qt::SortOrder::DescendingOrder);
}

BookmarkListSortFilterProxyModel::~BookmarkListSortFilterProxyModel()
{
    qDebug() << "~BookmarkListSortFilterProxyModel()";
}

bool BookmarkListSortFilterProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    QVariant leftData = sourceModel()->data(left, (int)BookmarkListModel::ModelRoles::UtcStartTimeRole);
    QVariant rightData = sourceModel()->data(right,  (int)BookmarkListModel::ModelRoles::UtcStartTimeRole);

    long long leftTime = leftData.toLongLong();
    long long rightTime = rightData.toLongLong();

    bool ret = leftTime < rightTime;

    return ret;
}

bool BookmarkListSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);

    auto bookMarkNameData = sourceModel()->data(sourceIndex, BookmarkListModel::ModelRoles::BookmarkNameRole);
    QString bookMarkName = bookMarkNameData.value<QString>();

    auto cameraNameData = sourceModel()->data(sourceIndex, BookmarkListModel::ModelRoles::CameraNameRole);
    QString cameraName = cameraNameData.value<QString>();

    auto descriptionData = sourceModel()->data(sourceIndex, BookmarkListModel::ModelRoles::DescriptionRole);
    QString description = descriptionData.value<QString>();

    if(!bookMarkName.contains(filterRegExp()) && !cameraName.contains(filterRegExp()) && !description.contains(filterRegExp()))
        return false;

    if(!m_cameraFilterEnabled && !m_datetimeFilterEnabled)
        return true;

    if(m_cameraFilterEnabled)
    {
        auto idData = sourceModel()->data(sourceIndex, BookmarkListModel::ModelRoles::SourceIdRole);
        QString itemId = idData.value<QString>();

        if(!m_channelList.contains(itemId))
            return false;
    }

    if(m_datetimeFilterEnabled)
    {
        auto typeData = sourceModel()->data(sourceIndex, BookmarkListModel::ModelRoles::UtcStartTimeRole);
        long long startTime = typeData.toLongLong();

        if(startTime < m_startTime)
            return false;

        if(m_endTime != 0 && m_endTime < startTime)
            return false;
    }

    return true;
}

void BookmarkListSortFilterProxyModel::setChannelList(QStringList itemIds)
{
    m_channelList.clear();

    foreach(const QString& itemId, itemIds) {
        if(!m_channelList.contains(itemId))
            m_channelList.insert(itemId);
    }

    invalidateFilter();
}

void BookmarkListSortFilterProxyModel::setUtcFilter(long long startTime, long long endTime)
{
    m_startTime = startTime;
    m_endTime = endTime;

    invalidateFilter();
}

void BookmarkListSortFilterProxyModel::setFilterText(const QString& filterText)
{
    m_filterText = filterText;

    setFilterRegExp(QRegExp(filterText, Qt::CaseInsensitive, QRegExp::FixedString));
    invalidateFilter();
}

void BookmarkListSortFilterProxyModel::updateFilter()
{
    invalidateFilter();
}

#include "SequenceListModel.h"

SequenceListModel::SequenceListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    m_roleNames[ButtonDisplayRole] = "buttonDisplay";
    m_roleNames[SelectedLayoutRole] = "selectedLayout";
    m_roleNames[IntervalRole] = "interval";
    m_roleNames[RowRole] = "row";

    //SequenceItem item;
    //append(item);
}

SequenceListModel::~SequenceListModel()
{
    qDebug() << "~SequenceListModel()";
    this->clear();
}

int SequenceListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QVariant SequenceListModel::data(const QModelIndex &idx, int role) const
{
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
        return QVariant();

    SequenceItem* item = m_data.at(row);

    switch(role)
    {
    case ButtonDisplayRole:
    {
        if(idx.row() == m_data.count() - 1)
            return true;
        else
            return false;
    }
    case SelectedLayoutRole:
        return item->layoutIndex;
    case IntervalRole:
        return item->interval;
    case RowRole:
        return idx.row();
    }

    return QVariant();
}

bool SequenceListModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    qDebug() << "SequenceListModel::setData ";
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
        return false;

    SequenceItem* item = m_data.at(row);

    QVector<int> roles;

    switch(role)
    {
    case SelectedLayoutRole:
    {
        item->layoutIndex = value.toInt();

        roles << RoleNames::SelectedLayoutRole;
        emit dataChanged(idx, idx, roles);
        return true;
    }
    case IntervalRole:
    {
        item->interval = value.toInt();

        roles << RoleNames::IntervalRole;
        emit dataChanged(idx, idx, roles);
        return true;
    }
    default:
        break;
    }

    return false;
}

void SequenceListModel::swapData(int firstIdx, int secondIdx)
{
    qDebug() << "SequenceListModel::swapData()"<<firstIdx<<"<->"<<secondIdx;
    QVector<int> roles;
    roles << RoleNames::SelectedLayoutRole;
    roles << RoleNames::IntervalRole;

    SequenceItem* firstItem = m_data.at(firstIdx);
    SequenceItem* secondItem = m_data.at(secondIdx);

//    SequenceItem tmpItem;
//    tmpItem.interval = firstItem->interval;
//    tmpItem.layoutIndex = firstItem->layoutIndex;

//    firstItem->interval = secondItem->interval;
//    firstItem->layoutIndex = secondItem->layoutIndex;

//    secondItem->interval = tmpItem.interval;
//    secondItem->layoutIndex = tmpItem.layoutIndex;

//    beginMoveRows(QModelIndex(index(firstIdx, 0)), firstIdx, firstIdx, QModelIndex(index(secondIdx, 0)), secondIdx);
//    m_data.move(firstIdx, secondIdx);
//    endMoveRows();

    SequenceItem* tmpItem = new SequenceItem();
    tmpItem->interval = firstItem->interval;
    tmpItem->layoutIndex = firstItem->layoutIndex;

    beginRemoveRows(QModelIndex(), firstIdx, firstIdx);
    delete m_data.takeAt(firstIdx);
    endRemoveRows();

    beginInsertRows(QModelIndex(), secondIdx, secondIdx);
    m_data.insert(secondIdx, tmpItem);
    endInsertRows();

    emit dataCountChanged(m_data.count());
    emit dataChanged(QModelIndex(index(firstIdx, 0)), QModelIndex(index(secondIdx, 0)), roles);
}

void SequenceListModel::insert(int idx, SequenceItem* item)
{
    if(idx < 0 || idx > m_data.count())
    {
        return;
    }

    beginInsertRows(QModelIndex(), idx, idx);
    m_data.insert(idx, item);
    endInsertRows();

    QVector<int> role;
    role << ButtonDisplayRole;
    role << SelectedLayoutRole;
    role << IntervalRole;
    emit dataCountChanged(m_data.count());
    emit dataChanged(QModelIndex(), QModelIndex(index(m_data.count()-1, 0)), role);
}

void SequenceListModel::append(SequenceItem* item)
{
    insert(m_data.count(), item);

    QVector<int> role;
    role << ButtonDisplayRole;
    role << SelectedLayoutRole;
    role << IntervalRole;
    emit dataCountChanged(m_data.count());
    emit dataChanged(QModelIndex(), QModelIndex(index(m_data.count()-1, 0)), role);
}

void SequenceListModel::remove(int idx)
{
    if(idx < 0 || idx > m_data.count() - 1)
    {
        return;
    }

    beginRemoveRows(QModelIndex(), idx, idx);
    delete m_data.takeAt(idx);
    endRemoveRows();

    QVector<int> role;
    role << ButtonDisplayRole;
    emit dataCountChanged(m_data.count());
    emit dataChanged(QModelIndex(), QModelIndex(index(m_data.count()-1, 0)), role);
}

void SequenceListModel::clear()
{
    if(m_data.count() == 0)
        return;

    beginResetModel();
    qDeleteAll(m_data);
    m_data.clear();
    endResetModel();
    emit dataCountChanged(m_data.count());
}

QList<SequenceItem*> SequenceListModel::getData()
{
    return m_data;
}

QHash<int, QByteArray> SequenceListModel::roleNames() const
{
    return m_roleNames;
}

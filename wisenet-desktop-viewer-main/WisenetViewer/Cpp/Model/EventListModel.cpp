#include "EventListModel.h"

EventListModel::EventListModel(QObject *parent)
    : QAbstractListModel(parent)
{
    Q_UNUSED(parent);
    setAlertAlarmCount(0);
    qDebug() << "EventListModel::EventListModel()";
}

EventListModel::~EventListModel()
{
    qDebug() << "EventListModel::~EventListModel()";
    qDeleteAll(m_data);
    m_data.clear();
}

int EventListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_data.size();
}

void EventListModel::setMaxEventCount(int count)
{
    qDebug() << "EventListModel::setMaxEventCount()" << count;
    m_maxEventCount = count;
    if (m_maxEventCount > 0) {
        m_data.reserve(m_maxEventCount * 2);
    }
}

void EventListModel::setAlertAlarmCount(int alertAlarmCount)
{
    m_alertAlarmCount = alertAlarmCount;
    emit alertAlarmCountChanged(alertAlarmCount);
}

void EventListModel::applySaveChannels(QString id, QString name)
{
    for(int i=0; i<m_data.size(); i++)
    {
        if(m_data[i]->sourceId() == id)
        {
            m_data[i]->setSourcelName(name);

            QVector<int> role;
            role << SourceNameRole;

            QModelIndex idx = index(i, 0, QModelIndex());
            emit dataChanged(idx, idx, role);
        }
    }
}

QHash<int, QByteArray> EventListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[SourceIdRole] = "sourceId";
    roles[SourceNameRole] = "sourceName";
    roles[AlarmTypeRole] = "alarmType";
    roles[AlarmNameRole] = "alarmName";
    roles[OccurrenceTimeRole] = "occurrenceTime";
    roles[UtcTimeRole] = "utcTime";
    roles[DeviceTimeRole] = "deviceTime";
    roles[IsPriorityAlarmRole] = "isPriorityAlarm";
    roles[PlayedRole] = "played";
    return roles;
}

QVariant EventListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case SourceIdRole:
        value = m_data[index.row()]->sourceId();
        break;
    case SourceNameRole:
        value = m_data[index.row()]->sourceName();
        break;
    case AlarmTypeRole:
        value = m_data[index.row()]->alarmType();
        break;
    case AlarmNameRole:
        value = m_data[index.row()]->alarmName();
        break;
    case OccurrenceTimeRole:
        value = m_data[index.row()]->occurrenceTime();
        break;
    case UtcTimeRole:
        value = m_data[index.row()]->utcTime();
        break;
    case DeviceTimeRole:
        value = m_data[index.row()]->deviceTime();
        break;
    case IsPriorityAlarmRole:
        value = m_data[index.row()]->isPriorityAlarm();
        break;
    case PlayedRole:
        value = m_data[index.row()]->played();
        break;
    default:
        break;
    }

    return value;
}

bool EventListModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    qDebug() << "EventListModel::setData()" << idx << value << role;
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
        return false;

    EventModel* item = m_data.at(row);

    switch(role)
    {
    case PlayedRole:
    {
        qDebug() << "EventListModel::setData PlayedRole " << value.toString();
        item->setPlayed(value.toBool());

        // Delete mark
        QVector<int> role;
        role << PlayedRole;
        emit dataChanged(idx, idx, role);

        // Move
        moveAlarm(idx);

        return true;
    }
    default:
        break;
    }

    return false;
}

void EventListModel::insertFirst(EventModel* model)
{
    // Delete
    if(m_maxEventCount != 0 && m_data.count() >= m_maxEventCount)
    {
        beginRemoveRows(QModelIndex(), m_data.count()-1, m_data.count()-1);
        EventModel* modelToDelete = m_data.takeLast();

        if(modelToDelete->isPriorityAlarm())
            setAlertAlarmCount(m_alertAlarmCount-1);

        delete modelToDelete;
        endRemoveRows();
    }

    if(model->isPriorityAlarm())
    {
        setAlertAlarmCount(m_alertAlarmCount+1);
        beginInsertRows(QModelIndex(), 0, 0);
        this->m_data.insert(0, model);
        endInsertRows();
    }
    else
    {
        if(m_alertAlarmCount >= m_maxEventCount)
        {
            qDebug() << "EventListModel::insertFirst() max priority alarm " << m_alertAlarmCount << m_maxEventCount ;
            delete model;
        }
        else
        {
            beginInsertRows(QModelIndex(), m_alertAlarmCount, m_alertAlarmCount);
            this->m_data.insert(m_alertAlarmCount, model);
            endInsertRows();
        }
    }

    emit countChanged();
}

void EventListModel::append(EventModel* model)
{
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.append(model);
    endInsertRows();

    emit countChanged();
}

void EventListModel::removeAt(int position)
{
    beginRemoveRows(QModelIndex(), position, position);
    delete this->m_data.takeAt(position);
    endRemoveRows();

    emit countChanged();
}

void EventListModel::removeAll()
{
    beginResetModel();

    qDeleteAll(m_data);
    m_data.clear();

    endResetModel();

    setAlertAlarmCount(0);


    emit countChanged();
}

// 만료된 장비 패스워드 연장 요청
void EventListModel::extendDevicePassword(int position)
{
    auto request = std::make_shared<Wisenet::Device::DeviceRequestBase>();
    request->deviceID = this->m_data.takeAt(position)->sourceId().toStdString();

    QCoreServiceManager::Instance().RequestToCoreService(
                &Wisenet::Core::ICoreService::DeviceExtendPasswordExpiration,
                this, request,
                [this, request](const QCoreServiceReplyPtr& reply)
    {
        auto response = std::static_pointer_cast<Wisenet::Device::DeviceGetPasswordExpiryResponse>(reply->responseDataPtr);

        SPDLOG_DEBUG("EventListModel::extendDevicePassword() - DeviceExtendPasswordExpiration - isSuccess:{}", response->isSuccess());

        if(response->isSuccess())
        {
        }
    });
}

void EventListModel::moveAlarm(const QModelIndex &idx)
{
    int row = idx.row();

    qDebug() << "EventListModel::moveAlarm() count" << m_data.count() << row;

    if(m_data.count() == 1 || m_data.count()-1 == row)
    {
        qDebug() << "EventListModel::moveAlarm() count 1 or row==end return" ;
        return;
    }

    EventModel* item = m_data.at(row);

    int indexToMove = -1;

    for(int i=m_alertAlarmCount; i<m_data.count(); i++)
    {
        EventModel* model = m_data.at(i);

        if(model->utcTime() < item->utcTime())
        {
            indexToMove = i;
            break;
        }
    }

    qDebug() << "EventListModel::moveAlarm() indexToMove" << indexToMove;

    if(indexToMove <= 1)
    {
        setAlertAlarmCount(m_alertAlarmCount-1);
        return;
    }

    if(indexToMove == -1)
    {
        beginMoveRows(QModelIndex(), row, row, QModelIndex(), m_data.count());
        EventModel* taked = m_data.takeAt(row);
        m_data.insert(m_data.count(), taked);
        endMoveRows();
    }
    else
    {
        beginMoveRows(QModelIndex(), row, row, QModelIndex(), indexToMove);
        EventModel* taked = m_data.takeAt(row);
        m_data.insert(indexToMove-1, taked);
        endMoveRows();
    }

    setAlertAlarmCount(m_alertAlarmCount-1);
}

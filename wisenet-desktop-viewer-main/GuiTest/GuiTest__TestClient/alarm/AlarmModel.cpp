#include "AlarmModel.h"
#include "LogSettings.h"
#include "BaseStructure.h"

AlarmModel::AlarmModel(QObject* parent)
{
    Q_UNUSED(parent);

    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &AlarmModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

int AlarmModel::rowCount(const QModelIndex& p) const
{
    Q_UNUSED(p)
    return m_data.size();
}

QHash<int, QByteArray> AlarmModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[ColorRole] = "colorCode";
    roles[ChannelNameRole] = "channel";
    roles[TimeRole] = "time";
    roles[AlarmTypeRole] = "alarmName";
    return roles;
}

QVariant AlarmModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role)

    QVariant value;

    switch (role)
    {
    case ColorRole:
        value = m_data[index.row()]->property("colorCode");
        break;
    case ChannelNameRole:
        value = m_data[index.row()]->property("channel");
        break;
    case TimeRole:
        value = m_data[index.row()]->property("time");
        break;
    case AlarmTypeRole:
        value = m_data[index.row()]->property("alarmName");
        break;
    default:
        break;
    }

    return value;
}

int AlarmModel::count() const
{
    return this->m_data.count();
}

void AlarmModel::append(QObject* o)
{
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.push_back(o);
    endInsertRows();
}

void AlarmModel::insert(QObject* o, int i)
{
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.insert(i, o);
    endInsertRows();
}

void AlarmModel::remove(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    delete this->m_data.takeAt(idx);
    endRemoveRows();
}

void AlarmModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    SPDLOG_INFO("AlarmModel get CoreServiceEvent!! EventType={}", serviceEvent->EventType());

    if (serviceEvent->EventTypeId() == Wisenet::Device::DeviceEventType::DeviceAlarmEventType)
    {
        auto alarmEvent = std::static_pointer_cast<Wisenet::Device::DeviceAlarmEvent>(event->eventDataPtr);

        QObject* item = new QObject();
        item->setProperty("colorCode", "white"); //GetAlarmColor(alarmEvent->alarmType));
        //item->setProperty("channel", QString::fromUtf8(alarmEvent->displayName.c_str()));
        //item->setProperty("time", QString::fromUtf8(alarmEvent->timeStamp.c_str()));
        item->setProperty("alarmName", QString(alarmEvent->type.c_str()) + " : " + (alarmEvent->data ? "Start" : "End"));

        this->insert(item, 0);
    }
}

//QString AlarmModel::GetAlarmColor(Wisenet::AlarmEventType::Type alarmType)
//{
//    QString color = "";
//
//    if(Wisenet::AlarmEventType::Type::DeviceDisconnectedByService <= alarmType && Wisenet::AlarmEventType::Type::OverwriteDecoding >= alarmType)
//        color = "white";
//    else if(Wisenet::AlarmEventType::Type::MotionDetection <= alarmType && Wisenet::AlarmEventType::Type::BodyTemperatureDetection >= alarmType)
//        color = "#7979df";
//    else if(Wisenet::AlarmEventType::Type::NetworkAlarmInput == alarmType || Wisenet::AlarmEventType::Type::DeviceAlarmInput == alarmType)
//        color = "#fdb77d";
//    return color;
//}
//
//QString AlarmModel::GetAlarmString(Wisenet::AlarmEventType::Type alarmType)
//{
//    QString alarmString = Wisenet::AlarmEventType::ToString(alarmType).c_str();
//
//
//    return alarmString;
//}

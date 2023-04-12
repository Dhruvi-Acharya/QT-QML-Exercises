#include "EventLogThread.h"
#include "LogSettings.h"
#include "QLocaleManager.h"

EventLogThread::EventLogThread(QObject *parent)
    :QThread(parent)
{

}
EventLogThread::~EventLogThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();
}

void EventLogThread::process(std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> deletedDevices,
                             std::shared_ptr<std::vector<Wisenet::Core::EventLog> > eventLogs)
{
    QMutexLocker locker(&m_mutex);

    this->m_eventLogs = eventLogs;
    this->m_deletedDevices = deletedDevices;

    if(!isRunning()){
        start(LowPriority);
    } else {
        m_restart = true;
        m_condition.wakeOne();
    }

}

void EventLogThread::registerQml()
{
    qRegisterMetaType<std::shared_ptr<std::vector<Wisenet::Core::EventLog>>>();
    qRegisterMetaType<QSharedPointer<QVector<QVector<QVariant>>>>();
    qRegisterMetaType<QSharedPointer<QVector<int>>>();
    qRegisterMetaType<QVector<QStringList>>();
    qRegisterMetaType<QSet<QString>>();
}

void EventLogThread::run()
{
    forever{

        m_mutex.lock();
        auto eventLogs = this->m_eventLogs;
        this->m_eventLogs.reset();
        m_mutex.unlock();

        QSharedPointer<QVector<QVector<QVariant>>> data(new QVector<QVector<QVariant>>);
        QSharedPointer<QVector<int>> dateTimeDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> cameraDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> eventDescIndex(new QVector<int>);

        QVector<QStringList> cameraList;
        QSet<QString> eventList;

        QSet<QString> cameraListFilter;


        if(m_abort){
            return;
        }

        // bookmark - event mapping
        std::map<Wisenet::uuid_string, Wisenet::Core::Bookmark> bookmarkMap = QCoreServiceManager::Instance().DB()->GetBookmarks();
        std::map<std::string, Wisenet::uuid_string> eventBookmarkMap;   // key:eventLogID, bookmarkID:
        auto iter = bookmarkMap.begin();
        while (iter != bookmarkMap.end()) {
            if(iter->second.eventLogID != "") {
                eventBookmarkMap[iter->second.eventLogID] = iter->first;
            }
            iter++;
        }

        if(eventLogs && !eventLogs->empty()){
            //1. build data for display
            for(auto& eventLog : *eventLogs){

                QString deviceID(QString::fromUtf8(eventLog.deviceID.c_str()));
                QString channelID(QString::fromUtf8(eventLog.channelID.c_str()));

                QString deletedDeviceName = GetDeletedDeviceName(eventLog.deviceID);
                QString deletedChannelName = GetDeletedChannelName(eventLog.deviceID,eventLog.channelID);

                QString id = deviceID + QString::fromUtf8("_") + channelID;
                if(cameraListFilter.end() == cameraListFilter.find(id)){
                    cameraListFilter.insert(id);
                    QStringList channel;
                    channel << deviceID << channelID << deletedDeviceName << deletedChannelName;
                    cameraList.push_back(channel);
                }

                QString eventID(QString::fromUtf8(eventLog.type.c_str()));
                if(eventList.end() == eventList.find(eventID)){
                    eventList.insert(eventID);
                }

                QVector<QVariant> display;
                //0.bookmarkID
                auto iter = eventBookmarkMap.find(eventLog.eventLogID);
                if(iter != eventBookmarkMap.end())
                    display.push_back(QString::fromUtf8(iter->second.c_str()));
                else
                    display.push_back("");

                //1.dateTime
                display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(eventLog.serviceUtcTimeMsec));

                //2.channelName
                Wisenet::Device::Device::Channel channel;

                bool isDeletedChannel = !QCoreServiceManager::Instance().DB()->FindChannel(eventLog.deviceID,eventLog.channelID,channel);
                if(!isDeletedChannel){
                    //SPDLOG_DEBUG("Start to build ChannelName deviceID = {}, channelID = {}, channelName = {}",eventLog.deviceID,eventLog.channelID, channel.name);
                    display.push_back(QString::fromUtf8(channel.name.c_str()));
                }else{
                    //SPDLOG_DEBUG("Not found ChannelName deviceID = {}, channelID = {}, eventLogChannelName = {}",eventLog.deviceID,eventLog.channelID, eventLog.channelName);
                    //display.push_back(QString::fromUtf8(eventLog.channelName.c_str()));
                    display.push_back(deletedChannelName);
                }

                //3.Event Name
                display.push_back(getEventName(eventLog.type));

                //4.Description
                display.push_back(getDescription(eventLog.parameters));

                //5.IsDeletedDevice
                if(isDeletedChannel)
                    display.push_back(true);
                else
                    display.push_back(false);

                data->push_back(display);
            }

            // sort of dateTime
            dateTimeDescIndex->fill(0,data->size());
            std::iota(dateTimeDescIndex->begin(), dateTimeDescIndex->end(), 0);


            // sort of cameraName
            cameraDescIndex->fill(0,data->size());
            std::iota(cameraDescIndex->begin(), cameraDescIndex->end(), 0);
            std::sort(cameraDescIndex->begin(),cameraDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][2];
                const QVariant& valueB = (*data)[indexB][2];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

            // sort of eventName
            eventDescIndex->fill(0,data->size());
            std::iota(eventDescIndex->begin(), eventDescIndex->end(), 0);
            std::sort(eventDescIndex->begin(),eventDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][3];
                const QVariant& valueB = (*data)[indexB][3];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

        }

        SPDLOG_DEBUG("Emit processCompleted");
        emit processCompleted(eventLogs,data,dateTimeDescIndex,cameraDescIndex,eventDescIndex,cameraList,eventList);

        m_mutex.lock();
        if(!m_restart){
            m_condition.wait(&m_mutex);
        }
        m_restart = false;
        m_mutex.unlock();
    }

}

QString EventLogThread::getEventName(std::string &type)
{
    QString eventName("");

    eventName = QCoreApplication::translate("WisenetLinguist",type.c_str());

    if(0 == eventName.size()){
        eventName = QString::fromUtf8(type.c_str());
    }

    return eventName;
}

QString EventLogThread::getDescription(Wisenet::Core::EventLog::Parameters &parameters)
{
    QString description("");

    unsigned int i = 0;
    for(auto& source : parameters.source){

        description += QString::fromUtf8(source.first.c_str());
        description += "=";
        description += QString::fromUtf8(source.second.c_str());
        i++;
        if(i < parameters.source.size())
            description += " , ";
    }

    if(description.size() > 0 && parameters.data.size() > 0){
        description += " , ";
    }

    i = 0;
    for(auto& data : parameters.data){

        description += QString::fromUtf8(data.first.c_str());
        description += "=";
        description += QString::fromUtf8(data.second.c_str());
        i++;
        if(i < parameters.data.size())
            description += " , ";
    }

    return description;
}

QString EventLogThread::GetDeletedDeviceName(std::string &deviceID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    name = itor->second.name.c_str();
    name += " [" + deletedItem + "]";
    return name;
}

QString EventLogThread::GetDeletedChannelName(std::string &deviceID, std::string &channelID)
{
    QString name;
    QString deletedItem = QCoreApplication::translate("WisenetLinguist","Deleted");

    if(!m_deletedDevices || m_deletedDevices->empty()){
        name = deletedItem;
        return name;
    }

    auto itor = m_deletedDevices->find(deviceID);
    if(itor == m_deletedDevices->end()){
        name = deletedItem;
        return name;
    }

    auto itorChannel = itor->second.channels.find(channelID);
    if(itorChannel == itor->second.channels.end()){
        name = deletedItem;
        return name;
    }

    name = itorChannel->second.name.c_str();
    name += " [" + deletedItem + "]";

    return name;
}

#include "SystemLogThread.h"
#include "LogSettings.h"
#include "QLocaleManager.h"

SystemLogThread::SystemLogThread(QObject *parent)
    :QThread(parent)
{

}
SystemLogThread::~SystemLogThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();
}

void SystemLogThread::process(std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> deletedDevices,
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

void SystemLogThread::run()
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

        if(eventLogs && !eventLogs->empty()){

            //1. build data for display
            for(auto& eventLog : *eventLogs){

                QString deviceID(QString::fromUtf8(eventLog.deviceID.c_str()));
                QString channelID(QString::fromUtf8(eventLog.channelID.c_str()));

                QString deletedDeviceName = GetDeletedDeviceName(eventLog.deviceID);
                QString deletedChannelName = GetDeletedChannelName(eventLog.deviceID,eventLog.channelID);

                QString id = deviceID;
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
                //1.dateTime
                display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(eventLog.serviceUtcTimeMsec));

                //2.Device Name
                Wisenet::Device::Device device;

                // 2022.12.29. coverity
                auto db = QCoreServiceManager::Instance().DB();                
                if (db == nullptr) {
                    return;
                }

                if (db != nullptr) {
                    bool found = false;
                    found = db->FindDevice(eventLog.deviceID,device);
                    if (found) {
                        //SPDLOG_DEBUG("Start to build ChannelName deviceID = {}, channelID = {}, channelName = {}",eventLog.deviceID,eventLog.channelID, channel.name);
                        display.push_back(QString::fromUtf8(device.name.c_str()));
                    }
                    else if(eventLog.isService) {
                        display.push_back(QCoreApplication::translate("WisenetLinguist","Service"));
                    }
                    else {
                        //SPDLOG_DEBUG("Not found ChannelName deviceID = {}, channelID = {}, eventLogChannelName = {}",eventLog.deviceID,eventLog.channelID, eventLog.channelName);
                        //display.push_back(QString::fromUtf8(eventLog.deviceName.c_str()));
                        display.push_back(deletedDeviceName);
                    }
                }

                //3.Event Name
                display.push_back(getEventName(eventLog.type));

                //4.Description
                display.push_back(getDescription(eventLog.type, eventLog.parameters));

                data->push_back(display);
            }

            // sort of dateTime
            dateTimeDescIndex->fill(0,data->size());
            std::iota(dateTimeDescIndex->begin(), dateTimeDescIndex->end(), 0);


            // sort of cameraName
            cameraDescIndex->fill(0,data->size());
            std::iota(cameraDescIndex->begin(), cameraDescIndex->end(), 0);
            std::sort(cameraDescIndex->begin(),cameraDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][1];
                const QVariant& valueB = (*data)[indexB][1];

                return valueA.toString().compare(valueB.toString(), Qt::CaseInsensitive) > 0;

            });

            // sort of eventName
            eventDescIndex->fill(0,data->size());
            std::iota(eventDescIndex->begin(), eventDescIndex->end(), 0);
            std::sort(eventDescIndex->begin(),eventDescIndex->end(),[data](int indexA, int indexB){

                const QVariant& valueA = (*data)[indexA][2];
                const QVariant& valueB = (*data)[indexB][2];

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

QString SystemLogThread::getEventName(std::string &type)
{
    QString eventName("");

    eventName = QCoreApplication::translate("WisenetLinguist",type.c_str());

    if(0 == eventName.size()){
        eventName = QString::fromUtf8(type.c_str());
    }

    return eventName;
}

QString SystemLogThread::getDescription(const std::string& type, const Wisenet::Core::EventLog::Parameters &parameters)
{
    if(0 == type.compare(Wisenet::FixedAlarmType::FanError)){

        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","Fan %1").arg(source.second.c_str());
            }
        }

    }else if((0 == type.compare(Wisenet::FixedAlarmType::HDDFull))
             ||(0 == type.compare(Wisenet::FixedAlarmType::HDDNone))
             ||(0 == type.compare(Wisenet::FixedAlarmType::HDDFail))
             ||(0 == type.compare(Wisenet::FixedAlarmType::HDDError))){

        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","HDD %1").arg(source.second.c_str());
            }
        }

    }else if((0 == type.compare(Wisenet::FixedAlarmType::SDFull))
             ||(0 == type.compare(Wisenet::FixedAlarmType::SDFail))){

        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","SD card %1").arg(source.second.c_str());;
            }else if(0 == source.first.compare("ChannelID")){
                return QCoreApplication::translate("WisenetLinguist","Channel %1").arg(source.second.c_str());
            }
        }

    }else if((0 == type.compare(Wisenet::FixedAlarmType::NASFull))
             ||(0 == type.compare(Wisenet::FixedAlarmType::NASFail))
             ||(0 == type.compare(Wisenet::FixedAlarmType::NASDisconnect))){

        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","NAS %1").arg(source.second.c_str());
            }
        }

    }else if((0 == type.compare(Wisenet::FixedAlarmType::RAIDDegrade))
             ||(0 == type.compare(Wisenet::FixedAlarmType::RAIDRebuildStart))
             ||(0 == type.compare(Wisenet::FixedAlarmType::RAIDFail))){

        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","RAID Array %1").arg(source.second.c_str());
            }
        }

    }else if(0 == type.compare(Wisenet::FixedAlarmType::DeviceAlarmInput)){
        for(auto& source : parameters.source){
            if(0 == source.first.compare("SystemID")){
                return QCoreApplication::translate("WisenetLinguist","Alarm input %1").arg(source.second.c_str());
            }
        }
    }

    else if(0 == type.compare(Wisenet::FixedAlarmType::IFrameMode)){
        for(auto& source : parameters.data){
            if(0 == source.first.compare("CPU")){
                return ("CPU : " + QString::fromStdString(source.second) + "%");
            }
        }
    }

    else if(0 == type.compare(Wisenet::AdditionalLogType::DeviceDisconnectedByService)){
        return QCoreApplication::translate("WisenetLinguist","SystemEvent.CoreService.DeviceDisconnectedByService.Message");
    }else if(0 == type.compare(Wisenet::AdditionalLogType::DeviceDisconnectedWithError)){
        return QCoreApplication::translate("WisenetLinguist","SystemEvent.CoreService.DeviceDisconnectedWithError");
    }else if(0 == type.compare(Wisenet::AdditionalLogType::DeviceDisconnectedWithUnauthorized)){
        return QCoreApplication::translate("WisenetLinguist","SystemEvent.CoreService.DeviceDisconnectedWithUnauthorized.Message");
    }else if(0 == type.compare(Wisenet::AdditionalLogType::DeviceDisconnectedWithRestriction)){
        return QCoreApplication::translate("WisenetLinguist","SystemEvent.CoreService.DeviceDisconnectedWithRestriction.Message");
    }

    return QString("");
}

QString SystemLogThread::GetDeletedDeviceName(std::string &deviceID)
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

QString SystemLogThread::GetDeletedChannelName(std::string &deviceID, std::string &channelID)
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


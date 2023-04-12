#include "GridListThread.h"
#include "LogSettings.h"
#include "QLocaleManager.h"

GridListThread::GridListThread(QObject *parent)
    :QThread(parent)
{

}
GridListThread::~GridListThread()
{
    m_mutex.lock();
    m_abort = true;
    m_condition.wakeOne();
    m_mutex.unlock();

    wait();
}

void GridListThread::process(std::vector<Wisenet::Device::MetaAttribute>* metaAttrs)
{
    //    qDebug() <<"GridListThread::process()";
    QMutexLocker locker(&m_mutex);

    this->m_metaAttrs = metaAttrs;

    if(!isRunning()){
        start(LowPriority);
    } else {
        m_restart = true;
        m_condition.wakeOne();
    }

}

void GridListThread::registerQml()
{
    qRegisterMetaType<std::shared_ptr<std::vector<Wisenet::Device::MetaAttribute>>>();\
}

void GridListThread::run()
{
    // qDebug() <<"GridListThread::run()";
    forever{

        m_mutex.lock();
        auto metaAttrs = this->m_metaAttrs;
        m_mutex.unlock();

        QSharedPointer<QVector<QVector<QVariant>>> data(new QVector<QVector<QVariant>>);
        QSharedPointer<QVector<int>> dateTimeDescIndex(new QVector<int>);
        QSharedPointer<QVector<int>> cameraDescIndex(new QVector<int>);

        QVector<QStringList> cameraList;
        QSet<QString> metaAttrSet;

        QSet<QString> cameraListFilter;


        if(m_abort){
            return;
        }

        if(metaAttrs && !metaAttrs->empty()){

            //1. build data for display
            for(auto& metaAttr : *metaAttrs){
                QString deviceID(QString::fromUtf8(metaAttr.aiBaseAttribute.deviceID.c_str()));
                QString channelID(QString::number(metaAttr.aiBaseAttribute.channelId));

                QString deletedDeviceName = GetDeletedDeviceName(metaAttr.aiBaseAttribute.deviceID);
                QString deletedChannelName = GetDeletedChannelName(metaAttr.aiBaseAttribute.deviceID, std::to_string(metaAttr.aiBaseAttribute.channelId));

                QString id = deviceID + QString::fromUtf8("_") + channelID;
                if(cameraListFilter.end() == cameraListFilter.find(id)){
                    cameraListFilter.insert(id);
                    QStringList channel;
                    channel << deviceID << channelID << deletedDeviceName << deletedChannelName;
                    cameraList.push_back(channel);
                }

                QString objectID(QString::number(metaAttr.objectId));
                if(metaAttrSet.end() == metaAttrSet.find(objectID)){
                    metaAttrSet.insert(objectID);
                }

                QVector<QVariant> display;
                //0.bookmarkID
                display.push_back("");

                //1.dateTime
                display.push_back(QLocaleManager::Instance()->getDateTimeFromMsec(metaAttr.aiBaseAttribute.dateTime));

                //2.channelName
                Wisenet::Device::Device::Channel channel;

                if(QCoreServiceManager::Instance().DB()->FindChannel(deviceID,channelID,channel)){
                    //SPDLOG_DEBUG("Start to build ChannelName deviceID = {}, channelID = {}, channelName = {}",metaAttr.deviceID,metaAttr.channelID, channel.name);
                    display.push_back(QString::fromUtf8(channel.name.c_str()));
                }

                //3.BestShot
                display.push_back(getBestShotCheck(metaAttr.aiBaseAttribute.imageUrl));

                //4.Attribute
                display.push_back(getAttribute(metaAttr));

                //5.ImageUrl
                QString url(QString::fromUtf8(metaAttr.aiBaseAttribute.imageUrl.c_str()));
                display.push_back(url);

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
        }

        SPDLOG_DEBUG("Emit processCompleted");
        emit processCompleted(metaAttrs,data,dateTimeDescIndex,cameraDescIndex,cameraList,metaAttrSet);

        m_mutex.lock();
        if(!m_restart){
            m_condition.wait(&m_mutex);
        }
        m_restart = false;
        m_mutex.unlock();
    }

}

QString GridListThread::getBestShotCheck(std::string imageUrl)
{
    // qDebug() <<"GridListThread::getBestShotCheck()";

    QString bestShotCheckStr = QString::fromUtf8("O");

    if(imageUrl.empty())
    {
        bestShotCheckStr = QString::fromUtf8("X");
    }

    return bestShotCheckStr;
}

QString GridListThread::getAttribute(Wisenet::Device::MetaAttribute& metaAttr)
{
    QString metaAttrStr = "";

    if(metaAttr.attributeType == Wisenet::Device::AiClassType::person)
    {
        // person
        if(metaAttr.personGender.size() > 0)
        {
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.personGender[0].c_str());
            metaAttrStr += ", ";
        }

        if(metaAttr.personClothingTopsColor.size() > 0)
        {
            QString top = "%1 (%2)";
            metaAttrStr += top.arg(QCoreApplication::translate("WisenetLinguist", metaAttr.personClothingTopsColor[0].c_str())).arg(QCoreApplication::translate("WisenetLinguist", "Top"));
            metaAttrStr += ", ";
        }

        if(metaAttr.personClothingTopsColor.size() > 1)
        {
            QString top = "%1 (%2)";
            metaAttrStr += top.arg(QCoreApplication::translate("WisenetLinguist", metaAttr.personClothingTopsColor[1].c_str())).arg(QCoreApplication::translate("WisenetLinguist", "Top"));
            metaAttrStr += ", ";
        }

        if(metaAttr.personClothingBottomsColor.size() > 0)
        {
            QString bottom = "%1 (%2)";
            metaAttrStr += bottom.arg(QCoreApplication::translate("WisenetLinguist", metaAttr.personClothingBottomsColor[0].c_str())).arg(QCoreApplication::translate("WisenetLinguist", "Bottom"));
            metaAttrStr += ", ";
        }

        if(metaAttr.personClothingBottomsColor.size() > 1)
        {
            QString bottom = "%1 (%2)";
            metaAttrStr += bottom.arg(QCoreApplication::translate("WisenetLinguist", metaAttr.personClothingBottomsColor[1].c_str())).arg(QCoreApplication::translate("WisenetLinguist", "Bottom"));
            metaAttrStr += ", ";
        }

        if(metaAttr.personBelongingBag.size() > 0)
        {
            if(metaAttr.personBelongingBag[0] == "Wear")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "Bag");

            else if(metaAttr.personBelongingBag[0] == "No")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "No bag");
        }
    }
    else if(metaAttr.attributeType == Wisenet::Device::AiClassType::face)
    {
        // face
        if(metaAttr.faceGender.size() > 0)
        {
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.faceGender[0].c_str());
            metaAttrStr += ", ";
        }

        if(metaAttr.faceAgeType.size() > 0)
        {
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.faceAgeType[0].c_str());
            metaAttrStr += ", ";
        }

        if(metaAttr.faceOpticals.size() > 0)
        {
            if(metaAttr.faceOpticals[0] == "Wear")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "Glasses");

            else if(metaAttr.faceOpticals[0] == "No")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "No glasses");

            metaAttrStr += ", ";
        }

        if(metaAttr.faceMask.size() > 0)
        {
            if(metaAttr.faceMask[0] == "Wear")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "Mask");

            else if(metaAttr.faceMask[0] == "No")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "No mask");
        }
    }
    else if(metaAttr.attributeType == Wisenet::Device::AiClassType::vehicle)
    {
        // vehicle
        if(metaAttr.vehicleType.size() > 0)
        {
            if(metaAttr.vehicleType[0] == "Car")
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", "Car (Sedan/SUV/Van)");
            else
                metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.vehicleType[0].c_str());

            metaAttrStr += ", ";
        }

        if(metaAttr.vehicleColor.size() == 1)
        {
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.vehicleColor[0].c_str());
        }
        else if(metaAttr.vehicleColor.size() == 2)
        {
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.vehicleColor[0].c_str());
            metaAttrStr += ", ";
            metaAttrStr += QCoreApplication::translate("WisenetLinguist", metaAttr.vehicleColor[1].c_str());
        }
    }
    else if(metaAttr.attributeType == Wisenet::Device::AiClassType::ocr)
    {
        metaAttrStr = QString::fromStdString(metaAttr.ocrText);
    }

    return metaAttrStr;
}

QString GridListThread::GetDeletedDeviceName(std::string &deviceID)
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

QString GridListThread::GetDeletedChannelName(std::string &deviceID, std::string channelID)
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

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
#include "EventFilterTreeSourceModel.h"
#include <QUuid>
#include <QDebug>

#include "QCoreServiceManager.h"

EventFilterTreeSourceModel::EventFilterTreeSourceModel(QObject *parent)
    :BaseTreeModel(parent)
{
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::MotionDetection.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::FaceDetection.c_str());
    //m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::Videoloss.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideolossStart.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideolossEnd.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::Tampering.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AudioDetection.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Passing.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Intrusion.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Entering.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Exiting.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Appearing.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Disappearing.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Loitering.c_str());
    //m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::Tracking.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::TrackingStart.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::TrackingEnd.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::DefocusDetection.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::FogDetection.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Scream.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Gunshot.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Explosion.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_GlassBreak.c_str());
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::NetworkAlarmInput.c_str());

    m_roleNameMapping[TreeModelRoleName] = "display";

    BaseTreeItemInfo* rootInfo = newTreeItemInfo(QUuid::createUuid().toString(QUuid::WithoutBraces), ItemType::MainRoot, ItemStatus::Normal, tr("Root"));
    rootItem = new TreeItemModel(rootInfo); 
}

EventFilterTreeSourceModel::~EventFilterTreeSourceModel()
{
    removeAll();
}

Qt::ItemFlags EventFilterTreeSourceModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    Qt::ItemFlags flags = QAbstractItemModel::flags(index);

    TreeItemModel *item = static_cast<TreeItemModel*>(index.internalPointer());

    if(item->childCount() > 0){

        flags ^= Qt::ItemIsSelectable;
        return flags;
    }

    return flags;
}

QModelIndex EventFilterTreeSourceModel::getIndexByValue(QString event)
{
    if(!m_treeItemModelMap.contains(event)){
        return QModelIndex();
    }

    TreeItemModel * item = m_treeItemModelMap[event];

    QModelIndex index = getIndex(item);

    return index;
}

void EventFilterTreeSourceModel::connectCoreService()
{
    connect(&QCoreServiceManager::Instance(), &QCoreServiceManager::coreServiceEventTriggered,
            this, &EventFilterTreeSourceModel::coreServiceEventTriggered, Qt::QueuedConnection);
}

void EventFilterTreeSourceModel::setAlertAlarm()
{
    m_fixedEvents << QString::fromUtf8(Wisenet::FixedAlarmType::AlertAlarm.c_str());
}
void EventFilterTreeSourceModel::setAllEventData()
{
    QSet<QString> events = m_fixedEvents;
    QSet<QString> dynamicList = QCoreServiceManager::Instance().DB()->GetDynamicEventList();

    foreach(QString dynamic , dynamicList){
        events << dynamic;
    }

    qDebug() << "EventFilterTreeSourceModel setAllEventData: " << events;
    setEventData(events);
}

void EventFilterTreeSourceModel::setEventData(QSet<QString> events)
{    
    removeAll();
    addEventData(events);
}

void EventFilterTreeSourceModel::coreServiceEventTriggered(QCoreServiceEventPtr event)
{
    auto serviceEvent = event->eventDataPtr;
    switch(serviceEvent->EventTypeId()){
    case Wisenet::Core::FullInfoEventType:
    case Wisenet::Core::AddDeviceEventType:
    case Wisenet::Core::RemoveDevicesEventType:
        updateAllEventData();
        break;
    }
}

void EventFilterTreeSourceModel::updateAllEventData()
{
    //추가된 이벤트 찾아서 추가.
    QSet<QString> addedEvents;

    foreach(QString fixed, m_fixedEvents){
        if(m_treeItemModelMap.end() == m_treeItemModelMap.find(fixed)){
            addedEvents << fixed;
        }
    }
    QSet<QString> dynamicList = QCoreServiceManager::Instance().DB()->GetDynamicEventList();

    foreach(QString dynamic , dynamicList){
        if(m_treeItemModelMap.end() == m_treeItemModelMap.find(dynamic)){
            addedEvents << dynamic;
        }
    }

    qDebug() << "EventFilterTreeSourceModel addedEvent: " << addedEvents;

    addEventData(addedEvents);

    //삭제된 이벤트 찾아서 삭제.
    QSet<QString> allEvents = m_fixedEvents;
    foreach(QString dynamic , dynamicList){
        allEvents << dynamic;
    }

    QSet<QString> removedEvents;
    foreach(TreeItemModel * item , m_treeItemModelMap){
        if(allEvents.end() == allEvents.find(item->getUuid())){
            if(ItemType::Event == item->getItemType()){
                removedEvents << item->getUuid();
            }
        }
    }

    qDebug() << "EventFilterTreeSourceModel removedEvent: " << removedEvents;
    removeEventData(removedEvents);

}

BaseTreeItemInfo *EventFilterTreeSourceModel::newTreeItemInfo(const QString& uuid, const ItemType &type, const ItemStatus &status, const QString &name)
{
    BaseTreeItemInfo *t = new BaseTreeItemInfo(this);
    t->setUuid(uuid);
    t->setType(type);
    t->setItemStatus(status);

    QString eventName = QCoreApplication::translate("WisenetLinguist", name.toStdString().c_str());

    if(0 == eventName.size()){
        eventName = name;
    }

    t->setDisplayName(eventName);

    return t;
}

TreeItemModel * EventFilterTreeSourceModel::AddChildend(TreeItemModel *parent, const ItemType& type, const QString &name)
{
    beginInsertRows(getIndex(parent),parent->childCount(),parent->childCount());
    BaseTreeItemInfo* baseItem = newTreeItemInfo(name, type, ItemStatus::Normal,name);
    TreeItemModel * model = new TreeItemModel(baseItem, parent);
    parent->appendChild(model);
    m_treeItemModelMap.insert(name,model);
    endInsertRows();

    return model;
}

void EventFilterTreeSourceModel::removeEventData(QSet<QString> &events)
{
    foreach(QString event, events){

        if(!m_treeItemModelMap.contains(event)){
            continue;
        }

        //아이템 삭제
        TreeItemModel* item = m_treeItemModelMap.take(event);
        TreeItemModel* parent = item->parentItem();

        beginRemoveRows(getIndex(parent),item->row(),item->row());
        m_treeItemModelMap.remove(item->getUuid());
        parent->removeChild(item->row());
        endRemoveRows();

        //parent가 MainRoot가 아닌 경우 child가 없는 상태가 되면 삭제.
        if((ItemType::MainRoot != parent->getItemType())
                &&(0 == parent->childCount())){

            item = parent;
            parent = item->parentItem();

            beginRemoveRows(getIndex(parent),item->row(),item->row());
            m_treeItemModelMap.remove(item->getUuid());
            parent->removeChild(item->row());
            endRemoveRows();
        }
    }
}

void EventFilterTreeSourceModel::addEventData(QSet<QString> &events)
{
    //순서
    //AlertAlarm
    //MotionDetection
    //VideoAnalytics (VideoAnalytics.Passing, VideoAnalytics.Intrusion, VideoAnalytics.Entering, VideoAnalytics.Exiting, VideoAnalytics.Appearing, VideoAnalytics.Disappearing, Tampering, VideoAnalytics.Loitering)
    //FaceDetection
    //DefocusDetection
    //FogDetection
    //AudioDetection
    //AudioAnalytics(AudioAnalytics.Scream, AudioAnalytics.Gunshot, AudioAnalytics.Explosion, AudioAnalytics.GlassBreak)
    //FaceRecognition
    //Tracking (Start, End)
    //ObjectDetection(Person, Face, Vehicle, License plate)
    //Videoloss (Start, End)
    //NetworkAlarmInput
    //Dynamic (.....)

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AlertAlarm.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AlertAlarm.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AlertAlarm.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::MotionDetection.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::MotionDetection.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::MotionDetection.c_str()));
    }

    if((events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Passing.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Intrusion.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Entering.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Exiting.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Appearing.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Disappearing.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Loitering.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::Tampering.c_str())))){

        TreeItemModel * parentModel = AddChildend(rootItem, ItemType::EventGroup, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics.c_str()));

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Passing.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Passing.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Passing.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Intrusion.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Intrusion.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Intrusion.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Entering.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Entering.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Entering.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Exiting.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Exiting.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Exiting.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Appearing.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Appearing.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Appearing.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Disappearing.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Disappearing.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Disappearing.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::Tampering.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::Tampering.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::Tampering.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Loitering.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Loitering.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideoAnalytics_Loitering.c_str()));
        }
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::FaceDetection.c_str()))){

        AddChildend(rootItem,ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::FaceDetection.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::FaceDetection.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::DefocusDetection.c_str()))){

        AddChildend(rootItem,ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::DefocusDetection.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::DefocusDetection.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::FogDetection.c_str()))){

        AddChildend(rootItem,ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::FogDetection.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::FogDetection.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioDetection.c_str()))){

        AddChildend(rootItem,ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AudioDetection.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioDetection.c_str()));
    }

    if((events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Scream.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Gunshot.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Explosion.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_GlassBreak.c_str())))){

        TreeItemModel * parentModel = AddChildend(rootItem, ItemType::EventGroup, QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics.c_str()));

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Scream.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Scream.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Scream.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Gunshot.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Gunshot.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Gunshot.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Explosion.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Explosion.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_Explosion.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_GlassBreak.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_GlassBreak.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::AudioAnalytics_GlassBreak.c_str()));
        }
    }

    if((events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingStart.c_str())))
            || (events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingEnd.c_str())))){

        TreeItemModel * parentModel = AddChildend(rootItem, ItemType::EventGroup, QString::fromUtf8(Wisenet::FixedAlarmType::Tracking.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::Tracking.c_str()));

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingStart.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::TrackingStart.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingStart.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingEnd.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::TrackingEnd.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::TrackingEnd.c_str()));
        }

    }

    if((events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossStart.c_str())))
            ||(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossEnd.c_str())))){

        TreeItemModel * parentModel = AddChildend(rootItem, ItemType::EventGroup, QString::fromUtf8(Wisenet::FixedAlarmType::Videoloss.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::Videoloss.c_str()));

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossStart.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideolossStart.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossStart.c_str()));
        }

        if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossEnd.c_str()))){

            AddChildend(parentModel, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VideolossEnd.c_str()));
            events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VideolossEnd.c_str()));
        }
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NetworkAlarmInput.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NetworkAlarmInput.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NetworkAlarmInput.c_str()));
    }

    //System Event.
    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::DualSMPSFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::DualSMPSFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::DualSMPSFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::FanError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::FanError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::FanError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::CPUFanError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::CPUFanError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::CPUFanError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::FrameFanError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::FrameFanError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::FrameFanError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::LeftFanError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::LeftFanError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::LeftFanError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RightFanError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RightFanError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RightFanError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::HDDFull.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::HDDFull.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::HDDFull.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::SDFull.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::SDFull.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::SDFull.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFull.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFull.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFull.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NASFull.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NASFull.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NASFull.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::HDDNone.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::HDDNone.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::HDDNone.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::HDDFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::HDDFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::HDDFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::SDFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::SDFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::SDFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NASFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NASFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NASFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelSDFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::HDDError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::HDDError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::HDDError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDDegrade.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RAIDDegrade.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDDegrade.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDRebuildStart.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RAIDRebuildStart.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDRebuildStart.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDFail.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RAIDFail.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RAIDFail.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::iSCSIDisconnect.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::iSCSIDisconnect.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::iSCSIDisconnect.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NASDisconnect.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NASDisconnect.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NASDisconnect.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RecordFiltering.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RecordFiltering.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RecordFiltering.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::RecordingError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::RecordingError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::RecordingError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::CpuOverload.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::CpuOverload.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::CpuOverload.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NetTxTrafficOverflow.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NetTxTrafficOverflow.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NetTxTrafficOverflow.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NetCamTrafficOverFlow.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NetCamTrafficOverFlow.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NetCamTrafficOverFlow.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::VPUError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::VPUError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::VPUError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::MemoryError.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::MemoryError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::MemoryError.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::LowFps.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::LowFps.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::LowFps.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelLowFps.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::ChannelLowFps.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::ChannelLowFps.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::FixedAlarmType::NewFWAvailable.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::FixedAlarmType::NewFWAvailable.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::NewFWAvailable.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceConnected.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AdditionalLogType::DeviceConnected.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceConnected.c_str()));
    }

    if ((events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedByService.c_str())))
            || events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithError.c_str()))
            || events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithRestriction.c_str()))
            || events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithUnauthorized.c_str()))) {

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedByService.c_str()));

        //하나만 표시하기 위해서 삭제.
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedByService.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithError.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithRestriction.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::DeviceDisconnectedWithUnauthorized.c_str()));
    }

    if (events.end() != events.find(QString::fromUtf8(Wisenet::AdditionalLogType::FirmwareUpdgrade.c_str()))) {

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AdditionalLogType::FirmwareUpdgrade.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AdditionalLogType::FirmwareUpdgrade.c_str()));
    }

    //AuditLog Type.
    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::LogIn.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::LogIn.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::LogIn.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::BackupSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::BackupSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::BackupSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::RestoreSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::RestoreSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::RestoreSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::InitializeSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::InitializeSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::InitializeSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::LogSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::LogSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::LogSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::UserSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::UserSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::UserSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::UserGroupSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::UserGroupSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::UserGroupSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::EventRuleSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::EventRuleSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::EventRuleSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::ScheduleSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::ScheduleSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::ScheduleSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::DeviceSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::DeviceSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::DeviceSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::ChannelSettings.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::ChannelSettings.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::ChannelSettings.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::DeviceStatus.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::DeviceStatus.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::DeviceStatus.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::PTZControl.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::PTZControl.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::PTZControl.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::WatchingLive.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::WatchingLive.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::WatchingLive.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::WatchingPlayback.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::WatchingPlayback.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::WatchingPlayback.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::ExportingVideo.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::ExportingVideo.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::ExportingVideo.c_str()));
    }

    if(events.end() != events.find(QString::fromUtf8(Wisenet::AuditLogType::UpdateSoftware.c_str()))){

        AddChildend(rootItem, ItemType::Event, QString::fromUtf8(Wisenet::AuditLogType::UpdateSoftware.c_str()));
        events.remove(QString::fromUtf8(Wisenet::AuditLogType::UpdateSoftware.c_str()));
    }

    // Smart Search Type
    if(events.end() != events.find("Entering")){
        AddChildend(rootItem, ItemType::Event, "Entering");
        events.remove("Entering");
    }
    if(events.end() != events.find("Exiting")){
        AddChildend(rootItem, ItemType::Event, "Exiting");
        events.remove("Exiting");
    }
    if(events.end() != events.find("Passing")){
        AddChildend(rootItem, ItemType::Event, "Passing");
        events.remove("Passing");
    }

    if(0 == events.count()){
        return;
    }

    TreeItemModel * dynamicEventModel = nullptr;

    if(!m_treeItemModelMap.contains(QString::fromUtf8(Wisenet::FixedAlarmType::DynamicEvent.c_str()))){

        dynamicEventModel = AddChildend(rootItem, ItemType::EventGroup, QString::fromUtf8(Wisenet::FixedAlarmType::DynamicEvent.c_str()));
        events.remove(QString::fromUtf8(Wisenet::FixedAlarmType::DynamicEvent.c_str()));

    }else{
        dynamicEventModel = m_treeItemModelMap.take(QString::fromUtf8(Wisenet::FixedAlarmType::DynamicEvent.c_str()));
    }

    foreach(QString event , events){
        AddChildend(dynamicEventModel, ItemType::Event, event);
        events.remove(event);
    }
}

void EventFilterTreeSourceModel::removeAll()
{
    beginResetModel();

    auto childrenList = rootItem->children();

    foreach(TreeItemModel* itemModel, childrenList){
        itemModel->removeAllChild();
    }
    rootItem->removeAllChild();
    m_treeItemModelMap.clear();
    endResetModel();
}

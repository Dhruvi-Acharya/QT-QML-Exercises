#include "EventNotificationsViewModel.h"
#include "../../../Utility/TranslationSort.h"


EventNotificationsViewModel::EventNotificationsViewModel(QObject* parent)
{
    mapEventSettings.clear();
    if( mapEventSettings.size() <= 0 )
        loadData();
    Q_UNUSED(parent);
}
EventNotificationsViewModel::~EventNotificationsViewModel()
{
    qDebug() << "~EventNotificationsViewModel()";
}

void EventNotificationsViewModel::loadData()
{
    QStringList uncheckedList = QCoreServiceManager::Instance().Settings()->eventNotifications();
    mapEventSettings.clear();
    foreach( QString eventNameContainer, uncheckedList )
        mapEventSettings[eventNameContainer] = true;
}

bool EventNotificationsViewModel::isCheckedEvent( QString eventName )
{
    return !(mapEventSettings[eventName]);
}

bool EventNotificationsViewModel::setEventNotification( QString eventName, bool isShow )
{
    mapEventSettings[eventName] = !isShow;
    return isShow;
}

QStringList EventNotificationsViewModel::getEventsList( QString type )
{
    QStringList returnList;
    if( type == "event" )
    {
        for(auto& event : Wisenet::EventSystemMap){
            if( event.second == Wisenet::EventSystemType::Event )
            {
                returnList  << QString::fromStdString(event.first);
            }
        }
        QSet<QString> dynamicEvents = QCoreServiceManager::Instance().DB()->GetDynamicEventList();
        foreach(auto & event,dynamicEvents){
            returnList.push_back(event);
        }
    }
    else if( type == "system" )
    {
        for(auto& event : Wisenet::EventSystemMap){
            if( event.second == Wisenet::EventSystemType::System )
            {
                returnList  << QString::fromStdString(event.first);
            }
        }
    }
    else
    {
        return QCoreServiceManager::Instance().DB()->GetTotalEventList();
    }

    return TranslationSort::getInstance()->sort(returnList);
}

bool EventNotificationsViewModel::isCheckedShowAll( QString type )
{
    if( type == "event")
    {
        return QCoreServiceManager::Instance().Settings()->eventNotificationShowAll();
    }
    else if( type == "system" )
    {
        return QCoreServiceManager::Instance().Settings()->systemNotificationShowAll();
    }
    else
    {
        return true;
    }
}

void EventNotificationsViewModel::saveSettings( bool isEventShowAll, bool isSystemShowAll )
{
    QStringList eventNotifications;
    bool isShowAllEvents = true;
    for( auto& i: mapEventSettings )
    {
        if( i.first.length() > 0 && i.second == true )
        {
            eventNotifications << i.first;
            isShowAllEvents = false;
        }
    }
    if( isShowAllEvents ) // Eventho the user didn't check the show all checkbox, if no event is unchecked, then turn on the show all events
    {
        QCoreServiceManager::Instance().Settings()->setEventNotificationShowAll( true );
        QCoreServiceManager::Instance().Settings()->setSystemNotificationShowAll( true );
    }
    else
    {
        QCoreServiceManager::Instance().Settings()->setEventNotificationShowAll( isEventShowAll );
        QCoreServiceManager::Instance().Settings()->setSystemNotificationShowAll( isSystemShowAll );
    }
    QCoreServiceManager::Instance().Settings()->setEventNotifications( eventNotifications );
    QCoreServiceManager::Instance().Settings()->saveSetting();
}

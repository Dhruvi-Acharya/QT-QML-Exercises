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
#pragma once
#include <QAbstractListModel>
#include "QCoreServiceManager.h"
#include <map>
#include <unordered_map>

class EventNotificationsViewModel : public QObject
{
    Q_OBJECT

  public:
    EventNotificationsViewModel(QObject* parent = nullptr);
    ~EventNotificationsViewModel();

    Q_INVOKABLE QStringList getEventsList( QString type );
    Q_INVOKABLE bool isCheckedEvent( QString eventName );
    Q_INVOKABLE bool setEventNotification( QString eventName, bool isShow );

    Q_INVOKABLE bool isCheckedShowAll( QString type );
    Q_INVOKABLE void saveSettings( bool isEventShowAll, bool isSystemShowAll );
    Q_INVOKABLE void loadData();

signals:
private:
    std::unordered_map<QString, bool> mapEventSettings; // Events Not showing. true : Do Not Notify
};


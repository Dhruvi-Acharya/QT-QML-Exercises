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
#include <QThread>
#include <QWaitCondition>
#include <QSharedPointer>
#include "QCoreServiceManager.h"

class EventLogThread : public QThread
{
    Q_OBJECT
public:
    EventLogThread(QObject *parent = nullptr);
    ~EventLogThread();

    void process(std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> deletedDevices,
                 std::shared_ptr<std::vector<Wisenet::Core::EventLog>> eventLogs);

    static void registerQml();

signals:
    void processCompleted(std::shared_ptr<std::vector<Wisenet::Core::EventLog>> eventLogs,
                          QSharedPointer<QVector<QVector<QVariant>>> data,
                          QSharedPointer<QVector<int>> dateTimeDescIndex,
                          QSharedPointer<QVector<int>> cameraDescIndex,
                          QSharedPointer<QVector<int>> eventDescIndex,
                          QVector<QStringList> cameraList,
                          QSet<QString> eventList);

protected:
    void run() override;

private:
    QString getEventName(std::string& type);
    QString getDescription(Wisenet::Core::EventLog::Parameters& parameters);

    QString GetDeletedDeviceName(std::string& deviceID);
    QString GetDeletedChannelName(std::string& deviceID, std::string& channelID);

    QMutex m_mutex;
    QWaitCondition m_condition;
    bool m_restart = false;
    bool m_abort = false;

    std::shared_ptr<std::vector<Wisenet::Core::EventLog>> m_eventLogs;
    std::shared_ptr<std::map<Wisenet::uuid_string,Wisenet::Core::DeletedDevice>> m_deletedDevices;

    QSharedPointer<QVector<QVector<QVariant>>> m_data;
    QSharedPointer<QVector<int>> m_dateTimeDescIndex;
    QSharedPointer<QVector<int>> m_cameraDescIndex;
    QSharedPointer<QVector<int>> m_eventDescIndex;

};

Q_DECLARE_METATYPE(std::shared_ptr<std::vector<Wisenet::Core::EventLog>>)
Q_DECLARE_METATYPE(QSharedPointer<QVector<QVector<QVariant>>>)
Q_DECLARE_METATYPE(QSharedPointer<QVector<int>>)
Q_DECLARE_METATYPE(QVector<QStringList>)
Q_DECLARE_METATYPE(QSet<QString>)

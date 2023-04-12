#pragma once

#include <QObject>
#include <QDebug>

class EventModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString sourceId READ sourceId WRITE setSourceId NOTIFY sourceIdChanged)
    Q_PROPERTY(QString sourceName READ sourceName WRITE setSourcelName NOTIFY sourceNameChanged)
    Q_PROPERTY(QString alarmType READ alarmType WRITE setAlarmType NOTIFY alarmTypeChanged)
    Q_PROPERTY(QString alarmName READ alarmName WRITE setAlarmName NOTIFY alarmNameChanged)
    Q_PROPERTY(QString occurrenceTime READ occurrenceTime WRITE setOccurrenceTime NOTIFY occurrenceTimeChanged)
    Q_PROPERTY(long long utcTime READ utcTime WRITE setUtcTime NOTIFY utcTimeChanged)
    Q_PROPERTY(long long deviceTime READ deviceTime WRITE setDeviceTime NOTIFY deviceTimeChanged)
    Q_PROPERTY(bool isPriorityAlarm READ isPriorityAlarm WRITE setIsPriorityAlarm NOTIFY isPriorityAlarmChanged)
    Q_PROPERTY(bool played READ played WRITE setPlayed NOTIFY playedChanged)

public:
    explicit EventModel(QObject *parent = nullptr);
    ~EventModel();

    QString sourceId() { return m_sourceid; }
    QString sourceName() { return m_sourcelName; }
    QString alarmType() { return m_alarmType; }
    QString alarmName() { return m_alarmName; }
    QString occurrenceTime() { return m_occurrenceTime; }
    long long utcTime() { return m_utcTime; }
    long long deviceTime() { return m_deviceTime; }
    bool isPriorityAlarm() { return m_isPriorityAlarm; }
    bool played() { return m_played; }

    void setSourceId(QString sourceId)
    {
        m_sourceid = sourceId;
        emit sourceIdChanged(sourceId);
    }

    void setSourcelName(QString sourceName)
    {
        m_sourcelName = sourceName;
        emit sourceNameChanged(sourceName);
    }

    void setAlarmType(QString alarmType)
    {
        m_alarmType = alarmType;
        emit alarmTypeChanged(alarmType);
    }

    void setAlarmName(QString alarmName)
    {
        m_alarmName = alarmName;
    }

    void setOccurrenceTime(QString occurrenceTime)
    {
        m_occurrenceTime = occurrenceTime;
        emit occurrenceTimeChanged(occurrenceTime);
    }

    void setUtcTime(long long utcTime)
    {
        m_utcTime = utcTime;
        emit utcTimeChanged(utcTime);
    }

    void setDeviceTime(long long deviceTime)
    {
        m_deviceTime = deviceTime;
        emit deviceTimeChanged(deviceTime);
    }

    void setIsPriorityAlarm(bool isPriorityAlarm)
    {
        m_isPriorityAlarm = isPriorityAlarm;
        emit isPriorityAlarmChanged(isPriorityAlarm);
    }

    void setPlayed(bool played){
        m_played = played;
        emit playedChanged(played);
    }

signals:
    void sourceIdChanged(QString sourceId);
    void sourceNameChanged(QString sourceName);
    void alarmTypeChanged(QString alarmType);
    void alarmNameChanged(QString alarmName);
    void occurrenceTimeChanged(QString occurrenceTime);
    void utcTimeChanged(long long utcTime);
    void deviceTimeChanged(long long deviceTime);
    void isPriorityAlarmChanged(bool isPriorityAlarm);
    void playedChanged(bool played);

private:
    QString m_sourceid;
    QString m_sourcelName;
    QString m_alarmType;
    QString m_alarmName;
    QString m_occurrenceTime;
    long long m_utcTime;
    long long m_deviceTime;
    bool m_isPriorityAlarm = false;
    bool m_played = false;
};

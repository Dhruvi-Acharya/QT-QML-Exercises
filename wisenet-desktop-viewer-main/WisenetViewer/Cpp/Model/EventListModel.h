#pragma once

#include <QAbstractListModel>
#include <QDebug>
#include "EventModel.h"
#include "QCoreServiceManager.h"
#include "DeviceClient/IDeviceClient.h"

#include <QCoreApplication>
#include "QLocaleManager.h"
#include "LogSettings.h"

class EventListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int alertAlarmCount READ alertAlarmCount WRITE setAlertAlarmCount NOTIFY alertAlarmCountChanged)

public:
    enum ModelRoles {
        SourceIdRole = Qt::UserRole + 1,
        SourceNameRole,
        AlarmTypeRole,
        AlarmNameRole,
        OccurrenceTimeRole,
        UtcTimeRole,
        DeviceTimeRole,
        IsPriorityAlarmRole,
        PlayedRole,
    };

    explicit EventListModel(QObject *parent = nullptr);
    ~EventListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Q_INVOKABLE  bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const;

    int count(){
        return m_data.size();
    }

    int alertAlarmCount(){
        return m_alertAlarmCount;
    }

    void setMaxEventCount(int count);
    void setAlertAlarmCount(int alertAlarmCount);
    void applySaveChannels(QString id, QString name);

    Q_INVOKABLE void extendDevicePassword(int position);

public slots:
    void insertFirst(EventModel* model);
    void append(EventModel* model);
    void removeAt(int position);
    void removeAll();

signals:
    void countChanged();
    void alertAlarmCountChanged(int alertAlarmCount);

private:
    void moveAlarm(const QModelIndex &idx);

private:
    int m_alertAlarmCount;
    QList<EventModel*> m_data;
    int m_maxEventCount = 0;
};

#pragma once
#include <QAbstractListModel>
#include "QCoreServiceManager.h"


class AlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ModelRoles {
        ColorRole = Qt::UserRole + 1,
        ChannelNameRole,
        TimeRole,
        AlarmTypeRole,
    };

    explicit AlarmModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& p) const;
    QVariant data(const QModelIndex& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    int count() const;

public slots:
    void append(QObject* o);
    void insert(QObject* o, int i);
    void remove(int idx);
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

signals:
    void countChanged(int count);

private:
    //QString GetAlarmColor(Wisenet::AlarmEventType::Type alarmType);
    //QString GetAlarmString(Wisenet::AlarmEventType::Type alarmType);

    QList<QObject*> m_data;
};

#pragma once
#include <QSortFilterProxyModel>
#include <QSet>

class EventListFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool cameraFilterEnabled READ cameraFilterEnabled WRITE setCameraFilterEnabled NOTIFY cameraFilterEnabledChanged)
    Q_PROPERTY(bool eventTypeFilterEnabled READ eventTypeFilterEnabled WRITE setEventTypeFilterEnabled NOTIFY eventTypeFilterEnabledChanged)

public:
    explicit EventListFilterProxyModel(QObject *parent = nullptr);
    ~EventListFilterProxyModel();

    bool cameraFilterEnabled()
    {
        return m_cameraFilterEnabled;
    }
    void setCameraFilterEnabled(const bool cameraFilterEnabled)
    {
        m_cameraFilterEnabled = cameraFilterEnabled;
        emit cameraFilterEnabledChanged(m_cameraFilterEnabled);
    }

    bool eventTypeFilterEnabled()
    {
        return m_eventTypeFilterEnabled;
    }
    void setEventTypeFilterEnabled(const bool eventTypeFilterEnabled)
    {
        m_eventTypeFilterEnabled = eventTypeFilterEnabled;
        emit eventTypeFilterEnabledChanged(m_eventTypeFilterEnabled);
    }

    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

    Q_INVOKABLE bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;

signals:
    void cameraFilterEnabledChanged(bool arg);
    void eventTypeFilterEnabledChanged(bool arg);

public slots:
    void setChannelList(QStringList list);
    void setEventTypeFilter(QStringList list);
    void setFilterText(const QString& filterText);
    void updateFilter();    

private:
    QSet<QString> m_channelList;
    QSet<QString> m_eventTypeFilter;
    QString m_filterText;

    bool m_cameraFilterEnabled = false;
    bool m_eventTypeFilterEnabled = false;
};

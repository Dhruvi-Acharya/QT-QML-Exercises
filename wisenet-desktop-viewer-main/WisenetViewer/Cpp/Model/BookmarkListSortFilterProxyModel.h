#pragma once
#include <QSortFilterProxyModel>
#include <QSet>

class BookmarkListSortFilterProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool cameraFilterEnabled READ cameraFilterEnabled WRITE setCameraFilterEnabled NOTIFY cameraFilterEnabledChanged)
    Q_PROPERTY(bool datetimeFilterEnabled READ datetimeFilterEnabled WRITE setEventTypeFilterEnabled NOTIFY datetimeFilterEnabledChanged)
public:
    explicit BookmarkListSortFilterProxyModel(QObject *parent = nullptr);
    virtual ~BookmarkListSortFilterProxyModel();

    bool cameraFilterEnabled()
    {
        return m_cameraFilterEnabled;
    }
    void setCameraFilterEnabled(const bool cameraFilterEnabled)
    {
        m_cameraFilterEnabled = cameraFilterEnabled;
        emit cameraFilterEnabledChanged(m_cameraFilterEnabled);
    }

    bool datetimeFilterEnabled()
    {
        return m_datetimeFilterEnabled;
    }
    void setEventTypeFilterEnabled(const bool datetimeFilterEnabled)
    {
        m_datetimeFilterEnabled = datetimeFilterEnabled;
        emit datetimeFilterEnabledChanged(m_datetimeFilterEnabled);
    }

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;

signals:
    void cameraFilterEnabledChanged(bool arg);
    void datetimeFilterEnabledChanged(bool arg);

public slots:
    void setChannelList(QStringList list);
    void setUtcFilter(long long startTime, long long endTime);
    void setFilterText(const QString& filterText);
    void updateFilter();

private:
    QSet<QString> m_channelList;
    long long m_startTime;
    long long m_endTime;
    QString m_filterText;

    bool m_cameraFilterEnabled = false;
    bool m_datetimeFilterEnabled = false;
};

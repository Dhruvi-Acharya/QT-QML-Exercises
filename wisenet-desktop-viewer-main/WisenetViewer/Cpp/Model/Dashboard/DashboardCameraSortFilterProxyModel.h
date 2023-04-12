#pragma once
#include "DashboardErrorListSortProxyModel.h"
#include <QDebug>

class DashboardCameraSortFilterProxyModel : public DashboardErrorListSortProxyModel
{
    Q_OBJECT
    Q_PROPERTY(bool useGroupFilter READ useGroupFilter WRITE setUseGroupFilter NOTIFY useGroupFilterChanged)
    Q_PROPERTY(bool useDeviceFilter READ useDeviceFilter WRITE setUseDeviceFilter NOTIFY useDeviceFilterChanged)

    Q_PROPERTY(QString groupId READ groupId WRITE setGroupId NOTIFY groupIdChanged)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
public:
    explicit DashboardCameraSortFilterProxyModel(QObject *parent = nullptr);
    virtual ~DashboardCameraSortFilterProxyModel();

    bool useGroupFilter(){ return m_useGroupFilter; }
    bool useDeviceFilter(){ return m_useGroupFilter; }

    QString groupId(){ return m_groupId; }
    QString deviceId(){ return m_deviceId; }

    void setUseGroupFilter(bool useGroup)
    {
        qDebug() << "setUseGroupFilter()" << useGroup;
        m_useGroupFilter = useGroup;
        emit useGroupFilterChanged();
    }

    void setUseDeviceFilter(bool useDevice)
    {
        qDebug() << "setUseDeviceFilter()" << useDevice;
        m_useDeviceFilter = useDevice;
        emit useDeviceFilterChanged();
    }

    void setGroupId(QString groupId)
    {
        qDebug() << "setGroupId()" << groupId;
        m_groupId = groupId;
        emit groupIdChanged();
    }

    void setDeviceId(QString deviceId)
    {
        qDebug() << "setDeviceId()" << deviceId;
        m_deviceId = deviceId;
        emit deviceIdChanged();
    }

    bool isParentGroup(const QString& id) const;

protected:
    bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

signals:
    void useGroupFilterChanged();
    void useDeviceFilterChanged();

    void groupIdChanged();
    void deviceIdChanged();

private:
    bool m_useGroupFilter = false;
    bool m_useDeviceFilter = false;

    QString m_groupId = "";
    QString m_deviceId = "";
};

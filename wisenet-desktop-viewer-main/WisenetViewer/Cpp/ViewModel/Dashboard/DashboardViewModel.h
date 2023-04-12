#pragma once

#include <QObject>
#include <QDebug>
#include <QDateTime>
#include "QCoreServiceManager.h"
#include "QLocaleManager.h"
#include "LogSettings.h"
#include "../../Model/Dashboard/DashboardCameraSortFilterProxyModel.h"
#include "../../Model/Dashboard/DashboardDeviceTreeModel.h"
#include "../../Model/Dashboard/DashboardDeviceErrorListModel.h"
#include "../../Model/Dashboard/DashboardErrorListModel.h"
#include "../../Model/Dashboard/DashboardErrorListSortProxyModel.h"
#include "../../Model/Dashboard/DashboardGroupTreeModel.h"
#include "../../Model/TreeProxyModel.h"

class DashboardViewModel : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int recorderCount READ recorderCount NOTIFY recorderCountChanged)

    Q_PROPERTY(int powerErrorCount READ powerErrorCount NOTIFY powerErrorCountChanged)
    Q_PROPERTY(int diskErrorCount READ diskErrorCount NOTIFY diskErrorCountChanged)
    Q_PROPERTY(int overloadErrorCount READ overloadErrorCount NOTIFY overloadErrorCountChanged)
    Q_PROPERTY(int connectionErrorCount READ connectionErrorCount NOTIFY connectionErrorCountChanged)
    Q_PROPERTY(int fanErrorCount READ fanErrorCount NOTIFY fanErrorCountChanged)

    Q_PROPERTY(int recorderErrorCount READ recorderErrorCount NOTIFY recorderErrorCountChanged)

    Q_PROPERTY(int disconnectedCameraCount READ disconnectedCameraCount NOTIFY disconnectedCameraCountChanged)
    Q_PROPERTY(int connectedCameraCount READ connectedCameraCount NOTIFY connectedCameraCountChanged)

    // Device (Recorder) error
    Q_PROPERTY(DashboardDeviceErrorListModel* connectionErrorListModel READ connectionErrorListModel NOTIFY connectionErrorListModelChanged)
    Q_PROPERTY(DashboardDeviceErrorListModel* diskErrorListModel READ diskErrorListModel NOTIFY diskErrorListModelChanged)
    Q_PROPERTY(DashboardDeviceErrorListModel* overloadErrorListModel READ overloadErrorListModel NOTIFY overloadErrorListModelChanged)
    Q_PROPERTY(DashboardDeviceErrorListModel* powerErrorListModel READ powerErrorListModel NOTIFY powerErrorListModelChanged)
    Q_PROPERTY(DashboardDeviceErrorListModel* fanErrorListModel READ fanErrorListModel NOTIFY fanErrorListModelChanged)

    // Camera error
    Q_PROPERTY(DashboardErrorListSortProxyModel* disconnectedCameraListSortFilterProxyModel READ disconnectedCameraListSortFilterProxyModel NOTIFY disconnectedCameraListSortFilterProxyModelChanged)
    Q_PROPERTY(DashboardErrorListSortProxyModel* connectedCameraListSortFilterProxyModel READ connectedCameraListSortFilterProxyModel NOTIFY connectedCameraListSortFilterProxyModelChanged)

    // Tree
    Q_PROPERTY (DashboardGroupTreeModel* groupTreeModel READ groupTreeModel NOTIFY groupTreeModelChanged)
    Q_PROPERTY (DashboardDeviceTreeModel* deviceTreeModel READ deviceTreeModel NOTIFY deviceTreeModelChanged)
    Q_PROPERTY (TreeProxyModel* groupTreeProxyModel READ groupTreeProxyModel NOTIFY groupTreeProxyModelChanged)
    Q_PROPERTY (TreeProxyModel* deviceTreeProxyModel READ deviceTreeProxyModel NOTIFY deviceTreeProxyModelChanged)
public:
    static DashboardViewModel* getInstance()
    {
        static DashboardViewModel instance;
        return &instance;
    }

    int recorderCount(){
        auto device = QCoreServiceManager::Instance().DB()->GetDevices();
        return (int)device.size();
    }
    int powerErrorCount(){ return m_powerErrorListModel.deviceCount(); }
    int diskErrorCount(){ return m_diskErrorListModel.deviceCount(); }
    int overloadErrorCount(){ return m_overloadErrorListModel.deviceCount(); }
    int connectionErrorCount(){ return m_connectionErrorListModel.deviceCount(); }
    int fanErrorCount(){ return m_fanErrorListModel.deviceCount(); }

    int recorderErrorCount(){
        return powerErrorCount() + diskErrorCount() + overloadErrorCount() + connectionErrorCount() + fanErrorCount();
    }

    int disconnectedCameraCount(){ return m_disconnectedCameraList.rowCount(); }
    int connectedCameraCount(){ return m_connectedCameraList.rowCount(); }

    DashboardDeviceErrorListModel* powerErrorListModel(){ return &m_powerErrorListModel; }
    DashboardDeviceErrorListModel* diskErrorListModel(){ return &m_diskErrorListModel; }
    DashboardDeviceErrorListModel* overloadErrorListModel(){ return &m_overloadErrorListModel; }
    DashboardDeviceErrorListModel* connectionErrorListModel(){ return &m_connectionErrorListModel; }
    DashboardDeviceErrorListModel* fanErrorListModel(){ return &m_fanErrorListModel; }

    DashboardErrorListSortProxyModel* disconnectedCameraListSortFilterProxyModel(){ return &m_disconnectedCameraListProxy; }
    DashboardErrorListSortProxyModel* connectedCameraListSortFilterProxyModel(){ return &m_connectedCameraListProxy; }

    DashboardGroupTreeModel* groupTreeModel(){ return &m_groupTreeModel; }
    TreeProxyModel* groupTreeProxyModel(){ return &m_groupTreeProxyModel; }
    DashboardDeviceTreeModel* deviceTreeModel(){ return &m_deviceTreeModel; }
    TreeProxyModel* deviceTreeProxyModel(){ return &m_deviceTreeProxyModel; }

    explicit DashboardViewModel(QObject *parent = nullptr);
    ~DashboardViewModel();

    void initialize();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);
    void Event_AddDevice(QCoreServiceEventPtr event);
    void Event_RemoveDevices(QCoreServiceEventPtr event);
    void Event_DeviceStatus(QCoreServiceEventPtr event);
    void Event_ChannelStatus(QCoreServiceEventPtr event);
    void Event_DeviceLatestStatus(QCoreServiceEventPtr event);

    void UpdateDeviceError(const Wisenet::Device::Device& device, const std::map<std::string, bool>& eventStatus);
    void UpdateConnectionStatus(const Wisenet::Device::Device& device, const Wisenet::Device::DeviceStatus& deviceStatus);

    ErrorItem* newErrorItem(const QString& objectId, const QString& objectName, const int objectType);
    void appendSystemEvent(const QString& eventType, const QString& objectId, const QString& objectName);    
    Q_INVOKABLE QVariantMap getChartData();
    void setDashboardData(std::map<int, Wisenet::Core::DashboardData>& dashboardData, QVariantMap& vMap, int interval);

signals:
    void recorderCountChanged();

    void powerErrorCountChanged();
    void diskErrorCountChanged();
    void overloadErrorCountChanged();
    void connectionErrorCountChanged();
    void fanErrorCountChanged();

    void recorderErrorCountChanged();

    void disconnectedCameraCountChanged();
    void connectedCameraCountChanged();

    void powerErrorListModelChanged();
    void diskErrorListModelChanged();
    void overloadErrorListModelChanged();
    void connectionErrorListModelChanged();
    void fanErrorListModelChanged();

    void disconnectedCameraListSortFilterProxyModelChanged();
    void connectedCameraListSortFilterProxyModelChanged();

    void groupTreeModelChanged();
    void groupTreeProxyModelChanged();
    void deviceTreeModelChanged();
    void deviceTreeProxyModelChanged();

    void chartDataUpdated(QVariantMap vMap, QVariantMap vMap2, QVariantMap vMap3, QVariantMap vMap4);

public slots:
    void printStatus();
    void refreshData();

private:
    DashboardDeviceErrorListModel m_powerErrorListModel;
    DashboardDeviceErrorListModel m_diskErrorListModel;
    DashboardDeviceErrorListModel m_overloadErrorListModel;
    DashboardDeviceErrorListModel m_connectionErrorListModel;
    DashboardDeviceErrorListModel m_fanErrorListModel;

    DashboardErrorListModel m_disconnectedCameraList;
    DashboardErrorListModel m_connectedCameraList;
    DashboardCameraSortFilterProxyModel m_disconnectedCameraListProxy;
    DashboardCameraSortFilterProxyModel m_connectedCameraListProxy;

    DashboardGroupTreeModel m_groupTreeModel;
    TreeProxyModel m_groupTreeProxyModel;
    DashboardDeviceTreeModel m_deviceTreeModel;
    TreeProxyModel m_deviceTreeProxyModel;
};

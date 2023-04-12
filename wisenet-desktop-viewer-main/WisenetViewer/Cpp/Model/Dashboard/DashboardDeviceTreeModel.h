#pragma once
#include "../BaseTreeModel.h"

class DashboardDeviceTreeModel : public BaseTreeModel
{
    Q_OBJECT
public:
    explicit DashboardDeviceTreeModel(QObject *parent = nullptr);
    virtual ~DashboardDeviceTreeModel();

    void SetupTree();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

private:
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);

    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_AddDevice(QCoreServiceEventPtr event);
    void Event_RemoveDevices(QCoreServiceEventPtr event);
    void Event_DeviceStatus(QCoreServiceEventPtr event);

    void AddTreeDeviceItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);

    void RemoveTreeDeviceItem(const QString& deviceId);
signals:
    void invalidate();
    void expandRoot();

private:
    TreeItemModel* m_rootItemModel;
    QMap<QString, TreeItemModel*> m_treeItemModelMap;
};

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

#include <Cpp/Model/BaseTreeModel.h>

using namespace WisenetViewerDefine;

class DeviceSelectionTreeSourceModel : public BaseTreeModel
{
    Q_OBJECT
    Q_PROPERTY(bool isObjectSearchMode READ isObjectSearchMode WRITE setIsObjectSearchMode NOTIFY isObjectSearchModeChanged)

public:
    explicit DeviceSelectionTreeSourceModel(QObject *parent = nullptr);
    virtual ~DeviceSelectionTreeSourceModel();

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

    Q_INVOKABLE QModelIndex getModelIndex(QString guid);

public:
    bool isObjectSearchMode()
    {
        return m_isObjectSearchMode;
    }
    void setIsObjectSearchMode(bool isObjectSearchMode)
    {
        m_isObjectSearchMode = isObjectSearchMode;
    }

public slots:
    void initializeTree();

signals:
    void invalidate();
    void isObjectSearchModeChanged(bool isObjectSearchMode);
    void treeInitialized();

private:
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name,
                                      const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber, const bool smartSearchCap = false);
    void clear();
    void SetupCategoryTree();

    void Event_FullInfo(QCoreServiceEventPtr event);

    void Event_SaveGroup(QCoreServiceEventPtr event);
    void Event_RemoveGroup(QCoreServiceEventPtr event);
    void Event_AddGroupMapping(QCoreServiceEventPtr event);
    void Event_RemoveGroupMapping(QCoreServiceEventPtr event);
    void Event_UpdateGroupMapping(QCoreServiceEventPtr event);

    void Event_AddDevice(QCoreServiceEventPtr event);
    void Event_RemoveDevices(QCoreServiceEventPtr event);
    void Event_DeviceStatus(QCoreServiceEventPtr event);
    void Event_ChannelStatus(QCoreServiceEventPtr event);
    void Event_SaveChannels(QCoreServiceEventPtr event);
    void Event_UpdateDevices(QCoreServiceEventPtr event);

    TreeItemModel* AddTreeGroupItem(const Wisenet::Core::Group& group);
    void AddTreeDeviceItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);
    void UpdateTreeGroupItem(const Wisenet::Core::Group& group);

    void AddTreeChannelItemToRoot(const Wisenet::Device::Device::Channel& channel);
    void UpdateTreeChannelItem(const Wisenet::Device::Device::Channel& channel);

    void RemoveTreeChannelItem(const QString& channelId);

    TreeItemModel*                  m_resourceRootModel;
    QMap<QString, TreeItemModel*>   m_treeItemModelMap;

    bool m_isObjectSearchMode = false;
};


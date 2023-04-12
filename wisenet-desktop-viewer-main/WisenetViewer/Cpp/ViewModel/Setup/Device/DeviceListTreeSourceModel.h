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

class DeviceListTreeSourceModel : public BaseTreeModel
{
public:
    Q_OBJECT

public:
    explicit DeviceListTreeSourceModel(QObject *parent = nullptr);
    virtual ~DeviceListTreeSourceModel();

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    Q_INVOKABLE QModelIndex getModelIndex(QString guid);

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

    Q_INVOKABLE void readDevices();
signals:
    void expand();

private:
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber);

    void setupCategoryTree();

    void addTreeDeviceItem(const Wisenet::Device::Device &device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);
    void addTreeDeviceChannelItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);

    void addDevice(QCoreServiceEventPtr event);
    void removeDevices(QCoreServiceEventPtr event);
    void deviceStatusEvent(QCoreServiceEventPtr event);
    void channelStatusEvent(QCoreServiceEventPtr event);

    TreeItemModel*                  m_resourceRootModel;
    QMap<QString, TreeItemModel*>   m_treeItemModelMap;
};


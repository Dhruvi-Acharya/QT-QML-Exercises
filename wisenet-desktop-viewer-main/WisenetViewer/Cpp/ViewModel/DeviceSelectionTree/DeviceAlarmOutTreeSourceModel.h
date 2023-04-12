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

class DeviceAlarmOutTreeSourceModel : public BaseTreeModel
{
    Q_OBJECT

public:
    explicit DeviceAlarmOutTreeSourceModel(QObject *parent = nullptr);
    virtual ~DeviceAlarmOutTreeSourceModel();


    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    Q_INVOKABLE QModelIndex getModelIndex(QString guid);
    Q_INVOKABLE void readAlarmOut();

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
private:
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber);

    void setupCategoryTree();


    void addTreeDeviceItem(const Wisenet::Device::Device &device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);

    void addTreeDeviceAlarmOutItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);


    TreeItemModel*                  m_resourceRootModel;
    QMap<QString, TreeItemModel*>   m_treeItemModelMap;
};


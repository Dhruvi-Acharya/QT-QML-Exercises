/*********************************************************************************
 * Copyright(c) 2021 by Hanwha Techwin Co., Ltd.
 *
 * This software is copyrighted by, and is the sole property of Hanwha Techwin.
 * All rigths, title, ownership, or other interests in the software remain the
 * property of Hanwha Techwin. This software may only be used in accordance with
 * the corresponding license agreement. Any unauthorized use, duplication,
 * transmission, distribution, or disclosure of this software is expressly
 * forbidden.
 *
 * This Copyright notice may not be removed or modified without prior written
 * consent of Hanwha Techwin.
 *
 * Hanwha Techwin reserves the right to modify this software without notice.
 *
 * Hanwha Techwin Co., Ltd.
 * KOREA
 * https://www.hanwha-security.com/
 *********************************************************************************/
#pragma once
#include "QCoreServiceManager.h"
#include <QModelIndex>
#include <QAbstractItemModel>
#include "MainTreeItemBase.h"

class MainTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    Q_INVOKABLE void showDeviceContextMenu(float x, float y);
    Q_INVOKABLE QString getSelectedIndex(const QModelIndex& itemIndex);
    Q_INVOKABLE void refreshRootItem();

public:
    explicit MainTreeModel(QObject *parent = nullptr);
    ~MainTreeModel();

    // Basic functionality:
    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QHash<int,QByteArray> roleNames() const override;

    void AddTreeDeviceItem(const Wisenet::Device::Device& device,
                           const Wisenet::Device::DeviceStatus& deviceStatus);

    void RemoveTreeDeviceItem(QCoreServiceEventPtr event);
    void UpdateTreeDeviceItem(QCoreServiceEventPtr event);
    void AddTreeUserItem(QCoreServiceEventPtr event) ;

public slots:    
    void coreServiceEventTriggered(QCoreServiceEventPtr event);


private:
    void SetupCategoryTree();
    MainTreeItemBase *getItem(const QModelIndex &index) const;

    MainTreeItemBase *m_rootItem;

    MainTreeItemBase* m_deviceRootItem;
    MainTreeItemBase* m_layoutRootItem;
    MainTreeItemBase* m_groupRootItem;
    MainTreeItemBase* m_userRootItem;
};


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

#include <QVector>
#include <QString>
#include <QVariant>
#include "BaseTreeItemInfo.h"
#include "LayoutTreeItemInfo.h"
#include "DeviceTreeItemInfo.h"
#include "WisenetViewerDefine.h"

using namespace WisenetViewerDefine;

class TreeItemModel
{
public:
    explicit TreeItemModel(BaseTreeItemInfo* treeItemInfo, TreeItemModel *parent = nullptr);
    explicit TreeItemModel(const QVariant &data, TreeItemModel *parent = nullptr);
    ~TreeItemModel();

    bool appendChild(TreeItemModel* child);
    bool insertChild(int position, TreeItemModel* child);
    bool unlinkChild(int position);
    bool removeChild(int position);
    void removeAllChild();

    QList<TreeItemModel*> children();
    TreeItemModel *child(int index);
    int childCount() const;
    int columnCount() const;
    QVariant data(int column) const;
    int row() const;
    TreeItemModel* parentItem();
    void setParentItem(TreeItemModel* parent);

    int indexOfChild(TreeItemModel * child);
    void getChildrenUuid(QSet<QString>& childrenSet);
    QString getName() const;
    void setName(QString name);
    QString getUuid() const;
    ItemType getItemType();
    BaseTreeItemInfo* getTreeItemInfo() const;
    void addCamera(const QString &deviceId, const QString &channelId);
    void setStatus(int status);
private:
    QList<TreeItemModel*> m_childItems;
    BaseTreeItemInfo* m_treeItemInfo;
    QVariant m_treeValue;
    TreeItemModel *m_parentItem;
};


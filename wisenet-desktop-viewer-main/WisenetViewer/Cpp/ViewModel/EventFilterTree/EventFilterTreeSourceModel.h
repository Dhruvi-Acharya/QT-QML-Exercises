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

class EventFilterTreeSourceModel : public BaseTreeModel
{
    Q_OBJECT

public:
    explicit EventFilterTreeSourceModel(QObject *parent = nullptr);
    virtual ~EventFilterTreeSourceModel();

    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;

    Q_INVOKABLE QModelIndex getIndexByValue(QString event);
    Q_INVOKABLE void connectCoreService();
    Q_INVOKABLE void setAlertAlarm();
    Q_INVOKABLE void setAllEventData();
    Q_INVOKABLE void updateAllEventData();

    Q_INVOKABLE void setEventData(QSet<QString> events);

    void coreServiceEventTriggered(QCoreServiceEventPtr event);

private:
    BaseTreeItemInfo *newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);
    TreeItemModel * AddChildend(TreeItemModel* parent, const ItemType& type, const QString& name);
    void addEventData(QSet<QString>& events);
    void removeEventData(QSet<QString>& events);
    void removeAll();

    TreeItemModel*                  m_treeItemModel;
    QMap<QString, TreeItemModel*>   m_treeItemModelMap;
    QSet<QString>                   m_fixedEvents;
};


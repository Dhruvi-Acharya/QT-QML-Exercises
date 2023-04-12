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
#include "BaseTreeModel.h"

#include "DragItemListModel.h"

class LayoutTreeModel : public BaseTreeModel
{
    Q_OBJECT
public:
    explicit LayoutTreeModel(QObject *parent = nullptr);
    virtual ~LayoutTreeModel();

    void coreServiceEventTriggered(QCoreServiceEventPtr event);
    void initializeTree();
    void clear();    

    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& itemId = "");
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name, const QString& modelName, const QString& deviceName, const QString& ipAddress, const int channelNumber = 0, const QString& deviceId = "", const QString& itemId = "");
    BaseTreeItemInfo* getLayoutItemInfo(const QString& layoutId);

public slots:
    void changeDisplayName(QString uuid, int itemType, QString name);
    void dropSource(const QString& parentUuid, DragItemListModel* source);
    void deleteItems(QString v);

    // from navigation tab
    void addLayoutItems(QString layoutId, QVariantList items);
    void removeLayoutItems(QString layoutId, QList<QString> itemIds);
    void updateLayoutItems(QString layoutId, QVariantList items);
    void updateLayoutProperty(QString layoutId, QVariant layoutModel);

    void tabClosedWithoutSave(QList<QString> itemIds);
    void makeNewLayout();

signals:
    void invalidate();
    void expandRoot();
    void layoutAdded();
    void sequenceInfoChanged(QString sequenceName, QString sequenceId);

private:
    void SetupCategoryTree();

    void Event_FullInfo(QCoreServiceEventPtr event);

    void Event_SaveLayout(QCoreServiceEventPtr event);
    void Event_SaveLayouts(QCoreServiceEventPtr event);
    void Event_RemoveLayout(QCoreServiceEventPtr event);

    void Event_SaveChannels(QCoreServiceEventPtr event);
    void Event_SaveWebpage(QCoreServiceEventPtr event);
    void Event_RemoveWebpage(QCoreServiceEventPtr event);

    void Event_SaveSequenceLayout(QCoreServiceEventPtr event);
    void Event_RemoveSequenceLayout(QCoreServiceEventPtr event);

    void Event_DeviceStatus(QCoreServiceEventPtr event);
    void Event_ChannelStatus(QCoreServiceEventPtr event);

    void AddTreeLayoutItem(Wisenet::Core::Layout& layout);
    void UpdateTreeLayoutItem(Wisenet::Core::Layout& layout);

    void Event_UpdateDevices(QCoreServiceEventPtr event);


    void AddCameraItem(QString layoutId, QString itemId, QString deviceId, QString channelNumber);
    void AddWebpageItem(QString layoutId, QString itemId, QString webpageId);
    void AddLocalFileItem(QString layoutId, QString itemId, QString fileUrl);

    void AddCameraItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem);
    void AddWebageItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem);
    void AddLocalFileItem(Wisenet::Core::Layout& layout, Wisenet::Core::LayoutItem& layoutItem);

    void AddTreeSequenceItem(Wisenet::Core::SequenceLayout& sequenceLayout);
    void UpdateTreeSequenceItem(Wisenet::Core::SequenceLayout& sequenceLayout);

    void Request_UpdateLayout(Wisenet::Core::Layout& layout);
    void Request_RemoveLayout(const QString& layoutId);

    void Request_UpdateSequenceLayout(Wisenet::Core::SequenceLayout& sequenceLayout);
    void Request_RemoveSequenceLayout(const QString& sequenceLayoutId);

    TreeItemModel* m_layoutRootItem;
    TreeItemModel* m_sequenceRootItem;

    QMap<QString, TreeItemModel*> m_treeItemModelMap;
    QMap<QString, QSet<QString>> m_channelItemIdMapping;

    QMap<QString, TreeItemModel*> m_sequenceTreeItemMap;
};

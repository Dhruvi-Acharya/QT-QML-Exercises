#pragma once
#include "../BaseTreeModel.h"

class DashboardGroupTreeModel : public BaseTreeModel
{
    Q_OBJECT
public:
    explicit DashboardGroupTreeModel(QObject *parent = nullptr);
    virtual ~DashboardGroupTreeModel();

    void SetupTree();
    void coreServiceEventTriggered(QCoreServiceEventPtr event);

private:
    BaseTreeItemInfo* newTreeItemInfo(const QString& uuid, const ItemType& type, const ItemStatus& status, const QString& name);

    void Event_FullInfo(QCoreServiceEventPtr event);
    void Event_SaveGroup(QCoreServiceEventPtr event);
    void Event_RemoveGroup(QCoreServiceEventPtr event);

    TreeItemModel* AddTreeGroupItem(const Wisenet::Core::Group& group);
    void UpdateTreeGroupItem(const Wisenet::Core::Group& group);

signals:
    void invalidate();
    void expandRoot();

private:
    TreeItemModel* m_rootItemModel;
    QMap<QString, TreeItemModel*> m_treeItemModelMap;
};

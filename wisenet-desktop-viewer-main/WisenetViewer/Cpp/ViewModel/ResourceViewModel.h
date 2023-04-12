#pragma once

#include <QObject>
#include <QDebug>
#include "../Model/ResourceTreeModel.h"
#include "../Model/TreeProxyModel.h"
#include "../Model/LayoutTreeModel.h"
#include "../Model/ContextMenuGroupListModel.h"
class ResourceViewModel : public QObject
{
    Q_OBJECT
public:
    explicit ResourceViewModel(QObject* parent = nullptr);
    ~ResourceViewModel();

public:
    Q_PROPERTY (ResourceTreeModel* treeModel READ treeModel NOTIFY treeModelChangedEvent)
    Q_PROPERTY (TreeProxyModel* treeProxyModel READ treeProxyModel NOTIFY treeProxyModelChangedEvent)
    Q_PROPERTY (LayoutTreeModel* layoutTreeModel READ layoutTreeModel NOTIFY layoutTreeModelChangedEvent)
    Q_PROPERTY (TreeProxyModel* layoutTreeProxyModel READ layoutTreeProxyModel NOTIFY layoutTreeProxyModelChangedEvent)

    Q_PROPERTY (ContextMenuGroupListModel* contextMenuGroupListModel READ contextMenuGroupListModel NOTIFY contextMenuGroupListModelChangedEvent)
    Q_PROPERTY (ContextMenuGroupListSortModel* contextMenuGroupListSortModel READ contextMenuGroupListSortModel NOTIFY contextMenuGroupListSortModelChangedEvent)

    ResourceTreeModel* treeModel()
    {
        return &m_resourceTreeModel;
    }

    TreeProxyModel* treeProxyModel()
    {
        return &m_treeProxyModel;
    }

    LayoutTreeModel* layoutTreeModel()
    {
        return &m_layoutTreeModel;
    }

    TreeProxyModel* layoutTreeProxyModel()
    {
        return &m_layoutTreeProxyModel;
    }

    ContextMenuGroupListModel* contextMenuGroupListModel()
    {
        return &m_contextMenuGroupListModel;
    }

    ContextMenuGroupListSortModel* contextMenuGroupListSortModel()
    {
        return &m_contextMenuGroupListSortModel;
    }

    void setSourceModel();
    void initializeTree();
    void clear();

signals:
    void treeModelChangedEvent(ResourceTreeModel* arg);
    void treeProxyModelChangedEvent(TreeProxyModel* arg);
    void layoutTreeModelChangedEvent(LayoutTreeModel* arg);
    void layoutTreeProxyModelChangedEvent(TreeProxyModel* arg);
    void contextMenuGroupListModelChangedEvent(ContextMenuGroupListModel* arg);
    void contextMenuGroupListSortModelChangedEvent(ContextMenuGroupListSortModel* arg);

private:
    ResourceTreeModel m_resourceTreeModel;
    TreeProxyModel m_treeProxyModel;

    LayoutTreeModel m_layoutTreeModel;
    TreeProxyModel m_layoutTreeProxyModel;

    ContextMenuGroupListModel m_contextMenuGroupListModel;
    ContextMenuGroupListSortModel m_contextMenuGroupListSortModel;
};

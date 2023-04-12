#pragma once

#include <QObject>
#include <QDebug>
#include "ResourceTreeModel.h"
#include "TreeProxyModel.h"

class ResourceViewModel : public QObject
{
    Q_OBJECT

public:
    explicit ResourceViewModel(QObject* parent = nullptr);
    ~ResourceViewModel();

public:
    Q_PROPERTY (ResourceTreeModel* treeModel READ treeModel NOTIFY treeModelChangedEvent)
    Q_PROPERTY (TreeProxyModel* treeProxyModel READ treeProxyModel NOTIFY treeProxyModelChangedEvent)

    ResourceTreeModel* treeModel()
    {
        return &m_resourceTreeModel;
    }

    TreeProxyModel* treeProxyModel()
    {
        return &m_treeProxyModel;
    }

    void setSourceModel();
    void initializeTree(QString filePath);
    void clear();

    Q_INVOKABLE void changeMediaFolder(QUrl folderUrl);

signals:
    void treeModelChangedEvent(ResourceTreeModel* arg);
    void treeProxyModelChangedEvent(TreeProxyModel* arg);

private:
    ResourceTreeModel m_resourceTreeModel;
    TreeProxyModel m_treeProxyModel;
};

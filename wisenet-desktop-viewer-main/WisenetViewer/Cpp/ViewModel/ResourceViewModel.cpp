#include "ResourceViewModel.h"

ResourceViewModel::ResourceViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "ResourceViewModel()";

    connect(&m_resourceTreeModel, &ResourceTreeModel::invalidate, &m_treeProxyModel, &TreeProxyModel::invalidate);
    connect(&m_layoutTreeModel, &LayoutTreeModel::invalidate, &m_layoutTreeProxyModel, &TreeProxyModel::invalidate);

    setSourceModel();
}

ResourceViewModel::~ResourceViewModel()
{
    qDebug() << "~ResourceViewModel()";
}

void ResourceViewModel::setSourceModel()
{
    m_treeProxyModel.setSourceModel(&m_resourceTreeModel);
    m_layoutTreeProxyModel.setSourceModel(&m_layoutTreeModel);

    m_treeProxyModel.doSort(Qt::SortOrder::AscendingOrder);
    m_layoutTreeProxyModel.doSort(Qt::SortOrder::AscendingOrder);

    m_contextMenuGroupListSortModel.setSourceModel(&m_contextMenuGroupListModel);
    m_contextMenuGroupListSortModel.doSort(Qt::SortOrder::AscendingOrder);
}

void ResourceViewModel::initializeTree()
{
    m_resourceTreeModel.initializeTree();
    m_layoutTreeModel.initializeTree();
}

void ResourceViewModel::clear()
{
    qDebug() << "ResourceViewModel::clear()";
    m_resourceTreeModel.clear();
    m_layoutTreeModel.clear();
}

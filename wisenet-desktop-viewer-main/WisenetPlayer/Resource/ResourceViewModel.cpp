#include "ResourceViewModel.h"
#include "FilePathConveter.h"

ResourceViewModel::ResourceViewModel(QObject *parent) : QObject(parent)
{
    qDebug() << "ResourceViewModel()";

    connect(&m_resourceTreeModel, &ResourceTreeModel::invalidate, &m_treeProxyModel, &TreeProxyModel::invalidate);

    setSourceModel();
}

ResourceViewModel::~ResourceViewModel()
{
    qDebug() << "~ResourceViewModel()";
}

void ResourceViewModel::setSourceModel()
{
    m_treeProxyModel.setSourceModel(&m_resourceTreeModel);
    m_treeProxyModel.doSort(Qt::SortOrder::AscendingOrder);
}

void ResourceViewModel::initializeTree(QString filePath)
{
    m_resourceTreeModel.initializeTree(filePath);
}

void ResourceViewModel::clear()
{
    qDebug() << "ResourceViewModel::clear()";
    m_resourceTreeModel.clear();
}

void ResourceViewModel::changeMediaFolder(QUrl folderUrl)
{
    initializeTree(FilePathConveter::getInstance()->getPathByUrl(folderUrl));
}

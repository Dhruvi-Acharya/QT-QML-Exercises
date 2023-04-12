#include "LayoutTreeItemInfo.h"

LayoutTreeItemInfo::LayoutTreeItemInfo(QObject *parent) : BaseTreeItemInfo(parent)
{
    connect(&m_MediaLayoutViewModel, &MediaLayoutViewModel::saveStatusChanged, this, &LayoutTreeItemInfo::saveStatusChanged);
}

LayoutTreeItemInfo::LayoutTreeItemInfo(const LayoutTreeItemInfo &other) : BaseTreeItemInfo(other)
{
}

LayoutTreeItemInfo::~LayoutTreeItemInfo()
{
    //qDebug() << "LayoutTreeItemInfo::~LayoutTreeItemInfo()";
}

void LayoutTreeItemInfo::setLayout(Wisenet::Core::Layout &layout)
{
    m_MediaLayoutViewModel.loadFromCoreService(layout);
}

void LayoutTreeItemInfo::addCamera(const QString &deviceId, const QString &channelId)
{
    m_MediaLayoutViewModel.addCameraItem(deviceId, channelId);
}

void LayoutTreeItemInfo::addWebpage(const QString &webpage)
{
    m_MediaLayoutViewModel.addWebpageItem(webpage);
}

void LayoutTreeItemInfo::addLocalfile(const QString &fileUrl)
{
    qDebug() << "LayoutTreeItemInfo::addLocalfile() " << fileUrl;

    if(!m_MediaLayoutViewModel.isAcceptableUrl(fileUrl))
    {
        qDebug() << "LayoutTreeItemInfo::addLocalfile() isAcceptableUrl == false" << fileUrl;
        return;
    }

    if(m_MediaLayoutViewModel.isVideoFile(fileUrl))
        m_MediaLayoutViewModel.addLocalVideoItem(fileUrl);
    else
        m_MediaLayoutViewModel.addLocalImageItem(fileUrl);
}

void LayoutTreeItemInfo::removeItem(const QString &itemId)
{
    m_MediaLayoutViewModel.removeItem(itemId);
}

void LayoutTreeItemInfo::saveLayout()
{
    qDebug() << "TreeItemInfo::saveLayout()";
    m_MediaLayoutViewModel.saveToCoreService();
}

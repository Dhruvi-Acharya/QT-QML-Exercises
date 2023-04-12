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
#include "MainTreeItemBase.h"

MainTreeItemBase::MainTreeItemBase(const QString& uuid,
                                   const TreeItemInfo& itemInfo,
                                   MainTreeItemBase *parent)
    : m_parentItem(parent)
    , m_uuid(uuid)
    , m_itemInfo(itemInfo)
{

}

MainTreeItemBase::~MainTreeItemBase()
{
    qDeleteAll(m_childItems);
}


MainTreeItemBase *MainTreeItemBase::child(int index)
{
    if (index < 0 || index >= m_childItems.size())
        return nullptr;
    return m_childItems.at(index);
}

int MainTreeItemBase::childCount() const
{
    return m_childItems.count();
}

int MainTreeItemBase::IndexOfParent() const
{
    if (m_parentItem)
        return m_parentItem->m_childItems.indexOf(const_cast<MainTreeItemBase*>(this));
    return 0;
}

bool MainTreeItemBase::insertChild(int position, MainTreeItemBase* child)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    m_childItems.insert(position, child);

    return true;
}

bool MainTreeItemBase::pushBackChild(MainTreeItemBase* child)
{
    m_childItems.push_back(child);
    return true;
}

MainTreeItemBase *MainTreeItemBase::parent()
{
    return m_parentItem;
}

void MainTreeItemBase::setData(const TreeItemInfo& itemInfo)
{
    m_itemInfo = itemInfo;
}

bool MainTreeItemBase::removeChild(int position)
{
    if (position < 0 || position > m_childItems.size())
        return false;

    delete m_childItems.takeAt(position);

    return true;
}

TreeItemInfo MainTreeItemBase::getData()
{
    return m_itemInfo;
}

void MainTreeItemBase::removeAllChild()
{
    m_childItems.clear();
}

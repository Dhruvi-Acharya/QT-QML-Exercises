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
#include "MainTreeModel.h"
#include <QUuid>
#include <QDebug>

MainTreeModel::MainTreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    TreeItemInfo rootInfo {TreeItemInfo::MainRoot, TreeItemInfo::Normal};
    rootInfo .SetNames("Root");

    m_rootItem = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), rootInfo);
    SetupCategoryTree();
}

MainTreeModel::~MainTreeModel()
{
    delete m_rootItem;
}


QModelIndex MainTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    MainTreeItemBase *parentItem = getItem(parent);
    if (!parentItem)
        return QModelIndex();

    MainTreeItemBase *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    return QModelIndex();
}

QModelIndex MainTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    MainTreeItemBase *childItem = getItem(index);
    MainTreeItemBase *parentItem = childItem ? childItem->parent() : nullptr;

    if (parentItem == m_rootItem || !parentItem)
        return QModelIndex();

    return createIndex(parentItem->IndexOfParent(), 0, parentItem);
}

int MainTreeModel::rowCount(const QModelIndex &parent) const
{
    const MainTreeItemBase *parentItem = getItem(parent);
    return parentItem ? parentItem->childCount() : 0;
}

int MainTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return 1;
}

QVariant MainTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        qDebug() << "***********invalid index-------------";
        return QVariant();
    }

    //if (role != Qt::DisplayRole && role != Qt::EditRole) {
    if (role != Qt::DisplayRole) {
        qDebug() << "------------invalid role-------------";
        return QVariant();
    }

    MainTreeItemBase *item = getItem(index);

    QVariant v;
    v.setValue(item->getData());

    return v;
}

MainTreeItemBase *MainTreeModel::getItem(const QModelIndex &index) const
{
    if (index.isValid()) {
        MainTreeItemBase *item = static_cast<MainTreeItemBase*>(index.internalPointer());
        if (item)
            return item;
    }
    return m_rootItem;
}

Qt::ItemFlags MainTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;

    return QAbstractItemModel::flags(index);
    //return Qt::ItemIsEditable | QAbstractItemModel::flags(index);
}


void MainTreeModel::SetupCategoryTree()
{

    TreeItemInfo info {TreeItemInfo::DeviceRoot, TreeItemInfo::Normal};
    info.SetNames("Device");
    auto deviceRoot = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::LayoutRoot); info.SetNames("Layout");
    auto layoutRoot = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::GroupRoot); info.SetNames("Group");
    auto groupRoot = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::UserRoot); info.SetNames("User");
    auto userRoot = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);

    info.SetType(TreeItemInfo::User); info.SetNames("Admin");
    userRoot->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));

    for (int i = 0 ; i < 3 ; i ++) {
        info.SetType(TreeItemInfo::Recorder);
        info.SetNames("NVR-0"+QString::number(i+1), "PRP-7777", "HELLO NVR1", "192.168.10.1");
        info.SetStatus(TreeItemInfo::Loading);
        deviceRoot->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));
    }
    for (int i = 0 ; i < 5 ; i ++) {
        info.SetType(TreeItemInfo::Recorder);
        info.SetNames("NVR-1"+QString::number(i+1), "XRP-1111", "HELLO NVR2", "192.168.50.1");
        info.SetStatus(TreeItemInfo::Normal);
        auto nvr = new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info);
        deviceRoot->pushBackChild(nvr);
        for (int j = 0 ; j < 32 ; j++) {
            info.SetType(TreeItemInfo::Channel);
            info.SetNames("CHANNEL"+QString::number(i+1), "XRP-1111-CHANNEL", "HELLO NVR2 CH");
            info.SetStatus(TreeItemInfo::Abnormal);
            nvr->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));
        }
    }
    for (int i = 0 ; i < 10 ; i ++) {
        info.SetType(TreeItemInfo::Camera);
        info.SetNames("CAM-0"+QString::number(i+1), "XNP-8888", "HELLO CAM", "192.168.100.1");
        info.SetStatus(TreeItemInfo::Normal);
        deviceRoot->pushBackChild(new MainTreeItemBase(QUuid::createUuid().toString(QUuid::WithoutBraces), info));
    }

    m_rootItem->pushBackChild(deviceRoot);
    m_rootItem->pushBackChild(layoutRoot);
    m_rootItem->pushBackChild(groupRoot);
    m_rootItem->pushBackChild(userRoot);
}

QHash<int,QByteArray> MainTreeModel::roleNames() const
{
    auto a = QAbstractItemModel::roleNames();
    return {
        //{Qt::UserRole + 1, "deviceTree"},
        {Qt::DisplayRole, "display"}//,
//        {roleDataType, "dataType"},
//        {roleView, "view"},
//        {roleCommand, "command"},
//        {roleHighlight, "highlight"},
//        {roleVisible, "visible"},
//        {roleRef1, "ref1"}
    };
}

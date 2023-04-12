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
#include "MainTreeSortFilterProxyModel.h"
#include "MainTreeItemBase.h"
#include "QCoreServiceManager.h"
#include <QDebug>
MainTreeSortFilterProxyModel::MainTreeSortFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

void MainTreeSortFilterProxyModel::setFilterText(const QString &value)
{
    qDebug() << "setfilter text::" << value;
    setFilterRegExp(QRegExp(value, Qt::CaseInsensitive, QRegExp::FixedString));
}

void MainTreeSortFilterProxyModel::activateModel(QVariant item)
{
    qDebug() << "MainTreeSortFilterProxyModel::activateModel()" << item;
    if (item.isValid()) {
        auto treeItem = item.value<TreeItemInfo>();
        if (treeItem.deviceUUID().length() > 0 && treeItem.channelNumber() > 0) {
            qDebug() << "deviceUUID=" << treeItem.deviceUUID() << ", channelIndex=" << treeItem.channelNumber();
//            QMediaRequestEventPtr qMediaReq(new QMediaRequestEvent);
//            qMediaReq->isOpen = true;
//            qMediaReq->deviceUUID = treeItem.deviceUUID();
//            qMediaReq->channelNumber = treeItem.channelNumber();
//            QCoreServiceManager::Instance().RequestMedia(qMediaReq);
        }
    }

    //auto data = sourceModel()->data(itemIndex);
//    if (data.isValid()) {
//    auto itemInfo = data.value<TreeItemInfo>();

//    qDebug() << "modelName:" << itemInfo.modelName();
//    }
}

bool MainTreeSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    //qDebug() << "filterAcceptsRow::" << sourceRow;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);
    auto data = sourceModel()->data(index);
    auto itemInfo = data.value<TreeItemInfo>();

    qDebug() << "modelName:" << itemInfo.modelName();
    return itemInfo.modelName().contains(filterRegExp());
}

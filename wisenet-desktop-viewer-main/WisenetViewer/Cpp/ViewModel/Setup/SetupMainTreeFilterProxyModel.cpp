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
#include "SetupMainTreeFilterProxyModel.h"
#include <QDebug>
SetupMainTreeFilterProxyModel::SetupMainTreeFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    qDebug()<< " SetupMainTreeFilterProxyModel";
    setRecursiveFilteringEnabled(true);
}

SetupMainTreeFilterProxyModel::~SetupMainTreeFilterProxyModel()
{
    qDebug()<< " ~SetupMainTreeFilterProxyModel";

}

void SetupMainTreeFilterProxyModel::setFilterText(const QString &value)
{
    setFilterRegExp(QRegExp(value, Qt::CaseInsensitive, QRegExp::RegExp));
}

bool SetupMainTreeFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QRegExp rx = filterRegExp();

    if (rx.isEmpty())
        return true;

    QModelIndex index = sourceModel()->index(sourceRow, 0, sourceParent);

    if(!index.isValid())
        return false;

    QAbstractItemModel *model = sourceModel();
    QObject *object = qvariant_cast<QObject*>(model->data(index));
    SetupTreeNode *data = qobject_cast<SetupTreeNode*>(object);

    //parent이면 무조건 false
    if(data->property("status") == 0)
        return false;

    QString display = data->property("display").toString();

    return display.contains(rx);
}

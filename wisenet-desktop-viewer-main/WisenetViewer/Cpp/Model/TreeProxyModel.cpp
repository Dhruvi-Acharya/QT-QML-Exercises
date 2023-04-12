#include "TreeProxyModel.h"
#include "WisenetViewerDefine.h"

TreeProxyModel::TreeProxyModel(QObject *parent) : QSortFilterProxyModel(parent)
{
    //qDebug() << "TreeProxyModel()";
    setRecursiveFilteringEnabled(true);
}

TreeProxyModel::~TreeProxyModel()
{
    //qDebug() << "~TreeProxyModel()";
}

void TreeProxyModel::setFilterText(const QString &value)
{
    qDebug() << "setfilter text: " << value;
    m_tempText = value;
    setFilterRegExp(QRegExp(value, Qt::CaseInsensitive, QRegExp::FixedString));

    emit filterTextChanged();
}

bool TreeProxyModel::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    //qDebug() << "lessThan called";
    QVariant leftData = sourceModel()->data(left, 0x100+1);
    QVariant rightData = sourceModel()->data(right, 0x100+1);

    BaseTreeItemInfo* leftTreeItem = leftData.value<BaseTreeItemInfo*>();
    BaseTreeItemInfo* rightTreeItem = rightData.value<BaseTreeItemInfo*>();

    if(leftTreeItem == nullptr)
    {
        qDebug() << "leftTreeItem == nullptr";
        return false;
    }

    if(rightTreeItem == nullptr)
    {
        qDebug() << "rightTreeItem == nullptr";
        return true;
    }

    if(leftTreeItem->itemType() == (int)ItemType::LocalDirectory && rightTreeItem->itemType() == (int)ItemType::LocalFile)
        return true;

    if(rightTreeItem->itemType() == (int)ItemType::LocalDirectory && leftTreeItem->itemType() == (int)ItemType::LocalFile)
        return false;

    // Root item은 sorting하지 않음.
    if(leftTreeItem->itemType() == (int)ItemType::ResourceRoot ||
            leftTreeItem->itemType() == (int)ItemType::LayoutRoot ||
            leftTreeItem->itemType() == (int)ItemType::SequenceRoot ||
            leftTreeItem->itemType() == (int)ItemType::SequenceItem ||
            leftTreeItem->itemType() == (int)ItemType::WebPageRoot ||
            leftTreeItem->itemType() == (int)ItemType::UserRoot ||
            leftTreeItem->itemType() == (int)ItemType::LocalFileRoot)
        return false;

    if(rightTreeItem->itemType() == (int)ItemType::ResourceRoot ||
            rightTreeItem->itemType() == (int)ItemType::LayoutRoot ||
            rightTreeItem->itemType() == (int)ItemType::SequenceRoot ||
            rightTreeItem->itemType() == (int)ItemType::SequenceItem ||
            rightTreeItem->itemType() == (int)ItemType::WebPageRoot ||
            rightTreeItem->itemType() == (int)ItemType::UserRoot ||
            rightTreeItem->itemType() == (int)ItemType::LocalFileRoot)
        return false;

    int compare = QString::localeAwareCompare(leftTreeItem->displayName(), rightTreeItem->displayName());
    bool ret = (compare < 0);
    //qDebug() << "lessThan " << leftTreeItem->displayName() << " - " << rightTreeItem->displayName() << ", compare " << compare <<  ", ret" << ret;

    return ret;
}

bool TreeProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    QModelIndex sourceIndex = sourceModel()->index(sourceRow, 0, sourceParent);
    auto data = sourceModel()->data(sourceIndex,  Qt::UserRole + 1);
    auto itemInfo = data.value<BaseTreeItemInfo*>();

    if(!itemInfo)
        return false;

    bool ret = itemInfo->displayName().contains(filterRegExp());

    //qDebug() << "DisplayName: " << itemInfo->displayName() << " ret : " << ret;

    if(m_tempText != ""){
        //qDebug() << "DisplayName: " << itemInfo->displayName() << " ret : " << ret << " tempText : " << m_tempText;
        if(itemInfo->itemType() == (int)ItemType::ResourceRoot ||
                itemInfo->itemType() == (int)ItemType::LayoutRoot ||
                itemInfo->itemType() == (int)ItemType::SequenceRoot ||
                itemInfo->itemType() == (int)ItemType::SequenceItem ||
                itemInfo->itemType() == (int)ItemType::WebPageRoot ||
                itemInfo->itemType() == (int)ItemType::UserRoot ||
                itemInfo->itemType() == (int)ItemType::LocalFileRoot)
            return false;
    }

    return ret;
}

void TreeProxyModel::doSort(Qt::SortOrder order)
{
    qDebug() << "doSort " << order;
    sort(0, order);
}

#include "DragItemListModel.h"

DragItemListModel::DragItemListModel(QObject *parent) : QObject(parent)
{

}

DragItemListModel::~DragItemListModel()
{
    qDebug() << "~DragItemListModel()";
    removeAll();
}

void DragItemListModel::append(QString uuid, int type)
{
    qDebug() << "DragItemListModel::append:" << uuid << ", type:" << type;
    DragItemModel* model = new DragItemModel();

    model->setUuid(uuid);
    //model->setChannelId(channelId);
    model->setItemType(type);

    m_itemList.append(model);
}

void DragItemListModel::removeAll()
{
    if(m_itemList.count() == 0)
        return;

    qDeleteAll(m_itemList);
    m_itemList.clear();
}

QList<DragItemModel*> DragItemListModel::itemList()
{
    return m_itemList;
}

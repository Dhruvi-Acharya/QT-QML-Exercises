#include "LayoutTabModel.h"
#include "WisenetViewerDefine.h"

LayoutTabModel::LayoutTabModel(QObject *parent) : QObject(parent)
{
    m_itemType = (int)WisenetViewerDefine::ItemType::Layout;
    qDebug() << "LayoutTabModel()";
}

LayoutTabModel::~LayoutTabModel()
{
    qDebug() << "~LayoutTabModel()";
}

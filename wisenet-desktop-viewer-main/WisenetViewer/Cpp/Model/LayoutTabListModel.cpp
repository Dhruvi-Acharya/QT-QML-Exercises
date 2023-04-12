#include "LayoutTabListModel.h"


LayoutTabListModel::LayoutTabListModel(QObject* parent)
{
    Q_UNUSED(parent);
    qDebug() << "LayoutTabListModel::LayoutTabListModel()";
}

LayoutTabListModel::~LayoutTabListModel()
{
    qDebug() << "LayoutTabListModel::~LayoutTabModel()";
}

int LayoutTabListModel::rowCount(const QModelIndex& p) const
{
    Q_UNUSED(p)
    return m_data.size();
}

QHash<int, QByteArray> LayoutTabListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[ItemTypeRole] = "itemType";
    roles[NameRole] = "tabText";
    roles[UuidRole] = "uuid";
    return roles;
}

QVariant LayoutTabListModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role)

    QVariant value;

    switch (role)
    {
    case ItemTypeRole:
        value = m_data[index.row()]->itemType();
        break;
    case NameRole:
        value = m_data[index.row()]->text();
        break;
    case UuidRole:
        value = m_data[index.row()]->uuid();
        break;
    default:
        break;
    }

    return value;
}

bool LayoutTabListModel::setData(const QModelIndex &idx, const QVariant &value, int role)
{
    qDebug() << "LayoutTabListModel::setData()";
    int row = idx.row();
    if(row < 0 || row >= m_data.count())
        return false;

    LayoutTabModel* item = m_data.at(row);

    switch(role)
    {
    case ItemTypeRole:
    {
        qDebug() << "LayoutTabListModel::setData ItemTypeRole " << value.toString();
        item->setItemType(value.toInt());
        return true;
    }
    case NameRole:
    {
        qDebug() << "LayoutTabListModel::setData NameRole " << value.toString();
        item->setText(value.toString());
        return true;
    }
    case UuidRole:
    {
        qDebug() << "LayoutTabListModel::setData UuidRole " << value.toString();
        item->setUuid(value.toString());
        return true;
    }
    default:
        break;
    }

    return false;
}

int LayoutTabListModel::count() const
{
    return this->m_data.count();
}

void LayoutTabListModel::append(LayoutTabModel* model)
{
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.append(model);
    this->m_dataMap.insert(model->uuid(), model);
    endInsertRows();
}

void LayoutTabListModel::insert(LayoutTabModel* model, int i)
{
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.insert(i, model);
    this->m_dataMap.insert(model->uuid(), model);
    endInsertRows();
}

void LayoutTabListModel::remove(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    LayoutTabModel* model = this->m_data.takeAt(idx);
    this->m_dataMap.remove(model->uuid());
    delete model;
    endRemoveRows();
}

void LayoutTabListModel::moveTab(int idx, int where)
{
    qDebug() << "LayoutTabListModel::moveTab() idx" << idx << " where" << where;
    //beginMoveRows(QModelIndex(), idx, idx, QModelIndex(), where);
    //beginResetModel();
    //this->m_data.move(idx, where);
    //endResetModel();
    //endMoveRows();
}

void LayoutTabListModel::closeTab(QString layoutId)
{
    int index = -1;
    for(int i=0; i<m_data.count(); i++)
    {
        if(m_data[i]->uuid() == layoutId)
        {
            index = i;
            break;
        }
    }

    if(index != -1)
        this->remove(index);
    else
    {
        qDebug() << "LayoutTabListModel::closeTab() index error " << index;
    }

}

void LayoutTabListModel::closeAll()
{
    beginResetModel();

    while (!this->m_data.isEmpty())
    {
        delete this->m_data.takeFirst();
    }

    endResetModel();
}

QList<QString> LayoutTabListModel::closeAllButThis(QString layoutId)
{
    QList<QString> closedLayout;

    qDebug() << "LayoutTabListModel::closeAllButThis() " << layoutId;

    for(int i=this->m_data.count()-1; i >= 0; i--)
    {
        if(this->m_data[i]->uuid() != layoutId)
        {
            closedLayout.append(this->m_data[i]->uuid());
            remove(i);
        }
    }

    return closedLayout;
}

bool LayoutTabListModel::contains(QString uuid)
{
    return m_dataMap.contains(uuid);
}

void LayoutTabListModel::updateLayout(QString uuid, QString name)
{
    LayoutTabModel* model = m_dataMap[uuid];
    model->setText(name);
}

QList<QString> LayoutTabListModel::getLayoutIdList()
{
    QList<QString> idList;

    for(int i=0; i<m_data.count(); i++)
    {
        idList.append(m_data[i]->uuid());
    }

    return idList;
}

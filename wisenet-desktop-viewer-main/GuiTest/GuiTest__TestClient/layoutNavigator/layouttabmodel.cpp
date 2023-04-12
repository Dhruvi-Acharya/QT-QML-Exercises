#include "layouttabmodel.h"


LayoutTabModel::LayoutTabModel(QObject* parent)
{
    Q_UNUSED(parent);
}

LayoutTabModel::~LayoutTabModel()
{
    qDebug() << "qml: ~LayoutTabModel()";
}

int LayoutTabModel::rowCount(const QModelIndex& p) const
{
    Q_UNUSED(p)
    return m_data.size();
}

QHash<int, QByteArray> LayoutTabModel::roleNames() const
{
    static QHash<int, QByteArray> roles;
    roles[NameRole] = "tabText";
    return roles;
}

QVariant LayoutTabModel::data(const QModelIndex& index, int role) const
{
    Q_UNUSED(role)

    QVariant value;

    switch (role)
    {
    case NameRole:
        value = m_data[index.row()]->property("tabText");
        break;
    default:
        break;
    }

    return value;
}

int LayoutTabModel::count() const
{
    return this->m_data.count();
}

void LayoutTabModel::append(QObject* o)
{
    int i = m_data.size();
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.push_back(o);
    endInsertRows();
}

void LayoutTabModel::insert(QObject* o, int i)
{
    beginInsertRows(QModelIndex(), i, i);
    this->m_data.insert(i, o);
    endInsertRows();
}

void LayoutTabModel::remove(int idx)
{
    beginRemoveRows(QModelIndex(), idx, idx);
    delete this->m_data.takeAt(idx);
    endRemoveRows();
}

void LayoutTabModel::closeTab(QString tabName)
{
    int index = -1;
    for(int i=0; i<m_data.count(); i++)
    {
        if(m_data[i]->property("tabText") == tabName)
        {
            index = i;
            break;
        }
    }

    if(index != -1)
        this->remove(index);

}

void LayoutTabModel::closeAllButThis(QString tabName)
{
    QObject* saveData;

    beginResetModel();

    while (!this->m_data.isEmpty())
    {
        if(this->m_data[0]->property("tabText") == tabName)
        {
            saveData = this->m_data.takeFirst();
        }
        else
        {
            delete this->m_data.takeFirst();
        }
    }

    if(saveData != nullptr)
        m_data.append(saveData);

    endResetModel();
}

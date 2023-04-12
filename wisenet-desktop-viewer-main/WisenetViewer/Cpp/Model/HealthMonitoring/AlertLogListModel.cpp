#include "AlertLogListModel.h"

AlertLogListModel::AlertLogListModel(QObject *parent) : QAbstractListModel(parent)
{
}

AlertLogListModel::~AlertLogListModel()
{
}

int AlertLogListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data.count();
}

QHash<int, QByteArray> AlertLogListModel::roleNames() const
{
    static QHash<int, QByteArray> roles;

    roles[UuidRole] = "uuid";
    roles[ObjectTypeRole] = "objectType";
    roles[OccurrenceTimeRole] = "occurrenceTime";
    roles[ErrorTypeRole] = "errorType";
    roles[ErrorMessageRole] = "errorMessage";
    roles[RecommendedActionRole] = "recommendedAction";

    return roles;
}

QVariant AlertLogListModel::data(const QModelIndex &index, int role) const
{
    QVariant value;

    switch (role)
    {
    case UuidRole:
        value = m_data[index.row()]->uuid();
        break;
    case ObjectTypeRole:
        value = m_data[index.row()]->objectType();
        break;
    case OccurrenceTimeRole:
        value = m_data[index.row()]->occurrenceTime();
        break;
    case ErrorTypeRole:
        value = m_data[index.row()]->errorType();
        break;
    case ErrorMessageRole:
        value = m_data[index.row()]->errorMessage();
        break;
    case RecommendedActionRole:
        value = m_data[index.row()]->recommendedAction();
        break;
    default:
        break;
    }

    return value;
}

#pragma once
#include <QObject>
#include <QAbstractListModel>
#include "AlertLogModel.h"

class AlertLogListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        UuidRole = Qt::UserRole + 1,
        ObjectTypeRole,
        OccurrenceTimeRole,
        ErrorTypeRole,
        ErrorMessageRole,
        RecommendedActionRole,
    };

    explicit AlertLogListModel(QObject *parent = nullptr);
    virtual ~AlertLogListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QList<AlertLogModel*> m_data;
};

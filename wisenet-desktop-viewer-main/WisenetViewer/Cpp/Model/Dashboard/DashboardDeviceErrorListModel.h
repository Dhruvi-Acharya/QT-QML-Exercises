#pragma once
#include <QAbstractListModel>
#include "DashboardErrorListModel.h"
#include "DashboardErrorListSortProxyModel.h"

class DashboardDeviceErrorListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        TitleRole = Qt::UserRole + 1,
        ListDataRole,
    };

    explicit DashboardDeviceErrorListModel(QObject *parent = nullptr);
    virtual ~DashboardDeviceErrorListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool contains(const QString& title);
    void appendEventType(const QString& title);
    DashboardErrorListModel* getSource(const QString& title);
    DashboardErrorListSortProxyModel* get(const QString& title);
    void remove(const QString& title);

    void appendDevice(const QString& eventKey, ErrorItem* item);
    void removeDevice(const QString& deviceId);
    int deviceCount();

private:
    QMap<QString, DashboardErrorListSortProxyModel*> m_data;
};

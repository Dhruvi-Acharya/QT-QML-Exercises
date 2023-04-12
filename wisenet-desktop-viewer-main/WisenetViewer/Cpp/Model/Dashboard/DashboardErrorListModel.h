#pragma once
#include <QAbstractListModel>

struct ErrorItem
{
    QString objectId = "";
    int objectType = 0;
    QString objectName = "";
};

class DashboardErrorListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum ModelRoles {
        ObjectIdRole = Qt::UserRole + 1,
        ObjectTypeRole,
        ObjectNameRole,
    };

    explicit DashboardErrorListModel(QObject *parent = nullptr);
    ~DashboardErrorListModel();

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QHash<int, QByteArray> roleNames() const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    bool contains(const QString& objectId);
    void append(ErrorItem* model);
    void remove(const QString& objectId);    

    void removeWithDeviceId(const QString& deviceId);

    QMap<QString, ErrorItem*> getData(){return m_data;}

private:
    QMap<QString, ErrorItem*> m_data;
};

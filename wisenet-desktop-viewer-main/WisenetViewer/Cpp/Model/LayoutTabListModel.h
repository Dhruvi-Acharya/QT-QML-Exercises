#pragma once
#include <QAbstractListModel>
#include <QDebug>
#include "LayoutTabModel.h"

class LayoutTabListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ModelRoles {
        ItemTypeRole = Qt::UserRole + 1,
        NameRole,
        UuidRole,
    };

    explicit LayoutTabListModel(QObject* parent = nullptr);
    ~LayoutTabListModel();

    int rowCount(const QModelIndex& p) const;
    QVariant data(const QModelIndex& index, int role) const;
    Q_INVOKABLE  bool setData(const QModelIndex &idx, const QVariant &value, int role = Qt::EditRole) override;
    QHash<int, QByteArray> roleNames() const;

    int count() const;

    bool contains(QString uuid);
    void updateLayout(QString uuid, QString name);
    QList<QString> getLayoutIdList();
    Q_INVOKABLE QList<QString> closeAllButThis(QString tabName);

public slots:
    void append(LayoutTabModel* o);
    void insert(LayoutTabModel* o, int i);
    void remove(int idx);
    void moveTab(int idx, int where);
    void closeTab(QString layoutId);
    void closeAll();

signals:
    void countChanged(int count);

private:
    QList<LayoutTabModel*> m_data;

    QMap<QString, LayoutTabModel*> m_dataMap;
};

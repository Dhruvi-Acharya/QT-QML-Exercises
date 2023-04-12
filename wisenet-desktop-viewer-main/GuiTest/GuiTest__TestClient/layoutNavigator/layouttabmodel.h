#ifndef LAYOUTTABMODEL_H
#define LAYOUTTABMODEL_H
#pragma once
#include <QAbstractListModel>
#include <QDebug>

class LayoutTabModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    enum ModelRoles {
        NameRole = Qt::UserRole + 1,
    };

    explicit LayoutTabModel(QObject* parent = nullptr);
    ~LayoutTabModel();

    int rowCount(const QModelIndex& p) const;
    QVariant data(const QModelIndex& index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    int count() const;

public slots:
    void append(QObject* o);
    void insert(QObject* o, int i);
    void remove(int idx);
    void closeTab(QString tabName);
    void closeAllButThis(QString tabName);

signals:
    void countChanged(int count);

private:
    QList<QObject*> m_data;
};

#endif // LAYOUTTABMODEL_H

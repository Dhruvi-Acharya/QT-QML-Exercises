#pragma once
#include <QObject>
#include "DragItemModel.h"
#include <QVector>
#include <QDebug>

class DragItemListModel : public QObject
{
    Q_OBJECT
public:
    explicit DragItemListModel(QObject *parent = nullptr);
    ~DragItemListModel();

public slots:
    void append(QString uuid, int type);
    void removeAll();
    QList<DragItemModel*> itemList();

private:
    QList<DragItemModel*> m_itemList;
};

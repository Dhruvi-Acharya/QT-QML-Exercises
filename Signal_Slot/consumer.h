#ifndef CONSUMER_H
#define CONSUMER_H

#include <QObject>
#include <QDebug>
#include <QTime>
#include <QSharedPointer>
#include "myclass.h"

class Consumer : public QObject
{
    Q_OBJECT
public:
    explicit Consumer(QObject *parent = nullptr);

signals:

public slots:
    void readyProduct(QSharedPointer<MyClass> ptr);
};

#endif // CONSUMER_H

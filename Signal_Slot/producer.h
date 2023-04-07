#ifndef PRODUCER_H
#define PRODUCER_H

#include <QObject>
#include <QDebug>
#include <QTimer>
#include <QSharedPointer>
#include "myclass.h"

class Producer : public QObject
{
    Q_OBJECT
public:
    explicit Producer(QObject *parent = nullptr);

public slots:
    void start();
    void stop();

private slots:
    void produce();

signals:
    void readyProduct(QSharedPointer<MyClass> ptr);

private:
    QTimer time;
};

#endif // PRODUCER_H

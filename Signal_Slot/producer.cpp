#include "producer.h"

Producer::Producer(QObject *parent)
    : QObject{parent}
{
    time.setInterval(3000);
    QObject::connect(&time,&QTimer::timeout,this,&Producer::produce);
}

void Producer::start()
{
    qInfo() << this << "Producing start";
    time.start();
}

void Producer::stop()
{
    qInfo() << this << "Producing Stop";
    time.stop();
}

void Producer::produce()
{
    qInfo() << this << "Producing item";
    QSharedPointer<MyClass> ptr(new MyClass());
    emit readyProduct(ptr);
    qInfo() << this << "Item Produced";

}

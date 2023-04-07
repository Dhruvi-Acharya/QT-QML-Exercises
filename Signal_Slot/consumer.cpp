#include "consumer.h"

Consumer::Consumer(QObject *parent)
    : QObject{parent}
{

}

void Consumer::readyProduct(QSharedPointer<MyClass> ptr)
{
    qInfo() << this << "Consumer is working";
    ptr->test(QTime::currentTime().toString());
    qInfo() << this << "Consumer work is done";

}

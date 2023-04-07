#include "myclass.h"

MyClass::MyClass(QObject *parent)
    : QThread{parent}
{
    qInfo() << this << " created on " << QThread::currentThread();
}

MyClass::~MyClass()
{
    qInfo() << this << " destroyed on " << QThread::currentThread();
}


void MyClass::run()
{
    qInfo() << this <<" run on " << currentThread();
    currentThread()->msleep(1000);
    qInfo() << this << " stop on " << QThread::currentThread();
}

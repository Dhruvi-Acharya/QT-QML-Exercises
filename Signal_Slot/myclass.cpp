#include "myclass.h"

MyClass::MyClass(QObject *parent)
    : QObject{parent}
{
    qInfo() << this << " Constructed";
}

MyClass::~MyClass()
{
    qInfo() << this << " Destructed";
}

void MyClass::test(QString str)
{
    qInfo() << this << str;
}

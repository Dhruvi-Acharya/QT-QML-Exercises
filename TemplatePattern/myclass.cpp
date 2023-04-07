#include "myclass.h"

MyClass::MyClass(QObject *parent)
    : QObject{parent}
{
    qInfo() << this << "Constructed";
}

MyClass::~MyClass()
{
    qInfo() << this << "Destructed";
}

int MyClass::getValue() const
{
    return value;
}

void MyClass::setValue(int newValue)
{
    value = newValue;
}

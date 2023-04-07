/*
 * program that shows template design pattern
 * add two object value with using add template function
 */

#include <QCoreApplication>
#include <QDebug>
#include "myclass.h"

template <typename T>
T add(T value1,T value2)
{
    return value1+value2;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << add(3,5);

    MyClass m1;
    m1.setValue(3);
    qInfo() << m1.getValue();

    MyClass m2;
    m2.setValue(5);

    MyClass m3;
    m3.setValue(add(m1.getValue(),m2.getValue()));
    qInfo() << m3.getValue();

//    m3=add(m1,m2); // this will not work

    MyClass m4;
    qInfo() << m4.add2(3,2);
    qInfo() << m4.add2(12.3,23.4);

    //qInfo() << m4.add2(m1,m2); // this will also not work, copy constructor error

    return a.exec();
}

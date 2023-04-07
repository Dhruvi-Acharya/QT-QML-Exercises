#include <QCoreApplication>
#include <QDebug>

template<typename T>
void print(T value)
{
    qInfo() << value;
}

template<class T, class F>
T add(T value1, F value2)
{
    return value1+value2;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    print<int>(10);
    print(10.23); // it also work without given type <double>
    print("Hello");
    print<QString>(QString("world"));

    qInfo() << add<int,double>(10.5,2.3);

    QObject obj;
    print<QObject *>(&obj);

    return a.exec();
}

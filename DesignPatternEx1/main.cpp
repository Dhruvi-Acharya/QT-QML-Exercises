#include <QCoreApplication>
#include <QDebug>
#include "vehicle.h"
#include "car.h"
#include "truck.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    qInfo() << "Welcome";

    Vehicle v;
    v = Car();
    v = Truck();

    return a.exec();
}

#include <QCoreApplication>
#include <QDebug>
#include "memento.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Memento controller;

    controller.addTransaction("Hello");
    controller.addTransaction("World");
    controller.commit();

    qInfo() << controller.command();

    controller.addTransaction("Some other Stuuff");
//    controller.commit();
    controller.addTransaction("blah blah blah");
    controller.rollback();

    qInfo() << controller.command();

    controller.cancel();
    qInfo() << controller.command();

    return a.exec();
}

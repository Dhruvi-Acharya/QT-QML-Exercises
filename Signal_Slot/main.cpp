#include <QCoreApplication>
#include "producer.h"
#include "consumer.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Producer produce;
    Consumer consume;

    QObject::connect(&produce,&Producer::readyProduct,&consume,&Consumer::readyProduct);
    QObject::connect(&a,&QCoreApplication::aboutToQuit,&produce,&Producer::stop);

    produce.start();

    return a.exec();
}

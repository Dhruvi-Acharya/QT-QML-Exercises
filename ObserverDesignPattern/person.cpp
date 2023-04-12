#include "person.h"

Person::Person(QObject *parent)
    : QObject{parent}
{

}

void Person::placeBid(QString item)
{
    Q_UNUSED(item);
    int value = QRandomGenerator::global()->bounded(1,1000);
    emit bid(value);
}

#include "cat.h"

Cat::Cat(QObject *parent)
    : QObject{parent}
{

}

void Cat::scared()
{
    qInfo() << "Cat scared";
    emit run();
}

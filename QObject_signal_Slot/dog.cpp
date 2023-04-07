#include "dog.h"

Dog::Dog(QObject *parent)
    : QObject{parent}
{

}

void Dog::chase()
{
    qInfo() << "Dog chase" << sender();
//    emit bark();
}

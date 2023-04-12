#include "baker.h"

Baker::Baker(QObject *parent)
    : Chef{parent}
{

}

void Baker::prepair()
{
    qInfo() << "mix the cake ingrediants";
}

void Baker::cook()
{
    qInfo() << "pre heat the oven";
    qInfo() << "bake the cake";
    qInfo() << "cool it down";
    qInfo() << "Extra!!!";
}

void Baker::cleanup()
{
    qInfo() << "make the cleanup";
}

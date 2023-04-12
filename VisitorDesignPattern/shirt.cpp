#include "shirt.h"

Shirt::Shirt(QObject *parent)
    : QObject{parent}
{
    value = 0;
}


void Shirt::accepet(iVisitor *visitor)
{
    qInfo() << this << "Prepare for a visitor";
    value = 55.55;

    visitor->visit(this);
    qInfo() << "Cleanup after visitor";
    value = 0;
}

double Shirt::amount()
{
    return value;
}

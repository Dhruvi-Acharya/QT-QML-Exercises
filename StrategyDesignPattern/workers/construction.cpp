#include "construction.h"

construction::construction(QObject *parent)
    : QObject{parent}
{

}

void construction::work()
{
    qInfo() << this << "Mix concrete";
    qInfo() << this << "Pour concrete";
    qInfo() << this << "Smooth concrete";
    qInfo() << this << "Fix concrete";
    qInfo() << this << "CLeanup";
}

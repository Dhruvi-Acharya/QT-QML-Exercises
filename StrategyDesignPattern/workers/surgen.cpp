#include "surgen.h"

Surgen::Surgen(QObject *parent)
    : QObject{parent}
{

}

void Surgen::work()
{
    qInfo() << this << "Put on gown and mask";
    qInfo() << this << "wash hands";
    qInfo() << this << "Put on gloves";
    qInfo() << this << "Perform the operation";
    qInfo() << this << "Cleanup";
}

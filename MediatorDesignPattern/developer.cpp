#include "developer.h"

Developer::Developer(QObject *parent)
    : QObject{parent}
{

}

void Developer::createPrograms()
{
    qInfo() << "Creating Programs...";
}

#include "marketing.h"

Marketing::Marketing(QObject *parent)
    : QObject{parent}
{

}

void Marketing::createHype()
{
    qInfo() << "Marketing create hype...";
}

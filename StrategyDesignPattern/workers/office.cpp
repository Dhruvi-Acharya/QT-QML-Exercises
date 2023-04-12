#include "office.h"
#include <QDebug>

Office::Office(QObject *parent)
    : QObject{parent}
{

}

void Office::work()
{
    qInfo() << this << "create worksheet and spreadsheet";
}

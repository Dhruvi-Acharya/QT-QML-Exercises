#include "manager.h"

Manager::Manager(QObject *parent)
    : QObject{parent}
{

}

void Manager::startProject()
{
    qInfo() << "Manager Starting project...";
    emit getToWork();
}

void Manager::completed()
{
    qInfo() << "Manger task completed...";
}

#include "chef.h"

Chef::Chef(QObject *parent)
    : QObject{parent}
{

}

void Chef::makedinner()
{
    prepair();
    cook();
    cleanup();
}

void Chef::prepair()
{
    qInfo() << "Prepair here";
}

void Chef::cook()
{
    qInfo() << "Cook here";
}

void Chef::cleanup()
{
    qInfo() << "Cleanup here";
}

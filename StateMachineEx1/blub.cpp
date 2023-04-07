#include "blub.h"

Blub::Blub(QObject *parent)
    : QObject{parent}
{
    powered = false; // initial class state
}

bool Blub::getPowered() const
{
    return powered;
}

void Blub::setPowered(bool newPowered)
{
    powered = newPowered;

    QString message;
    powered ? message = "on" : message = "off";

    qInfo() << this << message;
}

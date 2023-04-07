#include "memento.h"

Memento::Memento(QObject *parent)
    : QObject{parent}
{

}

void Memento::commit()
{
    commmand.clear();
    foreach (QString value, commands) {
        commmand.append(value + ";");
    }
}

void Memento::cancel()
{
    commands.clear();
}

void Memento::rollback()
{
    commands.clear();
    commands.append(commmand);
}

void Memento::addTransaction(QString value)
{
    commands.append(value);
}

QString Memento::command()
{
    return commmand;
}

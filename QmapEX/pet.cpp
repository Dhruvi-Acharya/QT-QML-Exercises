#include "pet.h"

Pet::Pet(QObject *parent)
    : QObject{parent}
{

}

QString Pet::getName() const
{
    return name;
}

void Pet::setName(const QString &newName)
{
    name = newName;
}

int Pet::getAge() const
{
    return age;
}

void Pet::setAge(int newAge)
{
    age = newAge;
}

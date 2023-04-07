#include <QCoreApplication>
#include <QDebug>
#include "pet.h"

typedef QMap<QString, Pet*> petlist;

petlist createpet(int max)
{
    petlist map;
    for(int i=0;i<max;i++)
    {
        QString id = QString::number(i);
        Pet* pet = new Pet();
        pet->setAge(i*10);
        pet->setName("Pet" + id);

        map.insert(id,pet);
    }
    return map;
}

void listPets(petlist pets)
{
    foreach (QString key, pets.keys()) {
        Pet *p = pets.value(key);
        qInfo() << key << p->getAge() << p->getName();
    }
}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    petlist pet = createpet(5);
    qInfo() << "Count: " << pet.count();
    listPets(pet);

//    qDeleteAll(pet.values());
    pet.clear();

    QMap<QString, int> map;
    map["one"] = 1;
    map["two"] = 2;
    map["three"] = 3;

    qInfo() << "Qmap: " << map;

    map.insert("four",4);

    qInfo() << "Qmap after insert: " << map;

    foreach (QString value, map.keys()) {
        qInfo() << "Key is: " << value;
    }

    foreach (int value, map.values()) {
        qInfo() << "value is: " << value;
    }

    map.remove("four");
    qInfo() << "Qmap after insert: " << map;

    return a.exec();
}

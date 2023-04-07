#include <QCoreApplication>
#include "developer.h"
#include "marketing.h"
#include "manager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Manager manager;
    Marketing marketing;
    QList<Developer *> developers;

    for(int i=0;i<5;i++)
    {
        Developer *developer = new Developer(&manager);
        developers.append(developer);

        QObject::connect(&manager,&Manager::getToWork,developer,&Developer::createPrograms);
        QObject::connect(developer,&Developer::completed,&manager,&Manager::completed);
    }

    QObject::connect(&manager,&Manager::getToWork,&marketing,&Marketing::createHype);
    QObject::connect(&marketing,&Marketing::finished,&manager,&Manager::completed);

    manager.startProject();

    return a.exec();
}

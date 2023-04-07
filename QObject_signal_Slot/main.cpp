#include <QCoreApplication>
#include "dog.h"
#include "cat.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Dog dog;
    dog.setObjectName("fido");
//    dog.chase();

    Cat cat;
    cat.setObjectName("Ghost");
//    cat.scared();

//    emit dog.bark();
    QObject::connect(&dog,&Dog::bark,&cat,&Cat::scared);
    QObject::connect(&cat,&Cat::run,&dog,&Dog::chase);

    emit dog.bark();

    return a.exec();
}

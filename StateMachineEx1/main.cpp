#include <QCoreApplication>
#include "blub.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Blub blub;

    blub.setPowered(true); // state change
    blub.setPowered(false); // state change

    return a.exec();
}

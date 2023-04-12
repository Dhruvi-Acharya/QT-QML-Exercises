#include <QCoreApplication>
#include "baker.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Baker bake;
    bake.makedinner();

    return a.exec();
}

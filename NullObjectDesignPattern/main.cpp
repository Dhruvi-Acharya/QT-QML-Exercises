#include <QCoreApplication>
#include "logger.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Logger logger;
    logger.log("Hello");

    QFile file("test.txt");
    logger.setFile(&file);

    logger.log("world");

    file.close();

    return a.exec();
}

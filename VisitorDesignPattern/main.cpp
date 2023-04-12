#include <QCoreApplication>
#include "cart.h"
#include "shirt.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    Cart cart;
    Shirt shirt;

    shirt.accepet(&cart);

    return a.exec();
}

#include <QCoreApplication>
#include "interface/iworker.h"
#include "workers/construction.h"
#include "workers/office.h"
#include "workers/surgen.h"
#include <QDebug>
#include <QRandomGenerator>

void fillList(QList<iworker*> &list)
{
    for(int i = 0; i < 5; i++)
    {
        int value = QRandomGenerator::global()->bounded(3);
        qInfo() << "Generated number is: " << value << " and count is: " << i;
        switch (value) {
        case 0:
            list.append(new construction());
            break;
        case 1:
            list.append(new Office());
            break;
        case 2:
            list.append(new Surgen());
            break;
        default:
            list.append(new construction());
            break;
        }
    }
}

void performWork(QList<iworker*> &list)
{
    foreach (iworker *worker, list)
    {
        worker->work();
    }
}


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QList<iworker*> list;
    fillList(list);
    performWork(list);
    qDeleteAll(list);
    list.clear();
    return a.exec();
}

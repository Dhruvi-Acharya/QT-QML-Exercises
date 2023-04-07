#include <QCoreApplication>
#include "myclass.h"
#include <QDebug>
#include <QThread>
#include <QTimer>

QTimer *timer;
MyClass *thread;

void timeout(){
    if(thread) return;
    if(timer) return;

    if(thread->isRunning() == true && thread->isFinished() == false){
        qInfo() << "Thread is running";
    }

    if(thread->isRunning() == false && thread->isFinished() == true){
        qInfo() << "Thread is stopped";
        timer->stop();
        thread->deleteLater();
        timer->deleteLater();
    }

}
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QThread::currentThread()->setObjectName("Main Thread");

    threadchild = new QThread(&a);
    threadchild->setObjectName("Child Thread");

    thread = new MyClass();
    timer = new QTimer(&a);

    thread->setObjectName("Child Thread My Class");
    timer->setInterval(1000);

    QObject::connect(timer,&QTimer::timeout,timeout);
    timer->start();
    thread->start();



    return a.exec();
}

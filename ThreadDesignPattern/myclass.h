#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QThread>
#include <QDebug>

class MyClass : public QThread
{
    Q_OBJECT
public:
    explicit MyClass(QObject *parent = nullptr);
    ~MyClass();

signals:


    // QThread interface
protected:
    void run();
};

#endif // MYCLASS_H

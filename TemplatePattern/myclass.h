#ifndef MYCLASS_H
#define MYCLASS_H

#include <QObject>
#include <QDebug>

class MyClass : public QObject
{
    Q_OBJECT
public:
    explicit MyClass(QObject *parent = nullptr);
    ~MyClass();

    int getValue() const;
    void setValue(int newValue);

    // you can also put template inside class
    template <typename T>
    T add2(T value1, T value2){
        return value1+value2;
    }

signals:
private:
    int value;

};

#endif // MYCLASS_H

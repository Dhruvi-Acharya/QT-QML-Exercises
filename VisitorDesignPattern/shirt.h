#ifndef SHIRT_H
#define SHIRT_H

#include <QObject>
#include <QDebug>
#include "interfaces/iElement.h"
#include "interfaces/iVisitor.h"

class Shirt : public QObject, public iElement
{
    Q_OBJECT
public:
    explicit Shirt(QObject *parent = nullptr);

signals:

private:
    double value;

    // iElement interface
public:
    void accepet(iVisitor *visitor);
    double amount();
};

#endif // SHIRT_H

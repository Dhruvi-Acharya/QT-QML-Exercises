#ifndef CART_H
#define CART_H

#include <QObject>
#include <QDebug>
#include "interfaces/iElement.h"

class Cart : public QObject, public iVisitor
{
    Q_OBJECT
public:
    explicit Cart(QObject *parent = nullptr);
    void visit(iElement *element);

signals:

};

#endif // CART_H

#ifndef SURGEN_H
#define SURGEN_H

#include <QObject>
#include <QDebug>
#include "interface/iworker.h"

class Surgen : public QObject, public iworker
{
    Q_OBJECT
public:
    explicit Surgen(QObject *parent = nullptr);
    void work();

signals:

};

#endif // SURGEN_H

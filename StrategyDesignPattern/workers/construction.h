#ifndef CONSTRUCTION_H
#define CONSTRUCTION_H

#include <QObject>
#include <QDebug>
#include "interface/iworker.h"

class construction : public QObject, public iworker
{
    Q_OBJECT
public:
    explicit construction(QObject *parent = nullptr);
    void work();

signals:

};

#endif // CONSTRUCTION_H

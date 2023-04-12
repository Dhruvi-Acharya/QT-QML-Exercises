#ifndef OFFICE_H
#define OFFICE_H

#include <QObject>
#include "interface/iworker.h"

class Office : public QObject, public iworker
{
    Q_OBJECT
public:
    explicit Office(QObject *parent = nullptr);
    void work();

signals:

};

#endif // OFFICE_H

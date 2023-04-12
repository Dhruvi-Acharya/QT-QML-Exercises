#ifndef BAKER_H
#define BAKER_H

#include <QObject>
#include <QDebug>
#include "chef.h"

class Baker : public Chef
{
    Q_OBJECT
public:
    explicit Baker(QObject *parent = nullptr);

signals:


    // Chef interface
private:
    void prepair();
    void cook();
    void cleanup();
};

#endif // BAKER_H

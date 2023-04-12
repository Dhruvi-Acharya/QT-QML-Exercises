#ifndef IELEMENT_H
#define IELEMENT_H

#include "iVisitor.h"
class iVisitor;

class iElement{
public:
    virtual ~iElement(){}
    virtual void accepet(iVisitor *visitor) = 0;
    virtual double amount() = 0;
};

#endif // IELEMENT_H

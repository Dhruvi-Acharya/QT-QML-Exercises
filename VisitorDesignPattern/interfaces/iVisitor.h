#ifndef IVISITOR_H
#define IVISITOR_H

#include "iElement.h"
class iElement;

class iVisitor{
public:
    virtual ~iVisitor(){}
    virtual void visit(iElement *element) = 0;

};

#endif // IVISITOR_H

#ifndef IWORKER_H
#define IWORKER_H

class iworker{
public:
    virtual ~iworker() {} //this will make people mad
    virtual void work() = 0;
};

#endif // IWORKER_H

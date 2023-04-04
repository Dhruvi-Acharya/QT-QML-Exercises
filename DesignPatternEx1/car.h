#ifndef CAR_H
#define CAR_H

#include "vehicle.h"
#include<QDebug>

class Car : public Vehicle
{
public:
    Car();
    ~Car();
    bool getVehicleReady(){
        qDebug() << "Car Vehicle is ready";
        return true;
    }
    void startVehicle(){
        qDebug() << "Vehicle Start";
    }
    void stopVehicle(){
        qDebug() << "Vehicle Stop";
    }
};

#endif // CAR_H

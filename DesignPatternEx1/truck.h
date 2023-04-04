#ifndef TRUCK_H
#define TRUCK_H

#include "vehicle.h"

class Truck : public Vehicle
{
public:
    Truck();
    ~Truck();

    bool getVehicleReady(){
        qInfo() << "Truck vehicle is ready";
        return true;
    }
    void startVehicle(){
        qInfo() << "Start Turck";
    }
    void stopVehicle(){
        qInfo() << "Stop Truck";
    }

};

#endif // TRUCK_H

#ifndef VEHICLE_H
#define VEHICLE_H

#include <QDebug>

class Vehicle
{
public:
    Vehicle();
    ~Vehicle();

    bool getVehicleReady();
    void startVehicle();
    void stopVehicle();
};

#endif // VEHICLE_H

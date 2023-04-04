#include "car.h"

Car::Car()
{
    startVehicle();
    getVehicleReady();
    stopVehicle();
}

Car::~Car()
{
    qInfo() << "Car destructor";
}

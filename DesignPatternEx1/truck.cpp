#include "truck.h"

Truck::Truck()
{
    getVehicleReady();
    startVehicle();
    stopVehicle();
}

Truck::~Truck()
{
    qInfo() << "Trck destructor";
}

#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include "delivery.h"
#include "vehicle.h"


// Restablece el estado de entregas y vehículos al inicio de la simulación
void reset_simulation(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles);

#endif

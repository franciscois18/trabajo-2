#ifndef VALIDATION_H
#define VALIDATION_H

#include "delivery.h"
#include "vehicle.h"

// Valida una entrega individual
int validate_delivery(Delivery* delivery);

// Valida un vehículo individual
int validate_vehicle(Vehicle* vehicle);

// Valida todas las entregas y vehículos
int validate_data(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles);

#endif

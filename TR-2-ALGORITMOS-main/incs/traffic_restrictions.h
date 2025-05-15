#ifndef TRAFFIC_RESTRICTIONS_H
#define TRAFFIC_RESTRICTIONS_H

#include "traffic_zones.h"
#include "driver_breaks.h"
#include "delivery.h"
#include "vehicle.h"

// Verifica si un vehículo puede realizar una entrega considerando tráfico y descansos
int can_deliver_with_traffic_restrictions(Vehicle* vehicle, Delivery* delivery, int current_time,
                                          TrafficSystem* traffic_system, DriverBreak* driver_breaks, int num_vehicles);

// Actualiza el estado del vehículo luego de asignar una entrega con restricciones
void update_vehicle_state_with_restrictions(Vehicle* vehicle, Delivery* delivery,
                                            TrafficSystem* traffic_system, DriverBreak* driver_breaks, int num_vehicles);

// Busca el mejor vehículo disponible para una entrega bajo restricciones
int find_best_vehicle_with_restrictions(Delivery* delivery, Vehicle* vehicles, int num_vehicles, int min_start_time,
                                        TrafficSystem* traffic_system, DriverBreak* driver_breaks);

#endif 

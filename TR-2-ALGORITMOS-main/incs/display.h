#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdio.h>
#include "vehicle.h"
#include "delivery.h"
#include "database.h"
#include "traffic_zones.h"
#include "driver_breaks.h"

// Mostrar el menú principal
void show_menu();


void display_deliveries(Delivery* deliveries, int num_deliveries);
void display_vehicles(Vehicle* vehicles, int num_vehicles);

// Muestra un informe de descansos de conductores
void print_driver_breaks_report(DriverBreak* driver_breaks, int num_vehicles);

// Muestra las zonas de tráfico activas
void print_traffic_zones(TrafficSystem* traffic_system);


#endif 
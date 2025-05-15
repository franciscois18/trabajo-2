#ifndef OPTIMIZATION_H
#define OPTIMIZATION_H

#include "database.h"

  // Detecta y resuelve conflictos de tiempo entre entregas asignadas
void detect_and_resolve_conflicts(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles);

 // Optimiza rutas de vehículos usando el algoritmo 2-opt
void optimize_routes_2opt(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles); 

 // Balancea la carga de trabajo repartiendo entregas entre vehículos
void balance_workload(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles); 

// Aplica todas las optimizaciones: conflictos, balanceo y 2-opt
void apply_optimizations(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles, TrafficSystem* traffic_system, DriverBreak* driver_breaks);  

#endif
#ifndef SCHEDULING_ALGO_H
#define SCHEDULING_ALGO_H

#include "delivery.h"
#include "vehicle.h"
#include "metrics.h"
#include "traffic_zones.h"
#include "driver_breaks.h"


// Estructura para almacenar distancias
typedef struct {
    int index;
    double dist;
} Distance;


//  Comparación para ordenar por distancia
int compare_distance(const void* a, const void* b);

// Comparación para ordenar por tiempo de procesamiento
int compare_processing_time(const void* a, const void* b);

// Comparación para ordenar entregas por fecha límite
int compare_deadline(const void* a, const void* b);


// Algoritmo Earliest Deadline First
int earliest_deadline_first(Delivery* deliveries, int num_deliveries, 
                          Vehicle* vehicles, int num_vehicles, 
                          Metrics* metrics,
                          TrafficSystem* traffic_system,
                          DriverBreak* driver_breaks);

// Algoritmo Nearest Neighbor
int nearest_neighbor(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles, 
                     Metrics* metrics, TrafficSystem* traffic_system, DriverBreak* driver_breaks);

// Algoritmo Shortest Processing Time
int shortest_processing_time(Delivery* deliveries, int num_deliveries, 
                           Vehicle* vehicles, int num_vehicles, 
                           Metrics* metrics,
                           TrafficSystem* traffic_system,
                           DriverBreak* driver_breaks);

#endif
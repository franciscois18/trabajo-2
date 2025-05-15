#ifndef SAVE_H
#define SAVE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "delivery.h"
#include "vehicle.h"
#include "metrics.h"
#include "traffic_zones.h"
#include "driver_breaks.h"


// Guarda los resultados en un archivo

void save_results(const char* file, Delivery* deliveries, int num_deliveries, Metrics* metrics);

// Exporta las métricas a un archivo CSV

void export_metrics_csv(const char* file, Metrics* metrics);

// Guarda todos los resultados (entregas y métricas)

void save_all_results(const char* deliveries_file, const char* metrics_file, 
    Delivery* deliveries, int num_deliveries, Metrics* metrics);

// Guarda resultados con métricas separadas

void save_results_with_metrics(const char* file, Delivery* deliveries, int num_deliveries, 
    Metrics* metrics);

// Guarda resultados comparativos de algoritmos

void save_comparison_results(const char* file, Delivery* deliveries, int num_deliveries, 
                             Vehicle* vehicles, int num_vehicles, 
                             TrafficSystem* traffic_system, DriverBreak* driver_breaks);


#endif
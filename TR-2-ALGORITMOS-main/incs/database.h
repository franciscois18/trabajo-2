#ifndef DATABASE_H
#define DATABASE_H

#include <stdio.h>
#include <stdlib.h>  
#include <string.h>  
#include "delivery.h"
#include "vehicle.h"
#include "aux.h"     
#include "validation.h"
#include "metrics.h"

// Carga entregas desde un archivo CSV
int load_deliveries(const char* file, Delivery* deliveries, int max_deliveries);

// Carga vehículos desde un archivo CSV
int load_vehicles(const char* file, Vehicle* vehicles, int max_vehicles);

#endif

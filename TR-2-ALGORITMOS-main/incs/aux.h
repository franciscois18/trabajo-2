#ifndef AUX_H
#define AUX_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <limits.h>

#include "delivery.h"
#include "vehicle.h"
#include "metrics.h"
#include "traffic_zones.h"
#include "driver_breaks.h"




// Función para obtener la fecha y hora actual como cadena
char* get_current_date_time();

// Limpiar el buffer de entrada
void clear_input_buffer();


// Convertir tiempo formato HH:MM a minutos
int convert_to_minutes(int hour, int min);

// Convertir minutos a formato HH:MM (devuelve una cadena)
char* convert_from_minutes(int minutes);

// Calcular distancia entre dos puntos
double calculate_distance(double x1, double y1, double x2, double y2);


int calculate_travel_time(double distance);


// Verificar si un vehículo puede realizar una entrega
int can_make_delivery(Vehicle* vehicle, Delivery* delivery, int current_time);

// Encontrar el mejor vehículo para una entrega
int find_best_vehicle(Delivery* delivery, Vehicle* vehicles, int num_vehicles, int preferred_start_time);

// Actualizar el estado del vehículo después de asignar una entrega
void update_vehicle_state(Vehicle* vehicle, Delivery* delivery);

// Comparadores para qsort
int compare_deadline(const void* a, const void* b);
int compare_processing_time(const void* a, const void* b);
int compare_distance(const void* a, const void* b);
int compare_priority(const void* a, const void* b);

// Reiniciar la simulación
void reset_simulation(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles);

// Ejecutar todos los algoritmos para comparación
void run_all_algorithms(Delivery* deliveries, int num_deliveries,
                       Vehicle* vehicles, int num_vehicles,
                       Metrics* metrics_array,
                       TrafficSystem* traffic_system,
                       DriverBreak* driver_breaks);


 // Devuelve ID de zona de tráfico para origen o destino, -1 si ninguna
int get_traffic_zone_for_location(TrafficSystem* traffic_system, double origin_x, double origin_y, double dest_x, double dest_y); 


// Devuelve factor de velocidad según zona y hora, 1.0 si sin restricción
double get_traffic_factor(TrafficSystem* traffic_system, int zone_id, int current_time);  


// convierte distancia (km) a tiempo (min) a velocidad base 50 km/h
int calculate_travel_time(double distance);  



#endif 
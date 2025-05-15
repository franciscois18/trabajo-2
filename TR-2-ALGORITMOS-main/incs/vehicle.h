#ifndef VEHICLE_H
#define VEHICLE_H

#include "delivery.h"

#define MAX_DELIVERIES_PER_VEHICLE 20
#define MAX_VEHICLES 50

// Estructura para representar un vehiculo
typedef struct {
   char id[5];              // ID unico 
   int type;                // Categoria 1 = pequeño - 2 = mediano - 3 = grande)
   double volume_capacity;  // Capacidad de volumen
   double weight_capacity;  // Capacidad de peso
   int start_hour, start_min; // Hora inicio jornada
   int end_hour, end_min;     // Hora fin jornada
   double pos_x, pos_y;      // Ubicacion inicial
   int specialty;           // Especialidad si aplica 
// Variables de estado actuales
   double current_volume;      // Volumen actualmente ocupado
   double current_weight;      // Peso actualmente cargado
   double current_pos_x, current_pos_y;  // Posición actual (coordenadas)
   int current_time;           // Hora actual en minutos desde el inicio del día

// Lista de entregas asignadas
   int num_deliveries;         // Número de entregas asignadas
   int deliveries[100];        // Índices de entregas asignadas (máximo 100 por vehículo)

} Vehicle;
#endif 
#ifndef DELIVERY_H
#define DELIVERY_H

#define MAX_DELIVERIES 100
typedef struct {
   char id[5];              // ID Unico
   double origin_x, origin_y;  // Coordenadas origen
   double destination_x, destination_y;    // Coordenadas destino
   int start_hour, start_min; // Hora inicio (24h)
   int end_hour, end_min;    // Hora limite (24h)
   int duration;            // Duracion en minutos
   int priority;            // Prioridad del 1 al 5
   int vehicle_type;        // 1 = pequeño - 2 = mediano - 3 = grande)
   double volume;           // Volumen en metros cubicos
   double weight;           // Peso en kg
   int assigned;            // Indica si la entrega está asignada (0 = no, 1 = sí)
   int assigned_vehicle;    // ID del vehículo asignado (0 si no está asignado)
   int start_time; // Hora de inicio
   
} Delivery;
#endif 

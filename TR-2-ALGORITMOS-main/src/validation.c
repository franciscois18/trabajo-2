#include "database.h"
#include "validation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Validar datos de entregas
int validate_delivery(Delivery* delivery) {
    // Verificar formato de ID (E seguido de tres dígitos)
    if (strlen(delivery->id) != 4 || delivery->id[0] != 'E' || 
        !isdigit(delivery->id[1]) || !isdigit(delivery->id[2]) || !isdigit(delivery->id[3])) {
        printf("Error: ID de entrega inválido: %s\n", delivery->id);
        return 0;
    }
    
    // Verificar coordenadas (no pueden ser negativas)
    if (delivery->origin_x < 0 || delivery->origin_y < 0 || 
        delivery->destination_x < 0 || delivery->destination_y < 0) {
        printf("Error: Coordenadas negativas en entrega %s\n", delivery->id);
        return 0;
    }
    
    // Verificar ventana de tiempo válida
    int start_time = convert_to_minutes(delivery->start_hour, delivery->start_min);
    int end_time = convert_to_minutes(delivery->end_hour, delivery->end_min);
    if (start_time >= end_time) {
        printf("Error: Ventana de tiempo inválida en entrega %s\n", delivery->id);
        return 0;
    }
    
    // Verificar duración positiva
    if (delivery->duration <= 0) {
        printf("Error: Duración inválida en entrega %s\n", delivery->id);
        return 0;
    }
    
    // Verificar prioridad válida (1-5)
    if (delivery->priority < 1 || delivery->priority > 5) {
        printf("Error: Prioridad fuera de rango en entrega %s\n", delivery->id);
        return 0;
    }
    
    // Verificar tipo de vehículo válido (1-3)
    if (delivery->vehicle_type < 1 || delivery->vehicle_type > 3) {
        printf("Error: Tipo de vehículo inválido en entrega %s\n", delivery->id);
        return 0;
    }
    
    // Verificar volumen y peso positivos
    if (delivery->volume <= 0 || delivery->weight <= 0) {
        printf("Error: Volumen o peso inválido en entrega %s\n", delivery->id);
        return 0;
    }
    
    return 1;
}

// Validar datos de vehículos
int validate_vehicle(Vehicle* vehicle) {
    // Verificar formato de ID (V seguido de tres dígitos)
    if (strlen(vehicle->id) != 4 || vehicle->id[0] != 'V' || 
        !isdigit(vehicle->id[1]) || !isdigit(vehicle->id[2]) || !isdigit(vehicle->id[3])) {
        printf("Error: ID de vehículo inválido: %s\n", vehicle->id);
        return 0;
    }
    
    // Verificar tipo de vehículo válido (1-3)
    if (vehicle->type < 1 || vehicle->type > 3) {
        printf("Error: Tipo de vehículo inválido en vehículo %s\n", vehicle->id);
        return 0;
    }
    
    // Verificar capacidades positivas
    if (vehicle->volume_capacity <= 0 || vehicle->weight_capacity <= 0) {
        printf("Error: Capacidad inválida en vehículo %s\n", vehicle->id);
        return 0;
    }
    
    // Verificar horario válido
    int start_time = convert_to_minutes(vehicle->start_hour, vehicle->start_min);
    int end_time = convert_to_minutes(vehicle->end_hour, vehicle->end_min);
    if (start_time >= end_time) {
        printf("Error: Horario inválido en vehículo %s\n", vehicle->id);
        return 0;
    }
    
    // Verificar coordenadas de posición inicial (no pueden ser negativas)
    if (vehicle->pos_x < 0 || vehicle->pos_y < 0) {
        printf("Error: Coordenadas negativas en vehículo %s\n", vehicle->id);
        return 0;
    }
    
    // Verificar especialidad válida (0-3)
    if (vehicle->specialty < 0 || vehicle->specialty > 3) {
        printf("Error: Especialidad inválida en vehículo %s\n", vehicle->id);
        return 0;
    }
    
    return 1;
}

// Validar datos de entregas y vehículos
int validate_data(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles) {
    int valid_data = 1;
    
    // Validar entregas
    for (int i = 0; i < num_deliveries; i++) {
        if (!validate_delivery(&deliveries[i])) {
            valid_data = 0;
        }
    }
    
    // Validar vehículos
    for (int i = 0; i < num_vehicles; i++) {
        if (!validate_vehicle(&vehicles[i])) {
            valid_data = 0;
        }
    }
    
    return valid_data;
} 
#include <stdio.h>
#include <stdlib.h>
#include "traffic_zones.h"
#include "driver_breaks.h"
#include "aux.h"



// Verificar si una entrega puede realizarse considerando restricciones de tráfico
int can_deliver_with_traffic_restrictions(Vehicle* vehicle, Delivery* delivery, int current_time,
                                         TrafficSystem* traffic_system, DriverBreak* driver_breaks, int num_vehicles) {
    // Extraer correctamente el ID del vehículo
    int vehicle_id = atoi(vehicle->id + 1);
    
    // Primero verificar si el conductor necesita un descanso
    int break_needed = driver_needs_break(vehicle_id, current_time, driver_breaks, num_vehicles);
    
    if (break_needed == 2) {
        // El conductor ha excedido su tiempo máximo diario
        return 0;
    } else if (break_needed == 1) {
        // El conductor necesita un descanso antes de continuar
        current_time += schedule_driver_break(vehicle_id, current_time, driver_breaks, num_vehicles);
    }
    
    // Calcular la distancia desde la posición actual del vehículo hasta el origen de la entrega
    double distance = calculate_distance(
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y
    );
    
    // Calcular el tiempo de viaje considerando zonas de tráfico
    int travel_time = calculate_travel_time_with_traffic(
        distance, current_time,
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y,
        traffic_system
    );
    
    int arrival_time = current_time + travel_time;
    
    // Verificar hora de inicio de la entrega
    int delivery_start_time = convert_to_minutes(delivery->start_hour, delivery->start_min);
    if (arrival_time < delivery_start_time) {
        arrival_time = delivery_start_time;
    }
    
    // Verificar hora límite de la entrega
    int delivery_end_time = convert_to_minutes(delivery->end_hour, delivery->end_min);
    if (arrival_time + delivery->duration > delivery_end_time) {
        return 0; // No se puede cumplir con la hora límite
    }
    
    // Verificar restricciones de capacidad
    if (vehicle->current_volume + delivery->volume > vehicle->volume_capacity ||
        vehicle->current_weight + delivery->weight > vehicle->weight_capacity) {
        return 0; // Excede la capacidad del vehículo
    }
    
    return 1; // La entrega puede realizarse
}

// Actualizar estados después de asignar una entrega considerando tráfico y descansos
void update_vehicle_state_with_restrictions(Vehicle* vehicle, Delivery* delivery,
                                          TrafficSystem* traffic_system, DriverBreak* driver_breaks, int num_vehicles) {
    // Extraer correctamente el ID del vehículo (usar atoi consistentemente)
    int vehicle_id = atoi(vehicle->id + 1);
    
    // Verificar si el conductor necesita un descanso
    int break_needed = driver_needs_break(vehicle_id, vehicle->current_time, driver_breaks, num_vehicles);
    
    // Si necesita descanso, programarlo
    if (break_needed == 1) {
        vehicle->current_time += schedule_driver_break(vehicle_id, vehicle->current_time, driver_breaks, num_vehicles);
    }
    
    // Calcular tiempo de viaje al origen de la entrega
    double distance_to_origin = calculate_distance(
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y
    );
    
    int travel_time = calculate_travel_time_with_traffic(
        distance_to_origin, vehicle->current_time,
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y,
        traffic_system
    );
    
    // Actualizar tiempo actual del vehículo
    vehicle->current_time += travel_time;
    
    // Verificar si llegamos antes del tiempo mínimo de inicio
    int min_start_time = convert_to_minutes(delivery->start_hour, delivery->start_min);
    if (vehicle->current_time < min_start_time) {
        vehicle->current_time = min_start_time;
    }
    
    // Asignar la entrega al vehículo
    vehicle->deliveries[vehicle->num_deliveries++] = atoi(delivery->id + 1);
    vehicle->current_volume += delivery->volume;
    vehicle->current_weight += delivery->weight;
    
    // Establecer tiempo de inicio de la entrega
    delivery->start_time = vehicle->current_time;
    delivery->assigned = 1;
    delivery->assigned_vehicle = vehicle_id;
    
    // Actualizar posición del vehículo al destino de la entrega
    vehicle->current_pos_x = delivery->destination_x;
    vehicle->current_pos_y = delivery->destination_y;
    
    // Aquí registramos el tiempo de trabajo: tiempo de viaje + duración de la entrega
    int work_time = travel_time + delivery->duration;
    
    // Avanzar el tiempo por la duración de la entrega
    vehicle->current_time += delivery->duration;
    
    // IMPORTANTE: Actualizar tiempo de trabajo del conductor con el ID correcto
    update_driver_work_time(vehicle_id, work_time, driver_breaks, num_vehicles);
}

// Buscar mejor vehículo considerando restricciones de tráfico y descansos
int find_best_vehicle_with_restrictions(Delivery* delivery, Vehicle* vehicles, int num_vehicles, int min_start_time,
                                      TrafficSystem* traffic_system, DriverBreak* driver_breaks) {
    int best_vehicle = -1;
    int earliest_finish_time = -1;
    
    for (int v = 0; v < num_vehicles; v++) {
        // Comprobar si el vehículo puede realizar la entrega con las restricciones
        if (can_deliver_with_traffic_restrictions(&vehicles[v], delivery, vehicles[v].current_time, 
                                                traffic_system, driver_breaks, num_vehicles)) {
            // Calcular cuando terminaría la entrega con este vehículo
            
            // Calcular distancia al origen de la entrega
            double distance = calculate_distance(
                vehicles[v].current_pos_x, vehicles[v].current_pos_y,
                delivery->origin_x, delivery->origin_y
            );
            
            // Calcular tiempo de viaje con tráfico
            int travel_time = calculate_travel_time_with_traffic(
                distance, vehicles[v].current_time,
                vehicles[v].current_pos_x, vehicles[v].current_pos_y,
                delivery->origin_x, delivery->origin_y,
                traffic_system
            );
            
            // Calcular tiempo de finalización
            int arrival_time = vehicles[v].current_time + travel_time;
            if (arrival_time < min_start_time) {
                arrival_time = min_start_time;
            }
            
            int finish_time = arrival_time + delivery->duration;
            
            // Verificar si este vehículo terminaría antes que el mejor actual
            if (best_vehicle == -1 || finish_time < earliest_finish_time) {
                best_vehicle = v;
                earliest_finish_time = finish_time;
            }
        }
    }
    
    return best_vehicle;
}
#include "optimization.h"
#include "aux.h"
#include "metrics.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Detectar y resolver conflictos de asignación entre vehículos
void detect_and_resolve_conflicts(Delivery* deliveries, int num_deliveries, 
                                 Vehicle* vehicles, int num_vehicles) {
    printf("Detectando y resolviendo conflictos de asignación...\n");
    
    // Buscar conflictos de tiempo entre entregas asignadas al mismo vehículo
    for (int v = 0; v < num_vehicles; v++) {
        if (vehicles[v].num_deliveries <= 1) continue; // No hay conflictos posibles 
        

        Delivery* assigned_deliveries[MAX_DELIVERIES_PER_VEHICLE];
        int assigned_count = 0;
        

        for (int d = 0; d < num_deliveries; d++) {
            if (deliveries[d].assigned && deliveries[d].assigned_vehicle == atoi(vehicles[v].id + 1)) {
                assigned_deliveries[assigned_count++] = &deliveries[d];
            }
        }
        

        for (int i = 0; i < assigned_count - 1; i++) {
            for (int j = i + 1; j < assigned_count; j++) {
                if (assigned_deliveries[i]->start_time > assigned_deliveries[j]->start_time) {
                    Delivery* temp = assigned_deliveries[i];
                    assigned_deliveries[i] = assigned_deliveries[j];
                    assigned_deliveries[j] = temp;
                }
            }
        }
        

        for (int i = 0; i < assigned_count - 1; i++) {
            Delivery* current = assigned_deliveries[i];
            Delivery* next = assigned_deliveries[i + 1];
            

            int finish_time = current->start_time + current->duration;
            
           
            double travel_dist = calculate_distance(
                current->destination_x, current->destination_y,
                next->origin_x, next->origin_y
            );
            int travel_time = calculate_travel_time(travel_dist);
            
            if (finish_time + travel_time > next->start_time) {
   
                next->start_time = finish_time + travel_time;
                
                int next_deadline = convert_to_minutes(next->end_hour, next->end_min);
                
                if (next->start_time + next->duration > next_deadline) {
                    printf("Advertencia: Conflicto no resuelto para la entrega %s, se excede el plazo.\n", next->id);
                    

                    int reassigned = 0;
                    for (int other_v = 0; other_v < num_vehicles; other_v++) {
                        if (other_v == v) continue;
                        
                        if (can_make_delivery(&vehicles[other_v], next, 
                                            vehicles[other_v].current_time)) {
           
                            for (int d = 0; d < vehicles[v].num_deliveries; d++) {
                                if (vehicles[v].deliveries[d] == atoi(next->id + 1)) {
              
                                    for (int k = d; k < vehicles[v].num_deliveries - 1; k++) {
                                        vehicles[v].deliveries[k] = vehicles[v].deliveries[k + 1];
                                    }
                                    vehicles[v].num_deliveries--;
                                    break;
                                }
                            }
                            
           
                            vehicles[v].current_volume -= next->volume;
                            vehicles[v].current_weight -= next->weight;
                            
                 
                            update_vehicle_state(&vehicles[other_v], next);
                            next->assigned_vehicle = atoi(vehicles[other_v].id + 1);
                            
                            printf("Entrega %s reasignada al vehículo %s para resolver conflicto.\n", 
                                  next->id, vehicles[other_v].id);
                            
                            reassigned = 1;
                            break;
                        }
                    }
                    
                    if (!reassigned) {
                        printf("No se pudo reasignar la entrega %s. Se mantiene asignada pero podría retrasarse.\n", next->id);
                    }
                } else {
                    printf("Ajustado tiempo de inicio de entrega %s para resolver conflicto.\n", next->id);
                }
            }
        }
    }
}




// Balancear la carga entre vehículos
void balance_workload(Delivery* deliveries, int num_deliveries, 
                      Vehicle* vehicles, int num_vehicles) {
    printf("Balanceando carga de trabajo entre vehículos...\n");
    
    // Calcular carga actual por vehículo (en términos de tiempo total de trabajo)
    int* workload = (int*)malloc(num_vehicles * sizeof(int));
    if (!workload) {
        printf("Error: No se pudo asignar memoria para balance de carga.\n");
        return;
    }
    
    // Inicializar cargas de trabajo
    for (int v = 0; v < num_vehicles; v++) {
        workload[v] = 0;
        
        // Calcular tiempo total de trabajo (viaje + duración de entregas)
        for (int d = 0; d < num_deliveries; d++) {
            if (deliveries[d].assigned && deliveries[d].assigned_vehicle == atoi(vehicles[v].id + 1)) {
                // Sumar duración de la entrega
                workload[v] += deliveries[d].duration;
                
                // Si no es la primera entrega, sumar tiempo de viaje desde la entrega anterior
                if (vehicles[v].num_deliveries > 1) {
                
                    workload[v] += 15; // 15 minutos promedio entre entregas
                }
            }
        }
    }
    
    // Identificar vehículos con mayor y menor carga
    while (1) {
        int max_workload_idx = 0;
        int min_workload_idx = 0;
        
        for (int v = 1; v < num_vehicles; v++) {
            if (workload[v] > workload[max_workload_idx]) {
                max_workload_idx = v;
            }
            if (workload[v] < workload[min_workload_idx]) {
                min_workload_idx = v;
            }
        }
        
        // Si la diferencia es pequeña, no es necesario balancear
        if (workload[max_workload_idx] - workload[min_workload_idx] < 30) {
            break;
        }
        
        // Intentar transferir una entrega del vehículo más cargado al menos cargado
        Vehicle* source = &vehicles[max_workload_idx];
        Vehicle* target = &vehicles[min_workload_idx];
        
        // Buscar la entrega más apropiada para transferir
        int best_delivery_idx = -1;
        int best_duration = 0;
        
        for (int d = 0; d < num_deliveries; d++) {
            if (deliveries[d].assigned && deliveries[d].assigned_vehicle == atoi(source->id + 1)) {
                // Verificar si el vehículo destino puede manejar esta entrega
                if (can_make_delivery(target, &deliveries[d], target->current_time)) {
                    // Si es la primera entrega válida o tiene menor duración que la mejor encontrada
                    if (best_delivery_idx == -1 || deliveries[d].duration < best_duration) {
                        best_delivery_idx = d;
                        best_duration = deliveries[d].duration;
                    }
                }
            }
        }
        
        // Si encontramos una entrega para transferir
        if (best_delivery_idx != -1) {
            Delivery* transfer_delivery = &deliveries[best_delivery_idx];
            
            // Eliminar la entrega del vehículo origen
            for (int d = 0; d < source->num_deliveries; d++) {
                if (source->deliveries[d] == atoi(transfer_delivery->id + 1)) {
                    for (int k = d; k < source->num_deliveries - 1; k++) {
                        source->deliveries[k] = source->deliveries[k + 1];
                    }
                    source->num_deliveries--;
                    break;
                }
            }
            
            // Actualizar el estado del vehículo origen
            source->current_volume -= transfer_delivery->volume;
            source->current_weight -= transfer_delivery->weight;
            
            // Asignar al vehículo destino
            update_vehicle_state(target, transfer_delivery);
            
            // Actualizar cargas de trabajo
            workload[max_workload_idx] -= (transfer_delivery->duration + 15);
            workload[min_workload_idx] += (transfer_delivery->duration + 15);
            
            printf("Entrega %s transferida del vehículo %s al vehículo %s para balancear carga.\n", 
                  transfer_delivery->id, source->id, target->id);
        } else {
            // No se encontró ninguna entrega para transferir
            printf("No se pudo transferir ninguna entrega para balancear carga.\n");
            break;
        }
    }
    
    free(workload);
}




// Optimizar rutas usando 2-opt
void optimize_routes_2opt(Delivery* deliveries, int num_deliveries, 
                         Vehicle* vehicles, int num_vehicles) {
    printf("Optimizando rutas con algoritmo 2-opt...\n");
    
    for (int v = 0; v < num_vehicles; v++) {
        if (vehicles[v].num_deliveries <= 2) continue; // 2-opt no es útil para rutas de 2 o menos entregas
        

        Delivery* route[MAX_DELIVERIES_PER_VEHICLE];
        int route_size = 0;
        
        for (int d = 0; d < num_deliveries; d++) {
            if (deliveries[d].assigned && deliveries[d].assigned_vehicle == atoi(vehicles[v].id + 1)) {
                route[route_size++] = &deliveries[d];
            }
        }
        
        // Ordenar por tiempo de inicio actual
        for (int i = 0; i < route_size - 1; i++) {
            for (int j = i + 1; j < route_size; j++) {
                if (route[i]->start_time > route[j]->start_time) {
                    Delivery* temp = route[i];
                    route[i] = route[j];
                    route[j] = temp;
                }
            }
        }
        
        // intentar intercambiar pares de entregas para reducir distancia total
        for (int i = 0; i < route_size - 1; i++) {
            for (int j = i + 1; j < route_size; j++) {
                // Calcular distancia actual
                double current_distance = 0;
                
                if (i == 0) {
                    current_distance += calculate_distance(
                        vehicles[v].pos_x, vehicles[v].pos_y,
                        route[0]->origin_x, route[0]->origin_y
                    );
                }
                
                // Distancia entre entregas en la ruta actual
                for (int k = i; k < j; k++) {
                    // Distancia desde el destino de la entrega actual al origen de la siguiente
                    current_distance += calculate_distance(
                        route[k]->destination_x, route[k]->destination_y,
                        route[k+1]->origin_x, route[k+1]->origin_y
                    );
                }
                
                // Calcular distancia si se intercambian las entregas i y j
                double new_distance = 0;
                
                // Intercambiar temporalmente
                Delivery* temp = route[i];
                route[i] = route[j];
                route[j] = temp;
                
                // Distancia desde la posición actual a la primera entrega
                if (i == 0) {
                    new_distance += calculate_distance(
                        vehicles[v].pos_x, vehicles[v].pos_y,
                        route[0]->origin_x, route[0]->origin_y
                    );
                }
                
                // Distancia entre entregas en la nueva ruta
                for (int k = i; k < j; k++) {
                    new_distance += calculate_distance(
                        route[k]->destination_x, route[k]->destination_y,
                        route[k+1]->origin_x, route[k+1]->origin_y
                    );
                }
                
                // Si la nueva distancia es mayor, deshacer el intercambio
                if (new_distance >= current_distance) {
                    // Revertir el intercambio
                    temp = route[i];
                    route[i] = route[j];
                    route[j] = temp;
                } else {
                    printf("Mejorada ruta del vehículo %s: intercambiadas entregas %s y %s.\n", 
                           vehicles[v].id, route[i]->id, route[j]->id);
                    
                    // Recalcular tiempos de inicio para la nueva ruta
                    int current_time = vehicles[v].current_time;
                    
                    for (int k = 0; k < route_size; k++) {
                        // Calcular tiempo de viaje desde la posición actual
                        double travel_dist;
                        if (k == 0) {
                            travel_dist = calculate_distance(
                                vehicles[v].pos_x, vehicles[v].pos_y,
                                route[k]->origin_x, route[k]->origin_y
                            );
                        } else {
                            travel_dist = calculate_distance(
                                route[k-1]->destination_x, route[k-1]->destination_y,
                                route[k]->origin_x, route[k]->origin_y
                            );
                        }
                        
                        int travel_time = calculate_travel_time(travel_dist);
                        current_time += travel_time;
                        
                        // Asegurarse de que no comenzamos antes de la hora mínima de inicio
                        int min_start_time = convert_to_minutes(route[k]->start_hour, route[k]->start_min);
                        if (current_time < min_start_time) {
                            current_time = min_start_time;
                        }
                        
                        // Actualizar tiempo de inicio
                        route[k]->start_time = current_time;
                        
                        // Avanzar el tiempo actual
                        current_time += route[k]->duration;
                    }
                }
            }
        }
    }
}

void apply_optimizations(Delivery* deliveries, int num_deliveries, 
                       Vehicle* vehicles, int num_vehicles,
                       TrafficSystem* traffic_system, 
                       DriverBreak* driver_breaks) {

    (void)traffic_system;
    (void)driver_breaks;

    // 1. Detectar y resolver conflictos de tiempo
    detect_and_resolve_conflicts(deliveries, num_deliveries, vehicles, num_vehicles);
    
    // 2. Balancear la carga de trabajo entre vehículos
    balance_workload(deliveries, num_deliveries, vehicles, num_vehicles);
    
    // 3. Optimizar rutas usando 2-opt
    optimize_routes_2opt(deliveries, num_deliveries, vehicles, num_vehicles);
    
    printf("Optimizaciones aplicadas con éxito.\n");
}
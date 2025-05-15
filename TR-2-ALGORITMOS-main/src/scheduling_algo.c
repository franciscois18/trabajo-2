#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "scheduling_algo.h"
#include "traffic_restrictions.h"
#include "aux.h"
#include "metrics.h"


// Earliest Deadline First (EDF)
int earliest_deadline_first(Delivery* deliveries, int num_deliveries, 
                          Vehicle* vehicles, int num_vehicles, 
                          Metrics* metrics,
                          TrafficSystem* traffic_system,
                          DriverBreak* driver_breaks) {
    clock_t start_time = clock();
    
    // Crear una copia de las entregas para ordenarlas
    Delivery* sorted_deliveries = (Delivery*)malloc(num_deliveries * sizeof(Delivery));
    if (!sorted_deliveries) {
        printf("Error: Memoria insuficiente para el algoritmo EDF\n");
        return 0;
    }
    
    // Copiar entregas y ordenarlas por fecha límite
    for (int i = 0; i < num_deliveries; i++) {
        sorted_deliveries[i] = deliveries[i];
    }
    qsort(sorted_deliveries, num_deliveries, sizeof(Delivery), compare_deadline);
    
    // Asignar entregas a vehículos
    int assigned_count = 0;
    
    for (int i = 0; i < num_deliveries; i++) {
        if (!sorted_deliveries[i].assigned) {
            // Buscar el mejor vehículo para esta entrega
            int best_vehicle = find_best_vehicle(&sorted_deliveries[i], vehicles, num_vehicles, 
                                               convert_to_minutes(sorted_deliveries[i].start_hour, sorted_deliveries[i].start_min));
            
            if (best_vehicle != -1) {
                // Verificar si el conductor necesita un descanso antes de esta entrega
                int vehicle_idx = best_vehicle;
                int current_time = vehicles[vehicle_idx].current_time;
                
                // Comprobar si es necesario programar un descanso para el conductor
                if (driver_needs_break(vehicles[vehicle_idx].id[1] - '0', current_time, driver_breaks, num_vehicles)) {
                    // Programar un descanso para el conductor
                    schedule_driver_break(vehicles[vehicle_idx].id[1] - '0', current_time, driver_breaks, num_vehicles);
                    
                    // Actualizar el tiempo actual del vehículo después del descanso
                    vehicles[vehicle_idx].current_time += MIN_BREAK_DURATION;
                }
                
                // Calcular la distancia para esta entrega
                double dist_to_origin = calculate_distance(
                    vehicles[best_vehicle].current_pos_x, vehicles[best_vehicle].current_pos_y,
                    sorted_deliveries[i].origin_x, sorted_deliveries[i].origin_y
                );
                double dist_to_dest = calculate_distance(
                    sorted_deliveries[i].origin_x, sorted_deliveries[i].origin_y,
                    sorted_deliveries[i].destination_x, sorted_deliveries[i].destination_y
                );
                
                // Calcular el tiempo de viaje considerando el tráfico
                int estimated_travel_time = calculate_travel_time_with_traffic(
                    dist_to_origin + dist_to_dest, 
                    vehicles[best_vehicle].current_time,
                    vehicles[best_vehicle].current_pos_x, vehicles[best_vehicle].current_pos_y,
                    sorted_deliveries[i].destination_x, sorted_deliveries[i].destination_y,
                    traffic_system
                );
                

                
                // Asignar la entrega al vehículo
                update_vehicle_state(&vehicles[best_vehicle], &sorted_deliveries[i]);
                
                // Actualizar la entrega original
                for (int j = 0; j < num_deliveries; j++) {
                    if (strcmp(deliveries[j].id, sorted_deliveries[i].id) == 0) {
                        deliveries[j].assigned = 1;
                        deliveries[j].assigned_vehicle = sorted_deliveries[i].assigned_vehicle;
                        deliveries[j].start_time = sorted_deliveries[i].start_time;
                        break;
                    }
                }
                
                // Actualizar el tiempo acumulado de conducción para el control de descansos
                update_driver_work_time(
                    vehicles[best_vehicle].id[1] - '0', 
                    estimated_travel_time, 
                    driver_breaks, 
                    num_vehicles
                );
                
                assigned_count++;
            }
        }
    }
    
    // Calcular métricas
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    update_metrics(metrics, deliveries, num_deliveries, vehicles, num_vehicles, execution_time);
    
    // Liberar memoria
    free(sorted_deliveries);
    
    return assigned_count;
}


// Algoritmo Nearest Neighbor
int nearest_neighbor(Delivery* deliveries, int num_deliveries,
                     Vehicle* vehicles, int num_vehicles,
                     Metrics* metrics,
                     TrafficSystem* traffic_system,
                     DriverBreak* driver_breaks) {
    clock_t start_time = clock();

    int assigned[MAX_DELIVERIES] = {0};
    int assigned_count = 0;

    while (assigned_count < num_deliveries) {
        int old_assigned_count = assigned_count;

        // Primera pasada: cada vehículo toma su entrega más cercana
        for (int v = 0; v < num_vehicles && assigned_count < num_deliveries; v++) {
            int nearest_delivery = -1;
            double min_distance = 0.0;

            // Buscar la entrega más cercana sin asignar
            for (int d = 0; d < num_deliveries; d++) {
                if (!assigned[d] && !deliveries[d].assigned) {
                    if (can_deliver_with_traffic_restrictions(
                            &vehicles[v], &deliveries[d],
                            vehicles[v].current_time,
                            traffic_system, driver_breaks, num_vehicles)) {
                        double dist = calculate_distance(
                            vehicles[v].current_pos_x, vehicles[v].current_pos_y,
                            deliveries[d].origin_x, deliveries[d].origin_y);
                        if (nearest_delivery == -1 || dist < min_distance) {
                            nearest_delivery = d;
                            min_distance = dist;
                        }
                    }
                }
            }

            if (nearest_delivery != -1) {
                update_vehicle_state_with_restrictions(
                    &vehicles[v], &deliveries[nearest_delivery],
                    traffic_system, driver_breaks, num_vehicles);

                assigned[nearest_delivery] = 1;
                assigned_count++;

                printf("Vehículo %s asignado para entrega %s - Tiempo: %02d:%02d\n",
                       vehicles[v].id,
                       deliveries[nearest_delivery].id,
                       vehicles[v].current_time / 60,
                       vehicles[v].current_time % 60);
            }
        }

        // Segunda pasada: intentar asignar cualquier entrega restante
        for (int d = 0; d < num_deliveries && assigned_count < num_deliveries; d++) {
            if (!assigned[d] && !deliveries[d].assigned) {
                int best_vehicle = find_best_vehicle_with_restrictions(
                    &deliveries[d], vehicles, num_vehicles,
                    convert_to_minutes(deliveries[d].start_hour, deliveries[d].start_min),
                    traffic_system, driver_breaks);

                if (best_vehicle != -1) {
                    update_vehicle_state_with_restrictions(
                        &vehicles[best_vehicle], &deliveries[d],
                        traffic_system, driver_breaks, num_vehicles);

                    assigned[d] = 1;
                    assigned_count++;

                    printf("Vehículo %s asignado para entrega %s (segunda vuelta) - Tiempo: %02d:%02d\n",
                           vehicles[best_vehicle].id,
                           deliveries[d].id,
                           vehicles[best_vehicle].current_time / 60,
                           vehicles[best_vehicle].current_time % 60);
                }
            }
        }

        // Si no hubo nuevas asignaciones, salimos
        if (assigned_count == old_assigned_count) {
            printf("No se pueden asignar más entregas. Saliendo...\n");
            break;
        }
    }

    // Estado final
    printf("\nEstado final de la asignación:\n");
    printf("Entregas asignadas: %d de %d\n", assigned_count, num_deliveries);
    int unassigned = num_deliveries - assigned_count;
    printf("Entregas no asignadas: %d\n", unassigned);

    // Cierre de medición de tiempo
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    update_metrics(metrics, deliveries, num_deliveries, vehicles, num_vehicles, execution_time);

    return assigned_count;
}



// Algoritmo Shortest Processing Time (SPT)
int shortest_processing_time(Delivery* deliveries, int num_deliveries, 
                            Vehicle* vehicles, int num_vehicles, 
                            Metrics* metrics,
                            TrafficSystem* traffic_system,
                            DriverBreak* driver_breaks) {
    clock_t start_time = clock();
    
    // Crear una copia de las entregas para ordenarlas
    Delivery* sorted_deliveries = (Delivery*)malloc(num_deliveries * sizeof(Delivery));
    if (!sorted_deliveries) {
        printf("Error: Memoria insuficiente para el algoritmo SPT\n");
        return 0;
    }
    
    // Copiar entregas y ordenarlas por tiempo de procesamiento (duración)
    for (int i = 0; i < num_deliveries; i++) {
        sorted_deliveries[i] = deliveries[i];
    }
    qsort(sorted_deliveries, num_deliveries, sizeof(Delivery), compare_processing_time);
    
    // Asignar entregas a vehículos
    int assigned_count = 0;
    
    for (int i = 0; i < num_deliveries; i++) {
        if (!sorted_deliveries[i].assigned) {
            // Buscar el mejor vehículo para esta entrega
            int best_vehicle = find_best_vehicle(
                &sorted_deliveries[i], vehicles, num_vehicles, 
                convert_to_minutes(sorted_deliveries[i].start_hour, sorted_deliveries[i].start_min)
            );
            
            if (best_vehicle != -1) {
                // Verificar si el conductor necesita un descanso antes de esta entrega
                if (driver_needs_break(vehicles[best_vehicle].id[1] - '0', vehicles[best_vehicle].current_time, driver_breaks, num_vehicles)) {
                    // Programar un descanso para el conductor
                    schedule_driver_break(vehicles[best_vehicle].id[1] - '0', vehicles[best_vehicle].current_time, driver_breaks, num_vehicles);
                    
                    // Actualizar el tiempo actual del vehículo después del descanso
                    vehicles[best_vehicle].current_time += MIN_BREAK_DURATION;
                }
                
                // Calcular la distancia para esta entrega
                double dist_to_origin = calculate_distance(
                    vehicles[best_vehicle].current_pos_x, vehicles[best_vehicle].current_pos_y,
                    sorted_deliveries[i].origin_x, sorted_deliveries[i].origin_y
                );
                double dist_to_dest = calculate_distance(
                    sorted_deliveries[i].origin_x, sorted_deliveries[i].origin_y,
                    sorted_deliveries[i].destination_x, sorted_deliveries[i].destination_y
                );
                
                // Calcular el tiempo estimado considerando el tráfico
                int estimated_travel_time = calculate_travel_time_with_traffic(
                    dist_to_origin + dist_to_dest,
                    vehicles[best_vehicle].current_time,
                    vehicles[best_vehicle].current_pos_x, vehicles[best_vehicle].current_pos_y,
                    sorted_deliveries[i].destination_x, sorted_deliveries[i].destination_y,
                    traffic_system
                );
                
                // Asignar la entrega al vehículo
                update_vehicle_state(&vehicles[best_vehicle], &sorted_deliveries[i]);
                
                // Actualizar la entrega original
                for (int j = 0; j < num_deliveries; j++) {
                    if (strcmp(deliveries[j].id, sorted_deliveries[i].id) == 0) {
                        deliveries[j].assigned = 1;
                        deliveries[j].assigned_vehicle = sorted_deliveries[i].assigned_vehicle;
                        deliveries[j].start_time = sorted_deliveries[i].start_time;
                        break;
                    }
                }
                
                // Actualizar el tiempo acumulado de conducción para el control de descansos
                update_driver_work_time(
                    vehicles[best_vehicle].id[1] - '0', 
                    estimated_travel_time, 
                    driver_breaks, 
                    num_vehicles
                );
                
                assigned_count++;
            }
        }
    }
    
    // Calcular métricas
    clock_t end_time = clock();
    double execution_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    
    update_metrics(metrics, deliveries, num_deliveries, vehicles, num_vehicles, execution_time);
    
    // Liberar memoria
    free(sorted_deliveries);
    
    return assigned_count;
}

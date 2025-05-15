
#include "aux.h"
#include "metrics.h"
#include "scheduling_algo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Inicializar métricas
void init_metrics(Metrics* metrics) {
    if (metrics) {
        metrics->completed_deliveries = 0;
        metrics->total_waiting_time = 0;
        metrics->total_distance = 0.0;
        metrics->customer_satisfaction = 0.0;
        metrics->resource_utilization = 0.0;
        metrics->execution_time = 0.0;
    }
}

// Actualizar métricas basadas en los resultados de la asignación
void update_metrics(Metrics* metrics, Delivery* deliveries, int num_deliveries, 
                   Vehicle* vehicles, int num_vehicles, double exec_time) {
    if (!metrics) return;
    
    // Reiniciar métricas
    init_metrics(metrics);
    
    // Establecer tiempo de ejecución
    metrics->execution_time = exec_time;
    
    // Variables para cálculos
    int total_priority = 0;
    int max_possible_priority = 0;
    int total_vehicle_minutes = 0;
    int used_vehicle_minutes = 0;
    
    // Contar entregas completadas y calcular satisfacción del cliente
    for (int i = 0; i < num_deliveries; i++) {
        // Sumar prioridad máxima posible
        max_possible_priority += deliveries[i].priority;
        
        if (deliveries[i].assigned) {
            metrics->completed_deliveries++;
            
            // Sumar prioridad de entregas completadas para satisfacción
            total_priority += deliveries[i].priority;
            
            // Calcular tiempo de espera (diferencia entre tiempo de inicio real y el mínimo posible)
            int min_start_time = convert_to_minutes(deliveries[i].start_hour, deliveries[i].start_min);
            int waiting_time = (deliveries[i].start_time > min_start_time) ? 
                              (deliveries[i].start_time - min_start_time) : 0;
            metrics->total_waiting_time += waiting_time;
        }
    }
    
    // Calcular distancia total y utilización de recursos
    for (int v = 0; v < num_vehicles; v++) {
        double vehicle_distance = 0.0;
        double origin_x = vehicles[v].pos_x;
        double origin_y = vehicles[v].pos_y;
        
        // Calcular tiempo disponible total del vehículo
        int start_time = convert_to_minutes(vehicles[v].start_hour, vehicles[v].start_min);
        int end_time = convert_to_minutes(vehicles[v].end_hour, vehicles[v].end_min);
        total_vehicle_minutes += (end_time - start_time);
        
        // Si el vehículo tiene entregas asignadas
        if (vehicles[v].num_deliveries > 0) {
            double current_x = origin_x;
            double current_y = origin_y;
            
            // Para cada entrega asignada a este vehículo
            for (int d = 0; d < vehicles[v].num_deliveries; d++) {
                int delivery_idx = -1;
                
                // Encontrar la entrega correspondiente
                for (int i = 0; i < num_deliveries; i++) {
                    if (atoi(deliveries[i].id + 1) == vehicles[v].deliveries[d]) {
                        delivery_idx = i;
                        break;
                    }
                }
                
                if (delivery_idx != -1) {
                    // Calcular distancia desde la posición actual al origen de la entrega
                    double dist_to_origin = calculate_distance(
                        current_x, current_y,
                        deliveries[delivery_idx].origin_x, deliveries[delivery_idx].origin_y
                    );
                    
                    // Calcular distancia desde el origen al destino de la entrega
                    double dist_to_dest = calculate_distance(
                        deliveries[delivery_idx].origin_x, deliveries[delivery_idx].origin_y,
                        deliveries[delivery_idx].destination_x, deliveries[delivery_idx].destination_y
                    );
                    
                    // Actualizar distancia total
                    vehicle_distance += (dist_to_origin + dist_to_dest);
                    
                    // Actualizar posición actual
                    current_x = deliveries[delivery_idx].destination_x;
                    current_y = deliveries[delivery_idx].destination_y;
                    
                    // Sumar tiempo utilizado (viaje + duración de la entrega)
                    int travel_time = calculate_travel_time(dist_to_origin + dist_to_dest);
                    used_vehicle_minutes += (travel_time + deliveries[delivery_idx].duration);
                }
            }
            
            // Actualizar distancia total
            metrics->total_distance += vehicle_distance;
        }
    }
    
    // Calcular satisfacción del cliente (porcentaje de prioridad completada)
    if (max_possible_priority > 0) {
        metrics->customer_satisfaction = (double)total_priority / max_possible_priority * 100.0;
    }
    
    // Calcular utilización de recursos (porcentaje de tiempo utilizado)
    if (total_vehicle_minutes > 0) {
        metrics->resource_utilization = (double)used_vehicle_minutes / total_vehicle_minutes * 100.0;
    }
}




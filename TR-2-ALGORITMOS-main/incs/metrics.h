#ifndef METRICS_H
#define METRICS_H

#include "simulation.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Estructura para almacenar los resultados de las métricas
typedef struct {
    int completed_deliveries;     // Número de entregas completadas
    int total_waiting_time;       // Tiempo total de espera
    double total_distance;        // Distancia total recorrida
    double customer_satisfaction; // Satisfacción del cliente
    double resource_utilization;  // Utilización de recursos
    double execution_time;        // Tiempo de ejecución del algoritmo
} Metrics;

// Inicializar métricas
void init_metrics(Metrics* metrics);

// Actualizar métricas basadas en los resultados de la asignación
void update_metrics(Metrics* metrics, Delivery* deliveries, int num_deliveries, 
                   Vehicle* vehicles, int num_vehicles, double exec_time);

// Imprimir métricas de rendimiento
void print_metrics(Metrics* metrics);

#endif
#include "save.h"
#include "metrics.h"
#include "aux.h"
#include "scheduling_algo.h"

void save_all_results(const char* deliveries_file, const char* metrics_file, 
    Delivery* deliveries, int num_deliveries,
    Metrics* metrics) 
{
    // Guardar resultados de asignación de entregas
    save_results(deliveries_file, deliveries, num_deliveries, metrics);

    // Guardar métricas en archivo separado
    export_metrics_csv(metrics_file, metrics);
}

void save_results(const char* file, Delivery* deliveries, int num_deliveries, Metrics* metrics) 
{
    FILE* file_ptr = fopen(file, "w");
    if (!file_ptr) {
        printf("Error: No se pudo crear el archivo %s\n", file);
        return;
    }

    // Escribir encabezado
    fprintf(file_ptr, "ID_Entrega,Vehículo_Asignado,Hora_Inicio,Duración,Prioridad,Coordenada_Origen_X,Coordenada_Origen_Y,Coordenada_Destino_X,Coordenada_Destino_Y\n");

    // Escribir datos de entregas asignadas
    for (int i = 0; i < num_deliveries; i++) {
        if (deliveries[i].assigned) {
            int time_hour = deliveries[i].start_time / 60;
            int time_min = deliveries[i].start_time % 60;

            fprintf(file_ptr, "%s,V%03d,%02d:%02d,%d,%d,%.2f,%.2f,%.2f,%.2f\n", 
                deliveries[i].id, 
                deliveries[i].assigned_vehicle,
                time_hour, time_min,
                deliveries[i].duration,
                deliveries[i].priority,
                deliveries[i].origin_x,
                deliveries[i].origin_y,
                deliveries[i].destination_x,
                deliveries[i].destination_y);
        }
    }

    // Escribir métricas al final del mismo archivo
    fprintf(file_ptr, "\nMÉTRICAS DE RENDIMIENTO\n");
    fprintf(file_ptr, "Entregas_Completadas,%d\n", metrics->completed_deliveries);
    fprintf(file_ptr, "Tiempo_Total_Espera,%d\n", metrics->total_waiting_time);
    fprintf(file_ptr, "Distancia_Total,%.2f\n", metrics->total_distance);
    fprintf(file_ptr, "Satisfacción_Cliente,%.2f\n", metrics->customer_satisfaction);
    fprintf(file_ptr, "Utilización_Recursos,%.2f\n", metrics->resource_utilization);
    fprintf(file_ptr, "Tiempo_Ejecución,%.4f\n", metrics->execution_time);

    fclose(file_ptr);
    printf("Resultados y métricas exportados a %s\n", file);
}

void export_metrics_csv(const char* file, Metrics* metrics) 
{
    FILE* file_ptr = fopen(file, "a"); 
    if (!file_ptr) {
        printf("Error: No se pudo crear el archivo %s\n", file);
        return;
    }

    // Escribir métricas al final
    fprintf(file_ptr, "\nMÉTRICAS DE RENDIMIENTO\n");
    fprintf(file_ptr, "Entregas_Completadas,%d\n", metrics->completed_deliveries);
    fprintf(file_ptr, "Tiempo_Total_Espera,%d\n", metrics->total_waiting_time);
    fprintf(file_ptr, "Distancia_Total,%.2f\n", metrics->total_distance);
    fprintf(file_ptr, "Satisfacción_Cliente,%.2f\n", metrics->customer_satisfaction);
    fprintf(file_ptr, "Utilización_Recursos,%.2f\n", metrics->resource_utilization);
    fprintf(file_ptr, "Tiempo_Ejecución,%.4f\n", metrics->execution_time);

    fclose(file_ptr);
    printf("Métricas exportadas a %s\n", file);
}

void save_results_with_metrics(const char* file, Delivery* deliveries, int num_deliveries, 
    Metrics* metrics) 
{
    save_results(file, deliveries, num_deliveries, metrics);

    char metrics_file[256];
    strcpy(metrics_file, file);

    char* ext = strrchr(metrics_file, '.');
    if (ext) {
        *ext = '\0'; 
    }

    strcat(metrics_file, "_metrics.csv");

    export_metrics_csv(metrics_file, metrics);
}


void save_comparison_results(const char* file, Delivery* deliveries, int num_deliveries, 
                             Vehicle* vehicles, int num_vehicles, 
                             TrafficSystem* traffic_system, 
                             DriverBreak* driver_breaks) {
    const int num_algorithms = 3;
    const char* algorithm_names[] = {
        "Earliest Deadline First",
        "Shortest Processing Time",
        "Nearest Neighbor"
    };

    Metrics metrics_array[num_algorithms];
    
    // Ejecutar todos los algoritmos con los nuevos parámetros
    run_all_algorithms(deliveries, num_deliveries, vehicles, num_vehicles, 
                       metrics_array, traffic_system, driver_breaks);

    FILE* file_ptr = fopen(file, "w");
    if (!file_ptr) {
        printf("Error: No se pudo crear el archivo %s\n", file);
        return;
    }

    fprintf(file_ptr, "COMPARACIÓN DE ALGORITMOS DE PLANIFICACIÓN\n");
    fprintf(file_ptr, "Fecha: %s\n\n", get_current_date_time());

    for (int i = 0; i < num_algorithms; i++) {
        reset_simulation(deliveries, num_deliveries, vehicles, num_vehicles);

        fprintf(file_ptr, "=== Algoritmo: %s ===\n", algorithm_names[i]);

        int completed_deliveries = 0;
        switch(i) {
            case 0:
                completed_deliveries = earliest_deadline_first(
                    deliveries, num_deliveries, vehicles, num_vehicles, 
                    &metrics_array[i], traffic_system, driver_breaks);
                break;
            case 1:
                completed_deliveries = shortest_processing_time(
                    deliveries, num_deliveries, vehicles, num_vehicles, 
                    &metrics_array[i], traffic_system, driver_breaks);
                break;
            case 2:
                completed_deliveries = nearest_neighbor(
                    deliveries, num_deliveries, vehicles, num_vehicles, 
                    &metrics_array[i], traffic_system, driver_breaks);
                break;
        }

        fprintf(file_ptr, "Entregas completadas: %d\n\n", completed_deliveries);
        fprintf(file_ptr, "ID_Entrega,Vehículo_Asignado,Hora_Inicio,Duración,Prioridad,Coordenada_Origen_X,Coordenada_Origen_Y,Coordenada_Destino_X,Coordenada_Destino_Y\n");

        for (int j = 0; j < num_deliveries; j++) {
            if (deliveries[j].assigned) {
                int time_hour = deliveries[j].start_time / 60;
                int time_min = deliveries[j].start_time % 60;

                fprintf(file_ptr, "%s,V%03d,%02d:%02d,%d,%d,%.2f,%.2f,%.2f,%.2f\n", 
                        deliveries[j].id, deliveries[j].assigned_vehicle,
                        time_hour, time_min,
                        deliveries[j].duration,
                        deliveries[j].priority,
                        deliveries[j].origin_x,
                        deliveries[j].origin_y,
                        deliveries[j].destination_x,
                        deliveries[j].destination_y);
            }
        }

        fprintf(file_ptr, "\nMÉTRICAS DE RENDIMIENTO:\n");
        fprintf(file_ptr, "Entregas_Completadas,%d\n", metrics_array[i].completed_deliveries);
        fprintf(file_ptr, "Tiempo_Total_Espera,%d\n", metrics_array[i].total_waiting_time);
        fprintf(file_ptr, "Distancia_Total,%.2f\n", metrics_array[i].total_distance);
        fprintf(file_ptr, "Satisfacción_Cliente,%.2f\n", metrics_array[i].customer_satisfaction);
        fprintf(file_ptr, "Utilización_Recursos,%.2f\n", metrics_array[i].resource_utilization);
        fprintf(file_ptr, "Tiempo_Ejecución,%.4f\n\n", metrics_array[i].execution_time);
    }

    fprintf(file_ptr, "\n=== TABLA COMPARATIVA DE ALGORITMOS ===\n");
    fprintf(file_ptr, "------------------------------------------------------------\n");
    fprintf(file_ptr, "%-25s,%-10s,%-10s,%-10s,%-10s\n", 
           "Algoritmo", "Completadas", "Distancia", "Satisfacción", "Tiempo Ejec.");
    fprintf(file_ptr, "------------------------------------------------------------\n");

    for (int i = 0; i < num_algorithms; i++) {
        fprintf(file_ptr, "%-25s,%10d,%10.2f,%10.2f%%,%10.6f\n", 
                algorithm_names[i], 
                metrics_array[i].completed_deliveries,
                metrics_array[i].total_distance, 
                metrics_array[i].customer_satisfaction,
                metrics_array[i].execution_time);
    }

    fprintf(file_ptr, "------------------------------------------------------------\n");

    int best_completions = 0, best_distance = 0, best_satisfaction = 0, best_time = 0;

    for (int i = 1; i < num_algorithms; i++) {
        if (metrics_array[i].completed_deliveries > metrics_array[best_completions].completed_deliveries) best_completions = i;
        if (metrics_array[i].total_distance < metrics_array[best_distance].total_distance &&
            metrics_array[i].completed_deliveries > 0) best_distance = i;
        if (metrics_array[i].customer_satisfaction > metrics_array[best_satisfaction].customer_satisfaction) best_satisfaction = i;
        if (metrics_array[i].execution_time < metrics_array[best_time].execution_time &&
            metrics_array[i].completed_deliveries > 0) best_time = i;
    }

    fprintf(file_ptr, "\nCONCLUSIONES - MEJORES ALGORITMOS POR CATEGORÍA:\n");
    fprintf(file_ptr, "- Mayor número de entregas: %s (%d)\n", algorithm_names[best_completions], metrics_array[best_completions].completed_deliveries);
    fprintf(file_ptr, "- Menor distancia total: %s (%.2f km)\n", algorithm_names[best_distance], metrics_array[best_distance].total_distance);
    fprintf(file_ptr, "- Mayor satisfacción del cliente: %s (%.2f%%)\n", algorithm_names[best_satisfaction], metrics_array[best_satisfaction].customer_satisfaction);
    fprintf(file_ptr, "- Menor tiempo de ejecución: %s (%.6f s)\n", algorithm_names[best_time], metrics_array[best_time].execution_time);

    fclose(file_ptr);
    printf("Comparación de algoritmos exportada a %s\n\n", file);
}
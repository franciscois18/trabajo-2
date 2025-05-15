#include "display.h"
#include "metrics.h"

// Mostrar el menú principal
void show_menu() {
    printf("     --SISTEMA DE OPTIMIZACIÓN DE ENTREGAS--      \n\n");
    printf("1. Ejecutar algoritmo Earliest Deadline First (EDF)\n");
    printf("2. Ejecutar algoritmo Nearest Neighbor (NN)\n");
    printf("3. Ejecutar algoritmo Shortest Processing Time\n");
    printf("4. Mostrar datos actuales\n");
    printf("5. Realizar y Guardar Comparacion algoritmos\n\n");
    printf("6. Guardar resultados individuales\n");
    printf("0. Salir\n");
    printf("Seleccione una opción: ");
}





//
void display_deliveries(Delivery* deliveries, int num_deliveries) {
    printf("\n----- LISTA DE ENTREGAS -----\n");
    printf("%-5s %-10s %-10s %-10s %-10s %-10s %-10s %-8s %-8s %-5s\n", 
           "ID", "Origen X", "Origen Y", "Destino X", "Destino Y", 
           "Inicio", "Fin", "Duración", "Prioridad", "Asignado");
    
    for (int i = 0; i < num_deliveries; i++) {
        Delivery* d = &deliveries[i];
        printf("%-5s %-10.2f %-10.2f %-10.2f %-10.2f %02d:%02d      %02d:%02d      %-8d %-8d %-5s\n", 
               d->id, d->origin_x, d->origin_y, d->destination_x, d->destination_y,
               d->start_hour, d->start_min, d->end_hour, d->end_min,
               d->duration, d->priority,
               d->assigned ? "Sí" : "No");
    }
    printf("\n");
}

// Mostrar todos los vehiculos
void display_vehicles(Vehicle* vehicles, int num_vehicles) {
    printf("\n----- LISTA DE VEHÍCULOS -----\n");
    printf("%-5s %-6s %-10s %-10s %-10s %-10s %-8s %-8s\n", 
           "ID", "Tipo", "Vol. Cap.", "Peso Cap.", "Pos X", "Pos Y", "Inicio", "Fin");
    
    for (int i = 0; i < num_vehicles; i++) {
        Vehicle* v = &vehicles[i];
        printf("%-5s %-6d %-10.2f %-10.2f %-10.2f %-10.2f %02d:%02d    %02d:%02d\n", 
               v->id, v->type, v->volume_capacity, v->weight_capacity,
               v->pos_x, v->pos_y, v->start_hour, v->start_min,
               v->end_hour, v->end_min);
    }
    printf("\n");
}

// Imprimir métricas de rendimiento
void print_metrics(Metrics* metrics) {
    if (!metrics) return;
    
    printf("             MÉTRICAS DE RENDIMIENTO                 \n");
    printf("Entregas completadas: %d\n", metrics->completed_deliveries);
    printf("Tiempo total de espera: %d minutos\n", metrics->total_waiting_time);
    printf("Distancia total recorrida: %.2f km\n", metrics->total_distance);
    printf("Satisfacción del cliente: %.2f%%\n", metrics->customer_satisfaction);
    printf("Utilización de recursos: %.2f%%\n", metrics->resource_utilization);
    printf("Tiempo de ejecución: %.4f segundos\n\n", metrics->execution_time);

}


// Mostrar un informe de descansos de conductores
void print_driver_breaks_report(DriverBreak* driver_breaks, int num_vehicles) {
    printf("\n--- Informe de Descansos de Conductores ---\n");
    printf("ID Vehículo | Tiempo Continuo | Tiempo Total | Descansos\n");
    printf("-----------+----------------+-------------+---------\n");
    
    for (int i = 0; i < num_vehicles; i++) {
        if (driver_breaks[i].vehicle_id != 0) {
            printf("    %2d      |      %3d min     |    %3d min   |    %d\n", 
                  driver_breaks[i].vehicle_id,
                  driver_breaks[i].continuous_work_time,
                  driver_breaks[i].total_work_time,
                  driver_breaks[i].breaks_taken);
        }
    }
    printf("-----------------------------------------------\n\n");
}

// Mostrar zonas de tráfico activas
void print_traffic_zones(TrafficSystem* traffic_system) {
    printf("\n--- Zonas de Tráfico Activas ---\n");
    printf("ID | Centro (X,Y) | Radio | Factor Velocidad | Ventanas de Tiempo\n");
    printf("---+-------------+-------+------------------+-------------------\n");
    
    for (int i = 0; i < traffic_system->num_zones; i++) {
        TrafficZone* zone = &traffic_system->zones[i];
        printf("%2d | (%5.1f,%5.1f) | %5.1f |      %.2f        | ", 
              zone->id, zone->center_x, zone->center_y, 
              zone->radius, zone->speed_factor);
        
        for (int j = 0; j < zone->num_time_windows; j++) {
            if (j > 0) printf(", ");
            printf("%02d:%02d-%02d:%02d", 
                  zone->time_windows[j].start_hour, zone->time_windows[j].start_min,
                  zone->time_windows[j].end_hour, zone->time_windows[j].end_min);
        }
        printf("\n");
    }
    printf("----------------------------------------------------------\n\n");
}
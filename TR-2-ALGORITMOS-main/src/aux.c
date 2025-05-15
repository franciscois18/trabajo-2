#include "aux.h"
#include "metrics.h"
#include "scheduling_algo.h"
#include "optimization.h"



// Limpiar buffer entrada
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Convertir horas y minutos a minutos desde el inicio del día
int convert_to_minutes(int hour, int min) {
    return hour * 60 + min;
}

// Calcular distancia euclidiana entre dos puntos
double calculate_distance(double x1, double y1, double x2, double y2) {
    return sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));
}
// Calcular tiempo de viaje en minutos 
int calculate_travel_time(double distance) {
    return (int)(distance * 60 / 50);
}


// Verificar si un vehículo puede realizar una entrega
int can_make_delivery(Vehicle* vehicle, Delivery* delivery, int current_time) {
    // Verificar tipo de vehículo
    if (vehicle->type < delivery->vehicle_type) {
        return 0;
    }
    
    // Verificar capacidad
    if (vehicle->current_volume + delivery->volume > vehicle->volume_capacity ||
        vehicle->current_weight + delivery->weight > vehicle->weight_capacity) {
        return 0;
    }
    
    // Calcular tiempo de viaje hasta el origen de la entrega
    double distance = calculate_distance(
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y
    );
    int travel_time = calculate_travel_time(distance);
    
    // Tiempo de llegada al origen
    int arrival_time = current_time + travel_time;
    
    // Tiempo de inicio y fin de la entrega en minutos
    int delivery_start = convert_to_minutes(delivery->start_hour, delivery->start_min);
    int delivery_end = convert_to_minutes(delivery->end_hour, delivery->end_min);
    
    // Verificar si llegaríamos a tiempo
    if (arrival_time > delivery_end) {
        return 0; // Llegaríamos tarde
    }
    
    // Si llegamos antes del inicio, tenemos que esperar
    int real_start_time = (arrival_time < delivery_start) ? delivery_start : arrival_time;
    
    // Tiempo de finalización de la entrega
    int delivery_end_time = real_start_time + delivery->duration;
    
    // Verificar si terminaríamos dentro del horario laboral del conductor
    int end_of_day = convert_to_minutes(vehicle->end_hour, vehicle->end_min);
    if (delivery_end_time > end_of_day) {
        return 0; // No alcanzaríamos a terminar
    }
    
    return 1; 
}


// Función de comparación para ordenar entregas por fecha límite (EDF)
int compare_deadline(const void* a, const void* b) {
    Delivery* d1 = (Delivery*)a;
    Delivery* d2 = (Delivery*)b;
    
    // Convertir los tiempos de finalización a minutos
    int end_time1 = convert_to_minutes(d1->end_hour, d1->end_min);
    int end_time2 = convert_to_minutes(d2->end_hour, d2->end_min);
    
    return end_time1 - end_time2;
}


// Función de comparación para ordenar entregas por prioridad
int compare_priority(const void* a, const void* b) {
    Delivery* d1 = (Delivery*)a;
    Delivery* d2 = (Delivery*)b;
    
    // Mayor prioridad primero (orden descendente)
    return d2->priority - d1->priority;
}

// Encontrar el vehículo más adecuado para una entrega
int find_best_vehicle(Delivery* delivery, Vehicle* vehicles, int num_vehicles, int current_time) {
    int best_vehicle = -1;
    int min_slack_time = INT_MAX;
    
    for (int v = 0; v < num_vehicles; v++) {
        if (can_make_delivery(&vehicles[v], delivery, current_time)) {
            // Calcular tiempo de viaje hasta el origen
            double distance = calculate_distance(
                vehicles[v].current_pos_x, vehicles[v].current_pos_y,
                delivery->origin_x, delivery->origin_y
            );
            int travel_time = calculate_travel_time(distance);
            
            // Calcular slack time (margen de tiempo disponible)
            int arrival_time = current_time + travel_time;
            int delivery_end = convert_to_minutes(delivery->end_hour, delivery->end_min);
            int slack_time = delivery_end - arrival_time - delivery->duration;
            
            // Encontrar el vehículo con menor slack time (más ajustado al tiempo)
            if (slack_time >= 0 && slack_time < min_slack_time) {
                min_slack_time = slack_time;
                best_vehicle = v;
            }
        }
    }
    
    return best_vehicle;
}

// Actualizar el estado del vehículo después de asignar una entrega
void update_vehicle_state(Vehicle* vehicle, Delivery* delivery) {
    // Calcular tiempo de viaje hasta el origen
    double distance = calculate_distance(
        vehicle->current_pos_x, vehicle->current_pos_y,
        delivery->origin_x, delivery->origin_y
    );
    int travel_time = calculate_travel_time(distance);
    
    // Actualizar posición al destino de la entrega
    vehicle->current_pos_x = delivery->destination_x;
    vehicle->current_pos_y = delivery->destination_y;
    
    // Actualizar tiempo actual
    int arrival_time = vehicle->current_time + travel_time;
    int delivery_start = convert_to_minutes(delivery->start_hour, delivery->start_min);
    
    // Si llegamos antes de la hora de inicio, esperamos
    int real_start_time = (arrival_time < delivery_start) ? delivery_start : arrival_time;
    
    // Actualizar el tiempo actual del vehículo (tiempo de llegada + duración de la entrega)
    vehicle->current_time = real_start_time + delivery->duration;
    
    // Actualizar capacidades
    vehicle->current_volume += delivery->volume;
    vehicle->current_weight += delivery->weight;
    
    // Registrar la entrega en el vehículo
    if (vehicle->num_deliveries < MAX_DELIVERIES_PER_VEHICLE) {
        vehicle->deliveries[vehicle->num_deliveries] = atoi(delivery->id + 1); // Eliminar la 'E' del ID
        vehicle->num_deliveries++;
    }
    
    // Actualizar la entrega
    delivery->assigned = 1;
    delivery->assigned_vehicle = atoi(vehicle->id + 1); // Eliminar la 'V' del ID
    delivery->start_time = real_start_time;
}

// Comparar entregas por distancia a un punto de referencia
int compare_distance(const void* a, const void* b) {
    return ((Distance*)a)->dist > ((Distance*)b)->dist ? 1 : -1;
}

// comparar entregas por tiempo de procesamiento 
int compare_processing_time(const void* a, const void* b) {
    const Delivery* delivery_a = (const Delivery*)a;
    const Delivery* delivery_b = (const Delivery*)b;
    
    // Ordenar por duración (de menor a mayor)
    return delivery_a->duration - delivery_b->duration;
}


// obtener la fecha y hora actual formato 24 horita
char* get_current_date_time() {
    static char buffer[30];
    time_t rawtime;
    struct tm* timeinfo;
    
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
    return buffer;
}

void run_all_algorithms(Delivery* deliveries, int num_deliveries,
                       Vehicle* vehicles, int num_vehicles,
                       Metrics* metrics_array,
                       TrafficSystem* traffic_system,
                       DriverBreak* driver_breaks) {
    
    Delivery* temp_deliveries = (Delivery*)malloc(num_deliveries * sizeof(Delivery));
    if (!temp_deliveries) {
        printf("Error: Memoria insuficiente para comparar algoritmos\n");
        return;
    }
    
    Vehicle* temp_vehicles = (Vehicle*)malloc(num_vehicles * sizeof(Vehicle));
    if (!temp_vehicles) {
        free(temp_deliveries);
        printf("Error: Memoria insuficiente para comparar algoritmos\n");
        return;
    }
    

    for (int i = 0; i < 3; i++) {  

        memcpy(temp_deliveries, deliveries, num_deliveries * sizeof(Delivery));
        memcpy(temp_vehicles, vehicles, num_vehicles * sizeof(Vehicle));

        init_metrics(&metrics_array[i]);
        

        switch (i) {
            case 0: // EDF
                earliest_deadline_first(temp_deliveries, num_deliveries, temp_vehicles, num_vehicles, 
                                       &metrics_array[i], traffic_system, driver_breaks);
                break;
            case 1: // SPT
                shortest_processing_time(temp_deliveries, num_deliveries, temp_vehicles, num_vehicles, 
                                        &metrics_array[i], traffic_system, driver_breaks);
                break;
            case 2: // NN
                nearest_neighbor(temp_deliveries, num_deliveries, temp_vehicles, num_vehicles, 
                               &metrics_array[i], traffic_system, driver_breaks);
                break;
        }
        
        update_metrics(&metrics_array[i], temp_deliveries, num_deliveries, temp_vehicles, num_vehicles, 
                      metrics_array[i].execution_time);
    }
    
    free(temp_deliveries);
    free(temp_vehicles);
}


void compare_algorithms(Delivery* deliveries, int num_deliveries,
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
    // Pasa los nuevos parámetros a run_all_algorithms
    run_all_algorithms(deliveries, num_deliveries, vehicles, num_vehicles, metrics_array, traffic_system, driver_breaks);

    printf("\n=== Comparación de algoritmos ===\n");
    printf("---------------------------------------------------------------------------\n"); 
    printf("%-25s | %12s | %12s | %15s | %15s\n",
           "Algoritmo", "Completadas", "Dist. Total", "Satisfacción (%)", "Tiempo Ejec. (s)");
    printf("---------------------------------------------------------------------------\n");

    for (int i = 0; i < num_algorithms; i++) {
        printf("%-25s | %12d | %12.2f | %15.2f | %15.6f\n",
               algorithm_names[i],
               metrics_array[i].completed_deliveries,
               metrics_array[i].total_distance,
               metrics_array[i].customer_satisfaction,
               metrics_array[i].execution_time);
    }

    printf("---------------------------------------------------------------------------\n");

    int best_completions_idx = 0;
    int best_distance_idx = 0;
    int best_satisfaction_idx = 0;
    int best_time_idx = 0;


    for (int i = 0; i < num_algorithms; i++) {
        if (metrics_array[i].completed_deliveries > 0) {
            best_distance_idx = i;
            best_time_idx = i;
            break; 
        }
    }


    for (int i = 0; i < num_algorithms; i++) { 
        // Mejor para entregas completadas
        if (metrics_array[i].completed_deliveries > metrics_array[best_completions_idx].completed_deliveries) {
            best_completions_idx = i;
        }

        // Mejor para distancia (menor es mejor), solo si hay entregas completadas
        if (metrics_array[i].completed_deliveries > 0) {
            if (metrics_array[best_distance_idx].completed_deliveries == 0 || // Si el actual 'mejor' no tiene entregas
                metrics_array[i].total_distance < metrics_array[best_distance_idx].total_distance) {
                best_distance_idx = i;
            }
        }
        
        // Mejor para satisfacción (mayor es mejor)
        if (metrics_array[i].customer_satisfaction > metrics_array[best_satisfaction_idx].customer_satisfaction) {
            best_satisfaction_idx = i;
        }

        // Mejor para tiempo de ejecución (menor es mejor), solo si hay entregas completadas
        if (metrics_array[i].completed_deliveries > 0) {
             if (metrics_array[best_time_idx].completed_deliveries == 0 || 
                metrics_array[i].execution_time < metrics_array[best_time_idx].execution_time) {
                best_time_idx = i;
            }
        }
    }

    printf("\nMejores algoritmos:\n");
    printf("- Mayor número de entregas: %s (%d)\n", algorithm_names[best_completions_idx], metrics_array[best_completions_idx].completed_deliveries);
    
    if (metrics_array[best_distance_idx].completed_deliveries > 0) {
        printf("- Menor distancia total: %s (%.2f km)\n", algorithm_names[best_distance_idx], metrics_array[best_distance_idx].total_distance);
    } else {
        printf("- Menor distancia total: N/A (ningún algoritmo completó entregas)\n");
    }
    
    printf("- Mayor satisfacción del cliente: %s (%.2f%%)\n", algorithm_names[best_satisfaction_idx], metrics_array[best_satisfaction_idx].customer_satisfaction);

    if (metrics_array[best_time_idx].completed_deliveries > 0) {
        printf("- Menor tiempo de ejecución: %s (%.6f s)\n\n", algorithm_names[best_time_idx], metrics_array[best_time_idx].execution_time);
    } else {
        printf("- Menor tiempo de ejecución: N/A (ningún algoritmo completó entregas)\n\n");
    }
}


// Obtener la zona de tráfico para una ubicación
int get_traffic_zone_for_location(TrafficSystem* traffic_system, 
                                 double origin_x, double origin_y, 
                                 double dest_x, double dest_y) {

    for (int i = 0; i < traffic_system->num_zones; i++) {
        TrafficZone* zone = &traffic_system->zones[i];
        
        // Verificar si el origen o el destino están en esta zona de tráfico
        if (is_in_traffic_zone(origin_x, origin_y, zone) || 
            is_in_traffic_zone(dest_x, dest_y, zone)) {
            return zone->id;
        }
    }
    
    return -1; // No está en ninguna zona de tráfico
}

// Obtener el factor de tráfico para una zona y tiempo determinados
double get_traffic_factor(TrafficSystem* traffic_system, int zone_id, int current_time) {
    // Buscar la zona por ID
    for (int i = 0; i < traffic_system->num_zones; i++) {
        if (traffic_system->zones[i].id == zone_id) {
            // Verificar si estamos en una ventana de tiempo de restricción
            if (is_in_restricted_time_window(current_time, &traffic_system->zones[i])) {
                return traffic_system->zones[i].speed_factor;
            }
            break;
        }
    }
    
    // Si no estamos en una ventana de tiempo de restricción, factor = 1.0 (normal)
    return 1.0;
}



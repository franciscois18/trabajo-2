#include "aux.h"
#include "delivery.h"
#include "vehicle.h"
#include "database.h"
#include "display.h"  
#include "metrics.h"
#include "simulation.h"
#include "save.h"
#include "optimization.h"
#include "traffic_zones.h"
#include "driver_breaks.h"

#include "scheduling_algo.h"

int main() {
    // Carga estructuras
    Delivery deliveries[MAX_DELIVERIES];
    Vehicle vehicles[MAX_VEHICLES];
    Metrics metrics;
    
    TrafficSystem traffic_system;
    DriverBreak driver_breaks[MAX_VEHICLES];
    
    int option;
    bool running = true;

    // Inicializar métricas
    init_metrics(&metrics);

    // Cargar datos desde archivos
    int num_deliveries = load_deliveries("database/deliveries.csv", deliveries, MAX_DELIVERIES);
    int num_vehicles = load_vehicles("database/vehicles.csv", vehicles, MAX_VEHICLES);
    
    // Inicializar sistema de tráfico
    init_traffic_system(&traffic_system);
    init_driver_breaks(driver_breaks, vehicles, num_vehicles);
    
    // Cargar zonas de tráfico y ventanas de tiempo
    load_traffic_zones("database/traffic_zones.csv", &traffic_system);
    load_traffic_time_windows("database/time_windows.csv", &traffic_system);
    
    if (!validate_data(deliveries, num_deliveries, vehicles, num_vehicles)) {
        printf("Validación de datos fallida. Corrija los errores e intente nuevamente.\n");
        return 1;
    }
    
    printf("Datos cargados: %d entregas, %d vehículos\n", num_deliveries, num_vehicles);
    
    
    while (running) {
        // Mostrar menú
        show_menu();
        
        // Leer opción del usuario
        if (scanf("%d", &option) != 1) {
            fprintf(stderr, "Error: entrada no válida.\n");
            clear_input_buffer();
            continue; 
        }
        
        switch(option) {
            case 1:
                printf("\nEjecutando algoritmo Earliest Deadline First...\n");
                reset_simulation(deliveries, num_deliveries, vehicles, num_vehicles);
                init_metrics(&metrics); 
    
                earliest_deadline_first(deliveries, num_deliveries, vehicles, num_vehicles, &metrics, &traffic_system, driver_breaks);


                apply_optimizations(deliveries, num_deliveries, vehicles, num_vehicles, &traffic_system, driver_breaks);
                
                update_metrics(&metrics, deliveries, num_deliveries, vehicles, num_vehicles, 0);
                print_metrics(&metrics);
                break;
            
            case 2:
                printf("\nEjecutando algoritmo Nearest Neighbor...\n");
                reset_simulation(deliveries, num_deliveries, vehicles, num_vehicles);
                init_metrics(&metrics); 

                nearest_neighbor(deliveries, num_deliveries, vehicles, num_vehicles, &metrics, &traffic_system, driver_breaks);
                

                apply_optimizations(deliveries, num_deliveries, vehicles, num_vehicles, &traffic_system, driver_breaks);

                update_metrics(&metrics, deliveries, num_deliveries, vehicles, num_vehicles, 0); 
                print_metrics(&metrics);
                break;
        
            case 3:
                printf("\nEjecutando algoritmo Shortest Processing Time...\n");
                reset_simulation(deliveries, num_deliveries, vehicles, num_vehicles);
                init_metrics(&metrics); 
                shortest_processing_time(deliveries, num_deliveries, vehicles, num_vehicles, &metrics, &traffic_system, driver_breaks);
                apply_optimizations(deliveries, num_deliveries, vehicles, num_vehicles, &traffic_system, driver_breaks);
                
                update_metrics(&metrics, deliveries, num_deliveries, vehicles, num_vehicles, 0);
                print_metrics(&metrics);
                break;
                        
            case 4:
                printf("\nMostrando datos actuales...\n");
                display_deliveries(deliveries, num_deliveries);
                display_vehicles(vehicles, num_vehicles);
                // Mostrar información del sistema de tráfico y descansos
                print_traffic_zones(&traffic_system);
                print_driver_breaks_report(driver_breaks, num_vehicles);
                break;
                        
            case 5:
                printf("\nRealizando comparación y guardado de algoritmos...\n");
                save_comparison_results("results/algorithms_comparison.csv", deliveries, num_deliveries, 
                                       vehicles, num_vehicles, &traffic_system, driver_breaks);
                break;
            
            case 6:
                printf("\nGuardando resultados...\n");
                save_results_with_metrics("results/assignment_results.csv", deliveries, num_deliveries, &metrics);
                printf("Resultados guardados exitosamente.\n\n");
                break;
        
            case 0:
                printf("Saliendo del programa...\n");
                running = false;
                break;
        
            default:
                printf("Opción no válida.\n");
        }
    }

    return 0;
}
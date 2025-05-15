#include "simulation.h"
#include "aux.h"

void reset_simulation(Delivery* deliveries, int num_deliveries, Vehicle* vehicles, int num_vehicles) {


    for (int i = 0; i < num_deliveries; i++) {
        deliveries[i].assigned = 0;
        deliveries[i].assigned_vehicle = -1;
        deliveries[i].start_time = 0;
    }
    

    for (int i = 0; i < num_vehicles; i++) {
       
        vehicles[i].current_pos_x = vehicles[i].pos_x;
        vehicles[i].current_pos_y = vehicles[i].pos_y;
        
        
        vehicles[i].current_volume = 0;
        vehicles[i].current_weight = 0;
        
        
        vehicles[i].current_time = convert_to_minutes(vehicles[i].start_hour, vehicles[i].start_min);
        
       
        vehicles[i].num_deliveries = 0;
    }
    
   
}
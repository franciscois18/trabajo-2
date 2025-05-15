#ifndef TRAFFIC_ZONES_H
#define TRAFFIC_ZONES_H

#include "delivery.h"
#include "vehicle.h"

#define MAX_TRAFFIC_ZONES 15
#define MAX_TIME_WINDOWS_PER_ZONE 6

typedef struct {
    int start_hour, start_min;
    int end_hour, end_min;
} TimeWindow;

typedef struct {
    int id;
    double center_x, center_y, radius;
    double speed_factor;
    TimeWindow time_windows[MAX_TIME_WINDOWS_PER_ZONE];
    int num_time_windows;
} TrafficZone;

typedef struct {
    TrafficZone zones[MAX_TRAFFIC_ZONES];
    int num_zones;
} TrafficSystem;

// Inicialización y carga
void init_traffic_system(TrafficSystem* ts);
int  load_traffic_zones(const char* filename, TrafficSystem* ts);
int  load_traffic_time_windows(const char* filename, TrafficSystem* ts);

// Consultas de zona y factor
int is_in_traffic_zone(double x, double y, TrafficZone* zone);

int is_in_restricted_time_window(int time_min, TrafficZone* zone);

double calculate_speed_factor(double sx, double sy,
                              double ex, double ey,
                              int current_time,
                              TrafficSystem* ts);
                              
int calculate_travel_time_with_traffic(double distance,
                                          int current_time,
                                          double sx, double sy,
                                          double ex, double ey,
                                          TrafficSystem* ts);

#endif 

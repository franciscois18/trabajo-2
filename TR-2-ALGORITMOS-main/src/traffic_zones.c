#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "traffic_restrictions.h"
#include "traffic_zones.h"
#include "aux.h"

// Inicializar el sistema de tráfico
void init_traffic_system(TrafficSystem* ts) {
    printf("Inicializando sistema de restricciones de tráfico...\n");
    ts->num_zones = 0;
}

// Cargar zonas de tráfico desde un archivo
int load_traffic_zones(const char* filename, TrafficSystem* ts) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se pudo abrir %s\n", filename);
        return 0;
    }
    char line[256];
    int count = 0;

    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        printf("Error: Failed to read header line\n");
        return 0;
    }
    while (fgets(line, sizeof(line), file) && count < MAX_TRAFFIC_ZONES) {
        TrafficZone* z = &ts->zones[count];
        if (sscanf(line, "%d,%lf,%lf,%lf,%lf",
                   &z->id, &z->center_x, &z->center_y,
                   &z->radius, &z->speed_factor) == 5) {
            z->num_time_windows = 0;
            count++;
        }
    }
    ts->num_zones = count;
    fclose(file);
    printf("Cargadas %d zonas de tráfico\n", count);
    return count;
}

// Cargar ventanas de tiempo para restricciones
int load_traffic_time_windows(const char* filename, TrafficSystem* ts) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: No se pudo abrir %s\n", filename);
        return 0;
    }
    char line[256];
    int loaded = 0;
    if (fgets(line, sizeof(line), file) == NULL) {
        fclose(file);
        printf("Error: Failed to read header line\n");
        return 0;
    }
    while (fgets(line, sizeof(line), file)) {
        int zid, sh, sm, eh, em;
        if (sscanf(line, "%d,%d,%d,%d,%d",
                   &zid, &sh, &sm, &eh, &em) == 5) {
            for (int i = 0; i < ts->num_zones; i++) {
                if (ts->zones[i].id == zid &&
                    ts->zones[i].num_time_windows < MAX_TIME_WINDOWS_PER_ZONE) {
                    TimeWindow* tw = &ts->zones[i]
                                      .time_windows[ts->zones[i].num_time_windows++];
                    tw->start_hour = sh; tw->start_min = sm;
                    tw->end_hour   = eh; tw->end_min   = em;
                    loaded++;
                    break;
                }
            }
        }
    }
    fclose(file);
    printf("Cargadas %d ventanas de tiempo\n", loaded);
    return loaded;
}

// Comprueba si (x,y) está en la zona
int is_in_traffic_zone(double x, double y, TrafficZone* zone) {
    double dx = x - zone->center_x, dy = y - zone->center_y;
    return (dx*dx + dy*dy) <= zone->radius*zone->radius;
}

// Comprueba si time_minutes cae en alguna ventana
int is_in_restricted_time_window(int time_minutes, TrafficZone* zone) {
    for (int i = 0; i < zone->num_time_windows; i++) {
        TimeWindow* tw = &zone->time_windows[i];
        int start = convert_to_minutes(tw->start_hour, tw->start_min);
        int end   = convert_to_minutes(tw->end_hour,   tw->end_min);
        if (time_minutes >= start && time_minutes <= end)
            return 1;
    }
    return 0;
}

// Calcula el factor de velocidad más restrictivo aplicable
double calculate_speed_factor(double sx, double sy,
                              double ex, double ey,
                              int current_time,
                              TrafficSystem* ts) {
    double factor = 1.0;
    for (int i = 0; i < ts->num_zones; i++) {
        TrafficZone* z = &ts->zones[i];
        if (is_in_traffic_zone(sx, sy, z) || is_in_traffic_zone(ex, ey, z)) {
            if (is_in_restricted_time_window(current_time, z)
                && z->speed_factor < factor) {
                factor = z->speed_factor;
            }
        }
    }
    return factor;
}

// Ajusta tiempo base según factor de tráfico
int calculate_travel_time_with_traffic(double distance, int current_time,
                                       double sx, double sy,
                                       double ex, double ey,
                                       TrafficSystem* ts) {
    double f = calculate_speed_factor(sx, sy, ex, ey, current_time, ts);
    int base = calculate_travel_time(distance);
    return (int)(base / f);
}
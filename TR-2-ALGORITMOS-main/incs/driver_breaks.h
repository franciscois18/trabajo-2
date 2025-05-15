#ifndef DRIVER_BREAKS_H
#define DRIVER_BREAKS_H

#include "vehicle.h"

#define MAX_CONTINUOUS_WORK_MINUTES 240
#define MIN_BREAK_DURATION          30
#define MAX_DAILY_WORK_MINUTES      600
#define MAX_BREAKS_HISTORY          10

typedef struct {
    int vehicle_id;
    int last_break_time;
    int continuous_work_time;
    int total_work_time;
    int breaks_taken;
} DriverBreak;

// Inicialización
void init_driver_breaks(DriverBreak* db, Vehicle* vehicles, int num_vehicles);

// Lógica de descanso
int  driver_needs_break(int vehicle_id, int current_time,
                        DriverBreak* db, int num_vehicles);
int  schedule_driver_break(int vehicle_id, int current_time,
                           DriverBreak* db, int num_vehicles);
void update_driver_work_time(int vehicle_id, int work_minutes,
                             DriverBreak* db, int num_vehicles);

#endif 
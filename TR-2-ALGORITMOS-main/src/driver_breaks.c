#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "driver_breaks.h"
#include "aux.h"

// Inicializar descansos para cada vehículo
void init_driver_breaks(DriverBreak* db, Vehicle* vehicles, int n) {
    printf("Inicializando descansos de conductores...\n");
    for (int i = 0; i < n; i++) {
        db[i].vehicle_id = atoi(vehicles[i].id + 1);
        db[i].last_break_time = 0;
        db[i].continuous_work_time = 0;
        db[i].total_work_time = 0;
        db[i].breaks_taken = 0;
        printf("  Vehículo %s → ID %d\n",
               vehicles[i].id, db[i].vehicle_id);
    }
}

// Devuelve 1 si necesita break, 2 si ya excedió jornada, 0 si OK
int driver_needs_break(int vehicle_id, int current_time,
                       DriverBreak* db, int n) {
    for (int i = 0; i < n; i++) {
        if (db[i].vehicle_id == vehicle_id) {
            if (db[i].continuous_work_time >= MAX_CONTINUOUS_WORK_MINUTES
                && current_time >= 720 && current_time <= 840) {
                return 1;
            }
            if (db[i].total_work_time >= MAX_DAILY_WORK_MINUTES) {
                return 2;
            }
            return 0;
        }
    }
    return 0;
}

// Programa un descanso y resetea contador contínuo
int schedule_driver_break(int vehicle_id, int current_time,
                          DriverBreak* db, int n) {
    for (int i = 0; i < n; i++) {
        if (db[i].vehicle_id == vehicle_id) {
            db[i].last_break_time      = current_time;
            db[i].continuous_work_time = 0;
            db[i].breaks_taken++;
            printf("Vehículo %d descansa a las %02d:%02d\n",
                   vehicle_id, current_time/60, current_time%60);
            return MIN_BREAK_DURATION;
        }
    }
    return 0;
}

// Actualiza los contadores de trabajo del conductor
void update_driver_work_time(int vehicle_id, int work_minutes,
                             DriverBreak* db, int n) {
    for (int i = 0; i < n; i++) {
        if (db[i].vehicle_id == vehicle_id) {
            db[i].continuous_work_time += work_minutes;
            db[i].total_work_time      += work_minutes;
            return;
        }
    }
}
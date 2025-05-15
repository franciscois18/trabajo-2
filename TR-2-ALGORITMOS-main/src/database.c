#include "database.h"
#include "vehicle.h"
#include "delivery.h"
#include "aux.h"



int load_deliveries(const char* file, Delivery* deliveries, int max_deliveries) {
    FILE* file_ptr = fopen(file, "r");
    if (!file_ptr) {
        printf("Error: No se pudo abrir el archivo %s\n", file);
        return 0;
    }
    
    char line[256];
    int num_deliveries = 0;
    

    if (!fgets(line, sizeof(line), file_ptr)) {
        fclose(file_ptr);
        return 0;
    }
    
    // Leer entregas
    while (fgets(line, sizeof(line), file_ptr) && num_deliveries < max_deliveries) {
        Delivery* d = &deliveries[num_deliveries];
        
        // Parsear la linea CSV
        char* token = strtok(line, ",");
        if (!token) continue;
        
        strcpy(d->id, token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->origin_x = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->origin_y = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->destination_x = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->destination_y = atof(token);
  
        token = strtok(NULL, ",");
        if (!token) continue;
        sscanf(token, "%d:%d", &d->start_hour, &d->start_min);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        sscanf(token, "%d:%d", &d->end_hour, &d->end_min);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->duration = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->priority = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->vehicle_type = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->volume = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        d->weight = atof(token);
        
        // Inicializar valores adicionales
        d->assigned = 0;
        d->assigned_vehicle = -1;
        d->start_time = 0;
        
        num_deliveries++;
    }
    
    fclose(file_ptr);
    return num_deliveries;
}

int load_vehicles(const char* file, Vehicle* vehicles, int max_vehicles) {
    FILE* file_ptr = fopen(file, "r");
    if (!file_ptr) {
        printf("Error: No se pudo abrir el archivo %s\n", file);
        return 0;
    }
    
    char line[256];
    int num_vehicles = 0;
    
    // Leer encabezado
    if (!fgets(line, sizeof(line), file_ptr)) {
        fclose(file_ptr);
        return 0;
    }
    
    // Leer vehiculos
    while (fgets(line, sizeof(line), file_ptr) && num_vehicles < max_vehicles) {
        Vehicle* v = &vehicles[num_vehicles];
        

        char* token = strtok(line, ",");
        if (!token) continue;
        
        strcpy(v->id, token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->type = atoi(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->volume_capacity = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->weight_capacity = atof(token);
        

        token = strtok(NULL, ",");
        if (!token) continue;
        sscanf(token, "%d:%d", &v->start_hour, &v->start_min);
        

        token = strtok(NULL, ",");
        if (!token) continue;
        sscanf(token, "%d:%d", &v->end_hour, &v->end_min);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->pos_x = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->pos_y = atof(token);
        
        token = strtok(NULL, ",");
        if (!token) continue;
        v->specialty = atoi(token);
        
        // Inicializar variables de estado
        v->current_volume = 0;
        v->current_weight = 0;
        v->current_pos_x = v->pos_x;
        v->current_pos_y = v->pos_y;
        v->current_time = convert_to_minutes(v->start_hour, v->start_min);
        v->num_deliveries = 0;
        
        num_vehicles++;
    }
    
    fclose(file_ptr);
    return num_vehicles;
}

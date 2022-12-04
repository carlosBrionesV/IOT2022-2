#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

#include "sensores.h"

float floatrand(float min, float max) {
    return min + (float)rand()/(float)(RAND_MAX/(max-min));
}

int intrand(int min, int max) {
    return min + rand()/(max-min);
}

float *acc_sensor_acc_x() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-16, 16);
    }
    return arr;
}

float *acc_sensor_acc_y() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-16, 16);
    }
    return arr;
}

float *acc_sensor_acc_z() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-16, 16);
    }
    return arr;
}

float *acc_sensor_rgyr_x() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-1000, 1000);
    }
    return arr;
}

float *acc_sensor_rgyr_y() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-1000, 1000);
    }
    return arr;
}

float *acc_sensor_rgyr_z() {
    float *arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++) {
        arr[i] = floatrand(-1000, 1000);
    }
    return arr;
}

int thpc_sensor_temp() {
    return intrand(5, 30);
}

int thpc_sensor_hum() {
    return intrand(30, 80);
}

int thpc_sensor_pres() {
    return intrand(1000, 1200);
}

float thpc_sensor_co() {
    return floatrand(30, 200);
}

float batt_sensor() {
    return floatrand(1, 100);
}

float acc_kpi_amp_x() {
    return floatrand(0.0059, 0.12);
}

float acc_kpi_frec_x() {
    return floatrand(29, 31);
}

float acc_kpi_amp_y() {
    return floatrand(0.0041, 0.11);
}

float acc_kpi_frec_y() {
    return floatrand(59, 61);
}

float acc_kpi_amp_z() {
    return floatrand(0.008, 0.15);
}

float acc_kpi_frec_z() {
    return floatrand(89, 91);
}

float acc_kpi_rms(float x, float y, float z) {
    return sqrtf(pow(x,2) + pow(y,2) + pow(z,2));
}

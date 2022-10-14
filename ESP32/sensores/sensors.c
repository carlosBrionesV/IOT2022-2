#include <math.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

/*

Aqui generamos los 5 tipos de protocolos con sus datos.
Las timestamps en realidad siempre mandamos 0, y por comodidad 
guardamos la timestampo del tiempo de llegada en el servidor de la raspberry.


En general los "mensajes" los creamos copiando a la mala (con memcpy) la memoria de los datos en un char*.
No es muy elegante pero funciona.

Al final lo único que se usa fuera de este archivo es:

message: dado un protocolo, crea un mensaje (con header y datos) codificado en un array de bytes (char*).
messageLength: dado un protocolo, entrega el largo del mensaje correspondiente

*/

#define PI 3.14159265358979323846

float floatrand(float min, float max){
    return min + (float)rand()/(float)(RAND_MAX/(max-min));
}
int intrand(int min, int max){
    return min + rand()/(max-min);
}

float* acc_sensor_accx() {
    float* arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++){
        arr[i] = 2*sinf(2*PI * 0.001 * intrand(-8000, 8000));
    }
    return arr;
}

float* acc_sensor_accy(){
    float* arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++){
        arr[i] = 3*cosf(2*PI * 0.001 * intrand(-8000, 8000));
    }
    return arr;
}

float* acc_sensor_accz(){
    float* arr = malloc(2000 * sizeof(float));
    for (int i = 0; i < 2000; i++){
        arr[i] = 10*sinf(2*PI * 0.001 * intrand(-8000, 8000));
    }
    return arr;
}

float THPC_sensor_temp(){
    return floatrand(5, 30);
}

int THPC_sensor_hum(){
    return intrand(30, 80);
}

int THPC_sensor_pres(){
    return intrand(1000, 1200);
}

float THPC_sensor_co2(){
    return floatrand(30, 200);
}

float batt_sensor() {
    return floatrand(1, 100);
}

float acc_sensor_ampx() {
    return floatrand(0.0059, 0.12);
}

float acc_sensor_frecx() {
    return floatrand(29, 31);
}

float acc_sensor_ampy() {
    return floatrand(0.0041, 0.11);
}

float acc_sensor_frecy() {
    return floatrand(59, 61);
}

float acc_sensor_ampz() {
    return floatrand(0.008, 0.15);
}

float acc_sensor_frecz() {
    return floatrand(89, 91);
}

float acc_sensor_rms() {
    return sqrtf(acc_sensor_ampx()*acc_sensor_ampx() + acc_sensor_ampy()*acc_sensor_ampy() + acc_sensor_ampz()*acc_sensor_ampz());
}

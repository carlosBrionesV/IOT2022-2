#include <stdio.h>
#include <string.h>
#include "esp_log.h"

#include "configuracion.h"
#include "almacenamiento.h"

// from a buffer of bytes, extract and return the value of the configuration
T2_CONFIG extractConfig(char *buffer) {
    T2_CONFIG config;

    // int8_t
    memcpy(&config.STATUS, &buffer[0], sizeof(int8_t));
    memcpy(&config.ID_PROTOCOL, &buffer[1], sizeof(int8_t));

    // int32_t
    memcpy(&config.BMI270_SAMPLING, &buffer[2], sizeof(int32_t));
    memcpy(&config.BMI270_ACC_SENSIBILITY, &buffer[6], sizeof(int32_t));
    memcpy(&config.BMI270_GYRO_SENSIBILITY, &buffer[10], sizeof(int32_t));
    memcpy(&config.BME688_SAMPLING, &buffer[14], sizeof(int32_t));
    memcpy(&config.DISCONTINUOS_TIME, &buffer[18], sizeof(int32_t));
    memcpy(&config.PORT_TCP, &buffer[22], sizeof(int32_t));
    memcpy(&config.PORT_UDP, &buffer[26], sizeof(int32_t));
    memcpy(&config.HOST_IP_ADDR, &buffer[30], sizeof(int32_t));

    // string
    memcpy(&config.SSID, &buffer[34], STRING_SIZE);
    memcpy(&config.PASS, &buffer[66], STRING_SIZE);

    return config;
}

T2_CONFIG readConfig() {
    T2_CONFIG config;

    int8_t *i8Value = (int8_t *) NULL;
    int32_t *i32Value = (int32_t *) NULL;
    char *strValue = (char *) NULL;

    // int8_t
    readStorageValue(STATUS_KEY, &config.STATUS, i32Value, strValue);
    readStorageValue(ID_PROTOCOL_KEY, &config.ID_PROTOCOL, i32Value, strValue);
    // int32_t
    readStorageValue(BMI270_SAMPLING_KEY, i8Value, &config.BMI270_SAMPLING, strValue);
    readStorageValue(BMI270_ACC_SENSIBILITY_KEY, i8Value, &config.BMI270_ACC_SENSIBILITY, strValue);
    readStorageValue(BMI270_GYRO_SENSIBILITY_KEY, i8Value, &config.BMI270_GYRO_SENSIBILITY, strValue);
    readStorageValue(BME688_SAMPLING_KEY, i8Value, &config.BME688_SAMPLING, strValue);
    readStorageValue(DISCONTINUOS_TIME_KEY, i8Value, &config.DISCONTINUOS_TIME, strValue);
    readStorageValue(PORT_TCP_KEY, i8Value, &config.PORT_TCP, strValue);
    readStorageValue(PORT_UDP_KEY, i8Value, &config.PORT_UDP, strValue);
    readStorageValue(HOST_IP_ADDR_KEY, i8Value, &config.HOST_IP_ADDR, strValue);
    // strings
    readStorageValue(SSID_KEY, i8Value, i32Value, config.SSID);
    readStorageValue(PASS_KEY, i8Value, i32Value, config.PASS);

    return config;
}

int writeConfig(T2_CONFIG config) {

    int8_t i8Value = 0;
    int32_t i32Value = 0;
    char *strValue = "";
    
    // int8_t
    writeStorageValue(STATUS_KEY, config.STATUS, i32Value, strValue);
    writeStorageValue(ID_PROTOCOL_KEY, config.ID_PROTOCOL, i32Value, strValue);
    // int32_t
    writeStorageValue(BMI270_SAMPLING_KEY, i8Value, config.BMI270_SAMPLING, strValue);
    writeStorageValue(BMI270_ACC_SENSIBILITY_KEY, i8Value, config.BMI270_ACC_SENSIBILITY, strValue);
    writeStorageValue(BMI270_GYRO_SENSIBILITY_KEY, i8Value, config.BMI270_GYRO_SENSIBILITY, strValue);
    writeStorageValue(BME688_SAMPLING_KEY, i8Value, config.BME688_SAMPLING, strValue);
    writeStorageValue(DISCONTINUOS_TIME_KEY, i8Value, config.DISCONTINUOS_TIME, strValue);
    writeStorageValue(PORT_TCP_KEY, i8Value, config.PORT_TCP, strValue);
    writeStorageValue(PORT_UDP_KEY, i8Value, config.PORT_UDP, strValue);
    writeStorageValue(HOST_IP_ADDR_KEY, i8Value, config.HOST_IP_ADDR, strValue);
    // strings
    writeStorageValue(SSID_KEY, i8Value, i32Value, config.SSID);
    writeStorageValue(PASS_KEY, i8Value, i32Value, config.PASS);

    return 0;
}

void resetConfig() {
    T2_CONFIG config;

    config.STATUS = 0;
    config.ID_PROTOCOL = 0;
    config.BMI270_SAMPLING = 0;
    config.BMI270_ACC_SENSIBILITY = 0;
    config.BMI270_GYRO_SENSIBILITY = 0;
    config.BME688_SAMPLING = 0;
    config.DISCONTINUOS_TIME = 0;
    config.PORT_TCP = 0;
    config.PORT_UDP = 0;
    config.HOST_IP_ADDR = 0;
    strcpy(config.SSID, "");
    strcpy(config.PASS, "");

    writeConfig(config);
}

char *numToIp(uint32_t ip) {
    static char ipStr[16];
    sprintf(ipStr, "%ld.%ld.%ld.%ld", (ip >> 0) & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
    return ipStr;
}

// function to print the configuration
void printConfig(T2_CONFIG config) {
    ESP_LOGI(CONFIG_TAG, "Printing configuration");
    printf("\tSTATUS: %d\n", config.STATUS);
    printf("\tID_PROTOCOL: %d\n", config.ID_PROTOCOL);
    printf("\tBMI270_SAMPLING: %ld\n", config.BMI270_SAMPLING);
    printf("\tBMI270_ACC_SENSIBILITY: %ld\n", config.BMI270_ACC_SENSIBILITY);
    printf("\tBMI270_GYRO_SENSIBILITY: %ld\n", config.BMI270_GYRO_SENSIBILITY);
    printf("\tBME688_SAMPLING: %ld\n", config.BME688_SAMPLING);
    printf("\tDISCONTINUOS_TIME: %ld\n", config.DISCONTINUOS_TIME);
    printf("\tPORT_TCP: %ld\n", config.PORT_TCP);
    printf("\tPORT_UDP: %ld\n", config.PORT_UDP);
    printf("\tHOST_IP_ADDR: %ld\n", config.HOST_IP_ADDR);
    printf("\tSSID: %s\n", config.SSID);
    printf("\tPASS: %s\n", config.PASS);
    printf("\n");
}

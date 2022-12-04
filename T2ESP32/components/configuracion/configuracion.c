#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "esp_log.h"

#include "configuracion.h"
#include "almacenamiento.h"

/**
 * @brief from a buffer of bytes, it extracts the configuration.
 *
 * The expected format is:
 * - 1 byte: OK
 * - 1 byte: STATUS
 * - 1 byte: ID_PROTOCOL
 * - 4 byte: BMI270_SAMPLING
 * - 4 byte: BMI270_ACC_SENSIBILITY
 * - 4 byte: BMI270_GYRO_SENSIBILITY
 * - 4 byte: BME688_SAMPLING
 * - 4 byte: DISCONTINUOS_TIME
 * - 4 byte: PORT_TCP
 * - 4 byte: PORT_UDP
 * - 4 byte: HOST_IP_ADDR
 * - 32 byte: SSID
 * - 32 byte: PASS
 *
 * TOTAL: 99 bytes + \0
 *
 * @param buffer
 * @return * T2_CONFIG
 */
T2_CONFIG extractConfig(char *buffer)
{
    T2_CONFIG config;

    // char recv_OK = buffer[0];

    // int8_t
    memcpy(&config.STATUS, &buffer[1], sizeof(int8_t));
    memcpy(&config.ID_PROTOCOL, &buffer[2], sizeof(int8_t));

    // int32_t
    memcpy(&config.BMI270_SAMPLING, &buffer[3], sizeof(int32_t));
    memcpy(&config.BMI270_ACC_SENSIBILITY, &buffer[7], sizeof(int32_t));
    memcpy(&config.BMI270_GYRO_SENSIBILITY, &buffer[11], sizeof(int32_t));
    memcpy(&config.BME688_SAMPLING, &buffer[15], sizeof(int32_t));
    memcpy(&config.DISCONTINUOS_TIME, &buffer[19], sizeof(int32_t));
    memcpy(&config.PORT_TCP, &buffer[23], sizeof(int32_t));
    memcpy(&config.PORT_UDP, &buffer[27], sizeof(int32_t));
    memcpy(&config.HOST_IP_ADDR, &buffer[31], sizeof(int32_t));

    // string
    memcpy(&config.SSID, &buffer[35], STRING_SIZE);
    memcpy(&config.PASS, &buffer[67], STRING_SIZE);

    return config;
}

int readConfigurated()
{
    int8_t configurated;

    int32_t *i32Value = (int32_t *)NULL;
    char *strValue = (char *)NULL;

    readStorageValue(CONFIGURATED_KEY, &configurated, i32Value, strValue);
    return configurated != 0;
}

T2_CONFIG readConfig()
{
    T2_CONFIG config;

    int8_t *i8Value = (int8_t *)NULL;
    int32_t *i32Value = (int32_t *)NULL;
    char *strValue = (char *)NULL;

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

int writeConfig(T2_CONFIG config)
{

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

    // save configurated as true
    writeStorageValue(CONFIGURATED_KEY, true, i32Value, strValue);

    return 0;
}

void resetConfig()
{
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
    // overwrite configurated as false
    writeStorageValue(CONFIGURATED_KEY, false, 0, "");
}

char *numToIp(uint32_t ip)
{
    static char ipStr[16];
    sprintf(ipStr, "%ld.%ld.%ld.%ld", (ip >> 0) & 0xFF, (ip >> 8) & 0xFF, (ip >> 16) & 0xFF, (ip >> 24) & 0xFF);
    return ipStr;
}

uint32_t ipToNum(char *ip)
{
    uint32_t ipNum = 0;
    char *token = strtok(ip, ".");
    int i = 0;
    while (token != NULL)
    {
        ipNum += atoi(token) << (i * 8);
        token = strtok(NULL, ".");
        i++;
    }
    return ipNum;
}

// function to print the configuration
void printConfig(T2_CONFIG config)
{
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

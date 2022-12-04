
#define CONFIG_SIZE 98

typedef struct T2_CONFIG {
    // int8_t of 1 byte
    int8_t STATUS;
    int8_t ID_PROTOCOL;
    // int32_t of 4 bytes
    int32_t BMI270_SAMPLING;
    int32_t BMI270_ACC_SENSIBILITY;
    int32_t BMI270_GYRO_SENSIBILITY;
    int32_t BME688_SAMPLING;
    int32_t DISCONTINUOS_TIME;
    int32_t PORT_TCP;
    int32_t PORT_UDP;
    int32_t HOST_IP_ADDR;
    // string of 32 bytes (including \0)
    char SSID[32];
    char PASS[32];
} T2_CONFIG;

#define CONFIG_TAG "[CONFIG]"

T2_CONFIG extractConfig(char *buffer);
T2_CONFIG readConfig();
int writeConfig(T2_CONFIG config);
void resetConfig();
char *numToIp(uint32_t ip);
void printConfig(T2_CONFIG config);
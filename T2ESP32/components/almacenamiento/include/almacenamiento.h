
// define keys index for switch case
#define STATUS_KEY 1
#define ID_PROTOCOL_KEY 2
#define BMI270_SAMPLING_KEY 3
#define BMI270_ACC_SENSIBILITY_KEY 4
#define BMI270_GYRO_SENSIBILITY_KEY 5
#define BME688_SAMPLING_KEY 6
#define DISCONTINUOS_TIME_KEY 7
#define PORT_TCP_KEY 8
#define PORT_UDP_KEY 9
#define HOST_IP_ADDR_KEY 10
#define SSID_KEY 11
#define PASS_KEY 12

// define string size for SSID and PASS
#define STRING_SIZE 32

int initNVS(void);
int deinitNVS(void);
int eraseStorage(void);
int readStorageValue(int key, int8_t *i8Value, int32_t *i32Value, char *strValue);
int writeStorageValue(int key, int8_t i8Value, int32_t i32Value, char *strValue);

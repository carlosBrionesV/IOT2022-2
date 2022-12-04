#include <stdio.h>
#include <time.h>

#include "empaquetamiento.h"
#include "sensores.h"



int dataLen(int protocol) {
    return DATA_SIZE[protocol];
}

int messageLen(int protocol) {
    return HEADER_SIZE + dataLen(protocol) + 1;
}

char *header(int protocol, int status) {
    char *head = malloc(HEADER_SIZE);

	char *ID = "D1";
	memcpy((void *) &(head[0]), (void *) ID, 2);

	uint8_t* MACaddrs = malloc(6);
	esp_efuse_mac_get_default(MACaddrs);
	memcpy((void *) &(head[2]), (void *) MACaddrs, 6);

    memcpy((void *) &(head[8]), (void *) &protocol, 1);
    memcpy((void *) &(head[9]), (void *) &status, 1);

	uint16_t dataL = (uint16_t) dataLen(protocol);
	memcpy((void *) &(head[10]), (void *) &dataL, 2);

	free(MACaddrs);
	return head;
}

char *mensaje(int protocol, int status, uint8_t val) {
    char *msg = malloc(messageLen(protocol));
    msg[messageLen(protocol) - 1] = '\0';

    char *head = header(protocol, status);
    char *data = malloc(dataLen(protocol));
    switch (protocol) 
    {
    case 1:
        data = dataProtocol1(val);
        break;
    case 2:
        data = dataProtocol2(val);
        break;
    case 3:
        data = dataProtocol3(val);
        break;
    case 4:
        data = dataProtocol4(val);
        break;
    case 5:
        data = dataProtocol5(val);
        break;
    
    default:
        data = dataProtocol0(val);
        break;
    }
    
    memcpy((void *) &(msg[0]), (void *) head, HEADER_SIZE);
    memcpy((void *) &(msg[HEADER_SIZE]), (void *) data, dataLen(protocol));

    free(head);
    free(data);

    return msg;
}

char *dataProtocol0(uint8_t val) {
    int l = dataLen(0);
    char *msg = malloc(l);

    memcpy((void *) &(msg[0]), (void *) &val, 1);

    return msg;
}

char* dataprotocol1(uint8_t val) {
	int l = dataLen(1);
	char* msg = malloc(l);
	
	memcpy((void *) &(msg[0]), (void *) &val, 1);

	float batt = batt_sensor();
	memcpy((void *) &(msg[1]), (void *) &batt, 1);

	uint32_t t = (uint32_t) time(NULL);
	memcpy((void*) &(msg[2]), (void*) &t, 4);
    
	return msg;
}

char* dataprotocol2(uint8_t val) {
	int l = dataLen(2);
	char* msg = malloc(l);
	
	memcpy((void *) &(msg[0]), (void *) &val, 1);

	float batt = batt_sensor();
	memcpy((void *) &(msg[1]), (void *) &batt, 1);

	uint32_t t = (uint32_t) time(NULL);
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	float temp = thpc_sensor_temp();
	memcpy((void*) &(msg[6]), (void*) &temp, 1);

	int press = thpc_sensor_pres();
	memcpy((void*) &(msg[7]), (void*) &press, 4);

	int hum = thpc_sensor_hum();
	memcpy((void*) &(msg[11]), (void*) &hum, 1);

	float co = thpc_sensor_co();
	memcpy((void*) &(msg[12]), (void*) &co, 4);

	return msg;
}

char* dataprotocol3(uint8_t val) {
	int l = dataLen(3);
	char* msg = malloc(l);

	memcpy((void *) &(msg[0]), (void *) &val, 1);

	float batt = batt_sensor();
	memcpy((void *) &(msg[1]), (void *) &batt, 1);

	uint32_t t = (uint32_t) time(NULL);
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	float temp = thpc_sensor_temp();
	memcpy((void*) &(msg[6]), (void*) &temp, 1);

	int press = thpc_sensor_pres();
	memcpy((void*) &(msg[7]), (void*) &press, 4);

	int hum = thpc_sensor_hum();
	memcpy((void*) &(msg[11]), (void*) &hum, 1);

	float co = thpc_sensor_co();
	memcpy((void*) &(msg[12]), (void*) &co, 4);

	float rms = acc_kpi_rms(
        accel_kpi_amp_x(),
        accel_kpi_amp_y(),
        accel_kpi_amp_z()
    );
	memcpy((void*) &(msg[16]), (void*) &rms, 4);

	return msg;
}

char* dataprotocol4(uint8_t val) {
	int l = dataLen(4);
	char* msg = malloc(l);

	memcpy((void *) &(msg[0]), (void *) &val, 1);

	float batt = batt_sensor();
	memcpy((void *) &(msg[1]), (void *) &batt, 1);

	uint32_t t = (uint32_t) time(NULL);
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	float temp = thpc_sensor_temp();
	memcpy((void*) &(msg[6]), (void*) &temp, 1);

	int press = thpc_sensor_pres();
	memcpy((void*) &(msg[7]), (void*) &press, 4);

	int hum = thpc_sensor_hum();
	memcpy((void*) &(msg[11]), (void*) &hum, 1);

	float co = thpc_sensor_co();
	memcpy((void*) &(msg[12]), (void*) &co, 4);

    float amp_x = acc_kpi_amp_x();
    float freq_x = acc_kpi_freq_x();
    float amp_y = acc_kpi_amp_y();
    float freq_y = acc_kpi_freq_y();
    float amp_z = acc_kpi_amp_z();
    float freq_z = acc_kpi_freq_z();
    float rms = acc_kpi_rms(amp_x, amp_y, amp_z);
    memcpy((void*) &(msg[16]), (void*) &rms, 4);
    memcpy((void*) &(msg[20]), (void*) &amp_x, 4);
    memcpy((void*) &(msg[24]), (void*) &freq_x, 4);
    memcpy((void*) &(msg[28]), (void*) &amp_y, 4);
    memcpy((void*) &(msg[32]), (void*) &freq_y, 4);
    memcpy((void*) &(msg[36]), (void*) &amp_z, 4);
    memcpy((void*) &(msg[40]), (void*) &freq_z, 4);

	return msg;
}

char* dataprotocol4(uint8_t val) {
	int l = dataLen(4);
	char* msg = malloc(l);

	msg[0] = val;
	
	float batt = batt_sensor();
	msg[1] = batt;
	// timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	char temp = thpc_sensor_temp();
	msg[5] = temp;

	float press = thpc_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4);

	char hum = thpc_sensor_hum();
	msg[10] = hum;

	float co = thpc_sensor_co2();
	memcpy((void*) &(msg[11]), (void*) &co, 4);

	float *accx = acc_sensor_accx();
	memcpy((void*) &(msg[15]), (void*) accx, 4*2000);

	float *accy = acc_sensor_accy();
	memcpy((void*) &(msg[8015]), (void*) accy, 4*2000);

	float *accz = acc_sensor_accz();
	memcpy((void*) &(msg[16015]), (void*) accz, 4*2000);

	return msg;
}

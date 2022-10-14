
#include <math.h>
#include <stdlib.h>
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"

#include "../sensores/sensors.c"

#define HEADER_SIZE 12

unsigned short lengmsg[6] = {6, 16, 20, 44, 24016, 1};
unsigned short dataLen(char protocol){
	return lengmsg[ (unsigned int) protocol];
}

unsigned short messageLength(char protocol){
	return HEADER_SIZE + dataLen(protocol) + 1;
}

char *dataprotocol0();
char *dataprotocol1();
char *dataprotocol2();
char *dataprotocol3();
char *dataprotocol4();
char *dataprotocol5();

//Genera el header de un mensaje, con ID, MAC, TLayer, protocol y el largo del mensaje.
char* header(char protocol, char transportLayer) {
	char* head = malloc(HEADER_SIZE);

	char *ID = "D1";
	memcpy((void*) &(head[0]), (void*) ID, 2);

	uint8_t* MACaddrs = malloc(6);
	esp_efuse_mac_get_default(MACaddrs);
	memcpy((void*) &(head[2]), (void*) MACaddrs, 6);

	head[8] = transportLayer;
	head[9] = protocol;
	unsigned short dataL = dataLen(protocol);
	memcpy((void*) &(head[10]), (void*) &dataL, 2);
	free(MACaddrs);
	return head;
}

char* mensaje(char protocol, char transportLayer){
	char* mnsj = malloc(messageLength(protocol));
	mnsj[messageLength(protocol) - 1] = '\0';
	char* hdr = header(protocol, transportLayer);
	char* data;
	switch (protocol) {
		case 0:
			data = dataprotocol0();
			break;
		case 1:
			data = dataprotocol1();
			break;
		case 2:
			data = dataprotocol2();
			break;
		case 3:
			data = dataprotocol3();
			break;
		case 4:
			data = dataprotocol4();
			break;
		case 5:
			data = dataprotocol5();
			break;
		default:
			data = dataprotocol5();
			break;
	}
	memcpy((void*) mnsj, (void*) hdr, HEADER_SIZE);
	memcpy((void*) &(mnsj[HEADER_SIZE]), (void*) data, dataLen(protocol));
	free(hdr);
	free(data);
	return mnsj;
}

// Arma un paquete para el protocolo de inicio, que busca solo respuesta
char* dataprotocol5(){
	char* msg = malloc(dataLen(5));
	unsigned char val = (unsigned char) 1;
	msg[0] = val;
	return msg;
}

// Arma un paquete para el protocolo 0, con la bateria
char* dataprotocol0(){
	int l = dataLen(0);
	char* msg = malloc(l);
	
	unsigned char val = (unsigned char) 1;
	msg[0] = val;

	int batt = batt_sensor();
	msg[1] = batt;
	//timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);
	return msg;
}

char* dataprotocol1(){
	int l = dataLen(1);
	char* msg = malloc(l);
	
	unsigned char val = (unsigned char) 1;
	msg[0] = val;

	int batt = batt_sensor();
	msg[1] = batt;
	// timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	float temp = THPC_sensor_temp();
	msg[5] = temp;

	int press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4);

	int hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co2();
	memcpy((void*) &(msg[11]), (void*) &co, 4);

	return msg;
}

char* dataprotocol2(){
	int l = dataLen(2);
	char* msg = malloc(l);

	unsigned char val = (unsigned char) 1;
	msg[0] = val;
	
	float batt = batt_sensor();
	msg[1] = batt;
	// timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	char temp = THPC_sensor_temp();
	msg[5] = temp;

	float press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4);

	char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co2();
	memcpy((void*) &(msg[11]), (void*) &co, 4);

	float rms = acc_sensor_rms();
	memcpy((void*) &(msg[15]), (void*) &rms, 4);

	return msg;
}

char* dataprotocol3(){
	int l = dataLen(3);
	char* msg = malloc(l);

	unsigned char val = (unsigned char) 1;
	msg[0] = val;
	
	float batt = batt_sensor();
	msg[1] = batt;
	// timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	char temp = THPC_sensor_temp();
	msg[5] = temp;

	float press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4);

	char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co2();
	memcpy((void*) &(msg[11]), (void*) &co, 4);

	float rms = acc_sensor_rms();
	memcpy((void*) &(msg[15]), (void*) &rms, 4);

	float ampx = acc_sensor_ampx();
	memcpy((void*) &(msg[19]), (void*) &ampx, 4);

	float frecx = acc_sensor_frecx();
	memcpy((void*) &(msg[23]), (void*) &frecx, 4);

	float ampy = acc_sensor_ampy();
	memcpy((void*) &(msg[27]), (void*) &ampy, 4);

	float frecy = acc_sensor_frecy();
	memcpy((void*) &(msg[31]), (void*) &frecy, 4);

	float ampz = acc_sensor_ampz();
	memcpy((void*) &(msg[35]), (void*) &ampz, 4);

	float frecz = acc_sensor_frecz();
	memcpy((void*) &(msg[39]), (void*) &frecz, 4);

	return msg;
}

char* dataprotocol4(){
	int l = dataLen(4);
	char* msg = malloc(l);

	unsigned char val = (unsigned char) 1;
	msg[0] = val;
	
	float batt = batt_sensor();
	msg[1] = batt;
	// timestamp
	int t = 0;
	memcpy((void*) &(msg[2]), (void*) &t, 4);

	char temp = THPC_sensor_temp();
	msg[5] = temp;

	float press = THPC_sensor_pres();
	memcpy((void*) &(msg[6]), (void*) &press, 4);

	char hum = THPC_sensor_hum();
	msg[10] = hum;

	float co = THPC_sensor_co2();
	memcpy((void*) &(msg[11]), (void*) &co, 4);

	float *accx = acc_sensor_accx();
	memcpy((void*) &(msg[15]), (void*) accx, 4*2000);

	float *accy = acc_sensor_accy();
	memcpy((void*) &(msg[8015]), (void*) accy, 4*2000);

	float *accz = acc_sensor_accz();
	memcpy((void*) &(msg[16015]), (void*) accz, 4*2000);

	return msg;
}


#include "esp_wifi.h"

#define WIFI_TAG "wifi station"

int wifi_init_sta(char *ssid, char *pass, int authmode);
int wifi_connect(char *ssid, char *pass, wifi_auth_mode_t authmode);
int wifi_stop();
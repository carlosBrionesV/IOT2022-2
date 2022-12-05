#include <stdio.h>
#include <sys/param.h>
#include <string.h>
#include <stdlib.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "almacenamiento.h"
#include "configuracion.h"
#include "empaquetamiento.h"
#include "tcp_client.h"
#include "udp_client.h"
#include "wifi_client.h"
#include "ble_server.h"

void app_main(void)
{

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Inicializar el sistema de archivos
    initNVS();

    xTaskCreate(bleConfig, "bleConfig", 4096, NULL, 5, NULL);
}
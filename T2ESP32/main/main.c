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
#include "ble_server.h"
#include "configuracion.h"
#include "tcp_client.h"
#include "udp_client.h"
#include "wifi_client.h"


#define TAG "MAIN"

void app_main(void)
{

    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // Inicializar el sistema de archivos
    initNVS();

    resetConfig();
    int configurated = readConfigurated();
    printConfig(readConfig());
    if (!configurated)
    {
        bleConfig();
    }

    while (1)
    {
        T2_CONFIG config = readConfig();

        if (20 <= config.STATUS && config.STATUS <= 23)
        {
            wifi_connect(config.SSID, config.PASS, WIFI_AUTH_WPA2_PSK);
            vTaskDelay(1000 / portTICK_PERIOD_MS);

            switch (config.STATUS)
            {
            case 20:
                configurateTcp();
                break;
            case 23:
                manageUdp();
                break;
            default:
                manageTcp();
                break;
            }

            wifi_stop();
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
        else
        {
            switch (config.STATUS)
            {
            case 0:
                bleConfig();
                break;
            case 30:
                ESP_LOGI(TAG, "BLE CONTINUOUS NOT IMPLEMENTED");
                break;
            case 31:
                ESP_LOGI(TAG, "BLE DISCONTINUOUS NOT IMPLEMENTED");
                break;
            default:
                ESP_LOGI(TAG, "INVALID STATUS, reseting configuration");
                resetConfig();
                break;
            }
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
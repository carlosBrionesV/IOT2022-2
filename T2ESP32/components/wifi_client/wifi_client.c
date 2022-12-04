#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "wifi_client.h"

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1

static const char *wifi_TAG = "wifi station";
static int s_retry_num = 0;
int max_retry = 10;

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    /* if starting, try to connect */
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }

    /* if connection to AP failed */
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        /* try to reconnect */
        if (s_retry_num < max_retry)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(wifi_TAG, "retry to connect to the AP");
        }
        else
        {
            /* set WIFI_FAIL_BIT and stop retrying */
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
    }

    /* if connected to AP and got IP asigned */
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(wifi_TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0; /* reset retry counter */
        /* set WIFI_CONNECTED_BIT as event result */
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

int wifi_init_sta(char *ssid, char *pass, int authmode)
{
    /* create event group to handle WiFi events */
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    /* register wifi event handler */
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &event_handler, NULL, &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &event_handler, NULL, &instance_got_ip));
    /* wifi config */
    wifi_config_t wifi_config;
    wifi_sta_config_t sta_config;
    memset(&sta_config, 0, sizeof(wifi_sta_config_t));
    memcpy(sta_config.ssid, ssid, strlen(ssid));
    memcpy(sta_config.password, pass, strlen(pass));
    sta_config.threshold.authmode = authmode;
    wifi_config.sta = sta_config;

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(wifi_TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT, pdFALSE, pdFALSE, portMAX_DELAY);
    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT)
    {
        ESP_LOGI(wifi_TAG, "connected to ap SSID:%s password:%s", ssid, pass);
        return 0;
    }
    else if (bits & WIFI_FAIL_BIT)
    {
        ESP_LOGI(wifi_TAG, "Failed to connect to SSID:%s, password:%s", ssid, pass);
        return 1;
    }
    else
    {
        ESP_LOGE(wifi_TAG, "UNEXPECTED EVENT");
        return 2;
    }
}

int wifi_connect(char *ssid, char *pass, wifi_auth_mode_t authmode)
{
    ESP_LOGI(wifi_TAG, "ESP_WIFI_MODE_STA");
    int ret = wifi_init_sta(ssid, pass, authmode);
    return ret;
}

int wifi_stop()
{
    esp_err_t err = esp_wifi_disconnect();
    if (err != ESP_OK)
    {
        ESP_LOGE(wifi_TAG, "esp_wifi_disconnect failed");
        return 1;
    }

    err = esp_wifi_stop();
    if (err != ESP_OK)
    {
        ESP_LOGE(wifi_TAG, "esp_wifi_stop failed");
        return 1;
    }
    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_netif_deinit());
    return 0;
}

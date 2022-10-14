
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

#include "../conexiones/wifi_client/wifi_client.c"
#include "../conexiones/udp_socket/udp_socket.c"
#include "../conexiones/tcp_socket/tcp_socket.c"
#include "../empaquetamiento/packeting.c"

static const char *TAG = "main program";

void wait_delay(int ms) {
  vTaskDelay(ms / portTICK_PERIOD_MS);
}

void app_main(void) {
  //Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "Starting app_main");
  wifi_connect(WIFI_SSID, WIFI_PASS, ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD);

  struct my_TCPsocket *s = tcp_create_socket(TCP_HOST_IP_ADDR, TCP_PORT);
  while (1) {
    if (tcp_connect_socket(s) == 0) {
      break;
    }
    ESP_LOGI(TAG, "TCP connection failed, retrying in 3 seconds");
    wait_delay(3000);
  }
  
  char protocolo = (char) 5;
  char tLayer = (char) 0;
  char *message = mensaje(protocolo, tLayer);
  char *message2 = "D112345605131";

  // ask for config until response
  while (1) {
    ESP_LOGI(TAG, "Sending message to %s:%d", TCP_HOST_IP_ADDR, TCP_PORT);
    int err = tcp_send_message(s->sock, message2, strlen(message2));
    if (err < 0) {continue;}
    ESP_LOGI(TAG, "Message sent: %s", message2);

    int len = udp_recieve_message(s->sock, s->rx_buffer, sizeof(s->rx_buffer) - 1);

    if (len < 0) {
      vTaskDelay(2000 / portTICK_PERIOD_MS);
      continue;
    }
    s->rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    break;
  }

  // recibida configuracion
  protocolo = s->rx_buffer[0];
  ESP_LOGI(TAG, "set protocol to %c", protocolo);
  tLayer = s->rx_buffer[1];
  ESP_LOGI(TAG, "set transport layer to %c", tLayer);
  
}
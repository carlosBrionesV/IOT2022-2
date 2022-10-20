
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

enum transportLayer {TCP, UDP}; //0 = TCP, 1 = UDP

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

  int TCPs = createTCPClient(TCP_HOST_IP_ADDR, TCP_PORT);
  int UDPs = createUDPClient(UDP_HOST_IP_ADDR, UDP_PORT);
  setTCPTimeout(TCPs, 5);
  setUDPTimeout(UDPs, 5);
  
  uint8_t currentProtocol = 5;
  uint8_t currentTransportLayer = TCP;

  uint8_t ok, change, IDProtocol, TLayer;
  char *message;
  char *rx_buffer;
  int rx_buffer_len = 8;
  int message_len, err;

  while(1) {
    err = connectTCPClient(TCPs, TCP_HOST_IP_ADDR, TCP_PORT);
    if (err != 0) {
      closeTCPClient(TCPs);
      TCPs = createTCPClient(TCP_HOST_IP_ADDR, TCP_PORT);
      ESP_LOGE(TAG, "Error connecting to TCP server, trying again in 5 seconds with a new socket");
      wait_delay(5000);
      continue;
    }
    break;
  }

  // ask for the TransportLayer configuration for IDProtocol 0
  message = mensaje(currentProtocol, currentTransportLayer, 0);
  message_len = messageLength(currentProtocol);
  while(1) {
    ESP_LOGI(TAG, "Sending message with IDProtocol: %d and TLayer: %d", currentProtocol, currentTransportLayer);
    sendTCPMessage(TCPs, message, message_len);
    wait_delay(1000);

    while(1) {
      rx_buffer = malloc(8);
      if (currentTransportLayer == TCP) {
        err = recieveTCPMessage(TCPs, rx_buffer, rx_buffer_len);
      } else {
        err = recieveUDPMessage(UDPs, rx_buffer, rx_buffer_len);
      }
      ESP_LOGI(TAG, "Current buffer: %d %d %d %d", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3]);

      ok = rx_buffer[0];
      if (ok == 1) {
        change = rx_buffer[1];
        TLayer = rx_buffer[2];
        IDProtocol = rx_buffer[3];
        ESP_LOGI(TAG, "Recieved response with OK: %d, change: %d, TLayer: %d, IDProtocol: %d", ok, change, TLayer, IDProtocol);
        break;
      }
      wait_delay(3000);
    }
    if (ok == 1) break;
    free(rx_buffer);
    rx_buffer = NULL;
    wait_delay(3000);
  }

  free(message);
  message = NULL;
  ESP_LOGI(TAG, "Configuration recieved: TLayer = %d, IDProtocol = %d", TLayer, IDProtocol);

  // start sending messages
  while(1) {
    if (change == 1) {
      ESP_LOGI(TAG, "Configuration changed");
      ESP_LOGI(TAG, "New configuration: TLayer = %d, IDProtocol = %d", TLayer, IDProtocol);
      currentProtocol = IDProtocol;
      currentTransportLayer = TLayer;
      change = 0;
    }

    message = mensaje(currentProtocol, currentTransportLayer, 1);
    message_len = messageLength(currentProtocol);
    rx_buffer = malloc(8);
    
    ESP_LOGI(TAG, "Sending with TLayer = %d, IDProtocol = %d", currentTransportLayer, currentProtocol);
    if (currentTransportLayer == TCP) {
      sendTCPMessage(TCPs, message, message_len);
    } else {
      sendUDPMessage(UDPs, UDP_HOST_IP_ADDR, UDP_PORT, message, message_len);
    }
    wait_delay(1000);

    while (1) {
      if (currentTransportLayer == TCP) {
        err = recieveTCPMessage(TCPs, rx_buffer, rx_buffer_len);
      } else {
        err = recieveUDPMessage(UDPs, rx_buffer, rx_buffer_len);
      }
      ESP_LOGI(TAG, "Current buffer: %d %d %d %d", rx_buffer[0], rx_buffer[1], rx_buffer[2], rx_buffer[3]);

      ok = rx_buffer[0];
      if (ok == 1) {
        change = rx_buffer[1];
        TLayer = rx_buffer[2];
        IDProtocol = rx_buffer[3];
        ESP_LOGI(TAG, "Recieved response with OK: %d, change: %d, TLayer: %d, IDProtocol: %d", ok, change, TLayer, IDProtocol);
        break;
      }
      wait_delay(3000);
    }
    
    free(message);
    message = NULL;
    free(rx_buffer);
    rx_buffer = NULL;
    wait_delay(3000);
  }
  
  closeTCPClient(TCPs);
  closeUDPClient(UDPs);
}
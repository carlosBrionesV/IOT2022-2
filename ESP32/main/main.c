
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
  setTCPTimeout(TCPs, 5000);
  setUDPTimeout(UDPs, 5000);
  
  uint8_t currentProtocol = 5;
  uint8_t currentTransportLayer = TCP;

  uint8_t ok, change, IDProtocol, TLayer;
  char *message;
  char *rx_buffer = malloc(5);
  int rx_buffer_len = sizeof(rx_buffer);
  int message_len, err, len;

  while(1) {
    err = connectTCPClient(TCPs, TCP_HOST_IP_ADDR, TCP_PORT);
    if (err != 0) {
      closeTCPClient(TCPs);
      TCPs = createTCPClient(TCP_HOST_IP_ADDR, TCP_PORT);
      wait_delay(3000);
      continue;
    }
    break;
  }

  // ask for the TransportLayer configuration for IDProtocol
  message = mensaje(currentProtocol, currentTransportLayer, 0);
  message_len = messageLength(currentProtocol);
  while(1) {
    ESP_LOGI(TAG, "Sending message with IDProtocol: %d and TLayer: %d", currentProtocol, currentTransportLayer);
    if (currentTransportLayer == TCP) {
      sendTCPMessage(TCPs, message, message_len);
    } else {
      sendUDPMessage(UDPs, UDP_HOST_IP_ADDR, UDP_PORT, message, message_len);
    }
    wait_delay(500);
    
    while(1) {
      if (currentTransportLayer == TCP) {
        len = recieveTCPMessage(TCPs, rx_buffer, rx_buffer_len);
      } else {
        len = recieveUDPMessage(UDPs, rx_buffer, rx_buffer_len);
      }
      if (len > 0) {
        break;
      }
      wait_delay(2000);
    }

    ok = rx_buffer[0];
    // configuration recieved
    if (!ok) {
      change = rx_buffer[1];
      TLayer = rx_buffer[2];
      IDProtocol = rx_buffer[3];
      break;
    }
  }

  free(message);
  message = NULL;
  ESP_LOGI(TAG, "Configuration recieved: TLayer = %d, IDProtocol = %d", TLayer, IDProtocol);

  // start sending messages
  while(1) {
    if (change) {
      ESP_LOGI(TAG, "Configuration changed");
      ESP_LOGI(TAG, "New configuration: TLayer = %d, IDProtocol = %d", TLayer, IDProtocol);
      currentProtocol = IDProtocol;
      currentTransportLayer = TLayer;
      change = 0;
    }

    message = mensaje(currentProtocol, currentTransportLayer, 1);
    message_len = messageLength(currentProtocol);
    
    ESP_LOGI(TAG, "Sending message: %s", message);
    if (currentTransportLayer == TCP) {
      sendTCPMessage(TCPs, message, message_len);
    } else {
      sendUDPMessage(UDPs, UDP_HOST_IP_ADDR, UDP_PORT, message, message_len);
    }
    
    while (1) {
      int tries = 0;
      if (tries > 5) {
        free(message);
        message = NULL;
        break;
      }
      if (currentTransportLayer == TCP) {
        len = recieveTCPMessage(TCPs, rx_buffer, rx_buffer_len);
      } else {
        len = recieveUDPMessage(UDPs, rx_buffer, rx_buffer_len);
      }
      if (len < 1) {
        wait_delay(1000);
        continue;
      }

      ok = rx_buffer[0];
      // configuration recieved
      if (!ok) {
        change = rx_buffer[1];
        TLayer = rx_buffer[2];
        IDProtocol = rx_buffer[3];
        break;
      }
    }
    // configuration recieved
    if (!ok) {
      change = rx_buffer[1];
      TLayer = rx_buffer[2];
      IDProtocol = rx_buffer[3];
      break;
    }

  }
  
  closeTCPClient(TCPs);
  closeUDPClient(UDPs);
}
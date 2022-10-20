
#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

#define UDP_HOST_IP_ADDR CONFIG_ESP_UDP_IPV4_ADDR
#define UDP_PORT CONFIG_ESP_UDP_PORT

static const char *UDP_TAG = "udp client";

int udp_addr_family = AF_INET;
int udp_ip_protocol = IPPROTO_IP;

int createUDPClient() {
  /* Create a UDP socket */
  int sock = socket(udp_addr_family, SOCK_DGRAM, udp_ip_protocol);
  if (sock < 0) {
    ESP_LOGE(UDP_TAG, "Unable to create socket: errno %d", errno);
    return -1;
  }
  ESP_LOGI(UDP_TAG, "Socket created");
  return sock;
}

void setUDPTimeout(int sock, int timeout_sec) {
  struct timeval tv;
  tv.tv_sec = timeout_sec;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
}

int sendUDPMessage(int sock, char *ip, int port, char *message, int message_len) {
  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = inet_addr(ip);
  dest_addr.sin_family = udp_addr_family;
  dest_addr.sin_port = htons(port);
  int err = sendto(sock, message, message_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err < 0) {
    ESP_LOGE(UDP_TAG, "Error occurred during sending: errno %d", errno);
    return -1;
  }
  ESP_LOGI(UDP_TAG, "Message sent");
  return 0;
}

int recieveUDPMessage(int sock, char *rx_buffer, int rx_buffer_len) {
  ESP_LOGI(UDP_TAG, "Waiting for data");
  int len = recv(sock, rx_buffer, rx_buffer_len - 1, 0);
  // Error occurred during receiving
  if (len < 0) {
    ESP_LOGE(UDP_TAG, "recv failed: errno %d", errno);
    return -1;
  }
  // Data received
  else {
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(UDP_TAG, "Received %d bytes: %s", len, rx_buffer);
    return len;
  }
}

void closeUDPClient(int sock) {
  shutdown(sock, 0);
  close(sock);
}

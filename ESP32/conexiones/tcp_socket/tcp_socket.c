
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

#define TCP_HOST_IP_ADDR CONFIG_ESP_TCP_IPV4_ADDR
#define TCP_PORT CONFIG_ESP_TCP_PORT

static const char *TCP_TAG = "tcp client";

int tcp_addr_family = AF_INET;
int tcp_ip_protocol = IPPROTO_IP;

int createTCPClient() {
  /* Create a TCP socket */
  int sock = socket(tcp_addr_family, SOCK_STREAM, tcp_ip_protocol);
  if (sock < 0) {
    ESP_LOGE(TCP_TAG, "Unable to create socket: errno %d", errno);
    return -1;
  }
  ESP_LOGI(TCP_TAG, "Socket created");
  return sock;
}

void setTCPTimeout(int sock, int timeout) {
  struct timeval receiving_timeout;
  receiving_timeout.tv_sec = timeout / 1000;
  receiving_timeout.tv_usec = (timeout % 1000) * 1000;
  if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout, sizeof(receiving_timeout)) < 0) {
    ESP_LOGE(TCP_TAG, "Failed to set socket receiving timeout");
  }
}

int connectTCPClient(int sock, char *ip, int port) {
  /* Connect to the remote host */
  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = inet_addr(ip);
  dest_addr.sin_family = tcp_addr_family;
  dest_addr.sin_port = htons(port);
  int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err != 0) {
    ESP_LOGE(TCP_TAG, "Socket unable to connect: errno %d", errno);
    return -1;
  }
  ESP_LOGI(TCP_TAG, "Successfully connected");
  return 0;
}

int sendTCPMessage(int sock, char *message, int message_len) {
  int err = send(sock, message, message_len, 0);
  if (err < 0) {
    ESP_LOGE(TCP_TAG, "Error occurred during sending: errno %d", errno);
    return -1;
  }
  ESP_LOGI(TCP_TAG, "Message sent");
  return 0;
}

int recieveTCPMessage(int sock, char *rx_buffer, int rx_buffer_len) {
  ESP_LOGI(TCP_TAG, "Waiting for data");
  int len = recv(sock, rx_buffer, rx_buffer_len - 1, 0);
  // Error occurred during receiving
  if (len < 0) {
    ESP_LOGE(TCP_TAG, "recv failed: errno %d", errno);
    return -1;
  }
  // Data received
  else {
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TCP_TAG, "Received %d bytes: %s", len, rx_buffer);
    return len;
  }
}

void closeTCPConnection(int sock) {shutdown(sock, 0);}
void closeTCPClient(int sock) {close(sock);}

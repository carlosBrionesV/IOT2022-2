
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

struct my_TCPsocket {
  char rx_buffer[128];
  int sock; // socket
  struct sockaddr_in dest_addr; // destination address
};

struct my_TCPsocket *tcp_create_socket(char *ip, int port) {
  struct my_TCPsocket *s = malloc(sizeof(struct my_TCPsocket));
  /* Create a TCP socket */
  s->sock = socket(tcp_addr_family, SOCK_STREAM, tcp_ip_protocol);
  if (s->sock < 0) {
    ESP_LOGE(TCP_TAG, "Unable to create socket: errno %d", errno);
    return NULL;
  }
  ESP_LOGI(TCP_TAG, "Socket created");
  /* Configure destination address */
  s->dest_addr.sin_addr.s_addr = inet_addr(ip);
  s->dest_addr.sin_family = AF_INET;
  s->dest_addr.sin_port = htons(port);
  ESP_LOGI(TCP_TAG, "Socket destination address configured: %s:%d", ip, port);
  return s;
}

int tcp_connect_socket(struct my_TCPsocket *s) {
  int err = connect(s->sock, (struct sockaddr *)&s->dest_addr, sizeof(s->dest_addr));
  if (err != 0) {
    ESP_LOGE(TCP_TAG, "Socket unable to connect: errno %d", errno);
    return -1;
  }
  ESP_LOGI(TCP_TAG, "Successfully connected");
  return 0;
}

struct sockaddr_in tcp_create_destination_addr(char *ip, int port) {
  struct sockaddr_in dest_addr;
  dest_addr.sin_addr.s_addr = inet_addr(ip);
  dest_addr.sin_family = AF_INET;
  dest_addr.sin_port = htons(port);
  return dest_addr;
}

int tcp_send_message(int sock, char *message, int message_len) {
  int err = send(sock, message, message_len, 0);
  if (err < 0) {
    ESP_LOGE(TCP_TAG, "Error occurred during sending: errno %d", errno);
    return -1;
  }
  return 0;
}

int tcp_recieve_message(int sock, char *rx_buffer, int rx_buffer_len) {
  int len = recv(sock, rx_buffer, rx_buffer_len - 1, 0);
  // Error occurred during receiving
  if (len < 0) {
    ESP_LOGE(TCP_TAG, "recv failed: errno %d", errno);
    return -1;
  }
  // Data received
  else {
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(TCP_TAG, "Received %d bytes:", len);
    ESP_LOGI(TCP_TAG, "%s", rx_buffer);
  }
  return len;
}

void tcp_close_socket(struct my_TCPsocket *s) {
  if (s->sock != -1) {
    ESP_LOGE(TCP_TAG, "Shutting down socket");
    shutdown(s->sock, 0);
    close(s->sock);
  }
  free(s);
}

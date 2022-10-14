
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

struct my_UDPsocket {
  char rx_buffer[128];
  int sock; // socket
  struct sockaddr_in dest_addr; // destination address
};

struct my_UDPsocket *udp_create_socket(char *ip, int port) {
  struct my_UDPsocket *s = malloc(sizeof(struct my_UDPsocket));
  /* Create a UDP socket */
  s->sock = socket(udp_addr_family, SOCK_DGRAM, udp_ip_protocol);
  if (s->sock < 0) {
    ESP_LOGE(UDP_TAG, "Unable to create socket: errno %d", errno);
    return NULL;
  }
  ESP_LOGI(UDP_TAG, "Socket created");
  /* Configure destination address */
  s->dest_addr.sin_addr.s_addr = inet_addr(ip);
  s->dest_addr.sin_family = AF_INET;
  s->dest_addr.sin_port = htons(port);
  ESP_LOGI(UDP_TAG, "Socket destination address configured: %s:%d", ip, port);
  return s;
}

void udp_set_timeout(int sock, int timeout_sec) {
  struct timeval tv;
  tv.tv_sec = timeout_sec;
  tv.tv_usec = 0;
  setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
}

int udp_send_message(int sock, struct sockaddr_in dest_addr, char *message, int message_len) {
  int err = sendto(sock, message, message_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
  if (err < 0) {
    ESP_LOGE(UDP_TAG, "Error occurred during sending: errno %d", errno);
    return -1;
  }
  ESP_LOGI(UDP_TAG, "Message sent: %s", message);
  return 0;
}

int udp_recieve_message(int sock, char *rx_buffer, int rx_buffer_len) {
  int len = recv(sock, rx_buffer, rx_buffer_len, 0);
  // Error occurred during receiving
  if (len < 0) {
    ESP_LOGE(UDP_TAG, "recv failed: errno %d", errno);
    return -1;
  }
  // Data received
  else {
    rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
    ESP_LOGI(UDP_TAG, "Received %d bytes:", len);
    ESP_LOGI(UDP_TAG, "%s", rx_buffer);
    return len;
  }
}

void udp_close_socket(struct my_UDPsocket *s) {
  if (s->sock != -1) {
    ESP_LOGE(UDP_TAG, "Shutting down socket");
    shutdown(s->sock, 0);
    close(s->sock);
  }
  free(s);
}

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/param.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"

#include "udp_client.h"
#include "configuracion.h"
// #include "empaquetamiento.h"

const char *UDP_TAG = "UDP client";

/**
 * @brief creates udp socket
 *
 * @return int socket descriptor, -1 if error
 */
int socketUdp()
{
    /* Create a UDP socket */
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);

    if (sock < 0)
    {
        ESP_LOGE(UDP_TAG, "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI(UDP_TAG, "Socket created");
        timeoutUdp(sock, UDP_TIMEOUT_SEC);
    }

    return sock;
}

bool checkUdp(T2_CONFIG config)
{
    return config.STATUS == 23;
}

/**
 * @brief sets the socket timeout
 *
 * @param sock socket descriptor
 * @param timeout_sec timeout in seconds
 * @return int 0 if ok, -1 if error
 */
int timeoutUdp(int sock, int timeout_sec)
{
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        ESP_LOGE(UDP_TAG, "Error setting socket timeout: errno %d", errno);
        return -1;
    }

    return 0;
}

/**
 * @brief function to send data to the server
 *
 * @param sock socket to send data
 * @param ip ip of the server
 * @param port port of the server
 * @param message message to send
 * @param message_len length of the message
 * @return int 0 if success, -1 if error
 */
int sendUdp(int sock, char *ip, int port, char *message, int message_len)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    int err = sendto(sock, message, message_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err < 0)
    {
        ESP_LOGE(UDP_TAG, "Error occurred during sending: errno %d", errno);
        return -1;
    }
    ESP_LOGI(UDP_TAG, "Message sent");
    return 0;
}

/**
 * @brief Receive data from the socket once.
 *
 * @param sock Socket descriptor
 * @param rx_buffer Buffer to store the received data
 * @param rx_buffer_len Length of the buffer
 * @return int Number of bytes received, or -1 if error
 */
int recvUdp(int sock, char *rx_buffer, int rx_buffer_len)
{
    ESP_LOGI(UDP_TAG, "Waiting for data");
    int len = recv(sock, rx_buffer, rx_buffer_len - 1, 0);
    // Error occurred during receiving
    if (len < 0)
    {
        ESP_LOGE(UDP_TAG, "recv failed: errno %d", errno);
        return -1;
    }
    // Data received
    else
    {
        rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(UDP_TAG, "Received %d bytes: %s", len, rx_buffer);
        return len;
    }
}

/**
 * @brief function to manage a udp client main loop.
 *
 * @return int
 */
int manageUdp()
{
    T2_CONFIG config = readConfig();

    char rx_buffer[16];
    int rx_buffer_len = sizeof(rx_buffer);

    while (1)
    {
        if (!checkUdp(config))
        {
            ESP_LOGI(UDP_TAG, "Status %d not allowed", config.STATUS);
            return -1;
        }

        // socket creation
        int sock = socketUdp();
        if (sock < 0)
        {
            if (sock == -1)
            {
                ESP_LOGE(UDP_TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        char *host_ip = numToIp(config.HOST_IP_ADDR);

        // send and recv loop
        int send_tries = 0;
        while (send_tries < UDP_MAX_SEND_TRIES)
        {
            ESP_LOGI(UDP_TAG, "Sending message to %s:%ld", host_ip, config.PORT_UDP);
            char *msg = createMsg(config.ID_PROTOCOL, config.STATUS, 1);
            // char *msg = "hola";
            // TODO: cambiar el mensaje por el empaquetamiento
            int err = sendUdp(sock, host_ip, config.PORT_UDP, msg, strlen(msg));
            free(msg);
            // int err = sendUdp(sock, host_ip, config.PORT_UDP, msg, 4);

            if (err < 0)
            {
                ESP_LOGE(UDP_TAG, "Error sending message");
                send_tries++;

                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            send_tries = 0;

            // recieve loop
            int recv_tries = 0;
            bool recieved = false;
            while (!recieved && recv_tries < UDP_MAX_RECV_TRIES)
            {
                if (recvUdp(sock, rx_buffer, rx_buffer_len) < 0)
                {
                    ESP_LOGE(UDP_TAG, "Error receiving message");
                    recv_tries++;
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }

                // check if the message is valid
                char recv_OK = rx_buffer[0];
                if (!recv_OK)
                {
                    ESP_LOGE(UDP_TAG, "invalid message received");
                    recv_tries++;
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }
                ESP_LOGI(UDP_TAG, "valid T2 message received");
                recv_tries = 0;
                recieved = true;

                char recv_CHANGE = rx_buffer[1];
                char recv_STATUS = rx_buffer[2];
                char recv_PROTOCOL = rx_buffer[3];
                ESP_LOGI(UDP_TAG, "OK: %d, CHANGE: %d, STATUS: %d, PROTOCOL: %d", recv_OK, recv_CHANGE, recv_STATUS, recv_PROTOCOL);

                if (recv_CHANGE)
                {
                    bool statusChanged = false;
                    if (recv_STATUS != config.STATUS)
                    {
                        statusChanged = true;
                    }

                    config.STATUS = recv_STATUS;
                    config.ID_PROTOCOL = recv_PROTOCOL;

                    writeConfig(config);

                    ESP_LOGI(UDP_TAG, "config changed");

                    if (statusChanged)
                    {
                        ESP_LOGI(UDP_TAG, "closing socket due to status change");
                        shutdown(sock, 0);
                        close(sock);
                        return 0;
                    }
                }
            }
        }
    }
}

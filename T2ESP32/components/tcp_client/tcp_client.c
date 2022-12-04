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
#include "esp_wifi.h"
#include "esp_sleep.h"

#include "lwip/err.h"
#include "lwip/sockets.h"

#include "tcp_client.h"
#include "configuracion.h"
#include "empaquetamiento.h"

/**
 * @brief creates tcp socket
 *
 * @return int socket descriptor, -1 if error
 */
int socketTcp()
{
    /* Create a TCP socket */
    int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);

    if (sock < 0)
    {
        ESP_LOGE(TCP_TAG, "Unable to create socket: errno %d", errno);
    }
    else
    {
        ESP_LOGI(TCP_TAG, "Socket created");
        timeoutTcp(sock, TCP_TIMEOUT_SEC);
    }

    return sock;
}

bool checkTcp(T2_CONFIG config)
{
    return (
        config.STATUS == 20 ||
        config.STATUS == 21 ||
        config.STATUS == 22);
}

/**
 * @brief sets the socket timeout
 *
 * @param sock socket descriptor
 * @param timeout_sec timeout in seconds
 * @return int 0 if ok, -1 if error
 */
int timeoutTcp(int sock, int timeout_sec)
{
    struct timeval tv;
    tv.tv_sec = timeout_sec;
    if (setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        ESP_LOGE(TCP_TAG, "Error setting socket timeout: errno %d", errno);
        return -1;
    }

    return 0;
}

/**
 * @brief connects to a server
 *
 * @param sock socket descriptor
 * @param ip server ip
 * @param port server port
 * @return int 0 if ok, -1 if error
 */
int connectTcp(int sock, char *ip, int port)
{
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(port);

    /* Connect to the remote host */
    int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0)
    {
        ESP_LOGE(TCP_TAG, "Socket unable to connect: errno %d", errno);
    }
    else
    {
        ESP_LOGI(TCP_TAG, "Successfully connected");
    }
    return err;
}

int sendTcp(int sock, char *message, int message_len)
{
    int err = send(sock, message, message_len, 0);
    if (err < 0)
    {
        ESP_LOGE(TCP_TAG, "Error occurred during sending: errno %d", errno);
    }
    else
    {
        ESP_LOGI(TCP_TAG, "Message sent");
    }
    return err;
}

int recvTcp(int sock, char *rx_buffer, int rx_buffer_len)
{
    ESP_LOGI(TCP_TAG, "Waiting for data");
    int len = recv(sock, rx_buffer, rx_buffer_len - 1, 0);
    // Error occurred during receiving
    if (len <= 0)
    {
        ESP_LOGE(TCP_TAG, "recv failed: errno %d", errno);
    }
    // Data received
    else
    {
        rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
        ESP_LOGI(TCP_TAG, "Received %d bytes: %s", len, rx_buffer);
    }
    return len;
}

/**
 * @brief function to manage a tcp client main loop.
 * It works for status 21 and 22
 *
 *
 * @return int
 */
int manageTcp()
{
    T2_CONFIG config = readConfig();

    char rx_buffer[16];
    int rx_buffer_len = sizeof(rx_buffer);

    while (1)
    {
        if (!checkTcp(config))
        {
            ESP_LOGI(TCP_TAG, "Status %d not allowed", config.STATUS);
            return -1;
        }

        // socket creation
        int sock = socketTcp();
        if (sock < 0)
        {
            if (sock != -1)
            {
                ESP_LOGE(TCP_TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // socket connection
        char *host_ip = numToIp(config.HOST_IP_ADDR);
        if (connectTcp(sock, host_ip, config.PORT_TCP) != 0)
        {
            if (sock != -1)
            {
                ESP_LOGE(TCP_TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // send and recv loop
        int send_tries = 0;
        while (send_tries < TCP_MAX_SEND_TRIES)
        {
            ESP_LOGI(TCP_TAG, "Sending message to %s:%ld", host_ip, config.PORT_TCP);
            char *msg = mensaje(config.ID_PROTOCOL, config.STATUS, 1);

            int err = sendTcp(sock, msg, messageLen(config.ID_PROTOCOL));
            free(msg);

            if (err < 0)
            {
                send_tries++;
                // wait for the next send try
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            send_tries = 0;

            // receive loop
            int recv_tries = 0;
            while (recv_tries < TCP_MAX_RECV_TRIES)
            {
                if (recvTcp(sock, rx_buffer, rx_buffer_len) < 0)
                {
                    recv_tries++;
                    // wait for the next receive try
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }

                // check if the message is valid
                char recv_OK = rx_buffer[0];
                if (recv_OK)
                {
                    ESP_LOGI(TCP_TAG, "valid T2 message received");
                    break;
                }
                else
                {
                    ESP_LOGE(TCP_TAG, "invalid message received");
                    recv_tries++;
                    // wait for the nex receive try
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }
            }
            recv_tries = 0;

            char recv_OK = rx_buffer[0];
            char recv_CHANGE = rx_buffer[1];
            char recv_STATUS = rx_buffer[2];
            char recv_PROTOCOL = rx_buffer[3];
            ESP_LOGI(TCP_TAG, "OK: %d, CHANGE: %d, STATUS: %d, PROTOCOL: %d", recv_OK, recv_CHANGE, recv_STATUS, recv_PROTOCOL);

            if (recv_CHANGE)
            {
                bool transportLayerChanged = false;
                if (recv_STATUS != 21 && recv_STATUS != 22)
                {
                    transportLayerChanged = true;
                }

                config.STATUS = recv_STATUS;
                config.ID_PROTOCOL = recv_PROTOCOL;

                writeConfig(config);

                ESP_LOGI(TCP_TAG, "config changed");

                if (transportLayerChanged)
                {
                    ESP_LOGI(TCP_TAG, "closing socket due to status change");
                    shutdown(sock, 0);
                    close(sock);
                    return 0;
                }
            }

            // if configured status is 22, then we deepsleep for the configured time
            if (config.STATUS == 22)
            {
                ESP_LOGI(TCP_TAG, "deepsleep enabled");
                esp_sleep_enable_timer_wakeup(config.DISCONTINUOS_TIME * 60 * 1000000);
                // stop the wifi
                esp_wifi_stop();
                ESP_LOGI(TCP_TAG, "wifi stopped, entering deepsleep for %ld minutes", config.DISCONTINUOS_TIME);

                // enter deepsleep
                esp_deep_sleep_start();

                // restore the wifi
                esp_wifi_restore();
            }

            // wait for the next send
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
}

int configurateTcpClient()
{
    T2_CONFIG config = readConfig();

    char rx_buffer[128];
    int rx_buffer_len = sizeof(rx_buffer);
    char *host_ip = numToIp(config.HOST_IP_ADDR);

    if (!checkTcp(config))
    {
        ESP_LOGI(TCP_TAG, "Status %d not allowed", config.STATUS);
        return -1;
    }

    ESP_LOGI(TCP_TAG, "Configuring ESP32 through TCP");

    while (1)
    {
        // socket creation
        int sock = socketTcp();
        if (sock < 0)
        {
            if (sock != -1)
            {
                ESP_LOGE(TCP_TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // socket connection
        if (connectTcp(sock, host_ip, config.PORT_TCP) != 0)
        {
            if (sock != -1)
            {
                ESP_LOGE(TCP_TAG, "Shutting down socket and restarting...");
                shutdown(sock, 0);
                close(sock);
            }

            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        // send and recv loop
        int send_tries = 0;
        bool recieved = false;
        while (!recieved && send_tries < TCP_MAX_SEND_TRIES)
        {
            ESP_LOGI(TCP_TAG, "Sending message to %s:%ld", host_ip, config.PORT_TCP);
            char *msg = mensaje(0, 20, 0);

            int err = sendTcp(sock, msg, messageLen(0));
            free(msg);

            if (err < 0)
            {
                send_tries++;
                // wait for the next send try
                vTaskDelay(1000 / portTICK_PERIOD_MS);
                continue;
            }
            send_tries = 0;

            // receive loop
            int recv_tries = 0;
            while (recv_tries < TCP_MAX_RECV_TRIES)
            {
                if (recvTcp(sock, rx_buffer, rx_buffer_len) < 0)
                {
                    recv_tries++;
                    // wait for the next receive try
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }

                // check if the message is valid
                char recv_OK = rx_buffer[0];
                if (recv_OK)
                {
                    ESP_LOGI(TCP_TAG, "valid T2 message received");
                    recieved = true;
                    break;
                }
                else
                {
                    ESP_LOGE(TCP_TAG, "invalid message received");
                    recv_tries++;
                    // wait for the nex receive try
                    vTaskDelay(1000 / portTICK_PERIOD_MS);
                    continue;
                }
            }
            recv_tries = 0;

            ESP_LOGI(TCP_TAG, "Extracting configuration from buffer:");
            T2_CONFIG new_config = extractConfig(rx_buffer);
            printConfig(new_config);

            writeConfig(new_config);

            ESP_LOGI(TCP_TAG, "config changed");

            ESP_LOGI(TCP_TAG, "finished configuration, closing socket");
            shutdown(sock, 0);
            close(sock);
            return 0;
        }
    }
}

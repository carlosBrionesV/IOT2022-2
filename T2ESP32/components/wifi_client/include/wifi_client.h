
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

static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);

int wifi_init_sta(char *ssid, char *pass, int authmode);

int wifi_connect(char *ssid, char *pass, wifi_auth_mode_t authmode);
int wifi_stop();
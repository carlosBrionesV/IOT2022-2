
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"

#include "esp_bt.h"          // implements BT controller and VHCI configuration procedures from the host side
#include "esp_gap_ble_api.h" // implements GAP configuration such as advertising and connections parameters
#include "esp_gatts_api.h"   // implements GATT Server configuration such as creating services and characteristics
#include "esp_bt_main.h"     // implements initialization and enabling of the bluedroid stack
#include "esp_gatt_common_api.h"

/* Attributes State Machine */
enum
{
    IDX_SVC,                // index of service

    // -- service characteristics

    IDX_CHAR_B,             // index characteristic B of the service
    IDX_CHAR_VAL_B,         // index value of B

    IDX_CHAR_C,
    IDX_CHAR_VAL_C,

    // --
    // number of table elements
    IDX_NB,
};


#define BLE_TAG "BLE_SERVER"

#define PROFILE_NUM                 1
#define PROFILE_APP_IDX             0
#define ESP_APP_ID                  0x55
#define SVC_INST_ID                 0

#ifdef CONFIG_ESP_DEVICE_NAME
#define SAMPLE_DEVICE_NAME          CONFIG_ESP_DEVICE_NAME
#else
#define SAMPLE_DEVICE_NAME          "ESP_32"
#endif

/* The max length of characteristic value. When the GATT client performs a write or prepare write operation,
*  the data length must be less than GATTS_DEMO_CHAR_VAL_LEN_MAX.
*/
#define GATTS_CHAR_VAL_LEN_MAX      500
#define PREPARE_BUF_MAX_SIZE        1024
#define CHAR_DECLARATION_SIZE       (sizeof(uint8_t))

#define ADV_CONFIG_FLAG             (1 << 0)
#define SCAN_RSP_CONFIG_FLAG        (1 << 1)

typedef struct {
    uint8_t                 *prepare_buf;
    int                     prepare_len;
} prepare_type_env_t;

struct gatts_profile_inst {
    esp_gatts_cb_t gatts_cb;
    uint16_t gatts_if;
    uint16_t app_id;
    uint16_t conn_id;
    uint16_t service_handle;
    esp_gatt_srvc_id_t service_id;
    uint16_t char_handle;
    esp_bt_uuid_t char_uuid;
    esp_gatt_perm_t perm;
    esp_gatt_char_prop_t property;
    uint16_t descr_handle;
    esp_bt_uuid_t descr_uuid;
};

void gap_event_handler(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
void example_prepare_write_event_env(esp_gatt_if_t gatts_if, prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void example_exec_write_event_env(prepare_type_env_t *prepare_write_env, esp_ble_gatts_cb_param_t *param);
void gatts_profile_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
void gatts_event_handler(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);
int initBleServer();
void bleConfig();

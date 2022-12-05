#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "almacenamiento.h"

int initNVS(void)
{
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
    return 0;
}

int deinitNVS(void)
{
    // deinitialize NVS
    esp_err_t ret = nvs_flash_deinit();
    ESP_ERROR_CHECK( ret );
    return 0;
}

int eraseStorage(void)
{
    nvs_handle_t my_handle;
    // open NVS
    esp_err_t ret = nvs_open("storage", NVS_READWRITE, &my_handle);
    ESP_ERROR_CHECK( ret );
    // erase NVS
    ret = nvs_erase_all(my_handle);
    ESP_ERROR_CHECK( ret );
    // close NVS
    nvs_close(my_handle);
    return 0;
}

int readStorageValue(int key, int8_t *i8Value, int32_t *i32Value, char *strValue)
{
    nvs_handle_t read_handle;
    // Open
    esp_err_t err = nvs_open("storage", NVS_READONLY, &read_handle);
    if (err != ESP_OK) return -1;

    size_t length = STRING_SIZE;
    // Read
    switch (key) 
    {
    // int8_t
    case CONFIGURATED_KEY:
        err = nvs_get_i8(read_handle, "CONFIGURATED", i8Value);
        if (err != ESP_OK) *i8Value = 0;
        break;
    case STATUS_KEY:
        err = nvs_get_i8(read_handle, "STATUS", i8Value);
        if (err != ESP_OK) *i8Value = 0;
        break;
    case ID_PROTOCOL_KEY:
        err = nvs_get_i8(read_handle, "ID_PROTOCOL", i8Value);
        if (err != ESP_OK) *i8Value = 0;
        break;
    // int32_t
    case BMI270_SAMPLING_KEY:
        err = nvs_get_i32(read_handle, "BMI270_SAMPLING", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case BMI270_ACC_SENSIBILITY_KEY:
        err = nvs_get_i32(read_handle, "BMI270_ACC_SENSIBILITY", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case BMI270_GYRO_SENSIBILITY_KEY:
        err = nvs_get_i32(read_handle, "BMI270_GYRO_SENSIBILITY", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case BME688_SAMPLING_KEY:
        err = nvs_get_i32(read_handle, "BME688_SAMPLING", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case DISCONTINUOS_TIME_KEY:
        err = nvs_get_i32(read_handle, "DISCONTINUOS_TIME", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case PORT_TCP_KEY:
        err = nvs_get_i32(read_handle, "PORT_TCP", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case PORT_UDP_KEY:
        err = nvs_get_i32(read_handle, "PORT_UDP", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    case HOST_IP_ADDR_KEY:
        err = nvs_get_i32(read_handle, "HOST_IP_ADDR", i32Value);
        if (err != ESP_OK) *i32Value = 0;
        break;
    // string
    case SSID_KEY:
        err = nvs_get_str(read_handle, "SSID", strValue, &length);
        if (err != ESP_OK) strcpy(strValue, "");
        break;
    case PASS_KEY:
        err = nvs_get_str(read_handle, "PASS", strValue, &length);
        if (err != ESP_OK) strcpy(strValue, "");
        break;
    default:
        err = ESP_ERR_NVS_NOT_FOUND;
        break;
    }
    
    // Close
    nvs_close(read_handle);
    if (err != ESP_OK) return -2;
    return 0;
}

int writeStorageValue(int key, int8_t i8Value, int32_t i32Value, char *strValue)
{
    nvs_handle_t write_handle;
    // Open
    esp_err_t err = nvs_open("storage", NVS_READWRITE, &write_handle);
    if (err != ESP_OK) return -1;
    // Write
    switch (key) 
    {
    // int8_t
    case CONFIGURATED_KEY:
        err = nvs_set_i8(write_handle, "CONFIGURATED", i8Value);
        break;
    case STATUS_KEY:
        err = nvs_set_i8(write_handle, "STATUS", i8Value);
        break;
    case ID_PROTOCOL_KEY:
        err = nvs_set_i8(write_handle, "ID_PROTOCOL", i8Value);
        break;
    // int32_t
    case BMI270_SAMPLING_KEY:
        err = nvs_set_i32(write_handle, "BMI270_SAMPLING", i32Value);
        break;
    case BMI270_ACC_SENSIBILITY_KEY:
        err = nvs_set_i32(write_handle, "BMI270_ACC_SENSIBILITY", i32Value);
        break;
    case BMI270_GYRO_SENSIBILITY_KEY:
        err = nvs_set_i32(write_handle, "BMI270_GYRO_SENSIBILITY", i32Value);
        break;
    case BME688_SAMPLING_KEY:
        err = nvs_set_i32(write_handle, "BME688_SAMPLING", i32Value);
        break;
    case DISCONTINUOS_TIME_KEY:
        err = nvs_set_i32(write_handle, "DISCONTINUOS_TIME", i32Value);
        break;
    case PORT_TCP_KEY:
        err = nvs_set_i32(write_handle, "PORT_TCP", i32Value);
        break;
    case PORT_UDP_KEY:
        err = nvs_set_i32(write_handle, "PORT_UDP", i32Value);
        break;
    case HOST_IP_ADDR_KEY:
        err = nvs_set_i32(write_handle, "HOST_IP_ADDR", i32Value);
        break;
    // string
    case SSID_KEY:
        err = nvs_set_str(write_handle, "SSID", strValue);
        break;
    case PASS_KEY:
        err = nvs_set_str(write_handle, "PASS", strValue);
        break;
    default:
        err = ESP_ERR_NVS_NOT_FOUND;
        break;
    }
    
    // Commit
    esp_err_t err2 = nvs_commit(write_handle);
    // Close
    nvs_close(write_handle);
    if (err2 != ESP_OK) return -1;
    if (err != ESP_OK) return -2;
    return 0;
}

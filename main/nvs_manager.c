
/* ===========================
*          INCLUDES
* =========================== */
#include <string.h>
#include "esp_check.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"

#include "config.h"
#include "nvs_manager.h"
/* ===========================
 *           DEFINES
 * =========================== */

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */
static const char *TAG = "NVS";
/* ===========================
 *    LOCAL PROTOTYPES
 * =========================== */
static esp_err_t nvs_manager_read_all();
static esp_err_t nvs_manager_write_all_default();
static esp_err_t nvs_open_namespace(nvs_handle_t *handle);

/* ===========================
 *   GLOBAL FUNCTIONS
 * =========================== */
esp_err_t nvs_manager_init(void)
{
    esp_err_t ret = nvs_flash_init();

    if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
        ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    esp_err_t err = nvs_read_u16(dNVS_KEY_FIRST_BOOT, &nvs_parameters.first_boot);
    if(err == ESP_ERR_NVS_NOT_FOUND)
    {
        ESP_LOGI(TAG, "NOT FOUND");
        ESP_RETURN_ON_ERROR(nvs_manager_write_all_default(), TAG, "nvs_manager_write_all_default fail");
    }
    ESP_RETURN_ON_ERROR(nvs_manager_read_all(), TAG, "nvs_manager_read_all fail");

    ESP_LOGI(TAG, "NVS Initialized");
    
    return ESP_OK;
}

esp_err_t nvs_write_u16(const char *key, uint16_t value)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    esp_err_t err = nvs_set_u16(handle, key, value);

    if (err == ESP_OK)
        err = nvs_commit(handle);

    nvs_close(handle);
    return err;
}

esp_err_t nvs_write_str(const char *key, const char *value)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    esp_err_t err = nvs_set_str(handle, key, value);

    if (err == ESP_OK)
        err = nvs_commit(handle);

    nvs_close(handle);
    return err;
}

esp_err_t nvs_write_blob(const char *key, const void *data, size_t length)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    esp_err_t err = nvs_set_blob(handle, key, data, length);

    if (err == ESP_OK)
        err = nvs_commit(handle);

    nvs_close(handle);
    return err;
}

esp_err_t nvs_read_u16(const char *key, uint16_t *value)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    esp_err_t err = nvs_get_u16(handle, key, value);

    nvs_close(handle);
    return err;
}

esp_err_t nvs_read_str(const char *key, char *buffer, size_t buffer_size)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    size_t required_size = buffer_size;
    esp_err_t err = nvs_get_str(handle, key, buffer, &required_size);

    nvs_close(handle);
    return err;
}

esp_err_t nvs_read_blob(const char *key, void *buffer, size_t *required_size)
{
    nvs_handle_t handle;
    if (nvs_open_namespace(&handle) != ESP_OK)
        return ESP_FAIL;

    esp_err_t err = nvs_get_blob(handle, key, buffer, required_size);

    nvs_close(handle);
    return err;
}
/* ===========================
 *   LOCAL FUNCTIONS
 * =========================== */
static esp_err_t nvs_manager_read_all(void)
{
    size_t blob_size = sizeof(nvs_parameters.wifi);

    ESP_RETURN_ON_ERROR(nvs_read_u16(dNVS_KEY_FIRST_BOOT, &nvs_parameters.first_boot), TAG, "read first_boot failed");

    ESP_RETURN_ON_ERROR(nvs_read_str(dNVS_KEY_DEVICE_ID, nvs_parameters.device_id, sizeof(nvs_parameters.device_id)), TAG, "read device_id failed");

    ESP_RETURN_ON_ERROR(nvs_read_blob(dNVS_KEY_WIFI_CFG, &nvs_parameters.wifi, &blob_size), TAG, "read wifi failed");

    return ESP_OK;
}

static esp_err_t nvs_manager_write_all_default(void)
{
    nvs_parameters.first_boot = dNVS_FIRST_BOOT_VALUE;

    strncpy(nvs_parameters.wifi.ssid, dNVS_WIFI_SSID_VALUE, WIFI_MAX_SSID_LEN - 1);

    strncpy(nvs_parameters.wifi.password, dNVS_WIFI_PASSWORD_VALUE, WIFI_MAX_PASS_LEN - 1);

    config_generate_device_id(nvs_parameters.device_id, sizeof(nvs_parameters.device_id));

    ESP_RETURN_ON_ERROR(nvs_write_u16(dNVS_KEY_FIRST_BOOT,  nvs_parameters.first_boot), TAG, "write first_boot failed");

    ESP_RETURN_ON_ERROR(nvs_write_blob(dNVS_KEY_WIFI_CFG, &nvs_parameters.wifi, sizeof(nvs_parameters.wifi)), TAG, "write wifi failed");

    ESP_RETURN_ON_ERROR(nvs_write_str(dNVS_KEY_DEVICE_ID, nvs_parameters.device_id), TAG, "write device_id failed");

    return ESP_OK;
}

static esp_err_t nvs_open_namespace(nvs_handle_t *handle)
{
    return nvs_open(dNVS_NAMESPACE, NVS_READWRITE, handle);
}
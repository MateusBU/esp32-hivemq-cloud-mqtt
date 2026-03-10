#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

/* ===========================
 *          INCLUDES
 * =========================== */
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include "esp_err.h"
#include "secrets.h"
/* ===========================
 *           DEFINES
 * =========================== */
#define dNVS_NAMESPACE     "app_storage"
#define dNVS_KEY_FIRST_BOOT "first_boot"
#define dNVS_KEY_DEVICE_ID "device_id_key"
#define dNVS_KEY_WIFI_CFG   "wifi_key"

#define dNVS_FIRST_BOOT_VALUE 1
#define dNVS_WIFI_SSID_VALUE dWIFI_SSID
#define dNVS_WIFI_PASSWORD_VALUE dWIFI_PASSWORD
#define dNVS_DEVICE_ID_VALUE "device_id_value"

#define DEVICE_ID_LEN 32
#define WIFI_MAX_SSID_LEN 32
#define WIFI_MAX_PASS_LEN 64
/* ===========================
 *            TYPES
 * =========================== */

/* ===========================
 *     GLOBAL VARIABLES
 * =========================== */
typedef struct
{
    char ssid[WIFI_MAX_SSID_LEN];
    char password[WIFI_MAX_PASS_LEN];
} wifi_credentials_t;

typedef struct
{
    uint16_t first_boot;
    wifi_credentials_t wifi;
    char device_id[DEVICE_ID_LEN];
} nvs_parameters_t;


nvs_parameters_t nvs_parameters;
/* ===========================
 *   FUNCTION DECLARATIONS
 * =========================== */
esp_err_t nvs_manager_init(void);

esp_err_t nvs_write_u16(const char *key, uint16_t value);
esp_err_t nvs_write_str(const char *key, const char *value);
esp_err_t nvs_write_blob(const char *key, const void *data, size_t length);

esp_err_t nvs_read_u16(const char *key, uint16_t *value);
esp_err_t nvs_read_str(const char *key, char *buffer, size_t buffer_size);
esp_err_t nvs_read_blob(const char *key, void *buffer, size_t *required_size);
#endif
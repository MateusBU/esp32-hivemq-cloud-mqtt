/* ===========================
 *          INCLUDES
 * =========================== */
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_flash.h"
#include "wifi_manager.h"

#include "secrets.h"
/* ===========================
 *           DEFINES
 * =========================== */
#define WIFI_SSID dWIFI_SSID
#define WIFI_PASS dWIFI_PASSWORD
#define WIFI_CONNECTED_BIT BIT0

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */
static const char *TAG = "WIFI";
static EventGroupHandle_t wifi_event_group;
static wifi_config_t wifi_config;
/* ===========================
 *    LOCAL PROTOTYPES
 * =========================== */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
/* ===========================
 *   GLOBAL FUNCTIONS
 * =========================== */
void wifi_init(void)
{
    wifi_event_group = xEventGroupCreate();

    esp_netif_init();
    esp_event_loop_create_default();
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);

    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL);

    esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &wifi_event_handler, NULL, NULL);

    memset(&wifi_config, 0, sizeof(wifi_config));

    strcpy((char *)wifi_config.sta.ssid, WIFI_SSID);
    strcpy((char *)wifi_config.sta.password, WIFI_PASS);

    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_set_config(WIFI_IF_STA, &wifi_config);
    esp_wifi_start();
}

bool wifi_is_connected(void)
{
    EventBits_t bits = xEventGroupGetBits(wifi_event_group);
    return (bits & WIFI_CONNECTED_BIT);
}
/* ===========================
 *   LOCAL FUNCTIONS
 * =========================== */
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        esp_wifi_connect();
        ESP_LOGI(TAG, "Reconnecting...");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        xEventGroupSetBits(wifi_event_group, WIFI_CONNECTED_BIT);
        ESP_LOGI(TAG, "WiFi Connected");
    }
}
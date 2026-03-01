/* ===========================
 *          INCLUDES
 * =========================== */
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "mqtt_manager.h"
#include "nvs_manager.h"
#include "secrets.h"
/* ===========================
 *           DEFINES
 * =========================== */

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */
static const char *TAG = "MQTT";
static esp_mqtt_client_handle_t client;
extern const uint8_t isrg_root_x1_cross_signed_pem_start[] 
    asm("_binary_isrg_root_x1_cross_signed_pem_start");
extern const uint8_t isrg_root_x1_cross_signed_pem_end[]   
    asm("_binary_isrg_root_x1_cross_signed_pem_end");
/* ===========================
 *    LOCAL PROTOTYPES
 * =========================== */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_task(void *pvParameters);
/* ===========================
 *   GLOBAL FUNCTIONS
 * =========================== */
void mqtt_start(void)
{
    xTaskCreate(mqtt_task, "mqtt_task", 4096, NULL, 5, NULL);
}
/* ===========================
 *   LOCAL FUNCTIONS
 * =========================== */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    switch ((esp_mqtt_event_id_t)event_id)
    {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT Connected");
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT Disconnected");
            break;

        default:
            break;
    }
}

static void mqtt_task(void *pvParameters)
{
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
        .cert_pem = (const char *)isrg_root_x1_cross_signed_pem_start,
    };

    char topic[64];
    sprintf(topic, "devices/%s/data", nvs_paramaters.device_id);
    ESP_LOGI(TAG, "device id: %s", nvs_paramaters.device_id);


    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); //register callback mqtt_event_handler
    esp_mqtt_client_start(client);

    while (1)
    {
        //esp_mqtt_client_publish(client, topic, payload, len, qos, retain);
        int msg_id = esp_mqtt_client_publish(client, topic, "Mensagem do ESP32", 0, 1, 0);
        ESP_LOGI(TAG, "Publish msg_id=%d", msg_id);

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}
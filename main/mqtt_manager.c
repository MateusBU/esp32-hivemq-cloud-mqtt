/* ===========================
 *          INCLUDES
 * =========================== */
#include "mqtt_client.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "cJSON.h"

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

char status_topic[64];
/* ===========================
 *    LOCAL PROTOTYPES
 * =========================== */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data);
static void mqtt_task(void *pvParameters);
static void mqtt_publish_telemetry(void);
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
            esp_mqtt_client_publish(client, status_topic, "online", 0, 1, 1);
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

    sprintf(status_topic, "devices/%s/status", nvs_parameters.device_id);
    ESP_LOGI(TAG, "USER: %s", nvs_parameters.device_id);

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_URI,
        .username = nvs_parameters.device_id,
        .password = MQTT_PASSWORD,
        .cert_pem = (const char *)isrg_root_x1_cross_signed_pem_start,

        // Last Will and Testament (broker publishs 'offline' automatically if the device is down)
        .lwt_topic = status_topic,
        .lwt_msg = "offline",
        .lwt_qos = 1,
        .lwt_retain = 1,
    };

    char topic[64];
    sprintf(topic, "devices/%s/data", nvs_parameters.device_id);
    ESP_LOGI(TAG, "device id: %s", nvs_parameters.device_id);


    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL); //register callback mqtt_event_handler
    esp_mqtt_client_start(client);

    int msg_id = esp_mqtt_client_publish(client, topic, "Running...", 0, 1, 0);
    while (1)
    {
        //esp_mqtt_client_publish(client, topic, payload, len, qos, retain);
        mqtt_publish_telemetry();

        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

static void mqtt_publish_telemetry(void)
{
    char topic[64];

    sprintf(topic, "devices/%s/telemetry", nvs_parameters.device_id);

    int heap = esp_get_free_heap_size();
    int uptime = esp_timer_get_time() / 1000000;

    int temperature = 20 + (esp_random() % 10);

    cJSON *root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "device_id", nvs_parameters.device_id);
    cJSON_AddNumberToObject(root, "heap", heap);
    cJSON_AddNumberToObject(root, "uptime", uptime);
    cJSON_AddNumberToObject(root, "temperature", temperature);

    char *payload = cJSON_PrintUnformatted(root);

    ESP_LOGI(TAG, "%s", payload);
    
    esp_mqtt_client_publish(client, topic, payload, 0, 1, 0);

    cJSON_Delete(root);
    free(payload);
}
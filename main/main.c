
/* ===========================
*          INCLUDES
* =========================== */
#include "nvs_flash.h"
#include "wifi_manager.h"
#include "mqtt_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

/* ===========================
 *           DEFINES
 * =========================== */

/* ===========================
 *     LOCAL VARIABLES
 * =========================== */

/* ===========================
 *    LOCAL PROTOTYPES
 * =========================== */

/* ===========================
 *   GLOBAL FUNCTIONS
 * =========================== */
void app_main(void)
{
    nvs_flash_init();

    wifi_init();

    // Espera WiFi conectar
    while (!wifi_is_connected())
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }

    mqtt_start();
}
/* ===========================
 *   LOCAL FUNCTIONS
 * =========================== */

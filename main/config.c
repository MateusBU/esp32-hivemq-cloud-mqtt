
/* ===========================
*          INCLUDES
* =========================== */
#include "esp_system.h"
#include "esp_mac.h"

#include "config.h"
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
void config_generate_device_id(char *buffer, size_t size)
{
    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    snprintf(buffer, size,
             "esp32_%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2],
             mac[3], mac[4], mac[5]);
}
/* ===========================
 *   LOCAL FUNCTIONS
 * =========================== */

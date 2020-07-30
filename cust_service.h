
#ifndef CUST_SERVICE_H__
#define CUST_SERVICE_H__

#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"

// FROM_SERVICE_TUTORIAL: Defining 16-bit service and 128-bit base UUIDs
#define BLE_UUID_OUR_BASE_UUID              {{0x23, 0xD1, 0x13, 0xEF, 0x5F, 0x78, 0x23, 0x15, 0xDE, 0xEF, 0x12, 0x12, 0x00, 0x00, 0x00, 0x00}} // 128-bit base UUID
#define BLE_UUID_OUR_SERVICE_UUID                0xF00D // Just a random, but recognizable value
#define BLE_UUID_FLS_SERVICE_UUID                0xABCD

// ALREADY_DONE_FOR_YOU: Defining 16-bit characteristic UUID
#define BLE_UUID_OUR_CHARACTERISTC_UUID          0xBEEF // Just a random, but recognizable value
#define BLE_UUID_OUR_CHARACTERISTC_UUID2         0xB00B // Just a random, but recognizable value
#define BLE_UUID_OUR_CHARACTERISTC_UUID3         0xBABA // Just a random, but recognizable value

#define BLE_UUID_FLASH_CHARACTERISTC_UUID          0x0007 // Just a random, but recognizable value
#define BLE_UUID_FLASH_CHARACTERISTC_UUID2         0xACAB // Just a random, but recognizable value
#define BLE_UUID_FLASH_CHARACTERISTC_UUID3         0xA555 // Just a random, but recognizable value


typedef enum 
{
    OS_EVT_TEMPERATURE_NOTIF_ENABLE,
    OS_EVT_SAADC_NOTIF_ENABLE
} os_evt_type;

typedef struct
{
    os_evt_type     event_type;
    uint16_t        conn_handle;
    uint8_t*        pdata;
    uint8_t         data_length;
} os_evt;

typedef struct ble_os_s ble_os_t;

typedef void (*ble_os_event_handler_t) (uint32_t char_val, const os_evt * p_evt);

typedef struct
{
    ble_os_event_handler_t  event_handler;
}ble_os_init_t;

// This structure contains various status information for our service. 
// The name is based on the naming convention used in Nordics SDKs. 
// 'ble’ indicates that it is a Bluetooth Low Energy relevant structure and 
// ‘os’ is short for Our Service). 
struct ble_os_s
{
    uint16_t                    conn_handle;    /**< Handle of the current connection (as provided by the BLE stack, is BLE_CONN_HANDLE_INVALID if not in a connection).*/
    uint16_t                    service_handle; /**< Handle of Our Service (as provided by the BLE stack). */
    // OUR_JOB: Step 2.D, Add handles for the characteristic attributes to our struct
    ble_gatts_char_handles_t    char_handles;
    ble_gatts_char_handles_t    char_handles2;
    ble_gatts_char_handles_t    char_handles3;
    ble_os_event_handler_t      event_handler;
};







/**@brief Function for handling BLE Stack events related to our service and characteristic.
 *
 * @details Handles all events from the BLE stack of interest to Our Service.
 *
 * @param[in]   p_our_service       Our Service structure.
 * @param[in]   p_ble_evt  Event received from the BLE stack.
 */
void ble_c_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context);

/**@brief Function for initializing our new service.
 *
 * @param[in]   p_our_service       Pointer to Our Service structure.
 */
void c_service_init(ble_os_t * p_our_service, ble_os_init_t * p_srv_init);

void c_flash_serv_init(ble_os_t * p_our_service);

/**@brief Function for updating and sending new characteristic values
 *
 * @details The application calls this function whenever our timer_timeout_handler triggers
 *
 * @param[in]   p_our_service                     Our Service structure.
 * @param[in]   characteristic_value     New characteristic value.
 */
void temperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value);


void saadc_characteristic_update(ble_os_t *p_our_service, int16_t *temperature_value);

void flash_cnt_char_update(ble_os_t *p_our_service, uint32_t *cnt_val);

#endif  /* _ CUST_SERVICE_H__ */


#include <stdint.h>
#include <string.h>
#include "nrf_gpio.h"
#include "cust_service.h"
#include "ble_srv_common.h"
#include "app_error.h"


#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"


static void on_write(ble_os_t *p_service, ble_evt_t const * p_ble_evt)
{
//    NRF_LOG_INFO("on_write: Called");
    ble_gatts_evt_write_t const * p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
    os_evt event;

    if((p_evt_write->handle == p_service->char_handles3.value_handle))
    {
         
//        NRF_LOG_INFO("on_write: started");
        uint32_t charval_val;
        uint8_t  value[4];
        for(int i=0;i<p_evt_write->len;i++)
        {
              value[i] = p_evt_write->data[i];
        }
//        NRF_LOG_INFO(" ------> %x %x %x %x",value[0],value[1],value[2],value[3]);
        charval_val = uint32_decode(value);
//        NRF_LOG_INFO(" ------> %x",charval_val);
//        NRF_LOG_INFO(" ------> %d",charval_val);

//         event.event_type = OS_EVT_TEMPERATURE_NOTIF_ENABLE;
        p_service->event_handler(charval_val,&event);

         
    }
}


void ble_c_service_on_ble_evt(ble_evt_t const * p_ble_evt, void * p_context)
{
  	ble_os_t * p_service =(ble_os_t *) p_context;  

	switch (p_ble_evt->header.evt_id)
        {
            case BLE_GAP_EVT_CONNECTED:
                p_service->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
                break;
            case BLE_GAP_EVT_DISCONNECTED:
                p_service->conn_handle = BLE_CONN_HANDLE_INVALID;
                break;
            case BLE_GATTS_EVT_WRITE:
                on_write(p_service, p_ble_evt);
                break;
            default:
                // No implementation needed.
                break;
        }
	
}

/**@brief Function for adding our new characterstic to "Our service" that we initiated in the previous tutorial. 
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
static uint32_t char_temp_add(ble_os_t * p_our_service)
{
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code); 

    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
//    char_md.char_props.write = 1;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    attr_char_value.max_len     = 1;
    attr_char_value.init_len    = 1;
    uint8_t value[1]            = {0};
    attr_char_value.p_value     = value;


    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles);
    APP_ERROR_CHECK(err_code);
    return NRF_SUCCESS;
}

static uint32_t char_adc_add(ble_os_t * p_our_service)
{
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;

    char_uuid.uuid     =BLE_UUID_OUR_CHARACTERISTC_UUID2;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code); 

    ble_gatts_char_md_t char_md;

    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
//    char_md.char_props.write = 1;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    attr_char_value.max_len     = 2;
    attr_char_value.init_len    = 2;
    uint8_t value[2]            = {0,0};
    attr_char_value.p_value     = value;
    
    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles2);
    APP_ERROR_CHECK(err_code);

    return NRF_SUCCESS;
}

static uint32_t char_interval_add(ble_os_t * p_our_service)
{
    uint32_t            err_code;
    ble_uuid_t          char_uuid;
    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
    char_uuid.uuid      = BLE_UUID_OUR_CHARACTERISTC_UUID3;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
    APP_ERROR_CHECK(err_code); 

    ble_gatts_char_md_t char_md;
    memset(&char_md, 0, sizeof(char_md));
    char_md.char_props.read = 1;
    char_md.char_props.write = 1;

    ble_gatts_attr_md_t cccd_md;
    memset(&cccd_md, 0, sizeof(cccd_md));
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
    char_md.p_cccd_md           = &cccd_md;
    char_md.char_props.notify   = 1;

    ble_gatts_attr_md_t attr_md;
    memset(&attr_md, 0, sizeof(attr_md));
    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 
//    attr_md.vlen        = 1;      //IF you want to set variabl char length

    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);

    ble_gatts_attr_t    attr_char_value;
    memset(&attr_char_value, 0, sizeof(attr_char_value));    
    attr_char_value.p_uuid      = &char_uuid;
    attr_char_value.p_attr_md   = &attr_md;

    attr_char_value.max_len     = 4;
    attr_char_value.init_len    = 4;
    uint8_t value[4]            = {0x00,0x00,0x00,0x00};
    attr_char_value.p_value     = value;

    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
                                       &char_md,
                                       &attr_char_value,
                                       &p_our_service->char_handles3);
    APP_ERROR_CHECK(err_code);
    return NRF_SUCCESS;
}

//static uint32_t char_flash_temp_add(ble_os_t * p_our_service)
//{
//    uint32_t            err_code;
//    ble_uuid_t          char_uuid;
//    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
//    char_uuid.uuid      = BLE_UUID_FLASH_CHARACTERISTC_UUID;
//    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
//    APP_ERROR_CHECK(err_code); 
//
//    ble_gatts_char_md_t char_md;
//    memset(&char_md, 0, sizeof(char_md));
//    char_md.char_props.read = 1;
////    char_md.char_props.write = 1;
//
//    ble_gatts_attr_md_t cccd_md;
//    memset(&cccd_md, 0, sizeof(cccd_md));
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
//    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
//    char_md.p_cccd_md           = &cccd_md;
//    char_md.char_props.notify   = 1;
//
//    ble_gatts_attr_md_t attr_md;
//    memset(&attr_md, 0, sizeof(attr_md));
//    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 
//
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
////    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
//
//    ble_gatts_attr_t    attr_char_value;
//    memset(&attr_char_value, 0, sizeof(attr_char_value));    
//    attr_char_value.p_uuid      = &char_uuid;
//    attr_char_value.p_attr_md   = &attr_md;
//
//    attr_char_value.max_len     = 1;
//    attr_char_value.init_len    = 1;
//    uint8_t value[1]            = {0};
//    attr_char_value.p_value     = value;
//
//    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
//                                       &char_md,
//                                       &attr_char_value,
//                                       &p_our_service->char_handles);
//    APP_ERROR_CHECK(err_code);
//    return NRF_SUCCESS;
//}
//
//static uint32_t char_flash_adc_add(ble_os_t * p_our_service)
//{
//    uint32_t            err_code;
//    ble_uuid_t          char_uuid;
//    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
//
//    char_uuid.uuid     =BLE_UUID_FLASH_CHARACTERISTC_UUID2;
//    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
//    APP_ERROR_CHECK(err_code); 
//
//    ble_gatts_char_md_t char_md;
//
//    memset(&char_md, 0, sizeof(char_md));
//    char_md.char_props.read = 1;
////    char_md.char_props.write = 1;
//
//    ble_gatts_attr_md_t cccd_md;
//    memset(&cccd_md, 0, sizeof(cccd_md));
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
//    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
//    char_md.p_cccd_md           = &cccd_md;
//    char_md.char_props.notify   = 1;
//
//    ble_gatts_attr_md_t attr_md;
//    memset(&attr_md, 0, sizeof(attr_md));
//    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 
//
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
////    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
//
//    ble_gatts_attr_t    attr_char_value;
//    memset(&attr_char_value, 0, sizeof(attr_char_value));    
//    attr_char_value.p_uuid      = &char_uuid;
//    attr_char_value.p_attr_md   = &attr_md;
//
//    attr_char_value.max_len     = 2;
//    attr_char_value.init_len    = 2;
//    uint8_t value[2]            = {0,0};
//    attr_char_value.p_value     = value;
//    
//    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
//                                       &char_md,
//                                       &attr_char_value,
//                                       &p_our_service->char_handles2);
//    APP_ERROR_CHECK(err_code);
//    return NRF_SUCCESS;
//}
//
//static uint32_t char_flash_cnt_add(ble_os_t * p_our_service)
//{
//    uint32_t            err_code;
//    ble_uuid_t          char_uuid;
//    ble_uuid128_t       base_uuid = BLE_UUID_OUR_BASE_UUID;
//    char_uuid.uuid      = BLE_UUID_FLASH_CHARACTERISTC_UUID3;
//    err_code = sd_ble_uuid_vs_add(&base_uuid, &char_uuid.type);
//    APP_ERROR_CHECK(err_code); 
//
//    ble_gatts_char_md_t char_md;
//    memset(&char_md, 0, sizeof(char_md));
//    char_md.char_props.read = 1;
////    char_md.char_props.write = 1;
//
//    ble_gatts_attr_md_t cccd_md;
//    memset(&cccd_md, 0, sizeof(cccd_md));
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.read_perm);
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&cccd_md.write_perm);
//    cccd_md.vloc                = BLE_GATTS_VLOC_STACK;    
//    char_md.p_cccd_md           = &cccd_md;
//    char_md.char_props.notify   = 1;
//
//    ble_gatts_attr_md_t attr_md;
//    memset(&attr_md, 0, sizeof(attr_md));
//    attr_md.vloc        = BLE_GATTS_VLOC_STACK; 
//
//    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.read_perm);
////    BLE_GAP_CONN_SEC_MODE_SET_OPEN(&attr_md.write_perm);
//
//    ble_gatts_attr_t    attr_char_value;
//    memset(&attr_char_value, 0, sizeof(attr_char_value));    
//    attr_char_value.p_uuid      = &char_uuid;
//    attr_char_value.p_attr_md   = &attr_md;
//
//    attr_char_value.max_len     = 1;
//    attr_char_value.init_len    = 1;
//    uint8_t value[1]            = {0};
//    attr_char_value.p_value     = value;
//
//    err_code = sd_ble_gatts_characteristic_add(p_our_service->service_handle,
//                                       &char_md,
//                                       &attr_char_value,
//                                       &p_our_service->char_handles3);
//    APP_ERROR_CHECK(err_code);
//    return NRF_SUCCESS;
//}


/**@brief Function for initiating our new service.
 *
 * @param[in]   p_our_service        Our Service structure.
 *
 */
void c_service_init(ble_os_t * p_our_service, ble_os_init_t * p_srv_init)
{
    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions

    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
    ble_uuid_t        service_uuid;
    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
    service_uuid.uuid = BLE_UUID_OUR_SERVICE_UUID;
    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
    APP_ERROR_CHECK(err_code);    
    
    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
    p_our_service->event_handler = p_srv_init->event_handler;

    // FROM_SERVICE_TUTORIAL: Add our service
		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
                                        &service_uuid,
                                        &p_our_service->service_handle);
    
    APP_ERROR_CHECK(err_code);
    
    char_temp_add(p_our_service);
    char_adc_add(p_our_service);
    char_interval_add(p_our_service);
}


//
//void c_flash_serv_init(ble_os_t * p_our_service)
//{
//    uint32_t   err_code; // Variable to hold return codes from library and softdevice functions
//
//    // FROM_SERVICE_TUTORIAL: Declare 16-bit service and 128-bit base UUIDs and add them to the BLE stack
//    ble_uuid_t        service_uuid;
//    ble_uuid128_t     base_uuid = BLE_UUID_OUR_BASE_UUID;
//    service_uuid.uuid = BLE_UUID_FLS_SERVICE_UUID;
//    err_code = sd_ble_uuid_vs_add(&base_uuid, &service_uuid.type);
//    APP_ERROR_CHECK(err_code);    
//    
//    p_our_service->conn_handle = BLE_CONN_HANDLE_INVALID;
////    p_our_service->event_handler = p_srv_init->event_handler;
//
//    // FROM_SERVICE_TUTORIAL: Add our service
//		err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,
//                                        &service_uuid,
//                                        &p_our_service->service_handle);
//    
//    APP_ERROR_CHECK(err_code);
//    
//    char_flash_temp_add(p_our_service);
//    char_flash_adc_add(p_our_service);
//    char_flash_cnt_add(p_our_service);
//}




void temperature_characteristic_update(ble_os_t *p_our_service, int32_t *temperature_value)
{
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = 1;
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)temperature_value;
        
        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
    }


}


void saadc_characteristic_update(ble_os_t *p_our_service, int16_t *adc_value)
{
    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
    {
        uint16_t               len = 2;
        ble_gatts_hvx_params_t hvx_params;
        memset(&hvx_params, 0, sizeof(hvx_params));

        hvx_params.handle = p_our_service->char_handles2.value_handle;
        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
        hvx_params.offset = 0;
        hvx_params.p_len  = &len;
        hvx_params.p_data = (uint8_t*)adc_value;  

        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
    }


}

//void flash_cnt_char_update(ble_os_t *p_our_service, uint32_t *cnt_val)
//{
//    if (p_our_service->conn_handle != BLE_CONN_HANDLE_INVALID)
//    {
//        uint16_t               len = 1;
//        ble_gatts_hvx_params_t hvx_params;
//        memset(&hvx_params, 0, sizeof(hvx_params));
//
//        hvx_params.handle = p_our_service->char_handles3.value_handle;
//        hvx_params.type   = BLE_GATT_HVX_NOTIFICATION;
//        hvx_params.offset = 0;
//        hvx_params.p_len  = &len;
//        hvx_params.p_data = (uint8_t*)cnt_val;
//        
//        sd_ble_gatts_hvx(p_our_service->conn_handle, &hvx_params);
//    }
//
//
//}

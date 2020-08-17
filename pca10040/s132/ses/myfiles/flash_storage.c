#include "flash_storage.h"
#include "app_error.h"
#include "nrf.h"
#include "nrf_soc.h"
#include "nordic_common.h"
#include "nrf_fstorage_sd.h"
#include "nrf_fstorage.h"
#include "nrf_sdh.h"

#define FS_LOG_ENABLED                  1

#define NRF_LOG_MODULE_NAME FS
#if FS_LOG_ENABLED 
#define NRF_LOG_LEVEL 3
#define NRF_LOG_INFO_COLOR 2

#else 
#define NRF_LOG_LEVEL 0
#endif 
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
NRF_LOG_MODULE_REGISTER();

extern uint32_t m_logIdx;
//uint32_t m_logIdx;

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt);

NRF_FSTORAGE_DEF(nrf_fstorage_t fstorage) =
{
    /* Set a handler for fstorage events. */
    .evt_handler = fstorage_evt_handler,

    /* These below are the boundaries of the flash space assigned to this instance of fstorage.
     * You must set these manually, even at runtime, before nrf_fstorage_init() is called.
     * The function nrf5_flash_end_addr_get() can be used to retrieve the last address on the
     * last page of flash available to write data. */
//    .start_addr = 0x00040000,
    .start_addr = 0x4e000,
//    .end_addr   = 0x000F0000,
    .end_addr   = 0x4ffff,
    // Reminder: A page is 0x1000 bytes (4096 bytes)
};

void wait_for_flash_ready(nrf_fstorage_t const * p_fstorage)
{
    /* While fstorage is busy, sleep and wait for an event. */
    while (nrf_fstorage_is_busy(p_fstorage))
    {
        sd_app_evt_wait();
    }
}

static void fstorage_evt_handler(nrf_fstorage_evt_t * p_evt)
{
    if (p_evt->result != NRF_SUCCESS)
    {
        NRF_LOG_INFO("--> Event received: ERROR while executing an fstorage operation. %d", p_evt->result);
        return;
    }

    switch (p_evt->id)
    {
        case NRF_FSTORAGE_EVT_WRITE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: wrote %d bytes at address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        case NRF_FSTORAGE_EVT_ERASE_RESULT:
        {
            NRF_LOG_INFO("--> Event received: erased %d page from address 0x%x.",
                         p_evt->len, p_evt->addr);
        } break;

        default:
            break;
    }
}

void FS_Write(uint32_t addr, uint32_t * p_data, uint32_t len)
{
    if(len % 4)
    {
        NRF_LOG_WARNING("Data not 4Byte aligned");
        return;
    }

    ret_code_t rc = nrf_fstorage_write(&fstorage, addr, p_data, len, NULL);
    APP_ERROR_CHECK(rc);

    wait_for_flash_ready(&fstorage);
}

void FS_Read(uint32_t addr, uint8_t * data, uint32_t len)
{
    ret_code_t rc = nrf_fstorage_read(&fstorage, addr, data, len);
    APP_ERROR_CHECK(rc);

    wait_for_flash_ready(&fstorage);
}

void FS_Erase(uint32_t startAddr, uint32_t pages_cnt)
{
    ret_code_t rc = nrf_fstorage_erase(&fstorage, startAddr, pages_cnt, NULL);
    APP_ERROR_CHECK(rc);

    wait_for_flash_ready(&fstorage);
}

// Erase all contents of flash storage
void FS_Format(void)
{
    FS_Erase(FS_START_ADDR, FS_PAGE_COUNT);
}

void FS_Init(void)
{
    ret_code_t rc;
    nrf_fstorage_api_t * p_fs_api;

    p_fs_api = &nrf_fstorage_sd;

    rc = nrf_fstorage_init(&fstorage, p_fs_api, NULL);
    APP_ERROR_CHECK(rc);

#if FS_FORMAT
    FS_Format();
#endif

    uint32_t * pAddr;
    // Search for next empty slot
    for(uint32_t k = FS_START_ADDR; k < FS_END_ADDR; k += sizeof(dataEntry_t))
    {      
        pAddr = (uint32_t *)k;

        if(*pAddr == 0xFFFFFFFF)
        {
            m_logIdx = (k - FS_START_ADDR)/sizeof(dataEntry_t);
            NRF_LOG_INFO("Available slot at address [%x]. Next log index set to [%d]", k, m_logIdx);
            return;
        }
#if FS_PRINT_LOGS
        logEntry_t * pLog = (logEntry_t *)pAddr;
        NRF_LOG_INFO("Time[%d], RSSI[%d]", pLog->timestamp, pLog->rssi);
        NRF_LOG_INFO("Mac[%x:%x:%x:%x:%x:%x]\n", pLog->devAddr[0], pLog->devAddr[1], pLog->devAddr[2],   
                                               pLog->devAddr[3], pLog->devAddr[4], pLog->devAddr[5]);
        while(NRF_LOG_PROCESS() == true);
#endif
    }

    NRF_LOG_ERROR("Couldn't find empty slot, memory full");
}
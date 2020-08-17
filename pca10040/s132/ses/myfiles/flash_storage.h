#ifndef _FLASH_STORAGE_H__
#define _FLASH_STORAGE_H__

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define FS_PAGE_SIZE                4096
//#define FS_START_ADDR               0x00040000
#define FS_START_ADDR               0x4e000
//#define FS_END_ADDR                 0x000F0000
#define FS_END_ADDR                 0x4ffff

#define FS_SIZE                     (FS_END_ADDR - FS_START_ADDR)
#define FS_PAGE_COUNT               (FS_SIZE/FS_PAGE_SIZE)

typedef struct
{
    uint32_t timestamp;
    uint8_t devAddr[6];
    int8_t rssi;
    uint8_t reserved;
} logEntry_t;


typedef struct
{
    uint32_t  timestamp;
    uint16_t  adc;
    uint8_t   temperature;
    uint8_t   index;
}dataEntry_t;

#define FS_MAX_ENTRY_COUNT          (FS_SIZE/sizeof(logEntry_t))

void FS_Init(void);
void FS_Format(void);
void FS_Write(uint32_t addr, uint32_t * p_data, uint32_t len);
void FS_Erase(uint32_t startAddr, uint32_t pages_cnt);

#endif
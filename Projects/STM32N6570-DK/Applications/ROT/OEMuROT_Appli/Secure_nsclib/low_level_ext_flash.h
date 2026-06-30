/**
  ******************************************************************************
  * @file    low_level_ext_flash.h
  * @author  MCD Application Team
  * @brief   This file contains device definition for low_level_flash_ext driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
#ifndef __LOW_LEVEL_EXT_FLASH_H
#define __LOW_LEVEL_EXT_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32n6xx_hal.h"

/* General return codes */
#define ARM_DRIVER_OK                 0 ///< Operation succeeded
#define ARM_DRIVER_ERROR             -1 ///< Unspecified error
#define ARM_DRIVER_ERROR_UNSUPPORTED -4 ///< Operation not supported
#define ARM_DRIVER_ERROR_PARAMETER   -5 ///< Parameter error

struct ext_flash_range
{
  uint32_t base;
  uint32_t limit;
};
struct ext_flash_vect
{
  uint32_t nb;
  struct ext_flash_range *range;
};
struct low_level_ext_flash_device
{
  struct ext_flash_vect erase;
  struct ext_flash_vect write;
};

/**
\brief Driver Version
*/
typedef struct _ARM_DRIVER_VERSION {
  uint16_t api;                         ///< API version
  uint16_t drv;                         ///< Driver version
} ARM_DRIVER_VERSION;

/**
\brief General power states
*/
typedef enum _ARM_POWER_STATE {
  ARM_POWER_OFF,                        ///< Power off: no operation possible
  ARM_POWER_LOW,                        ///< Low Power mode: retain state, detect and signal wake-up events
  ARM_POWER_FULL                        ///< Power on: full operation at maximum performance
} ARM_POWER_STATE;

/**
\brief Flash Sector information
*/
typedef struct _ARM_FLASH_SECTOR {
  uint32_t start;                       ///< Sector Start address
  uint32_t end;                         ///< Sector End address (start+size-1)
} const ARM_FLASH_SECTOR;

/**
\brief Flash information
*/
typedef struct _ARM_FLASH_INFO {
  ARM_FLASH_SECTOR *sector_info;        ///< Sector layout information (NULL=Uniform sectors)
  uint32_t          sector_count;       ///< Number of sectors
  uint32_t          sector_size;        ///< Uniform sector size in bytes (0=sector_info used)
  uint32_t          page_size;          ///< Optimal programming page size in bytes
  uint32_t          program_unit;       ///< Smallest programmable unit in bytes
  uint8_t           erased_value;       ///< Contents of erased memory (usually 0xFF)
} ARM_FLASH_INFO;

/**
\brief Flash Status
*/
typedef volatile struct _ARM_FLASH_STATUS {
  uint32_t busy     : 1;                ///< Flash busy flag
  uint32_t error    : 1;                ///< Read/Program/Erase error flag (cleared on start of next operation)
  uint32_t reserved : 30;
} ARM_FLASH_STATUS;

typedef void (*ARM_Flash_SignalEvent_t) (uint32_t event);    ///< Pointer to \ref ARM_Flash_SignalEvent : Signal Flash Event.

/**
\brief Flash Driver Capabilities.
*/
typedef struct _ARM_FLASH_CAPABILITIES {
  uint32_t event_ready  : 1;            ///< Signal Flash Ready event
  uint32_t data_width   : 2;            ///< Data width: 0=8-bit, 1=16-bit, 2=32-bit
  uint32_t erase_chip   : 1;            ///< Supports EraseChip operation
  uint32_t reserved     : 28;           ///< Reserved (must be zero)
} ARM_FLASH_CAPABILITIES;

/**
\brief Access structure of the Flash Driver
*/
typedef struct _ARM_DRIVER_FLASH {
  ARM_DRIVER_VERSION     (*GetVersion)     (void);                                          ///< Pointer to \ref ARM_Flash_GetVersion : Get driver version.
  ARM_FLASH_CAPABILITIES (*GetCapabilities)(void);                                          ///< Pointer to \ref ARM_Flash_GetCapabilities : Get driver capabilities.
  int32_t                (*Initialize)     (ARM_Flash_SignalEvent_t cb_event);              ///< Pointer to \ref ARM_Flash_Initialize : Initialize Flash Interface.
  int32_t                (*Uninitialize)   (void);                                          ///< Pointer to \ref ARM_Flash_Uninitialize : De-initialize Flash Interface.
  int32_t                (*PowerControl)   (ARM_POWER_STATE state);                         ///< Pointer to \ref ARM_Flash_PowerControl : Control Flash Interface Power.
  int32_t                (*ReadData)       (uint32_t addr,       void *data, uint32_t cnt); ///< Pointer to \ref ARM_Flash_ReadData : Read data from Flash.
  int32_t                (*ProgramData)    (uint32_t addr, const void *data, uint32_t cnt); ///< Pointer to \ref ARM_Flash_ProgramData : Program data to Flash.
  int32_t                (*EraseSector)    (uint32_t addr);                                 ///< Pointer to \ref ARM_Flash_EraseSector : Erase Flash Sector.
  int32_t                (*EraseChip)      (void);                                          ///< Pointer to \ref ARM_Flash_EraseChip : Erase complete Flash.
  ARM_FLASH_STATUS       (*GetStatus)      (void);                                          ///< Pointer to \ref ARM_Flash_GetStatus : Get Flash status.
  ARM_FLASH_INFO *       (*GetInfo)        (void);                                          ///< Pointer to \ref ARM_Flash_GetInfo : Get Flash information.
} const ARM_DRIVER_FLASH;

extern struct low_level_ext_flash_device EXT_FLASH0_DEV;

void EXTMEM_MemCopy(uint32_t *destination_Address, const uint8_t *ptrData, uint32_t DataSize);

#ifdef __cplusplus
}
#endif

#endif /* __LOW_LEVEL_EXT_FLASH_H */

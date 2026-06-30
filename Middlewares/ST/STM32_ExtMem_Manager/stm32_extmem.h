/**
  ******************************************************************************
  * @file    stm32_extmem.h
  * @author  MCD Application Team
  * @brief   This file contains the external memory functions prototypes.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_EXTMEM_H_
#define __STM32_EXTMEM_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @addtogroup EXTMEM
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported constants --------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup EXTMEM_Exported_Types External Memory Exported Types
  * @{
  */

/**
  * @brief Middleware version number
  */
#define EXTMEM_VERSION 0x00010600 /* Version v1.6.0 */

/**
  * @brief Types of supported memories
  */
typedef enum
{
  EXTMEM_NOR_SFDP,
  EXTMEM_NOR_CFI,
  EXTMEM_PSRAM,
  EXTMEM_SDCARD,
  EXTMEM_CUSTOM,
  EXTMEM_USER
} EXTMEM_TypeTypeDef;

/**
  * @brief EXTMEM module function call return values
  */
typedef enum
{
  EXTMEM_OK,                       /*!< Action performed with a status ok */
  EXTMEM_ERROR_NOTSUPPORTED  = -1, /*!< Action not supported by the memory type */
  EXTMEM_ERROR_UNKNOWNMEMORY = -2, /*!< Unknown memory type */
  EXTMEM_ERROR_DRIVER        = -3, /*!< Error return by the driver layer */
  EXTMEM_ERROR_SECTOR_SIZE   = -4, /*!< Inconsistency between the size and the sector size of the memory */
  EXTMEM_ERROR_INVALID_ID    = -5, /*!< Invalid memory ID */
  EXTMEM_ERROR_PARAM         = -6, /*!< Parameter value error */
} EXTMEM_StatusTypeDef;

/**
  * @brief Enable/disable state of the module
  */
typedef enum
{
  EXTMEM_ENABLE,              /*!< State enable */
  EXTMEM_DISABLE              /*!< State disable */
} EXTMEM_StateTypeDef;

/**
  * @brief NOR SFDP memory information structure
  */
typedef struct
{
  uint8_t  FlashSize;         /*!< Specifies the flash size as a power of 2 */
  uint32_t PageSize;          /*!< Page size in bytes */
  uint32_t EraseType1Size;    /*!< Erase Type 1 size */
  uint32_t EraseType2Size;    /*!< Erase Type 2 size */
  uint32_t EraseType3Size;    /*!< Erase Type 3 size */
  uint32_t EraseType4Size;    /*!< Erase Type 4 size */
} EXTMEM_NOR_SFDP_FlashInfoTypeDef;


/**
  * @brief USER memory information structure
  */
typedef struct
{
  uint8_t  MemSize;           /*!< Specifies the memory size as a power of 2 */
  uint32_t MpuCache;          /*!< MPU Cache */
}  EXTMEM_USER_MemInfoTypeDef;

/**
  * @brief SD card information structure
  */
typedef struct
{
  uint32_t CardType;        /*!< Specifies the card type                         */
  uint32_t CardVersion;     /*!< Specifies the card version                      */
  uint32_t Class;           /*!< Specifies the card class                        */
  uint32_t RelCardAdd;      /*!< Specifies the relative card address             */
  uint32_t BlockNbr;        /*!< Specifies the card capacity in blocks           */
  uint32_t BlockSize;       /*!< Specifies the block size in bytes               */
  uint32_t LogBlockNbr;     /*!< Specifies the card logical capacity in blocks   */
  uint32_t LogBlockSize;    /*!< Specifies the logical block size in bytes       */
  uint32_t CardSpeed;       /*!< Specifies the card speed                        */
} EXTMEM_DRIVER_SDCARD_InfoTypeDef;

/**
  * @brief Number of physical I/O line(s) connected with the memory
  */
typedef enum
{
  EXTMEM_LINK_CONFIG_1LINE,     /*!< Configuration using  1 line  */
  EXTMEM_LINK_CONFIG_2LINES,    /*!< Configuration using  2 lines */
  EXTMEM_LINK_CONFIG_4LINES,    /*!< Configuration using  4 lines */
  EXTMEM_LINK_CONFIG_8LINES,    /*!< Configuration using  8 lines */
  EXTMEM_LINK_CONFIG_16LINES,   /*!< Configuration using 16 lines */
} EXTMEM_LinkConfig_TypeDef;

/**
  * @}
  */

/* Exported functions --------------------------------------------------------*/
/** @addtogroup EXTMEM_Exported_Functions External Memory Exported Functions
  * @{
  */

EXTMEM_StatusTypeDef EXTMEM_Init(uint32_t MemId, uint32_t ClockInput);
EXTMEM_StatusTypeDef EXTMEM_DeInit(uint32_t MemId);
EXTMEM_StatusTypeDef EXTMEM_Read(uint32_t MemId, uint32_t Address, uint8_t *Data, uint32_t Size);
EXTMEM_StatusTypeDef EXTMEM_Write(uint32_t MemId, uint32_t Address, const uint8_t *Data, uint32_t Size);
EXTMEM_StatusTypeDef EXTMEM_WriteInMappedMode(uint32_t MemId, uint32_t Address, const uint8_t *const Data,
                                              uint32_t Size);
EXTMEM_StatusTypeDef EXTMEM_EraseSector(uint32_t MemId, uint32_t Address, uint32_t Size);
EXTMEM_StatusTypeDef EXTMEM_EraseAll(uint32_t MemId);
EXTMEM_StatusTypeDef EXTMEM_GetInfo(uint32_t MemId, void *Info);
EXTMEM_StatusTypeDef EXTMEM_MemoryMappedMode(uint32_t MemId, EXTMEM_StateTypeDef State);
EXTMEM_StatusTypeDef EXTMEM_GetMapAddress(uint32_t MemId, uint32_t *BaseAddress);

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_H_ */

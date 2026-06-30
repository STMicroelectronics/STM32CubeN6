/**
  ******************************************************************************
  * @file    stm32_custom_driver_api.h
  * @author  MCD Application Team
  * @brief   This file contains custom driver API definitions.
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
#ifndef __STM32_CUSTOM_DRIVER_H
#define __STM32_CUSTOM_DRIVER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#if EXTMEM_DRIVER_CUSTOM == 1
#include "stm32_custom_driver_type.h"

/** @addtogroup CUSTOM
  * @ingroup EXTMEM_DRIVER
  * @{
  */
/* Exported constants --------------------------------------------------------*/

/* Default EMM Custom Driver timeout: 500ms */
#define CUSTOM_XSPI_TIMEOUT_VALUE   (500)

/* Exported types ------------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Types Exported Types
  * @{
  */

/* Exported functions --------------------------------------------------------*/
/**
  * @addtogroup CUSTOM_Exported_Functions Exported functions
  * @{
  */

EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Init(XSPI_HandleTypeDef *hxspi,  uint32_t ClockInput,
                                                             EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_DeInit(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Read(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                             uint32_t Address, uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Write(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                              uint32_t Address, const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_WriteInMappedMode(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef
                                                                          *CustomObject, uint32_t Address,
                                                                          const uint8_t *Data, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_EraseSector(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                    uint32_t Address, uint32_t Size);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_MassErase(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode(
  EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode(
  EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetMapAddress(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                      uint32_t *BaseAddress);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_GetInfo(EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *CustomObject,
                                                                EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef *MemInfo);
__weak void EXTMEM_DRIVER_CUSTOM_MemCopy(uint32_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize);
EXTMEM_DRIVER_CUSTOM_StatusTypeDef EXTMEM_DRIVER_CUSTOM_UserCallback(void *CustomObjectCtx, uint32_t UserArg0,
                                                                     uint32_t UserArg1);

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_CUSTOM_DRIVER_H */

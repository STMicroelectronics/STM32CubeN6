/**
  ******************************************************************************
  * @file    Secure_nsclib/secure_nsc.h
  * @author  GPM Application Team
  * @brief   Header for secure non-secure callable APIs list
  ******************************************************************************
  * @attention
  *
  * COPYRIGHT 2024 STMicroelectronics
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef SECURE_NSC_H
#define SECURE_NSC_H

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "low_level_ext_flash.h"
/* Exported types ------------------------------------------------------------*/
/**
  * @brief  non-secure callback ID enumeration definition
  */
typedef enum
{
  SECURE_FAULT_CB_ID     = 0x00U, /*!< System secure fault callback ID */
  IAC_ERROR_CB_ID        = 0x01U, /*!< Illegal access secure error callback ID */
} SECURE_CallbackIDTypeDef;

/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SECURE_RegisterCallback(SECURE_CallbackIDTypeDef CallbackId, void *func);

void SECURE_GetInfo(ARM_FLASH_INFO* data);
int32_t SECURE_EraseSector(uint32_t addr);
int32_t SECURE_ProgramData(uint32_t addr, const void *data, uint32_t cnt);

#endif /* SECURE_NSC_H */

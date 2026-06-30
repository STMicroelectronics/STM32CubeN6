/**
  ******************************************************************************
  * @file    stm32_extmem_conf.h
  * @author  MCD Application Team
  * @brief   Header configuration for extmem module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef EXTMEM_CONF_H_
#define EXTMEM_CONF_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EXTMEN_CONF
  * @{
  */

/** @defgroup EXTMEN_CONF_Driver_selection External Memory configuration selection of the driver
  * this section is used to select the driver\n
  *              #define EXTMEM_DRIVER_NOR_SFDP   1\n
  *              #define EXTMEM_DRIVER_PSRAM      1\n
  *              #define EXTMEM_DRIVER_SDCARD     1\n
  * @{
  */
#define EXTMEM_DRIVER_NOR_SFDP   1
#define EXTMEM_DRIVER_PSRAM      0
#define EXTMEM_DRIVER_SDCARD     0

/**
  * @}
  */

/** @defgroup EXTMEN_CONF_SAL_selection External Memory configuration selection of the SAL
  * this section is used to select the SAL\n
  *              #define EXTMEM_SAL_XSPI   1\n
  *              #define EXTMEM_SAL_SD     1\n
  * @{
  */

#define EXTMEM_SAL_XSPI      1
/**
  * @}
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32n6xx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"

/** @defgroup EXTMEN_CONF_SAL_imported_variable External Memory configuration list of the imported variables
  * this section is used to import the HAL handle variable.
  * @{
  */

/*
  @brief import of the HAL handles for MEMORRY_SERIAL_0
*/
extern XSPI_HandleTypeDef       XSPI_HANDLE;

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** @defgroup EXTMEM_CONF_Exported_constants EXTMEM_CONF exported constants
  * @{
  */
enum
{
  MEMORY_SERIAL_0  = 0, /*!< ID 0 of extmem_list_config */
  MEMORY_PSRAM_0   = 1, /*!< ID 1 of extmem_list_config */
};
/**
  * @}
  */

/* Exported configuration --------------------------------------------------------*/
/** @defgroup EXTMEM_CONF_Exported_configuration EXTMEM_CONF exported configuration definition
  * @{
  */
#if defined(EXTMEM_C)
extern EXTMEM_DefinitionTypeDef extmem_list_config[];
EXTMEM_DefinitionTypeDef extmem_list_config[] =
{
  /* MEMORY_SERIAL_0 */
  {
    .MemType = EXTMEM_NOR_SFDP,
    .Handle = (void *) &XSPI_HANDLE,
    .ConfigType = EXTMEM_LINK_CONFIG_8LINES,
    .NorSfdpObject =
    {
      {0},
    }
  }
};
#endif /* EXTMEM_C */
/**
  * @}
  */

/* Exported trace --------------------------------------------------------*/
/** @defgroup EXTMEM_CONF_Exported_debug EXTMEM_CONF exported debug definition
  * @{
  */

/*
 * @brief import of the trace function
 */
/* extern void EXTMEM_TRACE(uint8_t *Message); */

/*
 * @brief definition of the debug macro
 */
/* #define EXTMEM_MACRO_DEBUG(_MSG_)  EXTMEM_TRACE((uint8_t *)_MSG_) */

/*
 * @brief debug level of the different layers
 */
#define EXTMEM_DEBUG_LEVEL               1

#define EXTMEM_DRIVER_SFDP_DEBUG_LEVEL   1
#define EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL  1

#define EXTMEM_SAL_XSPI_DEBUG_LEVEL      1
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* EXTMEM_CONF_H_ */

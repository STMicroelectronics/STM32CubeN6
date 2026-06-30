/**
  ******************************************************************************
  * @file    stm32_extmem_conf_template.h
  * @author  MCD Application Team
  * @brief   Configuration header for the EXTMEM module
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
#ifndef __STM32_EXTMEM_CONF__H__
#define __STM32_EXTMEM_CONF__H__

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup EXTMEM_CONF
  * @{
  */

/** @defgroup EXTMEM_CONF_Driver_selection External Memory configuration selection of the driver.
  * This section is used to select the driver\n
  *              #define EXTMEM_DRIVER_NOR_SFDP   1\n
  *              #define EXTMEM_DRIVER_PSRAM      1\n
  *              #define EXTMEM_DRIVER_SDCARD     0\n
  *              #define EXTMEM_DRIVER_USER       0
  *              #define EXTMEM_DRIVER_CUSTOM     0
  * @{
  */
#define EXTMEM_DRIVER_NOR_SFDP   1
#define EXTMEM_DRIVER_PSRAM      1
#define EXTMEM_DRIVER_SDCARD     0
#define EXTMEM_DRIVER_USER       0
#define EXTMEM_DRIVER_CUSTOM     0

/* At least one of the above driver type defines should be set to 1 for proper use of EMM Middleware */
#if (!(defined(EXTMEM_DRIVER_NOR_SFDP) && (EXTMEM_DRIVER_NOR_SFDP == 1))) \
  &&(!(defined(EXTMEM_DRIVER_PSRAM)    && (EXTMEM_DRIVER_PSRAM == 1)))    \
  &&(!(defined(EXTMEM_DRIVER_SDCARD)   && (EXTMEM_DRIVER_SDCARD == 1)))   \
  &&(!(defined(EXTMEM_DRIVER_USER)     && (EXTMEM_DRIVER_USER == 1)))     \
  &&(!(defined(EXTMEM_DRIVER_CUSTOM)   && (EXTMEM_DRIVER_CUSTOM == 1)))
#warning "Please enable at least one EMM driver type"
#endif /* Check on EMM driver type enabled defines */

/**
  * @}
  */

/** @defgroup EXTMEM_CONF_SAL_selection External Memory configuration selection of the SAL
  * This section is used to select the software adaptation layer (SAL)\n
  *              #define EXTMEM_SAL_XSPI          1\n
  *              #define EXTMEM_SAL_SD            0
  * @{
  */
#define EXTMEM_SAL_XSPI      1
#define EXTMEM_SAL_SD        0

/**
  * @}
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32h7rsxx_hal.h"
#include "stm32_extmem.h"
#include "stm32_extmem_type.h"

/** @defgroup EXTMEM_CONF_SAL_imported_variable External Memory configuration list of the imported variables
  * This section imports the HAL handle variable. The handle can be one of the following:
  *             extern XSPI_HandleTypeDef \n
  *             extern SD_HandleTypeDef
  * @{
  */

/*
  @brief Import of the HAL handle used for EXTMEMORY_2
*/
extern XSPI_HandleTypeDef       hxspi2;

/*
  @brief Import of the HAL handle used for EXTMEMORY_1
*/
extern XSPI_HandleTypeDef       hxspi1;
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
  EXTMEMORY_1  = 0,  /*!< ID=0 for the first external memory  */
  EXTMEMORY_2  = 1,  /*!< ID=1 for the second external memory */
};

/*
  @brief Management of the external memory used as boot layer
*/
#define EXTMEM_MEMORY_BOOTXIP  EXTMEMORY_1
/**
  * @}
  */

/*
  @brief Default value for the maximum clock frequency supplied to the memory
*/
#define EXTMEM_DEFAULT_MAX_CLOCK_FREQ  50000000U

/*
  @brief Enable dual-memory WIP check in NOR SFDP busy polling (Dual-Quad / Dual-Octal).
  @note Uncomment to enable dual configuration behavior.
*/
/* #define EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG */

/*
  @brief Option for skipping loading of the Appli from the external Flash in the load and run mode (debug purpose).
  @note This option is to be used when the external Flash is pre-loaded by the debugger and the copy
        from external Flash to internal memory is not needed.
  @note Uncomment to skip loading of the Appli from the external Flash.
*/
/* #define EXTMEM_DEBUG_NO_LOAD_FROM_EXT_FLASH */

/**
  * @}
  */

/* Exported configuration --------------------------------------------------------*/

/** @defgroup EXTMEM_CONF_Exported_configuration EXTMEM_CONF exported configuration definition
  * Memory type can be EXTMEM_SDCARD, EXTMEM_NOR_SFDP, EXTMEM_PSRAM
  * @{
  */
extern EXTMEM_DefinitionTypeDef extmem_list_config[2];
#if defined(EXTMEM_C)
EXTMEM_DefinitionTypeDef extmem_list_config[2] =
{
#if defined(EXTMEM_DRIVER_NOR_SFDP) && (EXTMEM_DRIVER_NOR_SFDP == 1)
  /* EXTMEMORY_1 */
  {
    .MemType = EXTMEM_NOR_SFDP,
    .Handle = (void *) &hxspi2,
    .ConfigType = EXTMEM_LINK_CONFIG_8LINES,
    .NorSfdpObject =
    {
      .sfdp_public.MaxFreq = EXTMEM_DEFAULT_MAX_CLOCK_FREQ,
    }
  },
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if defined(EXTMEM_DRIVER_PSRAM) && (EXTMEM_DRIVER_PSRAM == 1)
  /* EXTMEMORY_2 */
  {
    .MemType = EXTMEM_PSRAM,
    .Handle = (void *) &hxspi1,
    .ConfigType = EXTMEM_LINK_CONFIG_16LINES,
    .PsramObject =
    {
      .psram_public =
      {
        .MemorySize = HAL_XSPI_SIZE_256MB,
        .FreqMax = 200 * 1000000u,
        .NumberOfConfig = 1u,
        /* Config */
        {
          {.WriteMask = 0x4000u, .WriteValue = 0x4000u, .REGAddress = 0x08u},
        },
        /* Memory command configuration */
        .ReadREG           = 0x40u,
        .WriteREG          = 0xC0u,
        .ReadREGSize       = 2u,
        .REG_DummyCycle    = 4u,
        .Write_command     = 0xA0u,
        .Write_DummyCycle  = 4u,
        .Read_command      = 0x20u,
        .WrapRead_command  = 0x00u,
        .Read_DummyCycle   = 4u,
      }
    }
  }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
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
 * @brief Import of the trace function
 */
extern void EXTMEM_TRACE(uint8_t *Message);
/*
 * @brief Definition of the debug macro
 */
#define EXTMEM_MACRO_DEBUG(_MSG_)  EXTMEM_TRACE((uint8_t *)_MSG_)

/*
 * @brief Debug level of the different layers
 */
#define EXTMEM_DEBUG_LEVEL                   0

#define EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL   0
#define EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL      0

#define EXTMEM_SAL_XSPI_DEBUG_LEVEL          0
/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_EXTMEM_CONF__H__ */

/**
  ******************************************************************************
  * @file    stm32_w956d8mb.h
  * @author  MCD Application Team
  * @brief   This file includes a driver for custom support of
  *          Windbond W956D8MB Nor Flash memory
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_W956D8MB__H__
#define __STM32_W956D8MB__H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if defined(EXTMEM_DRIVER_CUSTOM) && (EXTMEM_DRIVER_CUSTOM == 1)
#include "stm32_custom_driver_api.h"
#include "stm32_custom_driver_type.h"

/** @defgroup CUSTOM CUSTOM driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup CUSTOM_Private_Macro Private Macro
  * @{
  */
/* CMD1: Command use to change Drive strenght from default (34 ohms) to 22 ohms. Depending on board impedence*/
#define EXTMEM_W956D8MB_CMD1_ADDR_CFGR           0x1000    /*!< Address of the register */
#define EXTMEM_W956D8MB_CMD1_VALUE_CFGR          0x6000    /*!< Value of the register [14-12] = 110b (22 ohms) */
#define EXTMEM_W956D8MB_CMD1_MASK_CFGR           0x7000    /*!< Mask for the register [14-12] */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_W956D8MB_INIT { \
                               .MemType                                  = EXTMEM_CUSTOM_HYPERRAM,               /*!< Common field for memory type        */ \
                               .MemStandard                              = EXTMEM_CUSTOM_STD_HYPERBUS,            /*!< e.g., Macronix, Micron, etc.        */ \
                               .MemSize                                  = EXTMEM_CUSTOM_SIZE_256MB,              /*!< Common field for memory Size        */ \
                               .MemChipSelectHighTimeCycle               = 1,                                     /*!< Common field for memory Chip select high time */ \
                               .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in Hz*/ \
                               .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_8D_8D_8D,                /*!< Access mode (e.g., 1S4D4D) */ \
                               .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                               .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_ENABLE,              /*!< Data strobe mode */ \
                               .NbRegisterConfig                         = 2,                                     /*!< Common field for memory Chip select */ \
                               .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_HB_RWR_REG_ADDR, /*!< Config step to read/write/read a register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_W956D8MB_CMD1_ADDR_CFGR,    /*!< Address of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_W956D8MB_CMD1_VALUE_CFGR,   /*!< Value of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_W956D8MB_CMD1_MASK_CFGR,    /*!< Mask for the register */ \
                               .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,/*!< Config step to use new Optional configuration */ \
                               .OptionalConfig.Frequency                              = 200000000,                         /*!< Operating frequency in Hz*/ \
                               .HyperRamConfig.HyperbusConfig.RwRecoveryTimeCycle     = 7,                                 /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.HyperbusConfig.AccessTimeCycle         = 7,                                 /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.HyperbusConfig.WriteZeroLatency        = EXTMEM_CUSTOM_WR_LATENCY_ON,       /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.HyperbusConfig.LatencyMode             = EXTMEM_CUSTOM_FIXED_LATENCY,       /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.RamConfig.WrapSize                     = EXTMEM_CUSTOM_WRAP_NOT_SUPPORTED,  /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.RamConfig.CsBoundarySize               = EXTMEM_CUSTOM_CS_BONDARYOF_NONE,   /*!< Config step to use new Optional configuration */ \
                               .HyperRamConfig.RamConfig.Refresh                      = 0,                                 /*!< Config step to use new Optional configuration */ \
                             }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w956d8mb = EXTMEM_W956D8MB_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w956d8mb;
#endif /* EXTMEM_C */

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

#endif /* __STM32_W956D8MB__H__ */

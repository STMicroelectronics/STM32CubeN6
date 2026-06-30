/**
  ******************************************************************************
  * @file    stm32_is66wvh8m8.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the ISSI IS66WVH8M8 HyperRAM memory,
  *          using EMM Custom Driver
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
#ifndef __STM32_IS66WVH8M8__H__
#define __STM32_IS66WVH8M8__H__

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
/* CMD1: Command use to change Drive Strength from default to 19 Ohms in CFR0 */
#define EXTMEM_IS66WVH8M8_CMD1_ADDR_CFGR           0x1000    /*!< Address of the register */
#define EXTMEM_IS66WVH8M8_CMD1_VALUE_CFGR          0x7000    /*!< Value of the register [14-12] = 110b (19 ohms) */
#define EXTMEM_IS66WVH8M8_CMD1_MASK_CFGR           0x7000    /*!< Mask for the register [14-12] */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_IS66WVH8M8_INIT { \
                               .MemType                                               = EXTMEM_CUSTOM_HYPERRAM,                /*!< Common field for memory type        */ \
                               .MemStandard                                           = EXTMEM_CUSTOM_STD_HYPERBUS,            /*!< e.g., Macronix, Micron, etc.        */ \
                               .MemSize                                               = EXTMEM_CUSTOM_SIZE_64MB,               /*!< Common field for memory Size        */ \
                               .MemChipSelectHighTimeCycle                            = 1,                                     /*!< Common field for memory Chip select high time */ \
                               .StartupConfig.Frequency                               = 50000000,                              /*!< Operating frequency in Hz*/ \
                               .StartupConfig.AccessMode                              = EXTMEM_CUSTOM_8D_8D_8D,                /*!< Access mode (e.g., 1S4D4D) */ \
                               .StartupConfig.AddressSize                             = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                               .StartupConfig.DqsMode                                 = EXTMEM_CUSTOM_DQS_ENABLE,              /*!< Data strobe mode */ \
                               .NbRegisterConfig                                      = 2,                                     /*!< Common field for memory Chip select */ \
                               .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_HB_RWR_REG_ADDR, /*!< Config step to read/write/read a register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_IS66WVH8M8_CMD1_ADDR_CFGR,      /*!< Address of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_IS66WVH8M8_CMD1_VALUE_CFGR,     /*!< Value of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_IS66WVH8M8_CMD1_MASK_CFGR,      /*!< Mask for the register */ \
                               .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,    /*!< Config step to use new Optional configuration */ \
                               .OptionalConfig.Frequency                              = 100000000,                             /*!< Operating frequency in Hz */ \
                               .HyperRamConfig.HyperbusConfig.RwRecoveryTimeCycle     = 6,                                     /*!< Read/Write recovery time cycle */ \
                               .HyperRamConfig.HyperbusConfig.AccessTimeCycle         = 6,                                     /*!< Access time cycle */ \
                               .HyperRamConfig.HyperbusConfig.WriteZeroLatency        = EXTMEM_CUSTOM_WR_LATENCY_ON,           /*!< Write zero latency */ \
                               .HyperRamConfig.HyperbusConfig.LatencyMode             = EXTMEM_CUSTOM_FIXED_LATENCY,           /*!< Latency mode */ \
                               .HyperRamConfig.RamConfig.WrapSize                     = EXTMEM_CUSTOM_WRAP_NOT_SUPPORTED,      /*!< Wrap mode support */ \
                               .HyperRamConfig.RamConfig.CsBoundarySize               = EXTMEM_CUSTOM_CS_BONDARYOF_NONE,       /*!< Chip Select boundary size */ \
                               .HyperRamConfig.RamConfig.Refresh                      = 0,                                     /*!< Refresh setting (units/controller-dependent) */ \
                             }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_is66wvh8m8 = EXTMEM_IS66WVH8M8_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_is66wvh8m8;
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

#endif /* __STM32_IS66WVH8M8__H__ */

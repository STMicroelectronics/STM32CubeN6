/**
  ******************************************************************************
  * @file    stm32_aps6408l.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the APMemory APS6408L-OBMx DDR Octal SPI PSRAM,
  *          using EMM Custom Driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __STM32_APS6408L__H__
#define __STM32_APS6408L__H__

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
#define EXTMEM_APS6408L_CMD_READ                 0x00      /*!< Command to read data */
#define EXTMEM_APS6408L_CMD_WRITE                0x80      /*!< Command to write data */
#define EXTMEM_APS6408L_DUMMY_READ               0x04      /*!< Number of dummy cycles for read operations */
#define EXTMEM_APS6408L_DUMMY_WRITE              0x04      /*!< Number of dummy cycles for write operations */
#define EXTMEM_APS6408L_DUMMY_REG_READ           0x04      /*!< Number of dummy cycles to read register */

/* CMD1: MA0 register: Command to set Latency Not Fixed, Read Latency to 7 and Drive Strength to 50 ohms (default) */
#define EXTMEM_APS6408L_CMD1_WRITE_CFG2          0xC0      /*!< Command to write to a register */
#define EXTMEM_APS6408L_CMD1_READ_CFG2           0x40      /*!< Command to read from a register */
#define EXTMEM_APS6408L_CMD1_ADDR_CFG2           0x00      /*!< Address of the register */
#define EXTMEM_APS6408L_CMD1_VALUE_CFG2          0x11      /*!< Value of the register -> Latency not Fixed */
#define EXTMEM_APS6408L_CMD1_MASK_CFG2           0x3F      /*!< Mask for the register */

/* CMD2: MA4 register: Command to set Write Latency to 7 */
#define EXTMEM_APS6408L_CMD2_WRITE_CFG2          0xC0      /*!< Command to write to a register */
#define EXTMEM_APS6408L_CMD2_READ_CFG2           0x40      /*!< Command to read from a register */
#define EXTMEM_APS6408L_CMD2_ADDR_CFG2           0x04      /*!< Address of the register */
#define EXTMEM_APS6408L_CMD2_VALUE_CFG2          0x20      /*!< Value of the register */
#define EXTMEM_APS6408L_CMD2_MASK_CFG2           0xE0      /*!< Mask for the register */

#define EXTMEM_APS6408L_OCTAL_CMD_READ           0x00      /*!< Command to read data : Sync Read */
#define EXTMEM_APS6408L_OCTAL_CMD_WRITE          0xA0      /*!< Command to write data (Only linear mode supported. No Wrap in Write) */
#define EXTMEM_APS6408L_OCTAL_DUMMY_READ         0x06      /*!< Number of dummy cycles for read operations: 7 D.C. on Memory side corresponding to 6 D.C. on XSPI side */
#define EXTMEM_APS6408L_OCTAL_DUMMY_WRITE        0x06      /*!< Number of dummy cycles for write operations: 7 D.C. on Memory side corresponding to 6 D.C. on XSPI side  */
#define EXTMEM_APS6408L_OCTAL_DUMMY_REG_READ     0x06      /*!< Number of dummy cycles to read register: 7 D.C. on Memory side corresponding to 6 on D.C. XSPI side  */
/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_APS6408L_INIT { \
                                   .MemType                                  = EXTMEM_CUSTOM_PSRAM,                   /*!< Common field for memory type        */ \
                                   .MemStandard                              = EXTMEM_CUSTOM_STD_APMEM,               /*!< Mem Standard e.g., Macronix, Micron, etc. */  \
                                   .MemSize                                  = EXTMEM_CUSTOM_SIZE_64MB,               /*!< Common field for memory Size        */ \
                                   .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_FF,         /*!< Reset method based on FFh instructions */ \
                                   .ResetDelay                               = 1,                                     /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                                   .MemChipSelectHighTimeCycle               = 10,                                    /*!< Common field for memory Chip select high time */ \
                                   .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in Hz*/ \
                                   .StartupConfig.CommandRead                = EXTMEM_APS6408L_CMD_READ,              /*!< Command to read data */ \
                                   .StartupConfig.CommandWrite               = EXTMEM_APS6408L_CMD_WRITE,             /*!< Command to write data */ \
                                   .StartupConfig.DummyCycleRead             = EXTMEM_APS6408L_DUMMY_READ,            /*!< Number of dummy cycles for read operations */ \
                                   .StartupConfig.DummyCycleWrite            = EXTMEM_APS6408L_DUMMY_WRITE,           /*!< Number of dummy cycles for write operations */ \
                                   .StartupConfig.DummyRegisterRead          = EXTMEM_APS6408L_DUMMY_REG_READ,        /*!< Number of dummy cycles to read register */ \
                                   .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_8S_8D_8D,                /*!< Access mode (e.g., 1S4D4D) */ \
                                   .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                   .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                                   .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_ENABLE,              /*!< Data strobe mode */ \
                                   .NbRegisterConfig                         = 3,                                     /*!< Common field for memory Chip select */ \
                                   .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR,/*!< Config step to read/write/read a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_APS6408L_CMD1_WRITE_CFG2,   /*!< Command to write to a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_APS6408L_CMD1_READ_CFG2,    /*!< Command to read from a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_APS6408L_CMD1_READ_CFG2,    /*!< Command to read from a register after write operation */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_APS6408L_CMD1_ADDR_CFG2,    /*!< Address of the register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_APS6408L_CMD1_VALUE_CFG2,   /*!< Value of the register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_APS6408L_CMD1_MASK_CFG2,    /*!< Mask for the register */ \
                                   .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR,/*!< Config step to read/write/read a register */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterWrite   = EXTMEM_APS6408L_CMD2_WRITE_CFG2,   /*!< Command to write to a register */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_APS6408L_CMD2_READ_CFG2,    /*!< Command to read from a register */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterReadAW  = EXTMEM_APS6408L_CMD2_READ_CFG2,    /*!< Command to read from a register after write operation */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterAddress        = EXTMEM_APS6408L_CMD2_ADDR_CFG2,    /*!< Address of the register */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterValue          = EXTMEM_APS6408L_CMD2_VALUE_CFG2,   /*!< Value of the register */ \
                                   .RegisterConfig[1].RWConfigStep.RegisterMask           = EXTMEM_APS6408L_CMD2_MASK_CFG2,    /*!< Mask for the register */ \
                                   .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,    /*!< Config step to use new Optional configuration */ \
                                   .OptionalConfig.Frequency                 = 100000000,                           /*!< Operating frequency in Hz*/ \
                                   .OptionalConfig.CommandRead               = EXTMEM_APS6408L_OCTAL_CMD_READ,      /*!< Command to read data */ \
                                   .OptionalConfig.CommandWrite              = EXTMEM_APS6408L_OCTAL_CMD_WRITE,     /*!< Command to write data */ \
                                   .OptionalConfig.DummyCycleRead            = EXTMEM_APS6408L_OCTAL_DUMMY_READ,    /*!< Number of dummy cycles for read operations */ \
                                   .OptionalConfig.DummyCycleWrite           = EXTMEM_APS6408L_OCTAL_DUMMY_WRITE,   /*!< Number of dummy cycles for write operations */ \
                                   .OptionalConfig.DummyRegisterRead         = EXTMEM_APS6408L_OCTAL_DUMMY_REG_READ,/*!< Number of dummy cycles to read register */ \
                                   .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_8S_8D_8D,              /*!< Access mode (e.g., 1S4D4D) */ \
                                   .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,    /*!< Size of the instruction */ \
                                   .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_32_BITS,       /*!< Size of the address */ \
                                   .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_ENABLE,            /*!< Data strobe mode */ \
                                   .PsramConfig.WrapSize                     = EXTMEM_CUSTOM_WRAP_32_BYTES,         /*!< Wrap size */ \
                                   .PsramConfig.CsBoundarySize               = EXTMEM_CUSTOM_CS_BONDARYOF_16KB,     /*!< Chip Select boundary size */ \
                                   .PsramConfig.Refresh                      = 0x0,                                 /*!< Refresh rate */ \
                                   }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_aps6408l = EXTMEM_APS6408L_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_aps6408l;
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

#endif /* __STM32_APS6408L__H__ */

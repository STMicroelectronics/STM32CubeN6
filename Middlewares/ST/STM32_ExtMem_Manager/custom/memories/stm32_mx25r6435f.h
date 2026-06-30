/**
  ******************************************************************************
  * @file    stm32_mx25r6435f.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the Macronix MX25R6435F NOR flash memory,
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
#ifndef __STM32_MX25R6435F_H__
#define __STM32_MX25R6435F_H__

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
#define EXTMEM_MX25R6435F_CMD_READ                     0x03U
#define EXTMEM_MX25R6435F_CMD_WRITE                    0x02U
#define EXTMEM_MX25R6435F_DUMMY_READ                   0U
#define EXTMEM_MX25R6435F_DUMMY_WRITE                  0U
#define EXTMEM_MX25R6435F_DUMMY_REG_READ               0U

#define EXTMEM_MX25R6435F_OPT_CMD_READ                 0xEBU
#define EXTMEM_MX25R6435F_OPT_CMD_WRITE                0x38U
#define EXTMEM_MX25R6435F_OPT_DUMMY_READ               6U
#define EXTMEM_MX25R6435F_OPT_DUMMY_WRITE              0U
#define EXTMEM_MX25R6435F_OPT_DUMMY_REG_READ           0U

#define EXTMEM_MX25R6435F_PAGE_SIZE                    256U
#define EXTMEM_MX25R6435F_SECTOR_SIZE                  4096U
#define EXTMEM_MX25R6435F_MAX_ERASE_SECTOR_TIME        240U
#define EXTMEM_MX25R6435F_MAX_MASS_ERASE_TIME          240000U

#define EXTMEM_MX25R6435F_CMD_ERASE_SECTOR             0x20U
#define EXTMEM_MX25R6435F_CMD_MASS_ERASE               0x60U
#define EXTMEM_MX25R6435F_CMD_RDSR                     0x05U
#define EXTMEM_MX25R6435F_CMD_WE                       0x06U
#define EXTMEM_MX25R6435F_MATCH_MASK_WEL               0x02U
#define EXTMEM_MX25R6435F_MATCH_VALUE_WEL              0x02U
#define EXTMEM_MX25R6435F_MATCH_MASK_WIP               0x01U
#define EXTMEM_MX25R6435F_MATCH_VALUE_WIP              0x00U

#define EXTMEM_MX25R6435F_CMD0_REG_WRITE               0x01U
#define EXTMEM_MX25R6435F_CMD0_REG_READ                0x05U
#define EXTMEM_MX25R6435F_CMD0_REG_READ_AW             0x05U
#define EXTMEM_MX25R6435F_CMD0_REG_VALUE               0x40U
#define EXTMEM_MX25R6435F_CMD0_REG_MASK                0x40U

#define EXTMEM_MX25R6435F_CMD1_REG_READ                0x15U

#define EXTMEM_MX25R6435F_CMD2_REG_WRITE               0x01U
#define EXTMEM_MX25R6435F_CMD2_REG_VALUE               0x400002U   /* Set L/H Switch to 1 (High Perf mode) + 6DC */
#define EXTMEM_MX25R6435F_CMD2_REG_MASK                0xFFFFFFU

EXTMEM_DRIVER_CUSTOM_StatusTypeDef Mx25r6435f_UserCallback(void *UserCtx, uint32_t UserArg0, uint32_t UserArg1);

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_MX25R6435F_INIT { \
                                   .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,             /*!< Common field for memory type        */ \
                                   .MemStandard                              = EXTMEM_CUSTOM_STD_MACRONIX,          /*!< e.g., Macronix, Micron, etc.        */  \
                                   .MemSize                                  = EXTMEM_CUSTOM_SIZE_64MB,             /*!< Common field for memory Size        */ \
                                   .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_66_99,    /*!< Reset method based on 6699h and 9966h instructions */ \
                                   .SampleShiftCfg                           = EXTMEM_CUSTOM_SSHIFT_CFG_UNDEFINED,  /*!< Sample shift based on memory standard */ \
                                   .ResetDelay                               = 100U,                                /*!< Time needed after Reset in ms, 0 if no required delay */ \
                                   .MemChipSelectHighTimeCycle               = 10U,                                 /*!< Common field for memory Chip select high time */ \
                                   .StartupConfig.Frequency                  = 33000000U,                           /*!< Operating frequency in Hz*/ \
                                   .StartupConfig.CommandRead                = EXTMEM_MX25R6435F_CMD_READ,          /*!< Command to read data */ \
                                   .StartupConfig.CommandWrite               = EXTMEM_MX25R6435F_CMD_WRITE,         /*!< Command to write data */ \
                                   .StartupConfig.DummyCycleRead             = EXTMEM_MX25R6435F_DUMMY_READ,        /*!< Number of dummy cycles for read operations */ \
                                   .StartupConfig.DummyCycleWrite            = EXTMEM_MX25R6435F_DUMMY_WRITE,       /*!< Number of dummy cycles for write operations */ \
                                   .StartupConfig.DummyRegisterRead          = EXTMEM_MX25R6435F_DUMMY_REG_READ,    /*!< Number of dummy cycles to read register */ \
                                   .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,              /*!< Access mode (e.g., 1S4D4D) */ \
                                   .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,    /*!< Size of the instruction */ \
                                   .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_24_BITS,       /*!< Size of the address */ \
                                   .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,           /*!< Data strobe mode */ \
                                   .NbRegisterConfig                         = 5U,                                  /*!< Number of register config steps */ \
                                   .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG,      /*!< Config step to read/write/read Status register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_MX25R6435F_CMD0_REG_WRITE,   /*!< Command to write to a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_MX25R6435F_CMD0_REG_READ,    /*!< Command to read from a register */ \
                                   .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_MX25R6435F_CMD0_REG_READ_AW, /*!< Command to read from a register */ \
                                   .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_MX25R6435F_CMD0_REG_VALUE,   /*!< Value of the register : QE enable*/ \
                                   .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_MX25R6435F_CMD0_REG_MASK,    /*!< Mask for the register */ \
                                   .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RR_REG,       /*!< Config step to read Configuration register 1 and 2 */ \
                                   .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_MX25R6435F_CMD1_REG_READ,    /*!< Command to read from Configuration register */ \
                                   .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_WWW_REG,      /*!< Config step to write Status and Configuration registers */ \
                                   .RegisterConfig[2].RWConfigStep.CommandRegisterWrite   = EXTMEM_MX25R6435F_CMD2_REG_WRITE,   /*!< Command to write to a register (3 bytes data) */ \
                                   .RegisterConfig[2].RWConfigStep.RegisterValue          = EXTMEM_MX25R6435F_CMD2_REG_VALUE,   /*!< Value of the registers (SR + CF1 + CF2) */ \
                                   .RegisterConfig[2].RWConfigStep.RegisterMask           = EXTMEM_MX25R6435F_CMD2_REG_MASK,    /*!< Mask for the register */ \
                                   .RegisterConfig[3].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RR_REG,       /*!< Config step to read Configuration register 1 and 2 */ \
                                   .RegisterConfig[3].RWConfigStep.CommandRegisterRead    = EXTMEM_MX25R6435F_CMD1_REG_READ,    /*!< Command to read from Configuration register */ \
                                   .RegisterConfig[4].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG, /*!< Config step to read/write/read a register */ \
                                   .OptionalConfig.Frequency                 = 80000000U,                               /*!< Operating frequency in Hz*/ \
                                   .OptionalConfig.CommandRead               = EXTMEM_MX25R6435F_OPT_CMD_READ,          /*!< Command to read data */ \
                                   .OptionalConfig.CommandWrite              = EXTMEM_MX25R6435F_OPT_CMD_WRITE,         /*!< Command to write data */ \
                                   .OptionalConfig.DummyCycleRead            = EXTMEM_MX25R6435F_OPT_DUMMY_READ,        /*!< Number of dummy cycles for read operations */ \
                                   .OptionalConfig.DummyCycleWrite           = EXTMEM_MX25R6435F_OPT_DUMMY_WRITE,       /*!< Number of dummy cycles for write operations */ \
                                   .OptionalConfig.DummyRegisterRead         = EXTMEM_MX25R6435F_OPT_DUMMY_REG_READ,    /*!< Number of dummy cycles to read register */ \
                                   .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_1S_4S_4S,                  /*!< Access mode (e.g., 1S4D4D) */ \
                                   .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,        /*!< Size of the instruction */ \
                                   .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_24_BITS,           /*!< Size of the address */ \
                                   .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_DISABLE,               /*!< Data strobe mode */ \
                                   .NorFlashConfig.PageSize                  = EXTMEM_MX25R6435F_PAGE_SIZE,             /*!< Page Size */ \
                                   .NorFlashConfig.SectorSize                = EXTMEM_MX25R6435F_SECTOR_SIZE,           /*!< Sector Size */ \
                                   .NorFlashConfig.MaxSectorEraseTime        = EXTMEM_MX25R6435F_MAX_ERASE_SECTOR_TIME, /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                                   .NorFlashConfig.MaxChipEraseTime          = EXTMEM_MX25R6435F_MAX_MASS_ERASE_TIME,   /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                                   .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_MX25R6435F_CMD_ERASE_SECTOR,      /*!< Command to erase a sector */ \
                                   .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_MX25R6435F_CMD_MASS_ERASE,        /*!< Command to perform mass erase */ \
                                   .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_MX25R6435F_CMD_RDSR,              /*!< Command to read status register */ \
                                   .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_MX25R6435F_CMD_WE,                /*!< Command to enable write operations */ \
                                   .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_MX25R6435F_MATCH_MASK_WEL,        /*!< Mask for write enable latch */ \
                                   .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_MX25R6435F_MATCH_VALUE_WEL,       /*!< Match Value for write enable latch */ \
                                   .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_MX25R6435F_MATCH_MASK_WIP,        /*!< Mask for write in progress */ \
                                   .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_MX25R6435F_MATCH_VALUE_WIP,       /*!< Match Value for write in progress */ \
                                   }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_mx25r6435f = EXTMEM_MX25R6435F_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_mx25r6435f;
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

#endif /* __STM32_MX25R6435F_H__ */
/**
  ******************************************************************************
  * @file    stm32_mx25um51245g_50Mhz.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the Macronix MX25UM51245G NOR flash memory,
  *          using EMM Custom Driver, with a maximum frequency of 50 MHz.
  *          Compared with stm32_mx25um51245g.h, which allows configuration at 100 MHz, this file
  *          provides an example configuration at a lower speed, with a reduced number
  *          of duty cycles in DTR-OPI mode.
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
#ifndef __STM32_MX25UM51245G_50MHZ_H__
#define __STM32_MX25UM51245G_50MHZ_H__

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
#define EXTMEM_MX25UM51245G_CMD_READ                 0x0C    /*!< Command to read data */
#define EXTMEM_MX25UM51245G_CMD_WRITE                0x12    /*!< Command to write data */
#define EXTMEM_MX25UM51245G_DUMMY_READ               0x08    /*!< Number of dummy cycles for read operations */
#define EXTMEM_MX25UM51245G_DUMMY_WRITE              0x00    /*!< Number of dummy cycles for write operations */
#define EXTMEM_MX25UM51245G_DUMMY_REG_READ           0       /*!< Number of dummy cycles to read register */

#define EXTMEM_MX25UM51245G_PAGE_SIZE                256     /*!< Page size 256 Bytes */
#define EXTMEM_MX25UM51245G_SECTOR_SIZE              0x1000  /*!< Sector size in bytes (4K) */
#define EXTMEM_MX25UM51245G_CMD_ERASE_SECTOR         0x21    /*!< Command to erase a sector */
#define EXTMEM_MX25UM51245G_CMD_MASS_ERASE           0x60    /*!< Command to perform mass erase */
#define EXTMEM_MX25UM51245G_CMD_RDSR                 0x05    /*!< Command to read status register */
#define EXTMEM_MX25UM51245G_CMD_WE                   0x06    /*!< Command to enable write operations */
#define EXTMEM_MX25UM51245G_MASK_WEL                 0x02    /*!< Mask for write enable latch */
#define EXTMEM_MX25UM51245G_VALUE_WEL                0x02    /*!< Match Value for write enable latch */
#define EXTMEM_MX25UM51245G_MASK_WIP                 0x01    /*!< Mask for write in progress */
#define EXTMEM_MX25UM51245G_VALUE_WIP                0x00    /*!< Match Value for write in progress */

#define EXTMEM_MX25UM51245G_CMD1_WRITE_CFG2          0x72    /*!< Command to write to a register */
#define EXTMEM_MX25UM51245G_CMD1_READ_CFG2           0x71    /*!< Command to read from a register */
#define EXTMEM_MX25UM51245G_CMD1_READAW_CFG2         0x71    /*!< Command to read after write operation */
#define EXTMEM_MX25UM51245G_CMD1_ADDR_CFG2           0x300   /*!< Address of the register => Dummy cycles */
#define EXTMEM_MX25UM51245G_CMD1_VALUE_CFG2          0x07    /*!< Value of the register : 7 => 6 cycles */
#define EXTMEM_MX25UM51245G_CMD1_MASK_CFG2           0x07    /*!< Mask for the register */

#define EXTMEM_MX25UM51245G_CMD2_WRITE_CFG2          0x72    /*!< Command to write to a register */
#define EXTMEM_MX25UM51245G_CMD2_READ_CFG2           0x71    /*!< Command to read from a register */
#define EXTMEM_MX25UM51245G_CMD2_ADDR_CFG2           0x00    /*!< Address of the register */
#define EXTMEM_MX25UM51245G_CMD2_VALUE_CFG2          0x02    /*!< Value of the register */
#define EXTMEM_MX25UM51245G_CMD2_MASK_CFG2           0x02    /*!< Mask for the register */

#define EXTMEM_MX25UM51245G_OCTAL_CMD_ERASE_SECTOR   0x21DE  /*!< Command to erase a sector */
#define EXTMEM_MX25UM51245G_OCTAL_CMD_MASS_ERASE     0x609F  /*!< Command to perform mass erase */
#define EXTMEM_MX25UM51245G_OCTAL_CMD_READ           0xEE11  /*!< Command to read data */
#define EXTMEM_MX25UM51245G_OCTAL_CMD_WRITE          0x12ED  /*!< Command to write data */
#define EXTMEM_MX25UM51245G_OCTAL_DUMMY_READ         6       /*!< Number of dummy cycles for read operations */
#define EXTMEM_MX25UM51245G_OCTAL_DUMMY_WRITE        0       /*!< Number of dummy cycles for write operations */
#define EXTMEM_MX25UM51245G_OCTAL_DUMMY_REG_READ     4       /*!< Number of dummy cycles to read register */
#define EXTMEM_MX25UM51245G_OCTAL_CMD_RDSR           0x05FA  /*!< Command to read status register in octal mode */
#define EXTMEM_MX25UM51245G_OCTAL_CMD_WE             0x06F9  /*!< Command to enable write operations in octal mode */
#define EXTMEM_MX25UM51245G_OCTAL_MASK_WEL           0x02    /*!< Mask for write enable latch */
#define EXTMEM_MX25UM51245G_OCTAL_VALUE_WEL          0x02    /*!< Match Value for write enable latch */
#define EXTMEM_MX25UM51245G_OCTAL_MASK_WIP           0x01    /*!< Mask for write in progress */
#define EXTMEM_MX25UM51245G_OCTAL_VALUE_WIP          0x00    /*!< Match Value for write in progress */
/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_MX25UM51245G_INIT { \
                                         .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,               /*!< Common field for memory type        */ \
                                         .MemStandard                              = EXTMEM_CUSTOM_STD_MACRONIX,            /*!< e.g., Macronix, Micron, etc.        */  \
                                         .MemSize                                  = EXTMEM_CUSTOM_SIZE_512MB,              /*!< Common field for memory Size        */ \
                                         .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_6699_9966,  /*!< Reset method based on 6699h and 9966h instructions */ \
                                         .ResetDelay                               = 100,                                   /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                                         .MemChipSelectHighTimeCycle               = 8,                                     /*!< Common field for memory Chip select high time */ \
                                         .StartupConfig.Frequency                  = 20000000,                              /*!< Operating frequency in Hz*/ \
                                         .StartupConfig.CommandRead                = EXTMEM_MX25UM51245G_CMD_READ,          /*!< Command to read data */ \
                                         .StartupConfig.CommandWrite               = EXTMEM_MX25UM51245G_CMD_WRITE,         /*!< Command to write data */ \
                                         .StartupConfig.DummyCycleRead             = EXTMEM_MX25UM51245G_DUMMY_READ,        /*!< Number of dummy cycles for read operations */ \
                                         .StartupConfig.DummyCycleWrite            = EXTMEM_MX25UM51245G_DUMMY_WRITE,       /*!< Number of dummy cycles for write operations */ \
                                         .StartupConfig.DummyRegisterRead          = EXTMEM_MX25UM51245G_DUMMY_REG_READ,    /*!< Number of dummy cycles to read register */ \
                                         .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,                /*!< Access mode (e.g., 1S4D4D) */ \
                                         .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                         .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                                         .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                                         .NbRegisterConfig                         = 3,                                     /*!< Common field for memory Chip select */ \
                                         .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR,    /*!< Config step to read/write/read a register */ \
                                         .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_MX25UM51245G_CMD1_WRITE_CFG2,   /*!< Command to write to a register */ \
                                         .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_MX25UM51245G_CMD1_READ_CFG2,    /*!< Command to read from a register */ \
                                         .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_MX25UM51245G_CMD1_READAW_CFG2,  /*!< Command to read from a register */ \
                                         .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_MX25UM51245G_CMD1_ADDR_CFG2,    /*!< Address of the register */ \
                                         .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_MX25UM51245G_CMD1_VALUE_CFG2,   /*!< Value of the register */ \
                                         .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_MX25UM51245G_CMD1_MASK_CFG2,    /*!< Mask for the register */ \
                                         .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR,      /*!< Config step to read/write/read a register */ \
                                         .RegisterConfig[1].RWConfigStep.CommandRegisterWrite   = EXTMEM_MX25UM51245G_CMD2_WRITE_CFG2,   /*!< Command to write to a register */ \
                                         .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_MX25UM51245G_CMD2_READ_CFG2,    /*!< Command to read from a register */ \
                                         .RegisterConfig[1].RWConfigStep.RegisterAddress        = EXTMEM_MX25UM51245G_CMD2_ADDR_CFG2,    /*!< Address of the register */ \
                                         .RegisterConfig[1].RWConfigStep.RegisterValue          = EXTMEM_MX25UM51245G_CMD2_VALUE_CFG2,   /*!< Value of the register */ \
                                         .RegisterConfig[1].RWConfigStep.RegisterMask           = EXTMEM_MX25UM51245G_CMD2_MASK_CFG2,    /*!< Mask for the register */ \
                                         .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,    /*!< Config step to use new Optional configuration */ \
                                         .OptionalConfig.Frequency                 = 50000000,                              /*!< Operating frequency in Hz*/ \
                                         .OptionalConfig.CommandRead               = EXTMEM_MX25UM51245G_OCTAL_CMD_READ,    /*!< Command to read data */ \
                                         .OptionalConfig.CommandWrite              = EXTMEM_MX25UM51245G_OCTAL_CMD_WRITE,   /*!< Command to write data */ \
                                         .OptionalConfig.DummyCycleRead            = EXTMEM_MX25UM51245G_OCTAL_DUMMY_READ,  /*!< Number of dummy cycles for read operations */ \
                                         .OptionalConfig.DummyCycleWrite           = EXTMEM_MX25UM51245G_OCTAL_DUMMY_WRITE, /*!< Number of dummy cycles for write operations */ \
                                         .OptionalConfig.DummyRegisterRead         = EXTMEM_MX25UM51245G_OCTAL_DUMMY_REG_READ,/*!< Number of dummy cycles to read register */ \
                                         .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_8D_8D_8D,                /*!< Access mode (e.g., 1S4D4D) */ \
                                         .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_16_BITS,     /*!< Size of the instruction */ \
                                         .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_32_BITS,         /*!< Size of the address */ \
                                         .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_ENABLE,              /*!< Data strobe mode */ \
                                         .NorFlashConfig.PageSize                  = EXTMEM_MX25UM51245G_PAGE_SIZE,         /*!< Page Size */ \
                                         .NorFlashConfig.SectorSize                = EXTMEM_MX25UM51245G_SECTOR_SIZE,       /*!< Sector Size */ \
                                         .NorFlashConfig.MaxSectorEraseTime        = 2000,                                  /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                                         .NorFlashConfig.MaxChipEraseTime          = 300000,                                /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                                         .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_MX25UM51245G_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                                         .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_MX25UM51245G_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                                         .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_MX25UM51245G_CMD_RDSR,          /*!< Command to read status register */ \
                                         .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_MX25UM51245G_CMD_WE,            /*!< Command to enable write operations */ \
                                         .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_MX25UM51245G_MASK_WEL,          /*!< Mask for write enable latch */ \
                                         .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_MX25UM51245G_VALUE_WEL,         /*!< Match Value for write enable latch */ \
                                         .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_MX25UM51245G_MASK_WIP,          /*!< Mask for write in progress */ \
                                         .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_MX25UM51245G_VALUE_WIP,         /*!< Match Value for write in progress */ \
                                         .NorFlashConfig.Startup.SRAccessDetails   = EXTMEM_CUSTOM_SR_ACCESS_CFG_UNDEFINED, /*!< No details for SR register access (no address required) */ \
                                         .NorFlashConfig.OptionalConfigEnable      = 1,                                     /*!< Optional config for NorFlash is used */ \
                                         .NorFlashConfig.Optional.Cmd_EraseSector  = EXTMEM_MX25UM51245G_OCTAL_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                                         .NorFlashConfig.Optional.Cmd_MassErase    = EXTMEM_MX25UM51245G_OCTAL_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                                         .NorFlashConfig.Optional.Cmd_RDSR         = EXTMEM_MX25UM51245G_OCTAL_CMD_RDSR,    /*!< Command to read status register in octal mode */ \
                                         .NorFlashConfig.Optional.Cmd_WE           = EXTMEM_MX25UM51245G_OCTAL_CMD_WE,      /*!< Command to enable write operations in octal mode */ \
                                         .NorFlashConfig.Optional.MatchMask_WEL    = EXTMEM_MX25UM51245G_OCTAL_MASK_WEL,    /*!< Mask for write enable latch */ \
                                         .NorFlashConfig.Optional.MatchValue_WEL   = EXTMEM_MX25UM51245G_OCTAL_VALUE_WEL,   /*!< Match Value for write enable latch */ \
                                         .NorFlashConfig.Optional.MatchMask_WIP    = EXTMEM_MX25UM51245G_OCTAL_MASK_WIP,    /*!< Mask for write in progress */ \
                                         .NorFlashConfig.Optional.MatchValue_WIP   = EXTMEM_MX25UM51245G_OCTAL_VALUE_WIP,   /*!< Match Value for write in progress */ \
                                         .NorFlashConfig.Optional.SRAccessDetails  = EXTMEM_CUSTOM_SR_ACCESS_ADD_0,         /*!< Access details for SR register in octal mode*/ \
                                         }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_mx25um51245g = EXTMEM_MX25UM51245G_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_mx25um51245g;
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

#endif /* __STM32_MX25UM51245G_50MHZ_H__ */

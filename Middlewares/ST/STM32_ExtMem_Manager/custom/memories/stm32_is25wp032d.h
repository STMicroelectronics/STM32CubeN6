/**
  ******************************************************************************
  * @file    stm32_is25wp032d.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the ISSI IS25WP032D NOR flash memory,
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
#ifndef __STM32_IS25WP032D__H__
#define __STM32_IS25WP032D__H__

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
#define EXTMEM_IS25WP032D_CMD_READ                 0x03     /*!< Command to read data */
#define EXTMEM_IS25WP032D_CMD_WRITE                0x02     /*!< Command to write data */
#define EXTMEM_IS25WP032D_DUMMY_READ               0x06     /*!< Number of dummy cycles for read operations */
#define EXTMEM_IS25WP032D_DUMMY_WRITE              0x00     /*!< Number of dummy cycles for write operations */
#define EXTMEM_IS25WP032D_DUMMY_REG_READ           0        /*!< Number of dummy cycles to read register */

#define EXTMEM_IS25WP032D_PAGE_SIZE                256      /*!< Page size 256 Bytes */
#define EXTMEM_IS25WP032D_SECTOR_SIZE              0x1000   /*!< Page size 4KBytes */
#define EXTMEM_IS25WP032D_CMD_ERASE_SECTOR         0x20     /*!< Command to erase a sector */
#define EXTMEM_IS25WP032D_CMD_MASS_ERASE           0x60     /*!< Command to perform mass erase */
#define EXTMEM_IS25WP032D_CMD_RDSR                 0x05     /*!< Command to read status register */
#define EXTMEM_IS25WP032D_CMD_WE                   0x06     /*!< Command to enable write operations */
#define EXTMEM_IS25WP032D_MASK_WEL                 0x02     /*!< Mask for write enable latch */
#define EXTMEM_IS25WP032D_VALUE_WEL                0x02     /*!< Match Value for write enable latch */
#define EXTMEM_IS25WP032D_MASK_WIP                 0x01     /*!< Mask for write in progress */
#define EXTMEM_IS25WP032D_VALUE_WIP                0x00     /*!< Match Value for write in progress */

/* 1st Register config command to be sent
   WE 06 lg data 0 / RDSR  lg data 1 / val |= 0x40 / WSR 01 lg data 1
*/
#define EXTMEM_IS25WP032D_CMD1_WRITE_SR            0x01     /*!< WRITE STATUS Register command WRSR 0x01  */
#define EXTMEM_IS25WP032D_CMD1_READ_SR             0x05     /*!< READ STATUS Register command RDSR 0x05 */
#define EXTMEM_IS25WP032D_CMD1_READAW_SR           0x05     /*!< Command to read after write operation */
#define EXTMEM_IS25WP032D_CMD1_VALUE_SR_QE         0x40     /*!< Value of the register : QE */
#define EXTMEM_IS25WP032D_CMD1_MASK_SR_QE          0x7C     /*!< Mask for the register : QE + BPx bits */

/* 2nd configuration step : 0x35 command to be sent */
#define EXTMEM_IS25WP032D_CMD2_INSTRUCTION         0x35     /*!< Instruction to enable QPI mode */

#define EXTMEM_IS25WP032D_QUAD_CMD_ERASE_SECTOR    0x20     /*!< Command to erase a sector */
#define EXTMEM_IS25WP032D_QUAD_CMD_MASS_ERASE      0x60     /*!< Command to perform mass erase */
#define EXTMEM_IS25WP032D_QUAD_CMD_READ            0xEB     /*!< Command to read data */
#define EXTMEM_IS25WP032D_QUAD_CMD_WRITE           0x02     /*!< Command to write data */
#define EXTMEM_IS25WP032D_QUAD_DUMMY_READ          0x06     /*!< Number of dummy cycles for read operations */
#define EXTMEM_IS25WP032D_QUAD_DUMMY_WRITE         0x00     /*!< Number of dummy cycles for write operations */
#define EXTMEM_IS25WP032D_QUAD_DUMMY_REG_READ      0x00     /*!< Number of dummy cycles to read register */
#define EXTMEM_IS25WP032D_QUAD_CMD_RDSR            0x05     /*!< Command to read status register in octal mode */
#define EXTMEM_IS25WP032D_QUAD_CMD_WE              0x06     /*!< Command to enable write operations in octal mode */
#define EXTMEM_IS25WP032D_QUAD_MASK_WEL            0x02     /*!< Mask for write enable latch */
#define EXTMEM_IS25WP032D_QUAD_VALUE_WEL           0x02     /*!< Match Value for write enable latch */
#define EXTMEM_IS25WP032D_QUAD_MASK_WIP            0x01     /*!< Mask for write in progress */
#define EXTMEM_IS25WP032D_QUAD_VALUE_WIP           0x00     /*!< Match Value for write in progress */
/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_IS25WP032D_INIT { \
                                 .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,               /*!< Common field for memory type         */ \
                                 .MemStandard                              = EXTMEM_CUSTOM_STD_MICRON,              /*!< e.g., Macronix, Micron, etc.         */ \
                                 .MemSize                                  = EXTMEM_CUSTOM_SIZE_32MB,               /*!< 32Mbits = 4MBytes memory Size        */ \
                                 .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_66_99,      /*!< Reset method based on 66h and 99h instructions */ \
                                 .ResetDelay                               = 2,                                     /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                                 .SampleShiftCfg                           = EXTMEM_CUSTOM_SSHIFT_CFG_HALFCYCLE,    /*!< Sample Shift setting */ \
                                 .MemChipSelectHighTimeCycle               = 4,                                     /*!< Chip select High time in cycles      */ \
                                 .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in MHz*/ \
                                 .StartupConfig.CommandRead                = EXTMEM_IS25WP032D_CMD_READ,            /*!< Command to read data */ \
                                 .StartupConfig.CommandWrite               = EXTMEM_IS25WP032D_CMD_WRITE,           /*!< Command to write data */ \
                                 .StartupConfig.DummyCycleRead             = EXTMEM_IS25WP032D_DUMMY_READ,          /*!< Number of dummy cycles for read operations */ \
                                 .StartupConfig.DummyCycleWrite            = EXTMEM_IS25WP032D_DUMMY_WRITE,         /*!< Number of dummy cycles for write operations */ \
                                 .StartupConfig.DummyRegisterRead          = EXTMEM_IS25WP032D_DUMMY_REG_READ,      /*!< Number of dummy cycles to read register */ \
                                 .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,                /*!< Access mode (e.g., 1S4D4D) */ \
                                 .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                 .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                                 .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                                 .NbRegisterConfig                         = 3,                                     /*!< Common field for memory Chip select */ \
                                 .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG,         /*!< Config step to read/write/read to a register */ \
                                 .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_IS25WP032D_CMD1_WRITE_SR,       /*!< Command to write to a register */ \
                                 .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_IS25WP032D_CMD1_READ_SR,        /*!< Command to read from status register */ \
                                 .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_IS25WP032D_CMD1_READAW_SR,      /*!< Command to read from a register after write operation */ \
                                 .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_IS25WP032D_CMD1_VALUE_SR_QE,    /*!< Value of the register */ \
                                 .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_IS25WP032D_CMD1_MASK_SR_QE,     /*!< Mask for the register */ \
                                 .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_INST,            /*!< Config Step : Instruction type */ \
                                 .RegisterConfig[1].INSConfigStep.Instruction           = EXTMEM_IS25WP032D_CMD2_INSTRUCTION,    /*!< Command to write to a register */ \
                                 .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,    /*!< Config step to use new Optional configuration */ \
                                 .OptionalConfig.Frequency                 = 104000000,                             /*!< IS25WP032D limited to 104 MHz */ \
                                 .OptionalConfig.CommandRead               = EXTMEM_IS25WP032D_QUAD_CMD_READ,       /*!< Command to read data */ \
                                 .OptionalConfig.CommandWrite              = EXTMEM_IS25WP032D_QUAD_CMD_WRITE,      /*!< Command to write data */ \
                                 .OptionalConfig.DummyCycleRead            = EXTMEM_IS25WP032D_QUAD_DUMMY_READ,     /*!< Number of dummy cycles for read operations */ \
                                 .OptionalConfig.DummyCycleWrite           = EXTMEM_IS25WP032D_QUAD_DUMMY_WRITE,    /*!< Number of dummy cycles for write operations */ \
                                 .OptionalConfig.DummyRegisterRead         = EXTMEM_IS25WP032D_QUAD_DUMMY_REG_READ, /*!< Number of dummy cycles to read register */ \
                                 .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_4S_4S_4S,                /*!< Access mode (e.g., 1S4D4D) */ \
                                 .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                                 .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                                 .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                                 .NorFlashConfig.PageSize                  = EXTMEM_IS25WP032D_PAGE_SIZE,         /*!< Page Size */ \
                                 .NorFlashConfig.SectorSize                = EXTMEM_IS25WP032D_SECTOR_SIZE,       /*!< Sector Size */ \
                                 .NorFlashConfig.MaxSectorEraseTime        = 1000,                                /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                                 .NorFlashConfig.MaxChipEraseTime          = 24000,                               /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                                 .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_IS25WP032D_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                                 .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_IS25WP032D_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                                 .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_IS25WP032D_CMD_RDSR,          /*!< Command to read status register */ \
                                 .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_IS25WP032D_CMD_WE,            /*!< Command to enable write operations */ \
                                 .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_IS25WP032D_MASK_WEL,          /*!< Mask for write enable latch */ \
                                 .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_IS25WP032D_VALUE_WEL,         /*!< Match Value for write enable latch */ \
                                 .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_IS25WP032D_MASK_WIP,          /*!< Mask for write in progress */ \
                                 .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_IS25WP032D_VALUE_WIP,         /*!< Match Value for write in progress */ \
                                 .NorFlashConfig.OptionalConfigEnable      = 1,                                   /*!< Optional config for NorFlash is used */ \
                                 .NorFlashConfig.Optional.Cmd_EraseSector  = EXTMEM_IS25WP032D_QUAD_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                                 .NorFlashConfig.Optional.Cmd_MassErase    = EXTMEM_IS25WP032D_QUAD_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                                 .NorFlashConfig.Optional.Cmd_RDSR         = EXTMEM_IS25WP032D_QUAD_CMD_RDSR,    /*!< Command to read status register in octal mode */ \
                                 .NorFlashConfig.Optional.Cmd_WE           = EXTMEM_IS25WP032D_QUAD_CMD_WE,      /*!< Command to enable write operations in octal mode */ \
                                 .NorFlashConfig.Optional.MatchMask_WEL    = EXTMEM_IS25WP032D_QUAD_MASK_WEL,    /*!< Mask for write enable latch */ \
                                 .NorFlashConfig.Optional.MatchValue_WEL   = EXTMEM_IS25WP032D_QUAD_VALUE_WEL,   /*!< Match Value for write enable latch */ \
                                 .NorFlashConfig.Optional.MatchMask_WIP    = EXTMEM_IS25WP032D_QUAD_MASK_WIP,    /*!< Mask for write in progress */ \
                                 .NorFlashConfig.Optional.MatchValue_WIP   = EXTMEM_IS25WP032D_QUAD_VALUE_WIP,   /*!< Match Value for write in progress */ \
}
#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_is25wp032d = EXTMEM_IS25WP032D_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_is25wp032d;
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

#endif /* __STM32_IS25WP032D__H__ */

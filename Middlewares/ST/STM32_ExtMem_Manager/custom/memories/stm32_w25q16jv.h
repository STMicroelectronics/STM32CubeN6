/**
  ******************************************************************************
  * @file    stm32_w25q16jv.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the Windbond W25Q16JV NOR flash memory,
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
#ifndef __STM32_W25Q16JV__H__
#define __STM32_W25Q16JV__H__

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
#define EXTMEM_W25Q16JV_CMD_READ                 0x0B    /*!< Command to read data */
#define EXTMEM_W25Q16JV_CMD_WRITE                0x02    /*!< Command to write data */
#define EXTMEM_W25Q16JV_DUMMY_READ               0x08    /*!< Number of dummy cycles for read operations */
#define EXTMEM_W25Q16JV_DUMMY_WRITE              0x00    /*!< Number of dummy cycles for write operations */
#define EXTMEM_W25Q16JV_DUMMY_REG_READ           0x00    /*!< Number of dummy cycles to read register */

#define EXTMEM_W25Q16JV_PAGE_SIZE                256     /*!< Page size 256 Bytes */
#define EXTMEM_W25Q16JV_SECTOR_SIZE              0x1000  /*!< Sector size in bytes (4K) */
#define EXTMEM_W25Q16JV_CMD_ERASE_SECTOR         0x20    /*!< Command to erase a sector */
#define EXTMEM_W25Q16JV_CMD_MASS_ERASE           0x60    /*!< Command to perform mass erase */
#define EXTMEM_W25Q16JV_CMD_RDSR                 0x05    /*!< Command to read status register */
#define EXTMEM_W25Q16JV_CMD_WE                   0x06    /*!< Command to enable write operations */
#define EXTMEM_W25Q16JV_MASK_WEL                 0x02    /*!< Mask for write enable latch */
#define EXTMEM_W25Q16JV_VALUE_WEL                0x02    /*!< Match Value for write enable latch */
#define EXTMEM_W25Q16JV_MASK_WIP                 0x01    /*!< Mask for write in progress */
#define EXTMEM_W25Q16JV_VALUE_WIP                0x00    /*!< Match Value for write in progress */

/* CMD1: Read and Write Status Register 3: Output Driver Strength 50% */
#define EXTMEM_W25Q16JV_CMD1_READ_SR3            0x15    /*!< READ STATUS Register 3 command */
#define EXTMEM_W25Q16JV_CMD1_WRITE_SR3           0x11    /*!< Command to write to a register SR3 */
#define EXTMEM_W25Q16JV_CMD1_VALUE               0x00    /*!< Value of the register (Output Driver Strength 50%)*/
#define EXTMEM_W25Q16JV_CMD1_MASK                0x60    /*!< Mask of the register */

/* CMD1: Read and Write Status Register 2: Quad Enable */
#define EXTMEM_W25Q16JV_CMD2_READ_SR2            0x35    /*!< READ STATUS Register 2 command */
#define EXTMEM_W25Q16JV_CMD2_WRITE_SR2           0x31    /*!< Command to write to a register SR2 */
#define EXTMEM_W25Q16JV_CMD2_VALUE               0x02    /*!< Value of the register (QE enable)*/
#define EXTMEM_W25Q16JV_CMD2_MASK                0x02    /*!< Mask of the register */

#define EXTMEM_W25Q16JV_QUAD_CMD_READ            0x6B    /*!< Command to read data */
#define EXTMEM_W25Q16JV_QUAD_CMD_WRITE           0x32    /*!< Command to write data */
#define EXTMEM_W25Q16JV_QUAD_DUMMY_READ          0x08    /*!< Number of dummy cycles for read. */
#define EXTMEM_W25Q16JV_QUAD_DUMMY_WRITE         0x00    /*!< Number of dummy cycles for write */
#define EXTMEM_W25Q16JV_QUAD_DUMMY_REG_READ      0x00    /*!< Number of dummy cycles to read register */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_W25Q16JV_INIT { \
                               .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,               /*!< Common field for memory type        */ \
                               .MemStandard                              = EXTMEM_CUSTOM_STD_MACRONIX,            /*!< e.g., Macronix, Micron, etc.        */ \
                               .MemSize                                  = EXTMEM_CUSTOM_SIZE_16MB,               /*!< Common field for memory Size        */ \
                               .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_66_99,      /*!< Reset method based on 66h and 99h instructions */ \
                               .ResetDelay                               = 2,                                     /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                               .MemChipSelectHighTimeCycle               = 7,                                     /*!< Common field for memory Chip select high time */ \
                               .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in Hz*/ \
                               .StartupConfig.CommandRead                = EXTMEM_W25Q16JV_CMD_READ,              /*!< Command to read data */ \
                               .StartupConfig.CommandWrite               = EXTMEM_W25Q16JV_CMD_WRITE,             /*!< Command to write data */ \
                               .StartupConfig.DummyCycleRead             = EXTMEM_W25Q16JV_DUMMY_READ,            /*!< Number of dummy cycles for read operations */ \
                               .StartupConfig.DummyCycleWrite            = EXTMEM_W25Q16JV_DUMMY_WRITE,           /*!< Number of dummy cycles for write operations */ \
                               .StartupConfig.DummyRegisterRead          = EXTMEM_W25Q16JV_DUMMY_REG_READ,        /*!< Number of dummy cycles to read register */ \
                               .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,                /*!< Access mode (e.g., 1S4D4D) */ \
                               .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                               .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                               .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                               .NbRegisterConfig                         = 3,                                     /*!< Common field for memory Chip select */ \
                               .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG,     /*!< Config step to read and write register (8 bits) */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_W25Q16JV_CMD1_READ_SR3,     /*!< Command to read SR3 register */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_W25Q16JV_CMD1_WRITE_SR3,    /*!< Command to write to SR3 register */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_W25Q16JV_CMD1_READ_SR3,     /*!< Command to write to SR2 register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_W25Q16JV_CMD1_VALUE,        /*!< Value to write into the SR3 register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_W25Q16JV_CMD1_MASK,         /*!< Mask for the SR3 register */ \
                               .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG,     /*!< Config step to read and write register (8 bits) */ \
                               .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_W25Q16JV_CMD2_READ_SR2,     /*!< Command to read SR2 register */ \
                               .RegisterConfig[1].RWConfigStep.CommandRegisterWrite   = EXTMEM_W25Q16JV_CMD2_WRITE_SR2,    /*!< Command to write to SR2 register */ \
                               .RegisterConfig[1].RWConfigStep.CommandRegisterReadAW  = EXTMEM_W25Q16JV_CMD2_READ_SR2,     /*!< Command to write to SR2 register */ \
                               .RegisterConfig[1].RWConfigStep.RegisterValue          = EXTMEM_W25Q16JV_CMD2_VALUE,        /*!< Value to write into the SR2 register */ \
                               .RegisterConfig[1].RWConfigStep.RegisterMask           = EXTMEM_W25Q16JV_CMD2_MASK,         /*!< Mask for the SR2 register */ \
                               .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,/*!< Config step to use new Optional configuration */ \
                               .OptionalConfig.Frequency                 = 133000000,                             /*!< Operating frequency in Hz*/ \
                               .OptionalConfig.CommandRead               = EXTMEM_W25Q16JV_QUAD_CMD_READ,         /*!< Command to read data */ \
                               .OptionalConfig.CommandWrite              = EXTMEM_W25Q16JV_QUAD_CMD_WRITE,        /*!< Command to write data */ \
                               .OptionalConfig.DummyCycleRead            = EXTMEM_W25Q16JV_QUAD_DUMMY_READ,       /*!< Number of dummy cycles for read operations */ \
                               .OptionalConfig.DummyCycleWrite           = EXTMEM_W25Q16JV_QUAD_DUMMY_WRITE,      /*!< Number of dummy cycles for write operations */ \
                               .OptionalConfig.DummyRegisterRead         = EXTMEM_W25Q16JV_QUAD_DUMMY_REG_READ,   /*!< Number of dummy cycles to read register */ \
                               .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_1S_1S_4S,                /*!< Access mode (e.g., 1S4D4D) */ \
                               .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                               .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                               .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                               .NorFlashConfig.PageSize                  = EXTMEM_W25Q16JV_PAGE_SIZE,             /*!< Page Size */ \
                               .NorFlashConfig.SectorSize                = EXTMEM_W25Q16JV_SECTOR_SIZE,           /*!< Sector Size */ \
                               .NorFlashConfig.MaxSectorEraseTime        = 2000,                                  /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                               .NorFlashConfig.MaxChipEraseTime          = 100000,                                /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                               .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_W25Q16JV_CMD_ERASE_SECTOR,      /*!< Command to erase a sector */ \
                               .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_W25Q16JV_CMD_MASS_ERASE,        /*!< Command to perform mass erase */ \
                               .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_W25Q16JV_CMD_RDSR,              /*!< Command to read status register */ \
                               .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_W25Q16JV_CMD_WE,                /*!< Command to enable write operations */ \
                               .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_W25Q16JV_MASK_WEL,              /*!< Mask for write enable latch */ \
                               .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_W25Q16JV_VALUE_WEL,             /*!< Match Value for write enable latch */ \
                               .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_W25Q16JV_MASK_WIP,              /*!< Mask for write in progress */ \
                               .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_W25Q16JV_VALUE_WIP,             /*!< Match Value for write in progress */ \
}

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w25q16jv = EXTMEM_W25Q16JV_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w25q16jv;
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

#endif /* __STM32_W25Q16JV__H__ */

/**
  ******************************************************************************
  * @file    stm32_w35t51nw.h
  * @author  MCD Application Team
  * @brief   This file contains configuration details for support of the Windbond W35T51NW NOR flash memory,
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
#ifndef __STM32_W35T51NW__H__
#define __STM32_W35T51NW__H__

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
#define EXTMEM_W35T51NW_CMD_READ                 0x0B    /*!< Command to read data */
#define EXTMEM_W35T51NW_CMD_WRITE                0x02    /*!< Command to write data */
#define EXTMEM_W35T51NW_DUMMY_READ               0x08    /*!< Number of dummy cycles for read operations */
#define EXTMEM_W35T51NW_DUMMY_WRITE              0x00    /*!< Number of dummy cycles for write operations */
#define EXTMEM_W35T51NW_DUMMY_REG_READ           0x08    /*!< Number of dummy cycles to read register */

#define EXTMEM_W35T51NW_PAGE_SIZE                256     /*!< Page size 256 Bytes */
#define EXTMEM_W35T51NW_SECTOR_SIZE              0x1000  /*!< Sector size in bytes (4K) */
#define EXTMEM_W35T51NW_CMD_ERASE_SECTOR         0x20    /*!< Command to erase a sector */
#define EXTMEM_W35T51NW_CMD_MASS_ERASE           0x60    /*!< Command to perform mass erase */
#define EXTMEM_W35T51NW_CMD_RDSR                 0x05    /*!< Command to read status register */
#define EXTMEM_W35T51NW_CMD_WE                   0x06    /*!< Command to enable write operations */
#define EXTMEM_W35T51NW_MASK_WEL                 0x02    /*!< Mask for write enable latch */
#define EXTMEM_W35T51NW_VALUE_WEL                0x02    /*!< Match Value for write enable latch */
#define EXTMEM_W35T51NW_MASK_WIP                 0x01    /*!< Mask for write in progress */
#define EXTMEM_W35T51NW_VALUE_WIP                0x00    /*!< Match Value for write in progress */

/* CMD1: Command use to change Dummy Cucle to 22DC @200MHz (default 16DC @166MHz)*/
#define EXTMEM_W35T51NW_CMD1_WRITE_CFGR          0x81    /*!< Command to write to a register */
#define EXTMEM_W35T51NW_CMD1_READ_CFGR           0x85    /*!< Command to read from a register */
#define EXTMEM_W35T51NW_CMD1_READAW_CFGR         0x85    /*!< Command to read after write operation */
#define EXTMEM_W35T51NW_CMD1_ADDR_CFGR           0x01    /*!< Address of the register */
#define EXTMEM_W35T51NW_CMD1_VALUE_CFGR          0x16    /*!< Value of the register 22 DC */
#define EXTMEM_W35T51NW_CMD1_MASK_CFGR           0x1F    /*!< Mask for the register */

/* CMD2: Command use to switch memory in Octal DDR mode 0xE7. (default 0xFF. Extended SPI) */
#define EXTMEM_W35T51NW_CMD2_WRITE_CFGR          0x81    /*!< Command to write to a register */
#define EXTMEM_W35T51NW_CMD2_READ_CFGR           0x85    /*!< Command to read from a register */
#define EXTMEM_W35T51NW_CMD2_READAW_CFGR         0x85    /*!< Command to read after write operation */
#define EXTMEM_W35T51NW_CMD2_ADDR_CFGR           0x00    /*!< Address of the register */
#define EXTMEM_W35T51NW_CMD2_VALUE_CFGR          0xE7    /*!< Value of the register (DTR + Octal Mode)*/
#define EXTMEM_W35T51NW_CMD2_MASK_CFGR           0xFF    /*!< Mask for the register */

/* CMD3: Command read  01h adress of the configuration register. Expercted value: 0xE7h (Octal DDR mode activated) */
#define EXTMEM_W35T51NW_CMD3_READ_CFGR           0x8585  /*!< Command to read from a register */
#define EXTMEM_W35T51NW_CMD3_ADDR_CFGR           0x00    /*!< Address of the register */

#define EXTMEM_W35T51NW_OCTAL_CMD_ERASE_SECTOR   0x2121  /*!< Command to erase a sector */
#define EXTMEM_W35T51NW_OCTAL_CMD_MASS_ERASE     0x6060  /*!< Command to perform mass erase */
#define EXTMEM_W35T51NW_OCTAL_CMD_READ           0xFDFD  /*!< Command to read data */
#define EXTMEM_W35T51NW_OCTAL_CMD_WRITE          0x8E8E  /*!< Command to write data */
#define EXTMEM_W35T51NW_OCTAL_DUMMY_READ         22      /*!< Number of dummy cycles for read. 22 DC for 4-Byte Aligned Start Address */
#define EXTMEM_W35T51NW_OCTAL_DUMMY_WRITE        0       /*!< Number of dummy cycles for write */
#define EXTMEM_W35T51NW_OCTAL_DUMMY_REG_READ     8       /*!< Number of dummy cycles to read register */
#define EXTMEM_W35T51NW_OCTAL_CMD_RDSR           0x0505  /*!< Command to read status register in octal mode */
#define EXTMEM_W35T51NW_OCTAL_CMD_WE             0x0606  /*!< Command to enable write operations in octal mode */
#define EXTMEM_W35T51NW_OCTAL_MASK_WEL           0x02    /*!< Mask for write enable latch */
#define EXTMEM_W35T51NW_OCTAL_VALUE_WEL          0x02    /*!< Match Value for write enable latch */
#define EXTMEM_W35T51NW_OCTAL_MASK_WIP           0x01    /*!< Mask for write in progress */
#define EXTMEM_W35T51NW_OCTAL_VALUE_WIP          0x00    /*!< Match Value for write in progress */
/**
  * @}
  */

/* Private typedefs ----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup CUSTOM_Exported_Functions Exported Functions
  * @{
  */

#define EXTMEM_W35T51NW_INIT { \
                               .MemType                                  = EXTMEM_CUSTOM_NOR_FLASH,               /*!< Common field for memory type        */ \
                               .MemStandard                              = EXTMEM_CUSTOM_STD_MACRONIX,            /*!< e.g., Macronix, Micron, etc.        */ \
                               .MemSize                                  = EXTMEM_CUSTOM_SIZE_512MB,              /*!< Common field for memory Size        */ \
                               .ResetMethod                              = EXTMEM_CUSTOM_RESET_METHOD_66_99,      /*!< Reset method based on 66h and 99h instructions */ \
                               .ResetDelay                               = 2,                                     /*!< Time needed after Reset (in ms, 0 if no required delay */ \
                               .MemChipSelectHighTimeCycle               = 5,                                     /*!< Common field for memory Chip select high time */ \
                               .StartupConfig.Frequency                  = 50000000,                              /*!< Operating frequency in Hz*/ \
                               .StartupConfig.CommandRead                = EXTMEM_W35T51NW_CMD_READ,              /*!< Command to read data */ \
                               .StartupConfig.CommandWrite               = EXTMEM_W35T51NW_CMD_WRITE,             /*!< Command to write data */ \
                               .StartupConfig.DummyCycleRead             = EXTMEM_W35T51NW_DUMMY_READ,            /*!< Number of dummy cycles for read operations */ \
                               .StartupConfig.DummyCycleWrite            = EXTMEM_W35T51NW_DUMMY_WRITE,           /*!< Number of dummy cycles for write operations */ \
                               .StartupConfig.DummyRegisterRead          = EXTMEM_W35T51NW_DUMMY_REG_READ,        /*!< Number of dummy cycles to read register */ \
                               .StartupConfig.AccessMode                 = EXTMEM_CUSTOM_1S_1S_1S,                /*!< Access mode (e.g., 1S4D4D) */ \
                               .StartupConfig.InstructionSize            = EXTMEM_CUSTOM_INSTRUCTION_8_BITS,      /*!< Size of the instruction */ \
                               .StartupConfig.AddressSize                = EXTMEM_CUSTOM_ADDRESS_24_BITS,         /*!< Size of the address */ \
                               .StartupConfig.DqsMode                    = EXTMEM_CUSTOM_DQS_DISABLE,             /*!< Data strobe mode */ \
                               .NbRegisterConfig                         = 4,                                     /*!< Common field for memory Chip select */ \
                               .RegisterConfig[0].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR,/*!< Config step to read/write/read a register */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterWrite   = EXTMEM_W35T51NW_CMD1_WRITE_CFGR,   /*!< Command to write to a register */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterRead    = EXTMEM_W35T51NW_CMD1_READ_CFGR,    /*!< Command to read from a register */ \
                               .RegisterConfig[0].RWConfigStep.CommandRegisterReadAW  = EXTMEM_W35T51NW_CMD1_READAW_CFGR,  /*!< Command to read from a register after write operation */ \
                               .RegisterConfig[0].RWConfigStep.RegisterAddress        = EXTMEM_W35T51NW_CMD1_ADDR_CFGR,    /*!< Address of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterValue          = EXTMEM_W35T51NW_CMD1_VALUE_CFGR,   /*!< Value of the register */ \
                               .RegisterConfig[0].RWConfigStep.RegisterMask           = EXTMEM_W35T51NW_CMD1_MASK_CFGR,    /*!< Mask for the register */ \
                               .RegisterConfig[1].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR, /*!< Config step to read/write/read a register */ \
                               .RegisterConfig[1].RWConfigStep.CommandRegisterWrite   = EXTMEM_W35T51NW_CMD2_WRITE_CFGR,   /*!< Command to write to a register */ \
                               .RegisterConfig[1].RWConfigStep.CommandRegisterRead    = EXTMEM_W35T51NW_CMD2_READ_CFGR,    /*!< Command to read from a register */ \
                               .RegisterConfig[1].RWConfigStep.RegisterAddress        = EXTMEM_W35T51NW_CMD2_ADDR_CFGR,    /*!< Address of the register */ \
                               .RegisterConfig[1].RWConfigStep.RegisterValue          = EXTMEM_W35T51NW_CMD2_VALUE_CFGR,   /*!< Value of the register */ \
                               .RegisterConfig[1].RWConfigStep.RegisterMask           = EXTMEM_W35T51NW_CMD2_MASK_CFGR,    /*!< Mask for the register */ \
                               .RegisterConfig[2].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG,/*!< Config step to use new Optional configuration */ \
                               .RegisterConfig[3].ConfigStepType                      = EXTMEM_CUSTOM_CFGSTEP_R_REG_ADDR,  /*!< Config step to read/write/read a register */ \
                               .RegisterConfig[3].RWConfigStep.CommandRegisterRead    = EXTMEM_W35T51NW_CMD3_READ_CFGR,    /*!< Command to read from a register */ \
                               .RegisterConfig[3].RWConfigStep.RegisterAddress        = EXTMEM_W35T51NW_CMD3_ADDR_CFGR,    /*!< Address of the register */ \
                               .OptionalConfig.Frequency                 = 200000000,                               /*!< Operating frequency in Hz*/ \
                               .OptionalConfig.CommandRead               = EXTMEM_W35T51NW_OCTAL_CMD_READ,          /*!< Command to read data */ \
                               .OptionalConfig.CommandWrite              = EXTMEM_W35T51NW_OCTAL_CMD_WRITE,         /*!< Command to write data */ \
                               .OptionalConfig.DummyCycleRead            = EXTMEM_W35T51NW_OCTAL_DUMMY_READ,        /*!< Number of dummy cycles for read operations */ \
                               .OptionalConfig.DummyCycleWrite           = EXTMEM_W35T51NW_OCTAL_DUMMY_WRITE,       /*!< Number of dummy cycles for write operations */ \
                               .OptionalConfig.DummyRegisterRead         = EXTMEM_W35T51NW_OCTAL_DUMMY_REG_READ,    /*!< Number of dummy cycles to read register */ \
                               .OptionalConfig.AccessMode                = EXTMEM_CUSTOM_8D_8D_8D,                  /*!< Access mode (e.g., 1S4D4D) */ \
                               .OptionalConfig.InstructionSize           = EXTMEM_CUSTOM_INSTRUCTION_16_BITS,       /*!< Size of the instruction */ \
                               .OptionalConfig.AddressSize               = EXTMEM_CUSTOM_ADDRESS_32_BITS,           /*!< Size of the address */ \
                               .OptionalConfig.DqsMode                   = EXTMEM_CUSTOM_DQS_ENABLE,                /*!< Data strobe mode */ \
                               .NorFlashConfig.PageSize                  = EXTMEM_W35T51NW_PAGE_SIZE,               /*!< Page Size */ \
                               .NorFlashConfig.SectorSize                = EXTMEM_W35T51NW_SECTOR_SIZE,             /*!< Sector Size */ \
                               .NorFlashConfig.MaxSectorEraseTime        = 6000,                                    /*!< Max time expected for completing a Sector Erase operation (in ms) */ \
                               .NorFlashConfig.MaxChipEraseTime          = 700000,                                  /*!< Max time expected for completing a Chip Erase operation (in ms) */ \
                               .NorFlashConfig.Startup.Cmd_EraseSector   = EXTMEM_W35T51NW_CMD_ERASE_SECTOR,        /*!< Command to erase a sector */ \
                               .NorFlashConfig.Startup.Cmd_MassErase     = EXTMEM_W35T51NW_CMD_MASS_ERASE,          /*!< Command to perform mass erase */ \
                               .NorFlashConfig.Startup.Cmd_RDSR          = EXTMEM_W35T51NW_CMD_RDSR,                /*!< Command to read status register */ \
                               .NorFlashConfig.Startup.Cmd_WE            = EXTMEM_W35T51NW_CMD_WE,                  /*!< Command to enable write operations */ \
                               .NorFlashConfig.Startup.MatchMask_WEL     = EXTMEM_W35T51NW_MASK_WEL,                /*!< Mask for write enable latch */ \
                               .NorFlashConfig.Startup.MatchValue_WEL    = EXTMEM_W35T51NW_VALUE_WEL,               /*!< Match Value for write enable latch */ \
                               .NorFlashConfig.Startup.MatchMask_WIP     = EXTMEM_W35T51NW_MASK_WIP,                /*!< Mask for write in progress */ \
                               .NorFlashConfig.Startup.MatchValue_WIP    = EXTMEM_W35T51NW_VALUE_WIP,               /*!< Match Value for write in progress */ \
                               .NorFlashConfig.OptionalConfigEnable      = 1,                                       /*!< Optional config for NorFlash is used */ \
                               .NorFlashConfig.Optional.Cmd_EraseSector  = EXTMEM_W35T51NW_OCTAL_CMD_ERASE_SECTOR,  /*!< Command to erase a sector */ \
                               .NorFlashConfig.Optional.Cmd_MassErase    = EXTMEM_W35T51NW_OCTAL_CMD_MASS_ERASE,    /*!< Command to perform mass erase */ \
                               .NorFlashConfig.Optional.Cmd_RDSR         = EXTMEM_W35T51NW_OCTAL_CMD_RDSR,          /*!< Command to read status register in octal mode */ \
                               .NorFlashConfig.Optional.Cmd_WE           = EXTMEM_W35T51NW_OCTAL_CMD_WE,            /*!< Command to enable write operations in octal mode */ \
                               .NorFlashConfig.Optional.MatchMask_WEL    = EXTMEM_W35T51NW_OCTAL_MASK_WEL,          /*!< Mask for write enable latch */ \
                               .NorFlashConfig.Optional.MatchValue_WEL   = EXTMEM_W35T51NW_OCTAL_VALUE_WEL,         /*!< Match Value for write enable latch */ \
                               .NorFlashConfig.Optional.MatchMask_WIP    = EXTMEM_W35T51NW_OCTAL_MASK_WIP,          /*!< Mask for write in progress */ \
                               .NorFlashConfig.Optional.MatchValue_WIP   = EXTMEM_W35T51NW_OCTAL_VALUE_WIP,         /*!< Match Value for write in progress */ \
                             }

#if defined(EXTMEM_C)
EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w35t51nw = EXTMEM_W35T51NW_INIT;
#else
extern EXTMEM_DRIVER_CUSTOM_ObjectTypeDef extmem_w35t51nw;
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

#endif /* __STM32_W35T51NW__H__ */

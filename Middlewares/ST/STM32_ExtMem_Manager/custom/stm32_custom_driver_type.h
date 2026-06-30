/**
  ******************************************************************************
  * @file    stm32_custom_driver_type.h
  * @author  MCD Application Team
  * @brief   This file contains custom driver type definitions.
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
#ifndef __STM32_CUSTOM_TYPE_H
#define __STM32_CUSTOM_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32_custom_driver_private_define.h"

/** @addtogroup CUSTOM
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_CUSTOM == 1

/*! Macro to check if the type is an access mode type. */
#define IS_ACCESS_MODE_TYPE(_TYPE_)              (((_TYPE_) & ACCESS_MODE_TYPE) == (ACCESS_MODE_TYPE))

/*! Macro to check if the memory type is not flash. */
#define IS_NOT_MEMORY_FLASH(_MEM_)               (((_MEM_) & FLASH_TYPE_MASK) != (FLASH_TYPE_MASK))

/*! Macro to check if the memory type is psram. */
#define IS_MEMORY_PSRAM(_MEM_)                   (((_MEM_) & PSRAM_TYPE_MASK) == (PSRAM_TYPE_MASK))

/*! Macro to check if the memory type is hyperbus. */
#define IS_MEMORY_HYPERBUS(_MEM_)                (((_MEM_) & HYPERBUS_TYPE_MASK) == (HYPERBUS_TYPE_MASK))

/*! Macro to check if the memory type is not hyperbus. */
#define IS_NOT_MEMORY_HYPERBUS(_MEM_)            (((_MEM_) & HYPERBUS_TYPE_MASK) != (HYPERBUS_TYPE_MASK))

/*!  Macro to check if configuration step is of HyperBus type. */
#define IS_CFGSTEP_HYPERBUS_TYPE(_CFGSTEP_)      (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == \
                                                  (CUSTOM_CFGSTEP_HYPERBUS_CONFIG))

/*! Macro to check if Address is present in configuration step. */
#define IS_CFGSTEP_ADDR(_CFGSTEP_)               (((_CFGSTEP_) & CFGSTEP_ADDRESS) == (CFGSTEP_ADDRESS))

/*! Macro to check if configuration step uses 8-bit data size. */
#define IS_CFGSTEP_DATASIZE_8BIT(_CFGSTEP_)      (((_CFGSTEP_) & CFGSTEP_DATASIZE_MASK) == (CFGSTEP_DATASIZE_8BIT))

/*! Macro to check if configuration step uses 16-bit data size. */
#define IS_CFGSTEP_DATASIZE_16BIT(_CFGSTEP_)     (((_CFGSTEP_) & CFGSTEP_DATASIZE_MASK) == (CFGSTEP_DATASIZE_16BIT))

/*! Macro to check if configuration step uses 24-bit data size. */
#define IS_CFGSTEP_DATASIZE_24BIT(_CFGSTEP_)     (((_CFGSTEP_) & CFGSTEP_DATASIZE_MASK) == (CFGSTEP_DATASIZE_24BIT))

/*!  Macro to check if configuration step is of Instruction type. */
#define IS_CFGSTEP_INST_TYPE(_CFGSTEP_)          (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_INST_CONFIG))

/*! Macro to check if configuration step is of Status Register access type. */
#define IS_CFGSTEP_REG_ACCESS_TYPE(_CFGSTEP_)    (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_REG_CONFIG))

/*! Macro to check if configuration step is of Execute Optional config type. */
#define IS_CFGSTEP_EXEC_OPT_CFG_TYPE(_CFGSTEP_)  (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == (CFGSTEP_EXEC_OPT_CONFIG))

/*! Macro to check if configuration step is of User callback type. */
#define IS_CFGSTEP_USER_CALLBACK_TYPE(_CFGSTEP_) (((_CFGSTEP_) & CFGSTEP_CONFIG_TYPE_MASK) == \
                                                  (CFGSTEP_USER_CALLBACK_CONFIG))

/*! Macro to check if configuration step is basic instruction. */
#define IS_CFGSTEP_INS_BASIC(_CFGSTEP_)          (((_CFGSTEP_) & CFGSTEP_INST_TYPE_MASK) == (CFGSTEP_INST_BASIC))

/*! Macro to check if configuration step is Read register operation. */
#define IS_CFGSTEP_REG_R_ACCESS(_CFGSTEP_)       (((_CFGSTEP_) & CFGSTEP_REG_R_TYPE) == (CFGSTEP_REG_R_TYPE))

/*! Macro to check if configuration step is Write register operation. */
#define IS_CFGSTEP_REG_W_ACCESS(_CFGSTEP_)       (((_CFGSTEP_) & CFGSTEP_REG_W_TYPE) == (CFGSTEP_REG_W_TYPE))

/*! Macro to check if configuration step is ReadAfterWrite register operation. */
#define IS_CFGSTEP_REG_RAW_ACCESS(_CFGSTEP_)     (((_CFGSTEP_) & CFGSTEP_REG_RAW_TYPE) == (CFGSTEP_REG_RAW_TYPE))

/*! Macro to check if a configuration setting is defined (i.e. provided in the configuration file). */
#define IS_CUSTOM_SETTING_CFG_DEFINED(SETTING)   (((SETTING) &\
                                                   CUSTOM_SETTING_CFG_DEFINED) == CUSTOM_SETTING_CFG_DEFINED)
/*! Macro to extract the raw setting value (masked) from a configuration setting field. */
#define CUSTOM_SETTING_CFG_VALUE(SETTING)        ((SETTING) & CUSTOM_SETTING_CFG_MASK)

/* Exported constants --------------------------------------------------------*/
/** @defgroup CUSTOM_Exported_constants Exported constants
  * @{
  */

/*!  Maximum number of register configurations. */
#define EXTMEM_CUSTOM_MAX_REG_CONFIG         6U

/*!  Read JEDEC ID command value. */
#define EXTMEM_CUSTOM_JEDECID_CMD            0x9F

/*! Size of JEDEC ID. */
#define EXTMEM_CUSTOM_JEDECID_SIZE           3U

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup CUSTOM_Exported_Types Exported Types
  * @{
  */

/**
  * @brief List of error codes of the CUSTOM driver
  * @note A return value of 0 indicates success; negative values indicate an error.
  */
typedef enum
{
  EXTMEM_DRV_CUSTOM_OK                     =  0,          /*!< Status of Operation OK */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_INIT          = -1,          /*!< XSPI initialization error */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_DEINIT        = -2,          /*!< XSPI de-initialization error */
  EXTMEM_DRV_CUSTOM_ERR_XSPI_TIMEOUT       = -3,          /*!< XSPI timeout error */
  EXTMEM_DRV_CUSTOM_ERR_CLOCK_INIT         = -4,          /*!< Clock initialization error */
  EXTMEM_DRV_CUSTOM_ERR_SET_COMMAND        = -5,          /*!< Error setting XSPI command */
  EXTMEM_DRV_CUSTOM_ERR_RESET_MEMORY       = -6,          /*!< Error resetting memory */
  EXTMEM_DRV_CUSTOM_ERR_JEDEC_ID           = -7,          /*!< JEDEC ID error */
  EXTMEM_DRV_CUSTOM_ERR_READ_REGISTER      = -8,          /*!< Error reading register */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_REGISTER     = -9,          /*!< Error writing register */
  EXTMEM_DRV_CUSTOM_ERR_SEND_INSTRUCTION   = -10,         /*!< Error sending instruction */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_INSTRUCTION  = -11,         /*!< Error writing instruction */
  EXTMEM_DRV_CUSTOM_ERR_CONFIG_STEP        = -12,         /*!< Configuration step error */
  EXTMEM_DRV_CUSTOM_ERR_MEMORY_TYPE        = -13,         /*!< Memory type error */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_ENABLE       = -14,         /*!< Write enable error */
  EXTMEM_DRV_CUSTOM_ERR_FLASH_BUSY         = -15,         /*!< Flash busy error */
  EXTMEM_DRV_CUSTOM_ERR_READ_MEMORY        = -16,         /*!< Error reading memory */
  EXTMEM_DRV_CUSTOM_ERR_WRITE_MEMORY       = -17,         /*!< Error writing memory */
  EXTMEM_DRV_CUSTOM_ERR_ENABLE_MEM_MAP     = -18,         /*!< Error enabling memory-mapped mode */
  EXTMEM_DRV_CUSTOM_ERR_DISABLE_MEM_MAP    = -19,         /*!< Error disabling memory-mapped mode */
  EXTMEM_DRV_CUSTOM_ERR_ADDRESS_ALIGNMENT  = -20,         /*!< Address alignment error */
  EXTMEM_DRV_CUSTOM_ERR_SECTOR_SIZE        = -21,         /*!< Sector size error */
  EXTMEM_DRV_CUSTOM_ERR_ERASE_TIMEOUT      = -22,         /*!< Erase timeout error */
  EXTMEM_DRV_CUSTOM_ERR_DEVICEC_ID         = -23,         /*!< Device ID error */
  EXTMEM_DRV_CUSTOM_ERR_USER_CALLBACK      = -24,         /*!< Error executing user callback */
  EXTMEM_DRV_CUSTOM_ERR                    = -128         /*!< Generic error */
} EXTMEM_DRIVER_CUSTOM_StatusTypeDef;

/**
  * @brief Enumeration of supported memory sizes.
  * @note This information can be found in the "Overview/Features" section of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_SIZE_16B    = HAL_XSPI_SIZE_16B,                                  /*!<  16 bits  (  2  Bytes = 2^( 0+1)) */
  EXTMEM_CUSTOM_SIZE_32B    = HAL_XSPI_SIZE_32B,                                  /*!<  32 bits  (  4  Bytes = 2^( 1+1)) */
  EXTMEM_CUSTOM_SIZE_64B    = HAL_XSPI_SIZE_64B,                                  /*!<  64 bits  (  8  Bytes = 2^( 2+1)) */
  EXTMEM_CUSTOM_SIZE_128B   = HAL_XSPI_SIZE_128B,                                 /*!< 128 bits  ( 16  Bytes = 2^( 3+1)) */
  EXTMEM_CUSTOM_SIZE_256B   = HAL_XSPI_SIZE_256B,                                 /*!< 256 bits  ( 32  Bytes = 2^( 4+1)) */
  EXTMEM_CUSTOM_SIZE_512B   = HAL_XSPI_SIZE_512B,                                 /*!< 512 bits  ( 64  Bytes = 2^( 5+1)) */
  EXTMEM_CUSTOM_SIZE_1KB    = HAL_XSPI_SIZE_1KB,                                  /*!<   1 Kbits (128  Bytes = 2^( 6+1)) */
  EXTMEM_CUSTOM_SIZE_2KB    = HAL_XSPI_SIZE_2KB,                                  /*!<   2 Kbits (256  Bytes = 2^( 7+1)) */
  EXTMEM_CUSTOM_SIZE_4KB    = HAL_XSPI_SIZE_4KB,                                  /*!<   4 Kbits (512  Bytes = 2^( 8+1)) */
  EXTMEM_CUSTOM_SIZE_8KB    = HAL_XSPI_SIZE_8KB,                                  /*!<   8 Kbits (  1 KBytes = 2^( 9+1)) */
  EXTMEM_CUSTOM_SIZE_16KB   = HAL_XSPI_SIZE_16KB,                                 /*!<  16 Kbits (  2 KBytes = 2^(10+1)) */
  EXTMEM_CUSTOM_SIZE_32KB   = HAL_XSPI_SIZE_32KB,                                 /*!<  32 Kbits (  4 KBytes = 2^(11+1)) */
  EXTMEM_CUSTOM_SIZE_64KB   = HAL_XSPI_SIZE_64KB,                                 /*!<  64 Kbits (  8 KBytes = 2^(12+1)) */
  EXTMEM_CUSTOM_SIZE_128KB  = HAL_XSPI_SIZE_128KB,                                /*!< 128 Kbits ( 16 KBytes = 2^(13+1)) */
  EXTMEM_CUSTOM_SIZE_256KB  = HAL_XSPI_SIZE_256KB,                                /*!< 256 Kbits ( 32 KBytes = 2^(14+1)) */
  EXTMEM_CUSTOM_SIZE_512KB  = HAL_XSPI_SIZE_512KB,                                /*!< 512 Kbits ( 64 KBytes = 2^(15+1)) */
  EXTMEM_CUSTOM_SIZE_1MB    = HAL_XSPI_SIZE_1MB,                                  /*!<   1 Mbits (128 KBytes = 2^(16+1)) */
  EXTMEM_CUSTOM_SIZE_2MB    = HAL_XSPI_SIZE_2MB,                                  /*!<   2 Mbits (256 KBytes = 2^(17+1)) */
  EXTMEM_CUSTOM_SIZE_4MB    = HAL_XSPI_SIZE_4MB,                                  /*!<   4 Mbits (512 KBytes = 2^(18+1)) */
  EXTMEM_CUSTOM_SIZE_8MB    = HAL_XSPI_SIZE_8MB,                                  /*!<   8 Mbits (  1 MBytes = 2^(19+1)) */
  EXTMEM_CUSTOM_SIZE_16MB   = HAL_XSPI_SIZE_16MB,                                 /*!<  16 Mbits (  2 MBytes = 2^(20+1)) */
  EXTMEM_CUSTOM_SIZE_32MB   = HAL_XSPI_SIZE_32MB,                                 /*!<  32 Mbits (  4 MBytes = 2^(21+1)) */
  EXTMEM_CUSTOM_SIZE_64MB   = HAL_XSPI_SIZE_64MB,                                 /*!<  64 Mbits (  8 MBytes = 2^(22+1)) */
  EXTMEM_CUSTOM_SIZE_128MB  = HAL_XSPI_SIZE_128MB,                                /*!< 128 Mbits ( 16 MBytes = 2^(23+1)) */
  EXTMEM_CUSTOM_SIZE_256MB  = HAL_XSPI_SIZE_256MB,                                /*!< 256 Mbits ( 32 MBytes = 2^(24+1)) */
  EXTMEM_CUSTOM_SIZE_512MB  = HAL_XSPI_SIZE_512MB,                                /*!< 512 Mbits ( 64 MBytes = 2^(25+1)) */
  EXTMEM_CUSTOM_SIZE_1GB    = HAL_XSPI_SIZE_1GB,                                  /*!<   1 Gbits (128 MBytes = 2^(26+1)) */
  EXTMEM_CUSTOM_SIZE_2GB    = HAL_XSPI_SIZE_2GB,                                  /*!<   2 Gbits (256 MBytes = 2^(27+1)) */
  EXTMEM_CUSTOM_SIZE_4GB    = HAL_XSPI_SIZE_4GB,                                  /*!<   4 Gbits (512 MBytes = 2^(28+1)) */
  EXTMEM_CUSTOM_SIZE_8GB    = HAL_XSPI_SIZE_8GB,                                  /*!<   8 Gbits (  1 GBytes = 2^(29+1)) */
  EXTMEM_CUSTOM_SIZE_16GB   = HAL_XSPI_SIZE_16GB,                                 /*!<  16 Gbits (  2 GBytes = 2^(30+1)) */
  EXTMEM_CUSTOM_SIZE_32GB   = HAL_XSPI_SIZE_32GB                                  /*!<  32 Gbits (  4 GBytes = 2^(31+1)) */
} EXTMEM_CUSTOM_MemSizeTypeDef;

#if defined (HAL_XSPI_CSSEL_NCS1)
/**
  * @brief Enumeration of nCS configurations.
  * @note This selects which XSPI chip select line is used to address the external memory.
  */
typedef enum
{
  EXTMEM_CUSTOM_NCS1 = HAL_XSPI_CSSEL_NCS1, /*!< The output of nCS is nCS1 */
  EXTMEM_CUSTOM_NCS2 = HAL_XSPI_CSSEL_NCS2  /*!< The output of nCS is nCS2 */
} EXTMEM_CUSTOM_MemNcsTypeDef;
#endif /* HAL_XSPI_CSSEL_NCS1 */

/**
  * @brief Enumeration of combined access mode configurations.
  * @note This information can be found in the "Command description" section (look for the transaction diagrams)
  *       of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_1S_1S_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_1S),      /*!< 1S instruction, 1S address, 1S data  */
  EXTMEM_CUSTOM_1S_1S_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_1D),      /*!< 1S instruction, 1S address, 1D data  */
  EXTMEM_CUSTOM_1S_1D_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_1S),      /*!< 1S instruction, 1D address, 1S data  */
  EXTMEM_CUSTOM_1S_1D_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_1D),      /*!< 1S instruction, 1D address, 1D data  */
  EXTMEM_CUSTOM_1D_1S_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1S | DATA_1S),      /*!< 1D instruction, 1S address, 1S data  */
  EXTMEM_CUSTOM_1D_1S_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1S | DATA_1D),      /*!< 1D instruction, 1S address, 1D data  */
  EXTMEM_CUSTOM_1D_1D_1S  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1D | DATA_1S),      /*!< 1D instruction, 1D address, 1S data  */
  EXTMEM_CUSTOM_1D_1D_1D  = (ACCESS_MODE_TYPE | INSTRUCTION_1D | ADDRESS_1D | DATA_1D),      /*!< 1D instruction, 1D address, 1D data  */
  EXTMEM_CUSTOM_1S_1S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_4S),      /*!< 1S instruction, 1S address, 4S data  */
  EXTMEM_CUSTOM_1S_1S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1S | DATA_4D),      /*!< 1S instruction, 1S address, 4D data  */
  EXTMEM_CUSTOM_1S_1D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_4S),      /*!< 1S instruction, 1D address, 4S data  */
  EXTMEM_CUSTOM_1S_1D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_1D | DATA_4D),      /*!< 1S instruction, 1D address, 4D data  */
  EXTMEM_CUSTOM_1S_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4S | DATA_4S),      /*!< 1S instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_1S_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4S | DATA_4D),      /*!< 1S instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_1S_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4D | DATA_4S),      /*!< 1S instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_1S_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_1S | ADDRESS_4D | DATA_4D),      /*!< 1S instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_4S_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4S | DATA_4S),      /*!< 4S instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_4S_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4S | DATA_4D),      /*!< 4S instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_4S_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4D | DATA_4S),      /*!< 4S instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_4S_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4S | ADDRESS_4D | DATA_4D),      /*!< 4S instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_4D_4S_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4S | DATA_4S),      /*!< 4D instruction, 4S address, 4S data  */
  EXTMEM_CUSTOM_4D_4S_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4S | DATA_4D),      /*!< 4D instruction, 4S address, 4D data  */
  EXTMEM_CUSTOM_4D_4D_4S  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4D | DATA_4S),      /*!< 4D instruction, 4D address, 4S data  */
  EXTMEM_CUSTOM_4D_4D_4D  = (ACCESS_MODE_TYPE | INSTRUCTION_4D | ADDRESS_4D | DATA_4D),      /*!< 4D instruction, 4D address, 4D data  */
  EXTMEM_CUSTOM_8S_8S_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8S | DATA_8S),      /*!< 8S instruction, 8S address, 8S data  */
  EXTMEM_CUSTOM_8S_8S_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8S | DATA_8D),      /*!< 8S instruction, 8S address, 8D data  */
  EXTMEM_CUSTOM_8S_8D_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_8S),      /*!< 8S instruction, 8D address, 8S data  */
  EXTMEM_CUSTOM_8S_8D_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_8D),      /*!< 8S instruction, 8D address, 8D data  */
  EXTMEM_CUSTOM_8D_8S_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8S | DATA_8S),      /*!< 8D instruction, 8S address, 8S data  */
  EXTMEM_CUSTOM_8D_8S_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8S | DATA_8D),      /*!< 8D instruction, 8S address, 8D data  */
  EXTMEM_CUSTOM_8D_8D_8S  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_8S),      /*!< 8D instruction, 8D address, 8S data  */
  EXTMEM_CUSTOM_8D_8D_8D  = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_8D),      /*!< 8D instruction, 8D address, 8D data  */
#if defined (HAL_XSPI_DATA_16_LINES)
  EXTMEM_CUSTOM_8S_8D_16D = (ACCESS_MODE_TYPE | INSTRUCTION_8S | ADDRESS_8D | DATA_16D),      /*!< 8S instruction, 8D address, 16D data */
  EXTMEM_CUSTOM_8D_8D_16D = (ACCESS_MODE_TYPE | INSTRUCTION_8D | ADDRESS_8D | DATA_16D),      /*!< 8D instruction, 8D address, 16D data */
#endif /* HAL_XSPI_DATA_16_LINES */
} EXTMEM_CUSTOM_AccessModeTypeDef;

/**
  * @brief Enumeration of instruction bit sizes.
  * @note This information can be found in the "Command description" section
  *       (look for "Instruction Length" or "Command Length") of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_INSTRUCTION_8_BITS  = HAL_XSPI_INSTRUCTION_8_BITS,                /*!< 8-bit instruction  */
  EXTMEM_CUSTOM_INSTRUCTION_16_BITS = HAL_XSPI_INSTRUCTION_16_BITS,               /*!< 16-bit instruction */
  EXTMEM_CUSTOM_INSTRUCTION_24_BITS = HAL_XSPI_INSTRUCTION_24_BITS,               /*!< 24-bit instruction */
  EXTMEM_CUSTOM_INSTRUCTION_32_BITS = HAL_XSPI_INSTRUCTION_32_BITS                /*!< 32-bit instruction */
} EXTMEM_CUSTOM_InstruSizeTypeDef;

/**
  * @brief Enumeration of address bit sizes.
  * @note This information can be found in the "Command description" section
  *     (look for "Address Length" or "Address Byte") of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_ADDRESS_8_BITS  = HAL_XSPI_ADDRESS_8_BITS,                        /*!< 8-bit address  */
  EXTMEM_CUSTOM_ADDRESS_16_BITS = HAL_XSPI_ADDRESS_16_BITS,                       /*!< 16-bit address */
  EXTMEM_CUSTOM_ADDRESS_24_BITS = HAL_XSPI_ADDRESS_24_BITS,                       /*!< 24-bit address */
  EXTMEM_CUSTOM_ADDRESS_32_BITS = HAL_XSPI_ADDRESS_32_BITS                        /*!< 32-bit address */
} EXTMEM_CUSTOM_AddrSizeTypeDef;

/**
  * @brief Enumeration of DQS configurations.
  * @note This information can be found in the "Overview/Features" section
  * (look for "DQS Signal" or "Data Strobe") of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_DQS_DISABLE = HAL_XSPI_DQS_DISABLE,                               /*!< DQS disabled */
  EXTMEM_CUSTOM_DQS_ENABLE  = HAL_XSPI_DQS_ENABLE                                 /*!< DQS enabled  */
} EXTMEM_CUSTOM_DqsModeTypeDef;

/**
  * @brief Enumeration of wrap configurations for wrapped reads.
  * @note Wrap read support and boundaries are memory-dependent.
  */
typedef enum
{
  EXTMEM_CUSTOM_WRAP_NOT_SUPPORTED = HAL_XSPI_WRAP_NOT_SUPPORTED,                 /*!< wrapped reads are not supported by the memory   */
  EXTMEM_CUSTOM_WRAP_16_BYTES      = HAL_XSPI_WRAP_16_BYTES,                      /*!< external memory supports wrap size of 16 bytes  */
  EXTMEM_CUSTOM_WRAP_32_BYTES      = HAL_XSPI_WRAP_32_BYTES,                      /*!< external memory supports wrap size of 32 bytes  */
  EXTMEM_CUSTOM_WRAP_64_BYTES      = HAL_XSPI_WRAP_64_BYTES,                      /*!< external memory supports wrap size of 64 bytes  */
  EXTMEM_CUSTOM_WRAP_128_BYTES     = HAL_XSPI_WRAP_128_BYTES                      /*!< external memory supports wrap size of 128 bytes */
} EXTMEM_CUSTOM_WrapTypeDef;

/**
  * @brief Enumeration of chip-select boundary sizes.
  * @note Some external memories require de-asserting CS at specific address boundaries.
  */
typedef enum
{
  EXTMEM_CUSTOM_CS_BONDARYOF_NONE  = HAL_XSPI_BONDARYOF_NONE,                     /*!< CS boundary disabled         */
  EXTMEM_CUSTOM_CS_BONDARYOF_16B   = HAL_XSPI_BONDARYOF_16B,                      /*!<  16 bits  (  2  Bytes = 2^(1))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_32B   = HAL_XSPI_BONDARYOF_32B,                      /*!<  32 bits  (  4  Bytes = 2^(2))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_64B   = HAL_XSPI_BONDARYOF_64B,                      /*!<  64 bits  (  8  Bytes = 2^(3))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_128B  = HAL_XSPI_BONDARYOF_128B,                     /*!< 128 bits  ( 16  Bytes = 2^(4))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_256B  = HAL_XSPI_BONDARYOF_256B,                     /*!< 256 bits  ( 32  Bytes = 2^(5))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_512B  = HAL_XSPI_BONDARYOF_512B,                     /*!< 512 bits  ( 64  Bytes = 2^(6))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_1KB   = HAL_XSPI_BONDARYOF_1KB,                      /*!<   1 Kbits (128  Bytes = 2^(7))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_2KB   = HAL_XSPI_BONDARYOF_2KB,                      /*!<   2 Kbits (256  Bytes = 2^(8))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_4KB   = HAL_XSPI_BONDARYOF_4KB,                      /*!<   4 Kbits (512  Bytes = 2^(9))  */
  EXTMEM_CUSTOM_CS_BONDARYOF_8KB   = HAL_XSPI_BONDARYOF_8KB,                      /*!<   8 Kbits (  1 KBytes = 2^(10)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_16KB  = HAL_XSPI_BONDARYOF_16KB,                     /*!<  16 Kbits (  2 KBytes = 2^(11)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_32KB  = HAL_XSPI_BONDARYOF_32KB,                     /*!<  32 Kbits (  4 KBytes = 2^(12)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_64KB  = HAL_XSPI_BONDARYOF_64KB,                     /*!<  64 Kbits (  8 KBytes = 2^(13)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_128KB = HAL_XSPI_BONDARYOF_128KB,                    /*!< 128 Kbits ( 16 KBytes = 2^(14)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_256KB = HAL_XSPI_BONDARYOF_256KB,                    /*!< 256 Kbits ( 32 KBytes = 2^(15)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_512KB = HAL_XSPI_BONDARYOF_512KB,                    /*!< 512 Kbits ( 64 KBytes = 2^(16)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_1MB   = HAL_XSPI_BONDARYOF_1MB,                      /*!<   1 Mbits (128 KBytes = 2^(17)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_2MB   = HAL_XSPI_BONDARYOF_2MB,                      /*!<   2 Mbits (256 KBytes = 2^(18)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_4MB   = HAL_XSPI_BONDARYOF_4MB,                      /*!<   4 Mbits (512 KBytes = 2^(19)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_8MB   = HAL_XSPI_BONDARYOF_8MB,                      /*!<   8 Mbits (  1 MBytes = 2^(20)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_16MB  = HAL_XSPI_BONDARYOF_16MB,                     /*!<  16 Mbits (  2 MBytes = 2^(21)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_32MB  = HAL_XSPI_BONDARYOF_32MB,                     /*!<  32 Mbits (  4 MBytes = 2^(22)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_64MB  = HAL_XSPI_BONDARYOF_64MB,                     /*!<  64 Mbits (  8 MBytes = 2^(23)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_128MB = HAL_XSPI_BONDARYOF_128MB,                    /*!< 128 Mbits ( 16 MBytes = 2^(24)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_256MB = HAL_XSPI_BONDARYOF_256MB,                    /*!< 256 Mbits ( 32 MBytes = 2^(25)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_512MB = HAL_XSPI_BONDARYOF_512MB,                    /*!< 512 Mbits ( 64 MBytes = 2^(26)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_1GB   = HAL_XSPI_BONDARYOF_1GB,                      /*!<   1 Gbits (128 MBytes = 2^(27)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_2GB   = HAL_XSPI_BONDARYOF_2GB,                      /*!<   2 Gbits (256 MBytes = 2^(28)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_4GB   = HAL_XSPI_BONDARYOF_4GB,                      /*!<   4 Gbits (512 MBytes = 2^(29)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_8GB   = HAL_XSPI_BONDARYOF_8GB,                      /*!<   8 Gbits (  1 GBytes = 2^(30)) */
  EXTMEM_CUSTOM_CS_BONDARYOF_16GB  = HAL_XSPI_BONDARYOF_16GB                      /*!<  16 Gbits (  2 GBytes = 2^(31)) */
} EXTMEM_CUSTOM_CsBoundaryTypeDef;

/**
  * @brief Enumeration of Hyperbus Write Zero Latency Activation
  */
typedef enum
{
  EXTMEM_CUSTOM_WR_LATENCY_ON  = HAL_XSPI_LATENCY_ON_WRITE,                       /*!< HyperBus Latency on write accesses    */
  EXTMEM_CUSTOM_WR_LATENCY_OFF = HAL_XSPI_NO_LATENCY_ON_WRITE                     /*!< HyperBus No latency on write accesses */
} EXTMEM_CUSTOM_WrLatencyTypeDef;

/**
  * @brief Enumeration of Hyperbus Latency Mode
  */
typedef enum
{
  EXTMEM_CUSTOM_VARIABLE_LATENCY = HAL_XSPI_VARIABLE_LATENCY,                     /*!< HyperBus Variable initial latency */
  EXTMEM_CUSTOM_FIXED_LATENCY    = HAL_XSPI_FIXED_LATENCY                         /*!< HyperBus Fixed latency            */
} EXTMEM_CUSTOM_LatencyModeTypeDef;

/**
  * @brief Enumeration of supported memory types.
  * @note This information depends on the memory type used, this information can be found in the "Overview/Features"
  *       section of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_HYPERRAM     = (CUSTOM_HYPERRAM  | HYPERBUS_TYPE_MASK | PSRAM_TYPE_MASK), /*!< HyperRAM memory type   */
  EXTMEM_CUSTOM_NOR_FLASH    = (CUSTOM_NOR_FLASH  | FLASH_TYPE_MASK),                     /*!< NOR Flash memory type  */
  EXTMEM_CUSTOM_PSRAM        = (CUSTOM_PSRAM     | PSRAM_TYPE_MASK)                       /*!< PSRAM memory type      */
} EXTMEM_CUSTOM_MemTypeTypeDef;

/**
  * @brief Enumeration of Sample Shift configurations.
  * @note This information can be found in the "Timing Characteristics" section of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_SSHIFT_CFG_UNDEFINED = CUSTOM_SETTING_CFG_UNDEFINED,                                 /*!< Sample Shift setting not provided in configuration file */
  EXTMEM_CUSTOM_SSHIFT_CFG_NONE      = CUSTOM_SETTING_CFG_DEFINED | HAL_XSPI_SAMPLE_SHIFT_NONE,      /*!< Sample Shift setting provided in configuration file, and to be set to None */
  EXTMEM_CUSTOM_SSHIFT_CFG_HALFCYCLE = CUSTOM_SETTING_CFG_DEFINED | HAL_XSPI_SAMPLE_SHIFT_HALFCYCLE  /*!< Sample Shift setting provided in configuration file, and to be set to Half-cycle */
} EXTMEM_CUSTOM_SShiftCfgTypeDef;

/**
  * @brief Enumeration of supported memory standards.
  * @note This setting selects the vendor/standard behavior used by the controller. It mainly impacts
  *       Octal-SPI DDR/DTR transfers (8-data-bit mode): data bit ordering and DQS usage.
  * @note Modes overview:
  *       - Micron mode: D0/D1 ordering in DTR 8-data-bit mode, regular-command protocol in 1/2/4/8-SPI modes.
  *         In this mode, DQS signal polarity is inverted with respect to the memory clock signal.
  *       - Macronix mode: D1/D0 ordering in DTR 8-data-bit mode, regular-command protocol in 1/2/4/8-SPI modes.
  *       - Macronix RAM mode: D1/D0 ordering in DTR 8-data-bit mode, regular-command protocol in 1/2/4/8-SPI modes,
  *         with dedicated address mapping (address is built with row and column to fit with Macronix requirements).
  *       - HyperBus mode: protocol follows the HyperBus specification. Supports memory mode and register mode
  *         (addressing register space).
  * @note EXTMEM_CUSTOM_STD_MICRON is the default value; ensure the selected standard matches the connected memory.
  */
typedef enum
{
  EXTMEM_CUSTOM_STD_MICRON       = HAL_XSPI_MEMTYPE_MICRON,                       /*!< Micron memory standard         */
  EXTMEM_CUSTOM_STD_MACRONIX     = HAL_XSPI_MEMTYPE_MACRONIX,                     /*!< Macronix memory standard       */
  EXTMEM_CUSTOM_STD_APMEM        = HAL_XSPI_MEMTYPE_APMEM,                        /*!< AP Memory standard             */
  EXTMEM_CUSTOM_STD_MACRONIX_RAM = HAL_XSPI_MEMTYPE_MACRONIX_RAM,                 /*!< Macronix RAM memory standard   */
  EXTMEM_CUSTOM_STD_HYPERBUS     = HAL_XSPI_MEMTYPE_HYPERBUS,                     /*!< HyperBus memory standard       */
  EXTMEM_CUSTOM_STD_APMEM_16BITS = HAL_XSPI_MEMTYPE_APMEM_16BITS                  /*!< AP Memory 16-bit standard      */
} EXTMEM_CUSTOM_MemStdTypeDef;

/**
  * @brief Enumeration of possible configuration step types for memory operations.
  * @note Register configuration can be an RWConfigStep or an INSConfigStep structure,
  *       depending on the ConfigStepType.
  * @note In these structures, some fields are applicable while others are not. The comment
  *       for each ConfigStepType describes the list of required fields.
  * @note For RWConfigStep, the fields available for configuration include:
  *       - RegisterConfig[n].RWConfigStep.CommandRegisterWrite: Command to write a register.
  *       - RegisterConfig[n].RWConfigStep.CommandRegisterRead: Command to read a register.
  *       - RegisterConfig[n].RWConfigStep.CommandRegisterReadAW: Command to read a register (after write operation).
  *       - RegisterConfig[n].RWConfigStep.RegisterAddress: Address of the register.
  *       - RegisterConfig[n].RWConfigStep.RegisterValue: Value to write inside register (masked with RegisterMask).
  *       - RegisterConfig[n].RWConfigStep.RegisterMask: Mask applied to the value before writing to the register.
  * @note In case of 8, 16 or 24 bits read or write operations, the RegisterValue and RegisterMask
  *       fields must contain the full value (8, 16 or 24 bits) to be written or the full mask (8, 16 or 24 bits).
  *       Coding of value and mask is as follows:
  *       - For 8 bits operations: value/mask in bits [7:0], bits [31:8] must be set to 0.
  *       - For 16 bits operations: value/mask in bits [15:0], bits [31:16] must be set to 0.
  *         bits [15:8] correspond to the first byte transmitted, bits [7:0] correspond to the second byte transmitted.
  *       - For 24 bits operations: value/mask in bits [23:0], bits [31:24] must be set to 0.
  *         bits [23:16] correspond to the first byte transmitted, bits [15:8] correspond to the second byte
  *         transmitted and bits [7:0] correspond to the third byte transmitted.
  * @note For INSConfigStep, the fields available for configuration include:
  *       - RegisterConfig[n].INSConfigStep.Instruction: Instruction sent to the memory.
  */
typedef enum
{
  EXTMEM_CUSTOM_CFGSTEP_UNDEFINED          = 0,                                                            /*!< Configuration step not defined                             */
  EXTMEM_CUSTOM_CFGSTEP_RW_REG_ADDR        = (CUSTOM_CFGSTEP_RW_REGISTER | CFGSTEP_ADDRESS),               /*!< Read then Write register operation with address.           Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, RegisterAddress,       RegisterValue,   RegisterMask                */
  EXTMEM_CUSTOM_CFGSTEP_RW_REG             = (CUSTOM_CFGSTEP_RW_REGISTER),                                 /*!< Read then Write register operation (no address)            Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, RegisterValue,         RegisterMask                                 */
  EXTMEM_CUSTOM_CFGSTEP_RWR_REG_ADDR       = (CUSTOM_CFGSTEP_RWR_REGISTER | CFGSTEP_ADDRESS),              /*!< Read, Write then Read register operation with address      Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, CommandRegisterReadAW, RegisterAddress, RegisterValue, RegisterMask */
  EXTMEM_CUSTOM_CFGSTEP_RWR_REG            = (CUSTOM_CFGSTEP_RWR_REGISTER),                                /*!< Read, Write then Read register operation (no address)      Mandatory fields: CommandRegisterWrite,  CommandRegisterRead, CommandRegisterReadAW, RegisterValue,   RegisterMask                */
  EXTMEM_CUSTOM_CFGSTEP_R_REG_ADDR         = (CUSTOM_CFGSTEP_R_REGISTER | CFGSTEP_ADDRESS),                /*!< Read register operation with address                       Mandatory fields: CommandRegisterRead,   RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_R_REG              = (CUSTOM_CFGSTEP_R_REGISTER),                                  /*!< Read register operation (no address)                       Mandatory fields: CommandRegisterRead,                                                                                            */
  EXTMEM_CUSTOM_CFGSTEP_W_REG_ADDR         = (CUSTOM_CFGSTEP_W_REGISTER | CFGSTEP_ADDRESS),                /*!< Write register operation with address                      Mandatory fields: CommandRegisterWrite,  RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_W_REG              = (CUSTOM_CFGSTEP_W_REGISTER),                                  /*!< Write register operation (no address)                      Mandatory fields: CommandRegisterWrite                                                                                            */
  EXTMEM_CUSTOM_CFGSTEP_RAW_REG_ADDR       = (CUSTOM_CFGSTEP_RAW_REGISTER | CFGSTEP_ADDRESS),              /*!< Read register after a Write (wait WIP) with address        Mandatory fields: CommandRegisterReadAW, RegisterAddress                                                                          */
  EXTMEM_CUSTOM_CFGSTEP_RAW_REG            = (CUSTOM_CFGSTEP_RAW_REGISTER),                                /*!< Read register after a Write (wait WIP) (no address)        Mandatory fields: CommandRegisterReadAW                                                                                           */
  EXTMEM_CUSTOM_CFGSTEP_RR_REG_ADDR        = (CUSTOM_CFGSTEP_RR_REGISTER | CFGSTEP_ADDRESS),               /*!< Read 2 bytes (or 16 bits) register operation with address  Mandatory fields: CommandRegisterRead,   RegisterAddress, RegisterValue, RegisterMask                                             */
  EXTMEM_CUSTOM_CFGSTEP_RR_REG             = (CUSTOM_CFGSTEP_RR_REGISTER),                                 /*!< Read 2 bytes (or 16 bits) register operation (no address)  Mandatory fields: CommandRegisterRead,   RegisterValue,   RegisterMask                                                            */
  EXTMEM_CUSTOM_CFGSTEP_WW_REG_ADDR        = (CUSTOM_CFGSTEP_WW_REGISTER | CFGSTEP_ADDRESS),               /*!< Write 2 bytes (or 16 bits) register operation with address Mandatory fields: CommandRegisterWrite,  RegisterAddress, RegisterValue, RegisterMask                                             */
  EXTMEM_CUSTOM_CFGSTEP_WW_REG             = (CUSTOM_CFGSTEP_WW_REGISTER),                                 /*!< Write 2 bytes (or 16 bits) register operation (no address) Mandatory fields: CommandRegisterWrite,  RegisterValue,   RegisterMask                                                            */
  EXTMEM_CUSTOM_CFGSTEP_RRR_REG_ADDR       = (CUSTOM_CFGSTEP_RRR_REGISTER | CFGSTEP_ADDRESS),              /*!< Read 3 bytes (or 24 bits) register operation with address  Mandatory fields: CommandRegisterRead,   RegisterAddress, RegisterValue, RegisterMask                                             */
  EXTMEM_CUSTOM_CFGSTEP_RRR_REG            = (CUSTOM_CFGSTEP_RRR_REGISTER),                                /*!< Read 3 bytes (or 24 bits) register operation (no address)  Mandatory fields: CommandRegisterRead,   RegisterValue,   RegisterMask                                                            */
  EXTMEM_CUSTOM_CFGSTEP_WWW_REG_ADDR       = (CUSTOM_CFGSTEP_WWW_REGISTER | CFGSTEP_ADDRESS),              /*!< Write 3 bytes (or 24bits) register operation with address  Mandatory fields: CommandRegisterWrite,  RegisterAddress, RegisterValue, RegisterMask                                             */
  EXTMEM_CUSTOM_CFGSTEP_WWW_REG            = (CUSTOM_CFGSTEP_WWW_REGISTER),                                /*!< Write 3 bytes (or 24bits) register operation (no address)  Mandatory fields: CommandRegisterWrite,  RegisterValue,   RegisterMask                                                            */
  EXTMEM_CUSTOM_CFGSTEP_INST               = (CUSTOM_CFGSTEP_BASIC_INSTRUCTION),                           /*!< Instruction operation (no address, no data)                Mandatory fields: Instruction                                                                                                     */
  EXTMEM_CUSTOM_CFGSTEP_USER_CALLBACK      = (CUSTOM_CFGSTEP_USER_CALLBACK),                               /*!< User callback operation                                    Mandatory fields: Callback                                                                                                        */
  EXTMEM_CUSTOM_CFGSTEP_HB_RWR_REG_ADDR    = (CUSTOM_CFGSTEP_HYPERBUS_RWR_REGISTER),                       /*!< HyperBus Rd, Wr then Rd register operation with address    Mandatory fields: RegisterAddress,       RegisterValue,   RegisterMask */
  EXTMEM_CUSTOM_CFGSTEP_HB_RW_REG_ADDR     = (CUSTOM_CFGSTEP_HYPERBUS_RW_REGISTER),                        /*!< HyperBus Read then Write register operation with address   Mandatory fields: RegisterAddress,       RegisterValue,   RegisterMask */
  EXTMEM_CUSTOM_CFGSTEP_HB_R_REG_ADDR      = (CUSTOM_CFGSTEP_HYPERBUS_R_REGISTER),                         /*!< HyperBus Read register operation with address              Mandatory fields: RegisterAddress */
  EXTMEM_CUSTOM_CFGSTEP_HB_W_REG_ADDR      = (CUSTOM_CFGSTEP_HYPERBUS_W_REGISTER),                         /*!< HyperBus Write register operation with address             Mandatory fields: RegisterAddress,       RegisterValue */
  EXTMEM_CUSTOM_CFGSTEP_EXEC_OPT_CFG       = (CUSTOM_CFGSTEP_EXEC_OPT_CFG)                                 /*!< Execute Optional configuration (New Freq, AccessMode, etc.)*/
} ConfigStepTypeDef;

/**
  * @brief Enumeration of possible reset methods defined in the configuration file.
  * @note This information can be found in the "Reset" or "Commands" section of the memory datasheet.
  */
typedef enum
{
  EXTMEM_CUSTOM_RESET_METHOD_UNDEFINED = 0,                                       /*!< Reset method is not used/defined in configuration file */
  EXTMEM_CUSTOM_RESET_METHOD_66_99,                                               /*!< Reset method using 66h and 99h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_6699_9966,                                           /*!< Reset method using 6699h and 9966h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_6666_9999,                                           /*!< Reset method using 6666h and 9999h instructions */
  EXTMEM_CUSTOM_RESET_METHOD_FF,                                                  /*!< Reset method using FFh instructions */
} EXTMEM_CUSTOM_ResetMethodTypeDef;

/**
  * @brief Enumeration of details describing access to register containing WIP and WEL bits.
  * @note Some memories require an explicit register address to access the status register bits.
  */
typedef enum
{
  EXTMEM_CUSTOM_SR_ACCESS_CFG_UNDEFINED = CUSTOM_SETTING_CFG_UNDEFINED,                        /*!< Setting undefined (no address required) */
  EXTMEM_CUSTOM_SR_ACCESS_ADD_0         = CUSTOM_SETTING_CFG_DEFINED | CUSTOM_SR_ACCESS_ADD_0, /*!< Address required (address value stored on bits 0-30) */
} EXTMEM_CUSTOM_SRAccessDetails;

/**
  * @brief Structure defining the base memory configuration.
  * @note Frequency information can be found in the "AC Characteristics" section (look for "Operating Frequency"
  *       or "Clock Frequency") of the memory datasheet.
  * @note CommandRead information can be found in the "Instruction Table" or "Command description" section
  *       (look for "Read Command" or "Read Data") of the memory datasheet.
  * @note CommandWrite information can be found in the "Instruction Table" section
  *       (look for "Write Command" or "Program Data") of the memory datasheet.
  * @note DummyCycleRead information can be found in the "Command description" section
  *       (look for "Read Dummy Cycles" or "Read Latency") of the memory datasheet.
  * @note DummyCycleWrite information can be found in the "Command description" section
  *       (look for "Write Dummy Cycles" or "Write Latency") of the memory datasheet.
  * @note DummyRegisterRead information can be found in the "Command description" section
  *       (look for "Register Read Dummy Cycles" or "Read Register latency") of the memory datasheet.
  */
typedef struct
{
  uint32_t                         Frequency;                                     /*!< Operating frequency */
  uint16_t                         CommandRead;                                   /*!< Command to read data */
  uint16_t                         CommandWrite;                                  /*!< Command to write data */
  uint16_t                         DummyCycleRead;                                /*!< Number of dummy cycles for read operations */
  uint16_t                         DummyCycleWrite;                               /*!< Number of dummy cycles for write operations */
  uint16_t                         DummyRegisterRead;                             /*!< Number of dummy cycles for Read Reg operations */
  EXTMEM_CUSTOM_AccessModeTypeDef  AccessMode;                                    /*!< Access mode (e.g., 1S4D4D) */
  EXTMEM_CUSTOM_AccessModeTypeDef  OptWriteAccessMode;                            /*!< Optional Access mode for Write operation (e.g., 1S4D4D) */
  EXTMEM_CUSTOM_InstruSizeTypeDef  InstructionSize;                               /*!< Size of the instruction */
  EXTMEM_CUSTOM_AddrSizeTypeDef    AddressSize;                                   /*!< Size of the address */
  EXTMEM_CUSTOM_DqsModeTypeDef     DqsMode;                                       /*!< Data strobe mode */
} MemoryBaseConfigTypeDef;


/**
  * @brief Structure defining optional/advanced memory configuration parameters.
  * @note Fields are interpreted by the driver depending on the selected memory standard and access mode.
  */
typedef struct
{
  uint32_t                         AlternateByteMode;                             /*!< Alternate byte mode (e.g., 1S, 4D) */
  uint32_t                         AlternateByteSize;                             /*!< Size of the alternate byte */
  uint32_t                         SampleShift;                                   /*!< 1/2 Cycle SDR */
  uint32_t                         Dhqc;                                          /*!< 1/4 Cycle (DTR) maybe STR */
} MemoryOptionalConfigTypeDef;

/**
  * @brief Structure defining base NORFlash memory configuration parameters.
  * @note Cmd_EraseSector information can be found in the "Instruction Table" or "Command description" section
  *       (look for "Sector Erase Command") of the memory datasheet.
  * @note Cmd_MassErase information can be found in the "Instruction Table" or  "Command description" section
  *       (look for "Chip Erase Command") of the memory datasheet.
  * @note Cmd_RDSR information can be found in the "Instruction Table" or "Command description" section
  *       (look for "Read Status Register Command") of the memory datasheet.
  * @note Cmd_WE information can be found in the "Instruction Table" or "Command description" section
  *       (look for "Write Enable Command") of the memory datasheet.
  * @note MatchMask_WEL information can be found in the "Status Register" section
  *       (look for "WEL bit" or "Write Enable Latch bit") of the memory datasheet.
  * @note MatchValue_WEL information can be found in the "Status Register" section
  *       (look for "WEL bit" or "Write Enable Latch bit") of the memory datasheet.
  * @note MatchMask_WIP information can be found in the "Status Register" section
  *       (look for "WIP bit" or "Write In Progress bit") of the memory datasheet.
  * @note MatchValue_WIP information can be found in the "Status Register" section
  *       (look for "WIP bit" or "Write In Progress bit") of the memory datasheet.
  */
typedef struct
{
  EXTMEM_CUSTOM_SRAccessDetails    SRAccessDetails;                               /*!< Details about how to access SR for checking WIP, WEL bits */
  uint16_t                         Cmd_EraseSector;                               /*!< Command to erase a sector */
  uint16_t                         Cmd_MassErase;                                 /*!< Command to perform mass erase */
  uint16_t                         Cmd_RDSR;                                      /*!< Command to read status register */
  uint16_t                         Cmd_WE;                                        /*!< Command to enable write operations */
  uint8_t                          MatchMask_WEL;                                 /*!< Mask for write enable latch */
  uint8_t                          MatchValue_WEL;                                /*!< Match Value for write enable latch */
  uint8_t                          MatchMask_WIP;                                 /*!< Mask for write in progress */
  uint8_t                          MatchValue_WIP;                                /*!< Match Value for write in progress */
} NorFlashBasConfigTypeDef;


/**
  * @brief Define the NOR configuration structure
  * @note PageSize information can be found in the "Overview/Features" section
  *       (look for "Page Size" or "Page buffer") of the memory datasheet.
  * @note SectorSize information can be found in the "Overview/Features" or "General description" section
  *       (look for "Sector Size") of the memory datasheet.
  * @note MaxSectorEraseTime information can be found in the "AC Characteristics" section
  *       (look for "Sector Erase Time" or "tSE") of the memory datasheet.
  * @note MaxChipEraseTime information can be found in the "AC Characteristics" section
  *       (look for "Chip Erase Time" or "tCE") of the memory datasheet.
  * @note OptionalConfigEnable indicates if the Optional configuration is used after boot (new RDSR/WE/Erase commands)
  *       and depends entirely on the chosen configuration
  */
typedef struct
{
  uint32_t                         PageSize;                                      /*!< Page Size (Max write) */
  uint32_t                         SectorSize;                                    /*!< Sector Size in bytes  */
  uint32_t                         MaxSectorEraseTime;                            /*!< Max time expected for completing a Sector Erase operation (in ms) */
  uint32_t                         MaxChipEraseTime;                              /*!< Max time expected for completing a Chip Erase operation (in ms) */
  NorFlashBasConfigTypeDef         Startup;                                       /*!< Config use after boot */
  uint8_t                          OptionalConfigEnable;                          /*!< Optional NorFlash config activation if different from boot (RDSR/WE/Erase commands)  */
  NorFlashBasConfigTypeDef         Optional;                                      /*!< Config use after memory setup if different from boot (RDSR/WE/Erase commands) */
} NorFlashConfigTypeDef;

/**
  * @brief Define the RAM configuration structure
  * @note WrapSize information can be found in the "Overview/Features" section
  *       (look for "Wrap Boundary" or "Wrap Size") of the memory datasheet.
  * @note CsBoundarySize information can be found in the "Overview/Features" section
  *       (look for "CS Boundary", "Burst boundary" or similar) of the memory datasheet.
  * @note Refresh information (if applicable) can be found in the "AC Characteristics" section.
  */
typedef struct
{
  EXTMEM_CUSTOM_WrapTypeDef        WrapSize;                                      /*!< Wrap size */
  EXTMEM_CUSTOM_CsBoundaryTypeDef  CsBoundarySize;                                /*!< Chip Select boundary size */
  uint32_t                         Refresh;                                       /*!< Refresh setting (units/controller-dependent). */
} RamConfigTypeDef;

/**
  * @brief Define the Hyperbus configuration structure
  * @note RwRecoveryTimeCycle information can be found in the "Timing Characteristics" section
  *       (look for "tRWREC" or "Read/Write Recovery Time") of the memory datasheet.
  * @note AccessTimeCycle information can be found in the "Timing Characteristics" section
  *       (look for "tACC" or "Access Time") of the memory datasheet.
  * @note WriteZeroLatency information can be found in the "Overview/Features" section
  *       (look for "Write Zero Latency") of the memory datasheet.
  * @note LatencyMode information can be found in the "Overview/Features" section
  *       (look for "Latency Mode", "Fixed Latency" or "Variable Latency") of the memory datasheet.
  */
typedef struct
{
  uint32_t                         RwRecoveryTimeCycle;                           /*!< Read/Write recovery time cycle */
  uint32_t                         AccessTimeCycle;                               /*!< Access time cycle */
  EXTMEM_CUSTOM_WrLatencyTypeDef   WriteZeroLatency;                              /*!< Write zero latency */
  EXTMEM_CUSTOM_LatencyModeTypeDef LatencyMode;                                   /*!< Latency mode */
} HyperbusConfigTypeDef;

/**
  * @brief Define the HyperRam configuration structure
  */
typedef struct
{
  HyperbusConfigTypeDef            HyperbusConfig;                                /*!<  HyperbusConfig */
  RamConfigTypeDef                 RamConfig;                                     /*!<  RamConfig */
} HyperRamConfigTypeDef;

/**
  * @brief Define the optional register configuration structure for RW register
  * @note In this structure, all fields can be found in the "Register description" section
  *       of the memory datasheet depending on your desired configuration.
  */
typedef struct
{
  uint32_t                         RegisterAddress;                               /*!< Address of the register */
  uint16_t                         CommandRegisterWrite;                          /*!< Command to write to a register */
  uint16_t                         CommandRegisterRead;                           /*!< Command to read from a register */
  uint16_t                         CommandRegisterReadAW;                         /*!< Command to read from a register after write operation (only used in RWR operation type) */
  uint32_t                         RegisterValue;                                 /*!< Value of the register */
  uint32_t                         RegisterExpectedValue;                         /*!< Value expected of the register. Used for comparison in RAW operation */
  uint32_t                         RegisterMask;                                  /*!< Mask for the register */
} RegAccess_TypeDef;

/**
  * @brief Define the optional register configuration structure for Additional Instruction sending
  * @note In this structure all fields can be found in the "Register description" section
  * of the memory datasheet depending on your desired configuration.
  */
typedef struct
{
  uint16_t                         Instruction;                                   /*!< Instruction to be sent */
} Inst_TypeDef;

/**
  * @brief User callback signature for EXTMEM_CUSTOM_CFGSTEP_USER_CALLBACK steps.
  * @note UserCtx is forwarded as-is to the callback.
  *       If you need access to driver context, set UserCtx to (void*)CustomObject when
  *       configuring the callback step, and cast it back to EXTMEM_DRIVER_CUSTOM_ObjectTypeDef *
  *       inside your callback.
  */
typedef EXTMEM_DRIVER_CUSTOM_StatusTypeDef(*EXTMEM_CUSTOM_UserCallbackTypeDef)(
  void *UserCtx,
  uint32_t UserArg0,
  uint32_t UserArg1);

/**
  * @brief Define the optional register configuration structure for User Callback.
  */
typedef struct
{
  EXTMEM_CUSTOM_UserCallbackTypeDef Callback;                                    /*!< User callback to execute */
  void                              *UserCtx;                                    /*!< Optional user context */
  uint32_t                          Arg0;                                        /*!< Optional argument 0 */
  uint32_t                          Arg1;                                        /*!< Optional argument 1 */
} UserCallback_TypeDef;

/**
  * @brief Define the optional register configuration and additional instructions structure
  * @note Register configuration can be an RWConfigStep or an INSConfigStep structure,
  *       depending on the ConfigStepType chosen.
  */
typedef struct
{
  ConfigStepTypeDef                ConfigStepType;                                /*!< Type of configuration step : Status Register access, Instruction, ...  */
  union
  {
    RegAccess_TypeDef              RWConfigStep;                                  /*!< Config Step of type Register Access (R, W, RW, ...) */
    Inst_TypeDef                   INSConfigStep;                                 /*!< Config Step of type Instruction */
    UserCallback_TypeDef           CBConfigStep;                                  /*!< Config Step of type User callback */
  };
} OptionalRegisterConfigTypeDef;

/**
  * @brief CUSTOM Driver JedecID Info structure definition
  */
typedef struct
{
  uint8_t                          ManufacturerID;                                /*!< JedecID (Manufacturer ID) */
  uint8_t                          MemoryType;                                    /*!< JedecID (Memory type)     */
  uint8_t                          MemoryDensity;                                 /*!< JedecID (Memory density)  */
} EXTMEM_CUSTOM_JedecIDTypeDef;

/**
  * @brief CUSTOM Driver HyperRamID Info structure definition
  */
typedef struct
{
  uint8_t                          RowAddrBitCount;                               /*!< ID Register 0 [12:8] (Row Address Bit Count)    */
  uint8_t                          ColumnAddrBitCount;                            /*!< ID Register 0 [7:4]  (Column Address Bit Count) */
  uint8_t                          Manufacturer;                                  /*!< ID Register 0 [3:0]  (Manufacturer)             */
  uint8_t                          DeviceType;                                    /*!< ID Register 1 [3:0]  (Device Type)              */
} EXTMEM_CUSTOM_DeviceIDTypeDef;

/**
  * @brief Define the private/runtime configuration structure.
  * @note This structure is maintained by the custom driver and is not intended to be modified by the user.
  */
typedef struct
{
  XSPI_HandleTypeDef               *Handle;                                       /*!< Pointer to the XSPI instance handle. */
  union
  {
    XSPI_RegularCmdTypeDef         Commandbase;                                   /*!< Command base configuration. */
    XSPI_HyperbusCmdTypeDef        CommandbaseHyperbus;                           /*!< HyperBus Command base configuration. */
  };
  uint32_t                         RealClock;                                     /*!< Actual clock used by the memory. */
  uint32_t                         ReadRegValue;                                  /*!< Value read from register. */
  NorFlashBasConfigTypeDef         *CurrentFlashConfig;                           /*!< Pointer to the current Flash configuration. */
  union
  {
    EXTMEM_CUSTOM_JedecIDTypeDef     JedecID;                                     /*!< JEDEC ID retrieved from memory. (For NOR Flash) */
    EXTMEM_CUSTOM_DeviceIDTypeDef    DeviceID;                                    /*!< DEVICE ID retrieved from memory.(For HyperRAM)  */
  };
} CustomPrivateTypeDef;

/**
  * @brief CUSTOM Driver object definition
  * @note ResetDelay can be found in the "Reset" or "AC characteristics" section of the memory datasheet.
  * @note MemChipSelectHighTimeCycle can be found in the "Timing Characteristics" section (look for "tCSH",
  *       "Chip Select High Time") of the memory datasheet.
  * @note NbRegisterConfig is the number of additional configuration commands or instructions to send
  *       to the memory in order to have the desired configuration.
  *       In order to define the desired configuration you must refer to the "Register description" section
  *       from the memory datasheet and count the number of steps needed to achieve your desired configuration.
  * @note Time-related fields are expressed either in milliseconds (ms) or in controller clock cycles,
  *       as specified by each field.
  */
typedef struct
{
  CustomPrivateTypeDef             Private;                                       /*!< Private driver data. */
  EXTMEM_CUSTOM_MemTypeTypeDef     MemType;                                       /*!< Memory type. */
  EXTMEM_CUSTOM_MemStdTypeDef      MemStandard;                                   /*!< Memory standard (e.g., Macronix, Micron, etc.). */
  EXTMEM_CUSTOM_MemSizeTypeDef     MemSize;                                       /*!< Memory size. */
  EXTMEM_CUSTOM_ResetMethodTypeDef ResetMethod;                                   /*!< Reset method. */
  EXTMEM_CUSTOM_SShiftCfgTypeDef   SampleShiftCfg;                                /*!< Sample Shift configuration. */
  uint16_t                         ResetDelay;                                    /*!< Time needed after reset (in ms, 0 if no required delay). */
  uint16_t                         MemChipSelectHighTimeCycle;                    /*!< Chip select high time. */
  MemoryBaseConfigTypeDef          StartupConfig;                                 /*!< Startup configuration. */
  uint16_t                         NbRegisterConfig;                              /*!< Number of additional configuration commands or instructions to send to the memory. */
  OptionalRegisterConfigTypeDef    RegisterConfig[EXTMEM_CUSTOM_MAX_REG_CONFIG];  /*!< Additional configuration commands or instructions to send to the memory. */
  MemoryBaseConfigTypeDef          OptionalConfig;                                /*!< Optional configuration. */
  union
  {
    NorFlashConfigTypeDef          NorFlashConfig;                                /*!< NOR Flash configuration             */
    RamConfigTypeDef               PsramConfig;                                   /*!< PSRAM configuration                 */
    HyperRamConfigTypeDef          HyperRamConfig;                                /*!< HyperRam configuration              */
  };
} EXTMEM_DRIVER_CUSTOM_ObjectTypeDef;

/**
  * @brief CUSTOM Driver Memory Info structure definition
  */
typedef struct
{
  EXTMEM_CUSTOM_MemTypeTypeDef     MemType;                                       /*!< Memory type. */
  EXTMEM_CUSTOM_MemSizeTypeDef     MemSize;                                       /*!< Memory size. */
  EXTMEM_CUSTOM_MemStdTypeDef      MemStandard;                                   /*!< Memory standard (e.g., Macronix, Micron, etc.). */
  EXTMEM_CUSTOM_AccessModeTypeDef  MemAccessMode;                                 /*!< Access mode (e.g., 1S4D4D). */
  uint32_t                         MemRealFreq;                                   /*!< Actual operating frequency. */
  EXTMEM_CUSTOM_JedecIDTypeDef     JedecID;                                       /*!< JEDEC ID retrieved from memory. (For NOR Flash) */
  EXTMEM_CUSTOM_DeviceIDTypeDef    DeviceID;                                      /*!< DEVICE ID retrieved from memory.(For HyperRAM)  */
} EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef;
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

#endif /* __STM32_CUSTOM_TYPE_H */

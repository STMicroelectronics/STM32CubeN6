/**
  ******************************************************************************
  * @file    stm32_psram_driver_type.h
  * @author  MCD Application Team
  * @brief   This file contains PSRAM driver type definitions.
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
#ifndef __STM32_PSRAM_TYPE_H
#define __STM32_PSRAM_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/** @addtogroup PSRAM
  * @ingroup EXTMEM_DRIVER
  * @{
  */

#if EXTMEM_DRIVER_PSRAM == 1

/* Exported constants --------------------------------------------------------*/
/** @defgroup PSRAM_Exported_constants Exported constants
  * @{
  */

/**
  * @brief PSRAM driver maximum number of commands
  */
#define PSRAM_MAX_COMMAND 3u

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/** @defgroup PSRAM_Exported_Types Exported Types
  * @{
  */

/**
  * @brief PSRAM driver object definition
  */
typedef struct
{
  struct
  {
    SAL_XSPI_ObjectTypeDef   SALObject;    /*!< SAL XSPI object. */
  } psram_private;                         /*!< Private data for the PSRAM driver object. */

  struct
  {
    uint32_t MemorySize;                   /*!< Memory size (see @ref XSPI_MemorySize). */
    uint32_t FreqMax;                      /*!< Maximum supported memory frequency. */

    /* Configuration */
    uint8_t NumberOfConfig;                /*!< Number of configuration steps; each step performs a read and write operation. */
    struct
    {
      uint16_t WriteMask;                  /*!< Mask for write operation.
                                                ReadREGSize==1: only low 8 bits are used.
                                                ReadREGSize==2:
                                                - Default mapping (16-bit): MSB applies to first byte (buffer[0]),
                                                                            LSB to second (buffer[1]).
                                                - Backward compatibility: if upper byte of mask AND value are 0x00,
                                                                          the low 8 bits apply to buffer[0] only. */
      uint16_t WriteValue;                 /*!< Value to write.
                                                ReadREGSize==1: only low 8 bits are used.
                                                ReadREGSize==2:
                                                - Default mapping (16-bit): MSB applies to first byte (buffer[0]),
                                                                            LSB to second (buffer[1]).
                                                - Backward compatibility: if upper byte of mask AND value are 0x00,
                                                                          the low 8 bits apply to buffer[0] only. */
      uint8_t REGAddress;                  /*!< Address of the register. */
    } config[PSRAM_MAX_COMMAND];

    /* Command REG */
    uint8_t ReadREG;
    uint8_t WriteREG;
    uint8_t ReadREGSize;
    uint8_t REG_DummyCycle;

    /* Command Read and Write */
    uint8_t Write_command;
    uint8_t Write_DummyCycle;
    uint8_t Read_command;
    uint8_t WrapRead_command;
    uint8_t Read_DummyCycle;
  } psram_public;                          /*!< Public data for the PSRAM driver object. */
} EXTMEM_DRIVER_PSRAM_ObjectTypeDef;

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */

#ifdef __cplusplus
}
#endif

#endif /* __STM32_PSRAM_TYPE_H */

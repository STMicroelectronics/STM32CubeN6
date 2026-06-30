/**
  ******************************************************************************
  * @file    stm32_psram_driver.c
  * @author  MCD Application Team
  * @brief   This file implements the PSRAM driver.
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

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"
#if EXTMEM_DRIVER_PSRAM == 1
#if EXTMEM_SAL_XSPI == 1
#include "../sal/stm32_sal_xspi_api.h"
#else
#error "the driver PSRAM requires the enable of EXTMEM_SAL_XSPI"
#endif /* EXTMEM_SAL_XSPI */
#include "stm32_psram_driver_api.h"
#if EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && EXTMEM_MACRO_DEBUG */

/** @defgroup PSRAM PSRAM driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */

/* Private Macro ------------------------------------------------------------*/
/** @defgroup PSRAM_Private_Macro Private Macro
  * @{
  */

/**
  * @brief Default Timeout value (ms)
  */
#define DRIVER_DEFAULT_TIMEOUT 300

#if EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG)
/**
  * @brief Debug macro for a string
  */
#define DEBUG_STR(_STR_)  {                      \
                            EXTMEM_MACRO_DEBUG("\tPSRAM::");  \
                            EXTMEM_MACRO_DEBUG(_STR_);        \
                            EXTMEM_MACRO_DEBUG("\n\r");       \
                            }
#else
/**
  * @brief Debug macro for a string
  */
#define DEBUG_STR(_STR_)
#endif /* EXTMEM_DRIVER_PSRAM_DEBUG_LEVEL > 0 && defined(EXTMEM_MACRO_DEBUG) */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup PSRAM_Private_Functions Private Functions
  * @{
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef PSRAM_ExecuteCommand(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject, uint8_t Index);
/**
  * @}
  */

/** @defgroup PSRAM_Exported_Functions Exported Functions
  * @{
  */

/**
  * @brief Initializes the PSRAM driver.
  * @param Peripheral Pointer to the peripheral handle.
  * @param Config Link configuration.
  * @param ClockInput Input clock frequency.
  * @param PsramObject Pointer to the PSRAM driver object.
  * @retval @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Init(void *Peripheral, EXTMEM_LinkConfig_TypeDef Config,
                                                           uint32_t ClockInput,
                                                           EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;
  uint32_t ClockOut;
  SAL_XSPI_PhysicalLinkTypeDef linkvalue;

  /* Initialize the instance */
  DEBUG_STR("initialize the instance")

  /* Initialize XSPI low layer */
  (void)SAL_XSPI_Init(&PsramObject->psram_private.SALObject, Peripheral);

  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject);

  /* Set the frequency prescaler */
  DEBUG_STR("set memory speed according freqIn and freqMax supported by the memory")
  if (HAL_OK != SAL_XSPI_SetClock(&PsramObject->psram_private.SALObject, ClockInput, PsramObject->psram_public.FreqMax,
                                  &ClockOut))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR;
    goto error;
  }

  /* Set the memory size */
  DEBUG_STR("set memory size according")
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_FLASHSIZE,
                              &PsramObject->psram_public.MemorySize);

  /* Set the memory size */
  DEBUG_STR("set xspi link config")
  linkvalue = PHY_LINK_RAM8;
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_PHY_LINK, &linkvalue);

  /* Set the configuration to perform register operation */
  (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_DUMMY_CYCLES,
                              &PsramObject->psram_public.REG_DummyCycle);

  /* Execute the command sequence */
  for (uint8_t command_index = 0u; command_index < PsramObject->psram_public.NumberOfConfig; command_index++)
  {
    retr = PSRAM_ExecuteCommand(PsramObject, command_index);
    if (retr != EXTMEM_DRIVER_PSRAM_OK)
    {
      goto error;
    }
  }

  switch (Config)
  {
#if defined (HAL_XSPI_DATA_16_LINES)
    case EXTMEM_LINK_CONFIG_16LINES:
      linkvalue = PHY_LINK_RAM16;
      (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_PHY_LINK, &linkvalue);
      break;
#endif /* HAL_XSPI_DATA_16_LINES */
    case EXTMEM_LINK_CONFIG_8LINES:
      linkvalue = PHY_LINK_RAM8;
      (void)SAL_XSPI_MemoryConfig(&PsramObject->psram_private.SALObject, PARAM_PHY_LINK, &linkvalue);
      break;

    default:
      retr = EXTMEM_DRIVER_PSRAM_ERROR;
      goto error;
      break;
  }

error:
  return retr;
}

/**
  * @brief Deinitializes the PSRAM driver.
  * @param PsramObject Pointer to the PSRAM driver object.
  * @retval @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_DeInit(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject)
{
  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject);
  return EXTMEM_DRIVER_PSRAM_OK;
}

/**
  * @brief Enables memory-mapped mode for the PSRAM device.
  * @param PsramObject Pointer to the PSRAM driver object.
  * @retval @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Enable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef
                                                                              *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;

  /* Configure the read wrap mode */
  if (HAL_OK != SAL_XSPI_ConfigureWrappMode(&PsramObject->psram_private.SALObject,
                                            PsramObject->psram_public.WrapRead_command,
                                            PsramObject->psram_public.Write_DummyCycle))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_ENABLE;
  }

  /* Enable memory mapped mode */
  if (HAL_OK != SAL_XSPI_EnableMapMode(&PsramObject->psram_private.SALObject,
                                       PsramObject->psram_public.Read_command,
                                       PsramObject->psram_public.Read_DummyCycle,
                                       PsramObject->psram_public.Write_command,
                                       PsramObject->psram_public.Write_DummyCycle))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_ENABLE;
  }
  return retr;
}

/**
  * @brief Disables memory-mapped mode for the PSRAM device.
  * @param PsramObject Pointer to the PSRAM driver object.
  * @retval @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef EXTMEM_DRIVER_PSRAM_Disable_MemoryMappedMode(EXTMEM_DRIVER_PSRAM_ObjectTypeDef
                                                                               *PsramObject)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;

  /* Disable memory mapped mode */
  if (HAL_OK != SAL_XSPI_DisableMapMode(&PsramObject->psram_private.SALObject))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_MAP_DISABLE;
  }
  return retr;
}

/**
  * @}
  */

/** @addtogroup PSRAM_Private_Functions
  * @{
  */

/**
  * @brief Executes a configuration command for the PSRAM device.
  *
  * @param PsramObject Pointer to the PSRAM driver object.
  * @param Index Index of the command to execute.
  * @retval @ref EXTMEM_DRIVER_PSRAM_StatusTypeDef
  */
EXTMEM_DRIVER_PSRAM_StatusTypeDef PSRAM_ExecuteCommand(EXTMEM_DRIVER_PSRAM_ObjectTypeDef *PsramObject, uint8_t Index)
{
  EXTMEM_DRIVER_PSRAM_StatusTypeDef retr = EXTMEM_DRIVER_PSRAM_OK;
  uint8_t regval[2];
  const uint16_t mask16  = PsramObject->psram_public.config[Index].WriteMask;
  const uint16_t value16 = PsramObject->psram_public.config[Index].WriteValue;

  if (PsramObject->psram_public.ReadREGSize > 2u)
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_REGSIZE;
    goto error;
  }

  /* Optimization:
   * - If the mask fully overwrites the register, the read-back value has no effect.
   * - For 8-bit register access (ReadREGSize==1), a 0x00FF legacy mask becomes 0xFF after cast.
   */
  if ((PsramObject->psram_public.ReadREGSize == 1u) && (((uint8_t)mask16) == 0xFFu))
  {
    regval[0] = (uint8_t)value16;
  }
  else if ((PsramObject->psram_public.ReadREGSize == 2u) && (mask16 == 0xFFFFu))
  {
    /* Wire order: MSB first. */
    regval[0] = (uint8_t)((value16 >> 8) & 0x00FFu);
    regval[1] = (uint8_t)(value16 & 0x00FFu);
  }
  else
  {
    if (HAL_OK != SAL_XSPI_Read(&PsramObject->psram_private.SALObject,
                                PsramObject->psram_public.ReadREG,
                                PsramObject->psram_public.config[Index].REGAddress,
                                regval, PsramObject->psram_public.ReadREGSize))
    {
      retr = EXTMEM_DRIVER_PSRAM_ERROR_READREG;
      goto error;
    }

    if (PsramObject->psram_public.ReadREGSize == 1u)
    {
      /* 8-bit register: apply low 8-bit mask/value on the single returned byte. */
      MODIFY_REG(regval[0], (uint8_t)mask16, (uint8_t)value16);
    }
    else
    {
      /* Register access over 2 bytes:
       * - Legacy configs used 8-bit mask/value even when ReadREGSize==2 (protocol requires 2 bytes on the bus).
       * - New configs can use full 16-bit mask/value to update both bytes.
       */

      /* Backward compatibility:
       * If the upper byte of BOTH mask and value is 0x00, treat the provided low 8 bits as targeting
       * the first transmitted byte (regval[0]) only; regval[1] stays as-read.
       */
      if (((mask16 & 0xFF00u) == 0u) && ((value16 & 0xFF00u) == 0u))
      {
        MODIFY_REG(regval[0], (uint8_t)mask16, (uint8_t)value16);
      }
      else
      {
        /* 16-bit mode (wire order): MSB updates regval[0], LSB updates regval[1]. */
        const uint8_t mask0  = (uint8_t)((mask16 >> 8) & 0x00FFu);
        const uint8_t value0 = (uint8_t)((value16 >> 8) & 0x00FFu);
        const uint8_t mask1  = (uint8_t)(mask16 & 0x00FFu);
        const uint8_t value1 = (uint8_t)(value16 & 0x00FFu);

        MODIFY_REG(regval[0], mask0, value0);
        MODIFY_REG(regval[1], mask1, value1);
      }
    }
  }

  if (HAL_OK != SAL_XSPI_Write(&PsramObject->psram_private.SALObject,
                               PsramObject->psram_public.WriteREG,
                               PsramObject->psram_public.config[Index].REGAddress,
                               regval, PsramObject->psram_public.ReadREGSize))
  {
    retr = EXTMEM_DRIVER_PSRAM_ERROR_WRITEREG;
    goto error;
  }

error:
  return retr;
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_PSRAM == 1 */

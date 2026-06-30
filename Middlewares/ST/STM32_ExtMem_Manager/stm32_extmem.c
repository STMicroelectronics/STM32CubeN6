/**
  ******************************************************************************
  * @file    stm32_extmem.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for external memory support
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
/**
  * @brief Macro to get memory table definition
  */
#define EXTMEM_C

/* Includes ------------------------------------------------------------------*/
#include "stm32_extmem.h"
#include "stm32_extmem_conf.h"

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "nor_sfdp/stm32_sfdp_driver_api.h"
#endif /* EXTMEM_DRIVER_NOR_SFDP */

#if EXTMEM_DRIVER_PSRAM == 1
#include "psram/stm32_psram_driver_api.h"
#endif /* EXTMEM_DRIVER_PSRAM */

#if EXTMEM_DRIVER_SDCARD == 1
#include "sdcard/stm32_sdcard_driver_api.h"
#endif /* EXTMEM_DRIVER_SDCARD */

#if EXTMEM_DRIVER_CUSTOM == 1
#include "custom/stm32_custom_driver_api.h"
#endif /* EXTMEM_DRIVER_CUSTOM */

#if EXTMEM_DRIVER_USER == 1
#include "user/stm32_user_driver_api.h"
#endif /* EXTMEM_DRIVER_USER */

/**
  * @defgroup EXTMEM_DRIVER
  * @{
  */

/**
  * @}
 */

/**
  * @defgroup EXTMEM_SAL
  * @{
  */

/**
  * @}
 */

/**
  * @defgroup EXTMEM
  * @{
  */

/* Private Macros ------------------------------------------------------------*/
/**
  *  @defgroup EXTMEM_Private_Macros External Memory Private Macros
  * @{
  */
/**
  * @brief EXTMEM debug function
  */
/**
  * @brief Macro used to trace EXTMEM function calls
  */

#if defined(EXTMEM_DEBUG_LEVEL) && defined(EXTMEM_MACRO_DEBUG)
#if EXTMEM_DEBUG_LEVEL > 0
#define EXTMEM_DEBUG(_MSG_)  EXTMEM_MACRO_DEBUG(_MSG_)
#if EXTMEM_DEBUG_LEVEL > 1
#define EXTMEM_FUNC_CALL()  \
  do                        \
  {                         \
    EXTMEM_DEBUG(__func__); \
    EXTMEM_DEBUG("\n");     \
  } while (0);
#else
#define EXTMEM_FUNC_CALL()
#endif /* EXTMEM_DEBUG_LEVEL > 1 */
#else
#define EXTMEM_DEBUG(_MSG_)
#define EXTMEM_FUNC_CALL()
#endif /* EXTMEM_DEBUG_LEVEL > 0 */
#else
#define EXTMEM_DEBUG(_MSG_)
#define EXTMEM_FUNC_CALL()
#endif /* defined(EXTMEM_DEBUG_LEVEL) && defined(EXTMEM_MACRO_DEBUG) */

/**
  * @brief Macro used to trace EXTMEM debug information
  */
#define EXTMEM_DEBUG_ENDL(_STRING_) \
  do                                \
  {                                 \
    EXTMEM_DEBUG(_STRING_);         \
    EXTMEM_DEBUG("\n");             \
  } while (0);

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/* Exported variables ---------------------------------------------------------*/


/** @defgroup EXTMEM_Exported_Functions External Memory Exported Functions
  * @{
  */

/**
  * @brief Initializes the external memory.
  * @param MemId Memory identifier.
  * @param ClockInput Clock input value for the memory.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_Init(uint32_t MemId, uint32_t ClockInput)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL();

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Initialize the memory using NOR SFDP driver */
        if (EXTMEM_DRIVER_NOR_SFDP_OK != EXTMEM_DRIVER_NOR_SFDP_Init(extmem_list_config[MemId].Handle,
                                                                     extmem_list_config[MemId].ConfigType,
                                                                     ClockInput,
                                                                     &extmem_list_config[MemId].NorSfdpObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Initialize the memory using SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_Init(extmem_list_config[MemId].Handle,
                                                                 &extmem_list_config[MemId].SdCardObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        /* Initialize the memory using PSRAM driver */
        if (EXTMEM_DRIVER_PSRAM_OK != EXTMEM_DRIVER_PSRAM_Init(extmem_list_config[MemId].Handle,
                                                               extmem_list_config[MemId].ConfigType,
                                                               ClockInput,
                                                               &extmem_list_config[MemId].PsramObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Initialize the memory using CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_Init(extmem_list_config[MemId].Handle,
                                                              ClockInput,
                                                              &extmem_list_config[MemId].CustomObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Initialize the memory using USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status;
        status = EXTMEM_DRIVER_USER_Init(MemId,
                                         &extmem_list_config[MemId].UserObject);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
          {
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          }
          case EXTMEM_DRIVER_USER_OK:
          {
            /* Nothing to do */
            retr = EXTMEM_OK;
            break;
          }
          default:
          {
            retr = EXTMEM_ERROR_DRIVER;
            break;
          }
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Deinitializes the external memory.
  * @param MemId Memory identifier.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_DeInit(uint32_t MemId)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL();

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Uninitialize the memory using NOR SFDP driver */
        /* Return is always OK no need to test the returned value */
        (void)EXTMEM_DRIVER_NOR_SFDP_DeInit(&extmem_list_config[MemId].NorSfdpObject);
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Uninitialize the memory using SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_DeInit(&extmem_list_config[MemId].SdCardObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        /* Uninitialize the memory using PSRAM driver */
        /* Return is always OK no need to test the returned value */
        (void)EXTMEM_DRIVER_PSRAM_DeInit(&extmem_list_config[MemId].PsramObject);
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Uninitialize the memory using CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_DeInit(&extmem_list_config[MemId].CustomObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Uninitialize the memory using USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status = EXTMEM_DRIVER_USER_DeInit(&extmem_list_config[MemId].UserObject);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            /* Nothing to do, the returned status is already set to EXTMEM_OK */
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Reads data from the external memory.
  * @param MemId Memory identifier.
  * @param Address Address in memory to read from.
  * @param Data Pointer to buffer where read data will be stored.
  * @param Size Number of bytes to read.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_Read(uint32_t MemId, uint32_t Address, uint8_t *Data, uint32_t Size)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the Read operation using the NOR SFDP driver */
        if (EXTMEM_DRIVER_NOR_SFDP_OK != EXTMEM_DRIVER_NOR_SFDP_Read(&extmem_list_config[MemId].NorSfdpObject,
                                                                     Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Perform the Read operation using the SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_Read(&extmem_list_config[MemId].SdCardObject,
                                                                 Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the Read operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_Read(&extmem_list_config[MemId].CustomObject,
                                                              Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the Read operation using the USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status = EXTMEM_DRIVER_USER_Read(&extmem_list_config[MemId].UserObject,
                                                                          Address, Data, Size);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            /* Nothing to do, the returned status is already set to EXTMEM_OK */
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Writes data to the external memory.
  * @param MemId Memory identifier.
  * @param Address Address in memory to write to.
  * @param Data Pointer to buffer containing data to be written.
  * @param Size Number of bytes to write.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_Write(uint32_t MemId, uint32_t Address, const uint8_t *Data, uint32_t Size)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the Write operation using the NOR SFDP driver */
        if (EXTMEM_DRIVER_NOR_SFDP_OK != EXTMEM_DRIVER_NOR_SFDP_Write(&extmem_list_config[MemId].NorSfdpObject,
                                                                      Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Perform the Write operation using the SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_Write(&extmem_list_config[MemId].SdCardObject,
                                                                  Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the Write operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_Write(&extmem_list_config[MemId].CustomObject,
                                                               Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the Write operation using the USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status = EXTMEM_DRIVER_USER_Write(&extmem_list_config[MemId].UserObject,
                                                                           Address, Data, Size);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Writes data to the external memory in mapped mode.
  * @param MemId Memory identifier.
  * @param Address Address in memory to write to.
  * @param Data Pointer to buffer (must be 32-bit aligned) containing data to be written.
  * @param Size Number of bytes to write.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  *
  * @note This function performs write operations in mapped mode and requires specific processing due
  *       to write enable management when the memory is mapped. The operation splits the write into packets
  *       of page size, and for each packet, performs a write before entering mapped mode.
  *
  * @note To avoid issues during this process, it is mandatory that the targeted memory area has the following
  *       characteristics: the write region must be non-bufferable, non-shareable, and non-cacheable.
  */
EXTMEM_StatusTypeDef EXTMEM_WriteInMappedMode(uint32_t MemId, uint32_t Address, const uint8_t *const Data,
                                              uint32_t Size)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      /* Perform the Write in mapped mode operation using the NOR SFDP driver */
      case EXTMEM_NOR_SFDP:
      {
        if (EXTMEM_DRIVER_NOR_SFDP_OK !=
            EXTMEM_DRIVER_NOR_SFDP_WriteInMappedMode(&extmem_list_config[MemId].NorSfdpObject,
                                                     Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD :
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      /* Perform the Write in mapped mode operation using the CUSTOM driver */
      case EXTMEM_CUSTOM :
      {
        if (EXTMEM_DRV_CUSTOM_OK !=
            EXTMEM_DRIVER_CUSTOM_WriteInMappedMode(&extmem_list_config[MemId].CustomObject,
                                                   Address, Data, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Erases a number of sectors in the external memory.
  * @param MemId Memory identifier.
  * @param Address Address in memory to start erase operation.
  * @param Size Number of bytes to erase.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_EraseSector(uint32_t MemId, uint32_t Address, uint32_t Size)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the Sector Erase operation using the NOR SFDP driver */
        const EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *const object = &extmem_list_config[MemId].NorSfdpObject;
        EXTMEM_DRIVER_NOR_SFDP_SectorTypeTypeDef sector_type = EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE1;
        uint32_t local_address = Address;
        uint32_t local_size = Size;
        uint32_t sector_size = (uint32_t)1u << object->sfdp_private.DriverInfo.EraseType4Size;

        while (local_size != 0u)
        {
          /* if address is a modulo a sector size if sector  */
          if ((object->sfdp_private.DriverInfo.EraseType4Size != 0u)
              && (local_size >= sector_size)
              && ((local_address % sector_size) == 0u))
          {
            sector_type = EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE4;
          }
          else
          {
            sector_size = (uint32_t)1u << object->sfdp_private.DriverInfo.EraseType3Size;
            if ((object->sfdp_private.DriverInfo.EraseType3Size != 0u)
                && (local_size >= sector_size)
                && ((local_address % sector_size) == 0u))
            {
              sector_type = EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE3;
            }
            else
            {
              sector_size = (uint32_t)1u << object->sfdp_private.DriverInfo.EraseType2Size;
              if ((object->sfdp_private.DriverInfo.EraseType2Size != 0u)
                  && (local_size >= sector_size)
                  && ((local_address % sector_size) == 0u))
              {
                sector_type = EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE2;
              }
              else
              {
                sector_size = (uint32_t)1u << object->sfdp_private.DriverInfo.EraseType1Size;
                if ((object->sfdp_private.DriverInfo.EraseType1Size != 0u)
                    && ((local_address % sector_size) == 0u))
                {
                  sector_type = EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE1;
                }
                else
                {
                  retr = EXTMEM_ERROR_SECTOR_SIZE;
                }
              }
            }
          }

          if (retr == EXTMEM_OK)
          {
            if (EXTMEM_DRIVER_NOR_SFDP_OK !=
                EXTMEM_DRIVER_NOR_SFDP_SectorErase(&extmem_list_config[MemId].NorSfdpObject,
                                                   local_address, sector_type))
            {
              retr = EXTMEM_ERROR_DRIVER;
            }
          }

          if (retr != EXTMEM_OK)
          {
            local_size = 0u;
          }
          else
          {
            local_address = local_address + sector_size;
            if (sector_size > local_size)
            {
              local_size = 0u;
            }
            else
            {
              local_size = local_size - sector_size;
            }
          }
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Perform the Sector Erase operation using the SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_EraseBlock(&extmem_list_config[MemId].SdCardObject,
                                                                       Address, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the Sector Erase operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_EraseSector(&extmem_list_config[MemId].CustomObject,
                                                                     Address, Size))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the Sector Erase operation using the USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status = EXTMEM_DRIVER_USER_EraseSector(&extmem_list_config[MemId].UserObject,
                                                                                 Address, Size);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Erases all sectors in the external memory.
  * @param MemId Memory identifier.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_EraseAll(uint32_t MemId)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the Mass Erase operation using the NOR SFDP driver */
        if (EXTMEM_DRIVER_NOR_SFDP_OK != EXTMEM_DRIVER_NOR_SFDP_MassErase(&extmem_list_config[MemId].NorSfdpObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD:
      {
        /* Perform the Mass Erase operation using the SD CARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_Erase(&extmem_list_config[MemId].SdCardObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM:
      {
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the Mass Erase operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_MassErase(&extmem_list_config[MemId].CustomObject))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the Mass Erase operation using the USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status = EXTMEM_DRIVER_USER_MassErase(&extmem_list_config[MemId].UserObject);
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Returns information about the external memory.
  * @param MemId Memory identifier.
  * @param Info Pointer to structure to receive memory information.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_GetInfo(uint32_t MemId, void *Info)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL()

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the Get Information operation using the NOR SFDP driver */
        EXTMEM_DRIVER_NOR_SFDP_GetFlashInfo(&extmem_list_config[MemId].NorSfdpObject,
                                            (EXTMEM_NOR_SFDP_FlashInfoTypeDef *)Info);
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD :
      {
        /* Perform the Get Information operation using the SDCARD driver */
        if (EXTMEM_DRIVER_SDCARD_OK != EXTMEM_DRIVER_SDCARD_GetInfo(&extmem_list_config[MemId].SdCardObject,
                                                                    (EXTMEM_DRIVER_SDCARD_InfoTypeDef *)Info))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the Get Information operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_GetInfo(&extmem_list_config[MemId].CustomObject,
                                                                 (EXTMEM_DRIVER_CUSTOM_MemInfoTypeDef *)Info))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the Get Information operation using the USER driver */
        if (EXTMEM_DRIVER_USER_OK != EXTMEM_DRIVER_USER_GetInfo(&extmem_list_config[MemId].UserObject,
                                                                (EXTMEM_USER_MemInfoTypeDef *)Info))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Enables or disables the memory mapped mode.
  * @param MemId Memory identifier.
  * @param State Enable or disable state (@ref EXTMEM_StateTypeDef).
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_MemoryMappedMode(uint32_t MemId, EXTMEM_StateTypeDef State)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL();

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1
      case EXTMEM_NOR_SFDP:
      {
        /* Perform the memory Map operation using the NOR SFDP driver */
        if (EXTMEM_ENABLE == State)
        {
          /* Start the memory mapped mode */
          if (EXTMEM_DRIVER_NOR_SFDP_OK !=
              EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(&extmem_list_config[MemId].NorSfdpObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        else
        {
          /* Stop the memory mapped mode */
          if (EXTMEM_DRIVER_NOR_SFDP_OK !=
              EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(&extmem_list_config[MemId].NorSfdpObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */
#if EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM :
      {
        /* Perform the memory Map operation using the PSRAM driver */
        if (EXTMEM_ENABLE == State)
        {
          /* Start the memory mapped mode */
          if (EXTMEM_DRIVER_PSRAM_OK !=
              EXTMEM_DRIVER_PSRAM_Enable_MemoryMappedMode(&extmem_list_config[MemId].PsramObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        else
        {
          /* Stop the memory mapped mode */
          if (EXTMEM_DRIVER_PSRAM_OK !=
              EXTMEM_DRIVER_PSRAM_Disable_MemoryMappedMode(&extmem_list_config[MemId].PsramObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        break;
      }
#endif /* EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM :
      {
        /* Perform the memory Map operation using the CUSTOM driver */
        if (EXTMEM_ENABLE == State)
        {
          /* Start the memory mapped mode */
          if (EXTMEM_DRV_CUSTOM_OK !=
              EXTMEM_DRIVER_CUSTOM_Enable_MemoryMappedMode(&extmem_list_config[MemId].CustomObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        else
        {
          /* Stop the memory mapped mode */
          if (EXTMEM_DRV_CUSTOM_OK !=
              EXTMEM_DRIVER_CUSTOM_Disable_MemoryMappedMode(&extmem_list_config[MemId].CustomObject))
          {
            return EXTMEM_ERROR_DRIVER;
          }
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER :
      {
        /* Perform the memory Map operation using the USER driver */
        EXTMEM_DRIVER_USER_StatusTypeDef status;
        if (EXTMEM_ENABLE == State)
        {
          /* Start the memory mapped mode */
          status = EXTMEM_DRIVER_USER_Enable_MemoryMappedMode(&extmem_list_config[MemId].UserObject);
        }
        else
        {
          /* Stop the memory mapped mode */
          status = EXTMEM_DRIVER_USER_Disable_MemoryMappedMode(&extmem_list_config[MemId].UserObject);
        }
        switch (status)
        {
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          case EXTMEM_DRIVER_USER_OK:
            break;
          default:
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD :
        /* Functionality not supported in this driver */
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
      /* Unknown type of EXTMEM driver */
      default:
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}

/**
  * @brief Gets the base address of the mapped external memory.
  * @param MemId Memory identifier.
  * @param BaseAddress Pointer to variable to receive the base address.
  * @retval EXTMEM_StatusTypeDef Status of the operation.
  */
EXTMEM_StatusTypeDef EXTMEM_GetMapAddress(uint32_t MemId, uint32_t *BaseAddress)
{
  EXTMEM_StatusTypeDef retr = EXTMEM_ERROR_INVALID_ID;
  EXTMEM_FUNC_CALL();

  /* Check the memory ID */
  if (MemId < (sizeof(extmem_list_config) / sizeof(EXTMEM_DefinitionTypeDef)))
  {
    retr = EXTMEM_OK;

    /* Check type of EXTMEM driver used to access memory */
    switch (extmem_list_config[MemId].MemType)
    {
#if EXTMEM_DRIVER_NOR_SFDP == 1 || EXTMEM_DRIVER_PSRAM == 1
      case EXTMEM_PSRAM:
      case EXTMEM_NOR_SFDP:
      {
        /* Define Map address based on Instance used for memory access */
#if defined(XSPI1)
        if (((XSPI_HandleTypeDef *)extmem_list_config[MemId].Handle)->Instance == XSPI1)
        {
          *BaseAddress = XSPI1_BASE;
        }
#if defined(XSPI2)
        else if (((XSPI_HandleTypeDef *)extmem_list_config[MemId].Handle)->Instance == XSPI2)
        {
          *BaseAddress = XSPI2_BASE;
        }
#if defined(XSPI3)
        else if (((XSPI_HandleTypeDef *)extmem_list_config[MemId].Handle)->Instance == XSPI3)
        {
          *BaseAddress = XSPI3_BASE;
        }
#endif /* XSPI3 */
#endif /* XSPI2 */
#else  /* XSPI1 */
        if (((XSPI_HandleTypeDef *)extmem_list_config[MemId].Handle)->Instance == OCTOSPI1)
        {
          *BaseAddress = OCTOSPI1_BASE;
        }
#endif  /* XSPI1 */
        else
        {
          retr = EXTMEM_ERROR_DRIVER;
        }

        break;
      }
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 || EXTMEM_DRIVER_PSRAM == 1 */
#if EXTMEM_DRIVER_SDCARD == 1
      case EXTMEM_SDCARD :
      {
        retr = EXTMEM_ERROR_NOTSUPPORTED;
        break;
      }
#endif /* EXTMEM_DRIVER_SDCARD == 1 */
#if EXTMEM_DRIVER_CUSTOM == 1
      case EXTMEM_CUSTOM:
      {
        /* Perform the Get Map Address operation using the CUSTOM driver */
        if (EXTMEM_DRV_CUSTOM_OK != EXTMEM_DRIVER_CUSTOM_GetMapAddress(&extmem_list_config[MemId].CustomObject,
                                                                       BaseAddress))
        {
          retr = EXTMEM_ERROR_DRIVER;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_CUSTOM == 1 */
#if EXTMEM_DRIVER_USER == 1
      case EXTMEM_USER:
      {
        /* Perform the Get Map Address operation using the USER driver */
        switch (EXTMEM_DRIVER_USER_GetMapAddress(&extmem_list_config[MemId].UserObject, BaseAddress))
        {
          case EXTMEM_DRIVER_USER_OK:
            break;
          case EXTMEM_DRIVER_USER_NOTSUPPORTED:
            retr = EXTMEM_ERROR_NOTSUPPORTED;
            break;
          default :
            retr = EXTMEM_ERROR_DRIVER;
            break;
        }
        break;
      }
#endif /* EXTMEM_DRIVER_USER == 1 */
      /* Unknown type of EXTMEM driver */
      default :
      {
        EXTMEM_DEBUG("\terror : unknown memory type\n");
        retr = EXTMEM_ERROR_UNKNOWNMEMORY;
        break;
      }
    }
  }
  return retr;
}
/**
  * @}
  */

/**
  * @}
  */

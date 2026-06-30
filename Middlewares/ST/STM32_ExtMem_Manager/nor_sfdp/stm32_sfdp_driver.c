/**
  ******************************************************************************
  * @file    stm32_sfdp_driver.c
  * @author  MCD Application Team
  * @brief   This file includes a driver for SFDP support
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
#include "stm32_extmem_conf.h"

#if EXTMEM_DRIVER_NOR_SFDP == 1
#include "stm32_sfdp_driver_type.h"
#include "stm32_sfdp_driver_api.h"
#include "stm32_sfdp_data.h"
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && defined(EXTMEM_MACRO_DEBUG)
#include <stdio.h>
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL != 0 && EXTMEM_MACRO_DEBUG */
#include <string.h>

/** @defgroup NOR_SFDP NOR SFDP driver
  * @ingroup EXTMEM_DRIVER
  * @{
  */


/* Private Macros ------------------------------------------------------------*/
/** @defgroup NOR_SFDP_Private_Macro DRIVER NOR SFDP Private Macro
  * @{
  */
/**
  * @brief MIN macro
  */
#define MIN(_A_,_B_)   ((_A_) > (_B_))?(_B_):(_A_);

/**
  * @brief Default timeout
  */
#define DRIVER_DEFAULT_TIMEOUT 300

/**
  * @brief Default clock value to read SFDP data
  */
#define DRIVER_SFDP_DEFAULT_CLOCK 50000000u

/**
  * @brief DEBUG macro
  */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 1 && defined(EXTMEM_MACRO_DEBUG)
/**
  * @brief Debug macro for a string
  * @param _STR_ string
  */
#define SFDP_DEBUG_STR(_STR_)       \
  {                                 \
    EXTMEM_MACRO_DEBUG("\tSFDP::"); \
    EXTMEM_MACRO_DEBUG(_STR_);      \
    EXTMEM_MACRO_DEBUG("\n\r");     \
  }

/**
  * @brief debug macro for low level
  */
#if EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 2
#define DEBUG_DRIVER(_STR_)                 \
  {                                         \
    EXTMEM_MACRO_DEBUG("\tSFDP::DRIVER::"); \
    EXTMEM_MACRO_DEBUG(_STR_);              \
    EXTMEM_MACRO_DEBUG("\n");               \
  }
#else
#define DEBUG_DRIVER(_STR_)
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 2 */

/**
  * @brief debug macro for an integer
  */
#define DEBUG_ID(_ID_)                               \
  {                                                  \
    char StrID[40];                                  \
    EXTMEM_MACRO_DEBUG("\tSFDP:: Flash ID(");        \
    (void)sprintf(StrID, "0x%x:0x%x:0x%x:0x%x",      \
                  _ID_[0],_ID_[1],_ID_[2],_ID_[3]);  \
    EXTMEM_MACRO_DEBUG(StrID);                       \
    EXTMEM_MACRO_DEBUG(")\n");                       \
  }

/**
  * @brief Debug macro for error
  */
#define DEBUG_DRIVER_ERROR(_STR_)   SFDP_DEBUG_STR(_STR_)
#else
#define DEBUG_ID(_ID_)
#define SFDP_DEBUG_STR(_STR_)
#define DEBUG_DRIVER(_STR_)
#define DEBUG_DRIVER_ERROR(_STR_)
#endif /* EXTMEM_DRIVER_NOR_SFDP_DEBUG_LEVEL > 1 && EXTMEM_MACRO_DEBUG */

/**
  * @}
  */

/* Private typedefs ---------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @defgroup DRIVER_SFDP_Private_Functions DRIVER SFDP Private Functions
  * @{
  */
static EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_set_FlagWEL(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                               uint32_t Timeout);
__weak void EXTMEM_MemCopy(uint32_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize);

/**
  * @}
  */

/** @defgroup DRIVER_SFDP_Exported_Functions DRIVER SFDP Exported Functions
  * @{
  */

/**
  * @brief Initializes the NOR SFDP driver.
  * @param Peripheral Pointer to peripheral.
  * @param Config Configuration type.
  * @param ClockInput Clock input value.
  * @param SFDPObject Pointer to memory object.
  * @retval EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef Status of the operation.
  */
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Init(void *Peripheral, EXTMEM_LinkConfig_TypeDef Config,
                                                                 uint32_t ClockInput,
                                                                 EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;
  SFDP_HeaderTypeDef JEDEC_SFDP_Header;
  uint8_t FreqUpdate = 0u;
  uint8_t DataID[6];
  uint32_t ClockOut;

  /* Reset data of SFDPObject to zero */
  SFDP_DEBUG_STR("1 - reset data SFDPObject to zero")
  (void)memset((void *)&SFDPObject->sfdp_private, 0x0, sizeof(SFDPObject->sfdp_private));

  /* Initialize the SFDPObject */
  SFDP_DEBUG_STR("2 - initialize the SFDPObject")
  SFDPObject->sfdp_private.Config = Config;
  SFDPObject->sfdp_private.DriverInfo.SpiPhyLink = PHY_LINK_1S1S1S;
  SFDPObject->sfdp_private.DriverInfo.ClockIn = ClockInput;
  SAL_XSPI_SET_SFDPDUMMYCYLE(SFDPObject->sfdp_private.SALObject, EXTMEM_READ_SFDP_NB_DUMMY_CYCLES_DEFAULT);

  /* Set memory speed to 50Mhz maximum */
  SFDP_DEBUG_STR("3 - set memory link and speed to 50Mhz maximum")
  (void)SAL_XSPI_Init(&SFDPObject->sfdp_private.SALObject, Peripheral);
  (void)SAL_XSPI_SetClock(&SFDPObject->sfdp_private.SALObject, ClockInput, DRIVER_SFDP_DEFAULT_CLOCK, &ClockOut);

  /* Abort any ongoing XSPI action */
  (void)SAL_XSPI_DisableMapMode(&SFDPObject->sfdp_private.SALObject);

  /* Analyze the SFDP structure to get driver information */
  SFDP_DEBUG_STR("4 - analyze the SFDP structure to get driver information")
  if (EXTMEM_SFDP_OK != SFDP_GetHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    /* Perform SFDP Header reading (several modes are tried) */
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* Reset the memory */
  SFDP_DEBUG_STR("5 - reset the memory")
  if (EXTMEM_SFDP_OK != SFDP_MemoryReset(SFDPObject))
  {
    /* Memory Reset is considered unsuccessful.
       Try to continue Initialisation procedure anyway. */
    SFDP_DEBUG_STR("ERROR::on the call of SFDP_MemoryReset but no error returned")
  }

  /* Wait few ms after the reset operation, this is done to avoid issue on SFDP read */
  HAL_Delay(10);

  /* Analyze the SFDP structure to get driver information after the reset */
  SFDP_DEBUG_STR("6 - analyze the SFDP structure to get driver information")
  if (EXTMEM_SFDP_OK != SFDP_GetHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    /* SFDP header reading is considered unsuccessful.
       Abort Initialisation procedure. */
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* Save information from the SFDP table
     Nb of parameters starts at 0 (0 means 1 parameter) */
  if (JEDEC_SFDP_Header.param_number >= SFDP_MAX_NB_OF_PARAM)
  {
    SFDPObject->sfdp_private.Sfdp_param_number = SFDP_MAX_NB_OF_PARAM - 1;
  }
  else
  {
    SFDPObject->sfdp_private.Sfdp_param_number = JEDEC_SFDP_Header.param_number;
  }
  SFDPObject->sfdp_private.Sfdp_AccessProtocol = JEDEC_SFDP_Header.AccessProtocol;

  /* Read the flash ID.
     This Flash ID could later be used to perform specific operations for some identified references. */
  SFDP_DEBUG_STR("7 - read the flash ID")
  (void)SAL_XSPI_GetId(&SFDPObject->sfdp_private.SALObject, DataID, EXTMEM_READ_JEDEC_ID_SIZE);
  DEBUG_ID(DataID);

  /* Keep manufacturer information, it could be used to help in
     building of consistent driver */
  SFDPObject->sfdp_private.ManuID = DataID[0];
#if defined(EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG)
  SFDPObject->sfdp_private.SALObject.ManufacturerId = DataID[0];
#endif /* EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG */

  /* Get the complete SFDP data */
  SFDP_DEBUG_STR("8 - collect the SFDP data")
  if (EXTMEM_SFDP_OK != SFDP_CollectData(SFDPObject))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SFDP;
    goto error;
  }

  /* Setup the generic driver information and prepare the physical layer */
  SFDP_DEBUG_STR("9 - build the generic driver information and prepare the physical layer")
  if (EXTMEM_SFDP_OK != SFDP_BuildGenericDriver(SFDPObject, &FreqUpdate))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_ERROR_BUILD")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_BUILD;
    goto error;
  }

  /* Adjust frequency supplied to memory, if required */
  SFDP_DEBUG_STR("10 - adjust the frequency if required")
  if ((FreqUpdate == 0u) && (SFDPObject->sfdp_public.MaxFreq != 0u))
  {
    (void)SAL_XSPI_SetClock(&SFDPObject->sfdp_private.SALObject, ClockInput,
                            SFDPObject->sfdp_public.MaxFreq, &ClockOut);
    SFDP_DEBUG_STR("--> new freq configured");
  }

  /* Read again SFDP header to confirm memory access is properly setup */
  SFDP_DEBUG_STR("11 - read again the SFDP header to adjust memory type if necessary")
  if (EXTMEM_SFDP_OK != SFDP_ReadHeader(SFDPObject, &JEDEC_SFDP_Header))
  {
    SFDP_DEBUG_STR("ERROR::EXTMEM_DRIVER_NOR_SFDP_MEMTYPE_CHECK")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MEMTYPE_CHECK;
    goto error;
  }

error:
  return retr;
}

/**
  * @brief This function un-initializes the NOR SFDP driver
  *
  * @param SFDPObject Memory object
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_DeInit(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  SFDPObject->sfdp_private.FlashSize = 0;
  return EXTMEM_DRIVER_NOR_SFDP_OK;
}

/**
  * @brief This function returns the flash memory information
  *
  * @param SFDPObject Memory object
  * @param FlashInfo Pointer on flash info structure
  **/
void EXTMEM_DRIVER_NOR_SFDP_GetFlashInfo(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                         EXTMEM_NOR_SFDP_FlashInfoTypeDef *FlashInfo)
{
  DEBUG_DRIVER((uint8_t *)__func__)
  /* Format the info into structure */
  FlashInfo->FlashSize      = SFDPObject->sfdp_private.FlashSize;
  FlashInfo->PageSize       = SFDPObject->sfdp_private.PageSize;
  FlashInfo->EraseType1Size = (SFDPObject->sfdp_private.DriverInfo.EraseType1Size == 0u) ? 0u :
                              ((uint32_t)1u << SFDPObject->sfdp_private.DriverInfo.EraseType1Size);
  FlashInfo->EraseType2Size = (SFDPObject->sfdp_private.DriverInfo.EraseType2Size == 0u) ? 0u :
                              ((uint32_t)1u << SFDPObject->sfdp_private.DriverInfo.EraseType2Size);
  FlashInfo->EraseType3Size = (SFDPObject->sfdp_private.DriverInfo.EraseType3Size == 0u) ? 0u :
                              ((uint32_t)1u << SFDPObject->sfdp_private.DriverInfo.EraseType3Size);
  FlashInfo->EraseType4Size = (SFDPObject->sfdp_private.DriverInfo.EraseType4Size == 0u) ? 0u :
                              ((uint32_t)1u << SFDPObject->sfdp_private.DriverInfo.EraseType4Size);
}

/**
  * @brief This function reads the memory
  *
  * @param SFDPObject Memory object
  * @param Address Memory address
  * @param Data Pointer on the data
  * @param Size Data size to read (in bytes)
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Read(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                                 uint32_t Address, uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* Check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Read::ERROR_CHECK_BUSY")
    goto error;
  }

  /* Perform read access using SAL service */
  if (HAL_OK != SAL_XSPI_Read(&SFDPObject->sfdp_private.SALObject, SFDPObject->sfdp_private.DriverInfo.ReadInstruction,
                              Address, Data, Size))
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Read::ERROR_READ")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_READ;
  }
error :
  return retr;
}

/**
  * @brief This function writes data in the memory
  *
  * @param SFDPObject Memory object
  * @param Address Memory address
  * @param Data Pointer on the data
  * @param Size Data size to write
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Write(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                                  uint32_t Address, const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  uint32_t size_write;
  uint32_t local_size = Size;
  uint32_t local_Address = Address;
  uint32_t local_Data = (uint32_t)Data;
  uint32_t misalignment = 0u;

  if (0u != (local_Address % SFDPObject->sfdp_private.PageSize))
  {
    misalignment = 1u;
  }

  DEBUG_DRIVER((uint8_t *)__func__)
  while (local_size != 0u)
  {
    if (misalignment == 1u)
    {
      size_write = SFDPObject->sfdp_private.PageSize - (local_Address % SFDPObject->sfdp_private.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      size_write = MIN(local_size, SFDPObject->sfdp_private.PageSize);
    }

    /* Check WIP flag */
    retr = driver_check_FlagBUSY(SFDPObject, 5000u);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_BUSY")
      goto error;
    }

    /* Wait for WEL flag */
    retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_WEL")
      goto error;
    }

    /* Write the data */
    if (HAL_OK != SAL_XSPI_Write(&SFDPObject->sfdp_private.SALObject,
                                 SFDPObject->sfdp_private.DriverInfo.PageProgramInstruction,
                                 local_Address, (uint8_t *)local_Data, size_write))
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_WRITE")
      retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITE;
      goto error;
    }

    local_size = local_size - size_write;
    local_Address = local_Address + size_write;
    local_Data = local_Data + size_write;
  }

  /* Check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000);
#if EXTMEM_MACRO_DEBUG
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_Write::ERROR_CHECK_BUSY_ON_EXIT")
  }
#endif /* EXTMEM_MACRO_DEBUG */

error:
  return retr;
}

/**
  * @brief This function writes data in the memory in mapped mode
  *
  * @param SFDPObject Memory object
  * @param Address Memory address in mapped mode
  * @param Data Pointer on the data
  * @param Size Data size to write
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_WriteInMappedMode(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef
                                                                              *SFDPObject, uint32_t Address,
                                                                              const uint8_t *Data, uint32_t Size)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;
  uint32_t size_write;
  uint32_t local_size = Size;
  uint32_t local_Address = Address;
  const uint8_t *local_Data = Data;
  uint32_t size;
  uint32_t misalignment = 0u;
#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  uint8_t dcache_status = 1;
#endif /* __DCACHE_PRESENT */

  DEBUG_DRIVER((uint8_t *)__func__)

  /* Check if the input address is 32bit aligned */
  if (0u != (local_Address % 4u))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ADDRESS_ALIGNMENT;
    goto error;
  }

  if (0u != (local_Address % SFDPObject->sfdp_private.PageSize))
  {
    misalignment = 1u;
  }

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* Check if DCache is already enabled */
  if (SCB->CCR & SCB_CCR_DC_Msk)
  {
    dcache_status = 1;
  }
  else
  {
    dcache_status = 0;
    /* Enable DCache */
    SCB_EnableDCache();
  }
#endif /* __DCACHE_PRESENT */

  while (local_size != 0u)
  {
    if (misalignment == 1u)
    {
      size_write = SFDPObject->sfdp_private.PageSize - (local_Address % SFDPObject->sfdp_private.PageSize);
      size_write = MIN(local_size, size_write);
      misalignment = 0u;
    }
    else
    {
      if (local_size > SFDPObject->sfdp_private.PageSize)
      {
        size_write = SFDPObject->sfdp_private.PageSize;
      }
      else
      {
        size_write = local_size;
        if (0u != (local_size % sizeof(uint32_t)))
        {
          size_write = size_write + sizeof(uint32_t) - (local_size % sizeof(uint32_t));
        }
      }
    }

    /* Wait for Write enable flag */
    retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_write::ERROR_CHECK_WEL")
      goto error;
    }

    /* Enter the mapped mode */
    retr = EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(SFDPObject);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      goto error;
    }

    size = size_write;
    /* Execute the copy */
    EXTMEM_MemCopy((uint32_t *)local_Address, local_Data, size_write);

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
    /* Clean DCache starting from a 32 byte aligned address in 32 byte granularity */
    SCB_CleanDCache_by_Addr((void *)(local_Address), size_write);
#endif /* __DCACHE_PRESENT */

    /* Enter the mapped mode */
    retr = EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(SFDPObject);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      goto error;
    }

    /* Check busy flag */
    retr = driver_check_FlagBUSY(SFDPObject, 5000);
    if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
    {
      DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_write::ERROR_CHECK_BUSY_ON_EXIT")
      goto error;
    }

    /* Decrement the transfer size */
    if (local_size > size)
    {
      local_size = local_size - size;
      local_Data = &local_Data[size];
      local_Address = local_Address + size;
    }
    else
    {
      local_size = 0;
    }
  }

error:

#if defined (__DCACHE_PRESENT) && (__DCACHE_PRESENT == 1U)
  /* Disable DCache if deactivated previously */
  if (!dcache_status)
  {
    SCB_DisableDCache();
  }
#endif /* __DCACHE_PRESENT */

  return retr;
}

/**
  * @brief This function erases all the memory
  *
  * @param SFDPObject Memory object
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_MassErase(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* Check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 1000);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_read::ERROR_CHECK_BUSY")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_FLASHBUSY;
    goto error;
  }

  /* Wait for write enable flag */
  retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_read::ERROR_CHECK_WEL")
    goto error;
  }

  /* Launch mass erase command */
  (void)SAL_XSPI_CommandSendData(&SFDPObject->sfdp_private.SALObject, SFDP_DRIVER_ERASE_CHIP_COMMAND, NULL, 0);


  /* Check busy flag : time to used should be set according the memory characteristic */
  retr = driver_check_FlagBUSY(SFDPObject, SFDPObject->sfdp_private.DriverInfo.EraseChipTiming);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    DEBUG_DRIVER_ERROR("EXTMEM_DRIVER_NOR_SFDP_MassErase::ERROR_CHECK_BUSY_ON_EXIT")
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ERASE_TIMEOUT;
    goto error;
  }

error:
  return retr;
}

/**
  * @brief This function erases memory sector
  *
  * @param SFDPObject Memory object
  * @param Address Memory address to start erase operation
  * @param SectorType Type of sector
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_SectorErase(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef
                                                                        *SFDPObject, uint32_t Address,
                                                                        EXTMEM_DRIVER_NOR_SFDP_SectorTypeTypeDef
                                                                        SectorType)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr;
  uint8_t command;
  uint8_t size;
  uint32_t timeout;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* Check if the selected sector type is available */
  switch (SectorType)
  {
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE1:
      command = SFDPObject->sfdp_private.DriverInfo.EraseType1Command;
      size = SFDPObject->sfdp_private.DriverInfo.EraseType1Size;
      timeout = SFDPObject->sfdp_private.DriverInfo.EraseType1Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE2:
      command = SFDPObject->sfdp_private.DriverInfo.EraseType2Command;
      size = SFDPObject->sfdp_private.DriverInfo.EraseType2Size;
      timeout = SFDPObject->sfdp_private.DriverInfo.EraseType2Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE3:
      command = SFDPObject->sfdp_private.DriverInfo.EraseType3Command;
      size = SFDPObject->sfdp_private.DriverInfo.EraseType3Size;
      timeout = SFDPObject->sfdp_private.DriverInfo.EraseType3Timing;
      break;
    case EXTMEM_DRIVER_NOR_SFDP_SECTOR_TYPE4:
      command = SFDPObject->sfdp_private.DriverInfo.EraseType4Command;
      size = SFDPObject->sfdp_private.DriverInfo.EraseType4Size;
      timeout = SFDPObject->sfdp_private.DriverInfo.EraseType4Timing;
      break;
    default :
      retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE;
      goto error;
      break;
  }

  /* Check if the command for this sector size is available */
  if (0x0u == command)
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_SECTORTYPE_UNAVAILABLE;
    goto error;
  }

  /* Check address alignment */
  if (0x0u != (Address % ((uint32_t)1u << size)))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_ADDRESS_ALIGNMENT;
    goto error;
  }

  /* Check busy flag */
  retr = driver_check_FlagBUSY(SFDPObject, 5000u);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    goto error;
  }

  /* Wait for write enable flag */
  retr = driver_set_FlagWEL(SFDPObject, DRIVER_DEFAULT_TIMEOUT);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    goto error;
  }

  /* Launch erase command */
  (void)SAL_XSPI_CommandSendAddress(&SFDPObject->sfdp_private.SALObject, command, Address);

  /* Check busy flag */
  /* Timeout is set according the memory characteristic */
  retr = driver_check_FlagBUSY(SFDPObject, timeout);
  if (EXTMEM_DRIVER_NOR_SFDP_OK != retr)
  {
    goto error;
  }


error:
  return retr;
}

/**
  * @brief This function enables the memory mapped mode
  *
  * @param SFDPObject Memory object
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Enable_MemoryMappedMode(
  EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;

  /* Enter the mapped mode */
  if (HAL_OK != SAL_XSPI_EnableMapMode(&SFDPObject->sfdp_private.SALObject,
                                       SFDPObject->sfdp_private.DriverInfo.ReadInstruction,
                                       (uint8_t)SFDPObject->sfdp_private.SALObject.Commandbase.DummyCycles,
                                       SFDPObject->sfdp_private.DriverInfo.PageProgramInstruction, 0))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MAP_ENABLE;
  }

  return retr;
}

/**
  * @brief This function disables the memory mapped mode
  *
  * @param SFDPObject Memory object
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef EXTMEM_DRIVER_NOR_SFDP_Disable_MemoryMappedMode(
  EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_OK;

  /* Exit the mapped mode */
  if (HAL_OK != SAL_XSPI_DisableMapMode(&SFDPObject->sfdp_private.SALObject))
  {
    retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_MAP_ENABLE;
  }

  return retr;
}


/**
  * @}
  */

/** @addtogroup DRIVER_SFDP_Internal_Functions DRIVER SFDP Internal Functions
  * @{
  */


/**
  * @}
  */

/** @addtogroup DRIVER_SFDP_Private_Functions DRIVER SFDP Private Functions
  * @{
  */

/**
  * @brief This function enables the WEL Flag and checks its activation
  *
  * @param SFDPObject Memory object
  * @param timeout Timeout value
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef driver_set_FlagWEL(EXTMEM_DRIVER_NOR_SFDP_ObjectTypeDef *SFDPObject,
                                                        uint32_t Timeout)
{
  EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef retr = EXTMEM_DRIVER_NOR_SFDP_ERROR_WRITEENABLE;
  DEBUG_DRIVER((uint8_t *)__func__)

  /* Send the command write enable */
  (void)SAL_XSPI_CommandSendData(&SFDPObject->sfdp_private.SALObject,
                                 SFDPObject->sfdp_private.DriverInfo.WriteWELCommand, NULL, 0);

  /* Wait for write enable status */
  if (0u != SFDPObject->sfdp_private.DriverInfo.ReadWELCommand)
  {
#if defined(EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG)
    uint8_t status_reg[2] = {0u, 0u};
    uint32_t tickstart = HAL_GetTick();
    uint8_t match_value = (uint8_t)(((SFDPObject->sfdp_private.DriverInfo.WELBusyPolarity == 0u) ? 1u : 0u)
                                    << SFDPObject->sfdp_private.DriverInfo.WELPosition);
    uint8_t match_mask = (uint8_t)(1u << SFDPObject->sfdp_private.DriverInfo.WELPosition);

    while ((HAL_GetTick() - tickstart) <= Timeout)
    {
      if (HAL_OK != SAL_XSPI_SendReadCommand(&SFDPObject->sfdp_private.SALObject,
                                             SFDPObject->sfdp_private.DriverInfo.ReadWELCommand,
                                             status_reg, sizeof(status_reg)))
      {
        goto error;
      }

      if (((status_reg[0] & match_mask) == match_value) && ((status_reg[1] & match_mask) == match_value))
      {
        retr = EXTMEM_DRIVER_NOR_SFDP_OK;
        return retr;
      }
    }
#else
    /* Check if flag write enable is enabled */
    if (HAL_OK == SAL_XSPI_CheckStatusRegister(&SFDPObject->sfdp_private.SALObject,
                                               SFDPObject->sfdp_private.DriverInfo.ReadWELCommand,
                                               SFDPObject->sfdp_private.DriverInfo.WELAddress,
                                               ((SFDPObject->sfdp_private.DriverInfo.WELBusyPolarity == 0u) ? 1u : 0u)
                                               << SFDPObject->sfdp_private.DriverInfo.WELPosition,
                                               1u << SFDPObject->sfdp_private.DriverInfo.WELPosition,
                                               SFDPObject->sfdp_private.ManuID, Timeout))
    {
      retr = EXTMEM_DRIVER_NOR_SFDP_OK;
    }
#endif /* EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG */
  }
#if defined(EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG)
error:
#endif /* EXTMEM_DRIVER_NOR_SFDP_DUAL_CONFIG */
  return retr;
}

/**
  * @brief This function provides a default implementation of MemCopy functionality
  *
  * @param Destination_Address Destination Address where data is to be copied
  * @param ptrData Pointer on source data to be copied
  * @param DataSize Size of data to be copied (in bytes)
  * @return @ref EXTMEM_DRIVER_NOR_SFDP_StatusTypeDef
  **/
__weak void EXTMEM_MemCopy(uint32_t *Destination_Address, const uint8_t *ptrData, uint32_t DataSize)
{
  uint32_t *ptrDest = Destination_Address;
  /* Write the data */
  for (uint32_t index = 0u; index < DataSize; index = index + 4u)
  {
    *ptrDest = ((uint32_t)ptrData[index] | ((uint32_t)ptrData[index + 1u] << 8u) |
                ((uint32_t)ptrData[index + 2u] << 16u) | ((uint32_t)ptrData[index + 3u] << 24u));
    ptrDest++;
  }
}

/**
  * @}
  */

/**
  * @}
  */
#endif /* EXTMEM_DRIVER_NOR_SFDP == 1 */

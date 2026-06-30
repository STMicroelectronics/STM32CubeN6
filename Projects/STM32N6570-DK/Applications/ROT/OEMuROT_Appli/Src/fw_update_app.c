/**
  ******************************************************************************
  * @file    fw_update_app.c
  * @author  MCD Application Team
  * @brief   Firmware Update module.
  *          This file provides set of firmware functions to manage Firmware
  *          Update functionalities.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/

#include "string.h"
#include "stm32n6xx_hal.h"
#include "common.h"
#include "com.h"
#include "ymodem.h"
#include "appli_flash_layout.h"
#include "secure_nsc.h"

#include "fw_update_app.h"


/** @addtogroup FW_UPDATE_Private_Typedef Private Typedef
  * @{
  */
typedef struct
{
  uint32_t  MaxSizeInBytes;        /*!< The maximum allowed size for the FwImage in User Flash (in Bytes) */
  uint32_t  DownloadAddr;          /*!< The download address for the FwImage in UserFlash */
  uint32_t  ImageOffsetInBytes;    /*!< Image write starts at this offset */
  uint32_t  ExecutionAddr;         /*!< The execution address for the FwImage in UserFlash */
} SFU_FwImageFlashTypeDef;
/**
  * @}
  */

/** @defgroup FW_UPDATE_Private_Variables Private Variables
  * @{
  */
static uint32_t m_uFileSizeYmodem = 0U;    /* !< Ymodem File size*/
static uint32_t m_uNbrBlocksYmodem = 0U;   /* !< Ymodem Number of blocks*/
static uint32_t m_uPacketsReceived = 0U;   /* !< Ymodem packets received*/
static uint32_t m_uFlashSectorSize = 0U;   /* !< Flash Sector Size */
static uint32_t m_uFlashMinWriteSize = 0U; /* !< FLash Min Write access*/
/**
  * @}
  */

/** @defgroup FW_UPDATE_Private_Const Private Const
  * @{
  */
const uint32_t MagicTrailerValue[] =
{
  0xf395c277,
  0x7fefd260,
  0x0f505235,
  0x8079b62c,
};
/**
  * @}
  */
extern ARM_DRIVER_FLASH Driver_EXT_FLASH0;

/** @defgroup  FW_UPDATE_Private_Functions Private Functions
  * @{
  */

static HAL_StatusTypeDef FW_UPDATE_SECURE_APP_IMAGE(void);
#if (S_DATA_IMAGE_NUMBER == 1)
static HAL_StatusTypeDef FW_UPDATE_SECURE_DATA_IMAGE(void);
#endif /* (S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_OVERWRITE_ONLY == 0)
static void FW_Valid_SecureAppImage(void);
#if (S_DATA_IMAGE_NUMBER == 1)
static void FW_Valid_SecureDataImage(void);
#endif /* (S_DATA_IMAGE_NUMBER == 1) */
#endif /* MCUBOOT_OVERWRITE_ONLY == 0 */
static void FW_UPDATE_PrintWelcome(void);
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea);

/** @defgroup  FW_UPDATE_Control_Functions Control Functions
  * @{
   */
void FW_UPDATE_Run(void)
{
  uint8_t key = 0U;
  uint8_t exit = 0U;

  /* Print Firmware Update welcome message */
  FW_UPDATE_PrintWelcome();

  while (exit == 0U)
  {
    key = 0U;

    /* Clean the input path */
    COM_Flush();

    /* Receive key */
    if (COM_Receive(&key, 1U, RX_TIMEOUT) == HAL_OK)
    {
      switch (key)
      {
      case '1' :
          printf("  -- Install image : reboot\r\n\n");
          NVIC_SystemReset();
          break;
      case '2' :
          FW_UPDATE_SECURE_APP_IMAGE();
          break;
#if (S_DATA_IMAGE_NUMBER == 1)
      case '4' :
          FW_UPDATE_SECURE_DATA_IMAGE();
          break;
#endif /* (S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_OVERWRITE_ONLY == 0)
      case '6':
          FW_Valid_SecureAppImage();
          break;
#if (S_DATA_IMAGE_NUMBER == 1)
      case '8':
          FW_Valid_SecureDataImage();
          break;
#endif /* S_DATA_IMAGE_NUMBER == 1 */
#endif /* MCUBOOT_OVERWRITE_ONLY == 0 */
      case 'x' :
          exit = 1U;
          break;
      default:
          printf("Invalid Number !\r");
          break;
      }
      /* Print main menu message */
      FW_UPDATE_PrintWelcome();
    }
  }
}
/**
  * @}
  */

/** @addtogroup  FW_UPDATE_Private_Functions
  * @{
  */

/**
  * @brief  Display the FW_UPDATE Main Menu choices on HyperTerminal
  * @param  None.
  * @retval None.
  */
static void FW_UPDATE_PrintWelcome(void)
{
  printf("\r\n================ New Fw Image ============================\r\n\n");
  printf("  Reset to trigger Installation ------------------------- 1\r\n\n");
  printf("  Download Secure App Image ----------------------------- 2\r\n\n");
#if (S_DATA_IMAGE_NUMBER == 1)
  printf("  Download Secure Data Image ---------------------------- 4\r\n\n");
#endif /* (S_DATA_IMAGE_NUMBER == 1) */
#if (MCUBOOT_OVERWRITE_ONLY == 0)
  printf("  Validate Secure App Image ----------------------------- 6\r\n\n");
#if (S_DATA_IMAGE_NUMBER == 1)
  printf("  Validate Secure Data Image ---------------------------- 8\r\n\n");
#endif /* S_DATA_IMAGE_NUMBER == 1 */
#endif /* MCUBOOT_OVERWRITE_ONLY == 0 */
  printf("  Previous Menu ----------------------------------------- x\r\n\n");
}

/**
  * @brief  Download secure application image.
  * @param  None
  * @retval HAL Status.
  */
static HAL_StatusTypeDef FW_UPDATE_SECURE_APP_IMAGE(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  SFU_FwImageFlashTypeDef fw_image_dwl_area = {0};
  ARM_FLASH_INFO data = {0};

  printf("Download Secure App Image\r\n");

  /* Get information about the download area */
  memcpy(&data, Driver_EXT_FLASH0.GetInfo(), sizeof(ARM_FLASH_INFO));
  fw_image_dwl_area.DownloadAddr = S_APPLI_SECONDARY_OFFSET;
  fw_image_dwl_area.MaxSizeInBytes = S_APPLI_PARTITION_SIZE;
  fw_image_dwl_area.ImageOffsetInBytes = 0x0;
  m_uFlashSectorSize = data.sector_size;
  m_uFlashMinWriteSize = data.program_unit;

  /* Download new firmware image*/
  ret = FW_UPDATE_DownloadNewFirmware(&fw_image_dwl_area);
  if (HAL_OK == ret)
  {
    printf("  -- Secure App Image correctly downloaded \r\n\n");
    HAL_Delay(1000U);
  }
  return ret;
}

#if (S_DATA_IMAGE_NUMBER == 1)
/**
  * @brief  Download secure data image.
  * @param  None
  * @retval HAL Status.
  */
static HAL_StatusTypeDef FW_UPDATE_SECURE_DATA_IMAGE(void)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  SFU_FwImageFlashTypeDef fw_image_dwl_area = {0};
  ARM_FLASH_INFO data = {0};

  printf("Download Secure Data Image\r\n");

  /* Get information about the download area */
  memcpy(&data, Driver_EXT_FLASH0.GetInfo(), sizeof(ARM_FLASH_INFO));
  fw_image_dwl_area.DownloadAddr = S_DATA_SECONDARY_OFFSET;
  fw_image_dwl_area.MaxSizeInBytes = S_DATA_PARTITION_SIZE;
  fw_image_dwl_area.ImageOffsetInBytes = 0x0;
  m_uFlashSectorSize = data.sector_size;
  m_uFlashMinWriteSize = data.program_unit;

  /* Download new firmware image*/
  ret = FW_UPDATE_DownloadNewFirmware(&fw_image_dwl_area);
  if (HAL_OK == ret)
  {
    printf("  -- Secure Data Image correctly downloaded \r\n\n");
    HAL_Delay(1000U);
  }
  return ret;
}
#endif /* S_DATA_IMAGE_NUMBER == 1 */


#if (MCUBOOT_OVERWRITE_ONLY == 0)
/**
  * @brief  Write confirm flag for secure application image
  * @param  None
  * @retval None
  */
static void FW_Valid_SecureAppImage(void)
{
  const uint8_t FlagPattern[16]={0x1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  const unsigned int ConfirmAddress = S_APPLI_OFFSET + S_APPLI_PARTITION_SIZE - (sizeof(MagicTrailerValue) + sizeof(FlagPattern));

  if (Driver_EXT_FLASH0.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
    printf("  --  Confirm Flag correctly written %x %x \r\n\n", ConfirmAddress, FlagPattern[0]);
  }
  else
  {
    printf("  -- Confirm Flag Not Correctlty Written \r\n\n");
  }
}

#if (S_DATA_IMAGE_NUMBER == 1)
/**
  * @brief  Write confirm flag for secure data image
  * @param  None
  * @retval None
  */
static void FW_Valid_SecureDataImage(void)
{
  const uint8_t FlagPattern[16]={0x1, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
  const unsigned int ConfirmAddress = S_DATA_OFFSET + S_DATA_PARTITION_SIZE - (sizeof(MagicTrailerValue) + sizeof(FlagPattern));

  if (Driver_EXT_FLASH0.ProgramData(ConfirmAddress, FlagPattern, sizeof(FlagPattern)) == ARM_DRIVER_OK)
  {
    printf("  --  Confirm Flag correctly written %x %x \r\n\n", ConfirmAddress, FlagPattern[0]);
  }
  else
  {
    printf("  -- Confirm Flag Not Correctlty Written \r\n\n");
  }
}
#endif /* S_DATA_IMAGE_NUMBER == 1 */
#endif /* (MCUBOOT_OVERWRITE_ONLY == 0) */

/**
  * @brief Download a new Firmware from the host.
  * @retval HAL status
  */
static HAL_StatusTypeDef FW_UPDATE_DownloadNewFirmware(SFU_FwImageFlashTypeDef *pFwImageDwlArea)
{
  HAL_StatusTypeDef ret = HAL_ERROR;
  COM_StatusTypeDef e_result;
  int32_t ret_arm;
  uint32_t u_fw_size = pFwImageDwlArea->MaxSizeInBytes ;
  uint32_t sector_address;

  /* Clear download area */
  printf("  -- Erasing download area \r\n\n");

  for (sector_address = pFwImageDwlArea->DownloadAddr;
       sector_address < pFwImageDwlArea->DownloadAddr + pFwImageDwlArea->MaxSizeInBytes;
       sector_address += m_uFlashSectorSize)
  {
    ret_arm = Driver_EXT_FLASH0.EraseSector(sector_address);
    if (ret_arm < 0)
    {
      return HAL_ERROR;
    }
  }

  printf("  -- Send Firmware \r\n\n");

  /* Download binary */
  printf("  -- -- File> Transfer> YMODEM> Send \t\n");

  /*Init of Ymodem*/
  Ymodem_Init();

  /*Receive through Ymodem*/
  e_result = Ymodem_Receive(&u_fw_size, pFwImageDwlArea->DownloadAddr);
  printf("\r\n\n");

  if (e_result == COM_OK)
  {
    printf("  -- -- Programming Completed Successfully!\r\n\n");
    printf("  -- -- Bytes: %u\r\n\n", (unsigned int)u_fw_size);
    ret = HAL_OK;
    if (u_fw_size <= (pFwImageDwlArea->MaxSizeInBytes - sizeof(MagicTrailerValue)))
    {
      unsigned int MagicAddress =
        pFwImageDwlArea->DownloadAddr + (pFwImageDwlArea->MaxSizeInBytes - sizeof(MagicTrailerValue));
      /* write the magic to trigger installation at next reset */
      printf("  Write Magic Trailer at %x\r\n\n", MagicAddress);
      if (Driver_EXT_FLASH0.ProgramData(MagicAddress, MagicTrailerValue, sizeof(MagicTrailerValue)) != ARM_DRIVER_OK)
      {
        ret = HAL_ERROR;
      }
    }
  }
  else if (e_result == COM_ABORT)
  {
    printf("  -- -- !!Aborted by user!!\r\n\n");
    COM_Flush();
    ret = HAL_ERROR;
  }
  else
  {
    printf("  -- -- !!Error during file download!!\r\n\n");
    ret = HAL_ERROR;
    HAL_Delay(500U);
    COM_Flush();
  }

  return ret;
}


/**
  * @}
  */

/** @defgroup FW_UPDATE_Callback_Functions Callback Functions
  * @{
  */

/**
  * @brief  Ymodem Header Packet Transfer completed callback.
  * @param  uFileSize Dimension of the file that will be received (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_HeaderPktRxCpltCallback(uint32_t uFlashDestination, uint32_t uFileSize)
{
  /*Reset of the ymodem variables */
  m_uFileSizeYmodem = 0U;
  m_uPacketsReceived = 0U;
  m_uNbrBlocksYmodem = 0U;

  /*Filesize information is stored*/
  m_uFileSizeYmodem = uFileSize;

  /* compute the number of 1K blocks */
  m_uNbrBlocksYmodem = (m_uFileSizeYmodem + (PACKET_1K_SIZE - 1U)) / PACKET_1K_SIZE;

  /* NOTE : delay inserted for Ymodem protocol*/
  HAL_Delay(1000);
  return HAL_OK;
}

/**
  * @brief  Ymodem Data Packet Transfer completed callback.
  * @param  pData Pointer to the buffer.
  * @param  uSize Packet dimension (Bytes).
  * @retval None
  */
HAL_StatusTypeDef Ymodem_DataPktRxCpltCallback(uint8_t *pData, uint32_t uFlashDestination, uint32_t uSize)
{
  int32_t ret;
  m_uPacketsReceived++;

  /* Increase the number of received packets */
  if (m_uPacketsReceived == m_uNbrBlocksYmodem) /*Last Packet*/
  {
    /* Extracting actual payload from last packet */
    if (0 == (m_uFileSizeYmodem % PACKET_1K_SIZE))
    {
      /* The last packet must be fully considered */
      uSize = PACKET_1K_SIZE;
    }
    else
    {
      /* The last packet is not full, drop the extra bytes */
      uSize = m_uFileSizeYmodem - ((uint32_t)(m_uFileSizeYmodem / PACKET_1K_SIZE) * PACKET_1K_SIZE);
    }

    m_uPacketsReceived = 0U;
  }
  /* Adjust dimension to 64-bit length */
  if (uSize %  m_uFlashMinWriteSize != 0U)
  {
    memset(&pData[uSize], 0xff, (m_uFlashMinWriteSize - (uSize % m_uFlashMinWriteSize)));
    uSize += (m_uFlashMinWriteSize - (uSize % m_uFlashMinWriteSize));
  }
  /* Write Data in Flash - size has to be 64-bit aligned */
  ret = Driver_EXT_FLASH0.ProgramData(uFlashDestination, pData,uSize);
  if (ret != ARM_DRIVER_OK)
  {
    /* Reset of the ymodem variables */
    m_uFileSizeYmodem = 0U;
    m_uPacketsReceived = 0U;
    m_uNbrBlocksYmodem = 0U;
    return HAL_ERROR;
  }
  else
    return HAL_OK;
}

/**
  * @}
  */

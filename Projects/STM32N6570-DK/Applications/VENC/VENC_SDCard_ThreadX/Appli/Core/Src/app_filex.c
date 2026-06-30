
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_filex.c
  * @author  MCD Application Team
  * @brief   FileX applicative file
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "app_filex.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "stm32n6570_discovery.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* Main thread stack size */
#define FX_APP_THREAD_STACK_SIZE         2*1024
/* Main thread priority */
#define FX_APP_THREAD_PRIO               10

/* USER CODE BEGIN PD */
#define DEFAULT_QUEUE_LENGTH             16
#define SD_DETECT_Pin GPIO_PIN_12
#define SD_DETECT_GPIO_Port GPION
#define SD_DETECT_EXTI_IRQn EXTI12_IRQn

/* Message content*/
typedef enum {
CARD_STATUS_CHANGED             = 99,
CARD_STATUS_DISCONNECTED        = 88,
CARD_STATUS_CONNECTED           = 77
} SD_ConnectionStateTypeDef;

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
#define MEDIA_CLOSED                     1UL
#define MEDIA_OPENED                     0UL
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* Main thread global data structures.  */
TX_THREAD       fx_app_thread;

/* Buffer for FileX FX_MEDIA sector cache. */
ALIGN_32BYTES (uint32_t fx_sd_media_memory[FX_STM32_SD_DEFAULT_SECTOR_SIZE / sizeof(uint32_t)]);
/* Define FileX global data structures.  */
FX_MEDIA        sdio_disk;

/* USER CODE BEGIN PV */
static UINT media_status;
/* Define FileX global data structures.  */
FX_FILE         fx_file;
/* Define ThreadX global data structures.  */
TX_QUEUE        tx_msg_queue;
ULONG queue_buf[DEFAULT_QUEUE_LENGTH];
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* Main thread entry function.  */
void fx_app_thread_entry(ULONG thread_input);

/* USER CODE BEGIN PFP */
static UINT SD_IsDetected(uint32_t Instance);
static VOID media_close_callback (FX_MEDIA *media_ptr);
/* USER CODE END PFP */

/**
  * @brief  Application FileX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT VENC_FileX_Init(void)
{
  UINT ret = FX_SUCCESS;
  ULONG r_msg;
  ULONG s_msg = CARD_STATUS_CHANGED;
  ULONG last_status = CARD_STATUS_DISCONNECTED;

  /* USER CODE BEGIN 0 */

  /* USER CODE END 0 */

  /* USER CODE BEGIN MX_FileX_Init */

  /* Create the message queue */
  ret = tx_queue_create(&tx_msg_queue, "sd_event_queue", 1, (VOID *) queue_buf, DEFAULT_QUEUE_LENGTH * sizeof(ULONG));

  /* Check msg queue creation */
  if (ret != FX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }
  /* USER CODE END MX_FileX_Init */

  /* Initialize FileX.  */
  fx_system_initialize();

  UINT sd_status = FX_SUCCESS;

  /* Open the SD disk driver */
  sd_status =  fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *) fx_sd_media_memory, sizeof(fx_sd_media_memory));

  /* Check the media open status. */
  if (sd_status != FX_SUCCESS)
  {
    /* USER CODE BEGIN SD open error */
    return sd_status;
    /* USER CODE END SD open error */
  }

  /* USER CODE BEGIN fx_app_thread_entry 1 */
  fx_media_close_notify_set(&sdio_disk, media_close_callback);

  if(SD_IsDetected(FX_STM32_SD_INSTANCE) == HAL_OK)
  {
    /* SD card is already inserted, place the info into the queue */
    tx_queue_send(&tx_msg_queue, &s_msg, TX_NO_WAIT);
  }
  else
  {
    /* Indicate that the SD card is not inserted at startup. */
    BSP_LED_On(LED_RED);
  }


  /* We wait here for a valid SD card insertion event, if it is not inserted already */
  while(1)
  {
    while(tx_queue_receive(&tx_msg_queue, &r_msg, TX_TIMER_TICKS_PER_SECOND / 2) != TX_SUCCESS)
    {
      /* Toggle GREEN LED to indicate idle state after a successful operation */
      if(last_status == CARD_STATUS_CONNECTED)
      {
        BSP_LED_Toggle(LED_GREEN);
      }
    }

    /* check if we received the correct event message */
    if(r_msg == CARD_STATUS_CHANGED)
    {
      /* reset the status */
      r_msg = 0;

      /* for debouncing purpose we wait a bit till it settles down */
      tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND / 2);

        if(SD_IsDetected(FX_STM32_SD_INSTANCE) == HAL_OK)
      {
        /* A valid SD insertion event was detected; proceed with initialization. */
        /* Update last known status */
        last_status = CARD_STATUS_CONNECTED;
        BSP_LED_Off(LED_RED);
        break;
      }
      else
      {
        /* Update last known status */
        last_status = CARD_STATUS_DISCONNECTED;
        BSP_LED_Off(LED_GREEN);
        BSP_LED_Off(LED_RED);
      }
    }
  }

  /* Reopen the media if it was previously closed. */
  if (media_status == MEDIA_CLOSED)
  {
    sd_status =  fx_media_open(&sdio_disk, FX_SD_VOLUME_NAME, fx_stm32_sd_driver, (VOID *)FX_NULL, (VOID *) fx_sd_media_memory, sizeof(fx_sd_media_memory));
    /* Check the media open status. */
    if (sd_status != FX_SUCCESS)
     {
    /* Error opening file, call error handler.  */
    return sd_status;
     }
    media_status = MEDIA_OPENED;
  }
  /* USER CODE END fx_app_thread_entry 1 */
  return 0;
}

/* USER CODE BEGIN 1 */

UINT VENC_FileX_Open(CHAR * filename)
{
  UINT sd_status = FX_SUCCESS;
  
  sd_status = fx_file_delete(&sdio_disk, filename);
  if (sd_status != FX_SUCCESS)
  {
    /* Check for an already created status. This is expected on the
    second pass of this loop!  */
    if (sd_status != FX_NOT_FOUND)
    {
      /* Create error, call error handler.  */
      return sd_status;
    }
  }
  sd_status =  fx_file_create(&sdio_disk, filename);
  /* Check the create status.  */
  if (sd_status != FX_SUCCESS)
  {
    return sd_status;
  }
  
  /* Open the requested file. */
  sd_status =  fx_file_open(&sdio_disk, &fx_file, filename, FX_OPEN_FOR_WRITE);
  
  /* Check the file open status.  */
  if (sd_status != FX_SUCCESS)
  {
    /* Error opening file, call error handler.  */
    return sd_status;
  }
  /* USER CODE END fx_app_thread_entry 1 */
  return 0;
}

UINT VENC_FileX_write(CHAR * data, LONG size)
{
  /* Write the given data to the file.  */
  UINT status =  fx_file_write(&fx_file, data, size);

  return status;
}

UINT VENC_FileX_close(void)
{
  /* Close the current file. */
  UINT status =  fx_file_close(&fx_file);
  /* Check the file close status.  */
  if (status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler.  */
    return status;
  }

  status = fx_media_flush(&sdio_disk);
  /* Check the media flush  status.  */
  if(status != FX_SUCCESS)
  {
    /* Error closing the file, call error handler.  */
    return status;
  }
  /* Close the media.  */
  //status =  fx_media_close(&sdio_disk);

  return status;
}




/**
 * @brief  Detects if SD card is correctly plugged in the memory slot or not.
 * @param Instance  SD Instance
 * @retval Returns if SD is detected or not
 */
static UINT SD_IsDetected(uint32_t Instance)
{
  UINT ret;

  if(Instance >= 1)
  {
    ret = HAL_ERROR;
  }
  else
  {
    /* Check SD card detect pin */
    if (HAL_GPIO_ReadPin(SD_DETECT_GPIO_Port, SD_DETECT_Pin) == GPIO_PIN_SET)
    {
      ret = HAL_ERROR;
    }
    else
    {
      ret = HAL_OK;
    }
  }

  return ret;
}

/**
  * @brief  EXTI line detection callback.
  * @param  GPIO_Pin: Specifies the port pin connected to corresponding EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
  ULONG s_msg = CARD_STATUS_CHANGED;

  if(GPIO_Pin == SD_DETECT_Pin)
  {
    tx_queue_send(&tx_msg_queue, &s_msg, TX_NO_WAIT);
  }
}

/**
  * @brief  Media close notify callback function.
  * @param  media_ptr: Media control block pointer
  * @retval None
  */
static VOID media_close_callback(FX_MEDIA *media_ptr)
{
  media_status = MEDIA_CLOSED;
}

/* USER CODE END 1 */

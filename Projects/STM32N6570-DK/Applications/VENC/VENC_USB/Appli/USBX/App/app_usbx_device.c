/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
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
#include "app_usbx_device.h"
#include "tx_api.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

static ULONG video_interface_number;
static ULONG video_configuration_number;
static UX_DEVICE_CLASS_VIDEO_PARAMETER video_parameter;
static UX_DEVICE_CLASS_VIDEO_STREAM_PARAMETER video_stream_parameter[USBD_VIDEO_STREAM_NMNBER];
static TX_THREAD ux_device_app_thread;
static TX_THREAD ux_video_thread;
TX_SEMAPHORE video_thread_semaphore;
VOID app_video_thread_entry(ULONG thread_input);

ULONG _ux_utility_time_get(VOID);

/* USER CODE BEGIN PV */
extern PCD_HandleTypeDef hpcd_USB1_OTG_HS;

TX_QUEUE ux_app_MsgQueue_UCPD;
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_MODE_STATE USB_Device_State_Msg __ALIGN_END;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_device_thread_entry(ULONG thread_input);
/* USER CODE BEGIN PFP */
ALIGN_32BYTES(uint32_t DataBuffer[512/ sizeof(uint32_t)]);
/* USER CODE END PFP */

/* USER CODE BEGIN UX_Device_Pool_Buffer */
/* USER CODE END UX_Device_Pool_Buffer */
#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static UCHAR ux_device_cached_buffer[UX_DEVICE_APP_MEM_CACHED_SIZE]  __ALIGN_END ;

#if defined ( __ICCARM__ )
#pragma data_alignment=4
#endif
__ALIGN_BEGIN static UCHAR ux_device_uncached_buffer[UX_DEVICE_APP_MEM_UNCACHED_SIZE] IN_UNCACHED_RAM __ALIGN_END ;

ULONG _ux_utility_time_get(VOID)
{
  return tx_time_get();
}



/**
  * @brief  Application USBX Device Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */
UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  UCHAR *device_framework_high_speed;
  UCHAR *device_framework_full_speed;
  ULONG device_framework_hs_length;
  ULONG device_framework_fs_length;
  ULONG string_framework_length;
  ULONG language_id_framework_length;
  UCHAR *string_framework;
  UCHAR *language_id_framework;
  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;

  /* USER CODE BEGIN MX_USBX_Device_Init0 */

  /* USER CODE END MX_USBX_Device_Init0 */



  /* Initialize USBX Memory */
  if (ux_system_initialize(ux_device_uncached_buffer, UX_DEVICE_APP_MEM_UNCACHED_SIZE, ux_device_cached_buffer, UX_DEVICE_APP_MEM_CACHED_SIZE) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_SYSTEM_INITIALIZE_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_SYSTEM_INITIALIZE_ERORR */
  }

  /* Get Device Framework High Speed and get the length */
  device_framework_high_speed = USBD_Get_Device_Framework_Speed(USBD_HIGH_SPEED,
                                                                &device_framework_hs_length);

  /* Get Device Framework Full Speed and get the length */
  device_framework_full_speed = USBD_Get_Device_Framework_Speed(USBD_FULL_SPEED,
                                                                &device_framework_fs_length);

  /* Get String Framework and get the length */
  string_framework = USBD_Get_String_Framework(&string_framework_length);

  /* Get Language Id Framework and get the length */
  language_id_framework = USBD_Get_Language_Id_Framework(&language_id_framework_length);

  /* Install the device portion of USBX */
  if (ux_device_stack_initialize(device_framework_high_speed,
                                 device_framework_hs_length,
                                 device_framework_full_speed,
                                 device_framework_fs_length,
                                 string_framework,
                                 string_framework_length,
                                 language_id_framework,
                                 language_id_framework_length,
                                 UX_NULL) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_INITIALIZE_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_INITIALIZE_ERORR */
  }

  /* Initialize the video class parameters for the device */
  video_stream_parameter[0].ux_device_class_video_stream_parameter_callbacks.ux_device_class_video_stream_change
    = USBD_VIDEO_StreamChange;

  video_stream_parameter[0].ux_device_class_video_stream_parameter_callbacks.ux_device_class_video_stream_payload_done
    = USBD_VIDEO_StreamPayloadDone;

  video_stream_parameter[0].ux_device_class_video_stream_parameter_callbacks.ux_device_class_video_stream_request
    = USBD_VIDEO_StreamRequest;

  video_stream_parameter[0].ux_device_class_video_stream_parameter_max_payload_buffer_nb
    = USBD_VIDEO_PAYLOAD_BUFFER_NUMBER;

  video_stream_parameter[0].ux_device_class_video_stream_parameter_max_payload_buffer_size
    = USBD_VIDEO_StreamGetMaxPayloadBufferSize();

#if defined(UX_DEVICE_STANDALONE)
  video_stream_parameter[0].ux_device_class_video_stream_parameter_task_function 
    = ux_device_class_video_write_task_function;
#else
  video_stream_parameter[0].ux_device_class_video_stream_parameter_thread_entry
    = ux_device_class_video_write_thread_entry;
#endif

  /* Set the parameters for Video device */
  video_parameter.ux_device_class_video_parameter_streams_nb  = USBD_VIDEO_STREAM_NMNBER;
  video_parameter.ux_device_class_video_parameter_streams     = video_stream_parameter;

  video_parameter.ux_device_class_video_parameter_callbacks.ux_slave_class_video_instance_activate
    = USBD_VIDEO_Activate;

  video_parameter.ux_device_class_video_parameter_callbacks.ux_slave_class_video_instance_deactivate
    = USBD_VIDEO_Deactivate;

  /* USER CODE BEGIN VIDEO_PARAMETER */

  /* USER CODE END VIDEO_PARAMETER */

  /* Get video configuration number */
  video_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_VIDEO, 0);

  /* Find video interface number */
  video_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_VIDEO, 0);

  /* Initialize the device VIDEO */
  if (ux_device_stack_class_register(_ux_system_device_class_video_name,
                                     ux_device_class_video_entry,
                                     video_configuration_number,
                                     video_interface_number,
                                     (VOID *)&video_parameter) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_VIDEO_REGISTER_ERORR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_VIDEO_REGISTER_ERORR */
  }

  /* Allocate the stack for device application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERORR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERORR */
  }

  /* Create the device application main thread */
  if (tx_thread_create(&ux_device_app_thread, UX_DEVICE_APP_THREAD_NAME, app_ux_device_thread_entry,
                       0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE, UX_DEVICE_APP_THREAD_PRIO,
                       UX_DEVICE_APP_THREAD_PREEMPTION_THRESHOLD, UX_DEVICE_APP_THREAD_TIME_SLICE,
                       UX_DEVICE_APP_THREAD_START_OPTION) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERORR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERORR */
  }

  /* USER CODE BEGIN MX_USBX_Device_Init1 */

  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, APP_QUEUE_SIZE*sizeof(ULONG),
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the MsgQueue */
  if (tx_queue_create(&ux_app_MsgQueue_UCPD, "Message Queue app", TX_1_ULONG,
                      pointer, APP_QUEUE_SIZE*sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }
  
    /* Create the MsgQueue */
  
  if(tx_semaphore_create(&video_thread_semaphore, "Video Semaphore", 0) != UX_SUCCESS)
  {
    return  UX_SEMAPHORE_ERROR;
  }
 
   /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_VIDEO_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the device application main thread */
  if (tx_thread_create(&ux_video_thread, UX_VIDEO_THREAD_NAME, app_video_thread_entry,
                       0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                       UX_VIDEO_THREAD_PRIO, UX_VIDEO_THREAD_PRIO, 
                       TX_NO_TIME_SLICE, TX_AUTO_START) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERORR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERORR */
  }

  /* USER CODE END MX_USBX_Device_Init1 */
  return ret;
  
}

#if defined(UX_DEVICE_STANDALONE)
ALIGN_TYPE _ux_utility_interrupt_disable(VOID)
{
  ALIGN_TYPE ret = __get_PRIMASK();

  __disable_irq();

  return ret;
}

VOID _ux_utility_interrupt_restore(ALIGN_TYPE flags)
{
  if (!flags)
    __enable_irq();
}
#endif

/**
  * @brief  Function implementing app_ux_device_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_device_thread_entry(ULONG thread_input)
{


  /* Initialization of USB device */
  USBX_APP_Device_Init();

  while (1)
  {
    /* Wait for a device to be connected */
    if ( tx_queue_receive(&ux_app_MsgQueue_UCPD, &USB_Device_State_Msg,
                         TX_WAIT_FOREVER)!= TX_SUCCESS)
    {
      /*Error*/
      Error_Handler();
    }
    /* Check if received message equal to USB_PCD_START */
    if (USB_Device_State_Msg == START_USB_DEVICE)
    {
      /* Start device USB */
      HAL_PCD_Start(&hpcd_USB1_OTG_HS);
    }
    /* Check if received message equal to USB_PCD_STOP */
    else if (USB_Device_State_Msg == STOP_USB_DEVICE)
    {
      /* Stop device USB */
      HAL_PCD_Stop(&hpcd_USB1_OTG_HS);
    }
    /* Else Error */
    else
    {
      /*Error*/
      Error_Handler();
    }
  }
  /* USER CODE END app_ux_device_thread_entry */
}

/* USER CODE BEGIN 1 */

/**
  * @brief USBX_APP_Device_Init
  *        Initialization of USB device.
  * @retval None
  */
VOID USBX_APP_Device_Init(VOID)
{
  /* USER CODE BEGIN USB_Device_Init_PreTreatment_0 */

  /* USER CODE END USB_Device_Init_PreTreatment_0 */

  /* USB_OTG_HS init function */
  MX_USB1_OTG_HS_PCD_Init();

  /* USER CODE BEGIN USB_Device_Init_PreTreatment_1 */
  HAL_PCDEx_SetRxFiFo(&hpcd_USB1_OTG_HS, 0x100);
  HAL_PCDEx_SetTxFiFo(&hpcd_USB1_OTG_HS, 0, 0x10);
  HAL_PCDEx_SetTxFiFo(&hpcd_USB1_OTG_HS, 1, 0x200);
  /* USER CODE END USB_Device_Init_PreTreatment_1 */

  /* Initialize and link controller HAL driver */
  ux_dcd_stm32_initialize((ULONG)USB1_OTG_HS, (ULONG)&hpcd_USB1_OTG_HS);

  /* USER CODE END USB_Device_Init_PostTreatment */
}

/* USER CODE END 1 */

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_usbx_device.c
  * @author  MCD Application Team
  * @brief   USBX Device applicative file
  ******************************************************************************
    * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_device_audio.h"
#include "ux_device_audio_play.h"
#include "ux_device_class_audio20.h"
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

static ULONG audio_interface_number;
static ULONG audio_configuration_number;
static UX_DEVICE_CLASS_AUDIO_PARAMETER audio_parameter;
static UX_DEVICE_CLASS_AUDIO_STREAM_PARAMETER audio_stream_parameter[USBD_AUDIO_STREAM_NMNBER];
static uint8_t audio_stream_index = 0U;
static TX_THREAD ux_device_app_thread;

/* USER CODE BEGIN PV */
extern PCD_HandleTypeDef hpcd_USB_OTG_HS1;
TX_QUEUE                 ux_app_MsgQueue_UCPD;
#if USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K
static TX_THREAD ux_audio_play_thread;
TX_QUEUE ux_app_MsgQueue;
#endif /* USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K */
BSP_AUDIO_Init_t AudioPlayInit;
#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
static TX_THREAD   monitor_thread;
ULONG monitor_thread_stack[MONITOR_THREAD_STACK_SIZE];
#endif
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma data_alignment=4
#endif /* defined ( __ICCARM__ ) */
__ALIGN_BEGIN USB_MODE_STATE USB_Device_State_Msg __ALIGN_END;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID app_ux_device_thread_entry(ULONG thread_input);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Device Initialization.
  * @param  memory_ptr: memory pointer
  * @retval status
  */

UINT MX_USBX_Device_Init(VOID *memory_ptr)
{
  UINT ret = UX_SUCCESS;
  UCHAR *pointer;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  /* USER CODE BEGIN MX_USBX_Device_Init 0 */
  /* USER CODE END MX_USBX_Device_Init 0 */

  /* USER CODE BEGIN MX_USBX_Device_Init 1 */
  /* USER CODE END MX_USBX_Device_Init 1 */

  /* Allocate the stack for device application main thread */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                       TX_NO_WAIT) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_ALLOCATE_STACK_ERROR */
    return TX_POOL_ERROR;
    /* USER CODE END MAIN_THREAD_ALLOCATE_STACK_ERROR */
  }

  /* Create the device application main thread */
  if (tx_thread_create(&ux_device_app_thread, UX_DEVICE_APP_THREAD_NAME, app_ux_device_thread_entry,
                       0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE, UX_DEVICE_APP_THREAD_PRIO,
                       UX_DEVICE_APP_THREAD_PREEMPTION_THRESHOLD, UX_DEVICE_APP_THREAD_TIME_SLICE,
                       UX_DEVICE_APP_THREAD_START_OPTION) != TX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_THREAD_CREATE_ERROR */
    return TX_THREAD_ERROR;
    /* USER CODE END MAIN_THREAD_CREATE_ERROR */
  }

  /* USER CODE BEGIN MX_USBX_Device_Init 2 */


  /* USER CODE BEGIN MX_USBX_Device_Init1 */

#if USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K
  /* Allocate the stack for main usbx audio thread.  */
  if(tx_byte_allocate(byte_pool, (VOID **) &pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                      TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main usbx audio thread.  */
  if(tx_thread_create(&ux_audio_play_thread, "usbx_audio_app_thread_entry",
                      usbx_audio_play_app_thread, 0, pointer, UX_DEVICE_APP_THREAD_STACK_SIZE,
                      20, 20, 1, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
#endif /* USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K */

#if defined(TX_ENABLE_EXECUTION_CHANGE_NOTIFY) || defined(TX_EXECUTION_PROFILE_ENABLE)
  /* Allocate the stack for monitor thread.  */
  if(tx_byte_allocate(byte_pool, (VOID **) &pointer, MONITOR_THREAD_STACK_SIZE,
                      TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  if(tx_thread_create(&monitor_thread, "playback_monitor_thread", playback_monitor_thread, 0, 
                      monitor_thread_stack, MONITOR_THREAD_STACK_SIZE, MONITOR_THREAD_PRIORITY, 
                      MONITOR_THREAD_PRIORITY,1, TX_AUTO_START) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }
#endif

#if USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K
  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* Create the MsgQueue */
  if (tx_queue_create(&ux_app_MsgQueue, "Message Queue app", TX_1_ULONG,
                      pointer, APP_QUEUE_SIZE * sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }
#endif /* USBD_AUDIO_PLAY_FREQ_MAX == USBD_AUDIO_FREQ_44_1_K */

  /* Allocate Memory for the Queue */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer,
                       APP_QUEUE_SIZE * sizeof(ULONG), TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  if (tx_queue_create(&ux_app_MsgQueue_UCPD, "Message Queue app USBPD", TX_1_ULONG,
                      pointer, APP_QUEUE_SIZE*sizeof(ULONG)) != TX_SUCCESS)
  {
    return TX_QUEUE_ERROR;
  }

  /* USER CODE END MX_USBX_Device_Init 2 */

  return ret;
}

/**
  * @brief  MX_USBX_Device_Stack_Init
  *         Intialization of USB Device.
  *         Initialize the device stack, register of device class stack
  *         Register of the usb device controller
  * @param  None
  * @retval ret
  */
UINT MX_USBX_Device_Stack_Init(void)
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

  /* USER CODE BEGIN MX_USBX_Device_Stack_Init 0 */

  /* USER CODE END MX_USBX_Device_Stack_Init 0 */
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
    /* USER CODE BEGIN USBX_DEVICE_INITIALIZE_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_INITIALIZE_ERROR */
  }

  /* Initialize audio playback control values */
  USBD_AUDIO_SetControlValues();

  /* Initialize the audio class parameters for audio playback */
  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_change
    = USBD_AUDIO_PlaybackStreamChange;

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_callbacks.ux_device_class_audio_stream_frame_done
    = USBD_AUDIO_PlaybackStreamFrameDone;

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_max_frame_buffer_nb
    = USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber();

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_max_frame_buffer_size
    = USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize();

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_thread_entry
    = ux_device_class_audio_read_thread_entry;

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_feedback_thread_entry
    = ux_device_class_audio_feedback_thread_entry;
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

  /* Set the parameters for audio device */
  audio_parameter.ux_device_class_audio_parameter_streams_nb  = USBD_AUDIO_STREAM_NMNBER;
  audio_parameter.ux_device_class_audio_parameter_streams     = audio_stream_parameter;

  audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_activate
    = USBD_AUDIO_Activate;

  audio_parameter.ux_device_class_audio_parameter_callbacks.ux_slave_class_audio_instance_deactivate
    = USBD_AUDIO_Deactivate;

 audio_parameter.ux_device_class_audio_parameter_callbacks.ux_device_class_audio_control_process
    = USBD_AUDIO_ControlProcess;

  /* Get audio configuration number */
  audio_configuration_number = USBD_Get_Configuration_Number(CLASS_TYPE_AUDIO_20, 0);

  /* Find audio interface number */
  audio_interface_number = USBD_Get_Interface_Number(CLASS_TYPE_AUDIO_20, 0);
  /* Initialize the device audio class */
  if (ux_device_stack_class_register(_ux_system_slave_class_audio_name,
                                     ux_device_class_audio_entry,
                                     audio_configuration_number,
                                     audio_interface_number,
                                     &audio_parameter) != UX_SUCCESS)
  {
    /* USER CODE BEGIN USBX_DEVICE_AUDIO_REGISTER_ERROR */
    return UX_ERROR;
    /* USER CODE END USBX_DEVICE_AUDIO_REGISTER_ERROR */
  }

  /* Initialize and link controller HAL driver */
  ux_dcd_stm32_initialize((ULONG)USB1_OTG_HS, (ULONG)&hpcd_USB_OTG_HS1);
  /* USER CODE BEGIN MX_USBX_Device_Stack_Init_PostTreatment */
  /* USER CODE END MX_USBX_Device_Stack_Init_PostTreatment */

  /* USER CODE BEGIN MX_USBX_Device_Stack_Init 1 */

  /* USER CODE END MX_USBX_Device_Stack_Init 1 */

  return ret;
}

/**
  * @brief  Function implementing app_ux_device_thread_entry.
  * @param  thread_input: User thread input parameter.
  * @retval none
  */
static VOID app_ux_device_thread_entry(ULONG thread_input)
{
  /* USER CODE BEGIN app_ux_device_thread_entry */

  if (USBD_AUDIO_PlaybackInit() != UX_SUCCESS)
  {
    Error_Handler();
  }

  /* Wait for message queue to start/stop the device */
  while(1)
  {
    /* Wait for a device to be connected */
    if (tx_queue_receive(&ux_app_MsgQueue_UCPD, &USB_Device_State_Msg,
                         TX_WAIT_FOREVER)!= TX_SUCCESS)
    {
      /*Error*/
      Error_Handler();
    }
    /* Check if received message equal to USB_PCD_START */
    if (USB_Device_State_Msg == START_USB_DEVICE)
    {
      /* Initialize the audio peripherals */
      AudioPlayInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
      AudioPlayInit.ChannelsNbr = USBD_AUDIO_PLAY_CHANNEL_COUNT;
      AudioPlayInit.SampleRate = USBD_AUDIO_PLAY_DEFAULT_FREQ;
      AudioPlayInit.BitsPerSample = USBD_AUDIO_PLAY_RES_BIT;
      AudioPlayInit.Volume = VOLUME_SPEAKER_DEFAULT;

      if(BSP_AUDIO_OUT_Init(0, &AudioPlayInit) != BSP_ERROR_NONE)
      {
        Error_Handler();
      }
      if (USBD_AUDIO_PlaybackStartSilent() != UX_SUCCESS)
      {
        Error_Handler();
      }
      /* USB_OTG_HS init function */
      MX_USB1_OTG_HS_PCD_Init();

      if (MX_USBX_Device_Stack_Init() != UX_SUCCESS)
      {
        /* USER CODE BEGIN MAIN_INITIALIZE_STACK_ERROR */
        Error_Handler();
        /* USER CODE END MAIN_INITIALIZE_STACK_ERROR */
      }

      /* Start device USB */
      HAL_PCD_Start(&hpcd_USB_OTG_HS1);
    }
    /* Check if received message equal to USB_PCD_STOP */
    else if (USB_Device_State_Msg == STOP_USB_DEVICE)
    {

      /* Stop audio playback before disconnecting the USB device and stopping the USB controller. */
      BSP_AUDIO_OUT_Stop(0);

      /* Deinitialize the audio output peripheral after playback has been halted to release resources. */
      BSP_AUDIO_OUT_DeInit(0);

      /* Disconnect USBX stack driver,  */
      ux_device_stack_disconnect();

      /* Stop device USB */
      HAL_PCD_Stop(&hpcd_USB_OTG_HS1);

      /* Uninitialize the device stack */
      if (MX_USBX_Device_Stack_DeInit() != UX_SUCCESS)
      {
        /* USER CODE BEGIN MAIN_DEINITIALIZE_STACK_ERROR */
        Error_Handler();
        /* USER CODE END MAIN_DEINITIALIZE_STACK_ERROR */
      }

      /* DeInit USB OTG HS */
      HAL_PCD_DeInit(&hpcd_USB_OTG_HS1);
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

/**
  * @brief  MX_USBX_Device_Stack_DeInit
  *         Unitialization of USB Device.
  *         uninitialize the device stack, unregister of device class stack
  *         unregister of the usb device controller
  * @retval ret
  */
UINT MX_USBX_Device_Stack_DeInit(void)
{
  UINT ret = UX_SUCCESS;

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PreTreatment_0 */
   /* Unregister audio class. */
  if (ux_device_stack_class_unregister(_ux_system_slave_class_audio_name,
                                       ux_device_class_audio_entry) != UX_SUCCESS)
  {
    return UX_ERROR;
  }
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PreTreatment_0 */

  /* Unregister USB device controller. */

  if (_ux_dcd_stm32_uninitialize((ULONG)USB1_OTG_HS, (ULONG)&hpcd_USB_OTG_HS1) != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* The code below is required for uninstalling the device portion of USBX.  */
  if (ux_device_stack_uninitialize() != UX_SUCCESS)
  {
    return UX_ERROR;
  }

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PreTreatment_1 */
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PreTreatment_1 */

  /* USER CODE BEGIN MX_USBX_Device_Stack_DeInit_PostTreatment */
  /* USER CODE END MX_USBX_Device_Stack_DeInit_PostTreatment */

  return ret;
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

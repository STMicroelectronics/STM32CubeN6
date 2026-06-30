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
#include "stm32n6570_discovery_audio.h"
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

/* USER CODE BEGIN PV */
extern PCD_HandleTypeDef            hpcd_USB_OTG_HS1;

BSP_AUDIO_Init_t AudioPlayInit;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application USBX Device Initialization.
  * @param  none
  * @retval status
  */

UINT MX_USBX_Device_Init(VOID)
{
  UINT ret = UX_SUCCESS;
  /* USER CODE BEGIN MX_USBX_Device_Init 0 */
  /* USB_OTG_HS init function */
   MX_USB1_OTG_HS_PCD_Init();

  /* USER CODE END MX_USBX_Device_Init 0 */

  /* Initialize the Stack USB Device*/
  if (MX_USBX_Device_Stack_Init() != UX_SUCCESS)
  {
    /* USER CODE BEGIN MAIN_INITIALIZE_STACK_ERROR */
    Error_Handler();
    /* USER CODE END MAIN_INITIALIZE_STACK_ERROR */
  }

  /* USER CODE BEGIN MX_USBX_Device_Init 1 */
  /* Start the USB device */
  HAL_PCD_Start(&hpcd_USB_OTG_HS1);

  /* USER CODE END MX_USBX_Device_Init 1 */

  /* USER CODE BEGIN MX_USBX_Device_Init 2 */
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

  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_task_function
    = ux_device_class_audio_read_task_function;

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
  audio_stream_parameter[audio_stream_index].ux_device_class_audio_stream_parameter_feedback_task_function
    = ux_device_class_audio_feedback_task_function;
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
  AudioPlayInit.Device = AUDIO_OUT_DEVICE_HEADPHONE;
  AudioPlayInit.ChannelsNbr = USBD_AUDIO_PLAY_CHANNEL_COUNT;
  AudioPlayInit.SampleRate = USBD_AUDIO_PLAY_DEFAULT_FREQ;
  AudioPlayInit.BitsPerSample = USBD_AUDIO_PLAY_RES_BIT;
  AudioPlayInit.Volume = VOLUME_SPEAKER_DEFAULT;
  if (USBD_AUDIO_PlaybackInit() != UX_SUCCESS)
  {
    Error_Handler();
  }
  if(BSP_AUDIO_OUT_Init(0, &AudioPlayInit) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  if (USBD_AUDIO_PlaybackStartSilent() != UX_SUCCESS)
  {
    Error_Handler();
  }
  /* USER CODE END MX_USBX_Device_Stack_Init 1 */

  return ret;
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

/**
  * @brief  MX_USBX_Device_Process
  *         Run USBX state machine.
  * @param  arg: not used
  * @retval none
  */
VOID USBX_Device_Process(VOID *arg)
{
  ux_device_stack_tasks_run();
}

/* USER CODE END 1 */

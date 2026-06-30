/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_play.c
  * @author  MCD Application Team
  * @brief   USBX Device Audio PlayBack applicative source file
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
#include "ux_device_audio_play.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "ux_device_class_audio20.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
#define BUF_OVERRUN_THRESHOLD                            ((3U * AUDIO_TOTAL_BUF_SIZE) / 4U)
#define BUF_UNDERRUN_THRESHOLD                           (AUDIO_TOTAL_BUF_SIZE / 4U)
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

#define AUDIO_HS_SERVICE_INTERVALS_PER_SECOND           (8000U / (1U << (USBD_AUDIO_PLAY_EPOUT_HS_BINTERVAL - 1U)))
#define AUDIO_FS_SERVICE_INTERVALS_PER_SECOND           (1000U / (1U << (USBD_AUDIO_PLAY_EPOUT_FS_BINTERVAL - 1U)))

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

static uint8_t playback_started;

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
static uint8_t feedback_buffer[4];
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

/* Set BufferCtl start address */
#if defined ( __ICCARM__ ) /* IAR Compiler */
#pragma location = ".AudioStreamBufferSection"
#else
__attribute__((section(".AudioStreamBufferSection")))
#endif
/* Double BUFFER for Output Audio stream */
__ALIGN_BEGIN AUDIO_OUT_BufferTypeDef  BufferCtl __ALIGN_END;
AUDIO_DescriptionTypeDef PlaybackAudioDescription;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
static VOID USBD_AUDIO_PlaybackResetState(VOID);
static uint32_t USBD_AUDIO_PlaybackGetReadPosition(VOID);

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
static uint32_t Audio_get_feedback(uint8_t speed);
static VOID USBD_AUDIO_PlaybackFeedbackEncode(uint32_t feedback_data_rate, uint8_t *feedback_buffer);
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  USBD_AUDIO_PlaybackResetState
  *         This function resets the playback ring buffer state.
  * @param  none
  * @retval none
  */
static VOID USBD_AUDIO_PlaybackResetState(VOID)
{
  uint32_t read_position;

  ux_utility_memory_set(BufferCtl.buff, 0, sizeof(BufferCtl.buff));
  BufferCtl.state = PLAY_BUFFER_OFFSET_UNKNOWN;

  /* Keep the pointers aligned with the current DMA reader. */
  read_position = USBD_AUDIO_PlaybackGetReadPosition();

  BufferCtl.rd_ptr = read_position;
  BufferCtl.wr_ptr = read_position;
}

/**
  * @brief  USBD_AUDIO_PlaybackGetReadPosition
  *         This function returns the current playback read position in the ring buffer.
  * @param  none
  * @retval read position in bytes
  */
static uint32_t USBD_AUDIO_PlaybackGetReadPosition(VOID)
{
  uint32_t dma_transfer_count;
  uint32_t dma_remaining_transfer_count;
  uint32_t read_position;

  /* Keep the last software read pointer if the DMA counter is not valid yet. */
  if ((playback_started == 0U) ||
      (BufferCtl.size == 0U) ||
      (USBD_AUDIO_PLAY_RES_BYTE == 0U) ||
      (haudio_out_sai.hdmatx == NULL))
  {
    return BufferCtl.rd_ptr;
  }

  dma_transfer_count = BufferCtl.size / USBD_AUDIO_PLAY_RES_BYTE;

  if (dma_transfer_count == 0U)
  {
    return BufferCtl.rd_ptr;
  }

  /* Translate the DMA remaining item count into a byte offset inside the circular playback buffer. */
  dma_remaining_transfer_count = (uint32_t)__HAL_DMA_GET_COUNTER(haudio_out_sai.hdmatx);

  /* Reject out-of-range values and keep the last known software position if the DMA state is transient. */
  if (dma_remaining_transfer_count > dma_transfer_count)
  {
    return BufferCtl.rd_ptr;
  }

  read_position = (dma_transfer_count - dma_remaining_transfer_count) * USBD_AUDIO_PLAY_RES_BYTE;

  if (read_position >= BufferCtl.size)
  {
    read_position -= BufferCtl.size;
  }

  return read_position;
}

/* USER CODE END 0 */

/**
  * @brief  USBD_AUDIO_PlaybackStreamChange
  *         This function is invoked to inform application that the
  *         alternate setting are changed.
  * @param  audio_play_stream: Pointer to audio playback class stream instance.
  * @param  alternate_setting: interface alternate setting.
  * @retval none
  */
VOID USBD_AUDIO_PlaybackStreamChange(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                     ULONG alternate_setting)
{
  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamChange */

  /* Do nothing if alternate setting is 0 (stream closed). */
  if (alternate_setting == 0)
  {
    USBD_AUDIO_PlaybackResetState();
    return;
  }

  USBD_AUDIO_PlaybackResetState();

  /* Start reception when the stream is opened. */
  ux_device_class_audio_reception_start(audio_play_stream);

  /* USER CODE END USBD_AUDIO_PlaybackStreamChange */

  return;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamFrameDone
  *         This function is invoked whenever a USB packet (audio frame) is received
  *         from the host.
  * @param  audio_play_stream: Pointer to audio playback class stream instance.
  * @param  length: transfer length.
  * @retval none
  */
VOID USBD_AUDIO_PlaybackStreamFrameDone(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                        ULONG length)
{
  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamFrameDone */
  UCHAR *frame_buffer;
  ULONG frame_length;
  ULONG write_position;

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
  uint32_t feedback_data_rate;
  uint8_t speed;
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

  /* Get access to the first audio input frame. */
  ux_device_class_audio_read_frame_get(audio_play_stream, &frame_buffer, &frame_length);

  if (length != 0U)
  {
    write_position = BufferCtl.wr_ptr;

    /* Split the USB frame when it crosses the end of the circular playback buffer. */
    if ((write_position + frame_length) > BufferCtl.size)
    {
      ULONG remaining_length;

      remaining_length = BufferCtl.size - write_position;
      ux_utility_memory_copy(&BufferCtl.buff[write_position], frame_buffer, remaining_length);
      ux_utility_memory_copy(&BufferCtl.buff[0], &frame_buffer[remaining_length], frame_length - remaining_length);
    }
    else
    {
      ux_utility_memory_copy(&BufferCtl.buff[write_position], frame_buffer, frame_length);
    }

    BufferCtl.wr_ptr += frame_length;

    if (BufferCtl.wr_ptr >= BufferCtl.size)
    {
      BufferCtl.wr_ptr -= BufferCtl.size;

      if (BufferCtl.state == PLAY_BUFFER_OFFSET_UNKNOWN)
      {
        /* Mark the first complete wrap. */
        BufferCtl.state = PLAY_BUFFER_OFFSET_FULL;
      }
    }
  }

  /* Release the first audio input frame so USBX can reuse it. */
  ux_device_class_audio_read_frame_free(audio_play_stream);

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT

  /* Get the active USB bus speed. */
  speed = ux_device_class_audio_speed_get(audio_play_stream);

  /* Derive the feedback value from the current buffer level. */
  feedback_data_rate = Audio_get_feedback(speed);

  /* Prepare the local feedback buffer before submitting the next IN transfer. */
  USBD_AUDIO_PlaybackFeedbackEncode(feedback_data_rate, feedback_buffer);

  /* Submit the feedback payload for the next IN transaction. */
  ux_device_class_audio_feedback_set(audio_play_stream, feedback_buffer);
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */

  /* USER CODE END USBD_AUDIO_PlaybackStreamFrameDone */

  return;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber
  *         This function is invoked to Set audio playback stream max Frame buffer number.
  * @param  none
  * @retval max frame buffer number
  */
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber(VOID)
{
  ULONG max_frame_buffer_number = 0U;

  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber */

  max_frame_buffer_number = 3U;

  /* USER CODE END USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber */

  return max_frame_buffer_number;
}

/**
  * @brief  USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize
  *         This function is invoked to Set audio playback stream max Frame buffer size.
  * @param  none
  * @retval max frame buffer size
  */
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize(VOID)
{
  ULONG max_frame_buffer_size = 0U;

  /* USER CODE BEGIN USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize */

  max_frame_buffer_size = USBD_AUDIO_PLAY_EPOUT_HS_MPS;

  /* USER CODE END USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize */

  return max_frame_buffer_size;
}

/* USER CODE BEGIN 1 */

/**
  * @brief  USBD_AUDIO_PlaybackInit
  *         Initializes playback state, format, and packet sizing.
  * @param  none
  * @retval UX_SUCCESS on success, or an error code if initialization fails.
  */
UINT USBD_AUDIO_PlaybackInit(VOID)
{
  PlaybackAudioDescription.audio_frequency      = USBD_AUDIO_PLAY_DEFAULT_FREQ;
  PlaybackAudioDescription.audio_channels_count = USBD_AUDIO_PLAY_CHANNEL_COUNT;
  PlaybackAudioDescription.audio_resolution     = USBD_AUDIO_PLAY_RES_BYTE;
  BufferCtl.size                                = AUDIO_TOTAL_BUF_SIZE;
  playback_started = 0U;
  ux_utility_memory_set(BufferCtl.buff, 0, sizeof(BufferCtl.buff));

  return UX_SUCCESS;
}

/**
  * @brief  USBD_AUDIO_PlaybackStartSilent
  *         Starts the output DMA on a silent buffer before the first host packet arrives.
  * @param  none
  * @retval UX_SUCCESS on success, or UX_ERROR if the BSP playback start fails.
  */
UINT USBD_AUDIO_PlaybackStartSilent(VOID)
{
  /* Start the output path once on silence so the cold-start transient is not audible
     when the host begins sending the first audio packets. */
  USBD_AUDIO_PlaybackResetState();

  if (BSP_AUDIO_OUT_Play(0, (uint8_t *)&BufferCtl.buff[0], AUDIO_TOTAL_BUF_SIZE) != BSP_ERROR_NONE)
  {
    return UX_ERROR;
  }

  playback_started = 1U;

  return UX_SUCCESS;
}

#ifdef UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT
/**
  * @brief  Calculates the playback feedback payload value from the ring buffer level.
  * @param  speed: USB device speed.
  * @retval Feedback payload value in fixed-point samples per service interval.
  */
static uint32_t Audio_get_feedback(uint8_t speed)
{
  uint32_t filled_size;
  uint32_t distance_size;
  uint32_t frame_size_byte;
  uint32_t correction_hz;
  uint32_t feedback_data_rate;
  uint32_t feedback_frequency_hz;

  /* Update the software read pointer from the active SAI DMA position. */
  BufferCtl.rd_ptr = USBD_AUDIO_PlaybackGetReadPosition();

  /* Compute the occupied size in the circular playback buffer. */
  if (BufferCtl.wr_ptr >= BufferCtl.rd_ptr)
  {
    filled_size = BufferCtl.wr_ptr - BufferCtl.rd_ptr;
  }
  else
  {
    filled_size = BufferCtl.size + BufferCtl.wr_ptr - BufferCtl.rd_ptr;
  }

  /* Convert the byte occupancy into audio frames before deriving a frequency correction. */
  frame_size_byte = PlaybackAudioDescription.audio_channels_count * PlaybackAudioDescription.audio_resolution;
  feedback_frequency_hz = PlaybackAudioDescription.audio_frequency;

  if (filled_size < BUF_UNDERRUN_THRESHOLD)
  {
    /* Increase the feedback frequency when the buffer level drops below the target threshold. */
    distance_size = BUF_UNDERRUN_THRESHOLD - filled_size;
    correction_hz = distance_size / frame_size_byte;
    feedback_frequency_hz += correction_hz;
  }
  else if (filled_size > BUF_OVERRUN_THRESHOLD)
  {
    /* Decrease the feedback frequency when the buffer level rises above the target threshold. */
    distance_size = filled_size - BUF_OVERRUN_THRESHOLD;
    correction_hz = distance_size / frame_size_byte;
    feedback_frequency_hz -= correction_hz;
  }

  if (speed == UX_HIGH_SPEED_DEVICE)
  {
    /* High-speed feedback always uses 16.16 sample frames per service interval. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 16)
                                      + (AUDIO_HS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                     / AUDIO_HS_SERVICE_INTERVALS_PER_SECOND);
  }
  else if (USBD_AUDIO_PLAY_EP_FEEDBACK_FS_MPS > 3U)
  {
    /* Keep full-speed feedback in 16.16 by default to work around the Windows
       Audio 2.0 full-speed feedback handling issue. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 16)
                                      + (AUDIO_FS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                     / AUDIO_FS_SERVICE_INTERVALS_PER_SECOND);
  }
  else
  {
    /* Switch back to 10.14 only when the user explicitly sets the FS feedback MPS to 3 bytes. */
    feedback_data_rate = (uint32_t)(((((uint64_t)feedback_frequency_hz) << 14)
                                      + (AUDIO_FS_SERVICE_INTERVALS_PER_SECOND / 2U))
                                     / AUDIO_FS_SERVICE_INTERVALS_PER_SECOND);
  }

  return feedback_data_rate;
}

/**
  * @brief  USBD_AUDIO_PlaybackFeedbackEncode
  *         This function always prepares the 4-byte local feedback buffer.
  *         The middleware uses the configured endpoint request length to send 3 or 4 bytes.
  * @param  feedback_data_rate: Feedback data rate in fixed-point format.
  * @param  feedback_buffer: Pointer to feedback payload buffer.
  * @retval none
  */
static VOID USBD_AUDIO_PlaybackFeedbackEncode(uint32_t feedback_data_rate, uint8_t *feedback_buffer)
{
  feedback_buffer[0] = (uint8_t)(feedback_data_rate & 0xFFU);
  feedback_buffer[1] = (uint8_t)((feedback_data_rate >> 8) & 0xFFU);
  feedback_buffer[2] = (uint8_t)((feedback_data_rate >> 16) & 0xFFU);
  feedback_buffer[3] = (uint8_t)((feedback_data_rate >> 24) & 0xFFU);
}
#endif /* UX_DEVICE_CLASS_AUDIO_FEEDBACK_SUPPORT */
/* USER CODE END 1 */

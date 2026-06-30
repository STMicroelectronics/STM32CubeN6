/**
  ******************************************************************************
  * @file           : opus_app.c
  * @brief          : Encoder support (audio PCM capture and callbacks, OPUS encoder)
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <stdbool.h>

#include "app_rtsp_over_rtp.h"
#include "app_netxduo.h"
#include "utils.h"
#include "opus.h"
#include "opus_app.h"
#include "audio_conf.h"
#include "plugin_audio.h"

/* Audio in buffer - placed in non-cached memory */
int16_t PCM_Buffer[CAPTURE_BUFFER_NB_SAMPLES] ALIGN_32 __NON_CACHEABLE;

/* Frame pointer and counters shared with ISR -> mark volatile */
static volatile int16_t *pcm_frame;
static volatile uint32_t pcm_frame_number;
static volatile uint32_t last_pcm_frame_number;

/* Private function prototypes -----------------------------------------------*/
void Error_Handler(void);


/* Event flags used by the video pipeline */
#define PCM_FRAME_RECEIVED_FLAG   (1U << 0)
#define AUDIO_START_FLAG            (1U << 1)

typedef struct {
  uint32_t  size;
  uint32_t *block_addr;
} audio_output_frame_t;


#define AUDIO_OUTPUT_BLOCK_NB_BYTES     CAPTURE_NB_BYTES
#define AUDIO_OUTPUT_BLOCK_NBR          4U

OpusEncoder *opus_encoder = NULL;

static uint32_t frame_received = 0;
static uint32_t last_frame_received = 0;

static audio_output_frame_t queue_buf[AUDIO_OUTPUT_BLOCK_NBR];
uint8_t audio_output_block_buffer[AUDIO_OUTPUT_BLOCK_NBR * AUDIO_OUTPUT_BLOCK_NB_BYTES] ALIGN_32;

TX_THREAD audio_thread;
TX_EVENT_FLAGS_GROUP audio_app_flags;
TX_QUEUE             audio_frame_queue;
TX_BLOCK_POOL        audio_block_pool;

/* Prototypes (typages uniformisés) */
static int32_t audio_init(void);
static int32_t audio_start(void);
static int32_t audio_encode(void);


/**
 * @brief  Initialize audio capture hardware.
 * @note   Uses BSP audio driver for digital microphone input.
 * @retval BSP status code (BSP_ERROR_NONE on success)
 */
int32_t audio_init(void)
{
  uint32_t audioState;
  int32_t err;
  int opus_status = 0;
  BSP_AUDIO_Init_t AudioInit;

  /* Test audio input state */
  err = BSP_AUDIO_IN_GetState(1, &audioState);
  if (err != BSP_ERROR_NONE)
  {
    printf("BSP_AUDIO_IN_GetState failed !! \n");
    return err;
  }

  if (audioState != AUDIO_IN_STATE_RESET)
  {
    printf("audioState != AUDIO_IN_STATE_RESET !!!\n");
    return BSP_ERROR_BUSY;
  }

  AudioInit.Device        = AUDIO_IN_DEVICE_DIGITAL_MIC;
  AudioInit.SampleRate    = AUDIO_FREQUENCY;
  AudioInit.BitsPerSample = AUDIO_RESOLUTION_16B;
  AudioInit.ChannelsNbr   = NB_MICS;
  AudioInit.Volume        = 80;

  err = BSP_AUDIO_IN_Init(1, &AudioInit);
  if (err != BSP_ERROR_NONE)
  {
    printf("BSP_AUDIO_IN_Init failed !! \n");
  }

  if (opus_encoder == NULL)
  {
    opus_encoder = opus_encoder_create(16000, 1, OPUS_APPLICATION_VOIP, &opus_status);
    if ((opus_encoder == NULL) || (opus_status != OPUS_OK))
    {
      return -1;
    }
  }

  return err;
}

/**
 * @brief  Start audio capture (record).
 * @retval BSP status code (BSP_ERROR_NONE on success)
 */
int32_t audio_start(void)
{
  int32_t err;

  pcm_frame_number = 0;
  last_pcm_frame_number = 0;

  /* Start record. Buffer length in bytes */
  err = BSP_AUDIO_IN_Record(1, (uint8_t *)PCM_Buffer, CAPTURE_BUFFER_NB_SAMPLES * sizeof(int16_t));
  if (err != BSP_ERROR_NONE)
  {
    printf("BSP_AUDIO_IN_Record failed !! \n");
  }
  return err;
}

/**
 * @brief  Stops audio capture
 * @retval BSP status code (BSP_ERROR_NONE on success)
 */
int32_t audio_stop(void)
{
  return BSP_AUDIO_IN_Stop(1);
}



/* IRQ Handler ---------------------------------------------------------------*/
/**
 * @brief  GPDMA channel IRQ handler forwards to BSP audio IRQ handler.
 */
void GPDMA1_Channel0_IRQHandler(void)
{
  BSP_AUDIO_IN_IRQHandler(1, AUDIO_IN_DEVICE_DIGITAL_MIC);
}

/* BSP audio callbacks ------------------------------------------------------*/

/**
 * @brief  BSP audio-in transfer complete callback.
 * @param  Instance  Audio instance (expected 1)
 * @retval None
 */
void BSP_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  if (Instance == 1U)
  {
    /* Point to second half of the double buffer */
    pcm_frame = PCM_Buffer + (CAPTURE_BUFFER_NB_SAMPLES / 2);
    pcm_frame_number++;
    tx_event_flags_set(&audio_app_flags, PCM_FRAME_RECEIVED_FLAG, TX_OR);
  }
}

/**
 * @brief  BSP audio-in half-transfer complete callback.
 * @param  Instance  Audio instance (expected 1)
 * @retval None
 */
void BSP_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  if (Instance == 1U)
  {
    /* Point to first half of the double buffer */
    pcm_frame = PCM_Buffer;
    pcm_frame_number++;
    tx_event_flags_set(&audio_app_flags, PCM_FRAME_RECEIVED_FLAG, TX_OR);
  }
}

/**
 * @brief  BSP audio-in error callback.
 * @param  Instance  Audio instance
 * @retval None
 */
void BSP_AUDIO_IN_Error_CallBack(uint32_t Instance)
{
  (void) Instance;
  printf("BSP_AUDIO_IN_Error_CallBack\n");
}

/**
 * @brief  Starts the audio encoding process.
 * @retval 0
 */
uint32_t AUDIO_APP_EncodingStart(void)
{
  audio_start();
  tx_event_flags_set(&audio_app_flags, AUDIO_START_FLAG, TX_OR);
  return 0U;
}

uint32_t AUDIO_APP_EncodingStop(void)
{
  ULONG flags;
  tx_event_flags_get(&audio_app_flags, AUDIO_START_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);
  return 0U;
}



/**
 * @brief  Return the last PCM frame sequence number produced by ISR.
 * @retval pcm frame sequence number
 */
uint32_t AUDIO_APP_LastPcmFrameNumber(void)
{
  return pcm_frame_number;
}
/**
 * @brief  Get pointer to the last ready PCM data and its size in bytes.
 * @param  data  Output pointer to PCM buffer (points to half-buffer region)
 * @param  size  Output size in bytes
 * @retval 0
 *
 * @note   This function checks for buffer overflow (producer/consumer mismatch).
 */
int32_t AUDIO_APP_GetData(uint8_t **data, size_t *size)
{
  static uint32_t *curr_block = NULL;

  if (curr_block != NULL)
  {
    tx_block_release(curr_block);
    curr_block = NULL;
  }

  audio_output_frame_t frame_block;
  if (tx_queue_receive(&audio_frame_queue, (void *) &frame_block, TX_NO_WAIT) != TX_SUCCESS)
  {
    *data = NULL;
    *size = 0;
    return -1;
  }

  *data      = (uint8_t *) frame_block.block_addr;
  *size      = frame_block.size;
  curr_block = frame_block.block_addr;
  return 0;
}

/**
 * @brief  Check audio capture overflow
 * @retval True if overflow happened
 */
bool IsAudioOverflow(void)
{
  bool isAudioOverflow = (frame_received > (last_frame_received + 1U));
  last_frame_received = frame_received;
  return isAudioOverflow;
}

/**
 * @brief  Encode audio sample
 * @retval status code (0 on success)
 */
int32_t audio_encode(void)
{
  int32_t err = 0;
  int32_t status;
  audio_output_frame_t frame_buffer = (audio_output_frame_t){0};

  if (tx_block_allocate(&audio_block_pool, (void **) &frame_buffer.block_addr, TX_WAIT_FOREVER) != TX_SUCCESS)
  {
    printf("AUDIO : failed to allocate output buffer\n");
    return -1;
  }

  status = opus_encode(opus_encoder,
                       (int16_t const  *) pcm_frame,
                       CAPTURE_NB_SAMPLES,
                       (unsigned char *)frame_buffer.block_addr,
                       AUDIO_OUTPUT_BLOCK_NB_BYTES);

  if (status <= 0)
  {
    tx_block_release(frame_buffer.block_addr);
    printf("opus_encode failed with error: %ld\n", (long)status);
    return -1;
  }

  frame_buffer.size = (uint32_t) status;

  if (tx_queue_send(&audio_frame_queue, (void *) &frame_buffer, TX_NO_WAIT) != TX_SUCCESS)
  {
    tx_block_release(frame_buffer.block_addr);
  }

  return err;
}


/**
 * @brief  Audio encode  encode thread
 */
void audio_thread_func(ULONG arg)
{
  ULONG flags;
  (void) arg;

  if (tx_event_flags_create(&audio_app_flags, "audio_app_events") != TX_SUCCESS)
  {
    return;
  }

  if (tx_queue_create(&audio_frame_queue,
                      "Audio frame queue",
                      sizeof(audio_output_frame_t) / 4U,
                      &queue_buf,
                      sizeof(queue_buf)) != TX_SUCCESS)
  {
    Error_Handler();
  }

  if (tx_block_pool_create(&audio_block_pool,
                           "audio output block pool",
                           AUDIO_OUTPUT_BLOCK_NB_BYTES,
                           audio_output_block_buffer,
                           AUDIO_OUTPUT_BLOCK_NBR * AUDIO_OUTPUT_BLOCK_NB_BYTES) != TX_SUCCESS)
  {
    Error_Handler();
  }

  if (audio_init() != BSP_ERROR_NONE)
  {
    printf("audio_init failed !!\n");
  }

  while (1)
  {
    tx_event_flags_get(&audio_app_flags, AUDIO_START_FLAG, TX_AND, &flags, TX_WAIT_FOREVER);
    tx_event_flags_get(&audio_app_flags, PCM_FRAME_RECEIVED_FLAG, TX_AND_CLEAR, &flags, TX_WAIT_FOREVER);

    if (IsAudioOverflow())
    {
      printf("Audio Overflow - Skip Frame\n");
      continue;
    }

    if (audio_encode() != 0)
    {
      printf("error encoding frame\n");
    }
    else
    {
      tx_event_flags_set(&demo_test_events, DEMO_AUDIO_DATA_READY_EVENT, TX_OR);
    }
  }
}

extern TX_BYTE_POOL tx_app_byte_pool;

/**
 * @brief  Creates audio  encode thread
 */
void audio_thread_create(void)
{
    UINT status = TX_SUCCESS;
    void *thread_stack_pointer = NULL;
    
    if(tx_byte_allocate(&tx_app_byte_pool, &thread_stack_pointer, 32000, TX_NO_WAIT) != TX_SUCCESS){
      Error_Handler();
    }
    /* Start the AUDIO Thread */
    status = tx_thread_create(&audio_thread, "AUDIO App Thread", audio_thread_func, 0,
              thread_stack_pointer, 32000, DEFAULT_PRIORITY+2, DEFAULT_PRIORITY+2, TX_NO_TIME_SLICE, TX_AUTO_START);
    if(status != TX_SUCCESS)
    {
      Error_Handler();
    }
}

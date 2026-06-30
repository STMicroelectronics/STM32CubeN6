/**
  ******************************************************************************
  * @file    venc_app.h
  * @author  MCD Application Team
  * @brief   Header for venc_app.c module
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef VENC_APP_H
#define VENC_APP_H

/* Includes ------------------------------------------------------------------*/
#include "stdio.h"
#include "stm32n6xx_hal.h"
#include "tx_api.h"
/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
/* Event flags used by the video pipeline */
#define FRAME_RECEIVED_FLAG   (1U << 0)
#define VIDEO_START_FLAG      (1U << 1)

/* Exported variables --------------------------------------------------------*/
/**
 * @brief  Event flags group used for USB/video device signalling.
 * @note   Defined in the corresponding C module.
 */
extern TX_EVENT_FLAGS_GROUP USB_video_device_flags;

/**
 * @brief  Queue carrying encoded frames (TX_QUEUE).
 * @note   Defined in the corresponding C module.
 */
extern TX_QUEUE enc_frame_queue;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Main VENC thread entry function.
 * @param  arg Thread argument (unused or user-defined)
 */
void venc_thread_func(ULONG arg);

/**
 * @brief  Start the video encoding pipeline.
 * @retval None
 */
void VENC_APP_EncodingStart(void);

/**
 * @brief  Retrieve encoded data from the encoder.
 * @param  data  Output pointer to the data buffer (UCHAR **)
 * @param  size  Output pointer to the data size (ULONG *)
 * @retval UINT Status code (NX/ThreadX style, 0 for success)
 */
INT VENC_APP_GetData(UCHAR **data, ULONG *size);

/**
 * @brief  Peek the saved H264 stream-start blob (typically SPS/PPS).
 * @param  data  Output pointer to the blob buffer.
 * @param  size  Output pointer to the blob size.
 * @retval INT   0 if a blob is available, -1 otherwise.
 */
INT VENC_APP_PeekStreamStartBlob(UCHAR **data, ULONG *size);

/**
 * @brief  Mark the saved H264 stream-start blob as consumed.
 */
void VENC_APP_ClearStreamStartBlob(void);

/**
 * @brief  Stop the video encoding pipeline and release resources.
 * @retval UINT Status code
 */
UINT VENC_APP_EncodingStop(void);
#endif /* VENC_APP_H */

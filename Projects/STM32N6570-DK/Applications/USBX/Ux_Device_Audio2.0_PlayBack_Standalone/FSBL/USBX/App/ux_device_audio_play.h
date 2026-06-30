/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    ux_device_audio_play.h
  * @author  MCD Application Team
  * @brief   USBX Device Audio Playback applicative header file
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __UX_DEVICE_AUDIO_PLAY_H__
#define __UX_DEVICE_AUDIO_PLAY_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "ux_api.h"
#include "ux_device_class_audio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ux_device_descriptors.h"
#include <stdint.h>
#include "stm32n6570_discovery_audio.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
VOID USBD_AUDIO_PlaybackStreamChange(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                     ULONG alternate_setting);
VOID USBD_AUDIO_PlaybackStreamFrameDone(UX_DEVICE_CLASS_AUDIO_STREAM *audio_play_stream,
                                        ULONG length);
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferNumber(VOID);
ULONG USBD_AUDIO_PlaybackStreamGetMaxFrameBufferSize(VOID);
/* USER CODE BEGIN EFP */

UINT USBD_AUDIO_PlaybackInit(VOID);
UINT USBD_AUDIO_PlaybackStartSilent(VOID);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AUDIO_TOTAL_BUF_SIZE                           (192U * 50U)

typedef enum
{
  PLAY_BUFFER_OFFSET_UNKNOWN = 0,
  PLAY_BUFFER_OFFSET_FULL,
}BUFFER_StateTypeDef;

/* Audio buffer control structure */
typedef struct {
  uint8_t buff[AUDIO_TOTAL_BUF_SIZE];
  BUFFER_StateTypeDef state;   /* empty (none), half, full*/
  uint32_t rd_ptr;
  uint32_t wr_ptr;
  uint32_t size;
}AUDIO_OUT_BufferTypeDef;

/* Properties of audio stream */
typedef struct
{
  ULONG audio_frequency;      /* Audio frequency */
  CHAR  audio_channels_count; /* Audio channels count */
  SHORT audio_volume_db_256;  /* Audio volume */
  CHAR  audio_mute;           /* Audio mute state */
  CHAR  audio_resolution;     /* Audio resolution */
}AUDIO_DescriptionTypeDef;

/* USER CODE END PD */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

#ifdef __cplusplus
}
#endif
#endif  /* __UX_DEVICE_AUDIO_PLAY_H__ */

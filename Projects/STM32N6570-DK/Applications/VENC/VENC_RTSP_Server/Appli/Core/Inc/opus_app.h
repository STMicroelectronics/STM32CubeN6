/**
  ******************************************************************************
  * @file           : opus_app.h
  * @brief          : Header for opus_app.c module
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

#ifndef __AUDIO_APP_H__
#define __AUDIO_APP_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdint.h>
#include "stm32n6xx_hal.h"
#include "tx_api.h"

/* Exported constants --------------------------------------------------------*/

/* Exported variables --------------------------------------------------------*/
/**
 * @brief  Queue carrying encoded frames (TX_QUEUE).
 * @note   Defined in the corresponding C module.
 */
extern TX_QUEUE audio_frame_queue;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Main VENC thread entry function.
 * @param  arg Thread argument (unused or user-defined)
 */
void audio_thread_func(ULONG arg);

#ifdef __cplusplus
}
#endif

#endif /* __VENC_APP_H__ */

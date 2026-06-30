/**
 *******************************************************************************
 * @file    Appli/Core/Inc/st_monitor_bitrate.h
 * @author  MCD Application Team
 * @brief   Bitrate monitoring interface
 *******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may obtain a copy of the License in the LICENSE file
 * in the root directory of this software component.
 *
 *******************************************************************************
 */

#ifndef __ST_MONITOR_BITRATE_H__
#define __ST_MONITOR_BITRATE_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/**
 * @brief  Monitor bitrate for a named stream.
 *
 * This function is intended to be called for each frame (or audio packet)
 * transmitted. Implementations may aggregate frame sizes and timestamps to
 * compute instantaneous and average bitrates for the given stream name.
 *
 * @param  stream     Null-terminated name of the stream (e.g., "audio" or "video").
 * @param  frameSize  Size of the frame in bytes.
 * @retval None
 */
void monitor_bitrate(const char *stream, uint32_t frameSize);

#ifdef __cplusplus
}
#endif

#endif /* __ST_MONITOR_BITRATE_H__ */



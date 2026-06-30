/**
 ******************************************************************************
 * @file    Appli/Core/Inc/venc_h264_config_720p_Slice_noPSRAM.h
 * @author  MCD Application Team
 * @brief   VENC H.264 configuration for 720p slice mode (no PSRAM)
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 STMicroelectronics.
 * All rights reserved.
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may obtain a copy of the License in the LICENSE file
 * in the root directory of this software component.
 ******************************************************************************
 */

#ifndef __VENC_H264_CONFIG_720P_SLICE_NO_PSRAM_H__
#define __VENC_H264_CONFIG_720P_SLICE_NO_PSRAM_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Frame dimensions and framerate -------------------------------------------*/
/** Frame height in pixels */
#define VENC_HEIGHT    720U
/** Frame width in pixels  */
#define VENC_WIDTH     1280U
/** Target framerate (frames per second) */
#define FRAMERATE      20U

/* Hardware mode ------------------------------------------------------------*/
/** Enable hardware handshake / slice / stream mode (1 = enabled) */
#define VENC_HW_MODE_ENABLE     1U

/* Buffer placement ---------------------------------------------------------*/
/** Location macro for VENC internal buffer */
#define VENC_BUFFER_LOCATION    IN_UNCACHED_RAM

/** Location macro for input frame buffer */
#define INPUT_FRAME_LOCATION    IN_RAM

/* Pixel / format configuration ---------------------------------------------*/
#if 1
/** DCMIPP pixel packer format */
#define DCMIPP_FORMAT                   DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2
/** VENC input pixel format */
#define VENC_INPUT_FORMAT               H264ENC_YUV420_SEMIPLANAR
/** Bytes per pixel (YUV420 = 12 bits per pixel => 1.5 bytes) */
#define DCMIPP_BYTES_PER_PIXELS         (1.5f)
/**
 * Pool size for encoder buffers (bytes).
 * Trimmed for the inter-RAM-only 720p slice profile to reduce uncached usage.
 */
#define VENC_POOL_SIZE                  (2144U * 1024U)
#else
/** Alternative configuration (YUV422) */
#define DCMIPP_FORMAT                   DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1
#define VENC_INPUT_FORMAT               H264ENC_YUV422_INTERLEAVED_YUYV
#define DCMIPP_BYTES_PER_PIXELS         (2U)            /* YUV422: 16 bits per pixel */
#define VENC_POOL_SIZE                  (2144U * 1024U)
#endif



/* Size of output h264 bitstream - to be adjusted according to targeted bitrate*.. and available memory*/
#ifndef VENC_OUTPUT_BUFFER_SIZE
#define VENC_OUTPUT_BUFFER_SIZE     (4U * 75U * 1024U)
#endif

/* This use case is tight in memory => lower the bitrate*/
#ifndef VENC_BITRATE
#define VENC_BITRATE (1*1000U*1000U)
#endif

  
#ifdef __cplusplus
}
#endif

#endif /* __VENC_H264_CONFIG_720P_SLICE_NO_PSRAM_H__ */



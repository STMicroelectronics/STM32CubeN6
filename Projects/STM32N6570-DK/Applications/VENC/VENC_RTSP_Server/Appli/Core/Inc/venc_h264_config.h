/**
 ******************************************************************************
* @file          venc_h264_config.h
 * @author  MCD Application Team
* @brief         venc configuration 
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



#ifndef venc_h264_config_h
#define venc_h264_config_h


#ifdef __cplusplus
extern "C"
{
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdarg.h>
#include <stdint.h>
#include "stm32n6xx_ll_venc.h"
#include "h264encapi.h"

/* Exported types ------------------------------------------------------------*/

/**
 * @brief  Camera configuration for H.264 (dimensions).
 */
typedef struct cam_h264_cfg_t
{
  uint32_t width;  /**< frame width in pixels */
  uint32_t height; /**< frame height in pixels */
} cam_h264_cfg_t;

/**
 * @brief  DCMIPP (pixel packer) configuration.
 */
typedef struct dcmipp_h264_cfg_t
{
  float    bytes_per_pixel;
  uint32_t format; /**< pixel packer format identifier */
  uint32_t pitch;  /**< line pitch in bytes */
} dcmipp_h264_cfg_t;

/**
 * @brief  Aggregated VENC H.264 configuration (encoder API structures).
 */
typedef struct venc_h264_cfg_t
{
  H264EncConfig           cfgH264Main;     /**< main H.264 config */
  H264EncPreProcessingCfg cfgH264Preproc;  /**< pre-processing config */
  H264EncCodingCtrl       cfgH264Coding;   /**< coding control config */
  H264EncRateCtrl         cfgH264Rate;     /**< rate control config */
} venc_h264_cfg_t;

/* Exported variables --------------------------------------------------------*/
extern cam_h264_cfg_t     hCamH264Instance;
extern dcmipp_h264_cfg_t  hDcmippH264Instance;
extern venc_h264_cfg_t    hVencH264Instance;

/* Exported functions --------------------------------------------------------*/

/**
 * @brief  Provide encoder working buffer pool choice to EWL.
 * @param  pool_ptr Pointer to buffer pointer (output)
 * @param  size     Pointer to size (output)
 */
void EWLPoolChoiceCb(u8 **pool_ptr, size_t *size);

/**
 * @brief  Get a pointer to the input frame buffer and its size.
 * @param  framSize Output: frame size in bytes.
 * @retval uint8_t* Pointer to input frame data.
 */
uint8_t *GetInputFrame(uint32_t *framSize);

/**
 * @brief  Return whether hardware handshake (slice/stream) mode is active.
 * @retval bool true if hw handshake mode enabled.
 */
bool IsHwHanshakeMode(void);

/**
 * @brief  Get DCMIPP pixel packer format id used by the pipeline.
 * @retval uint32_t format identifier
 */
uint32_t GetDCMIPPFormat(void);

/**
 * @brief  Retrieve DCMIPP lines configuration (warp/irq lines).
 * @param  pWarpLines Output warp lines count
 * @param  pIrqLines  Output irq lines count
 * @retval HAL_StatusTypeDef HAL status
 */
HAL_StatusTypeDef GetDCMIPPLinesConfig(uint32_t *pWarpLines, uint32_t *pIrqLines);

/**
 * @brief Return the last frame address captured by DCMIPP
 * @param  frame_number Input frame ID to be retrieved
 * @retval frame address
 */
uint8_t * GetNextFrame(uint32_t frame_number);


/**
 * @brief  Return maximmum number of frames available for DCMIPP capture.
 * @retval Number of frames
 */
uint32_t GetNbInputFrame(void);


/**
 * @brief  Get pointer to output  buffer and  its size.
 * @param  bufferSize Optional output: size of buffer in bytes.
 * @retval uint8_t* Pointer to the buffer
 */
uint8_t * GetOutputBuffer(uint32_t * bufferSize);


/**
 * @brief  Return number of captured lines by DCMIPP.
 * @retval uint32_t number of lines captured
 */
uint32_t GetDCMIPPNbLinesCaptured(void);


/**
 * @brief Returns the video frame rate (FPS) from the H.264 encoder configuration.
 * @retval uint32_t The computed frames-per-second value.
 */
uint32_t GetVideoFramerate(void);

#ifdef __cplusplus
};
#endif

#endif



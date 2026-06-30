/**
******************************************************************************
  * @file           : dcmipp_app.c
  * @brief          : VENC application for STM32N6xx: handles video encoding
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
#include "main.h"
#include "stdio.h"
#include "h264encapi.h"
#include "stm32n6570_discovery_camera.h"
#include "venc_h264_config.h"
#include "venc_app.h"
#include "dcmipp_app.h"


extern DCMIPP_HandleTypeDef hcamera_dcmipp;

extern void Error_Handler(void);

HAL_StatusTypeDef  dcmipp_downsize(DCMIPP_HandleTypeDef *hdcmipp,int32_t pipe, int32_t camWidth,int32_t camHeight,int32_t captureWidth,int32_t captureHeight);
static HAL_StatusTypeDef  dcmipp_enable_HW_Handshake(DCMIPP_HandleTypeDef *hdcmipp);


/**
 * @brief  Initializes the DCMIPP peripheral.
 * @param  hdcmipp: Pointer to a DCMIPP_HandleTypeDef structure that contains
 *         the configuration information for the DCMIPP peripheral.
 * @retval HAL status:
 *           - HAL_OK: Initialization successful
 *           - HAL_ERROR: Initialization error
 *           - HAL_BUSY: Peripheral busy
 *           - HAL_TIMEOUT: Timeout occurred
 *
 * This function configures and initializes the DCMIPP peripheral according to
 * the specified parameters in the DCMIPP handle. It must be called before
 * using the DCMIPP peripheral.
 */
HAL_StatusTypeDef MX_DCMIPP_Init(DCMIPP_HandleTypeDef *hdcmipp)
{
  DCMIPP_PipeConfTypeDef pPipeConf = {0};
  DCMIPP_CSI_PIPE_ConfTypeDef pCSIPipeConf = {0};
  DCMIPP_CSI_ConfTypeDef csiconf = {0};
   HAL_StatusTypeDef ret = HAL_OK;

  if (HAL_DCMIPP_Init(hdcmipp) != HAL_OK)
  {
    return HAL_ERROR;
  }

  /* Configure the CSI */
  csiconf.DataLaneMapping = DCMIPP_CSI_PHYSICAL_DATA_LANES;
  csiconf.NumberOfLanes   = DCMIPP_CSI_TWO_DATA_LANES;
  csiconf.PHYBitrate      = DCMIPP_CSI_PHY_BT_1600;
  HAL_DCMIPP_CSI_SetConfig(hdcmipp, &csiconf);

  /* Configure the Virtual Channel 0 */
  /* Set Virtual Channel config */
  HAL_DCMIPP_CSI_SetVCConfig(hdcmipp, DCMIPP_VIRTUAL_CHANNEL0, DCMIPP_CSI_DT_BPP10);


  /* Configure the serial Pipe */
  pCSIPipeConf.DataTypeMode = DCMIPP_DTMODE_DTIDA;
  pCSIPipeConf.DataTypeIDA  = DCMIPP_DT_RAW10;
  pCSIPipeConf.DataTypeIDB  = DCMIPP_DT_RAW10; /* Don't Care */


  if (HAL_DCMIPP_CSI_PIPE_SetConfig(hdcmipp, DCMIPP_PIPE1, &pCSIPipeConf) != HAL_OK)
  {
    return HAL_ERROR;
  }

   pPipeConf.FrameRate  = DCMIPP_FRAME_RATE_ALL;  /* Sensor framerate is set with IMX335_SetFramerate(..FRAMERATE);*/
   pPipeConf.PixelPackerFormat = hDcmippH264Instance.format;

  /* Set Pitch for Main and Ancillary Pipes */
   pPipeConf.PixelPipePitch  =hDcmippH264Instance.pitch; /* Number of bytes */

  /* Configure Pipe */
  if (HAL_DCMIPP_PIPE_SetConfig(hdcmipp, DCMIPP_PIPE1, &pPipeConf) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (HAL_DCMIPP_PIPE_EnableRedBlueSwap(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  #define N10(val) (((val) ^ 0x7FF) + 1)
  DCMIPP_ColorConversionConfTypeDef color_conf = {
    .ClampOutputSamples = ENABLE,
    .OutputSamplesType = DCMIPP_CLAMP_YUV,
    .RR = N10(26), .RG = N10(87), .RB = 112, .RA = 128,
    .GR = 47, .GG = 157, .GB = 16, .GA = 16,
    .BR = 112, .BG = N10(102), .BB = N10(10), .BA = 128,
  };
  if (HAL_DCMIPP_PIPE_SetYUVConversionConfig(hdcmipp, DCMIPP_PIPE1, &color_conf) != HAL_OK)
  {
    return HAL_ERROR;
  }
  if (HAL_DCMIPP_PIPE_EnableYUVConversion(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
  /* Configure DCMIPP output size*/
  ret= dcmipp_downsize(hdcmipp,DCMIPP_PIPE1,
                        hCamH264Instance.width,
                        hCamH264Instance.height,
                        hVencH264Instance.cfgH264Main.width,
                        hVencH264Instance.cfgH264Main.height);


  
  if (ret == HAL_OK)
  {
  /* Specific Hardware Handshake if needed*/
    if (IsHwHanshakeMode())
  {
    ret = dcmipp_enable_HW_Handshake(hdcmipp);
    }
    else  /* Frame Mode : avoid spurious interrupts */
    { 
      /* Software workaround for Linemult triggering VENC interrupt. Minimize occurrences as much as possible. */
      MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_128_LINES);
      
      /* Disable Line events interrupts*/
      ret = HAL_DCMIPP_PIPE_DisableLineEvent(hdcmipp, DCMIPP_PIPE1);
    }
  }

  return ret;

}


/**
 * @brief Determine whether the current DCMIPP pixel format is semiplanar.
 *
 * @return int
 *   - 1: The format is semiplanar (e.g., YUV420_2).
 *   - 0: The format is planar (e.g., YUV422_1).
 *   - -1: Unreachable fallback after Error_Handler() for unsupported formats.
 *
 * @note This function may call Error_Handler() if the format is not supported.
 *
 * @see GetDCMIPPFormat(), Error_Handler()
 */
int dcmipp_is_semiplanar(void)
{
  uint32_t fmt = GetDCMIPPFormat();
  if (fmt == DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1) return 0;
  if (fmt == DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2) return 1;
  printf("Unsupported DCMIPP format (0x%08lx)\n", (unsigned long)fmt);
  Error_Handler();
  return -1;
}

/**
 * @brief Compute DCMIPP destination buffer addresses for planar and semiplanar layouts.
 *
 * This helper fills both full-planar (Y, U, V) and semi-planar (Y, UV) address structures
 * based on the provided luma base address and current capture geometry.
 *
 * @param luma_address          Base address of the luma buffer.
 * @param planar_address        Output pointer to full planar address structure (Y/U/V).
 * @param semi_planar_address   Output pointer to semi-planar address structure (Y/UV).
 */
void dcmipp_get_address(void *luma_address,
                 DCMIPP_FullPlanarDstAddressTypeDef  *planar_address,
                 DCMIPP_SemiPlanarDstAddressTypeDef *semi_planar_address)
{
  uint32_t yAddr = (uint32_t)luma_address;
  uint32_t ySize = hVencH264Instance.cfgH264Main.width * GetDCMIPPNbLinesCaptured();

  planar_address->YAddress = yAddr;
  planar_address->UAddress = yAddr;
  planar_address->VAddress = yAddr;

  semi_planar_address->YAddress  = yAddr;
  semi_planar_address->UVAddress = yAddr + ySize;
}

/**
 * @brief Validate DCMIPP pixel format against H.264 encoder input type.
 *
 * Ensures the DCMIPP output format matches what the H.264 encoder expects
 * in its preprocessing configuration. Supported mappings:
 *  - DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1 -> H264ENC_YUV422_INTERLEAVED_YUYV
 *  - DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2 -> H264ENC_YUV420_SEMIPLANAR
 *
 * @retval int 1 if compatible, 0 otherwise.
 */
int dcmipp_check_config(void)
{
  uint32_t fmt = GetDCMIPPFormat();
  uint32_t inputType = hVencH264Instance.cfgH264Preproc.inputType;

  switch (fmt)
  {
    case DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1:
      return (inputType == H264ENC_YUV422_INTERLEAVED_YUYV) ? 1 : 0;
    case DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2:
      return (inputType == H264ENC_YUV420_SEMIPLANAR) ? 1 : 0;
    default:
      return 0;
  }
}

/**
 * @brief Configure DCMIPP capture for the current pixel format and start the camera.
 *
 * - Validates DCMIPP output format against H.264 preproc input type.
 * - Computes destination buffer addresses (planar or semi-planar).
 * - Starts camera in continuous mode using the appropriate BSP API.
 * - Enables DCMIPP line interrupt for PIPE1.
 *
 * @param luma_address Base address of the luma buffer (Y) for the destination frame.
 *                     For semi-planar (YUV420), chroma (UV) follows luma.
 *                     For interleaved (YUV422), only Y address is used by BSP.
 * @return 0 on success, -1 on error.
 */
int dcmipp_config(void *luma_address)
{
  DCMIPP_FullPlanarDstAddressTypeDef   planar_address;
  DCMIPP_SemiPlanarDstAddressTypeDef   semi_planar_address;

  if (!dcmipp_check_config())
  {
    printf("DCMIPP and VENC configuration mismatch\n");
    return -1;
  }

  int semiplanar = dcmipp_is_semiplanar();
  if (semiplanar < 0) return -1;

  dcmipp_get_address(luma_address, &planar_address, &semi_planar_address);

  int ret;
  if (semiplanar)
  {
    ret = BSP_CAMERA_SemiPlanarStart(0, &semi_planar_address, CAMERA_MODE_CONTINUOUS);
  }
  else
  {
    ret = BSP_CAMERA_Start(0, luma_address, CAMERA_MODE_CONTINUOUS);
  }

  if (ret != BSP_ERROR_NONE)
  {
    printf("Camera start failed (ret=%d)\n", ret);
    return -1;
  }

  __HAL_DCMIPP_ENABLE_IT(&hcamera_dcmipp, DCMIPP_IT_PIPE1_LINE);
  return 0;
}

/**
 * @brief Update DCMIPP destination memory addresses for next frame.
 *
 * Computes Y/UV (semiplanar) or Y/U/V (planar) destination addresses for the
 * next captured frame and programs the DCMIPP pipe accordingly.
 *
 * - Uses current capture geometry and pixel format to build destination layout.
 * - In semiplanar mode (YUV420), sets both Y and UV addresses.
 * - In interleaved/planar mode (YUV422), sets Y address (BSP uses Y).
 *
 * @note This should be called on each frame event to provide the next buffer
 *       to the hardware, ensuring continuous capture without overflow.
 */
void dcmipp_set_memory_address(void * next_dcmipp_address)
{
  DCMIPP_FullPlanarDstAddressTypeDef  planar_address;
  DCMIPP_SemiPlanarDstAddressTypeDef  semi_planar_address;

  dcmipp_get_address(next_dcmipp_address, &planar_address, &semi_planar_address);

  if (dcmipp_is_semiplanar())
  {
    HAL_DCMIPP_PIPE_SetSemiPlanarMemoryAddress(&hcamera_dcmipp, DCMIPP_PIPE1, &semi_planar_address);
  }
  else
  {
    HAL_DCMIPP_PIPE_SetMemoryAddress(&hcamera_dcmipp, DCMIPP_PIPE1, DCMIPP_MEMORY_ADDRESS_0, (uint32_t)planar_address.YAddress);
  }
}

/**
 * @brief Configures the DCMIPP downsize parameters to scale the camera input to the desired capture size.
 *
 * This function calculates and sets the horizontal and vertical downsize ratios and factors
 * for the DCMIPP peripheral, enabling the hardware to scale the camera input resolution
 * (camWidth x camHeight) down to the target capture resolution (captureWidth x captureHeight).
 *
 * @param hdcmipp        Pointer to the DCMIPP handle structure.
 * @param pipe           DCMIPP pipe number to configure.
 * @param camWidth       Width of the camera sensor input.
 * @param camHeight      Height of the camera sensor input.
 * @param captureWidth   Desired output width after downscaling.
 * @param captureHeight  Desired output height after downscaling.
 * @return HAL_OK on success, HAL_ERROR on failure.
 *
 * @note The function enables the downsize feature on the specified DCMIPP pipe.
 */
HAL_StatusTypeDef  dcmipp_downsize(DCMIPP_HandleTypeDef *hdcmipp,int32_t pipe, int32_t camWidth,int32_t camHeight,int32_t captureWidth,int32_t captureHeight)
{
  /* Calculation explained in RM0486v2 table 354*/

  DCMIPP_DownsizeTypeDef DownsizeConf ={0};
  /* Configure the downsize */
  DownsizeConf.HRatio      = (uint32_t)((((float)(camWidth)) / ((float)(captureWidth))) * 8192.F);
  DownsizeConf.VRatio      = (uint32_t)((((float)(camHeight )) / ((float)(captureHeight ))) * 8192.F);
  DownsizeConf.HSize       = captureWidth;
  DownsizeConf.VSize       = captureHeight;

  DownsizeConf.HDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DownsizeConf.HRatio);
  DownsizeConf.VDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DownsizeConf.VRatio);

  if(HAL_DCMIPP_PIPE_SetDownsizeConfig(hdcmipp, pipe, &DownsizeConf) != HAL_OK) return HAL_ERROR;
  if(HAL_DCMIPP_PIPE_EnableDownsize(hdcmipp, pipe)!= HAL_OK) return HAL_ERROR;
  return HAL_OK;
}

/**
 * @brief  Enables the hardware handshake for the DCMIPP peripheral.
 * @param  hdcmipp: Pointer to a DCMIPP_HandleTypeDef structure that contains
 *         the configuration information for the DCMIPP peripheral.
 * @retval HAL status
 */
static HAL_StatusTypeDef  dcmipp_enable_HW_Handshake(DCMIPP_HandleTypeDef *hdcmipp)
{
  if (HAL_DCMIPP_PIPE_SetLineWrappingConfig(hdcmipp, DCMIPP_PIPE1, DCMIPP_WRAP_ADDRESS_64_LINES) != HAL_OK)
  {
    return HAL_ERROR;
  }

  if (HAL_DCMIPP_PIPE_EnableLineWrapping(hdcmipp, DCMIPP_PIPE1) != HAL_OK)
  {
    return HAL_ERROR;
  }
 /* Enable line event with no interrupt to use the hardware trigger to the video encoder  */
  MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_32_LINES);
  return HAL_OK;
}
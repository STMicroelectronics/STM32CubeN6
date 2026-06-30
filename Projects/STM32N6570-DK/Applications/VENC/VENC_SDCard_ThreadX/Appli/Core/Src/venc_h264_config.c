/**
******************************************************************************
* @file          venc_h264_config.c
 * @brief   App plugin instances and VENC/H264 configuration
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
#include "utils.h"
#include "stm32n6xx_hal.h"
#include "venc_h264_config.h"

#define FULL_1080P_SLICE               1U
#define FULL_1080P_FRAME               2U
#define FULL_720P_SLICE                3U
#define FULL_720P_SLICE_INTER_RAM_ONLY 4U
#define FULL_720P_FRAME                5U
#define FULL_480P_SLICE                6U
#define FULL_480P_FRAME                7U


/* Select target frame configuration */
#ifndef H264_CONFIG
#define H264_CONFIG FULL_720P_FRAME
#endif


#if H264_CONFIG == FULL_720P_FRAME
#include "venc_h264_config_720p_Frame.h"
#elif H264_CONFIG == FULL_480P_FRAME
#include "venc_h264_config_480p_Frame.h"
#else
#error "Undefined VENC configuration"
#endif

#ifndef DCMIPP_FORMAT
#define DCMIPP_FORMAT            DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1
#endif

#ifndef VENC_INPUT_FORMAT
#define VENC_INPUT_FORMAT        H264ENC_YUV422_INTERLEAVED_YUYV
#endif

#ifndef DCMIPP_BYTES_PER_PIXELS
#define DCMIPP_BYTES_PER_PIXELS  (2U)  /* YUV422 : 16 bits per pixel */
#endif

/* Pitch depends on chosen pixel packer format */
#if DCMIPP_FORMAT == DCMIPP_PIXEL_PACKER_FORMAT_YUV422_1
#define DCMIPP_PITCH  (2U * VENC_WIDTH)
#elif DCMIPP_FORMAT == DCMIPP_PIXEL_PACKER_FORMAT_YUV420_2
#define DCMIPP_PITCH  (VENC_WIDTH)
#elif DCMIPP_FORMAT == DCMIPP_PIXEL_PACKER_FORMAT_YUV420_3
#define DCMIPP_PITCH  (VENC_WIDTH)
#else
#error "DCMIPP_FORMAT is not correct"
#endif

#define CAM_MACROBLOCK_HEIGHT   (16U)

/* HW handshake mode aka SLICE*/
#if (VENC_HW_MODE_ENABLE == 1U)
#define CAM_NB_BUFFERS          (2U)
#define VENC_LINE_BUF_DEPTH     (2U) /* 2 lines of 16-pixel macroblocks */
#define INPUT_FRAME_SIZE        ((uint32_t)(VENC_WIDTH * CAM_MACROBLOCK_HEIGHT * CAM_NB_BUFFERS * VENC_LINE_BUF_DEPTH * DCMIPP_BYTES_PER_PIXELS))
#define VIEW_MODE               H264ENC_BASE_VIEW_SINGLE_BUFFER 
#define NB_INPUT_FRAME          (1U)
#else /* Frame mode*/
#define CAM_NB_BUFFERS          (1U) 
#define VENC_LINE_BUF_DEPTH     (1U)
#define INPUT_FRAME_SIZE        ((uint32_t)(VENC_WIDTH * VENC_HEIGHT * CAM_NB_BUFFERS * DCMIPP_BYTES_PER_PIXELS))
#define VIEW_MODE               H264ENC_BASE_VIEW_DOUBLE_BUFFER
#define NB_INPUT_FRAME          (2U)
#endif 

/* Maximum size of VENC buffer (1080p)*/
#ifndef VENC_POOL_SIZE
#define VENC_POOL_SIZE (8U * 1024U * 1024U)
#endif

/* Size of output h264 bitstream - to be adjusted according to targeted bitrate*/
#ifndef VENC_OUTPUT_BUFFER_SIZE
#define VENC_OUTPUT_BUFFER_SIZE     (4U * 500U * 1024U)
#endif


#ifndef VENC_BITRATE
#define VENC_BITRATE (2*1000U*1000U)
#endif


/* Input frame (camera capture) */
uint8_t input_frame[NB_INPUT_FRAME][INPUT_FRAME_SIZE] ALIGN_32 INPUT_FRAME_LOCATION;

/* Encoder working pool */
uint8_t ewl_pool[VENC_POOL_SIZE] ALIGN_32 VENC_BUFFER_LOCATION;

/* h264 bitstream (encoded output buffer) */
uint8_t h264_bitstream[VENC_OUTPUT_BUFFER_SIZE] ALIGN_32 __NON_CACHEABLE;


/* IMX335 : CAMERA_R2592x1944, CAMERA_PF_RAW_RGGB10*/ 
cam_h264_cfg_t hCamH264Instance  ={
  .width  = 2592,
  .height = 1944,
};

dcmipp_h264_cfg_t hDcmippH264Instance={
    .bytes_per_pixel =    DCMIPP_BYTES_PER_PIXELS,
    .format          = DCMIPP_FORMAT,
    .pitch           = DCMIPP_PITCH,
};

/* VENC configuration instance (main, preproc, coding, rate) */
venc_h264_cfg_t hVencH264Instance={
    .cfgH264Main.streamType                   = H264ENC_BYTE_STREAM,                 /* Byte stream / Plain NAL units */
    .cfgH264Main.viewMode                     = VIEW_MODE,                           /* Mode of stream to be generated and  the corresponding amount of encoder internal frame buffers required */
    .cfgH264Main.width                        = VENC_WIDTH,                          /* Encoded picture width in pixels, multiple of 4. */
    .cfgH264Main.height                       = VENC_HEIGHT,                         /* Encoded picture height in pixels, multiple of 2. */
    .cfgH264Main.level                        = H264ENC_LEVEL_4_1,                     /* encoding Level */
    .cfgH264Main.frameRateNum                 = FRAMERATE,                           /*  The stream time scale, [1..1048575] */
    .cfgH264Main.frameRateDenom               = 1,                                   /* Maximum frame rate is frameRateNum/frameRateDenom in frames/second. The actual frame rate will be defined by timeIncrement of encoded pictures, [1..frameRateNum] */
    .cfgH264Main.scaledWidth                  = 0,                                   /* Optional down-scaled output picture width,multiple of 4. 0=disabled. [16..width] */
    .cfgH264Main.scaledHeight                 = 0,                                   /* Optional down-scaled output picture height,multiple of 2. [96..height] */
    .cfgH264Main.refFrameAmount               = 1,                                   /* Amount of reference frame buffers, [1..2],1 = only last frame buffered, 2 = last and long term frames buffered */
    .cfgH264Main.refFrameCompress             = 0,                                   /* frame compress: 0=disable; 1=enable */
    .cfgH264Main.refFrameCompress             = 0,                                   /* reference frame compress: 0=disable; 1=enable */
    .cfgH264Main.rfcLumBufLimit               = 0,                                   /* Limit of luma RFC buffer in percent of original reference frame size. */
    .cfgH264Main.rfcChrBufLimit               = 0,                                   /* Limit of chroma RFC buffer in percent of original reference frame size. */
    .cfgH264Main.svctLevel                    = 0,                                   /* [0~3] Max Layers number SVC Temporal Scalable Coding. */
    .cfgH264Main.enableSvctPrefix             = 0,                                   /* [0~1] Enable SVCT prefix 0-Disable 1-Enable. */
    .cfgH264Preproc.origWidth                 = VENC_WIDTH,                          /* Input camera picture width. */
    .cfgH264Preproc.origHeight                = VENC_HEIGHT,                         /* Input camera picture height. */
    .cfgH264Preproc.xOffset                   = 0,                                   /* Horizontal offset. */
    .cfgH264Preproc.yOffset                   = 0,                                   /* Vertical offset . */
    .cfgH264Preproc.inputType                 = VENC_INPUT_FORMAT,                   /* Input picture color format. */
    .cfgH264Preproc.rotation                  = H264ENC_ROTATE_0,                    /* Input picture rotation. */
    .cfgH264Preproc.videoStabilization        = 0,                                   /* Enable video stabilization. */
    .cfgH264Preproc.colorConversion           = H264ENC_RGBTOYUV_BT601,              /* Define color conversionparameters for RGB input. */
    .cfgH264Preproc.scaledOutput              = 0,                                   /* Enable output of down-scaled encoder picture. Dimensionsspecified at Init. */
    .cfgH264Preproc.interlacedFrame           = 0,                                   /* Enable input frame formatwith two interlaced fields.Even pictures will be readfrom top field, odd picturesfrom bottom field. */
    .cfgH264Coding.sliceSize                  = 0,                                   /* Slice size in macroblock rows,0 to encode each picture in one slice,[0..height/16]. */
    .cfgH264Coding.seiMessages                = 0,                                   /* Insert picture timing and buffering period SEI messages into the stream,[0,1] */
    .cfgH264Coding.idrHeader                  = 1,                                   /* Insert SPS/PPS header to stream either for every IDR frame or only to the beginning of the sequence.[0,1]. */
    .cfgH264Coding.videoFullRange             = 0,                                   /* Input video signal sample range, [0,1],0 = Y range in [16..235],Cb&Cr range in [16..240],1 = Y, Cb and Cr range in [0..255]. */
    .cfgH264Coding.constrainedIntraPrediction = 0,                                   /* 0 = No constrains, 1 = Only use intra neighbours. */
    .cfgH264Coding.disableDeblockingFilter    = 0,                                   /* 0 = Filter enabled, 1 = Filter disabled, 2 = Filter disabled on slice edges. */
    .cfgH264Coding.sampleAspectRatioWidth     = 0,                                   /* Horizontal size of the sample aspect ratio (in arbitrary units), 0 for unspecified, [0..65535]. */
    .cfgH264Coding.sampleAspectRatioHeight    = 0,                                   /* Vertical size of the sample aspect ratio(in same units as sampleAspectRatioWidth)0 for unspecified, [0..65535]. */
    .cfgH264Coding.enableCabac                = 1,                                   /* 0 = CAVLC - Baseline profile, 1 = CABAC - Main profile,2 = CABAC/CAVLC frame based - Performance optimized Main profile with Intra frames encoded using CAVLC and Inter frames encoded using CABAC. */
    .cfgH264Coding.cabacInitIdc               = 0,                                   /* [0,2] CABAC table initial value. */
    .cfgH264Coding.transform8x8Mode           = 1,                                   /* Enable 8x8 transform mode, High profile 0=disabled, 1=adaptive 8x8, 2=always 8x8 */
    .cfgH264Coding.quarterPixelMv             = 0,                                   /* 1/4 pixel motion estimation 0=disabled, 1=adaptive, 2=enabled */
    .cfgH264Coding.cirStart                   = 0,                                   /* 0..mbTotal] First macroblock forCyclic Intra Refresh. */
    .cfgH264Coding.cirInterval                = 0,                                   /* [0..mbTotal] Macroblock interval forCyclic Intra Refresh, 0=disabled. */
    .cfgH264Coding.intraSliceMap1             = 0,                                   /* Bitmap for forcing slices 0..31 to intra,LSB=slice 0, MSB=slice 31, 1=intra. */
    .cfgH264Coding.intraSliceMap2             = 0,                                   /* Bitmap for forcing slices 32..63 to intra,LSB=slice 32, MSB=slice 63, 1=intra. */
    .cfgH264Coding.intraSliceMap3             = 0,                                   /* Bitmap for forcing slices 64..95 to intra,LSB=slice 64, MSB=slice 95, 1=intra. */
    .cfgH264Coding.roi1DeltaQp                = 0,                                   /* [-15..0] QP delta value for 1st ROI. */
    .cfgH264Coding.adaptiveRoi                = 0,                                   /* [-51..0] QP delta value for adaptive ROI */
    .cfgH264Coding.adaptiveRoiColor           = 0,                                   /*  [-10..10] Color temperature sensitivity* for adaptive ROI skin detection.* -10 = 2000K, 0=3000K, 10=5000K. */
    .cfgH264Coding.roiMapEnable               = 0,                                   /*  ROI map status, 0=disable, 1=enable. */
    .cfgH264Coding.roiMapEnable               = 0,                                   /*  ROI map status, 0=disable, 1=enable. */
    .cfgH264Coding.fieldOrder                 = 0,                                   /* Field order for interlaced coding,0 = bottom field first, 1 = top field first. */
    .cfgH264Coding.gdrDuration                = 0,                                   /* how many pictures it will take to do GDR, if 0, not do GDR. */
    .cfgH264Coding.svctLevel                  = 0,                                   /* [0~3] Max Layers number SVC Temporal Scalable Coding. */
    .cfgH264Coding.noiseReductionEnable       = 0,                                   /* Disable noise reduction; 1 = enable noise reduction . */
    .cfgH264Coding.noiseLow                   = 1,                                   /* valid value range :[1,30] , default: 5. */
    .cfgH264Coding.noiseLevel                 = 1,                                   /* valid value range :[1,30] , default :10. */
    .cfgH264Coding.inputLineBufEn             = VENC_HW_MODE_ENABLE,                 /* enable input image control signals. */
    .cfgH264Coding.inputLineBufLoopBackEn     = VENC_HW_MODE_ENABLE,                 /* input buffer loopback mode enable. */
    .cfgH264Coding.inputLineBufDepth          = VENC_LINE_BUF_DEPTH,                 /* input buffer depth in mb lines( 1 = 1 macro block ( ie. 16 pixels) */
    .cfgH264Coding.inputLineBufHwModeEn       = VENC_HW_MODE_ENABLE,                 /* w handshake. */
    .cfgH264Coding.nBaseLayerPID              = 0,                                   /*  priority_id of base temporal layer */
    .cfgH264Coding.level                      = 0,                                   /* Reserved vendor SVC control field; kept disabled in this configuration. */
    .cfgH264Coding.enableSVC                  = 0,                                   /* Reserved vendor SVC enable flag; kept disabled in this configuration. */
    .cfgH264Rate.pictureRc                    = 1,                                   /* QP between pictures, [0,1] */
    .cfgH264Rate.mbRc                         = 1,                                   /* QP inside picture, [0,1] */
    .cfgH264Rate.pictureSkip                  = 0,                                   /* pictureSkip [0,1] */
    .cfgH264Rate.qpHdr                        = 26,                                  /* for next encoded picture, [-1..51],-1 = Let rate control calculate initial QP,This QP is used for all pictures if,HRD and pictureRc and mbRc are disabled,If HRD is enabled it may override this QP */
    .cfgH264Rate.qpMin                        = 0,                                  /* Minimum QP for any picture, [0..51] */
    .cfgH264Rate.qpMax                        = 51,                                  /* Maximum QP for any picture, [0..51] */
    .cfgH264Rate.bitPerSecond                 = VENC_BITRATE,                              /* Target bitrate in bits/second, this is needed if pictureRc, mbRc, pictureSkip or,hrd is enabled [10000..60000000] */
    .cfgH264Rate.hrd                          = 1,                                   /*  Hypothetical Reference Decoder model, [0,1],restricts the instantaneous bitrate and total bit amount of every coded picture.Enabling HRD will cause tight constrains on the operation of the rate control */
    .cfgH264Rate.hrdCpbSize                   = 75000000,                            /* Size of Coded Picture Buffer in HRD (bits) */
    .cfgH264Rate.gopLen                       = 30,                                  /* Length for Group of Pictures, indicates,the distance of two intra pictures,including first intra [1..300] */
    .cfgH264Rate.intraQpDelta                 = -3,                                  /* Intra QP delta. intraQP = QP + intraQpDelta This can be used to change the relative quality of the Intra pictures or to lower the size of Intra pictures. [-12..12] */
    .cfgH264Rate.fixedIntraQp                 = 0,                                   /* Fixed QP value for all Intra pictures, [0..51] 0 = Rate control calculates intra QP. */
    .cfgH264Rate.mbQpAdjustment               = 0,                                   /* Encoder uses MAD thresholding to recognize macroblocks with least details. This value is macroblocks with least details. This value is increasing the subjective quality. [-8..7] */
    .cfgH264Rate.longTermPicRate              = 15,                                  /* period between long term pic refreshes */
    .cfgH264Rate.mbQpAutoBoost                = 0,                                   /* Encoder uses motion vectors and variance to recognize background/object macroblocks.This value is used to enable the auto QP boost of these macroblocks. [0,1] */
};

/* EWL pool callbacks and helpers -------------------------------------------*/

/**
 * @brief  Provide encoder working buffer to EWL (External Wrapper Layer).
 * @param  pool_ptr Output: pointer to EWL pool buffer pointer.
 * @param  size     Output: size of the pool buffer in bytes.
 */
void EWLPoolChoiceCb(u8 **pool_ptr, size_t *size)
{
  *pool_ptr = ewl_pool;
  *size = VENC_POOL_SIZE;
}

/**
 * @brief  Release EWL pool pointer (weakly used; no-op).
 * @param  pool_ptr Pointer previously returned by EWLPoolChoiceCb (unused).
 */
void EWLPoolReleaseCb(u8 **pool_ptr)
{
  UNUSED(pool_ptr);
}

/**
 * @brief  Get pointer to input frame buffer and optionally its size.
 * @param  frameSize Optional output: size of input frame in bytes.
 * @retval uint8_t* Pointer to input frame buffer.
 */
uint8_t * GetInputFrame(uint32_t * frameSize)
{
  if (frameSize)
  {
    *frameSize = INPUT_FRAME_SIZE;
  }
    return input_frame[0];
}

/**
 * @brief  Get pointer to output  buffer and  its size.
 * @param  bufferSize Optional output: size of buffer in bytes.
 * @retval uint8_t* Pointer to the buffer
 */
uint8_t * GetOutputBuffer(uint32_t * bufferSize)
{
    if (bufferSize)
    {
        *bufferSize = VENC_OUTPUT_BUFFER_SIZE;
    }
    return h264_bitstream;
}


uint8_t * GetNextFrame(uint32_t frame_number)
{
   return input_frame[frame_number % NB_INPUT_FRAME];
}

uint32_t GetNbInputFrame(void)
{
  return NB_INPUT_FRAME;
}

/**
 * @brief  Return whether hardware handshake (slice/stream) mode is enabled.
 * @retval true if HW handshake mode enabled, false otherwise.
 */
bool IsHwHanshakeMode(void)
{
#if (VENC_HW_MODE_ENABLE == 1U)
    return true;
#else
    return false;
#endif
}

/**
 * @brief  Return configured DCMIPP pixel packer format identifier.
 * @retval uint32_t DCMIPP format constant.
 */
uint32_t GetDCMIPPFormat(void)
{
    return (uint32_t)DCMIPP_FORMAT;
}

/**
 * @brief  Retrieve DCMIPP lines configuration (wrap and irq lines) for HW handshake mode.
 * @param  pWarpLines Output pointer to wrap lines constant (may be NULL).
 * @param  pIrqLines  Output pointer to irq lines constant (may be NULL).
 * @retval HAL_StatusTypeDef HAL_OK when valid configuration is returned, HAL_ERROR otherwise.
 *
 * @note   This function is meaningful only when hardware handshake mode is enabled.
 */
HAL_StatusTypeDef GetDCMIPPLinesConfig(uint32_t *pWarpLines, uint32_t *pIrqLines)
{
    uint32_t nbLinesCaptured = VENC_LINE_BUF_DEPTH * CAM_MACROBLOCK_HEIGHT;
    HAL_StatusTypeDef ret = HAL_OK;

    if (!IsHwHanshakeMode())
    {
        return HAL_ERROR;
    }

    /* In hardware handshake mode, capture N lines within a 2*N buffer */
    switch (nbLinesCaptured)
    {
        case 16U:
            if (pWarpLines) *pWarpLines = DCMIPP_WRAP_ADDRESS_32_LINES;
            if (pIrqLines)  *pIrqLines  = DCMIPP_MULTILINE_16_LINES;
            break;

        case 32U:
            if (pWarpLines) *pWarpLines = DCMIPP_WRAP_ADDRESS_64_LINES;
            if (pIrqLines)  *pIrqLines  = DCMIPP_MULTILINE_32_LINES;
            break;

        case 64U:
            if (pWarpLines) *pWarpLines = DCMIPP_WRAP_ADDRESS_128_LINES;
            if (pIrqLines)  *pIrqLines  = DCMIPP_MULTILINE_64_LINES;
            break;

        default:
            ret = HAL_ERROR;
            break;
    }

    return ret;
}

/**
 * @brief  Return number of lines captured by DCMIPP (used for buffer addressing).
 * @retval uint32_t Number of lines captured (depends on HW mode or full frame).
 */
uint32_t GetDCMIPPNbLinesCaptured(void)
{
    /* In hardware handshake mode, capture N lines within a 2*N buffer */
    if (IsHwHanshakeMode())
    {
        return (uint32_t)(2U * VENC_LINE_BUF_DEPTH * CAM_MACROBLOCK_HEIGHT);
    }
    else
    {
        return (uint32_t)hVencH264Instance.cfgH264Main.height;
    }
}

/**
 * @brief Returns the video frame rate (FPS) from the H.264 encoder configuration.
 * @retval uint32_t The computed frames-per-second value.
 */
uint32_t GetVideoFramerate(void)
{
    uint32_t num = hVencH264Instance.cfgH264Main.frameRateNum;
    uint32_t den = hVencH264Instance.cfgH264Main.frameRateDenom;
    return den ? (num / den) : 0U;
}

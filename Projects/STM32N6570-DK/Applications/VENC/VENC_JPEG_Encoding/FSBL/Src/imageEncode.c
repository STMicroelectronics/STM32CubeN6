#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "jpegencapi.h"
#include "stm32n6xx_ll_venc.h"






typedef struct jpegInfo_t
{
  
  uint32_t  frameSeqNum;
  uint32_t  frameHeader;
}jpegInfo_t;


typedef struct venc_jpeg_cfg_t
{
  JpegEncCfg                    cfgJpeg;
  jpegInfo_t                    cfgJpegInfo;
}venc_jpeg_cfg_t;



venc_jpeg_cfg_t hVencJpegPluginInstance={
	 .cfgJpeg.qLevel                           = 9,                                   // Quantization level (0 - 9)
//	 .cfgJpeg.inputWidth                       = 1920,                                // Number of pixels/line in input image
//	 .cfgJpeg.inputHeight                      = 1080,                                // Number of lines in input image
	 .cfgJpeg.frameType                        = JPEGENC_RGB565,                      // Input frame YUV / RGB format
//	 .cfgJpeg.codingWidth                      = 1920,                                // Width of encoded image
//	 .cfgJpeg.codingHeight                     = 1080,                                // Height of encoded image
	 .cfgJpeg.xOffset                          = 0,                                   // Pixels from top-left corner of input image
	 .cfgJpeg.yOffset                          = 0,                                   // to top-left corner of encoded image
	 .cfgJpeg.restartInterval                  = 0,                                   // Restart interval (MCU lines)
	 .cfgJpegInfo.frameHeader                  = 1,                                   // Enable/disable creation of frame headers 
	 .cfgJpegInfo.frameSeqNum                  = 1,                                   // Number of images to encode sequentially 0=Disabled
	 .cfgJpeg.rotation                         = JPEGENC_ROTATE_0,                    //  RGB to YUV conversion
	 .cfgJpeg.codingType                       = JPEGENC_WHOLE_FRAME,                 // Whole frame / restart interval
	 .cfgJpeg.codingMode                       = JPEGENC_420_MODE,                    //  4:2:0 / 4:2:2 coding
	 .cfgJpeg.unitsType                        = JPEGENC_DOTS_PER_INCH,               // Units for X & Y density in APP0
	 .cfgJpeg.markerType                       = JPEGENC_SINGLE_MARKER,               // Table marker type
	 .cfgJpeg.xDensity                         = 72,                                  // Horizontal pixel density
	 .cfgJpeg.yDensity                         = 72,                                  // Vertical pixel density
};

static JpegEncInst jpegInstance;
static JpegEncInst jpegInstance;
static JpegEncIn   jpegEncIn= {0};
static JpegEncOut  jpegEncOut= {0};
static uint32_t *pOutpout;
static uint32_t  szOutpout;


uint8_t encoder_jpeg_init(uint32_t type,int32_t width,int32_t height, void *pOut,uint32_t szOut)
{
  int32_t  ret;
  LL_VENC_Init();
  pOutpout = pOut;
  szOutpout = szOut;
  hVencJpegPluginInstance.cfgJpeg.inputWidth = width;
  hVencJpegPluginInstance.cfgJpeg.inputHeight = height;
  hVencJpegPluginInstance.cfgJpeg.codingWidth = width;
  hVencJpegPluginInstance.cfgJpeg.codingHeight = height;
  
  hVencJpegPluginInstance.cfgJpeg.frameType    = (JpegEncFrameType)type;
  
   
  /* ATTENTION: alloc uncached memory for the stream */
//   pOutpout  = allocated st_os_mem_alloc(st_os_mem_type_from_name("JPEG"),szPool);
  if(pOutpout == NULL)
  {
    printf("Jpeg alloc %lu\n", (unsigned long)szOutpout);
    return 0;
  }
  ret = JpegEncInit(&hVencJpegPluginInstance.cfgJpeg, &jpegInstance);
  if (ret != JPEGENC_OK)
  {
    printf("Error: JpegEncInit\n");
    return 0;
  }
  
  
  ret = JpegEncSetPictureSize(jpegInstance, &hVencJpegPluginInstance.cfgJpeg);
  
  if(ret  != JPEGENC_OK)
  {
    printf("Error : JpegEncSetPictureSize \n");
    return 0;
  }
  memset(&jpegEncIn,0,sizeof(jpegEncIn));
  jpegEncIn.pOutBuf = (u8 *)pOutpout;
  jpegEncIn.busOutBuf = (uint32_t) pOutpout;
  jpegEncIn.outBufSize = szOutpout;
  return 1;
}

void encoder_jpeg_end(void){
  LL_VENC_DeInit();
  JpegEncRelease(&jpegInstance);
}




int32_t encode_jpeg_frame(const void *pFrame2Encode )
{
  int32_t ret=0;
  
  /* ATTENTION: the frame to encode must be in memory uncached */
  
  jpegEncIn.busLum = (uint32_t)pFrame2Encode; 
  jpegEncIn.busCb = jpegEncIn.busLum + hVencJpegPluginInstance.cfgJpeg.inputWidth * hVencJpegPluginInstance.cfgJpeg.inputHeight;
  jpegEncIn.busCr = jpegEncIn.busCb + (hVencJpegPluginInstance.cfgJpeg.inputWidth/2) * (hVencJpegPluginInstance.cfgJpeg.inputHeight/2);
  jpegEncIn.frameHeader = hVencJpegPluginInstance.cfgJpegInfo.frameHeader;
  
  
  /* Loop until the frame is ready, seveal loop in case of slicing configuration */
  do
  {
    ret = JpegEncEncode(jpegInstance, &jpegEncIn, &jpegEncOut, NULL, NULL);
    switch (ret)
    {
    case JPEGENC_RESTART_INTERVAL:
      break;
    case JPEGENC_FRAME_READY:
      return jpegEncOut.jfifSize;
    case JPEGENC_SYSTEM_ERROR:
    default:
      printf("Error: JpegEncEncode \n");
      return 0;
    }
  }
  while (ret == JPEGENC_RESTART_INTERVAL);
  return 0;
}


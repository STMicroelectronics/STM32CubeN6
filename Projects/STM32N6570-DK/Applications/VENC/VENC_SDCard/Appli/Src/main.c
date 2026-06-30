/**
******************************************************************************
* @file    main.c
* @author  MCD Application Team
* @brief   VENC SDCard bare-metal application for STM32N6xx devices.
******************************************************************************
* @attention
*
* Copyright (c) 2023 STMicroelectronics.
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
#include "stdio.h"

#include "ewl.h"
#include "h264encapi.h"
#include "stm32n6xx_ll_venc.h"
#include "stm32n6570_discovery.h"
#include "stm32n6570_discovery_sd.h"
#include "stm32n6570_discovery_lcd.h"
#include "stm32n6570_discovery_camera.h"

/** @addtogroup Templates
* @{
*/

/** @addtogroup HAL
* @{
*/

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define FRAMERATE 30
/* number of frames to film and encode */
#define VIDEO_FRAME_NB 600

#define USE_SD_AS_OUTPUT 1

/* Private macro -------------------------------------------------------------*/
#ifndef TEST_WIDTH
#define TEST_WIDTH     800
#endif
#ifndef TEST_HEIGHT
#define TEST_HEIGHT    480
#endif

#define VENC_WIDTH     TEST_WIDTH
#define VENC_HEIGHT    TEST_HEIGHT


/* enable trace if possible */
#if USE_COM_LOG || defined(TERMINAL_IO)
#define TRACE_MAIN(...) printf(__VA_ARGS__)
#else
#define TRACE_MAIN(...)
#endif /* USE_COM_LOG || defined(TERMINAL_IO) */

/* Private variables ---------------------------------------------------------*/

uint16_t * pipe_buffer[2];
volatile uint8_t buf_index_changed = 0;
H264EncIn encIn;
H264EncOut encOut;
H264EncInst encoder;
H264EncConfig cfg;
uint32_t output_size = 0;
uint32_t img_addr = 0;

EWLLinearMem_t outbuf;
static int frame_nb = 0;
uint32_t output_buffer[VENC_WIDTH*VENC_HEIGHT/8] __NON_CACHEABLE __attribute__((aligned(8)));

#if USE_SD_AS_OUTPUT
/* Each write = 512 Blocks of 512 Bytes */
#define BLOCK_SIZE         (512) /* 1 Block = 512 Bytes */
#define NB_BLOCKS_TO_WRITE (128)  /* Write 128 Blocks at once*/
#define NB_BYTES_TO_WRITE (NB_BLOCKS_TO_WRITE*BLOCK_SIZE)
#define NB_WORDS_TO_WRITE (NB_BYTES_TO_WRITE/4)
#define NB_BLOCKS_ERASED  (1024*1024) /*Erase 512 MBytes = 512*1024*1024 */

uint32_t sd_buf1[NB_WORDS_TO_WRITE] __NON_CACHEABLE; 
uint32_t sd_buf2[NB_WORDS_TO_WRITE] __NON_CACHEABLE;

uint32_t * curr_buf = sd_buf1;
size_t buf_index = 0;
size_t SD_index = 0;
#endif /* USE_SD_AS_OUTPUT */
/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static void MPU_Config(void);
static int encoder_prepare(uint32_t width, uint32_t height, uint32_t * output_buffer);
static int Encode_frame(void);
static int encoder_end(void);

/* change save stream and read frame for preferred in/output */
static int save_stream(uint32_t offset, uint32_t * buf, size_t size);
static int erase_enc_output(void);

/* Private functions ---------------------------------------------------------*/

/**
* @brief  Save an encoded buffer fragment at the given offset.
* @param  offset
* @param  buf  pointer to the buffer to save
* @param  size  size (in bytes) of the buffer to save
* @retval err error code. 0 On success.
*/
int save_stream(uint32_t offset, uint32_t * buf, size_t size){
  int err = 0;
  size += 15; /* Alignment*/
  size = size / sizeof(uint32_t);
  
#if USE_SD_AS_OUTPUT
  for(int i = 0; i<size; i++){
    curr_buf[buf_index] = buf[i];
    buf_index++;
    /* upload to sd every 512 blocks to limit the impact of access latency */
    if(buf_index >= NB_WORDS_TO_WRITE){
      if(BSP_SD_WriteBlocks_DMA(0, curr_buf, SD_index, NB_BLOCKS_TO_WRITE)!= BSP_ERROR_NONE){
        err = -1;
      }
      SD_index+=NB_BLOCKS_TO_WRITE;
      /* swap buffers */
      buf_index = 0;
      curr_buf = (curr_buf == sd_buf1)?sd_buf2:sd_buf1;
    }
  }
#endif
  return err;
}

int flush_out_buffer(void){
#if USE_SD_AS_OUTPUT
  if(BSP_SD_WriteBlocks(0, (uint32_t *) curr_buf, SD_index, NB_BLOCKS_TO_WRITE)!= BSP_ERROR_NONE){
        return -1;
      }
#endif
  return 0;
}
/**
* @brief  erases data in output medium
* @retval err error code. 0 On success.
*/
int erase_enc_output(void){
#if USE_SD_AS_OUTPUT
  /* Erase beginning of SDCard */
  if (BSP_SD_Erase(0, 0, NB_BLOCKS_ERASED) != BSP_ERROR_NONE)
  {
    TRACE_MAIN("failed to erase external flash block nb \n");
    return -1;
  }
#endif
  return 0;
}


/**
* @brief  Main program
* @param  None
* @retval None
*/
int main(void)
{
  /* enable MPU configuration to create non cacheable sections */
  MPU_Config();

  /* Enable DCache */
  SCB_EnableDCache();
  /* Enable ICache */
  SCB_EnableICache();

  HAL_Init();

  SystemCoreClockUpdate();

  /* Initialize the HAL timebase (eg. SysTick) */
  HAL_Init();

  /* oscillator and PLL already configured. Configure periph clocks */
  SystemClock_Config();

  __HAL_RCC_SYSCFG_CLK_ENABLE();

  /* set all required IPs as secure privileged */
  __HAL_RCC_RIFSC_CLK_ENABLE();
  RIMC_MasterConfig_t RIMC_master = {0};
  RIMC_master.MasterCID = RIF_CID_1;
  RIMC_master.SecPriv = RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV;
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_SDMMC2, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_DCMIPP, &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC1 , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_LTDC2 , &RIMC_master);
  HAL_RIF_RIMC_ConfigMasterAttributes(RIF_MASTER_INDEX_VENC  , &RIMC_master);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_SDMMC2 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_DCMIPP , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_CSI    , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_VENC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDC   , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL1 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);
  HAL_RIF_RISC_SetSlaveSecureAttributes(RIF_RISC_PERIPH_INDEX_LTDCL2 , RIF_ATTRIBUTE_SEC | RIF_ATTRIBUTE_PRIV);

  /* Check expected frequency */
  /* UART log */
#if USE_COM_LOG
  COM_InitTypeDef COM_Init;

  /* Initialize COM init structure */
  COM_Init.BaudRate   = 115200;
  COM_Init.WordLength = COM_WORDLENGTH_8B;
  COM_Init.StopBits   = COM_STOPBITS_1;
  COM_Init.Parity     = COM_PARITY_NONE;
  COM_Init.HwFlowCtl  = COM_HWCONTROL_NONE;

  BSP_COM_Init(COM1, &COM_Init);

  if (BSP_COM_SelectLogPort(COM1) != BSP_ERROR_NONE)
  {
    TRACE_MAIN("failed to set up log port\n");
    Error_Handler();
  }
#endif
  /* initialize LEDs to signal processing is ongoing */
  BSP_LED_Init(LED1);
  BSP_LED_Init(LED2);

  TRACE_MAIN("CPU frequency    : %lu\n", (unsigned long)(HAL_RCC_GetCpuClockFreq() / 1000000U));
  TRACE_MAIN("sysclk frequency : %lu\n", (unsigned long)(HAL_RCC_GetSysClockFreq() / 1000000U));
  TRACE_MAIN("pclk5 frequency  : %lu\n", (unsigned long)(HAL_RCC_GetPCLK5Freq() / 1000000U));

  /* initialize ext flash interface and driver */
#if USE_SD_AS_OUTPUT
  if (BSP_SD_Init(0) != BSP_ERROR_NONE){
    TRACE_MAIN("error initializing SD card\n");
    Error_Handler();
  }
  BSP_SD_CardInfo card_info;
  BSP_SD_GetCardInfo(0, &card_info);
  TRACE_MAIN("SD card info : \nblock Nbr : %lu\nblock size : %lu\ncard speed : %lu\n",
             (unsigned long)card_info.BlockNbr,
             (unsigned long)card_info.BlockSize,
             (unsigned long)card_info.CardSpeed);

#endif
   /* Initialize camera */
  if(BSP_CAMERA_Init(0, CAMERA_R2592x1944, CAMERA_PF_RAW_RGGB10) != BSP_ERROR_NONE){
    Error_Handler();
  }

  /* Initialize LCD */
  int err = BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LCD_PIXEL_FORMAT_RGB565, LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
  if(err){
    TRACE_MAIN("error initializing LCD : %d\n", err);
    Error_Handler();
  }
  /* start camera acquisition */
  if(BSP_CAMERA_DoubleBufferStart(0, (uint8_t *)(0x34040000),(uint8_t *)(0x34040000 + VENC_WIDTH*VENC_HEIGHT*2), CAMERA_MODE_CONTINUOUS)!= BSP_ERROR_NONE){
    Error_Handler();
  }

  /* erase output*/
  TRACE_MAIN("erasing flash output blocks\n");
  erase_enc_output();
  TRACE_MAIN("Done erasing output flash blocks\n");

#if USE_SD_AS_OUTPUT
  /* wait for erase operation to be done */
  while(BSP_SD_GetCardState(0) != SD_TRANSFER_OK);
#endif
  /* initialize VENC */
  LL_VENC_Init();

  /* initialization done. Turn on the LEDs */
  BSP_LED_On(LED1);
  BSP_LED_On(LED2);
  /* allocate output buffer */


  /* initialize encoder software for camera feed encoding */
  encoder_prepare(VENC_WIDTH,VENC_HEIGHT,output_buffer);

  while (frame_nb < VIDEO_FRAME_NB)
  {
    if(BSP_CAMERA_BackgroundProcess() != BSP_ERROR_NONE)
    {
      TRACE_MAIN("Error in BSP image processing\n");
    }
    if(buf_index_changed){
      /* new frame available */
      buf_index_changed = 0;
      Encode_frame();
    }
  }
  /* after encoding a certain nb of frames, end program */
  encoder_end();
  flush_out_buffer();
  BSP_LED_Off(LED1);
  BSP_LED_Off(LED2);

  /* program ended */
  while(1);
}


static int encoder_prepare(uint32_t width, uint32_t height, uint32_t * output_buffer)
{
  H264EncRet ret;

  H264EncPreProcessingCfg preproc_cfg = {0};

  /* software workaround for Linemult triggering VENC interrupt. Make it happen as little as possible */
  MODIFY_REG(DCMIPP->P1PPCR, DCMIPP_P1PPCR_LINEMULT_Msk,DCMIPP_MULTILINE_128_LINES);

  frame_nb = 0;
  /* Step 1: Initialize an encoder instance */
  /* set config to 1 ref frame */
  cfg.refFrameAmount = 1;
  /* 30 fps frame rate */
  cfg.frameRateDenom = 1;
  cfg.frameRateNum = FRAMERATE;
  /* Image resolution */
  cfg.width = width;
  cfg.height = height;
  /* Stream type */
  cfg.streamType = H264ENC_BYTE_STREAM;

  /* encoding level*/
  /*See API guide for level depending on resolution and framerate*/
  cfg.level = H264ENC_LEVEL_2_2;
  cfg.svctLevel = 0;

  /* Output buffer size */
  outbuf.size = cfg.width * cfg.height;

  ret = H264EncInit(&cfg, &encoder);
  if (ret != H264ENC_OK)
  {
    TRACE_MAIN("error initializing encoder %d\n", ret);
    return -1;
  }

  /* set format conversion for preprocessing */
  ret = H264EncGetPreProcessing(encoder, &preproc_cfg);
  if(ret != H264ENC_OK){
    TRACE_MAIN("error getting preproc data\n");
    return -1;
  }
  preproc_cfg.inputType = H264ENC_RGB565;
  ret = H264EncSetPreProcessing(encoder, &preproc_cfg);
  if(ret != H264ENC_OK){
    TRACE_MAIN("error setting preproc data\n");
    return -1;
  }

  /*assign buffers to input structure */
  encIn.pOutBuf = output_buffer;
  encIn.busOutBuf = (uint32_t) output_buffer;
  encIn.outBufSize = width * height/2;

  /* create stream */
  ret = H264EncStrmStart(encoder, &encIn, &encOut);
  if (ret != H264ENC_OK)
  {
    TRACE_MAIN("error starting stream\n");
    return -1;
  }

  /* save the stream header */
  if (save_stream(output_size, encIn.pOutBuf,  encOut.streamSize))
  {
    TRACE_MAIN("error saving stream\n");
    return -1;
  }
  TRACE_MAIN("stream started. saved %lu bytes\n", (unsigned long)encOut.streamSize);
  output_size+= encOut.streamSize;
  return 0;
}


static int Encode_frame(){
  int ret = H264ENC_FRAME_READY;
  if(!img_addr){
    TRACE_MAIN("Error : NULL image address");
    return -1;
  }
  if (!frame_nb)
  {
    /* if frame is the first : set as intra coded */
    encIn.timeIncrement = 0;
    encIn.codingType = H264ENC_INTRA_FRAME;
  }
  else
  {
    /* if there was a frame previously, set as predicted */
    encIn.timeIncrement = 1;
    encIn.codingType = H264ENC_PREDICTED_FRAME;
  }
  encIn.ipf = H264ENC_REFERENCE_AND_REFRESH;
  encIn.ltrf = H264ENC_REFERENCE;
  /* set input buffers to structures */
  encIn.busLuma = img_addr;
  ret = H264EncStrmEncode(encoder, &encIn, &encOut, NULL, NULL, NULL);
  switch (ret)
  {
  case H264ENC_FRAME_READY:
    /*save stream */
    if (save_stream(output_size, encIn.pOutBuf,  encOut.streamSize))
    {
      TRACE_MAIN("error saving stream frame %d\n", frame_nb);
      return -1;
    }
    output_size += encOut.streamSize;
    break;
  case H264ENC_SYSTEM_ERROR:
    TRACE_MAIN("fatal error while encoding\n");
    break;
  default:
    TRACE_MAIN("error encoding frame %d : %d\n", frame_nb, ret);
    break;
  }
  frame_nb++;
  return 0;
}


static int encoder_end(void){
  int ret = H264EncStrmEnd(encoder, &encIn, &encOut);
  TRACE_MAIN("done encoding %d frames. size : %lu - Blocks : %lu\n",
             frame_nb,
             (unsigned long)output_size,
             (unsigned long)((output_size + 511U) / 512U));
  if (ret != H264ENC_OK)
  {
    return -1;
  }
  else
  {
    /* save stream tail */
    if (save_stream(output_size, encIn.pOutBuf,  encOut.streamSize))
    {
      TRACE_MAIN("error saving stream\n");
      return -1;
    }
    output_size+=encOut.streamSize;
  }

  return 0;
}

/**
* @brief  System Clock Configuration
*         The system Clock is configured as follow :
*            CPU Clock source               = IC1
*            System bus Clock source        = IC2
*            SYSCLK(Hz)                     = 400000000
*            HCLK(Hz)                       = 200000000
*            AHB Prescaler                  = 2
*            APB1 Prescaler                 = 1
*            APB2 Prescaler                 = 1
*            APB4 Prescaler                 = 1
*            APB5 Prescaler                 = 1
*            HSI Frequency(Hz)              = 64000000
*            PLL1 State                     = ON
*            PLL2 State                     = ON
*            PLL3 State                     = BYPASS
*            PLL4 State                     = ON
* @retval None
*/
static void SystemClock_Config(void)
{
#if defined(DEBUG)
  /* if in debug mode, clock is not configured when entering app. Configure it here */
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Configure the system Power Supply */
  if (HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  /* Get current CPU/System buses clocks configuration */
  /* and if necessary switch to intermediate HSI clock */
  /* to ensure target clock can be set                 */
  HAL_RCC_GetClockConfig(&RCC_ClkInitStruct);
  if ((RCC_ClkInitStruct.CPUCLKSource == RCC_CPUCLKSOURCE_IC1) ||
      (RCC_ClkInitStruct.SYSCLKSource == RCC_SYSCLKSOURCE_IC2_IC6_IC11))
  {
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK);
    RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
    {
      /* Initialization Error */
      Error_Handler();
    }
  }

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL1.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL1.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL1.PLLM = 4;
  RCC_OscInitStruct.PLL1.PLLN = 75;
  RCC_OscInitStruct.PLL1.PLLP1 = 1;
  RCC_OscInitStruct.PLL1.PLLP2 = 1;
  RCC_OscInitStruct.PLL1.PLLFractional = 0;

  RCC_OscInitStruct.PLL2.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL2.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL2.PLLM = 8;
  RCC_OscInitStruct.PLL2.PLLFractional = 0;
  RCC_OscInitStruct.PLL2.PLLN = 125;
  RCC_OscInitStruct.PLL2.PLLP1 = 1;
  RCC_OscInitStruct.PLL2.PLLP2 = 1;

  RCC_OscInitStruct.PLL3.PLLState = RCC_PLL_BYPASS;
  RCC_OscInitStruct.PLL3.PLLSource = RCC_PLLSOURCE_HSI;

  RCC_OscInitStruct.PLL4.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL4.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL4.PLLM = 32;
  RCC_OscInitStruct.PLL4.PLLFractional = 0;
  RCC_OscInitStruct.PLL4.PLLN = 40;
  RCC_OscInitStruct.PLL4.PLLP1 = 1;
  RCC_OscInitStruct.PLL4.PLLP2 = 1;

  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL1 outputs as CPU and System bus clock source */
  /* CPUCLK (sysa_ck) = ic1_ck = PLL1 output/ic1_divider = 600 MHz */
  /* SYSCLK AXI (sysb_ck) = ic2_ck = PLL1 output/ic2_divider = 400 MHz */
  /* SYSCLK NPU (sysc_ck) = ic6_ck = PLL1 output/ic6_divider = 400 MHz */
  /* SYSCLK AXISRAM3/4/5/6 (sysd_ck) = ic11_ck = PLL1 output/ic11_divider = 400 MHz */
  /* Configure the HCLK, PCLK1, PCLK2, PCLK4 and PCLK5 clocks dividers */
  /* HCLK = ic2_ck = PLL1 output/HCLK divider = 200 MHz */
  /* PCLKx = HCLK / PCLKx divider = 200 MHz */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_CPUCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK  | \
                                 RCC_CLOCKTYPE_PCLK1  | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_PCLK4  | RCC_CLOCKTYPE_PCLK5);
  RCC_ClkInitStruct.CPUCLKSource = RCC_CPUCLKSOURCE_IC1;
  RCC_ClkInitStruct.IC1Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC1Selection.ClockDivider = 2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_IC2_IC6_IC11;
  RCC_ClkInitStruct.IC2Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC2Selection.ClockDivider = 3;
  RCC_ClkInitStruct.IC6Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC6Selection.ClockDivider = 3;
  RCC_ClkInitStruct.IC11Selection.ClockSelection = RCC_ICCLKSOURCE_PLL1;
  RCC_ClkInitStruct.IC11Selection.ClockDivider = 3;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
  RCC_ClkInitStruct.APB5CLKDivider = RCC_APB5_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
#endif
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_SDMMC2;
  PeriphClkInit.Sdmmc2ClockSelection = RCC_SDMMC2CLKSOURCE_IC4;
  PeriphClkInit.ICSelection[RCC_IC4].ClockSelection = RCC_ICCLKSOURCE_PLL1;
  PeriphClkInit.ICSelection[RCC_IC4].ClockDivider = 6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    while (1);
  }
}

HAL_StatusTypeDef MX_DCMIPP_ClockConfig(DCMIPP_HandleTypeDef *hdcmipp)
{
 RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /* Configure DCMIPP ck_ker_dcmipp to ic17 with PLL2 (1000MHz) / 3 = 333MHz */
  PeriphClkInit.PeriphClockSelection |= RCC_PERIPHCLK_DCMIPP;
  PeriphClkInit.DcmippClockSelection = RCC_DCMIPPCLKSOURCE_IC17;
  PeriphClkInit.ICSelection[RCC_IC17].ClockSelection = RCC_ICCLKSOURCE_PLL2;
  PeriphClkInit.ICSelection[RCC_IC17].ClockDivider = 3;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    return HAL_ERROR;
  }
  LL_RCC_IC18_SetSource(LL_RCC_ICCLKSOURCE_PLL1);
  LL_RCC_IC18_SetDivider(60);   /* 1200/60=20Mhz */
  LL_RCC_IC18_Enable();

  return HAL_OK;
}


static void dcmipp_downsize(DCMIPP_DownsizeTypeDef * DonwsizeConf, int32_t camWidth,int32_t camHeight,int32_t captureWidth,int32_t captureHeight)
{
  /* Calcultation explained in RM0486v2 table 354*/
  
  /* Configure the downsize */
  DonwsizeConf->HRatio      = (uint32_t)((((float)(camWidth)) / ((float)(captureWidth))) * 8192.F); 
  DonwsizeConf->VRatio      = (uint32_t)((((float)(camHeight )) / ((float)(captureHeight ))) * 8192.F); 
  DonwsizeConf->HSize       = captureWidth;
  DonwsizeConf->VSize       = captureHeight;

  DonwsizeConf->HDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DonwsizeConf->HRatio);
  DonwsizeConf->VDivFactor  = (uint32_t)floor((1024 * 8192 -1) / DonwsizeConf->VRatio);

  return ;
}

/**
  * @brief  Initializes the DCMIPP peripheral
  * @param  hdcmipp  DCMIPP handle
  * @note   Being __weak it can be overwritten by the application
  * @retval HAL status
  */
HAL_StatusTypeDef MX_DCMIPP_Init(DCMIPP_HandleTypeDef *hdcmipp)
{
  DCMIPP_PipeConfTypeDef pPipeConf = {0};
  DCMIPP_CSI_PIPE_ConfTypeDef pCSIPipeConf = {0};
  DCMIPP_CSI_ConfTypeDef csiconf = {0};
  DCMIPP_DownsizeTypeDef DonwsizeConf ={0};

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

  pPipeConf.FrameRate  = DCMIPP_FRAME_RATE_ALL;
  pPipeConf.PixelPackerFormat = DCMIPP_PIXEL_PACKER_FORMAT_RGB565_1;

  /* Set Pitch for Main and Ancillary Pipes */
  pPipeConf.PixelPipePitch  = VENC_WIDTH*2 ; /* Number of bytes */

  /* Configure Pipe */
  if (HAL_DCMIPP_PIPE_SetConfig(hdcmipp, DCMIPP_PIPE1, &pPipeConf) != HAL_OK)
  {
    return HAL_ERROR;
  }

   /* Calcultation explained in RM0486v2 table 354*/
  

  dcmipp_downsize(&DonwsizeConf, 2592/*camWidth*/, 1944 /*camHeight*/, VENC_WIDTH, VENC_HEIGHT);

  HAL_DCMIPP_PIPE_SetDownsizeConfig(hdcmipp, DCMIPP_PIPE1, &DonwsizeConf);
  HAL_DCMIPP_PIPE_EnableDownsize(hdcmipp, DCMIPP_PIPE1);

  return HAL_OK;
}

void BSP_CAMERA_FrameEventCallback(uint32_t instance)
{
  /* swap buffers and signal new frame*/
  img_addr = DCMIPP->P1STM0AR;
  buf_index_changed = 1;
  BSP_LCD_SetLayerAddress(0, 0, img_addr);
  BSP_LCD_Reload(0, BSP_LCD_RELOAD_VERTICAL_BLANKING);
}



HAL_StatusTypeDef MX_LTDC_ClockConfig(LTDC_HandleTypeDef *hltdc)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(hltdc);
  RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};

  RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LTDC;
  RCC_PeriphCLKInitStruct.LtdcClockSelection = RCC_LTDCCLKSOURCE_IC16;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockSelection = RCC_ICCLKSOURCE_PLL4;
  RCC_PeriphCLKInitStruct.ICSelection[RCC_IC16].ClockDivider = 2;
  if (HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct) != HAL_OK)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

void Error_Handler(void)
{
  BSP_LED_Off(LED_GREEN);
  while (1)
  {
    BSP_LED_Toggle(LED_RED);
    HAL_Delay(250);
  }
}

#if defined(__ICCARM__)
#pragma section="HEAP"
#define HEAP_START   ((uint32_t) __sfb("HEAP"))
#define HEAP_END      ((uint32_t) __sfe("HEAP"))
#elif defined(__ARMCC_VERSION)
extern uint32_t Image$$ARM_LIB_HEAP$$ZI$$Base;
extern uint32_t Image$$ARM_LIB_HEAP$$ZI$$Limit;
#define HEAP_START ((uint32_t) &Image$$ARM_LIB_HEAP$$ZI$$Base)
#define HEAP_END   ((uint32_t) &Image$$ARM_LIB_HEAP$$ZI$$Limit-1)
#elif defined(__GNUC__)
extern uint8_t _end; /* Symbol defined in the linker script */
extern uint32_t _Min_Heap_Size; /* Symbol defined in the linker script */
#define HEAP_START    ((uint32_t) &_end)
#define HEAP_END      ((uint32_t) &_end + (uint32_t) &_Min_Heap_Size - 1)
#endif
static void MPU_Config(void)
{

  MPU_Region_InitTypeDef default_config = {0};
  MPU_Attributes_InitTypeDef attr_config = {0};
  uint32_t primask_bit = __get_PRIMASK();
  __disable_irq();

  /* disable the MPU */
  HAL_MPU_Disable();

  /* create an attribute configuration for the MPU */
  attr_config.Attributes = INNER_OUTER(MPU_NOT_CACHEABLE);
  attr_config.Number = MPU_ATTRIBUTES_NUMBER0;

  HAL_MPU_ConfigMemoryAttributes(&attr_config);

  /* Create a non cacheable region */
  /*Normal memory type, code execution unallowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER0;
  default_config.BaseAddress = __NON_CACHEABLE_SECTION_BEGIN;
  default_config.LimitAddress =  __NON_CACHEABLE_SECTION_END;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigRegion(&default_config);

  /* Create a non cacheable region for VENC HW/SW shared buffers */
  /*Normal memory type, code execution unallowed */
  default_config.Enable = MPU_REGION_ENABLE;
  default_config.Number = MPU_REGION_NUMBER1;
  default_config.BaseAddress = HEAP_START;
  default_config.LimitAddress = HEAP_END;
  default_config.DisableExec = MPU_INSTRUCTION_ACCESS_DISABLE;
  default_config.AccessPermission = MPU_REGION_ALL_RW;
  default_config.IsShareable = MPU_ACCESS_NOT_SHAREABLE;
  default_config.AttributesIndex = MPU_ATTRIBUTES_NUMBER0;
  HAL_MPU_ConfigRegion(&default_config);

  /* enable the MPU */
  HAL_MPU_Enable(MPU_PRIVILEGED_DEFAULT);

  /* Exit critical section to lock the system and avoid any issue around MPU mechanisme */
  __set_PRIMASK(primask_bit);
}
#ifdef USE_FULL_ASSERT

/**
* @brief  Reports the name of the source file and the source line number
*         where the assert_param error has occurred.
* @param  file  pointer to the source file name
* @param  line  assert_param error line source number
* @retval None
*/
void assert_failed(uint8_t *file, uint32_t line)
{
  TRACE_MAIN("assert failed at line %d of file %s\n", line, file);
  /* Infinite loop */
  while (1)
  {
  }
}
#endif

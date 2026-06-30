/**
******************************************************************************
* @file    sdcard_app.c
* @author  MCD Application Team
* @brief   VENC SDCard application for STM32N6xx: handles video encoding and SD card recording.
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

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32n6570_discovery.h"
#include "stdio.h"
#include "tx_api.h"
#include "utils.h"
#include "app_filex.h"
#include "h264encapi.h"
#include "st_monitor_bitrate.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#define NB_FRAMES_PER_FILE (30U*10U) /* 10sec  @ 30 fps*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

INT VENC_APP_GetData(UCHAR **data, ULONG *size);


/**
* @brief  SDCard application thread function.
* @param  arg Thread argument (unused).
*
*/
void sdcard_thread_func(ULONG arg)
{
  CHAR filename[10];
  UINT filenumber = 0;
  UCHAR * data = NULL;
  ULONG size = 0;
  ULONG nb_frames = 0;
  ULONG record = 1;
  INT res;
  UINT open_new_file = 1;
  
  H264EncPictureCodingType frame_type = H264ENC_NOTCODED_FRAME;
  
  if (VENC_FileX_Init() != FX_SUCCESS)
  {
    printf("FileX init failed\n");
    return;
  }
  
  /* Wait for the first I-frame. */
  do {
    res = VENC_APP_GetData(&data, &size);
  }
  while (res != H264ENC_INTRA_FRAME);
  /* Ensure the very first frame written is the I-frame we just received */
  frame_type = (H264EncPictureCodingType)res;
  
  while(record)
  {
    if (open_new_file)
    {
      if (filenumber)
      {
        if  (VENC_FileX_close() !=  FX_SUCCESS)
        {
          printf("FileX failed to close %s\n", filename);
        }
      }
      
      sprintf(filename, "%04d.h264", filenumber);
      
      printf("Open file %s\n", filename);
      if (VENC_FileX_Open(filename)  != FX_SUCCESS)
      {
        printf("FileX failed to open %s\n", filename);
        return;
      }
      filenumber++;
      nb_frames = 1;
    }
    
    /* Write the frame to the SD card. */
    if (data && size)
    {
      VENC_FileX_write((CHAR*)data, (LONG)size);
      monitor_bitrate("video", (uint32_t)size);
      data = NULL; size = 0;
      BSP_LED_Toggle(LED_RED);
    }
    
    res = VENC_APP_GetData(&data, &size);
    if (res < 0)
    {
      printf("Failed to get encoded data\n");
      tx_thread_sleep(15U*TX_TIMER_TICKS_PER_SECOND/1000U);
    }
    else
    { 
      frame_type = (H264EncPictureCodingType)res;
      nb_frames++;
    } 
    
    open_new_file = (nb_frames >= NB_FRAMES_PER_FILE && frame_type == H264ENC_INTRA_FRAME);
  }
  
  VENC_FileX_close();
}


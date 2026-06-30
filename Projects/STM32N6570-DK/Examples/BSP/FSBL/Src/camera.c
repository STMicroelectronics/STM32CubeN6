/**
  ******************************************************************************
  * @file    camera.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the camera features of the
  *          stm32n6570_discovery_camera driver
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
/** @addtogroup STM32N6xx_HAL_Examples
  * @{
  */

/** @addtogroup BSP
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define WVGA_RES_X      800
#define WVGA_RES_Y      480
#define CAMERA_FRAME_BUFFER LCD_LAYER_1_ADDRESS
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static uint8_t MirrorFlipIndex = 0;
/* Camera resolutions */

uint8_t  CameraMirrorFlip[4] =
{
  CAMERA_MIRRORFLIP_NONE,
  CAMERA_MIRRORFLIP_FLIP,
  CAMERA_MIRRORFLIP_MIRROR,
  CAMERA_MIRRORFLIP_FLIP | CAMERA_MIRRORFLIP_MIRROR,
};

CAMERA_Capabilities_t Cap;

/* Private function prototypes -----------------------------------------------*/
static void Camera_SetHint(void);
static uint32_t Camera_ISP_IdleWait(uint32_t delay);

/* Private functions ---------------------------------------------------------*/
/**
  * @brief  Camera demo
  * @param  None
  * @retval None
  */
void Camera_Demo (void)
{
  Camera_SetHint();

  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  /* Display USB initialization message */
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_DARKBLUE);

  UTIL_LCD_SetFont(&Font24);
  BSP_LCD_LayerConfig_t cam_layer;
  cam_layer.Address = LCD_LAYER_1_ADDRESS;
  cam_layer.PixelFormat = LCD_PIXEL_FORMAT_RGB565;
  cam_layer.X0 = 0;
  cam_layer.X1 = 800;
  cam_layer.Y0 = 0;
  cam_layer.Y1 = 480;
  BSP_LCD_ConfigLayer(0, 1, &cam_layer);
  BSP_LCD_SetColorKeying(0, 1, UTIL_LCD_COLOR_WHITE);
  BSP_LCD_SetActiveLayer(0,1);
  BSP_LCD_SetLayerAddress(0, 0, LCD_LAYER_1_ADDRESS);
  BSP_LCD_SetLayerAddress(0, 1, LCD_LAYER_0_ADDRESS);
  /* Initialize the Camera */
  if(BSP_CAMERA_Init(0, CAMERA_R2592x1944, CAMERA_PF_RAW_RGGB10) != BSP_ERROR_NONE)
  {
    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    UTIL_LCD_DisplayStringAt(20, 0, (uint8_t *)" NO CAMERA CONNECTED", CENTER_MODE);
  }
  else
  {
    UTIL_LCD_DisplayStringAt(20, 35, (uint8_t *)"CAMERA CONTINUOUS MODE", CENTER_MODE);
    /* Wait for the camera initialization after HW reset*/
    HAL_Delay(100);

    BSP_CAMERA_GetCapabilities(0, &Cap);

    /* Start the Camera Capture */
    BSP_CAMERA_Start(0, (uint8_t *)CAMERA_FRAME_BUFFER, CAMERA_MODE_CONTINUOUS);

    UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_RED);
    Camera_ISP_IdleWait(5000);
    /* Start the Camera Capture */
    UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
    if(Cap.MirrorFlip != 1)
    {
      UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"Camera MirrorFlip feature not supported", CENTER_MODE);
      Camera_ISP_IdleWait(2000);
    }
    else
    {
      UTIL_LCD_DisplayStringAt(0, 35, (uint8_t *)"                                               ", CENTER_MODE);
      UTIL_LCD_DisplayStringAt(0, 70, (uint8_t *)"Camera Mirror/Flip", CENTER_MODE);
      /* Test Camera Mirror/Flip */
      while(MirrorFlipIndex < 4)
      {
        Camera_ISP_IdleWait(2000);
        BSP_CAMERA_SetMirrorFlip(0, CameraMirrorFlip[MirrorFlipIndex]);
        MirrorFlipIndex++;
      }
      Camera_ISP_IdleWait(2000);
      MirrorFlipIndex = 1;
      BSP_CAMERA_SetMirrorFlip(0, CameraMirrorFlip[MirrorFlipIndex]);
    }
    BSP_CAMERA_SetMirrorFlip(0, CAMERA_MIRRORFLIP_NONE);

    Camera_ISP_IdleWait(2000);
  }
  BSP_CAMERA_Stop(0);
  UTIL_LCD_FillRect(0, 0, 800, 480, UTIL_LCD_COLOR_WHITE);
  BSP_LCD_ResetColorKeying(0, 1);
  BSP_LCD_SetLayerAddress(0, 0, LCD_LAYER_0_ADDRESS);
  BSP_LCD_SetLayerAddress(0, 1, LCD_LAYER_1_ADDRESS);
  BSP_LCD_SetLayerVisible(0, 1, DISABLE);
  BSP_LCD_SetActiveLayer(0,0);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_DisplayStringAt(20, 35, (uint8_t *)"CAMERA Demo done. press USR1 to continue", CENTER_MODE);



  while (1)
  {
    if(CheckForUserInput() > 0)
    {
      ClearUserInput();
      BSP_CAMERA_DeInit(0);
      return;
    }
  }
}

/**
  * @brief  Display Camera demo hint
  * @param  None
  * @retval None
  */
static void Camera_SetHint(void)
{
  uint32_t x_size, y_size;

  BSP_LCD_GetXSize(0, &x_size);
  BSP_LCD_GetYSize(0, &y_size);

  /* Clear the LCD */
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  /* Set Camera Demo description */
  UTIL_LCD_FillRect(0, 0, x_size, 120, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_SetFont(&Font24);
  UTIL_LCD_DisplayStringAt(0, 0, (uint8_t *)"CAMERA EXAMPLE", CENTER_MODE);
  UTIL_LCD_SetFont(&Font12);
  UTIL_LCD_DisplayStringAt(0, 30, (uint8_t *)"     Press User button for next menu     ", CENTER_MODE);

  /* Set the LCD Text Color */
  UTIL_LCD_DrawRect(10, 130, x_size - 20, y_size - 130, UTIL_LCD_COLOR_BLUE);
  UTIL_LCD_DrawRect(11, 131, x_size - 22, y_size - 132, UTIL_LCD_COLOR_BLUE);
}

/**
  * @brief  Camera demo idle loop
  * @param  None
  * @retval None
  */
static uint32_t Camera_ISP_IdleWait(uint32_t delay){
  uint32_t initial_tick =  HAL_GetTick();
  while((HAL_GetTick() - initial_tick) < delay){
    int err = BSP_CAMERA_BackgroundProcess();
    if(err != BSP_ERROR_NONE){
      return err;
    }
  }
  return 0;
}
/**
  * @}
  */

/**
  * @}
  */


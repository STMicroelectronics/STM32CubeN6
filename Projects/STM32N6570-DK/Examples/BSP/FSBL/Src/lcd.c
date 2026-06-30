/**
  ******************************************************************************
  * @file    lcd.c
  * @author  MCD Application Team
  * @brief   This example code shows how to use the LCD display feature in the
  *          STM32N6570-DK driver
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
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t  colors[24] = {UTIL_LCD_COLOR_BLUE , UTIL_LCD_COLOR_GREEN, UTIL_LCD_COLOR_RED, UTIL_LCD_COLOR_CYAN,
                        UTIL_LCD_COLOR_MAGENTA, UTIL_LCD_COLOR_YELLOW,UTIL_LCD_COLOR_LIGHTBLUE, UTIL_LCD_COLOR_LIGHTGREEN,
                        UTIL_LCD_COLOR_LIGHTRED, UTIL_LCD_COLOR_WHITE, UTIL_LCD_COLOR_LIGHTMAGENTA, UTIL_LCD_COLOR_LIGHTYELLOW,
                        UTIL_LCD_COLOR_DARKGREEN, UTIL_LCD_COLOR_DARKRED, UTIL_LCD_COLOR_DARKCYAN,UTIL_LCD_COLOR_DARKMAGENTA,
                        UTIL_LCD_COLOR_LIGHTGRAY, UTIL_LCD_COLOR_GRAY, UTIL_LCD_COLOR_DARKGRAY, UTIL_LCD_COLOR_DARKYELLOW,
                        UTIL_LCD_COLOR_BLACK, UTIL_LCD_COLOR_BROWN, UTIL_LCD_COLOR_ORANGE, UTIL_LCD_COLOR_DARKBLUE};
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/**
  * @brief  LCD demo
  * @param  None
  * @retval None
  */
void Lcd_Demo(void)
{
  Point    triangle[3] = {{0, 0}, {0, 0}, {0, 0}};
  uint32_t colorIndex = 0;
  uint32_t x;
  uint32_t y;
  uint32_t  i, color_index;

  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(0, 0, 800, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test LCD  ", CENTER_MODE);

  /* Display a grid of colored rectangles with each rectangle being 80x80 pixels in size.
     There are 8 rectangles per row and 3 rows, each with a color from the colors array */
  for (color_index = 0; color_index < 24; color_index++)
  {
    int x = 80 * ((color_index % 8) + 1);
    int y = 80 * (color_index / 8 + 2);

    UTIL_LCD_FillRect(x, y, 80, 80, colors[color_index]);
  }

  HAL_Delay(1000);

  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);

  /* Display a grid of circles on an LCD screen */
  for(color_index = 0; color_index < 4; color_index++)
  {
    for(i = 0; i < 6; i++)
    {
      UTIL_LCD_DrawCircle(150 + (100*i), 100*(color_index+1), 40, UTIL_LCD_COLOR_BLACK);
    }
  }

  HAL_Delay(1000);

  /* Display a grid of colored circles */
  for(color_index = 0; color_index < 4; color_index++)
  {
    for(i = 0; i < 6; i++)
    {
      UTIL_LCD_FillCircle(150 + (100*i), 100*(color_index+1), 40, colors[colorIndex]);
      colorIndex++;
      HAL_Delay(50);
    }
  }

  HAL_Delay(1000);
  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);

  colorIndex = 0;
  /* Display a grid of colored triangles */
  for(color_index = 0; color_index < 6; color_index++)
  {
    for(i = 0; i < 4; i++)
    {
      triangle[0].X = 40*color_index;
      triangle[0].Y = 40 + 50*(i);
      triangle[1].X = 40*(color_index+1);
      triangle[1].Y = 65 + 50*(i);
      triangle[2].X = 40*color_index;
      triangle[2].Y = 40 + 50*(1+i);
      UTIL_LCD_FillPolygon(triangle, 3, colors[colorIndex]);
      if(colors[colorIndex] == UTIL_LCD_COLOR_WHITE)
      {
        UTIL_LCD_DrawPolygon(triangle, 3, UTIL_LCD_COLOR_BLACK);
      }
      colorIndex++;
    }
  }
  HAL_Delay(1000);

  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);

  /* Draw bitmap */
  UTIL_LCD_DrawBitmap(80, 130, (uint8_t *)stlogo);
  HAL_Delay(500);

  UTIL_LCD_DrawBitmap(240, 130, (uint8_t *)stlogo);

  HAL_Delay(500);

  UTIL_LCD_DrawBitmap(400, 130, (uint8_t *)stlogo);

  HAL_Delay(500);

  UTIL_LCD_DrawBitmap(560, 130, (uint8_t *)stlogo);

  HAL_Delay(1000);
  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);

  BSP_LCD_DisplayOff(0);

  if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LCD_PIXEL_FORMAT_RGB888, LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  BSP_LCD_DisplayOn(0);


  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(0, 0, 800, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test LCD  ", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"LCD RGB888", CENTER_MODE);

  for (color_index = 0; color_index < 16; color_index++)
  {
     x = 40 * ((color_index % 4) + 8) ;
     y = (40 * (color_index / 4 + 4));

    UTIL_LCD_FillRect(x, y, 40, 40, colors[color_index]);
  }

  HAL_Delay(1000);
  if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  BSP_LCD_DisplayOff(0);

  BSP_LCD_InitEx(0, LCD_ORIENTATION_LANDSCAPE, LCD_PIXEL_FORMAT_ARGB4444, LCD_DEFAULT_WIDTH, LCD_DEFAULT_HEIGHT);
  UTIL_LCD_SetFuncDriver(&LCD_Driver);
  BSP_LCD_DisplayOn(0);

  UTIL_LCD_FillRect(0, 40, 800, 440, UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_SetBackColor(UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_SetTextColor(UTIL_LCD_COLOR_WHITE);
  UTIL_LCD_FillRect(0, 0, 800, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test LCD  ", CENTER_MODE);
  UTIL_LCD_DisplayStringAt(0, 100, (uint8_t *)"LCD ARGB4444", CENTER_MODE);

  for (color_index = 8; color_index < 24; color_index++)
  {
     x = 40 * ((color_index % 4) + 8) ;
     y = (40 * (color_index / 4 + 4));

    UTIL_LCD_FillRect(x, y, 40, 40, colors[color_index]);
  }

  HAL_Delay(1000);
  UTIL_LCD_Clear(UTIL_LCD_COLOR_WHITE);

  if (BSP_LCD_DeInit(0) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  if(BSP_LCD_Init(0, LCD_ORIENTATION_LANDSCAPE) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  UTIL_LCD_SetFuncDriver(&LCD_Driver);

  UTIL_LCD_FillRect(0, 0, 800, 40, UTIL_LCD_COLOR_ST_BLUE_DARK);
  UTIL_LCD_DisplayStringAt(0, 15, (uint8_t *)" Test LCD  ", CENTER_MODE);
}


/**
  * @}
  */

/**
  * @}
  */

